#include "category_defs.h"
#include "circle.h"
#include "delete.h"
#include "line.h"
#include "midpoint.h"
#include "move.h"
#include "point.h"

ModeInfo const manage_category_modes[] = {
    {
        .mode = EM_MOVE,
        .name = "Move (M)",
        .keycode = SDLK_M,
        .on_mouse_down = move__on_mouse_down,
        .on_mouse_move = move__on_mouse_move,
        .on_mouse_up = move__on_mouse_up,
    },
    {
        .mode = EM_DELETE,
        .name = "Delete (D)",
        .keycode = SDLK_D,
        .on_mouse_down = delete__on_mouse_down,
    },
};

ModeInfo const points_category_modes[] = {
    {
        .mode = EM_POINT,
        .name = "Point (P)",
        .keycode = SDLK_P,
        .on_mouse_down = point__on_mouse_down,
        .on_render = point__on_render,
    },
    {
        .mode = EM_MIDPOINT,
        .name = "Midpoint",
        .keycode = SDLK_UNKNOWN,
        .on_mouse_down = midpoint__on_mouse_down,
        .on_render = midpoint__on_render,
    },
};

ModeInfo const lines_category_modes[] = {
    {
        .mode = EM_SEGMENT,
        .name = "Segment (S)",
        .keycode = SDLK_S,
        .on_mouse_down = line__on_mouse_down,
        .on_render = line__on_render,
    },
    {
        .mode = EM_LINE,
        .name = "Line (L)",
        .keycode = SDLK_L,
        .on_mouse_down = line__on_mouse_down,
        .on_render = line__on_render,
    },
    {
        .mode = EM_RAY,
        .name = "Ray (R)",
        .keycode = SDLK_R,
        .on_mouse_down = line__on_mouse_down,
        .on_render = line__on_render,
    },
    {
        .mode = EM_PARALLEL,
        .name = "Parallel",
        .keycode = SDLK_UNKNOWN,
        .on_mouse_down = NULL, // TODO
        .on_render = NULL, // TODO
    },
    {
        .mode = EM_PERPENDICULAR,
        .name = "Perpendicular",
        .keycode = SDLK_UNKNOWN,
        .on_mouse_down = NULL, // TODO
        .on_render = NULL, // TODO
    },
};

ModeInfo const circles_category_modes[] = {
    {
        .mode = EM_CIRCLE,
        .name = "Circle (C)",
        .keycode = SDLK_C,
        .on_mouse_down = circle__on_mouse_down,
        .on_render = circle__on_render,
    },
    {
        .mode = EM_CIRCLE_BY_LEN,
        .name = "Circle by len",
        .keycode = SDLK_UNKNOWN,
        .on_mouse_down = NULL, // TODO
        .on_render = NULL, // TODO
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
