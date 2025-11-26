#ifndef MODE_UTIL_H
#define MODE_UTIL_H
#include "defs.h"

void incr_curr_cat_mode_ind(EditorState *es);

void decr_curr_cat_mode_ind(EditorState *es);

void reset_mode_data(EditorState *es);

void select_mode_from_inds(EditorState *es);

#endif
