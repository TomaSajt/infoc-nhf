#ifndef MODE_POINT_H
#define MODE_POINT_H

#include "../state.h"

void enter_point_mode(EditorState *es);

SDL_AppResult point__on_click(AppState *as, Pos2D w_mouse_pos);

void point__on_render(AppState *as, Pos2D w_mouse_pos);

#endif
