#ifndef HOVER_H
#define HOVER_H

#include "state.h"

PointDef *get_hovered_point(AppState const *as, Pos2D const *w_mouse_pos);
LineDef *get_hovered_line(AppState const *as, Pos2D const *w_mouse_pos);
CircleDef *get_hovered_circle(AppState const *as, Pos2D const *w_mouse_pos);

PointDef *get_potential_point(AppState const *as, Pos2D const *w_mouse_pos,
                              PointDef *pot_out);

#endif
