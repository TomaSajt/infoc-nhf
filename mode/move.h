#pragma once

#include "../state.h"

void enter_move_mode(EditorState *es);

void move__on_mouse_down(AppState *as, Pos2D w_mouse_pos);

void move__on_mouse_motion(AppState *as, Pos2D w_mouse_pos);

void move__on_mouse_up(AppState *as);
