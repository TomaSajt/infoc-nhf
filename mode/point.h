#ifndef MODE_POINT_H
#define MODE_POINT_H

#include "../state.h"

void enter_point_mode(EditorState *es);

bool point__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

bool point__on_render(AppState *as, Pos2D const *w_mouse_pos);

#endif
