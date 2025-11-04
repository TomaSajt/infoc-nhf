#include "SDL3/SDL_scancode.h"
#include "draw.h"
#include "hover.h"
#include "mode/defs.h"
#include "savedata.h"
#include "state.h"

#include "geom/defs.h"
#include "geom/state.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <SDL3_ttf/SDL_ttf.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SDL_WINDOW_WIDTH 1000
#define SDL_WINDOW_HEIGHT 600

ModeInfo const *calc_mode_from_inds(NewEditorState const *es) {
  CategoryState const *cs = &es->category_states[es->sel_cat_ind];
  return &cs->cat_info->modes[cs->sel_mode_ind];
}

void incr_curr_cat_mode_ind(NewEditorState *es) {
  CategoryState *cs = &es->category_states[es->sel_cat_ind];
  cs->sel_mode_ind++;
  int n = cs->cat_info->num_modes;
  if (cs->sel_mode_ind >= n)
    cs->sel_mode_ind -= n;
}

void decr_curr_cat_mode_ind(NewEditorState *es) {
  CategoryState *cs = &es->category_states[es->sel_cat_ind];
  cs->sel_mode_ind--;
  int n = cs->cat_info->num_modes;
  if (cs->sel_mode_ind < 0)
    cs->sel_mode_ind += n;
}

void select_mode_from_inds(NewEditorState *es) {
  es->mode_info = calc_mode_from_inds(es);
  if (es->mode_info->init_data != NULL)
    es->mode_info->init_data(&es->data);
}

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

void do_save(GeometryState const *gs) {
  SDL_Log("Saving...\n");
  FILE *handle = fopen("save.dat", "w");
  if (handle != NULL) {
    save_to_file(handle, gs);
    fclose(handle);
  } else {
    printf("Failed to open file\n");
  }
}

void do_load(GeometryState *gs) {
  SDL_Log("Loading...\n");
  FILE *handle = fopen("save.dat", "r");
  if (handle != NULL) {
    GeometryState new_gs;
    bool res = load_from_file(handle, &new_gs);
    if (res) {
      clear_geometry_state(gs);
      *gs = new_gs;
      printf("Load successful\n");
    } else {
      printf("Load failed\n");
    }
    fclose(handle);
  } else {
    printf("Failed to open file\n");
  }
}

SDL_AppResult on_key_down(AppState *as, SDL_Scancode key_code) {
  switch (key_code) {
  case SDL_SCANCODE_W:
    return SDL_APP_SUCCESS;
  case SDL_SCANCODE_ESCAPE:
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_TAB:
    incr_curr_cat_mode_ind(&as->nes);
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_1:
    as->nes.sel_cat_ind = 0;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_2:
    as->nes.sel_cat_ind = 1;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_3:
    as->nes.sel_cat_ind = 2;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_4:
    as->nes.sel_cat_ind = 3;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_M:
    as->nes.sel_cat_ind = 0;
    as->nes.category_states[0].sel_mode_ind = 0;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_D:
    as->nes.sel_cat_ind = 0;
    as->nes.category_states[0].sel_mode_ind = 1;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_P:
    as->nes.sel_cat_ind = 1;
    as->nes.category_states[0].sel_mode_ind = 0;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_S:
    as->nes.sel_cat_ind = 2;
    as->nes.category_states[0].sel_mode_ind = 0;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_L:
    as->nes.sel_cat_ind = 2;
    as->nes.category_states[0].sel_mode_ind = 1;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_R:
    as->nes.sel_cat_ind = 2;
    as->nes.category_states[0].sel_mode_ind = 2;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_C:
    as->nes.sel_cat_ind = 3;
    as->nes.category_states[0].sel_mode_ind = 0;
    select_mode_from_inds(&as->nes);
    break;
  case SDL_SCANCODE_I: {
    PointDef *pd = alloc_and_reg_point(
        &as->gs, make_point_intsec_line_line(as->gs.line_defs[as->gs.l_n - 2],
                                             as->gs.line_defs[as->gs.l_n - 1]));
    if (pd == NULL)
      return SDL_APP_FAILURE;
    break;
  }
  case SDL_SCANCODE_K: {
    PointDef *pd = alloc_and_reg_point(
        &as->gs, make_point_intsec_circle_circle(
                     as->gs.circle_defs[as->gs.c_n - 2],
                     as->gs.circle_defs[as->gs.c_n - 1], ICC_RIGHT));
    if (pd == NULL)
      return SDL_APP_FAILURE;
    break;
  }
  case SDL_SCANCODE_8: {
    do_save(&as->gs);
    break;
  }
  case SDL_SCANCODE_9: {
    do_load(&as->gs);
    break;
  }
  case SDL_SCANCODE_N: {
    SDL_Log("Creating new canvas...\n");
    clear_geometry_state(&as->gs);
    break;
  }
  default: {
    break;
  }
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult on_mouse_move(AppState *as, SDL_MouseMotionEvent *motion) {
  Pos2D s_mouse_pos = (Pos2D){.x = motion->x, .y = motion->y};
  Pos2D w_mouse_pos =
      pos_screen_to_world(as->renderer, &as->view_info, s_mouse_pos);

  bool (*mode__on_mouse_move)(AppState *as, Pos2D const *w_mouse_pos) =
      as->nes.mode_info->on_mouse_move;

  if (mode__on_mouse_move != NULL)
    if (!mode__on_mouse_move(as, &w_mouse_pos))
      return SDL_APP_FAILURE;

  return SDL_APP_CONTINUE;
}

SDL_AppResult on_mouse_button_down(AppState *as, SDL_MouseButtonEvent *event) {
  if (event->button != 1)
    return SDL_APP_CONTINUE;

  Pos2D s_mouse_pos = (Pos2D){.x = event->x, .y = event->y};
  Pos2D w_mouse_pos =
      pos_screen_to_world(as->renderer, &as->view_info, s_mouse_pos);

  bool (*mode__on_mouse_down)(AppState *as, Pos2D const *w_mouse_pos) =
      as->nes.mode_info->on_mouse_down;

  if (mode__on_mouse_down != NULL)
    if (!mode__on_mouse_down(as, &w_mouse_pos))
      return SDL_APP_FAILURE;

  return SDL_APP_CONTINUE;
}

SDL_AppResult on_mouse_button_up(AppState *as, SDL_MouseButtonEvent *event) {
  if (event->button != 1)
    return SDL_APP_CONTINUE;

  bool (*mode__on_mouse_up)(AppState *as) = as->nes.mode_info->on_mouse_up;
  if (mode__on_mouse_up != NULL)
    if (!mode__on_mouse_up(as))
      return SDL_APP_FAILURE;

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
    return SDL_APP_SUCCESS;
  case SDL_EVENT_KEY_DOWN:
    return on_key_down(as, event->key.scancode);
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
  draw_text_to(as, as->nes.mode_info->name, WHITE, 10, 10);
}

void clear_screen(AppState *as, SDL_Color color) {
  SDL_SetRenderDrawColor(as->renderer, color.r, color.g, color.b, color.a);
  SDL_RenderClear(as->renderer);
}

SDL_AppResult on_render(AppState *as) {
  printf("n=(%d, %d, %d)\n", as->gs.p_n, as->gs.l_n, as->gs.c_n);

  clear_screen(as, BLACK);

  render_mode_info(as);

  {
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
    float wmx, wmy;
    SDL_GetMouseState(&wmx, &wmy);
    float mx, my;
    SDL_RenderCoordinatesFromWindow(as->renderer, wmx, wmy, &mx, &my);
    Pos2D s_mouse_pos = (Pos2D){.x = mx, .y = my};
    w_mouse_pos =
        pos_screen_to_world(as->renderer, &as->view_info, s_mouse_pos);
  }

  bool (*mode__on_render)(AppState *as, Pos2D const *w_mouse_pos) =
      as->nes.mode_info->on_render;

  if (mode__on_render != NULL)
    if (!mode__on_render(as, &w_mouse_pos))
      return SDL_APP_FAILURE;

  PointDef *hovered_point = get_hovered_point(as, &w_mouse_pos);
  LineDef *hovered_line = get_hovered_line(as, &w_mouse_pos);
  CircleDef *hovered_circle = get_hovered_circle(as, &w_mouse_pos);

  for (int i = 0; i < as->gs.p_n; i++) {
    PointDef *pd = as->gs.point_defs[i];
    if (pd != hovered_point) {
      draw_point(as, pd, WHITE);
    }
  }
  if (hovered_point != NULL) {
    draw_point(as, hovered_point, RED);
  }

  for (int i = 0; i < as->gs.l_n; i++) {
    LineDef *ld = as->gs.line_defs[i];
    if (ld != hovered_line) {
      draw_line(as, ld, WHITE);
    }
  }
  if (hovered_line != NULL) {
    draw_line(as, hovered_line, RED);
  }

  for (int i = 0; i < as->gs.c_n; i++) {
    CircleDef *cd = as->gs.circle_defs[i];
    if (cd != hovered_circle) {
      draw_circle(as, cd, WHITE);
    }
  }
  if (hovered_circle != NULL) {
    draw_circle(as, hovered_circle, RED);
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

  if (!SDL_CreateWindowAndRenderer("Geometry", SDL_WINDOW_WIDTH,
                                   SDL_WINDOW_HEIGHT, window_flags, &as->window,
                                   &as->renderer)) {
    return SDL_APP_FAILURE;
  }

  if (!TTF_Init()) {
    return SDL_APP_FAILURE;
  }
  as->font = TTF_OpenFont("./fonts/liberation/LiberationSerif-Regular.ttf", 32);
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

  clear_geometry_state(&as->gs);
}

void make_default_editor_state(NewEditorState *es) {
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

int main(void) {
  // TODO: maybe put appstate onto heap?
  AppState appstate = {
      .window = NULL,
      .renderer = NULL,
      .view_info =
          {
              .center = {.x = 0, .y = 0},
              .scale = 1.0,
          },
      .gs =
          {
              .point_defs = {0},
              .p_n = 0,
              .line_defs = {0},
              .l_n = 0,
              .circle_defs = {0},
              .c_n = 0,
          },
      .es =
          {
              .mode = EM_MOVE,
              .elem_type = FE_NONE,
          },
  };
  make_default_editor_state(&appstate.nes);

  SDL_AppResult rc = init_app(&appstate);

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
