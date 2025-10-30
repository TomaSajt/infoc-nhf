#pragma once

#include "defs.h"

void eval_point(PointDef *pd);
void eval_line(LineDef *ld);
void eval_circle(CircleDef *cd);

bool eval_point_del_flag(PointDef *pd);
bool eval_line_del_flag(LineDef *ld);
bool eval_circle_del_flag(CircleDef *cd);

double clamp_line_prog(double prog, LineExtMode ext_mode);

double line_closest_prog_from_pos(Pos2D const *pos, Pos2D const *s,
                                  Pos2D const *e, LineExtMode ext_mode);
double circle_closest_prog_from_pos(Pos2D const *pos, Pos2D const *c);

double dist_from_pos(Pos2D const *pos1, Pos2D const *pos2);
double dist_from_line(Pos2D const *pos, Pos2D const *start, Pos2D const *end,
                      LineExtMode ext_mode);
double dist_from_circle(Pos2D const *pos, Pos2D const *center, double radius);
