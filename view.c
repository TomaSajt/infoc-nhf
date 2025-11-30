#include "view.h"

/**
 * Multiply the input ViewInfo's `.scale` field by `mul` and modify the
 * `.center` field so that the given fix point stays on the screen where it
 * originally was.
 *
 * \param view_info the ViewInfo struct to modify
 * \param fp the fix point on the plane
 * \param mul the scaling factor
 */
void zoom(ViewInfo *view_info, Pos2D fp, double mul) {
  *view_info = (ViewInfo){
      .scale = view_info->scale * mul,
      .center =
          {
              .x = fp.x + (view_info->center.x - fp.x) / mul,
              .y = fp.y + (view_info->center.y - fp.y) / mul,
          },
  };
}

Pos2D pos_world_to_view(ViewInfo const *view_info, Pos2D pos) {
  return (Pos2D){
      .x = (pos.x - view_info->center.x) * view_info->scale,
      .y = (pos.y - view_info->center.y) * view_info->scale,
  };
}

Pos2D pos_view_to_world(ViewInfo const *view_info, Pos2D pos) {
  return (Pos2D){
      .x = pos.x / view_info->scale + view_info->center.x,
      .y = pos.y / view_info->scale + view_info->center.y,
  };
}

Pos2D pos_view_to_screen(SDL_Renderer *renderer, Pos2D pos) {
  int sc_w, sc_h;
  SDL_GetRenderOutputSize(renderer, &sc_w, &sc_h);
  return (Pos2D){
      .x = sc_w * 0.5 + pos.x,
      .y = sc_h * 0.5 - pos.y,
  };
}

Pos2D pos_screen_to_view(SDL_Renderer *renderer, Pos2D pos) {
  int sc_w, sc_h;
  SDL_GetRenderOutputSize(renderer, &sc_w, &sc_h);
  return (Pos2D){
      .x = pos.x - sc_w * 0.5,
      .y = sc_h * 0.5 - pos.y,
  };
}

Pos2D pos_world_to_screen(SDL_Renderer *renderer, ViewInfo const *view_info,
                          Pos2D pos) {
  return pos_view_to_screen(renderer, pos_world_to_view(view_info, pos));
}

Pos2D pos_screen_to_world(SDL_Renderer *renderer, ViewInfo const *view_info,
                          Pos2D pos) {
  return pos_view_to_world(view_info, pos_screen_to_view(renderer, pos));
}
