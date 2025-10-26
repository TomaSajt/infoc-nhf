#pragma once

#include "state.h"

#define POINT_RENDER_RADIUS 4.0
#define POINT_HITBOX_RADIUS 10.0

#define LINE_HITBOX_RADIUS 8.0

Pos2D pos_world_to_view(ViewInfo *view_info, Pos2D pos);
Pos2D pos_view_to_world(ViewInfo *view_info, Pos2D pos);
Pos2D pos_view_to_screen(SDL_Renderer *renderer, Pos2D pos);
Pos2D pos_screen_to_view(SDL_Renderer *renderer, Pos2D pos);
Pos2D pos_world_to_screen(SDL_Renderer *renderer, ViewInfo *view_info,
                          Pos2D pos);

Pos2D pos_screen_to_world(SDL_Renderer *renderer, ViewInfo *view_info,
                          Pos2D pos);

void draw_point(AppState *as, PointDef *pd, SDL_Color color);
void draw_line(AppState *as, LineDef *ld, SDL_Color color);
void draw_circle(AppState *as, CircleDef *cd, SDL_Color color);

SDL_Texture *make_text_texture(AppState *as, char *text, SDL_Color color);
bool draw_texture_to(AppState *as, SDL_Texture *texture, float x, float y);
bool draw_text_to(AppState *as, char *text, SDL_Color color, float x, float y);

static const SDL_Color BLACK = {.r = 0x00, .g = 0x00, .b = 0x00, .a = 0xff};
static const SDL_Color RED = {.r = 0xff, .g = 0x00, .b = 0x00, .a = 0xff};
static const SDL_Color GREEN = {.r = 0x00, .g = 0xff, .b = 0x00, .a = 0xff};
static const SDL_Color YELLOW = {.r = 0xff, .g = 0xff, .b = 0x00, .a = 0xff};
static const SDL_Color BLUE = {.r = 0x00, .g = 0x00, .b = 0xff, .a = 0xff};
static const SDL_Color MAGENTA = {.r = 0xff, .g = 0x00, .b = 0xff, .a = 0xff};
static const SDL_Color CYAN = {.r = 0x00, .g = 0xff, .b = 0xff, .a = 0xff};
static const SDL_Color WHITE = {.r = 0xff, .g = 0xff, .b = 0xff, .a = 0xff};
