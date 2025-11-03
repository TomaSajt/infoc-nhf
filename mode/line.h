#ifndef MODE_LINE_H
#define MODE_LINE_H

#include "../state.h"

void enter_line_mode(EditorState *es);

SDL_AppResult line__on_click(AppState *as, Pos2D w_mouse_pos);

void line__on_render(AppState *as, Pos2D w_mouse_pos);

#endif  // INCLUDE_MODE_LINE_H_
