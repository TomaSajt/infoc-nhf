#include "dialog.h"
#include "../mode/util.h"
#include "SDL3/SDL_messagebox.h"
#include "data.h"

#include <stdio.h>
#include <stdlib.h>

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

void do_load_from_file(AppState *as, char const *file_path, bool show_confirm) {
  if (show_confirm && !show_lose_data_prompt(as)) {
    return;
  }
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
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                               "Failed to load file", as->window);
    }
    fclose(handle);
  } else {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Failed to open file", as->window);
  }
}

void open__on_file_selected(void *userdata, char const *const *filelist,
                            int filter) {
  AppState *as = userdata;
  (void)filter; // unused parameter

  if (filelist == NULL) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "An error occurred with the file selector",
                             as->window);
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
  do_load_from_file(as, file_path, true);
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
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Failed to open file", as->window);
  }
}

void save__on_file_selected(void *userdata, char const *const *filelist,
                            int filter) {
  AppState *as = userdata;
  (void)filter; // unused parameter

  if (filelist == NULL) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "An error occurred with the file selector",
                             as->window);
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

bool show_lose_data_prompt(AppState *as) {
  SDL_MessageBoxButtonData const buttons[] = {
      {
          .buttonID = 1,
          .text = "Yes",
          .flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
      },
      {
          .buttonID = 0,
          .text = "Cancel",
          .flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,
      },
  };
  SDL_MessageBoxData const messageboxdata = {
      .window = as->window,
      .title = "Continue?",
      .message = "Do you want to continue? Any unsaved data will be lost.",
      .numbuttons = 2,
      .buttons = buttons,
      .flags = SDL_MESSAGEBOX_WARNING,
  };

  int button_id;
  bool success = SDL_ShowMessageBox(&messageboxdata, &button_id);
  if (!success) {
    printf("SDL error: %s\n", SDL_GetError());
    return false;
  }
  return button_id == 1;
}
