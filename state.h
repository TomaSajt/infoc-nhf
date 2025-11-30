#ifndef STATE_H
#define STATE_H

#include "geom/defs.h"
#include "geom/state.h"
#include "mode/defs.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
  Pos2D center;
  double scale;
} ViewInfo;

typedef struct AppState {
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
  ViewInfo view_info;
  GeometryState gs;
  EditorState es;
  char *save_path;
} AppState;

void zoom(ViewInfo *view_info, Pos2D fp, double mul);

#endif
