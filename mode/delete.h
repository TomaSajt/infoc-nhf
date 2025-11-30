#ifndef MODE_DELETE_H_
#define MODE_DELETE_H_

#include "../state.h"

bool delete__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

bool delete__on_render(AppState *as, Pos2D const *w_mouse_pos);

#endif
