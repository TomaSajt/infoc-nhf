#ifndef MODE_MIDPOINT_H
#define MODE_MIDPOINT_H

#include "../state.h"

void enter_midpoint_mode(EditorState *es);

SDL_AppResult midpoint__on_click(AppState *as, Pos2D w_mouse_pos);

void midpoint__on_render(AppState *as, Pos2D w_mouse_pos);

#endif
