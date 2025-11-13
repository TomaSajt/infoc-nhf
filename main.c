#include "draw.h"
#include "geom/defs.h"
#include "geom/state.h"
#include "mode/defs.h"
#include "savedata.h"
#include "state.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

const SDL_DialogFileFilter file_filters[] = {
    {"Geom savefiles", "geom"},
    {"All files", "*"},
};

ModeInfo const *calc_mode_from_inds(EditorState const *es) {
  CategoryState const *cs = &es->category_states[es->sel_cat_ind];
  return &cs->cat_info->modes[cs->sel_mode_ind];
}

void incr_curr_cat_mode_ind(EditorState *es) {
  CategoryState *cs = &es->category_states[es->sel_cat_ind];
  cs->sel_mode_ind++;
  int n = cs->cat_info->num_modes;
  if (cs->sel_mode_ind >= n)
    cs->sel_mode_ind -= n;
}

void decr_curr_cat_mode_ind(EditorState *es) {
  CategoryState *cs = &es->category_states[es->sel_cat_ind];
  cs->sel_mode_ind--;
  int n = cs->cat_info->num_modes;
  if (cs->sel_mode_ind < 0)
    cs->sel_mode_ind += n;
}

void reset_mode_data(EditorState *es) {
  if (es->mode_info->init_data != NULL)
    es->mode_info->init_data(&es->data);
}

void select_mode_from_inds(EditorState *es) {
  es->mode_info = calc_mode_from_inds(es);
  reset_mode_data(es);
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

void open__on_file_selected(void *userdata, char const *const *filelist,
                            int filter) {
  AppState *as = userdata;
  (void)filter; // unused parameter

  if (filelist == NULL) {
    SDL_Log("An error occurred: %s\n", SDL_GetError());
    return;
  }
  char const *file_path = filelist[0];
  if (file_path == NULL) {
    SDL_Log("No files selected!\n");
    return;
  }

  if (filelist[1] != NULL) {
    printf("Multiple files were selected, only using the first!\n");
  }

  SDL_Log("Loading file %s\n", file_path);
  FILE *handle = fopen(file_path, "r");
  if (handle != NULL) {
    GeometryState new_gs;
    bool res = load_from_file(handle, &new_gs);
    if (res) {
      clear_geometry_state(&as->gs);
      as->gs = new_gs;
      reset_mode_data(&as->es);
      printf("Load successful\n");
    } else {
      printf("Load failed\n");
    }
    fclose(handle);
  } else {
    printf("Failed to open file\n");
  }
}

void show_open_prompt(AppState *as) {
  char *cwd = SDL_GetCurrentDirectory();
  SDL_ShowOpenFileDialog(open__on_file_selected, as, as->window, file_filters,
                         sizeof(file_filters) / sizeof(SDL_DialogFileFilter),
                         cwd, false);
  SDL_free(cwd);
}

void save__on_file_selected(void *userdata, char const *const *filelist,
                            int filter) {
  AppState *as = userdata;
  (void)filter; // unused parameter

  if (filelist == NULL) {
    SDL_Log("An error occurred: %s\n", SDL_GetError());
    return;
  }
  char const *file_path = filelist[0];
  if (file_path == NULL) {
    SDL_Log("No files selected!\n");
    return;
  }

  if (filelist[1] != NULL) {
    printf("Multiple files were selected, only using the first!\n");
  }

  SDL_Log("Saving...\n");
  FILE *handle = fopen(file_path, "w");
  if (handle != NULL) {
    save_to_file(handle, &as->gs);
    fclose(handle);
  } else {
    printf("Failed to open file\n");
  }
}

void show_save_as_prompt(AppState *as) {
  char *cwd = SDL_GetCurrentDirectory();
  SDL_ShowSaveFileDialog(save__on_file_selected, as, as->window, file_filters,
                         sizeof(file_filters) / sizeof(SDL_DialogFileFilter),
                         cwd);
  SDL_free(cwd);
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
        ModeInfo const *mode_info = &cat_info->modes[i];
        if (mode_info->keycode == key_code) {
          as->es.sel_cat_ind = i;
          as->es.category_states[i].sel_mode_ind = j;
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
  case SDLK_I: {
    PointDef *pd = alloc_and_reg_point(
        &as->gs, make_point_intsec_line_line(as->gs.line_defs[as->gs.l_n - 2],
                                             as->gs.line_defs[as->gs.l_n - 1]));
    if (pd == NULL)
      return SDL_APP_FAILURE;
    break;
  }
  case SDLK_K: {
    PointDef *pd = alloc_and_reg_point(
        &as->gs, make_point_intsec_circle_circle(
                     as->gs.circle_defs[as->gs.c_n - 2],
                     as->gs.circle_defs[as->gs.c_n - 1], ICC_RIGHT));
    if (pd == NULL)
      return SDL_APP_FAILURE;
    break;
  }
  case SDLK_S:
    if (ctrl_held) {
      show_save_as_prompt(as);
    }
    break;

  case SDLK_O:
    if (ctrl_held) {
      show_open_prompt(as);
    }
    break;

  case SDLK_N:
    if (ctrl_held) {
      SDL_Log("Creating new canvas...\n");
      clear_geometry_state(&as->gs);
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
    return SDL_APP_SUCCESS;
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
    draw_text_to(as, cat_state->cat_info->name, BLUE, 10 + 150 * i, 10);
    for (int j = 0; j < cat_state->cat_info->num_modes; j++) {
      ModeInfo const *mode_info = &cat_state->cat_info->modes[j];
      bool is_mode_sel_in_cat = j == cat_state->sel_mode_ind;
      SDL_Color color = is_mode_sel_in_cat ? CYAN : BLUE;
      draw_text_to(as, mode_info->name, color, 10 + 150 * i, 10 + 30 * (j + 1));
    }
  }
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

  if (as->es.mode_info->on_render != NULL) {
    if (!as->es.mode_info->on_render(as, &w_mouse_pos))
      return SDL_APP_FAILURE;
  }

  for (int i = 0; i < as->gs.p_n; i++) {
    PointDef *pd = as->gs.point_defs[i];
    draw_point(as, pd, WHITE);
  }

  for (int i = 0; i < as->gs.l_n; i++) {
    LineDef *ld = as->gs.line_defs[i];
    draw_line(as, ld, WHITE);
  }

  for (int i = 0; i < as->gs.c_n; i++) {
    CircleDef *cd = as->gs.circle_defs[i];
    draw_circle(as, cd, WHITE);
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
  };
  make_default_editor_state(&as->es);
}

int main(void) {
  // TODO: maybe put appstate onto heap?
  AppState appstate;
  make_default_appstate(&appstate);

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
