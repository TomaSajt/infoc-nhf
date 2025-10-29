#pragma once

#include "../state.h"

void enter_line_mode(EditorState *es);

SDL_AppResult line__on_click(AppState *as, Pos2D w_mouse_pos);

void line__on_render(AppState *as, Pos2D w_mouse_pos);
