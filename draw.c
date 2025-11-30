#include "draw.h"
#include "geom/defs.h"
#include "geom/util.h"

#include <SDL3_gfxPrimitives.h>

SDL_Color const BLACK = {.r = 0x00, .g = 0x00, .b = 0x00, .a = 0xff};
SDL_Color const RED = {.r = 0xff, .g = 0x00, .b = 0x00, .a = 0xff};
SDL_Color const GREEN = {.r = 0x00, .g = 0xff, .b = 0x00, .a = 0xff};
SDL_Color const YELLOW = {.r = 0xff, .g = 0xff, .b = 0x00, .a = 0xff};
SDL_Color const BLUE = {.r = 0x00, .g = 0x00, .b = 0xff, .a = 0xff};
SDL_Color const MAGENTA = {.r = 0xff, .g = 0x00, .b = 0xff, .a = 0xff};
SDL_Color const CYAN = {.r = 0x00, .g = 0xff, .b = 0xff, .a = 0xff};
SDL_Color const WHITE = {.r = 0xff, .g = 0xff, .b = 0xff, .a = 0xff};

bool is_point_movable(PointDef const *pd) {
  return pd->type == PD_LITERAL || pd->type == PD_GLIDER_ON_LINE ||
         pd->type == PD_GLIDER_ON_CIRCLE;
}

void draw_point(AppState const *as, PointDef *pd) {
  eval_point(pd);
  if (pd->val.invalid)
    return;

  Pos2D screen_pos =
      pos_world_to_screen(as->renderer, &as->view_info, pd->val.pos);

  if (is_point_movable(pd)) {
    filledCircleRGBA(as->renderer, screen_pos.x, screen_pos.y,
                     POINT_HITBOX_RADIUS, pd->color.r, pd->color.g, pd->color.b,
                     pd->color.a / 4);
  }
  filledCircleRGBA(as->renderer, screen_pos.x, screen_pos.y,
                   POINT_RENDER_RADIUS, pd->color.r, pd->color.g, pd->color.b,
                   pd->color.a);
}

void clamp_line_ends_onto_screen(Pos2D const *s, Pos2D const *e, double sc_w,
                                 double sc_h, double *prog_s_out,
                                 double *prog_e_out) {
  double vx = e->x - s->x;
  double vy = e->y - s->y;
  // bool use_x_bounds = fabs(vy) / fabs(vx) < (double)sc_h / sc_w;
  bool use_x_bounds = fabs(vy) * sc_w < fabs(vx) * sc_h;
  if (use_x_bounds) {
    *prog_s_out = (0 - s->x) / vx;    // x == 0
    *prog_e_out = (sc_w - s->x) / vx; // x == w
  } else {
    *prog_s_out = (0 - s->y) / vy;    // y == 0
    *prog_e_out = (sc_h - s->y) / vy; // y == h
  }
}

void draw_line(AppState const *as, LineDef *ld) {
  eval_line(ld);
  if (ld->val.invalid)
    return;

  Pos2D screen_start =
      pos_world_to_screen(as->renderer, &as->view_info, ld->val.start);
  Pos2D screen_end =
      pos_world_to_screen(as->renderer, &as->view_info, ld->val.end);

  int sc_w, sc_h;
  SDL_GetRenderOutputSize(as->renderer, &sc_w, &sc_h);

  double prog_s, prog_e;
  if (ld->ext_mode == L_EXT_SEGMENT) {
    prog_s = 0;
    prog_e = 1;
  } else {
    clamp_line_ends_onto_screen(&screen_start, &screen_end, sc_w, sc_h, &prog_s,
                                &prog_e);
    if (ld->ext_mode == L_EXT_RAY) {
      prog_e = prog_s > prog_e ? prog_s : prog_e;
      if (prog_e < 0)
        return;
      prog_s = 0;
    }
  }

  Pos2D s = lerp(&screen_start, &screen_end, prog_s);
  Pos2D e = lerp(&screen_start, &screen_end, prog_e);

  if (false) {
    // TODO: add toggle
    filledCircleRGBA(as->renderer, s.x, s.y, POINT_RENDER_RADIUS, RED.r, RED.g,
                     RED.b, RED.a);
    filledCircleRGBA(as->renderer, e.x, e.y, POINT_RENDER_RADIUS, GREEN.r,
                     GREEN.g, GREEN.b, GREEN.a);
  }

  lineRGBA(as->renderer, s.x, s.y, e.x, e.y, ld->color.r, ld->color.g,
           ld->color.b, ld->color.a);
}

void draw_circle(AppState const *as, CircleDef *cd) {
  eval_circle(cd);
  if (cd->val.invalid)
    return;

  Pos2D screen_center =
      pos_world_to_screen(as->renderer, &as->view_info, cd->val.center);
  double screen_radius = as->view_info.scale * cd->val.radius;

  // TODO: fix circle drawing glitch when circle is much bigger than the screen
  circleRGBA(as->renderer, screen_center.x, screen_center.y, screen_radius,
             cd->color.r, cd->color.g, cd->color.b, cd->color.a);
}

SDL_Texture *make_text_texture(AppState const *as, char const *text,
                               SDL_Color color) {
  SDL_Surface *text_surf = TTF_RenderText_Blended(as->font, text, 0, color);
  if (text_surf == NULL)
    return NULL;
  SDL_Texture *texture = SDL_CreateTextureFromSurface(as->renderer, text_surf);
  SDL_DestroySurface(text_surf);
  return texture;
}

bool draw_texture_to(SDL_Renderer *renderer, SDL_Texture *texture, float x,
                     float y) {
  SDL_FRect rect = {.x = x, .y = y, .w = texture->w, .h = texture->h};
  return SDL_RenderTexture(renderer, texture, NULL, &rect);
}

bool draw_text_to(AppState const *as, char const *text, SDL_Color color,
                  float x, float y) {
  SDL_Texture *texture = make_text_texture(as, text, color);
  if (texture == NULL)
    return false;
  bool res = draw_texture_to(as->renderer, texture, x, y);
  SDL_DestroyTexture(texture);
  return res;
}

void clear_screen(AppState *as, SDL_Color color) {
  SDL_SetRenderDrawColor(as->renderer, color.r, color.g, color.b, color.a);
  SDL_RenderClear(as->renderer);
}
