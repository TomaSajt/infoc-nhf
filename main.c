#include "debugmalloc.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <SDL3_gfxPrimitives.h>

#include <stdio.h>
#include <stdlib.h>

#include "geom.h"
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
  PointDef *point_defs[100];
  int point_def_count;
  LineDef *line_defs[100];
  int line_def_count;
  CircleDef *circle_defs[100];
  int circle_def_count;
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
  for (int i = 0; i < as->circle_def_count; i++) {
    as->circle_defs[i]->val.dirty = true;
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
                   .val = {.dirty = true},
                   .id = as->point_def_count};
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
        .val = {.dirty = true},
        .id = as->line_def_count};
    as->line_defs[as->line_def_count++] = line_def;
    break;
  }
  case SDL_SCANCODE_C: {
    CircleDef *circle_def = malloc(sizeof(CircleDef));
    if (circle_def == NULL)
      return SDL_APP_FAILURE;

    *circle_def =
        (CircleDef){.type = CD_CENTER_POINT_OUTER_POINT,
                    .center_point_outer_point =
                        {.center = as->point_defs[as->point_def_count - 2],
                         .outer = as->point_defs[as->point_def_count - 1]},
                    .val = {.dirty = true},
                    .id = as->circle_def_count};
    as->circle_defs[as->circle_def_count++] = circle_def;
    break;
  }
  case SDL_SCANCODE_I: {

    PointDef *point_def = malloc(sizeof(PointDef));
    if (point_def == NULL)
      return SDL_APP_FAILURE;

    *point_def = (PointDef){
        .type = PD_INTSEC_LINE_LINE,
        .intsec_line_line = {.l1 = as->line_defs[as->line_def_count - 2],
                             .l2 = as->line_defs[as->line_def_count - 1]},
        .val = {.dirty = true},
        .id = as->point_def_count};
    as->point_defs[as->point_def_count++] = point_def;

    break;
  }
  case SDL_SCANCODE_J: {

    PointDef *point_def = malloc(sizeof(PointDef));
    if (point_def == NULL)
      return SDL_APP_FAILURE;

    *point_def = (PointDef){
        .type = PD_INTSEC_LINE_CIRCLE,
        .intsec_line_circle = {.l = as->line_defs[as->line_def_count - 1],
                               .c = as->circle_defs[as->circle_def_count - 1],
                               .prog_type = ILC_PROG_LOWER},
        .val = {.dirty = true},
        .id = as->point_def_count};
    as->point_defs[as->point_def_count++] = point_def;

    break;
  }
  case SDL_SCANCODE_K: {

    PointDef *point_def = malloc(sizeof(PointDef));
    if (point_def == NULL)
      return SDL_APP_FAILURE;

    *point_def = (PointDef){
        .type = PD_INTSEC_CIRCLE_CIRCLE,
        .intsec_circle_circle = {.c1 =
                                     as->circle_defs[as->circle_def_count - 2],
                                 .c2 =
                                     as->circle_defs[as->circle_def_count - 1],
                                 .side = ICC_LEFT},
        .val = {.dirty = true},
        .id = as->point_def_count};
    as->point_defs[as->point_def_count++] = point_def;
    break;
  }
  case SDL_SCANCODE_S: {
    SDL_Log("Saving...\n");
    FILE *handle = fopen("save.dat", "w");
    save_to_file(handle, as->point_defs, as->point_def_count, as->line_defs,
                 as->line_def_count, as->circle_defs, as->circle_def_count);
    fclose(handle);
    break;
  }
  case SDL_SCANCODE_R: {
    SDL_Log("Loading...\n");
    FILE *handle = fopen("save.dat", "r");
    SaveData sd;
    bool res = load_from_file(handle, &sd);
    if (res) {
      printf("Success\n");
      eval_point(sd.point_defs[0]);
      printf("Point0: %lf %lf\n", sd.point_defs[0]->val.pos.x,
             sd.point_defs[0]->val.pos.y);
    } else
      printf("Fail\n");

    fclose(handle);
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
      double x = event->button.x;
      double y = event->button.y;
      Pos2D w_pos =
          pos_screen_to_world(as->renderer, &as->view_info, (Pos2D){x, y});
      PointDef *point_def = malloc(sizeof(PointDef));
      if (point_def == NULL)
        return SDL_APP_FAILURE;

      *point_def = (PointDef){.type = PD_LITERAL,
                              .literal = {.pos = w_pos},
                              .val = {.dirty = true},
                              .id = as->point_def_count};
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

  SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(as->renderer);

  {
    PointDef p1 = {.literal = {.pos = {0.0, 0.0}}, .val = {.dirty = true}};
    PointDef p2 = {.literal = {.pos = {10.0, 0.0}}, .val = {.dirty = true}};
    PointDef p3 = {.literal = {.pos = {0.0, 10.0}}, .val = {.dirty = true}};
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

      if (closest_line_def == NULL || best_dist > d) {
        best_dist = d;
        closest_line_def = ld;
      }
    }
  }

  CircleDef *closest_circle_def = NULL;
  {
    double best_dist = 0;
    for (int i = 0; i < as->circle_def_count; i++) {
      CircleDef *cd = as->circle_defs[i];
      eval_circle(cd);
      if (cd->val.invalid)
        continue;

      double d =
          distance_from_circle(&mouse_pos, &cd->val.center, cd->val.radius);
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
      mark_everyting_dirty(as);
    }
  }

  for (int i = 0; i < as->point_def_count; i++) {
    PointDef *pd = as->point_defs[i];
    if (pd != closest_point_def) {
      w_draw_point(as, pd, 0xffffffff);
    }
  }
  if (closest_point_def != NULL) {
    w_draw_point(as, closest_point_def, 0xff0000ff);
  }

  for (int i = 0; i < as->line_def_count; i++) {
    LineDef *ld = as->line_defs[i];
    if (ld != closest_line_def) {
      w_draw_line(as, ld, 0xffffffff);
    }
  }
  if (closest_line_def != NULL) {
    w_draw_line(as, closest_line_def, 0xff0000ff);
  }

  for (int i = 0; i < as->circle_def_count; i++) {
    CircleDef *cd = as->circle_defs[i];
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

  AppState as = {.window = NULL,
                 .renderer = NULL,
                 .view_info = {.center = {.x = 0, .y = 0}, .scale = 1.0},
                 .point_defs = {},
                 .point_def_count = 0,
                 .line_defs = {},
                 .line_def_count = 0,
                 .circle_defs = {},
                 .circle_def_count = 0};

  SDL_AppResult rc = init_app(&as);
  mark_everyting_dirty(&as);

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

  for (int i = 0; i < as.circle_def_count; i++) {
    free(as.circle_defs[i]);
  }

  SDL_Quit();

  return rc == SDL_APP_FAILURE ? 1 : 0;
}
