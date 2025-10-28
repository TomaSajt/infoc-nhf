#pragma once

#include "state.h"

PointDef *get_hovered_point(AppState *as, Pos2D w_mouse_pos);
LineDef *get_hovered_line(AppState *as, Pos2D w_mouse_pos);
CircleDef *get_hovered_circle(AppState *as, Pos2D w_mouse_pos);

PointDef *get_hovered_or_make_potential_point(AppState *as, Pos2D w_mouse_pos,
                                              PointDef *pot_out);
