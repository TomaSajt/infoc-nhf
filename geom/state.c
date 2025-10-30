#include "state.h"

#include "util.h"
#include "defs.h"

#include <stdlib.h>

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

PointDef *alloc_and_reg_point(GeometryState *gs, PointDef pd) {
  PointDef *alloc_pd = malloc(sizeof(PointDef));
  if (alloc_pd == NULL)
    return NULL;
  *alloc_pd = pd;
  register_point(gs, alloc_pd);
  return alloc_pd;
}

LineDef *alloc_and_reg_line(GeometryState *gs, LineDef ld) {
  LineDef *alloc_ld = malloc(sizeof(LineDef));
  if (alloc_ld == NULL)
    return NULL;
  *alloc_ld = ld;
  register_line(gs, alloc_ld);
  return alloc_ld;
}

CircleDef *alloc_and_reg_circle(GeometryState *gs, CircleDef cd) {
  CircleDef *alloc_cd = malloc(sizeof(CircleDef));
  if (alloc_cd == NULL)
    return NULL;
  *alloc_cd = cd;
  register_circle(gs, alloc_cd);
  return alloc_cd;
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

void delete_marked_cascading(GeometryState *gs) {
  for (int i = 0; i < gs->p_n; i++) {
    eval_point_del_flag(gs->point_defs[i]);
  }
  for (int i = 0; i < gs->l_n; i++) {
    eval_line_del_flag(gs->line_defs[i]);
  }
  for (int i = 0; i < gs->c_n; i++) {
    eval_circle_del_flag(gs->circle_defs[i]);
  }

  // TODO: implement in-place solution
  GeometryState new_gs = {
      .point_defs = {0},
      .line_defs = {0},
      .circle_defs = {0},
      .p_n = 0,
      .l_n = 0,
      .c_n = 0,
  };

  for (int i = 0; i < gs->p_n; i++) {
    PointDef *pd = gs->point_defs[i];
    if (pd->del_flag == DF_YES) {
      free(pd);
    } else {
      register_point(&new_gs, pd);
      pd->del_flag = DF_DONT_KNOW;
    }
  }
  for (int i = 0; i < gs->l_n; i++) {
    LineDef *ld = gs->line_defs[i];
    if (ld->del_flag == DF_YES) {
      free(ld);
    } else {
      register_line(&new_gs, ld);
      ld->del_flag = DF_DONT_KNOW;
    }
  }
  for (int i = 0; i < gs->c_n; i++) {
    CircleDef *cd = gs->circle_defs[i];
    if (cd->del_flag == DF_YES) {
      free(cd);
    } else {
      register_circle(&new_gs, cd);
      cd->del_flag = DF_DONT_KNOW;
    }
  }

  *gs = new_gs;
}

void delete_point(GeometryState *gs, PointDef *pd) {
  pd->del_flag = DF_YES;
  delete_marked_cascading(gs);
}

void delete_line(GeometryState *gs, LineDef *ld) {
  ld->del_flag = DF_YES;
  delete_marked_cascading(gs);
}

void delete_circle(GeometryState *gs, CircleDef *cd) {
  cd->del_flag = DF_YES;
  delete_marked_cascading(gs);
}
