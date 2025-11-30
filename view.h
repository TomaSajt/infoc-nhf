#ifndef VIEW_H
#define VIEW_H

#include "geom/defs.h"
#include <SDL3/SDL_render.h>

typedef struct {
  Pos2D center;
  double scale;
} ViewInfo;

void zoom(ViewInfo *view_info, Pos2D fp, double mul);

Pos2D pos_world_to_view(ViewInfo const *view_info, Pos2D pos);

Pos2D pos_view_to_world(ViewInfo const *view_info, Pos2D pos);

Pos2D pos_view_to_screen(SDL_Renderer *renderer, Pos2D pos);

Pos2D pos_screen_to_view(SDL_Renderer *renderer, Pos2D pos);

Pos2D pos_world_to_screen(SDL_Renderer *renderer, ViewInfo const *view_info,
                          Pos2D pos);

Pos2D pos_screen_to_world(SDL_Renderer *renderer, ViewInfo const *view_info,
                          Pos2D pos);

#endif
