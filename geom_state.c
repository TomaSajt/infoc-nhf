#include "debugmalloc.h"

#include <stdlib.h>

#include "geom_state.h"

// free() all elements in the geometry state and set counts back to zero
void clear_geometry_state(GeometryState *gs) {
  for (int i = 0; i < gs->p_n; i++) {
    free(gs->point_defs[i]);
  }
  for (int i = 0; i < gs->l_n; i++) {
    free(gs->line_defs[i]);
  }
  for (int i = 0; i < gs->c_n; i++) {
    free(gs->circle_defs[i]);
  }
  gs->p_n = 0;
  gs->l_n = 0;
  gs->c_n = 0;
}

void register_point(GeometryState *gs, PointDef *pd) {
  gs->point_defs[gs->p_n++] = pd;
}

void register_line(GeometryState *gs, LineDef *ld) {
  gs->line_defs[gs->l_n++] = ld;
}

void register_circle(GeometryState *gs, CircleDef *cd) {
  gs->circle_defs[gs->c_n++] = cd;
}

void mark_everyting_dirty(GeometryState *gs) {
  for (int i = 0; i < gs->p_n; i++) {
    gs->point_defs[i]->val.dirty = true;
  }
  for (int i = 0; i < gs->l_n; i++) {
    gs->line_defs[i]->val.dirty = true;
  }
  for (int i = 0; i < gs->c_n; i++) {
    gs->circle_defs[i]->val.dirty = true;
  }
}
