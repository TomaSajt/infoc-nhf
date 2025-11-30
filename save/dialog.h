#ifndef SAVE_DIALOG_H
#define SAVE_DIALOG_H

#include "../state.h"

void do_load_from_file(AppState *as, char const *file_path, bool show_confirm);

void show_open_prompt(AppState *as);

void do_save_to_file(AppState *as, char const *file_path);

void show_save_as_prompt(AppState *as);

void save_or_save_as(AppState *as);

void make_new_canvas(AppState *as);

bool show_lose_data_prompt(AppState *as);

#endif
