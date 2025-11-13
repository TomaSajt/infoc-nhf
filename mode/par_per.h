#ifndef MODE_PAR_PER_H_
#define MODE_PAR_PER_H_

#include "../state.h"

void par_per__init_data(EditorStateData *data);

bool parallel__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);
bool perpendicular__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

bool parallel__on_render(AppState *as, Pos2D const *w_mouse_pos);
bool perpendicular__on_render(AppState *as, Pos2D const *w_mouse_pos);

#endif
