#ifndef MODE_DELETE_H
#define MODE_DELETE_H

#include "../state.h"

void enter_delete_mode(EditorState *es);

void delete__on_click(AppState *as, Pos2D w_mouse_pos);

#endif
