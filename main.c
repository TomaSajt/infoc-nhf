#include "debugmalloc.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <SDL3_gfxPrimitives.h>

#include <stdio.h>
#include <stdlib.h>

#include "SDL3/SDL_init.h"
#include "geom.h"

#define SDL_WINDOW_WIDTH 1000
#define SDL_WINDOW_HEIGHT 600

#define POINT_RENDER_RADIUS 10.0
#define POINT_HITBOX_RADIUS 12.0

#define LINE_HITBOX_RADIUS 8.0

typedef struct {
  Pos2D center;
  double scale;
} ViewInfo;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  ViewInfo view_info;
  PointDef *point_defs[100];
  int point_def_count;
  LineDef *line_defs[100];
  int line_def_count;
} AppState;

Pos2D pos_world_to_view(ViewInfo *view_info, Pos2D pos) {
  return (Pos2D){.x = (pos.x - view_info->center.x) * view_info->scale,
                 .y = (pos.y - view_info->center.y) * view_info->scale};
}

Pos2D pos_view_to_world(ViewInfo *view_info, Pos2D pos) {
  return (Pos2D){.x = pos.x / view_info->scale + view_info->center.x,
                 .y = pos.y / view_info->scale + view_info->center.y};
}

Pos2D pos_view_to_screen(SDL_Renderer *renderer, Pos2D pos) {
  int ww, wh;
  SDL_GetRenderOutputSize(renderer, &ww, &wh);
  return (Pos2D){.x = ww * 0.5 + pos.x, .y = wh * 0.5 - pos.y};
}

Pos2D pos_screen_to_view(SDL_Renderer *renderer, Pos2D pos) {
  int ww, wh;
  SDL_GetRenderOutputSize(renderer, &ww, &wh);
  return (Pos2D){.x = pos.x - ww * 0.5, .y = wh * 0.5 - pos.y};
}

Pos2D pos_world_to_screen(SDL_Renderer *renderer, ViewInfo *view_info,
                          Pos2D pos) {
  return pos_view_to_screen(renderer, pos_world_to_view(view_info, pos));
}

Pos2D pos_screen_to_world(SDL_Renderer *renderer, ViewInfo *view_info,
                          Pos2D pos) {
  return pos_view_to_world(view_info, pos_screen_to_view(renderer, pos));
}

void mark_everyting_dirty(AppState *as) {
  for (int i = 0; i < as->point_def_count; i++) {
    as->point_defs[i]->val.dirty = true;
  }
  for (int i = 0; i < as->line_def_count; i++) {
    as->line_defs[i]->val.dirty = true;
  }
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

    PointDef *point_def = malloc(sizeof(PointDef));
    if (point_def == NULL)
      return SDL_APP_FAILURE;

    *point_def =
        (PointDef){.type = PD_MIDPOINT,
                   .midpoint = {.p1 = as->point_defs[as->point_def_count - 2],
                                .p2 = as->point_defs[as->point_def_count - 1]},
                   .val = {.dirty = true}};
    as->point_defs[as->point_def_count++] = point_def;

    break;
  }
  case SDL_SCANCODE_L: {
    LineDef *line_def = malloc(sizeof(LineDef));
    if (line_def == NULL)
      return SDL_APP_FAILURE;

    *line_def = (LineDef){
        .type = LD_POINT_TO_POINT,
        .point_to_point = {.p1 = as->point_defs[as->point_def_count - 2],
                           .p2 = as->point_defs[as->point_def_count - 1]},
        .val = {.dirty = true}};
    as->line_defs[as->line_def_count++] = line_def;
  }
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult handle_event(AppState *as, SDL_Event *event) {
  SDL_Log("%d\n", event->type);
  switch (event->type) {
  case SDL_EVENT_QUIT: {
    return SDL_APP_SUCCESS;
  }
  case SDL_EVENT_KEY_DOWN: {
    return handle_key_event(as, event->key.scancode);
  }
  case SDL_EVENT_MOUSE_BUTTON_DOWN: {
    if (event->button.button == 1) {
      SDL_Log("Left-clicked!\n");
      double x = event->button.x;
      double y = event->button.y;
      Pos2D w_pos =
          pos_screen_to_world(as->renderer, &as->view_info, (Pos2D){x, y});
      PointDef *point_def = malloc(sizeof(PointDef));
      if (point_def == NULL)
        return SDL_APP_FAILURE;

      *point_def = (PointDef){.type = PD_LITERAL,
                              .literal = {.pos = w_pos},
                              .val = {.dirty = true}};
      as->point_defs[as->point_def_count++] = point_def;
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

void w_draw_point(SDL_Renderer *renderer, ViewInfo *view_info, Pos2D p,
                  Uint8 shade) {

  Pos2D screen_pos = pos_world_to_screen(renderer, view_info, p);
  filledCircleRGBA(renderer, screen_pos.x, screen_pos.y, POINT_RENDER_RADIUS,
                   shade, shade, shade, 255);
}

void w_draw_line(SDL_Renderer *renderer, ViewInfo *view_info, Pos2D start,
                 Pos2D end, Uint8 shade) {

  Pos2D screen_start = pos_world_to_screen(renderer, view_info, start);
  Pos2D screen_end = pos_world_to_screen(renderer, view_info, end);
  lineRGBA(renderer, screen_start.x, screen_start.y, screen_end.x, screen_end.y,
           shade, shade, shade, 255);
}

SDL_AppResult do_render(AppState *as) {

  SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(as->renderer);

  SDL_Log("%lf (%lf %lf)\n", as->view_info.scale, as->view_info.center.x,
          as->view_info.center.y);

  PointEvaled p = {.pos = {0.0, 0.0}};
  PointEvaled p2 = {.pos = {10.0, 0.0}};
  PointEvaled p3 = {.pos = {0.0, 10.0}};
  w_draw_point(as->renderer, &as->view_info, p.pos, 255);
  w_draw_point(as->renderer, &as->view_info, p2.pos, 255);
  w_draw_point(as->renderer, &as->view_info, p3.pos, 255);

  mark_everyting_dirty(as);

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
    for (int i = 0; i < as->point_def_count; i++) {
      PointDef *pd = as->point_defs[i];
      eval_point(pd);
      if (pd->val.invalid)
        continue;

      double d = pos_distance(&pd->val.pos, &mouse_pos);
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
    for (int i = 0; i < as->line_def_count; i++) {
      LineDef *ld = as->line_defs[i];
      eval_line(ld);
      if (ld->val.invalid)
        continue;

      double d = distance_from_line(&mouse_pos, &ld->val.start, &ld->val.end);
      if (d * as->view_info.scale > LINE_HITBOX_RADIUS)
        continue;

      if (closest_point_def == NULL || best_dist > d) {
        best_dist = d;
        closest_line_def = ld;
      }
    }
  }

  if ((flags & SDL_BUTTON_RMASK) != 0) {
    if (closest_point_def != NULL && closest_point_def->type == PD_LITERAL) {
      closest_point_def->literal.pos = mouse_pos;
      mark_everyting_dirty(as);
    }
  }

  for (int i = 0; i < as->point_def_count; i++) {
    PointDef *pd = as->point_defs[i];
    eval_point(pd);
    if (pd != closest_point_def && !pd->val.invalid) {
      w_draw_point(as->renderer, &as->view_info, pd->val.pos, 200);
    }
  }
  if (closest_point_def != NULL) {
    w_draw_point(as->renderer, &as->view_info, closest_point_def->val.pos, 255);
  }

  for (int i = 0; i < as->line_def_count; i++) {
    LineDef *ld = as->line_defs[i];
    eval_line(ld);
    if (ld != closest_line_def && !ld->val.invalid) {
      w_draw_line(as->renderer, &as->view_info, ld->val.start, ld->val.end,
                  200);
    }
  }
  if (closest_line_def != NULL) {
    w_draw_line(as->renderer, &as->view_info, closest_line_def->val.start,
                closest_line_def->val.end, 255);
  }

  SDL_RenderPresent(as->renderer);
  return SDL_APP_CONTINUE;
}

int main(int argc, char *argv[]) {

  AppState as = {.window = NULL,
                 .renderer = NULL,
                 .view_info = {.center = {.x = 0, .y = 0}, .scale = 1.0},
                 .point_defs = {},
                 .point_def_count = 0,
                 .line_defs = {},
                 .line_def_count = 0};

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

  for (int i = 0; i < as.point_def_count; i++) {
    free(as.point_defs[i]);
  }

  for (int i = 0; i < as.line_def_count; i++) {
    free(as.line_defs[i]);
  }

  SDL_Quit();

  return rc == SDL_APP_FAILURE ? 1 : 0;
}
