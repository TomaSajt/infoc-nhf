#ifndef MODE_CIRCLE_H
#define MODE_CIRCLE_H

#include "../state.h"

void enter_circle_mode(EditorState *es);

SDL_AppResult circle__on_click(AppState *as, Pos2D w_mouse_pos);

void circle__on_render(AppState *as, Pos2D w_mouse_pos);

#endif
