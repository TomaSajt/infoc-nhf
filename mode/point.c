#include "point.h"
#include "../draw.h"
#include "../hover.h"

bool point__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  PointDef pot;
  PointDef *hov = get_hovered_or_make_potential_point(as, w_mouse_pos, &pot);
  if (hov != NULL)
    return true;

  PointDef *pd = alloc_and_reg_point(&as->gs, pot);
  if (pd == NULL)
    return false;

  return true;
}

bool point__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  PointDef pot;
  PointDef *hov = get_hovered_or_make_potential_point(as, w_mouse_pos, &pot);
  if (hov != NULL)
    return true;

  draw_point(as, &pot, CYAN);

  return true;
}
