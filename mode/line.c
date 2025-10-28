#include "line.h"

#include "../draw.h"
#include "../hover.h"

void enter_line_mode(EditorState *es) {
  es->mode = EM_LINE;
  es->elem_type = FE_NONE;
}

SDL_AppResult line__on_mouse_down(AppState *as, Pos2D w_mouse_pos) {
  PointDef *hovered = get_hovered_point(as, w_mouse_pos);
  if (hovered == NULL)
    return SDL_APP_CONTINUE;

  if (as->es.elem_type == FE_POINT) {
    LineDef *ld = alloc_and_reg_line(
        &as->gs, make_line_point_to_point(L_EXT_SEGMENT, as->es.p, hovered));
    if (ld == NULL)
      return SDL_APP_FAILURE;
    as->es.elem_type = FE_NONE;
  } else {
    as->es.elem_type = FE_POINT;
    as->es.p = hovered;
  }
  return SDL_APP_CONTINUE;
}

void line__on_render(AppState *as, Pos2D w_mouse_pos) {
  if (as->es.elem_type != FE_POINT)
    return;

  PointDef cursor_point = make_point_literal(w_mouse_pos);
  LineDef cursor_line =
      make_line_point_to_point(L_EXT_SEGMENT, as->es.p, &cursor_point);

  draw_line(as, &cursor_line, CYAN);
}
