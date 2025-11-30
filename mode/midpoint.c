#include "midpoint.h"
#include "../draw.h"
#include "../hover.h"

void midpoint__init_data(EditorStateData *data) { data->midpoint.saved = NULL; }

bool midpoint__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  MidpointModeData *data = &as->es.data.midpoint;

  PointDef *pd = maybe_alloc_reg_potential_point(as, w_mouse_pos);
  if (pd == NULL)
    return false;

  if (data->saved == NULL) {
    data->saved = pd;
    return true;
  }

  PointDef *mid_pd =
      alloc_and_reg_point(&as->gs, make_point_midpoint(data->saved, pd));
  if (mid_pd == NULL)
    return false;

  data->saved = NULL;
  return true;
}

bool midpoint__on_render(AppState *as, Pos2D const *w_mouse_pos) {
  MidpointModeData *data = &as->es.data.midpoint;

  PointDef pot;
  PointDef *pd = get_potential_point(as, w_mouse_pos, &pot);

  if (pd == NULL) {
    pot.color = CYAN;
    draw_point(as, &pot);
  } else {
    pd->color = CYAN;
  }

  if (data->saved != NULL) {
    data->saved->color = GREEN;

    PointDef pot_midpoint = make_point_midpoint(data->saved, &pot);
    pot_midpoint.color = CYAN;
    draw_point(as, &pot_midpoint);
  }

  return true;
}
