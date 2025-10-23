#include "debugmalloc.h"

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <SDL3_gfxPrimitives.h>

#include <stdio.h>
#include <stdlib.h>

#define SDL_WINDOW_WIDTH 1000
#define SDL_WINDOW_HEIGHT 600

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
} AppState;

SDL_AppResult handle_key_event(AppState *as, SDL_Scancode key_code) {
  switch (key_code) {
  case SDL_SCANCODE_ESCAPE:
  case SDL_SCANCODE_Q:
    return SDL_APP_SUCCESS;
  case SDL_SCANCODE_RIGHT:
    printf("RIGHT\n");
    break;
  case SDL_SCANCODE_UP:
    printf("UP\n");
    break;
  case SDL_SCANCODE_LEFT:
    printf("LEFT\n");
    break;
  case SDL_SCANCODE_DOWN:
    printf("DOWN\n");
    break;
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *as = (AppState *)appstate;

  SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(as->renderer);

  SDL_FRect r = {.x = 30.0, .y = 30.0, .w = 100.0, .h = 100.0};
  SDL_SetRenderDrawColor(as->renderer, 255, 255, 0, SDL_ALPHA_OPAQUE); /*head*/
  SDL_RenderFillRect(as->renderer, &r);

  circleRGBA(as->renderer, 100.0, 100.0, 50.0, 0, 255, 0, 255);

  SDL_RenderPresent(as->renderer);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **_as, int argc, char *argv[]) {
  if (!SDL_SetAppMetadata("Test SDL3 App", "1.0",
                          "net.tomasajt.TestSDL3App")) {
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }
  AppState *as = (AppState *)malloc(sizeof(AppState));
  if (!as) {
    return SDL_APP_FAILURE;
  }
  *as = (AppState){.window = NULL, .renderer = NULL};

  int window_flags = SDL_WINDOW_RESIZABLE;

  if (!SDL_CreateWindowAndRenderer("NHF1", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT,
                                   window_flags, &as->window, &as->renderer)) {
    return SDL_APP_FAILURE;
  }

  *_as = as;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *_as, SDL_Event *event) {
  AppState *as = (AppState *)_as;
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_KEY_DOWN:
    return handle_key_event(as, event->key.scancode);
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *_as, SDL_AppResult result) {
  AppState *as = (AppState *)_as;
  if (as != NULL) {
    SDL_DestroyRenderer(as->renderer);
    SDL_DestroyWindow(as->window);
    free(as);
  }
}
