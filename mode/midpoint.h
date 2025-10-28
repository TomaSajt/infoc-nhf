#pragma once

#include "../state.h"

void enter_midpoint_mode(EditorState *es);

SDL_AppResult midpoint__on_mouse_down(AppState *as, Pos2D w_mouse_pos);

void midpoint__on_render(AppState *as, Pos2D w_mouse_pos);
