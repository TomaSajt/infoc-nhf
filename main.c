#include "debugmalloc.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <SDL3_gfxPrimitives.h>

#include <stdio.h>
#include <stdlib.h>

#include "geom.h"
#include "geom_defs.h"
#include "geom_state.h"
#include "savedata.h"

#define SDL_WINDOW_WIDTH 1000
#define SDL_WINDOW_HEIGHT 600

#define POINT_RENDER_RADIUS 10.0
#define POINT_HITBOX_RADIUS 15.0

#define LINE_HITBOX_RADIUS 8.0

typedef struct {
  Pos2D center;
  double scale;
} ViewInfo;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  ViewInfo view_info;
  GeometryState gs;
} AppState;

Pos2D pos_world_to_view(ViewInfo *view_info, Pos2D pos) {
  return (Pos2D){
      .x = (pos.x - view_info->center.x) * view_info->scale,
      .y = (pos.y - view_info->center.y) * view_info->scale,
  };
}

Pos2D pos_view_to_world(ViewInfo *view_info, Pos2D pos) {
  return (Pos2D){
      .x = pos.x / view_info->scale + view_info->center.x,
      .y = pos.y / view_info->scale + view_info->center.y,
  };
}

Pos2D pos_view_to_screen(SDL_Renderer *renderer, Pos2D pos) {
  int ww, wh;
  SDL_GetRenderOutputSize(renderer, &ww, &wh);
  return (Pos2D){
      .x = ww * 0.5 + pos.x,
      .y = wh * 0.5 - pos.y,
  };
}

Pos2D pos_screen_to_view(SDL_Renderer *renderer, Pos2D pos) {
  int ww, wh;
  SDL_GetRenderOutputSize(renderer, &ww, &wh);
  return (Pos2D){
      .x = pos.x - ww * 0.5,
      .y = wh * 0.5 - pos.y,
  };
}

Pos2D pos_world_to_screen(SDL_Renderer *renderer, ViewInfo *view_info,
                          Pos2D pos) {
  return pos_view_to_screen(renderer, pos_world_to_view(view_info, pos));
}

Pos2D pos_screen_to_world(SDL_Renderer *renderer, ViewInfo *view_info,
                          Pos2D pos) {
  return pos_view_to_world(view_info, pos_screen_to_view(renderer, pos));
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

  if (!SDL_CreateWindowAndRenderer("NHF1", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT,
                                   window_flags, &as->window, &as->renderer)) {
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

void do_save(AppState *as) {
  SDL_Log("Saving...\n");
  FILE *handle = fopen("save.dat", "w");
  if (handle != NULL) {
    save_to_file(handle, &as->gs);
    fclose(handle);
  } else {
    printf("Failed to open file\n");
  }
}

SDL_AppResult handle_key_event(AppState *as, SDL_Scancode key_code) {
  switch (key_code) {
  case SDL_SCANCODE_ESCAPE:
  case SDL_SCANCODE_Q:
    return SDL_APP_SUCCESS;
  case SDL_SCANCODE_RIGHT:
    printf("RIGHT\n");
    as->view_info.center.x += 1.0;
    break;
  case SDL_SCANCODE_UP:
    printf("UP\n");
    as->view_info.center.y += 1.0;
    break;
  case SDL_SCANCODE_LEFT:
    printf("LEFT\n");
    as->view_info.center.x -= 1.0;
    break;
  case SDL_SCANCODE_DOWN:
    printf("DOWN\n");
    as->view_info.center.y -= 1.0;
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
                                        as->gs.circle_defs[as->gs.c_n - 1]);
    register_point(&as->gs, pd);
    break;
  }
  case SDL_SCANCODE_K: {
    PointDef *pd = malloc(sizeof(PointDef));
    if (pd == NULL)
      return SDL_APP_FAILURE;

    *pd = make_point_intsec_circle_circle(as->gs.circle_defs[as->gs.c_n - 2],
                                          as->gs.circle_defs[as->gs.c_n - 1]);
    register_point(&as->gs, pd);
    break;
  }
  case SDL_SCANCODE_S: {
    do_save(as);
    break;
  }
  case SDL_SCANCODE_R: {
    SDL_Log("Loading...\n");
    FILE *handle = fopen("save.dat", "r");
    if (handle != NULL) {
      GeometryState new_gs = {};
      bool res = load_from_file(handle, &new_gs);
      if (res) {
        clear_geometry_state(&as->gs);
        as->gs = new_gs;
        printf("Load successful\n");
      } else {
        printf("Load failed\n");
      }

      fclose(handle);
    } else {
      printf("Failed to open file\n");
    }
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

SDL_AppResult handle_event(AppState *as, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT: {
    return SDL_APP_SUCCESS;
  }
  case SDL_EVENT_KEY_DOWN: {
    return handle_key_event(as, event->key.scancode);
  }
  case SDL_EVENT_MOUSE_BUTTON_DOWN: {
    if (event->button.button == 1) {
      Pos2D s_pos = (Pos2D){
          .x = event->button.x,
          .y = event->button.y,
      };
      Pos2D w_pos = pos_screen_to_world(as->renderer, &as->view_info, s_pos);

      PointDef *pd = malloc(sizeof(PointDef));
      if (pd == NULL)
        return SDL_APP_FAILURE;

      *pd = make_point_literal(w_pos);
      register_point(&as->gs, pd);
    }
    break;
  }
  case SDL_EVENT_MOUSE_WHEEL: {
    as->view_info.scale *= pow(1.1, event->wheel.y);
    break;
  }
  }
  return SDL_APP_CONTINUE;
}

void w_draw_point(AppState *as, PointDef *pd, Uint32 color) {
  eval_point(pd);
  if (pd->val.invalid)
    return;
  Pos2D screen_pos =
      pos_world_to_screen(as->renderer, &as->view_info, pd->val.pos);

  filledCircleColor(as->renderer, screen_pos.x, screen_pos.y,
                    POINT_RENDER_RADIUS, color);
}

void w_draw_line(AppState *as, LineDef *ld, Uint32 color) {
  eval_line(ld);

  Pos2D screen_start =
      pos_world_to_screen(as->renderer, &as->view_info, ld->val.start);
  Pos2D screen_end =
      pos_world_to_screen(as->renderer, &as->view_info, ld->val.end);
  lineColor(as->renderer, screen_start.x, screen_start.y, screen_end.x,
            screen_end.y, color);
}

void w_draw_circle(AppState *as, CircleDef *cd, Uint32 color) {
  eval_circle(cd);

  Pos2D screen_center =
      pos_world_to_screen(as->renderer, &as->view_info, cd->val.center);
  double screen_radius = as->view_info.scale * cd->val.radius;
  circleColor(as->renderer, screen_center.x, screen_center.y, screen_radius,
              color);
}

SDL_AppResult do_render(AppState *as) {
  printf("%d %d %d\n", as->gs.p_n, as->gs.l_n, as->gs.c_n);

  SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(as->renderer);

  {
    PointDef p1 = make_point_literal((Pos2D){0.0, 0.0});
    PointDef p2 = make_point_literal((Pos2D){10.0, 0.0});
    PointDef p3 = make_point_literal((Pos2D){0.0, 10.0});
    w_draw_point(as, &p1, 0xffffffff);
    w_draw_point(as, &p2, 0xffffffff);
    w_draw_point(as, &p3, 0xffffffff);
  }

  Pos2D mouse_pos;
  SDL_MouseButtonFlags flags;
  {
    float wmx, wmy;
    flags = SDL_GetMouseState(&wmx, &wmy);
    float mx, my;
    SDL_RenderCoordinatesFromWindow(as->renderer, wmx, wmy, &mx, &my);
    mouse_pos = pos_screen_to_world(as->renderer, &as->view_info,
                                    (Pos2D){.x = mx, .y = my});
  }

  PointDef *closest_point_def = NULL;
  {
    double best_dist = 0;
    for (int i = 0; i < as->gs.p_n; i++) {
      PointDef *pd = as->gs.point_defs[i];
      eval_point(pd);
      if (pd->val.invalid)
        continue;

      double d = dist_from_pos(&pd->val.pos, &mouse_pos);
      if (d * as->view_info.scale > POINT_HITBOX_RADIUS)
        continue;

      if (closest_point_def == NULL || best_dist > d) {
        best_dist = d;
        closest_point_def = pd;
      }
    }
  }

  LineDef *closest_line_def = NULL;
  {
    double best_dist = 0;
    for (int i = 0; i < as->gs.l_n; i++) {
      LineDef *ld = as->gs.line_defs[i];
      eval_line(ld);
      if (ld->val.invalid)
        continue;

      double d = dist_from_line(&mouse_pos, &ld->val.start, &ld->val.end);
      if (d * as->view_info.scale > LINE_HITBOX_RADIUS)
        continue;

      if (closest_line_def == NULL || best_dist > d) {
        best_dist = d;
        closest_line_def = ld;
      }
    }
  }

  CircleDef *closest_circle_def = NULL;
  {
    double best_dist = 0;
    for (int i = 0; i < as->gs.c_n; i++) {
      CircleDef *cd = as->gs.circle_defs[i];
      eval_circle(cd);
      if (cd->val.invalid)
        continue;

      double d = dist_from_circle(&mouse_pos, &cd->val.center, cd->val.radius);
      if (d * as->view_info.scale > LINE_HITBOX_RADIUS)
        continue;

      if (closest_circle_def == NULL || best_dist > d) {
        best_dist = d;
        closest_circle_def = cd;
      }
    }
  }

  if ((flags & SDL_BUTTON_RMASK) != 0) {
    if (closest_point_def != NULL && closest_point_def->type == PD_LITERAL) {
      closest_point_def->literal.pos = mouse_pos;
      mark_everyting_dirty(&as->gs);
    }
  }
  if ((flags & SDL_BUTTON_MMASK) != 0) {
    if (closest_point_def != NULL) {
      delete_point(&as->gs, closest_point_def);
      // just to be safe
      // TODO: recalculate correct closest
      closest_point_def = NULL;
      closest_line_def = NULL;
      closest_circle_def = NULL;
    }
  }

  for (int i = 0; i < as->gs.p_n; i++) {
    PointDef *pd = as->gs.point_defs[i];
    if (pd != closest_point_def) {
      w_draw_point(as, pd, 0xffffffff);
    }
  }
  if (closest_point_def != NULL) {
    w_draw_point(as, closest_point_def, 0xff0000ff);
  }

  for (int i = 0; i < as->gs.l_n; i++) {
    LineDef *ld = as->gs.line_defs[i];
    if (ld != closest_line_def) {
      w_draw_line(as, ld, 0xffffffff);
    }
  }
  if (closest_line_def != NULL) {
    w_draw_line(as, closest_line_def, 0xff0000ff);
  }

  for (int i = 0; i < as->gs.c_n; i++) {
    CircleDef *cd = as->gs.circle_defs[i];
    if (cd != closest_circle_def) {
      w_draw_circle(as, cd, 0xffffffff);
    }
  }
  if (closest_circle_def != NULL) {
    w_draw_circle(as, closest_circle_def, 0xff0000ff);
  }

  SDL_RenderPresent(as->renderer);
  return SDL_APP_CONTINUE;
}

int main(int argc, char *argv[]) {

  AppState as = {
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
  };

  SDL_AppResult rc = init_app(&as);

  while (rc == SDL_APP_CONTINUE) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    rc = handle_event(&as, &event);

    if (rc != SDL_APP_CONTINUE)
      break;

    rc = do_render(&as);
  }

  if (as.renderer != NULL)
    SDL_DestroyRenderer(as.renderer);
  if (as.window != NULL)
    SDL_DestroyWindow(as.window);

  clear_geometry_state(&as.gs);

  SDL_Quit();

  return rc == SDL_APP_FAILURE ? 1 : 0;
}
