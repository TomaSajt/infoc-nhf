#include "delete.h"
#include "../hover.h"

bool delete__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  PointDef *hovered_point = get_hovered_point(as, w_mouse_pos);
  if (hovered_point != NULL) {
    hovered_point->del_flag = DF_YES;
    delete_marked_cascading(&as->gs);
  }

  LineDef *hovered_line = get_hovered_line(as, w_mouse_pos);
  if (hovered_line != NULL) {
    hovered_line->del_flag = DF_YES;
    delete_marked_cascading(&as->gs);
  }

  CircleDef *hovered_circle = get_hovered_circle(as, w_mouse_pos);
  if (hovered_circle != NULL) {
    hovered_circle->del_flag = DF_YES;
    delete_marked_cascading(&as->gs);
    return true;
  }

  return true;
}

bool delete__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  PointDef *hovered_point = get_hovered_point(as, w_mouse_pos);
  if (hovered_point != NULL) {
    hovered_point->del_flag = DF_YES;
    color_df_cascading_and_reset(&as->gs);
    return true;
  }

  LineDef *hovered_line = get_hovered_line(as, w_mouse_pos);
  if (hovered_line != NULL) {
    hovered_line->del_flag = DF_YES;
    color_df_cascading_and_reset(&as->gs);
    return true;
  }

  CircleDef *hovered_circle = get_hovered_circle(as, w_mouse_pos);
  if (hovered_circle != NULL) {
    hovered_circle->del_flag = DF_YES;
    color_df_cascading_and_reset(&as->gs);
    return true;
  }

  return true;
}
