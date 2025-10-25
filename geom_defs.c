#include "geom_defs.h"

PointDef make_point_literal(Pos2D pos) {
  return (PointDef){
      .type = PD_LITERAL,
      .literal = {.pos = pos},
      .val = {.dirty = true},
  };
}
PointDef make_point_midpoint(PointDef *p1, PointDef *p2) {
  return (PointDef){
      .type = PD_MIDPOINT,
      .midpoint = {.p1 = p1, .p2 = p2},
      .val = {.dirty = true},
  };
}
PointDef make_point_glider_on_line(LineDef *l, float prog) {
  return (PointDef){
      .type = PD_GLIDER_ON_LINE,
      .glider_on_line = {.l = l, .prog = prog},
      .val = {.dirty = true},
  };
}
PointDef make_point_intsec_line_line(LineDef *l1, LineDef *l2) {
  return (PointDef){
      .type = PD_INTSEC_LINE_LINE,
      .intsec_line_line = {.l1 = l1, .l2 = l2},
      .val = {.dirty = true},
  };
}
PointDef make_point_glider_on_circle(CircleDef *c, float prog) {
  return (PointDef){
      .type = PD_GLIDER_ON_CIRCLE,
      .glider_on_circle = {.c = c, .prog = prog},
      .val = {.dirty = true},
  };
}
PointDef make_point_intsec_line_circle(LineDef *l, CircleDef *c) {
  return (PointDef){
      .type = PD_INTSEC_LINE_CIRCLE,
      .intsec_line_circle = {.l = l, .c = c},
      .val = {.dirty = true},
  };
}
PointDef make_point_intsec_circle_circle(CircleDef *c1, CircleDef *c2) {
  return (PointDef){
      .type = PD_INTSEC_CIRCLE_CIRCLE,
      .intsec_circle_circle = {.c1 = c1, .c2 = c2},
      .val = {.dirty = true},
  };
}

LineDef make_line_point_to_point(LineExtMode ext_mode, PointDef *p1,
                                 PointDef *p2) {
  return (LineDef){
      .ext_mode = ext_mode,
      .type = LD_POINT_TO_POINT,
      .point_to_point = {.p1 = p1, .p2 = p2},
      .val = {.dirty = true},
  };
}

LineDef make_line_parallel(LineDef *l, PointDef *p) {
  return (LineDef){
      .ext_mode = L_EXT_LINE,
      .type = LD_PARALLEL,
      .parallel = {.l = l, .p = p},
      .val = {.dirty = true},
  };
}
LineDef make_line_perpendicular(LineDef *l, PointDef *p) {
  return (LineDef){
      .ext_mode = L_EXT_LINE,
      .type = LD_PERPENDICULAR,
      .parallel = {.l = l, .p = p},
      .val = {.dirty = true},
  };
}

CircleDef make_circle_center_point_outer_point(PointDef *center,
                                               PointDef *outer) {
  return (CircleDef){
      .type = CD_CENTER_POINT_OUTER_POINT,
      .center_point_outer_point = {.center = center, .outer = outer},
      .val = {.dirty = true},
  };
}
CircleDef make_circle_center_point_radius_seg(PointDef *center,
                                              LineDef *rad_seg) {
  return (CircleDef){
      .type = CD_CENTER_POINT_RADIUS_SEG,
      .center_point_radius_seg = {.center = center, .rad_seg = rad_seg},
      .val = {.dirty = true},
  };
}
