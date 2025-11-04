#ifndef MODE_CATEGORY_DEFS_H
#define MODE_CATEGORY_DEFS_H

#include <SDL3/SDL_keycode.h>

#include "../geom/defs.h"

// TODO: is there anything better to avoid circular imports?
typedef struct AppState AppState;
typedef struct NewEditorState NewEditorState;

typedef union {
  struct {
    PointDef *grabbed;
  } move;
  struct {
    PointDef *saved;
  } midpoint;
  struct {
    PointDef *saved;
  } line;
  struct {
    LineDef *saved;
  } par_per;
  struct {
    PointDef *saved;
  } circle;
  struct {
    LineDef *saved;
  } circle_by_len;
} EditorStateData;

typedef struct {
  char const *name;
  SDL_Keycode keycode;
  void (*init_data)(EditorStateData *data);
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

extern EditorInfo const editor_info;

typedef struct {
  CategoryInfo const *cat_info;
  int sel_mode_ind;
} CategoryState;

typedef struct NewEditorState {
  CategoryState category_states[4]; // TODO: don't hardcode, somehow
  int sel_cat_ind;
  int num_cats;
  ModeInfo const *mode_info;
  EditorStateData data;
} NewEditorState;

#endif
