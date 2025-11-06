#ifndef MODE_CIRCLE_BY_LEN_H
#define MODE_CIRCLE_BY_LEN_H

#include "../state.h"
#include "defs.h"

void circle_by_len__init_data(EditorStateData *data);

bool circle_by_len__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

bool circle_by_len__on_render(AppState *as, Pos2D const *w_mouse_pos);

#endif
