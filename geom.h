#pragma once

#include "geom_defs.h"

void eval_point(PointDef *pd);
void eval_line(LineDef *ld);
void eval_circle(CircleDef *cd);

double dist_from_pos(Pos2D *pos1, Pos2D *pos2);
double dist_from_line(Pos2D *pos, Pos2D *start, Pos2D *end);
double dist_from_circle(Pos2D *pos, Pos2D *center, double radius);
