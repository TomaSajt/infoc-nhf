#ifndef MODE_DELETE_H
#define MODE_DELETE_H

#include "../state.h"

void enter_delete_mode(EditorState *es);

bool delete__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

#endif
