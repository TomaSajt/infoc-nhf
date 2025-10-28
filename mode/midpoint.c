#include "midpoint.h"

#include "../draw.h"
#include "../hover.h"

void enter_midpoint_mode(EditorState *es) {
  es->mode = EM_MIDPOINT;
  es->elem_type = FE_NONE;
}

LineDef *get_hovered_seg(AppState *as, Pos2D w_mouse_pos) {
  LineDef *hovered_line = get_hovered_line(as, w_mouse_pos);
  if (hovered_line == NULL)
    return NULL;

  if (hovered_line->ext_mode != L_EXT_SEGMENT)
    return NULL;

  return hovered_line;
}

SDL_AppResult midpoint__on_mouse_down(AppState *as, Pos2D w_mouse_pos) {
  PointDef *hovered_p = get_hovered_point(as, w_mouse_pos);
  if (hovered_p != NULL) {
    if (as->es.elem_type == FE_POINT) {
      PointDef *pd = alloc_and_reg_point(
          &as->gs, make_point_midpoint(as->es.p, hovered_p));
      if (pd == NULL)
        return SDL_APP_FAILURE;
      as->es.elem_type = FE_NONE;
    } else {
      as->es.elem_type = FE_POINT;
      as->es.p = hovered_p;
    }
  } else {
    if (as->es.elem_type == FE_NONE) {
      LineDef *hovered_seg = get_hovered_seg(as, w_mouse_pos);
      if (hovered_seg != NULL) {
        PointDef *pd = alloc_and_reg_point(
            &as->gs, make_point_midpoint(as->es.l->point_to_point.p1,
                                         as->es.l->point_to_point.p2));
        if (pd == NULL)
          return SDL_APP_FAILURE;
      }
    }
  }
  return SDL_APP_CONTINUE;
}

void midpoint__on_render(AppState *as, Pos2D w_mouse_pos) {
  if (as->es.elem_type == FE_POINT) {
    PointDef cursor_point = make_point_literal(w_mouse_pos);
    PointDef midpoint = make_point_midpoint(as->es.p, &cursor_point);
    draw_point(as, &midpoint, CYAN);
  }
}
