#include "point.h"
#include "../draw.h"
#include "../hover.h"

bool point__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  PointDef pot;
  PointDef *pd = get_potential_point(as, w_mouse_pos, &pot);

  // don't do anything if the potential point already exists
  if (pd != NULL)
    return true;

  pd = alloc_and_reg_point(&as->gs, pot);
  if (pd == NULL)
    return false;

  return true;
}

bool point__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  PointDef pot;
  PointDef *pd = get_potential_point(as, w_mouse_pos, &pot);

  // don't render point again if it already exists
  if (pd != NULL)
    return true;

  draw_point(as, &pot, CYAN);

  return true;
}
