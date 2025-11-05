#include "defs.h"
#include "circle.h"
#include "delete.h"
#include "linelike.h"
#include "midpoint.h"
#include "move.h"
#include "par_per.h"
#include "point.h"

ModeInfo const manage_category_modes[] = {
    {
        .name = "Move (M)",
        .keycode = SDLK_M,
        .init_data = move__init_data,
        .on_mouse_down = move__on_mouse_down,
        .on_mouse_move = move__on_mouse_move,
        .on_mouse_up = move__on_mouse_up,
    },
    {
        .name = "Delete (D)",
        .keycode = SDLK_D,
        .on_mouse_down = delete__on_mouse_down,
    },
};

ModeInfo const points_category_modes[] = {
    {
        .name = "Point (P)",
        .keycode = SDLK_P,
        .on_mouse_down = point__on_mouse_down,
        .on_render = point__on_render,
    },
    {
        .name = "Midpoint",
        .keycode = SDLK_UNKNOWN,
        .init_data = midpoint__init_data,
        .on_mouse_down = midpoint__on_mouse_down,
        .on_render = midpoint__on_render,
    },
};

ModeInfo const lines_category_modes[] = {
    {
        .name = "Segment (S)",
        .keycode = SDLK_S,
        .init_data = linelike__init_data,
        .on_mouse_down = segment__on_mouse_down,
        .on_render = segment__on_render,
    },
    {
        .name = "Line (L)",
        .keycode = SDLK_L,
        .init_data = linelike__init_data,
        .on_mouse_down = line__on_mouse_down,
        .on_render = line__on_render,
    },
    {
        .name = "Ray (R)",
        .keycode = SDLK_R,
        .init_data = linelike__init_data,
        .on_mouse_down = ray__on_mouse_down,
        .on_render = ray__on_render,
    },
    {
        .name = "Parallel",
        .keycode = SDLK_UNKNOWN,
        .init_data = par_per__init_data,
        .on_mouse_down = parallel__on_mouse_down,
        .on_render = parallel__on_render,
    },
    {
        .name = "Perpendicular",
        .keycode = SDLK_UNKNOWN,
        .init_data = par_per__init_data,
        .on_mouse_down = perpendicular__on_mouse_down,
        .on_render = perpendicular__on_render,
    },
};

ModeInfo const circles_category_modes[] = {
    {
        .name = "Circle (C)",
        .keycode = SDLK_C,
        .init_data = circle__init_data,
        .on_mouse_down = circle__on_mouse_down,
        .on_render = circle__on_render,
    },
    {
        .name = "Circle by len",
        .keycode = SDLK_UNKNOWN,
        .init_data = NULL,     // TODO
        .on_mouse_down = NULL, // TODO
        .on_render = NULL,     // TODO
    },
};

CategoryInfo const editor_categories[] = {
    {
        .name = "Manage (1)",
        .keycode = SDLK_1,
        .modes = manage_category_modes,
        .num_modes = sizeof(manage_category_modes) / sizeof(ModeInfo),
    },
    {
        .name = "Points (2)",
        .keycode = SDLK_2,
        .modes = points_category_modes,
        .num_modes = sizeof(points_category_modes) / sizeof(ModeInfo),
    },
    {
        .name = "Lines (3)",
        .keycode = SDLK_3,
        .modes = lines_category_modes,
        .num_modes = sizeof(lines_category_modes) / sizeof(ModeInfo),
    },
    {
        .name = "Circles (4)",
        .keycode = SDLK_4,
        .modes = circles_category_modes,
        .num_modes = sizeof(circles_category_modes) / sizeof(ModeInfo),
    },

};

EditorInfo const editor_info = {
    .cats = editor_categories,
    .num_cats = sizeof(editor_categories) / sizeof(CategoryInfo),
};
