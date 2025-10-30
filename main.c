#include "draw.h"
#include "geom/defs.h"
#include "geom/state.h"
#include "hover.h"
#include "savedata.h"
#include "state.h"

#include "mode/circle.h"
#include "mode/delete.h"
#include "mode/line.h"
#include "mode/midpoint.h"
#include "mode/move.h"
#include "mode/point.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <SDL3_ttf/SDL_ttf.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SDL_WINDOW_WIDTH 1000
#define SDL_WINDOW_HEIGHT 600

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
  case SDL_SCANCODE_ESCAPE:
  case SDL_SCANCODE_Q:
    return SDL_APP_SUCCESS;
  case SDL_SCANCODE_0:
  case SDL_SCANCODE_GRAVE:
  case SDL_SCANCODE_M:
    enter_move_mode(&as->es);
    break;
  case SDL_SCANCODE_1:
  case SDL_SCANCODE_D:
    enter_delete_mode(&as->es);
    break;
  case SDL_SCANCODE_2:
  case SDL_SCANCODE_P:
    if (as->es.mode != EM_POINT)
      enter_point_mode(&as->es);
    else
      enter_midpoint_mode(&as->es);
    break;
  case SDL_SCANCODE_3:
  case SDL_SCANCODE_L:
    enter_line_mode(&as->es);
    break;
  case SDL_SCANCODE_4:
  case SDL_SCANCODE_C:
    enter_circle_mode(&as->es);
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
  case SDL_SCANCODE_S: {
    do_save(&as->gs);
    break;
  }
  case SDL_SCANCODE_R: {
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

SDL_AppResult on_mouse_motion(AppState *as, SDL_MouseMotionEvent *motion) {
  Pos2D s_mouse_pos = (Pos2D){.x = motion->x, .y = motion->y};
  Pos2D w_mouse_pos =
      pos_screen_to_world(as->renderer, &as->view_info, s_mouse_pos);

  switch (as->es.mode) {
  case EM_MOVE:
    move__on_mouse_motion(as, w_mouse_pos);
    break;
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult on_mouse_button_down(AppState *as, SDL_MouseButtonEvent *event) {
  if (event->button != 1)
    return SDL_APP_CONTINUE;

  Pos2D s_mouse_pos = (Pos2D){.x = event->x, .y = event->y};
  Pos2D w_mouse_pos =
      pos_screen_to_world(as->renderer, &as->view_info, s_mouse_pos);

  switch (as->es.mode) {
  case EM_MOVE:
    move__on_click(as, w_mouse_pos);
    return SDL_APP_CONTINUE;
  case EM_DELETE:
    delete__on_click(as, w_mouse_pos);
    return SDL_APP_CONTINUE;
  case EM_POINT:
    return point__on_click(as, w_mouse_pos);
  case EM_MIDPOINT:
    return midpoint__on_click(as, w_mouse_pos);
  case EM_LINE:
    return line__on_click(as, w_mouse_pos);
  case EM_CIRCLE:
    return circle__on_click(as, w_mouse_pos);
  default:
    return SDL_APP_CONTINUE;
  }
}

SDL_AppResult on_mouse_button_up(AppState *as, SDL_MouseButtonEvent *event) {
  if (event->button != 1)
    return SDL_APP_CONTINUE;

  switch (as->es.mode) {
  case EM_MOVE:
    move__on_click_release(as);
    return SDL_APP_CONTINUE;
  default:
    return SDL_APP_CONTINUE;
  }
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
    return on_mouse_motion(as, &event->motion);
  }
  return SDL_APP_CONTINUE;
}

void render_mode_info(AppState *as) {
  switch (as->es.mode) {
  case EM_MOVE:
    draw_text_to(as, "Move", WHITE, 10, 10);
    break;
  case EM_DELETE:
    draw_text_to(as, "Delete", WHITE, 10, 10);
    break;
  case EM_POINT:
    draw_text_to(as, "Point", WHITE, 10, 10);
    break;
  case EM_MIDPOINT:
    draw_text_to(as, "Midpoint", WHITE, 10, 10);
    break;
  case EM_LINE:
    draw_text_to(as, "Line", WHITE, 10, 10);
    break;
  case EM_CIRCLE:
    draw_text_to(as, "Circle", WHITE, 10, 10);
    break;
  case EM_CIRCLE_BY_LEN:
    draw_text_to(as, "Circle by length", WHITE, 10, 10);
    break;
  default:
    draw_text_to(as, "Unknown", WHITE, 10, 10);
    break;
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

  PointDef *hovered_point = get_hovered_point(as, w_mouse_pos);
  LineDef *hovered_line = get_hovered_line(as, w_mouse_pos);
  CircleDef *hovered_circle = get_hovered_circle(as, w_mouse_pos);

  switch (as->es.mode) {
  case EM_MOVE:
    break;
  case EM_DELETE:
    break;
  case EM_POINT:
    point__on_render(as, w_mouse_pos);
    break;
  case EM_MIDPOINT:
    midpoint__on_render(as, w_mouse_pos);
    break;
  case EM_LINE:
    line__on_render(as, w_mouse_pos);
    break;
  case EM_CIRCLE:
    circle__on_render(as, w_mouse_pos);
    break;
  case EM_CIRCLE_BY_LEN:
    break;
  }

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
