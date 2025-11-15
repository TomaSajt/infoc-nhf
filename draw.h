#ifndef DRAW_H
#define DRAW_H

#include "state.h"

#define POINT_RENDER_RADIUS 4.0
#define POINT_HITBOX_RADIUS 10.0

#define LINE_HITBOX_RADIUS 8.0

extern SDL_Color const BLACK;
extern SDL_Color const RED;
extern SDL_Color const GREEN;
extern SDL_Color const YELLOW;
extern SDL_Color const BLUE;
extern SDL_Color const MAGENTA;
extern SDL_Color const CYAN;
extern SDL_Color const WHITE;

Pos2D pos_world_to_view(ViewInfo const *view_info, Pos2D pos);
Pos2D pos_view_to_world(ViewInfo const *view_info, Pos2D pos);
Pos2D pos_view_to_screen(SDL_Renderer *renderer, Pos2D pos);
Pos2D pos_screen_to_view(SDL_Renderer *renderer, Pos2D pos);
Pos2D pos_world_to_screen(SDL_Renderer *renderer, ViewInfo const *view_info,
                          Pos2D pos);

Pos2D pos_screen_to_world(SDL_Renderer *renderer, ViewInfo const *view_info,
                          Pos2D pos);

void draw_point(AppState const *as, PointDef *pd, SDL_Color color);
void draw_line(AppState const *as, LineDef *ld, SDL_Color color);
void draw_circle(AppState const *as, CircleDef *cd, SDL_Color color);

SDL_Texture *make_text_texture(AppState const *as, char const *text,
                               SDL_Color color);
bool draw_texture_to(SDL_Renderer *renderer, SDL_Texture *texture, float x,
                     float y);
bool draw_text_to(AppState const *as, char const *text, SDL_Color color,
                  float x, float y);

#endif
