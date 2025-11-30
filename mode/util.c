#include "util.h"

static ModeInfo const *calc_mode_from_inds(EditorState const *es) {
  CategoryState const *cs = &es->category_states[es->sel_cat_ind];
  return &cs->cat_info->modes[cs->sel_mode_ind];
}

void incr_curr_cat_mode_ind(EditorState *es) {
  CategoryState *cs = &es->category_states[es->sel_cat_ind];
  cs->sel_mode_ind++;
  int n = cs->cat_info->num_modes;
  if (cs->sel_mode_ind >= n)
    cs->sel_mode_ind -= n;
}

void decr_curr_cat_mode_ind(EditorState *es) {
  CategoryState *cs = &es->category_states[es->sel_cat_ind];
  cs->sel_mode_ind--;
  int n = cs->cat_info->num_modes;
  if (cs->sel_mode_ind < 0)
    cs->sel_mode_ind += n;
}

void reset_mode_data(EditorState *es) {
  if (es->mode_info->init_data != NULL)
    es->mode_info->init_data(&es->data);
}

void select_mode_from_inds(EditorState *es) {
  es->mode_info = calc_mode_from_inds(es);
  reset_mode_data(es);
}
