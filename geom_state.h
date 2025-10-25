#pragma once

#include "geom_defs.h"

typedef struct {
  PointDef *point_defs[100];
  LineDef *line_defs[100];
  CircleDef *circle_defs[100];
  int p_n;
  int l_n;
  int c_n;
} GeometryState;


void clear_geometry_state(GeometryState *gs);

void register_point(GeometryState *gs, PointDef *pd);
void register_line(GeometryState *gs, LineDef *ld);
void register_circle(GeometryState *gs, CircleDef *cd);

void mark_everyting_dirty(GeometryState *gs);

