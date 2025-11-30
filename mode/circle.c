#include "circle.h"
#include "../draw.h"
#include "../hover.h"

void circle__init_data(EditorStateData *data) { data->circle.saved = NULL; }

bool circle__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  CircleModeData *data = &as->es.data.circle;

  PointDef *pd = maybe_alloc_reg_potential_point(as, w_mouse_pos);
  if (pd == NULL)
    return false;

  if (data->saved == NULL) {
    data->saved = pd;
    return true;
  }

  CircleDef *cd = alloc_and_reg_circle(
      &as->gs, make_circle_center_point_outer_point(data->saved, pd));
  if (cd == NULL)
    return false;

  data->saved = NULL;
  return true;
}

bool circle__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  CircleModeData *data = &as->es.data.circle;

  PointDef pot;
  PointDef *pd = get_potential_point(as, w_mouse_pos, &pot);

  if (data->saved != NULL) {
    CircleDef pot_circle =
        make_circle_center_point_outer_point(data->saved, &pot);
    pot_circle.color = CYAN;
    draw_circle(as, &pot_circle);
  }

  if (pd == NULL) {
    pot.color = CYAN;
    draw_point(as, &pot);
  } else {
    pd->color = CYAN;
  }

  return true;
}
