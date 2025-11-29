#ifndef GEOM_STATE_H_
#define GEOM_STATE_H_

#include "defs.h"

typedef struct {
  PointDef *point_defs[100];
  LineDef *line_defs[100];
  CircleDef *circle_defs[100];
  int p_n;
  int l_n;
  int c_n;
} GeometryState;

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
} GeometryState2;

void clear_geometry_state(GeometryState *gs);

void register_point(GeometryState *gs, PointDef *pd);
void register_line(GeometryState *gs, LineDef *ld);
void register_circle(GeometryState *gs, CircleDef *cd);

PointDef *alloc_and_reg_point(GeometryState *gs, PointDef pd);
LineDef *alloc_and_reg_line(GeometryState *gs, LineDef ld);
CircleDef *alloc_and_reg_circle(GeometryState *gs, CircleDef cd);

void mark_everyting_dirty(GeometryState *gs);

void delete_point(GeometryState *gs, PointDef *pd);
void delete_line(GeometryState *gs, LineDef *ld);
void delete_circle(GeometryState *gs, CircleDef *cd);

#endif
