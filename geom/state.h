#ifndef GEOM_STATE_H_
#define GEOM_STATE_H_

#include "defs.h"

typedef struct GenericElemList GenericElemList;
struct GenericElemList {
  union {
    PointDef *pd;
    LineDef *ld;
    CircleDef *cd;
  };
  GenericElemList *next;
};

typedef struct {
  GenericElemList *pd_list;
  GenericElemList *ld_list;
  GenericElemList *cd_list;
} GeometryState;

void clear_geometry_state(GeometryState *gs);

bool register_point(GeometryState *gs, PointDef *pd);
bool register_line(GeometryState *gs, LineDef *ld);
bool register_circle(GeometryState *gs, CircleDef *cd);

PointDef *alloc_and_reg_point(GeometryState *gs, PointDef pd);
LineDef *alloc_and_reg_line(GeometryState *gs, LineDef ld);
CircleDef *alloc_and_reg_circle(GeometryState *gs, CircleDef cd);

void mark_everyting_dirty(GeometryState *gs);

void delete_marked_cascading(GeometryState *gs);
void color_df_cascading_and_reset(GeometryState *gs);

#endif
