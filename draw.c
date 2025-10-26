#include "draw.h"
#include "geom.h"

#include <SDL3_gfxPrimitives.h>

Pos2D pos_world_to_view(ViewInfo *view_info, Pos2D pos) {
  return (Pos2D){
      .x = (pos.x - view_info->center.x) * view_info->scale,
      .y = (pos.y - view_info->center.y) * view_info->scale,
  };
}

Pos2D pos_view_to_world(ViewInfo *view_info, Pos2D pos) {
  return (Pos2D){
      .x = pos.x / view_info->scale + view_info->center.x,
      .y = pos.y / view_info->scale + view_info->center.y,
  };
}

Pos2D pos_view_to_screen(SDL_Renderer *renderer, Pos2D pos) {
  int ww, wh;
  SDL_GetRenderOutputSize(renderer, &ww, &wh);
  return (Pos2D){
      .x = ww * 0.5 + pos.x,
      .y = wh * 0.5 - pos.y,
  };
}

Pos2D pos_screen_to_view(SDL_Renderer *renderer, Pos2D pos) {
  int ww, wh;
  SDL_GetRenderOutputSize(renderer, &ww, &wh);
  return (Pos2D){
      .x = pos.x - ww * 0.5,
      .y = wh * 0.5 - pos.y,
  };
}

Pos2D pos_world_to_screen(SDL_Renderer *renderer, ViewInfo *view_info,
                          Pos2D pos) {
  return pos_view_to_screen(renderer, pos_world_to_view(view_info, pos));
}

Pos2D pos_screen_to_world(SDL_Renderer *renderer, ViewInfo *view_info,
                          Pos2D pos) {
  return pos_view_to_world(view_info, pos_screen_to_view(renderer, pos));
}

void draw_point(AppState *as, PointDef *pd, SDL_Color color) {
  eval_point(pd);
  if (pd->val.invalid)
    return;

  Pos2D screen_pos =
      pos_world_to_screen(as->renderer, &as->view_info, pd->val.pos);

  filledCircleRGBA(as->renderer, screen_pos.x, screen_pos.y,
                   POINT_HITBOX_RADIUS, color.r, color.g, color.b, color.a / 3);
  filledCircleRGBA(as->renderer, screen_pos.x, screen_pos.y,
                   POINT_RENDER_RADIUS, color.r, color.g, color.b, color.a);
}

void draw_line(AppState *as, LineDef *ld, SDL_Color color) {
  eval_line(ld);
  if (ld->val.invalid)
    return;

  Pos2D screen_start =
      pos_world_to_screen(as->renderer, &as->view_info, ld->val.start);
  Pos2D screen_end =
      pos_world_to_screen(as->renderer, &as->view_info, ld->val.end);
  lineRGBA(as->renderer, screen_start.x, screen_start.y, screen_end.x,
           screen_end.y, color.r, color.g, color.b, color.a);
}

void draw_circle(AppState *as, CircleDef *cd, SDL_Color color) {
  eval_circle(cd);
  if (cd->val.invalid)
    return;

  Pos2D screen_center =
      pos_world_to_screen(as->renderer, &as->view_info, cd->val.center);
  double screen_radius = as->view_info.scale * cd->val.radius;
  circleRGBA(as->renderer, screen_center.x, screen_center.y, screen_radius,
             color.r, color.g, color.b, color.a);
}

SDL_Texture *make_text_texture(AppState *as, char *text, SDL_Color color) {
  SDL_Surface *text_surf = TTF_RenderText_Blended(as->font, text, 0, color);
  if (text_surf == NULL)
    return NULL;
  SDL_Texture *texture = SDL_CreateTextureFromSurface(as->renderer, text_surf);
  SDL_DestroySurface(text_surf);
  return texture;
}

bool draw_texture_to(AppState *as, SDL_Texture *texture, float x, float y) {
  SDL_FRect rect = {.x = x, .y = y, .w = texture->w, .h = texture->h};
  return SDL_RenderTexture(as->renderer, texture, NULL, &rect);
}

bool draw_text_to(AppState *as, char *text, SDL_Color color, float x, float y) {
  SDL_Texture *texture = make_text_texture(as, text, color);
  if (texture == NULL)
    return false;
  bool res = draw_texture_to(as, texture, x, y);
  SDL_DestroyTexture(texture);
  return res;
}
