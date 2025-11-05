#ifndef MODE_LINE_H
#define MODE_LINE_H

#include "../state.h"

void linelike__init_data(EditorStateData *data);

bool segment__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);
bool line__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);
bool ray__on_mouse_down(AppState *as, Pos2D const *w_mouse_pos);

bool segment__on_render(AppState *as, Pos2D const *w_mouse_pos);
bool line__on_render(AppState *as, Pos2D const *w_mouse_pos);
bool ray__on_render(AppState *as, Pos2D const *w_mouse_pos);

#endif
