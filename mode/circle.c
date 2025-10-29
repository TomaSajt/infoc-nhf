#include "circle.h"

#include "../draw.h"
#include "../hover.h"

void enter_circle_mode(EditorState *es) {
  es->mode = EM_CIRCLE;
  es->elem_type = FE_NONE;
}

SDL_AppResult circle__on_click(AppState *as, Pos2D w_mouse_pos) {
  PointDef *hovered = get_hovered_point(as, w_mouse_pos);
  if (hovered == NULL)
    return SDL_APP_CONTINUE;

  if (as->es.elem_type == FE_POINT) {
    CircleDef *cd = alloc_and_reg_circle(
        &as->gs, make_circle_center_point_outer_point(as->es.p, hovered));
    if (cd == NULL)
      return SDL_APP_FAILURE;
    as->es.elem_type = FE_NONE;
  } else {
    as->es.elem_type = FE_POINT;
    as->es.p = hovered;
  }
  return SDL_APP_CONTINUE;
}

void circle__on_render(AppState *as, Pos2D w_mouse_pos) {
  if (as->es.elem_type != FE_POINT)
    return;

  PointDef cursor_point = make_point_literal(w_mouse_pos);
  CircleDef cursor_circle =
      make_circle_center_point_outer_point(as->es.p, &cursor_point);

  draw_circle(as, &cursor_circle, CYAN);
}
