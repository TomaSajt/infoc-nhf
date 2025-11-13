#ifndef MODE_CIRCLE_H_
#define MODE_CIRCLE_H_

#include "../state.h"
#include "defs.h"

void circle__init_data(EditorStateData *data);

bool circle__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

bool circle__on_render(AppState *as, Pos2D const *w_mouse_pos);

#endif
