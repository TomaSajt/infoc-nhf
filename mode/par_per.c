#include "par_per.h"
#include "../draw.h"
#include "../hover.h"

void par_per__init_data(EditorStateData *data) { data->par_per.saved = NULL; }

bool par_per__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos,
                            bool is_par) {
  ParPerModeData *data = &as->es.data.par_per;

  if (data->saved == NULL) {
    LineDef *ld = get_hovered_line(as, w_mouse_pos);
    if (ld == NULL)
      return true;

    data->saved = ld;
    return true;
  }

  PointDef *pd = maybe_alloc_reg_potential_point(as, w_mouse_pos);
  if (pd == NULL)
    return false;

  LineDef *ld = alloc_and_reg_line(
      &as->gs, is_par ? make_line_parallel(data->saved, pd)
                      : make_line_perpendicular(data->saved, pd));
  if (ld == NULL)
    return false;

  data->saved = NULL;
  return true;
}

bool par_per__on_render(AppState *as, Pos2D const *w_mouse_pos, bool is_par) {
  ParPerModeData *data = &as->es.data.par_per;

  if (data->saved == NULL)
    return true;

  PointDef pot;
  PointDef *pd = get_potential_point(as, w_mouse_pos, &pot);

  LineDef pot_line = is_par ? make_line_parallel(data->saved, &pot)
                            : make_line_perpendicular(data->saved, &pot);
  draw_line(as, &pot_line, CYAN);

  // don't redraw already existing point
  if (pd == NULL) {
    draw_point(as, &pot, CYAN);
  }

  return true;
}

bool parallel__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  return par_per__on_mouse_down(as, w_mouse_pos, true);
}

bool perpendicular__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  return par_per__on_mouse_down(as, w_mouse_pos, false);
}

bool parallel__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  return par_per__on_render(as, w_mouse_pos, true);
}

bool perpendicular__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  return par_per__on_render(as, w_mouse_pos, false);
}
