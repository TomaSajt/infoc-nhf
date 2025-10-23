// #include "debugmalloc.h"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <SDL3_gfxPrimitives.h>

#include <stdio.h>
#include <stdlib.h>

#include "geom.h"

#define SDL_WINDOW_WIDTH 1000
#define SDL_WINDOW_HEIGHT 600

typedef struct {
  Pos2D center;
  double scale;
} ViewInfo;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  ViewInfo view_info;
} AppState;

Pos2D pos_world_to_view(ViewInfo *view_info, Pos2D pos) {
  return (Pos2D){.x = (pos.x - view_info->center.x) * view_info->scale,
                 .y = (pos.y - view_info->center.y) * view_info->scale};
}

Pos2D pos_view_to_screen(SDL_Renderer *renderer, Pos2D pos) {
  int ww, wh;
  SDL_GetRenderOutputSize(renderer, &ww, &wh);
  return (Pos2D){.x = ww * 0.5 + pos.x, .y = wh * 0.5 - pos.y};
}

Pos2D pos_world_to_screen(SDL_Renderer *renderer, ViewInfo *view_info,
                          Pos2D pos) {
  return pos_view_to_screen(renderer, pos_world_to_view(view_info, pos));
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
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult handle_event(AppState *as, SDL_Event *event) {
  SDL_Log("%d\n", event->type);
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_KEY_DOWN:
    return handle_key_event(as, event->key.scancode);
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    if (event->button.button == 1) {
      SDL_Log("Left-clicked!\n");
    }
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    as->view_info.scale *= pow(1.1, event->wheel.y);
    break;
  }
  return SDL_APP_CONTINUE;
}

void w_draw_point(SDL_Renderer *renderer, ViewInfo *view_info, Pos2D p) {

  Pos2D screen_pos = pos_world_to_screen(renderer, view_info, p);
  filledCircleRGBA(renderer, screen_pos.x - 5.0, screen_pos.y - 5.0, 10.0, 255,
                   255, 255, 255);
}

SDL_AppResult do_render(AppState *as) {

  SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(as->renderer);

  SDL_Log("%lf (%lf %lf)\n", as->view_info.scale, as->view_info.center.x,
          as->view_info.center.y);

  Point p = {.pos = {0.0, 0.0}};
  Point p2 = {.pos = {10.0, 0.0}};
  Point p3 = {.pos = {10.0, 10.0}};
  w_draw_point(as->renderer, &as->view_info, p.pos);
  w_draw_point(as->renderer, &as->view_info, p2.pos);
  w_draw_point(as->renderer, &as->view_info, p3.pos);

  SDL_FRect r = {.x = 30.0, .y = 30.0, .w = 100.0, .h = 100.0};
  SDL_SetRenderDrawColor(as->renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(as->renderer, &r);

  circleRGBA(as->renderer, 100.0, 100.0, 50.0, 0, 255, 0, 255);

  SDL_RenderPresent(as->renderer);
  return SDL_APP_CONTINUE;
}

int main(int argc, char *argv[]) {

  AppState as = {.window = NULL,
                 .renderer = NULL,
                 .view_info = {.center = {.x = 0, .y = 0}, .scale = 1.0}};

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

  SDL_Quit();

  return rc == SDL_APP_FAILURE ? 1 : 0;
}
