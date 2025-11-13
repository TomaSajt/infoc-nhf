#ifndef MODE_MOVE_H_
#define MODE_MOVE_H_

#include "../state.h"
#include "defs.h"

void move__init_data(EditorStateData *data);

bool move__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

bool move__on_mouse_move(AppState *as, Pos2D const *w_mouse_pos);

bool move__on_mouse_up(AppState *as);

#endif
