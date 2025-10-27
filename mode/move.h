#pragma once

#include "../state.h"

void move__on_mouse_down(AppState *as, Pos2D w_mouse_pos);

void move__on_mouse_motion(AppState *as, Pos2D w_mouse_pos);

void move__on_mouse_up(AppState *as);
