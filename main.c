#include "draw.h"
#include "geom/defs.h"
#include "geom/state.h"
#include "mode/defs.h"
#include "mode/util.h"
#include "save/dialog.h"
#include "state.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void zoom(ViewInfo *view_info, Pos2D fp, double mul) {
  *view_info = (ViewInfo){
      .scale = view_info->scale * mul,
      .center =
          {
              .x = fp.x + (view_info->center.x - fp.x) / mul,
              .y = fp.y + (view_info->center.y - fp.y) / mul,
          },
  };
}

SDL_AppResult on_key_down(AppState *as, SDL_KeyboardEvent *event) {
  SDL_Keycode key_code = event->key;
  // we use SDLK_UNKNOWN as the "No keybind" marker for a mode,
  // so even if we somehow got this keycode, we shouldn't actually do anything
  if (key_code == SDLK_UNKNOWN)
    return SDL_APP_CONTINUE;

  bool shift_held = (event->mod & SDL_KMOD_SHIFT) != 0;
  bool ctrl_held = (event->mod & SDL_KMOD_CTRL) != 0;

  if (!ctrl_held && !shift_held) {
    for (int i = 0; i < as->es.num_cats; i++) {
      CategoryInfo const *cat_info = as->es.category_states[i].cat_info;
      if (cat_info->keycode == key_code) {
        as->es.sel_cat_ind = i;
        select_mode_from_inds(&as->es);
        return SDL_APP_CONTINUE;
      }
      for (int j = 0; j < cat_info->num_modes; j++) {
        ModeInfo const *mode_info = &cat_info->modes[j];
        if (mode_info->keycode == key_code) {
          as->es.sel_cat_ind = i;
          as->es.category_states[i].sel_mode_ind = j;
          select_mode_from_inds(&as->es);
          return SDL_APP_CONTINUE;
        }
      }
    }
  }
  switch (key_code) {
  case SDLK_W:
    if (ctrl_held) {
      return SDL_APP_SUCCESS;
    }
    break;
  case SDLK_ESCAPE:
    reset_mode_data(&as->es);
    break;
  case SDLK_TAB:
    if (shift_held)
      decr_curr_cat_mode_ind(&as->es);
    else
      incr_curr_cat_mode_ind(&as->es);
    select_mode_from_inds(&as->es);
    break;
  case SDLK_S:
    if (ctrl_held) {
      if (shift_held) {
        show_save_as_prompt(as);
      } else {
        save_or_save_as(as);
      }
    }
    break;
  case SDLK_O:
    if (ctrl_held) {
      show_open_prompt(as);
    }
    break;

  case SDLK_N:
    if (ctrl_held) {
      if (show_lose_data_prompt(as))
        make_new_canvas(as);
    }
    break;
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult on_mouse_move(AppState *as, SDL_MouseMotionEvent *motion) {
  Pos2D s_mouse_pos = (Pos2D){.x = motion->x, .y = motion->y};
  Pos2D w_mouse_pos =
      pos_screen_to_world(as->renderer, &as->view_info, s_mouse_pos);

  if (as->es.mode_info->on_mouse_move != NULL) {
    if (!as->es.mode_info->on_mouse_move(as, &w_mouse_pos))
      return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult on_mouse_button_down(AppState *as, SDL_MouseButtonEvent *event) {
  if (event->button != 1)
    return SDL_APP_CONTINUE;

  Pos2D s_mouse_pos = (Pos2D){.x = event->x, .y = event->y};
  Pos2D w_mouse_pos =
      pos_screen_to_world(as->renderer, &as->view_info, s_mouse_pos);

  if (as->es.mode_info->on_mouse_down != NULL) {
    if (!as->es.mode_info->on_mouse_down(as, &w_mouse_pos))
      return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult on_mouse_button_up(AppState *as, SDL_MouseButtonEvent *event) {
  if (event->button != 1)
    return SDL_APP_CONTINUE;

  if (as->es.mode_info->on_mouse_up != NULL) {
    if (!as->es.mode_info->on_mouse_up(as))
      return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult on_mouse_wheel(AppState *as, SDL_MouseWheelEvent *event) {
  double mul = pow(1.1, event->y);
  Pos2D s_mouse_pos = (Pos2D){.x = event->mouse_x, .y = event->mouse_y};
  Pos2D w_mouse_pos =
      pos_screen_to_world(as->renderer, &as->view_info, s_mouse_pos);
  zoom(&as->view_info, w_mouse_pos, mul);
  return SDL_APP_CONTINUE;
}

SDL_AppResult on_event(AppState *as, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return show_lose_data_prompt(as) ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
  case SDL_EVENT_KEY_DOWN:
    return on_key_down(as, &event->key);
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    return on_mouse_button_down(as, &event->button);
  case SDL_EVENT_MOUSE_BUTTON_UP:
    return on_mouse_button_up(as, &event->button);
  case SDL_EVENT_MOUSE_WHEEL:
    return on_mouse_wheel(as, &event->wheel);
  case SDL_EVENT_MOUSE_MOTION:
    return on_mouse_move(as, &event->motion);
  }
  return SDL_APP_CONTINUE;
}

void render_mode_info(AppState *as) {
  for (int i = 0; i < as->es.num_cats; i++) {
    CategoryState *cat_state = &as->es.category_states[i];
    bool is_cat_sel = i == as->es.sel_cat_ind;
    SDL_FRect rect = {
        .x = 10 + 150 * i,
        .y = 10,
        .w = 150,
        .h = 30 * (cat_state->cat_info->num_modes + 1),
    };
    if (is_cat_sel) {
      SDL_SetRenderDrawColor(as->renderer, YELLOW.r, YELLOW.g, YELLOW.b,
                             YELLOW.a);
      SDL_RenderFillRect(as->renderer, &rect);
    }
    draw_text_to(as, cat_state->cat_info->name, MAGENTA, 10 + 150 * i, 10);
    for (int j = 0; j < cat_state->cat_info->num_modes; j++) {
      ModeInfo const *mode_info = &cat_state->cat_info->modes[j];
      bool is_mode_sel_in_cat = j == cat_state->sel_mode_ind;
      SDL_Color color = is_mode_sel_in_cat ? CYAN : BLUE;
      draw_text_to(as, mode_info->name, color, 10 + 150 * i, 10 + 30 * (j + 1));
    }
  }
}

void render_save_info(AppState const *as) {
  char const *save_name = "Unsaved";
  if (as->save_path != NULL) {
    // Note: this might not work on windows
    char const *pos = strrchr(as->save_path, '/');
    save_name = pos == NULL ? as->save_path : pos + 1;
  }
  draw_text_to(as, save_name, WHITE, 700, 10);
}

SDL_AppResult on_render(AppState *as) {
  clear_screen(as, BLACK);
  render_mode_info(as);
  render_save_info(as);

  {
    // Display origin and x y axes as short segments
    PointDef p1 = make_point_literal((Pos2D){0.0, 0.0});
    PointDef p2 = make_point_literal((Pos2D){10.0, 0.0});
    PointDef p3 = make_point_literal((Pos2D){0.0, 10.0});
    LineDef l1 = make_line_point_to_point(L_EXT_SEGMENT, &p1, &p2);
    LineDef l2 = make_line_point_to_point(L_EXT_SEGMENT, &p1, &p3);
    draw_line(as, &l1, YELLOW);
    draw_line(as, &l2, YELLOW);
  }

  Pos2D w_mouse_pos;
  {
    // TODO: actually use proper render coodinates everywhere
    // TODO: maybe transform renderer instead of using ViewInfo
    float sc_mx, sc_my;
    SDL_GetMouseState(&sc_mx, &sc_my);
    Pos2D sc_mouse_pos = (Pos2D){.x = sc_mx, .y = sc_my};
    w_mouse_pos =
        pos_screen_to_world(as->renderer, &as->view_info, sc_mouse_pos);
  }

  if (as->es.mode_info->on_render != NULL) {
    if (!as->es.mode_info->on_render(as, &w_mouse_pos))
      return SDL_APP_FAILURE;
  }

  for (GenericElemList *curr = as->gs.pd_list; curr != NULL;
       curr = curr->next) {
    draw_point(as, curr->pd, WHITE);
  }

  for (GenericElemList *curr = as->gs.ld_list; curr != NULL;
       curr = curr->next) {
    draw_line(as, curr->ld, WHITE);
  }

  for (GenericElemList *curr = as->gs.cd_list; curr != NULL;
       curr = curr->next) {
    draw_circle(as, curr->cd, WHITE);
  }

  SDL_RenderPresent(as->renderer);
  return SDL_APP_CONTINUE;
}

SDL_AppResult init_app(AppState *as) {
  if (!SDL_SetAppMetadata("NHF Geometry", "0.0.0",
                          "net.tomasajt.NHFGeometry")) {
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  const int window_flags = SDL_WINDOW_RESIZABLE;

  if (!SDL_CreateWindowAndRenderer("NHF Geometry", 1000, 600, window_flags,
                                   &as->window, &as->renderer)) {
    return SDL_APP_FAILURE;
  }

  if (!TTF_Init()) {
    return SDL_APP_FAILURE;
  }

  as->font = TTF_OpenFont("./fonts/liberation/LiberationSerif-Regular.ttf", 24);
  if (as->font == NULL) {
    printf("Font not found!\n");
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

void deinit_appstate(AppState *as) {
  if (as->renderer != NULL) {
    SDL_DestroyRenderer(as->renderer);
    as->renderer = NULL;
  }
  if (as->window != NULL) {
    SDL_DestroyWindow(as->window);
    as->window = NULL;
  }
  if (as->font != NULL) {
    TTF_CloseFont(as->font);
    as->font = NULL;
  }
  if (as->save_path != NULL) {
    free(as->save_path);
    as->save_path = NULL;
  }

  clear_geometry_state(&as->gs);
}

void make_default_editor_state(EditorState *es) {
  assert(editor_info.num_cats <= 4);

  es->num_cats = editor_info.num_cats;
  for (int i = 0; i < editor_info.num_cats; i++) {
    es->category_states[i] = (CategoryState){
        .cat_info = &editor_info.cats[i],
        .sel_mode_ind = 0,
    };
  }
  es->sel_cat_ind = 0;
  select_mode_from_inds(es);
}

void make_default_appstate(AppState *as) {
  *as = (AppState){
      .window = NULL,
      .renderer = NULL,
      .view_info = {.center = {.x = 0, .y = 0}, .scale = 1.0},
      .gs = {.pd_list = NULL, .ld_list = NULL, .cd_list = NULL},
      .save_path = NULL,
  };
  make_default_editor_state(&as->es);
}

int main(int argc, char const **argv) {
  // TODO: maybe put appstate onto heap?
  AppState appstate;
  make_default_appstate(&appstate);

  SDL_AppResult rc = init_app(&appstate);

  if (argc >= 2) {
    if (argc >= 3) {
      printf("Multiple arguments were provided, only using the first!\n");
    }
    do_load_from_file(&appstate, argv[1], false);
  }

  while (rc == SDL_APP_CONTINUE) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    rc = on_event(&appstate, &event);

    if (rc != SDL_APP_CONTINUE)
      break;

    rc = on_render(&appstate);
  }

  deinit_appstate(&appstate);

  SDL_Quit();

  return rc == SDL_APP_FAILURE ? 1 : 0;
}
