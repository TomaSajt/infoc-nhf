#include "hover.h"
#include "draw.h" // TODO: this is only here for the hitbox radius
#include "geom/defs.h"
#include "geom/state.h"
#include "geom/util.h"
#include "mode/defs.h"

/**
 * Gets the closest PointDef that is considered to be hovered if the mouse is at
 * w_mouse_pos
 *
 * \param as           a pointer to the current AppState.
 * \param w_mouse_pos  the position of the mouse in world coordinates
 *
 * \returns a pointer to the PointDef closest to the mouse or NULL if no
 * PointDef is considered hovered
 */
PointDef *get_hovered_point(AppState const *as, Pos2D const *w_mouse_pos) {
  PointDef *best = NULL;
  double best_dist = 0;
  for (GenericElemList *curr = as->gs.pd_list; curr != NULL;
       curr = curr->next) {
    PointDef *pd = curr->pd;
    eval_point(pd);
    if (pd->val.invalid)
      continue;

    double d = dist_from_pos(w_mouse_pos, &pd->val.pos);
    if (d * as->view_info.scale > POINT_HITBOX_RADIUS)
      continue;

    if (best == NULL || best_dist > d) {
      best_dist = d;
      best = pd;
    }
  }

  return best;
}

typedef struct {
  union {
    LineDef *ld;
    CircleDef *cd;
  };
  double dist;
} SortData;

/** Swaps the values behind a and b so that a->dist < b->dist after the function
 * finishes.
 * Used by the tiny insertion sorts when calculating the 2 closest lines/circles
 */
static void swap_for_minmax(SortData *a, SortData *b) {
  if (a->dist >= b->dist) {
    SortData temp = *a;
    *a = *b;
    *b = temp;
  }
}

/** Gets the closest two lines to the mouse
 *
 * \param as            the current AppState
 * \param w_mouse_pos   the position of the mouse in world coordinates
 * \param other_out     an output parameter for the second closest hovered
 *                      LineDef. Can be set to NULL to not use the value. If
 *                      there aren't two lines being hovered, the output value
 *                      will be set to NULL.
 *
 * \returns   A pointer to the closest hovered LineDef to the mouse or NULL if
 *            there is no hovered LineDef.
 */
LineDef *get_hovered_lines(AppState const *as, Pos2D const *w_mouse_pos,
                           LineDef **other_out) {
  SortData bad = {.ld = NULL, .dist = 1e10};
  SortData arr[3] = {bad, bad, bad};

  for (GenericElemList *curr = as->gs.ld_list; curr != NULL;
       curr = curr->next) {
    LineDef *ld = curr->ld;
    eval_line(ld);
    if (ld->val.invalid)
      continue;

    double d =
        dist_from_line(w_mouse_pos, &ld->val.start, &ld->val.end, ld->ext_mode);
    if (d * as->view_info.scale > LINE_HITBOX_RADIUS)
      continue;

    // tiny insertion sort
    arr[2] = (SortData){.ld = ld, .dist = d};
    swap_for_minmax(&arr[1], &arr[2]);
    swap_for_minmax(&arr[0], &arr[1]);
  }

  if (other_out != NULL) {
    *other_out = arr[1].ld;
  }
  return arr[0].ld;
}

LineDef *get_hovered_line(AppState const *as, Pos2D const *w_mouse_pos) {
  return get_hovered_lines(as, w_mouse_pos, NULL);
}

CircleDef *get_hovered_circles(AppState const *as, Pos2D const *w_mouse_pos,
                               CircleDef **other_out) {
  SortData bad = {.cd = NULL, .dist = 1e10};
  SortData arr[3] = {bad, bad, bad};

  for (GenericElemList *curr = as->gs.cd_list; curr != NULL;
       curr = curr->next) {
    CircleDef *cd = curr->cd;
    eval_circle(cd);
    if (cd->val.invalid)
      continue;

    double d = dist_from_circle(w_mouse_pos, &cd->val.center, cd->val.radius);
    if (d * as->view_info.scale > LINE_HITBOX_RADIUS)
      continue;

    // tiny insertion sort
    arr[2] = (SortData){.cd = cd, .dist = d};
    swap_for_minmax(&arr[1], &arr[2]);
    swap_for_minmax(&arr[0], &arr[1]);
  }

  if (other_out != NULL) {
    *other_out = arr[1].cd;
  }
  return arr[0].cd;
}

CircleDef *get_hovered_circle(AppState const *as, Pos2D const *w_mouse_pos) {
  return get_hovered_circles(as, w_mouse_pos, NULL);
}

// Case 1: the potential point is a hovered, already existing point
//   - return pointer to the already allocated hovered point
//   - copy the hovered point's value *pot_out
// Case 2: the potential point doesn't already exist
//   - return NULL
//   - set *pot_out to the potential point's value
PointDef *get_potential_point(AppState const *as, Pos2D const *w_mouse_pos,
                              PointDef *pot_out) {
  PointDef *hovered_point = get_hovered_point(as, w_mouse_pos);
  if (hovered_point != NULL) {
    *pot_out = *hovered_point;
    return hovered_point;
  }
  LineDef *hovered_line_2;
  LineDef *hovered_line = get_hovered_lines(as, w_mouse_pos, &hovered_line_2);

  CircleDef *hovered_circle_2;
  CircleDef *hovered_circle =
      get_hovered_circles(as, w_mouse_pos, &hovered_circle_2);

  if (hovered_line != NULL) {
    if (hovered_line_2 != NULL) {
      PointDef pot = make_point_intsec_line_line(hovered_line, hovered_line_2);
      eval_point(&pot);
      if (!pot.val.invalid) {
        if (dist_from_pos(w_mouse_pos, &pot.val.pos) <= POINT_HITBOX_RADIUS) {
          *pot_out = pot;
          return NULL;
        }
      }
    }

    if (hovered_circle != NULL) {
      PointDef pot1 = make_point_intsec_line_circle(
          hovered_line, hovered_circle, ILC_PROG_LOWER);
      PointDef pot2 = make_point_intsec_line_circle(
          hovered_line, hovered_circle, ILC_PROG_HIGHER);
      eval_point(&pot1);
      eval_point(&pot2);
      // it is not guaranteed that if we are hovering both they are actually
      // intersecting
      if (!pot1.val.invalid && !pot2.val.invalid) {

        double d1 = dist_from_pos(w_mouse_pos, &pot1.val.pos);
        double d2 = dist_from_pos(w_mouse_pos, &pot2.val.pos);
        PointDef pot = d1 < d2 ? pot1 : pot2;
        if (dist_from_pos(w_mouse_pos, &pot.val.pos) <= POINT_HITBOX_RADIUS) {
          *pot_out = pot;
          return NULL;
        }
      }
    }

    double prog = line_closest_prog_from_pos(
        w_mouse_pos, &hovered_line->val.start, &hovered_line->val.end,
        hovered_line->ext_mode);
    *pot_out = make_point_glider_on_line(hovered_line, prog);
    return NULL;
  }

  if (hovered_circle != NULL) {
    if (hovered_circle_2 != NULL) {
      PointDef pot1 = make_point_intsec_circle_circle(
          hovered_circle, hovered_circle_2, ICC_LEFT);
      PointDef pot2 = make_point_intsec_circle_circle(
          hovered_circle, hovered_circle_2, ICC_RIGHT);
      eval_point(&pot1);
      eval_point(&pot2);
      // it is not guaranteed that if we are hovering both they are actually
      // intersecting
      if (!pot1.val.invalid && !pot2.val.invalid) {
        double d1 = dist_from_pos(w_mouse_pos, &pot1.val.pos);
        double d2 = dist_from_pos(w_mouse_pos, &pot2.val.pos);
        PointDef pot = d1 < d2 ? pot1 : pot2;
        if (dist_from_pos(w_mouse_pos, &pot.val.pos) <= POINT_HITBOX_RADIUS) {
          *pot_out = pot;
          return NULL;
        }
      }
    }
    double prog =
        circle_closest_prog_from_pos(w_mouse_pos, &hovered_circle->val.center);
    *pot_out = make_point_glider_on_circle(hovered_circle, prog);
    return NULL;
  }

  *pot_out = make_point_literal(*w_mouse_pos);
  return NULL;
}

PointDef *maybe_alloc_reg_potential_point(AppState *as,
                                          Pos2D const *w_mouse_pos) {
  PointDef pot;
  PointDef *pd = get_potential_point(as, w_mouse_pos, &pot);
  if (pd != NULL)
    return pd;

  // maybe returns NULL
  return alloc_and_reg_point(&as->gs, pot);
}
