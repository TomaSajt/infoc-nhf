#ifndef MODE_CIRCLE_H
#define MODE_CIRCLE_H

#include "../state.h"

void enter_circle_mode(EditorState *es);

bool circle__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

bool circle__on_render(AppState *as, Pos2D const *w_mouse_pos);

#endif
