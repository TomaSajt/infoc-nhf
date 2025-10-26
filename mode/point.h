#pragma once

#include "../state.h"

SDL_AppResult point__on_mouse_down(AppState *as, Pos2D w_mouse_pos);

void point__on_render(AppState *as, Pos2D w_mouse_pos);
