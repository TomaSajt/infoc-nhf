#include "linelike.h"
#include "../draw.h"
#include "../hover.h"

void linelike__init_data(EditorStateData *data) { data->linelike.saved = NULL; }

bool linelike__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos,
                             LineExtMode ext_mode) {
  LinelikeModeData *data = &as->es.data.linelike;

  if (data->saved != NULL) {
    PointDef pot;
    PointDef *pd = get_potential_point(as, w_mouse_pos, &pot);
    if (pd == NULL)
      pd = alloc_and_reg_point(&as->gs, pot);
    if (pd == NULL)
      return false;

    LineDef *ld = alloc_and_reg_line(
        &as->gs, make_line_point_to_point(ext_mode, data->saved, pd));
    if (ld == NULL)
      return false;
    data->saved = NULL;
  } else {
    PointDef *hovered = get_hovered_point(as, w_mouse_pos);
    if (hovered == NULL)
      return true;
    data->saved = hovered;
  }
  return true;
}

bool linelike__on_render(AppState *as, Pos2D const *w_mouse_pos,
                         LineExtMode ext_mode) {
  LinelikeModeData *data = &as->es.data.linelike;

  if (data->saved == NULL)
    return true;

  PointDef pot;
  get_potential_point(as, w_mouse_pos, &pot);
  draw_point(as, &pot, CYAN);
  LineDef cursor_line = make_line_point_to_point(ext_mode, data->saved, &pot);

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
