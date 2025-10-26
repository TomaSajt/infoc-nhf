#include "geom.h"

#include "geom_defs.h"

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
      .a = y1 - y2,
      .b = x2 - x1,
      .c = (x2 - x1) * y1 + (y1 - y2) * x1,
  };
}

// returns false if no or infinite solutions
// puts point into res
bool calc_intsec_line_eqs(LineEq le1, LineEq le2, Pos2D *res) {
  double denom = le1.a * le2.b - le1.b * le2.a;

  if (fabs(denom) < 1e-10) {
    return false;
  }

  *res = (Pos2D){
      .x = (le1.c * le2.b - le1.b * le2.c) / denom,
      .y = (le1.a * le2.c - le1.c * le2.a) / denom,
  };
  return true;
}

Pos2D lerp(Pos2D *start, Pos2D *end, double prog) {
  return (Pos2D){
      .x = start->x * (1.0 - prog) + end->x * prog,
      .y = start->y * (1.0 - prog) + end->y * prog,
  };
}

Pos2D pos_from_circle_prog(Pos2D *center, double radius, double prog) {
  return (Pos2D){
      .x = center->x + radius * cos(M_TAU * prog),
      .y = center->y + radius * sin(M_TAU * prog),
  };
}

double vec_len(double x, double y) { return sqrt(x * x + y * y); }

bool calc_intsec_line_circle(Pos2D *start, Pos2D *end, Pos2D *center,
                             double radius, ILCProgType prog_type, Pos2D *res) {
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

  *res = (Pos2D){
      .x = start->x + vx * prog,
      .y = start->y + vy * prog,
  };
  return true;
}

bool calc_intsec_circle_circle(Pos2D *center1, double radius1, Pos2D *center2,
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

  *res = (Pos2D){
      .x = center1->x + radius1 * cos(angle),
      .y = center1->y + radius1 * sin(angle),
  };
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
      pd->val.pos = lerp(&p1->val.pos, &p2->val.pos, 0.5);
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
      pd->val.invalid = !calc_intsec_line_eqs(le1, le2, &pd->val.pos);
    }
    break;
  }
  case PD_GLIDER_ON_CIRCLE: {
    CircleDef *c = pd->glider_on_circle.c;
    double prog = pd->glider_on_circle.prog;
    eval_circle(c);
    pd->val.invalid = c->val.invalid;
    if (!pd->val.invalid) {
      pd->val.pos = pos_from_circle_prog(&c->val.center, c->val.radius, prog);
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
          !calc_intsec_line_circle(&l->val.start, &l->val.end, &c->val.center,
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
      pd->val.invalid = !calc_intsec_circle_circle(
          &c1->val.center, c1->val.radius, &c2->val.center, c2->val.radius,
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
                      dist_from_pos(&p1->val.pos, &p2->val.pos) < 1e-10;
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
      ld->val.end = (Pos2D){
          .x = p->val.pos.x + (l->val.end.x - l->val.start.x),
          .y = p->val.pos.y + (l->val.end.y - l->val.end.y),
      };
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
      ld->val.end = (Pos2D){
          .x = p->val.pos.x + (l->val.end.y - l->val.end.y),
          .y = p->val.pos.y - (l->val.end.x - l->val.start.x),
      };
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
      cd->val.radius = dist_from_pos(&p1->val.pos, &p2->val.pos);
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
      cd->val.radius = dist_from_pos(&l->val.start, &l->val.end);
    }
    break;
  }
  }

  cd->val.dirty = false;
}

bool eval_point_del_flag(PointDef *pd) {
  if (pd->del_flag != DF_DONT_KNOW)
    return pd->del_flag == DF_YES;

  bool res = false;
  switch (pd->type) {
  case PD_LITERAL: {
    break;
  }
  case PD_MIDPOINT: {
    PointDef *p1 = pd->midpoint.p1;
    PointDef *p2 = pd->midpoint.p2;
    res |= eval_point_del_flag(p1);
    res |= eval_point_del_flag(p2);
    break;
  }
  case PD_GLIDER_ON_LINE: {
    LineDef *l = pd->glider_on_line.l;
    res |= eval_line_del_flag(l);
    break;
  }
  case PD_INTSEC_LINE_LINE: {
    LineDef *l1 = pd->intsec_line_line.l1;
    LineDef *l2 = pd->intsec_line_line.l2;
    res |= eval_line_del_flag(l1);
    res |= eval_line_del_flag(l2);
    break;
  }
  case PD_GLIDER_ON_CIRCLE: {
    CircleDef *c = pd->glider_on_circle.c;
    res |= eval_circle_del_flag(c);
    break;
  }
  case PD_INTSEC_LINE_CIRCLE: {
    LineDef *l = pd->intsec_line_circle.l;
    CircleDef *c = pd->intsec_line_circle.c;
    res |= eval_line_del_flag(l);
    res |= eval_circle_del_flag(c);
    break;
  }
  case PD_INTSEC_CIRCLE_CIRCLE: {
    CircleDef *c1 = pd->intsec_circle_circle.c1;
    CircleDef *c2 = pd->intsec_circle_circle.c2;
    res |= eval_circle_del_flag(c1);
    res |= eval_circle_del_flag(c2);
    break;
  }
  }
  pd->del_flag = res ? DF_YES : DF_NO;
  return res;
}

bool eval_line_del_flag(LineDef *ld) {
  if (ld->del_flag != DF_DONT_KNOW)
    return ld->del_flag == DF_YES;

  bool res = false;
  switch (ld->type) {
  case LD_POINT_TO_POINT: {
    PointDef *p1 = ld->point_to_point.p1;
    PointDef *p2 = ld->point_to_point.p2;
    res |= eval_point_del_flag(p1);
    res |= eval_point_del_flag(p2);
    break;
  }
  case LD_PARALLEL: {
    LineDef *l = ld->parallel.l;
    PointDef *p = ld->parallel.p;
    res |= eval_line_del_flag(l);
    res |= eval_point_del_flag(p);
    break;
  }
  case LD_PERPENDICULAR: {
    LineDef *l = ld->perpendicular.l;
    PointDef *p = ld->perpendicular.p;
    res |= eval_line_del_flag(l);
    res |= eval_point_del_flag(p);
    break;
  }
  }

  ld->del_flag = res ? DF_YES : DF_NO;
  return res;
}

bool eval_circle_del_flag(CircleDef *cd) {
  if (cd->del_flag != DF_DONT_KNOW)
    return cd->del_flag == DF_YES;

  bool res = false;
  switch (cd->type) {
  case CD_CENTER_POINT_OUTER_POINT: {
    PointDef *p1 = cd->center_point_outer_point.center;
    PointDef *p2 = cd->center_point_outer_point.outer;
    res |= eval_point_del_flag(p1);
    res |= eval_point_del_flag(p2);
    break;
  }
  case CD_CENTER_POINT_RADIUS_SEG: {
    PointDef *p = cd->center_point_radius_seg.center;
    LineDef *l = cd->center_point_radius_seg.rad_seg;
    res |= eval_point_del_flag(p);
    res |= eval_line_del_flag(l);
    break;
  }
  }

  cd->del_flag = res ? DF_YES : DF_NO;
  return res;
}

double dist_from_pos(Pos2D *pos1, Pos2D *pos2) {
  return vec_len(pos1->x - pos2->x, pos1->y - pos2->y);
}

double clamp_line_prog(double prog, LineExtMode ext_mode) {
  switch (ext_mode) {
  case L_EXT_SEGMENT:
    return prog < 0 ? 0 : prog > 1 ? 1 : prog;
  case L_EXT_RAY:
    return prog < 0 ? 0 : prog;
  default:
    return prog;
  }
}

double line_closest_prog_from_pos(Pos2D *pos, Pos2D *s, Pos2D *e,
                                  LineExtMode ext_mode) {
  double vx = e->x - s->x;
  double vy = e->y - s->y;
  double dx = pos->x - s->x;
  double dy = pos->y - s->y;
  double prog = (dx * vx + dy * vy) / (vx * vx + vy * vy);
  return clamp_line_prog(prog, ext_mode);
}

double dist_from_line(Pos2D *pos, Pos2D *start, Pos2D *end,
                      LineExtMode ext_mode) {
  double prog = line_closest_prog_from_pos(pos, start, end, ext_mode);
  Pos2D res_pos = lerp(start, end, prog);
  return dist_from_pos(pos, &res_pos);
}

double circle_closest_prog_from_pos(Pos2D *pos, Pos2D *c) {
  double dx = pos->x - c->x;
  double dy = pos->y - c->y;
  double angle = atan2(dy, dx);
  double prog = angle / M_TAU;
  return prog - floor(prog); // [0;1)
}

double dist_from_circle(Pos2D *pos, Pos2D *center, double radius) {
  double signed_dist = dist_from_pos(pos, center) - radius;
  return fabs(signed_dist);
}
