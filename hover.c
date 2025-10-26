#include "hover.h"

#include "geom.h"

// TODO: this is only here for the hitbox radius
#include "draw.h"

PointDef *get_hovered_point(AppState *as, Pos2D w_mouse_pos) {
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

LineDef *get_hovered_line(AppState *as, Pos2D w_mouse_pos) {
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

CircleDef *get_hovered_circle(AppState *as, Pos2D w_mouse_pos) {
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
