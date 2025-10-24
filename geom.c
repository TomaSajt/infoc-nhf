#include "geom.h"
#include <math.h>

#define M_TAU 6.28318530717958647693

// a*x + b*y = c
typedef struct {
  double a;
  double b;
  double c;
} LineEq;

LineEq line_eq_from_start_end(Pos2D start, Pos2D end) {
  int x1 = start.x;
  int y1 = start.y;
  int x2 = end.x;
  int y2 = end.y;
  return (LineEq){
      .a = y1 - y2, .b = x2 - x1, .c = (x2 - x1) * y1 + (y1 - y2) * x1};
}

// returns false if no or infinite solutions
// puts point into res
bool intsec_line_eqs(LineEq le1, LineEq le2, Pos2D *res) {
  double denom = le1.a * le2.b - le1.b * le2.a;

  if (fabs(denom) < 1e-10) {
    return false;
  }

  *res = (Pos2D){.x = (le1.c * le2.b - le1.b * le2.c) / denom,
                 .y = (le1.a * le2.c - le1.c * le2.a) / denom};
  return true;
}

Pos2D calc_mid(Pos2D *pos1, Pos2D *pos2) {
  return (Pos2D){.x = (pos1->x + pos2->x) * 0.5,
                 .y = (pos1->y + pos2->y) * 0.5};
}

Pos2D lerp(Pos2D *start, Pos2D *end, double prog) {
  return (Pos2D){.x = start->x * (1.0 - prog) + end->x * prog,
                 .y = start->y * (1.0 - prog) + end->y * prog};
}

Pos2D calc_point_center_rot(Pos2D *center, double radius, double prog) {
  return (Pos2D){.x = center->x + radius * cos(M_TAU * prog),
                 .y = center->y + radius * sin(M_TAU * prog)};
}

double vec_len(double x, double y) { return sqrt(x * x + y * y); }

double pos_distance(Pos2D *pos1, Pos2D *pos2) {
  return vec_len(pos1->x - pos2->x, pos1->y - pos2->y);
}

double distance_from_circle(Pos2D *pos, Pos2D *center, double radius) {
  double signed_dist = pos_distance(pos, center) - radius;
  return fabs(signed_dist);
}

double distance_from_line(Pos2D *pos, Pos2D *start, Pos2D *end) {
  double nx = end->y - start->y;
  double ny = -(end->x - start->x);

  double signed_dist =
      ((pos->x - start->x) * nx + (pos->y - start->y) * ny) / vec_len(nx, ny);

  return fabs(signed_dist);
}

bool intsec_line_circle(Pos2D *start, Pos2D *end, Pos2D *center, double radius,
                        ILCProgType prog_type, Pos2D *res) {
  double vx = end->x - start->x;
  double vy = end->y - start->y;
  double dx = center->x - start->x;
  double dy = center->y - start->y;

  double denom = vx * vx + vy * vy;

  // assuming the line is not invalid, this should never be zero
  // but let's be safe
  if (denom < 1e-20) {
    return false;
  }
  double p = (dx * vx + dy * vy) / denom;
  double q = (dx * dx + dy * dy - radius * radius) / denom;

  double ppmq = p * p - q;
  if (ppmq < 0) {
    // no intersections
    return false;
  }

  double prog = prog_type == ILC_PROG_HIGHER ? p + sqrt(ppmq) : p - sqrt(ppmq);

  *res = (Pos2D){.x = start->x + vx * prog, .y = start->y + vy * prog};
  return true;
}

bool intsec_circle_circle(Pos2D *center1, double radius1, Pos2D *center2,
                          double radius2, ICCSide side, Pos2D *res) {
  double dx = center2->x - center1->x;
  double dy = center2->y - center1->y;

  double numer = dx * dx + dy * dy + radius1 * radius1 - radius2 * radius2;
  double denom = 2 * radius1 * sqrt(dx * dx + dy * dy);
  if (fabs(denom) < 1e-20) {
    return false;
  }
  if (fabs(numer) > fabs(denom)) {
    return false;
  }

  // frac is in [-1;1]
  double frac = numer / denom;
  double angle = atan2(dy, dx) + (side == ICC_LEFT ? 1 : -1) * acos(frac);

  *res = (Pos2D){.x = center1->x + radius1 * cos(angle),
                 .y = center1->y + radius1 * sin(angle)};
  return true;
}

void eval_point(PointDef *pd) {
  if (!pd->val.dirty)
    return;

  switch (pd->type) {
  case PD_LITERAL: {
    pd->val.invalid = false;
    pd->val.pos = pd->literal.pos;
    break;
  }
  case PD_MIDPOINT: {
    PointDef *p1 = pd->midpoint.p1;
    PointDef *p2 = pd->midpoint.p2;
    eval_point(p1);
    eval_point(p2);

    pd->val.invalid = p1->val.invalid || p2->val.invalid;
    if (!pd->val.invalid) {
      pd->val.pos = calc_mid(&p1->val.pos, &p2->val.pos);
    }
    break;
  }
  case PD_GLIDER_ON_LINE: {
    LineDef *l = pd->glider_on_line.l;
    double prog = pd->glider_on_line.prog;
    eval_line(l);

    pd->val.invalid = l->val.invalid;
    if (!pd->val.invalid) {
      pd->val.pos = lerp(&l->val.start, &l->val.end, prog);
    }
    break;
  }
  case PD_INTSEC_LINE_LINE: {
    LineDef *l1 = pd->intsec_line_line.l1;
    LineDef *l2 = pd->intsec_line_line.l2;
    eval_line(l1);
    eval_line(l2);

    pd->val.invalid = l1->val.invalid || l2->val.invalid;
    if (!pd->val.invalid) {
      LineEq le1 = line_eq_from_start_end(l1->val.start, l1->val.end);
      LineEq le2 = line_eq_from_start_end(l2->val.start, l2->val.end);
      pd->val.invalid = !intsec_line_eqs(le1, le2, &pd->val.pos);
    }
    break;
  }
  case PD_GLIDER_ON_CIRCLE: {
    CircleDef *c = pd->glider_on_circle.c;
    double prog = pd->glider_on_circle.prog;
    eval_circle(c);
    pd->val.invalid = c->val.invalid;
    if (!pd->val.invalid) {
      pd->val.pos = calc_point_center_rot(&c->val.center, c->val.radius, prog);
    }
    break;
  }
  case PD_INTSEC_LINE_CIRCLE: {
    LineDef *l = pd->intsec_line_circle.l;
    CircleDef *c = pd->intsec_line_circle.c;
    ILCProgType prog_type = pd->intsec_line_circle.prog_type;
    eval_line(l);
    eval_circle(c);

    pd->val.invalid = l->val.invalid || c->val.invalid;
    if (!pd->val.invalid) {
      pd->val.invalid =
          !intsec_line_circle(&l->val.start, &l->val.end, &c->val.center,
                              c->val.radius, prog_type, &pd->val.pos);
    }
    break;
  }
  case PD_INTSEC_CIRCLE_CIRCLE: {
    CircleDef *c1 = pd->intsec_circle_circle.c1;
    CircleDef *c2 = pd->intsec_circle_circle.c2;
    ICCSide side = pd->intsec_circle_circle.side;
    eval_circle(c1);
    eval_circle(c2);

    pd->val.invalid = c1->val.invalid || c2->val.invalid;
    if (!pd->val.invalid) {
      pd->val.invalid = !intsec_circle_circle(&c1->val.center, c1->val.radius,
                                              &c2->val.center, c2->val.radius,
                                              side, &pd->val.pos);
    }
    break;
  }
  }
  pd->val.dirty = false;
}

void eval_line(LineDef *ld) {
  if (!ld->val.dirty)
    return;

  switch (ld->type) {
  case LD_POINT_TO_POINT: {
    PointDef *p1 = ld->point_to_point.p1;
    PointDef *p2 = ld->point_to_point.p2;
    eval_point(p1);
    eval_point(p2);

    ld->val.invalid = p1->val.invalid || p2->val.invalid ||
                      pos_distance(&p1->val.pos, &p2->val.pos) < 1e-10;
    if (!ld->val.invalid) {
      ld->val.start = p1->val.pos;
      ld->val.end = p2->val.pos;
    }
    break;
  }
  case LD_PARALLEL: {
    LineDef *l = ld->parallel.l;
    PointDef *p = ld->parallel.p;
    eval_line(l);
    eval_point(p);

    ld->val.invalid = l->val.invalid || p->val.invalid;
    if (!ld->val.invalid) {
      ld->val.start = p->val.pos;
      ld->val.end = (Pos2D){.x = p->val.pos.x + (l->val.end.x - l->val.start.x),
                            .y = p->val.pos.y + (l->val.end.y - l->val.end.y)};
    }
    break;
  }
  case LD_PERPENDICULAR: {
    LineDef *l = ld->perpendicular.l;
    PointDef *p = ld->perpendicular.p;
    eval_line(l);
    eval_point(p);

    ld->val.invalid = l->val.invalid || p->val.invalid;
    if (!ld->val.invalid) {
      ld->val.start = p->val.pos;
      ld->val.end =
          (Pos2D){.x = p->val.pos.x + (l->val.end.y - l->val.end.y),
                  .y = p->val.pos.y - (l->val.end.x - l->val.start.x)};
    }
    break;
  }
  }
  ld->val.dirty = false;
}

void eval_circle(CircleDef *cd) {
  if (!cd->val.dirty)
    return;

  switch (cd->type) {
  case CD_CENTER_POINT_OUTER_POINT: {
    PointDef *p1 = cd->center_point_outer_point.center;
    PointDef *p2 = cd->center_point_outer_point.outer;
    eval_point(p1);
    eval_point(p2);

    cd->val.invalid = p1->val.invalid || p2->val.invalid;
    if (!cd->val.invalid) {
      cd->val.center = p1->val.pos;
      cd->val.radius = pos_distance(&p1->val.pos, &p2->val.pos);
    }
    break;
  }
  case CD_CENTER_POINT_RADIUS_SEG: {
    PointDef *p = cd->center_point_radius_seg.center;
    LineDef *l = cd->center_point_radius_seg.rad_seg;
    eval_point(p);
    eval_line(l);

    cd->val.invalid =
        p->val.invalid || l->val.invalid || l->ext_mode != L_EXT_SEGMENT;
    if (!cd->val.invalid) {
      cd->val.center = p->val.pos;
      cd->val.radius = pos_distance(&l->val.start, &l->val.end);
    }
    break;
  }
  }

  cd->val.dirty = false;
}
