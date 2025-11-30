#include "defs.h"

SDL_Color const default_color = {.r = 255, .g = 255, .b = 255, .a = 255};

static void init_misc_point_data(PointDef *p) {
  p->val.dirty = true;
  p->del_flag = DF_DONT_KNOW;
  p->color = default_color;
}

static void init_misc_line_data(LineDef *l) {
  l->val.dirty = true;
  l->del_flag = DF_DONT_KNOW;
  l->color = default_color;
}

static void init_misc_circle_data(CircleDef *c) {
  c->val.dirty = true;
  c->del_flag = DF_DONT_KNOW;
  c->color = default_color;
}

PointDef make_point_literal(Pos2D pos) {
  PointDef p = {
      .type = PD_LITERAL,
      .literal = {.pos = pos},
  };
  init_misc_point_data(&p);
  return p;
}

PointDef make_point_midpoint(PointDef *p1, PointDef *p2) {
  PointDef p = {
      .type = PD_MIDPOINT,
      .midpoint = {.p1 = p1, .p2 = p2},
  };
  init_misc_point_data(&p);
  return p;
}

PointDef make_point_glider_on_line(LineDef *l, float prog) {
  PointDef p = {
      .type = PD_GLIDER_ON_LINE,
      .glider_on_line = {.l = l, .prog = prog},
  };
  init_misc_point_data(&p);
  return p;
}

PointDef make_point_intsec_line_line(LineDef *l1, LineDef *l2) {
  PointDef p = {
      .type = PD_INTSEC_LINE_LINE,
      .intsec_line_line = {.l1 = l1, .l2 = l2},
  };
  init_misc_point_data(&p);
  return p;
}

PointDef make_point_glider_on_circle(CircleDef *c, float prog) {
  PointDef p = {
      .type = PD_GLIDER_ON_CIRCLE,
      .glider_on_circle = {.c = c, .prog = prog},
  };
  init_misc_point_data(&p);
  return p;
}

PointDef make_point_intsec_line_circle(LineDef *l, CircleDef *c,
                                       ILCProgType prog_type) {
  PointDef p = {
      .type = PD_INTSEC_LINE_CIRCLE,
      .intsec_line_circle = {.l = l, .c = c, .prog_type = prog_type},
  };
  init_misc_point_data(&p);
  return p;
}

PointDef make_point_intsec_circle_circle(CircleDef *c1, CircleDef *c2,
                                         ICCSide side) {
  PointDef p = {
      .type = PD_INTSEC_CIRCLE_CIRCLE,
      .intsec_circle_circle = {.c1 = c1, .c2 = c2, .side = side},
  };
  init_misc_point_data(&p);
  return p;
}

LineDef make_line_point_to_point(LineExtMode ext_mode, PointDef *p1,
                                 PointDef *p2) {
  LineDef l = {
      .ext_mode = ext_mode,
      .type = LD_POINT_TO_POINT,
      .point_to_point = {.p1 = p1, .p2 = p2},
  };
  init_misc_line_data(&l);
  return l;
}

LineDef make_line_parallel(LineDef *l, PointDef *p) {
  LineDef nl = {
      .ext_mode = L_EXT_LINE,
      .type = LD_PARALLEL,
      .parallel = {.l = l, .p = p},
  };
  init_misc_line_data(&nl);
  return nl;
}

LineDef make_line_perpendicular(LineDef *l, PointDef *p) {
  LineDef nl = {
      .ext_mode = L_EXT_LINE,
      .type = LD_PERPENDICULAR,
      .parallel = {.l = l, .p = p},
  };
  init_misc_line_data(&nl);
  return nl;
}

CircleDef make_circle_center_point_outer_point(PointDef *center,
                                               PointDef *outer) {
  CircleDef c = {
      .type = CD_CENTER_POINT_OUTER_POINT,
      .center_point_outer_point = {.center = center, .outer = outer},
  };
  init_misc_circle_data(&c);
  return c;
}

CircleDef make_circle_center_point_radius_seg(PointDef *center,
                                              LineDef *rad_seg) {
  CircleDef c = {
      .type = CD_CENTER_POINT_RADIUS_SEG,
      .center_point_radius_seg = {.center = center, .rad_seg = rad_seg},
  };
  init_misc_circle_data(&c);
  return c;
}
