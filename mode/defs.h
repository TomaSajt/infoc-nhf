#ifndef MODE_CATEGORY_DEFS_H_
#define MODE_CATEGORY_DEFS_H_

#include "../geom/defs.h"

#include <SDL3/SDL_keycode.h>

typedef struct MoveModeData {
  PointDef *grabbed;
} MoveModeData;

typedef struct MidpointModeData {
  PointDef *saved;
} MidpointModeData;

typedef struct LinelikeModeData {
  PointDef *saved;
} LinelikeModeData;

typedef struct ParPerModeData {
  LineDef *saved;
} ParPerModeData;

typedef struct CircleModeData {
  PointDef *saved;
} CircleModeData;

typedef struct CircleByLenModeData {
  LineDef *saved;
} CircleByLenModeData;

typedef union {
  MoveModeData move;
  MidpointModeData midpoint;
  LinelikeModeData linelike;
  ParPerModeData par_per;
  CircleModeData circle;
  CircleByLenModeData circle_by_len;
} EditorStateData;

// We can't import the definition of Appstate, since it uses EditorState in
// itself instead, we use opaque struct declaration

typedef struct AppState AppState;

typedef struct {
  char const *name;
  SDL_Keycode keycode;
  void (*init_data)(EditorStateData *data);
  bool (*on_mouse_down)(AppState *as, Pos2D const *w_mouse_pos);
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

typedef struct EditorState {
  CategoryState category_states[4]; // TODO: don't hardcode, somehow
  int sel_cat_ind;
  int num_cats;
  ModeInfo const *mode_info;
  EditorStateData data;
} EditorState;

#endif
