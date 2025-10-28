#include "point.h"

#include "../draw.h"
#include "../geom.h"
#include "../hover.h"

void enter_point_mode(EditorState *es) { es->mode = EM_POINT; }

PointDef point__get_potential_point(AppState *as, Pos2D w_mouse_pos) {
  // TODO: implement get_hovered_lines and get_hovered_circles
  //       for line-line and circle-circle intersections
  LineDef *hovered_line = get_hovered_line(as, w_mouse_pos);
  CircleDef *hovered_circle = get_hovered_circle(as, w_mouse_pos);
  if (hovered_line != NULL) {
    if (hovered_circle != NULL) {
      PointDef pot1 = make_point_intsec_line_circle(
          hovered_line, hovered_circle, ILC_PROG_LOWER);
      PointDef pot2 = make_point_intsec_line_circle(
          hovered_line, hovered_circle, ILC_PROG_HIGHER);
      eval_point(&pot1);
      eval_point(&pot2);
      double d1 = dist_from_pos(&w_mouse_pos, &pot1.val.pos);
      double d2 = dist_from_pos(&w_mouse_pos, &pot2.val.pos);
      return d1 < d2 ? pot1 : pot2;
    } else {
      double prog = line_closest_prog_from_pos(
          &w_mouse_pos, &hovered_line->val.start, &hovered_line->val.end,
          hovered_line->ext_mode);
      return make_point_glider_on_line(hovered_line, prog);
    }
  } else {
    if (hovered_circle != NULL) {
      double prog = circle_closest_prog_from_pos(&w_mouse_pos,
                                                 &hovered_circle->val.center);
      return make_point_glider_on_circle(hovered_circle, prog);
    } else {
      return make_point_literal(w_mouse_pos);
    }
  }
}

SDL_AppResult point__on_mouse_down(AppState *as, Pos2D w_mouse_pos) {
  PointDef *pd =
      alloc_and_reg_point(&as->gs, point__get_potential_point(as, w_mouse_pos));
  if (pd == NULL)
    return SDL_APP_FAILURE;
  return SDL_APP_CONTINUE;
}

void point__on_render(AppState *as, Pos2D w_mouse_pos) {
  PointDef potential_point = point__get_potential_point(as, w_mouse_pos);
  draw_point(as, &potential_point, CYAN);
}
