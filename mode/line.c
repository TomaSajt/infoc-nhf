#include "line.h"

#include "../draw.h"
#include "../hover.h"

void enter_line_mode(EditorState *es) {
  es->mode = EM_LINE;
  es->elem_type = FE_NONE;
}

void linelike__init_data(EditorStateData *data) { data->line.saved = NULL; }

bool linelike__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos,
                             LineExtMode ext_mode) {
  if (as->es.elem_type == FE_POINT) {
    PointDef pot;
    PointDef *hov = get_hovered_or_make_potential_point(as, w_mouse_pos, &pot);
    PointDef *actual = hov == NULL ? alloc_and_reg_point(&as->gs, pot) : hov;
    if (actual == NULL)
      return false;
    LineDef *ld = alloc_and_reg_line(
        &as->gs, make_line_point_to_point(ext_mode, as->es.p, actual));
    if (ld == NULL)
      return false;
    as->es.elem_type = FE_NONE;
  } else {
    PointDef *hovered = get_hovered_point(as, w_mouse_pos);
    if (hovered == NULL)
      return true;
    as->es.elem_type = FE_POINT;
    as->es.p = hovered;
  }
  return true;
}

bool linelike__on_render(AppState *as, Pos2D const *w_mouse_pos,
                         LineExtMode ext_mode) {
  if (as->es.elem_type != FE_POINT)
    return true;

  PointDef pot;
  PointDef *hov = get_hovered_or_make_potential_point(as, w_mouse_pos, &pot);
  PointDef *to_draw = hov == NULL ? &pot : hov;
  draw_point(as, to_draw, CYAN);
  LineDef cursor_line = make_line_point_to_point(ext_mode, as->es.p, to_draw);

  draw_line(as, &cursor_line, CYAN);

  return true;
}

bool segment__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  return linelike__on_mouse_down(as, w_mouse_pos, L_EXT_LINE);
}

bool line__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  return linelike__on_mouse_down(as, w_mouse_pos, L_EXT_SEGMENT);
}

bool ray__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  return linelike__on_mouse_down(as, w_mouse_pos, L_EXT_RAY);
}

bool segment__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  return linelike__on_render(as, w_mouse_pos, L_EXT_SEGMENT);
}

bool line__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  return linelike__on_render(as, w_mouse_pos, L_EXT_LINE);
}

bool ray__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  return linelike__on_render(as, w_mouse_pos, L_EXT_RAY);
}
