#include "midpoint.h"

#include "../draw.h"
#include "../hover.h"

void midpoint__init_data(EditorStateData *data) { data->midpoint.saved = NULL; }

bool midpoint__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  MidpointModeData *data = &as->es.data.midpoint;

  // TODO: use potential point
  PointDef *hovered_p = get_hovered_point(as, w_mouse_pos);
  if (hovered_p == NULL)
    return true;

  if (data->saved == NULL) {
    data->saved = hovered_p;
    return true;
  }

  PointDef *pd =
      alloc_and_reg_point(&as->gs, make_point_midpoint(data->saved, hovered_p));
  if (pd == NULL)
    return false;

  data->saved = NULL;

  return true;
}

bool midpoint__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  MidpointModeData *data = &as->es.data.midpoint;

  if (data->saved == NULL)
    return true;

  // TODO: use potential point
  PointDef cursor_point = make_point_literal(*w_mouse_pos);
  PointDef midpoint = make_point_midpoint(data->saved, &cursor_point);
  draw_point(as, &midpoint, CYAN);

  return true;
}
