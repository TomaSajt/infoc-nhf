#include "point.h"
#include "../draw.h"
#include "../hover.h"

bool point__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  PointDef *pd = maybe_alloc_reg_potential_point(as, w_mouse_pos);
  if (pd == NULL)
    return false;

  return true;
}

bool point__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  PointDef pot;
  PointDef *pd = get_potential_point(as, w_mouse_pos, &pot);

  if (pd == NULL) {
    pot.color = CYAN;
    draw_point(as, &pot);
  } else {
    pd->color = CYAN;
  }

  return true;
}
