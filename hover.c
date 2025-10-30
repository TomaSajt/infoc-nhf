#include "hover.h"

#include "geom/util.h"

// TODO: this is only here for the hitbox radius
#include "draw.h"
#include "geom/defs.h"

PointDef *get_hovered_point(AppState const *as, Pos2D w_mouse_pos) {
  PointDef *best = NULL;
  double best_dist = 0;
  for (int i = 0; i < as->gs.p_n; i++) {
    PointDef *pd = as->gs.point_defs[i];
    eval_point(pd);
    if (pd->val.invalid)
      continue;

    double d = dist_from_pos(&w_mouse_pos, &pd->val.pos);
    if (d * as->view_info.scale > POINT_HITBOX_RADIUS)
      continue;

    if (best == NULL || best_dist > d) {
      best_dist = d;
      best = pd;
    }
  }

  return best;
}

LineDef *get_hovered_line(AppState const *as, Pos2D w_mouse_pos) {
  LineDef *best = NULL;
  double best_dist = 0;
  for (int i = 0; i < as->gs.l_n; i++) {
    LineDef *ld = as->gs.line_defs[i];
    eval_line(ld);
    if (ld->val.invalid)
      continue;

    double d = dist_from_line(&w_mouse_pos, &ld->val.start, &ld->val.end,
                              ld->ext_mode);
    if (d * as->view_info.scale > LINE_HITBOX_RADIUS)
      continue;

    if (best == NULL || best_dist > d) {
      best_dist = d;
      best = ld;
    }
  }

  return best;
}

CircleDef *get_hovered_circle(AppState const *as, Pos2D w_mouse_pos) {
  CircleDef *best = NULL;
  double best_dist = 0;
  for (int i = 0; i < as->gs.c_n; i++) {
    CircleDef *cd = as->gs.circle_defs[i];
    eval_circle(cd);
    if (cd->val.invalid)
      continue;

    double d = dist_from_circle(&w_mouse_pos, &cd->val.center, cd->val.radius);
    if (d * as->view_info.scale > LINE_HITBOX_RADIUS)
      continue;

    if (best == NULL || best_dist > d) {
      best_dist = d;
      best = cd;
    }
  }

  return best;
}

PointDef *get_hovered_or_make_potential_point(AppState const *as, Pos2D w_mouse_pos,
                                              PointDef *pot_out) {
  PointDef *hovered_point = get_hovered_point(as, w_mouse_pos);
  if (hovered_point != NULL)
    return hovered_point;
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
      *pot_out = d1 < d2 ? pot1 : pot2;
      return NULL;
    } else {
      double prog = line_closest_prog_from_pos(
          &w_mouse_pos, &hovered_line->val.start, &hovered_line->val.end,
          hovered_line->ext_mode);
      *pot_out = make_point_glider_on_line(hovered_line, prog);
      return NULL;
    }
  } else {
    if (hovered_circle != NULL) {
      double prog = circle_closest_prog_from_pos(&w_mouse_pos,
                                                 &hovered_circle->val.center);
      *pot_out = make_point_glider_on_circle(hovered_circle, prog);
      return NULL;
    } else {
      *pot_out = make_point_literal(w_mouse_pos);
      return NULL;
    }
  }
}
