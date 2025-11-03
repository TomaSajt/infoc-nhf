#ifndef MODE_CATEGORY_DEFS_H
#define MODE_CATEGORY_DEFS_H

#include "../state.h"

#include <SDL3/SDL_keycode.h>

typedef struct {
  EditorMode mode;
  char const *name;
  SDL_Keycode keycode;
  bool (*on_mouse_down)(AppState *as, Pos2D const *w_mouse_pos);
  bool (*on_mouse_move)(AppState *as, Pos2D const *w_mouse_pos);
  bool (*on_mouse_up)(AppState *as);
  bool (*on_render)(AppState *as, Pos2D const *w_mouse_pos);
} ModeInfo;

typedef struct {
  char const *name;
  SDL_Keycode keycode;
  ModeInfo const *modes;
  int num_modes;
} CategoryInfo;

typedef struct {
  CategoryInfo const *cats;
  int num_cats;
} EditorInfo;

typedef struct {
  CategoryInfo *cat_info;
  int sel_mode_ind;
} CategoryState;

typedef struct {
  // TODO: maybe allow larger array
  CategoryState category_states[4];
  int sel_cat_ind;

} NewEditorState;

extern EditorInfo const editor_info;

#endif
