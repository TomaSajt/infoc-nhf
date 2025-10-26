#pragma once

#include "geom_defs.h"

void eval_point(PointDef *pd);
void eval_line(LineDef *ld);
void eval_circle(CircleDef *cd);

bool eval_point_del_flag(PointDef *pd);
bool eval_line_del_flag(LineDef *ld);
bool eval_circle_del_flag(CircleDef *cd);

double clamp_line_prog(double prog, LineExtMode ext_mode);

double pos_to_closest_line_prog(Pos2D *pos, Pos2D *s, Pos2D *e);
double pos_to_closest_circle_prog(Pos2D *pos, Pos2D *c);

double dist_from_pos(Pos2D *pos1, Pos2D *pos2);
double dist_from_line(Pos2D *pos, Pos2D *start, Pos2D *end);
double dist_from_circle(Pos2D *pos, Pos2D *center, double radius);
