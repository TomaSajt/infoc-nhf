#include "circle.h"
#include "../draw.h"
#include "../hover.h"

void circle__init_data(EditorStateData *data) { data->circle.saved = NULL; }

bool circle__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  CircleModeData *data = &as->es.data.circle;

  PointDef *hovered = get_hovered_point(as, w_mouse_pos);
  if (hovered == NULL)
    return true;

  if (as->es.data.circle.saved != NULL) {
    CircleDef *cd = alloc_and_reg_circle(
        &as->gs, make_circle_center_point_outer_point(data->saved, hovered));
    if (cd == NULL)
      return false;
    data->saved = NULL;
  } else {
    data->saved = hovered;
  }

  return true;
}

bool circle__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  CircleModeData *data = &as->es.data.circle;

  if (data->saved == NULL)
    return true;

  PointDef cursor_point = make_point_literal(*w_mouse_pos);
  CircleDef cursor_circle =
      make_circle_center_point_outer_point(data->saved, &cursor_point);

  draw_circle(as, &cursor_circle, CYAN);

  return true;
}
