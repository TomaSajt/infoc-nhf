#include "delete.h"

#include "../hover.h"

void enter_delete_mode(EditorState *es) { es->mode = EM_DELETE; }

void delete__on_mouse_down(AppState *as, Pos2D w_mouse_pos) {
  {
    PointDef *hovered_point = get_hovered_point(as, w_mouse_pos);
    if (hovered_point != NULL) {
      delete_point(&as->gs, hovered_point);
      return;
    }
  }
  {
    LineDef *hovered_line = get_hovered_line(as, w_mouse_pos);
    if (hovered_line != NULL) {
      delete_line(&as->gs, hovered_line);
      return;
    }
  }
  {
    CircleDef *hovered_circle = get_hovered_circle(as, w_mouse_pos);
    if (hovered_circle != NULL) {
      delete_circle(&as->gs, hovered_circle);
      return;
    }
  }
}
