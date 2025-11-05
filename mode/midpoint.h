#ifndef MODE_MIDPOINT_H
#define MODE_MIDPOINT_H

#include "../state.h"

void midpoint__init_data(EditorStateData *data);

bool midpoint__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

bool midpoint__on_render(AppState *as, Pos2D const *w_mouse_pos);

#endif
