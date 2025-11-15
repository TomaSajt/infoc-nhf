#include "move.h"
#include "../geom/util.h"
#include "../hover.h"

#include <stdio.h>

void try_move_point_to_pos(PointDef *pd, Pos2D const *pos) {
  switch (pd->type) {
  case PD_LITERAL: {
    pd->literal.pos = *pos;
    break;
  }
  case PD_GLIDER_ON_LINE: {
    LineExtMode ext_mode = pd->glider_on_line.l->ext_mode;
    Pos2D *s = &pd->glider_on_line.l->val.start;
    Pos2D *e = &pd->glider_on_line.l->val.end;
    double prog = line_closest_prog_from_pos(pos, s, e, ext_mode);
    pd->glider_on_line.prog = prog;
    break;
  }
  case PD_GLIDER_ON_CIRCLE: {
    Pos2D *c = &pd->glider_on_circle.c->val.center;
    double prog = circle_closest_prog_from_pos(pos, c);
    pd->glider_on_circle.prog = prog;
    break;
  }
  default:
    printf("Can't move non-literal, non-glider points!\n");
    break;
  }
}

void move__init_data(EditorStateData *data) { data->move.grabbed = NULL; }

bool move__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  MoveModeData *data = &as->es.data.move;

  if (data->grabbed != NULL) {
    printf("Got mousedown even though an element was already grabbed");
    return true;
  }

  PointDef *hovered_point = get_hovered_point(as, w_mouse_pos);
  if (hovered_point == NULL)
    return true;

  data->grabbed = hovered_point;

  return true;
}

bool move__on_mouse_move(AppState *as, Pos2D const *w_mouse_pos) {

  MoveModeData *data = &as->es.data.move;

  if (data->grabbed == NULL)
    return true;

  try_move_point_to_pos(data->grabbed, w_mouse_pos);
  mark_everyting_dirty(&as->gs);

  return true;
}

bool move__on_mouse_up(AppState *as) {
  MoveModeData *data = &as->es.data.move;
  data->grabbed = NULL;
  return true;
}
