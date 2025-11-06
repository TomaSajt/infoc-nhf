#include "circle_by_len.h"
#include "../draw.h"
#include "../hover.h"

void circle_by_len__init_data(EditorStateData *data) {
  data->circle_by_len.saved = NULL;
}

bool circle_by_len__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  CircleByLenModeData *data = &as->es.data.circle_by_len;

  if (data->saved == NULL) {
    LineDef *ld = get_hovered_line(as, w_mouse_pos);
    if (ld == NULL)
      return true;
    if (ld->ext_mode != L_EXT_SEGMENT)
      return true;

    data->saved = ld;
    return true;
  }

  PointDef *pd = maybe_alloc_reg_potential_point(as, w_mouse_pos);
  if (pd == NULL)
    return false;

  CircleDef *cd = alloc_and_reg_circle(
      &as->gs, make_circle_center_point_radius_seg(pd, data->saved));
  if (cd == NULL)
    return false;

  data->saved = NULL;
  return true;
}

bool circle_by_len__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  CircleByLenModeData *data = &as->es.data.circle_by_len;

  if (data->saved == NULL)
    return true;

  PointDef pot;
  PointDef *pd = get_potential_point(as, w_mouse_pos, &pot);

  CircleDef pot_circle = make_circle_center_point_radius_seg(&pot, data->saved);
  draw_circle(as, &pot_circle, CYAN);

  // don't redraw already existing point
  if (pd == NULL) {
    draw_point(as, &pot, CYAN);
  }

  return true;
}
