#pragma once

#include "../state.h"

void enter_move_mode(EditorState *es);

void move__on_click(AppState *as, Pos2D w_mouse_pos);

void move__on_mouse_motion(AppState *as, Pos2D w_mouse_pos);

void move__on_click_release(AppState *as);
