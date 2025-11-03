#ifndef STATE_H
#define STATE_H

#include "geom/state.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
  Pos2D center;
  double scale;
} ViewInfo;

typedef enum {
  EM_MOVE,
  EM_DELETE,
  EM_POINT,
  EM_MIDPOINT,
  EM_LINE,
  EM_CIRCLE,
  EM_CIRCLE_BY_LEN,
} EditorMode;

typedef enum {
  FE_NONE = 0,
  FE_POINT = 1,
  FE_LINE = 2,
  FE_CIRCLE = 3,
} FocusedElemType;

typedef struct {
  EditorMode mode;
  FocusedElemType elem_type;
  union {
    PointDef *p;
    LineDef *l;
    CircleDef *c;
  };
} EditorState;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
  ViewInfo view_info;
  GeometryState gs;
  EditorState es;
} AppState;

#endif
