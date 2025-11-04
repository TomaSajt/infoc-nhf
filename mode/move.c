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

void enter_move_mode(EditorState *es) {
  es->elem_type = FE_NONE;
  es->mode = EM_MOVE;
}

bool move__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos) {
  if (as->es.elem_type != FE_NONE) {
    printf("Got mousedown even though an element was already grabbed");
    return true;
  }

  // TODO: implement grabbing other elements
  PointDef *hovered_point = get_hovered_point(as, w_mouse_pos);
  if (hovered_point == NULL)
    return true;

  as->es.elem_type = FE_POINT;
  as->es.p = hovered_point;

  return true;
}

bool move__on_mouse_move(AppState *as, Pos2D const *w_mouse_pos) {
  if (as->es.elem_type == FE_NONE)
    return true;

  if (as->es.elem_type != FE_POINT) {
    // TODO: maybe implement
    printf("Moving is only implemented for points!\n");
    return true;
  }
  try_move_point_to_pos(as->es.p, w_mouse_pos);
  mark_everyting_dirty(&as->gs);

  return true;
}

bool move__on_mouse_up(AppState *as) {
  as->es.elem_type = FE_NONE;
  return true;
}
