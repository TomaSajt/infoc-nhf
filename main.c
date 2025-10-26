#include "debugmalloc.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <SDL3_ttf/SDL_ttf.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "draw.h"
#include "geom.h"
#include "geom_defs.h"
#include "geom_state.h"
#include "savedata.h"
#include "state.h"

#define SDL_WINDOW_WIDTH 1000
#define SDL_WINDOW_HEIGHT 600

const SDL_Color BLACK = /*  */ {.r = 0x00, .g = 0x00, .b = 0x00, .a = 0xff};
const SDL_Color RED = /*    */ {.r = 0xff, .g = 0x00, .b = 0x00, .a = 0xff};
const SDL_Color GREEN = /*  */ {.r = 0x00, .g = 0xff, .b = 0x00, .a = 0xff};
const SDL_Color YELLOW = /* */ {.r = 0xff, .g = 0xff, .b = 0x00, .a = 0xff};
const SDL_Color BLUE = /*   */ {.r = 0x00, .g = 0x00, .b = 0xff, .a = 0xff};
const SDL_Color MAGENTA = /**/ {.r = 0xff, .g = 0x00, .b = 0xff, .a = 0xff};
const SDL_Color CYAN = /*   */ {.r = 0x00, .g = 0xff, .b = 0xff, .a = 0xff};
const SDL_Color WHITE = /*  */ {.r = 0xff, .g = 0xff, .b = 0xff, .a = 0xff};

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

void do_save(GeometryState *gs) {
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

bool is_point_movable(PointDef *pd) {
  return pd->type == PD_LITERAL || pd->type == PD_GLIDER_ON_LINE ||
         pd->type == PD_GLIDER_ON_CIRCLE;
}

PointDef *get_hovered_point(AppState *as, Pos2D w_mouse_pos) {
  PointDef *best = NULL;
  double best_dist = 0;
  for (int i = 0; i < as->gs.p_n; i++) {
    PointDef *pd = as->gs.point_defs[i];
    eval_point(pd);
    if (pd->val.invalid)
      continue;

    double d = dist_from_pos(&w_mouse_pos, &pd->val.pos);
    if (d * as->view_info.scale > POINT_HITBOX_RADIUS)
      continue;

    if (best == NULL || best_dist > d) {
      best_dist = d;
      best = pd;
    }
  }

  return best;
}

LineDef *get_hovered_line(AppState *as, Pos2D w_mouse_pos) {
  LineDef *best = NULL;
  double best_dist = 0;
  for (int i = 0; i < as->gs.l_n; i++) {
    LineDef *ld = as->gs.line_defs[i];
    eval_line(ld);
    if (ld->val.invalid)
      continue;

    double d = dist_from_line(&w_mouse_pos, &ld->val.start, &ld->val.end,
                              ld->ext_mode);
    if (d * as->view_info.scale > LINE_HITBOX_RADIUS)
      continue;

    if (best == NULL || best_dist > d) {
      best_dist = d;
      best = ld;
    }
  }

  return best;
}

CircleDef *get_hovered_circle(AppState *as, Pos2D w_mouse_pos) {
  CircleDef *best = NULL;
  double best_dist = 0;
  for (int i = 0; i < as->gs.c_n; i++) {
    CircleDef *cd = as->gs.circle_defs[i];
    eval_circle(cd);
    if (cd->val.invalid)
      continue;

    double d = dist_from_circle(&w_mouse_pos, &cd->val.center, cd->val.radius);
    if (d * as->view_info.scale > LINE_HITBOX_RADIUS)
      continue;

    if (best == NULL || best_dist > d) {
      best_dist = d;
      best = cd;
    }
  }

  return best;
}

void try_move_point_to_pos(PointDef *pd, Pos2D pos) {
  switch (pd->type) {
  case PD_LITERAL: {
    pd->literal.pos = pos;
    break;
  }
  case PD_GLIDER_ON_LINE: {
    LineExtMode ext_mode = pd->glider_on_line.l->ext_mode;
    Pos2D *s = &pd->glider_on_line.l->val.start;
    Pos2D *e = &pd->glider_on_line.l->val.end;
    double prog = line_closest_prog_from_pos(&pos, s, e, ext_mode);
    pd->glider_on_line.prog = prog;
    break;
  }
  case PD_GLIDER_ON_CIRCLE: {
    Pos2D *c = &pd->glider_on_circle.c->val.center;
    double prog = circle_closest_prog_from_pos(&pos, c);
    pd->glider_on_circle.prog = prog;
    break;
  }
  default:
    printf("Can't move non-literal, non-glider points!\n");
    break;
  }
}

void move__on_mouse_down(AppState *as, Pos2D w_mouse_pos) {
  if (as->es.elem_type != FE_NONE)
    return;

  PointDef *hovered_point = get_hovered_point(as, w_mouse_pos);
  if (hovered_point == NULL)
    return;

  if (is_point_movable(hovered_point)) {
    as->es.elem_type = FE_POINT;
    as->es.p = hovered_point;
  }
}

void move__on_mouse_motion(AppState *as, Pos2D w_mouse_pos) {
  if (as->es.elem_type != FE_POINT)
    return;
  try_move_point_to_pos(as->es.p, w_mouse_pos);
  mark_everyting_dirty(&as->gs);
}

void move__on_mouse_up(AppState *as) { as->es.elem_type = FE_NONE; }

void delete__on_mouse_down(AppState *as, Pos2D w_mouse_pos) {
  {
    PointDef *hovered_point = get_hovered_point(as, w_mouse_pos);
    if (hovered_point != NULL) {
      delete_point(&as->gs, hovered_point);
      return;
    }
  }
  {
    LineDef *hovered_line = get_hovered_line(as, w_mouse_pos);
    if (hovered_line != NULL) {
      delete_line(&as->gs, hovered_line);
      return;
    }
  }
  {
    CircleDef *hovered_circle = get_hovered_circle(as, w_mouse_pos);
    if (hovered_circle != NULL) {
      delete_circle(&as->gs, hovered_circle);
      return;
    }
  }
}

PointDef point__get_potential_point(AppState *as, Pos2D w_mouse_pos) {
  // TODO: implement get_hovered_lines and get_hovered_circles
  //       for line-line and circle-circle intersections
  LineDef *hovered_line = get_hovered_line(as, w_mouse_pos);
  CircleDef *hovered_circle = get_hovered_circle(as, w_mouse_pos);
  if (hovered_line != NULL) {
    if (hovered_circle != NULL) {
      PointDef pot1 = make_point_intsec_line_circle(
          hovered_line, hovered_circle, ILC_PROG_LOWER);
      PointDef pot2 = make_point_intsec_line_circle(
          hovered_line, hovered_circle, ILC_PROG_HIGHER);
      eval_point(&pot1);
      eval_point(&pot2);
      double d1 = dist_from_pos(&w_mouse_pos, &pot1.val.pos);
      double d2 = dist_from_pos(&w_mouse_pos, &pot2.val.pos);
      return d1 < d2 ? pot1 : pot2;
    } else {
      double prog = line_closest_prog_from_pos(
          &w_mouse_pos, &hovered_line->val.start, &hovered_line->val.end,
          hovered_line->ext_mode);
      return make_point_glider_on_line(hovered_line, prog);
    }
  } else {
    if (hovered_circle != NULL) {
      double prog = circle_closest_prog_from_pos(&w_mouse_pos,
                                                 &hovered_circle->val.center);
      return make_point_glider_on_circle(hovered_circle, prog);
    } else {
      return make_point_literal(w_mouse_pos);
    }
  }
}

SDL_AppResult point__on_mouse_down(AppState *as, Pos2D w_mouse_pos) {
  PointDef *pd = malloc(sizeof(PointDef));
  if (pd == NULL)
    return SDL_APP_FAILURE;

  *pd = point__get_potential_point(as, w_mouse_pos);
  register_point(&as->gs, pd);
  return SDL_APP_CONTINUE;
}

SDL_AppResult on_key_down(AppState *as, SDL_Scancode key_code) {
  switch (key_code) {
  case SDL_SCANCODE_ESCAPE:
  case SDL_SCANCODE_Q:
    return SDL_APP_SUCCESS;
  case SDL_SCANCODE_0:
  case SDL_SCANCODE_GRAVE:
    as->es.mode = EM_MOVE;
    as->es.elem_type = FE_NONE;
    break;
  case SDL_SCANCODE_1:
    as->es.mode = EM_DELETE;
    break;
  case SDL_SCANCODE_2:
    as->es.mode = EM_POINT;
    break;
  case SDL_SCANCODE_M: {
    PointDef *pd = malloc(sizeof(PointDef));
    if (pd == NULL)
      return SDL_APP_FAILURE;

    *pd = make_point_midpoint(as->gs.point_defs[as->gs.p_n - 2],
                              as->gs.point_defs[as->gs.p_n - 1]);
    register_point(&as->gs, pd);
    break;
  }
  case SDL_SCANCODE_G: {
    PointDef *pd = malloc(sizeof(PointDef));
    if (pd == NULL)
      return SDL_APP_FAILURE;

    *pd = make_point_glider_on_line(as->gs.line_defs[as->gs.l_n - 1], 0.5);
    register_point(&as->gs, pd);
    break;
  }
  case SDL_SCANCODE_H: {
    PointDef *pd = malloc(sizeof(PointDef));
    if (pd == NULL)
      return SDL_APP_FAILURE;

    *pd = make_point_glider_on_circle(as->gs.circle_defs[as->gs.c_n - 1], 0.5);
    register_point(&as->gs, pd);
    break;
  }
  case SDL_SCANCODE_L: {
    LineDef *ld = malloc(sizeof(LineDef));
    if (ld == NULL)
      return SDL_APP_FAILURE;

    *ld = make_line_point_to_point(L_EXT_SEGMENT,
                                   as->gs.point_defs[as->gs.p_n - 2],
                                   as->gs.point_defs[as->gs.p_n - 1]);
    register_line(&as->gs, ld);
    break;
  }
  case SDL_SCANCODE_C: {
    CircleDef *cd = malloc(sizeof(CircleDef));
    if (cd == NULL)
      return SDL_APP_FAILURE;

    *cd = make_circle_center_point_outer_point(
        as->gs.point_defs[as->gs.p_n - 2], as->gs.point_defs[as->gs.p_n - 1]);
    register_circle(&as->gs, cd);
    break;
  }
  case SDL_SCANCODE_I: {
    PointDef *pd = malloc(sizeof(PointDef));
    if (pd == NULL)
      return SDL_APP_FAILURE;

    *pd = make_point_intsec_line_line(as->gs.line_defs[as->gs.l_n - 2],
                                      as->gs.line_defs[as->gs.l_n - 1]);
    register_point(&as->gs, pd);
    break;
  }
  case SDL_SCANCODE_J: {
    PointDef *pd = malloc(sizeof(PointDef));
    if (pd == NULL)
      return SDL_APP_FAILURE;

    *pd = make_point_intsec_line_circle(as->gs.line_defs[as->gs.l_n - 1],
                                        as->gs.circle_defs[as->gs.c_n - 1],
                                        ILC_PROG_LOWER);
    register_point(&as->gs, pd);
    break;
  }
  case SDL_SCANCODE_K: {
    PointDef *pd = malloc(sizeof(PointDef));
    if (pd == NULL)
      return SDL_APP_FAILURE;

    *pd = make_point_intsec_circle_circle(as->gs.circle_defs[as->gs.c_n - 2],
                                          as->gs.circle_defs[as->gs.c_n - 1],
                                          ICC_RIGHT);
    register_point(&as->gs, pd);
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
  Pos2D s_mouse_pos = (Pos2D){.x = event->x, .y = event->y};
  Pos2D w_mouse_pos =
      pos_screen_to_world(as->renderer, &as->view_info, s_mouse_pos);

  switch (as->es.mode) {
  case EM_MOVE:
    if (event->button == 1)
      move__on_mouse_down(as, w_mouse_pos);
    break;
  case EM_DELETE:
    if (event->button == 1)
      delete__on_mouse_down(as, w_mouse_pos);
    break;
  case EM_POINT:
    if (event->button == 1)
      return point__on_mouse_down(as, w_mouse_pos);
    break;
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult on_mouse_button_up(AppState *as, SDL_MouseButtonEvent *event) {
  switch (as->es.mode) {
  case EM_MOVE:
    if (event->button == 1)
      move__on_mouse_up(as);
    break;
  default:
    break;
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
  case EM_SEGMENT:
    draw_text_to(as, "Segment", WHITE, 10, 10);
    break;
  case EM_RAY:
    draw_text_to(as, "Ray", WHITE, 10, 10);
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
  printf("%d %d %d\n", as->gs.p_n, as->gs.l_n, as->gs.c_n);

  clear_screen(as, BLACK);

  render_mode_info(as);

  {
    PointDef p1 = make_point_literal((Pos2D){0.0, 0.0});
    PointDef p2 = make_point_literal((Pos2D){10.0, 0.0});
    PointDef p3 = make_point_literal((Pos2D){0.0, 10.0});
    draw_point(as, &p1, WHITE);
    draw_point(as, &p2, WHITE);
    draw_point(as, &p3, WHITE);
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

  if (as->es.mode == EM_POINT) {
    PointDef potential_point = point__get_potential_point(as, w_mouse_pos);
    draw_point(as, &potential_point, CYAN);
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

int main(int argc, char *argv[]) {

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
              .point_defs = {},
              .p_n = 0,
              .line_defs = {},
              .l_n = 0,
              .circle_defs = {},
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
