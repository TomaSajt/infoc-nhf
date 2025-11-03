#ifndef MODE_MOVE_H
#define MODE_MOVE_H

#include "../state.h"

void enter_move_mode(EditorState *es);

bool move__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

bool move__on_mouse_move(AppState *as, Pos2D const *w_mouse_pos);

bool move__on_mouse_up(AppState *as);

#endif
