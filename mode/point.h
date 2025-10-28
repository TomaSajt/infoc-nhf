#pragma once

#include "../state.h"

void enter_point_mode(EditorState *es);

SDL_AppResult point__on_mouse_down(AppState *as, Pos2D w_mouse_pos);

void point__on_render(AppState *as, Pos2D w_mouse_pos);
