#include "dialog.h"
#include "../mode/util.h"
#include "data.h"

const SDL_DialogFileFilter file_filters[] = {
    {"Geom savefiles", "geom"},
    {"All files", "*"},
};

void set_save_path(AppState *as, char const *save_path) {
  char *old_save_path = as->save_path;
  as->save_path = save_path == NULL ? NULL : strdup(save_path);

  if (old_save_path != NULL) {
    free(old_save_path);
  }
}

void do_load_from_file(AppState *as, char const *file_path) {
  printf("Loading file %s\n", file_path);
  FILE *handle = fopen(file_path, "r");
  if (handle != NULL) {
    GeometryState new_gs;
    bool res = load_from_file(handle, &new_gs);
    if (res) {
      clear_geometry_state(&as->gs);
      as->gs = new_gs;
      reset_mode_data(&as->es);
      set_save_path(as, file_path);
      printf("Load successful\n");
    } else {
      printf("Load failed\n");
    }
    fclose(handle);
  } else {
    printf("Failed to open file\n");
  }
}

void open__on_file_selected(void *userdata, char const *const *filelist,
                            int filter) {
  AppState *as = userdata;
  (void)filter; // unused parameter

  if (filelist == NULL) {
    printf("An error occurred: %s\n", SDL_GetError());
    return;
  }
  char const *file_path = filelist[0];
  if (file_path == NULL) {
    printf("No files selected!\n");
    return;
  }

  if (filelist[1] != NULL) {
    printf("Multiple files were selected, only using the first!\n");
  }
  do_load_from_file(as, file_path);
}

void show_open_prompt(AppState *as) {
  char *cwd = SDL_GetCurrentDirectory();
  SDL_ShowOpenFileDialog(open__on_file_selected, as, as->window, file_filters,
                         sizeof(file_filters) / sizeof(SDL_DialogFileFilter),
                         cwd, false);
  SDL_free(cwd);
}

void do_save_to_file(AppState *as, char const *file_path) {
  printf("Saving to %s...\n", file_path);
  FILE *handle = fopen(file_path, "w");
  if (handle != NULL) {
    save_to_file(handle, &as->gs);
    set_save_path(as, file_path);
    fclose(handle);
  } else {
    printf("Failed to open file\n");
  }
}

void save__on_file_selected(void *userdata, char const *const *filelist,
                            int filter) {
  AppState *as = userdata;
  (void)filter; // unused parameter

  if (filelist == NULL) {
    printf("An error occurred: %s\n", SDL_GetError());
    return;
  }
  char const *file_path = filelist[0];
  if (file_path == NULL) {
    printf("No files selected!\n");
    return;
  }

  if (filelist[1] != NULL) {
    printf("Multiple files were selected, only using the first!\n");
  }
  do_save_to_file(as, file_path);
}

void show_save_as_prompt(AppState *as) {
  char *cwd = SDL_GetCurrentDirectory();
  SDL_ShowSaveFileDialog(save__on_file_selected, as, as->window, file_filters,
                         sizeof(file_filters) / sizeof(SDL_DialogFileFilter),
                         cwd);
  SDL_free(cwd);
}

void save_or_save_as(AppState *as) {
  if (as->save_path == NULL) {
    show_save_as_prompt(as);
  } else {
    do_save_to_file(as, as->save_path);
  }
}

void make_new_canvas(AppState *as) {
  printf("Creating new canvas...\n");
  set_save_path(as, NULL);
  reset_mode_data(&as->es);
  clear_geometry_state(&as->gs);
}
