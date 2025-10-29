#include "point.h"

#include "../draw.h"
#include "../hover.h"

void enter_point_mode(EditorState *es) { es->mode = EM_POINT; }

SDL_AppResult point__on_click(AppState *as, Pos2D w_mouse_pos) {
  PointDef pot;
  PointDef *hov = get_hovered_or_make_potential_point(as, w_mouse_pos, &pot);
  if (hov != NULL)
    return SDL_APP_CONTINUE;

  PointDef *pd = alloc_and_reg_point(&as->gs, pot);
  if (pd == NULL)
    return SDL_APP_FAILURE;
  return SDL_APP_CONTINUE;
}

void point__on_render(AppState *as, Pos2D w_mouse_pos) {
  PointDef pot;
  PointDef *hov = get_hovered_or_make_potential_point(as, w_mouse_pos, &pot);
  if (hov != NULL)
    return;

  draw_point(as, &pot, CYAN);
}
