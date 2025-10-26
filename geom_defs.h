#pragma once

#include <stdbool.h>

typedef struct {
  double x;
  double y;
} Pos2D;

typedef enum {
  DF_DONT_KNOW = 0,
  DF_NO = 1,
  DF_YES = 2,
} DeletionFlag;

typedef enum {
  L_EXT_SEGMENT = 0, //    start -> end
  L_EXT_RAY = 1,     //    start -> end ->
  L_EXT_LINE = 2,    // -> start -> end ->
} LineExtMode;

typedef struct PointDef PointDef;
typedef struct LineDef LineDef;
typedef struct CircleDef CircleDef;

typedef enum {
  PD_LITERAL = 0,
  PD_MIDPOINT = 1,
  PD_GLIDER_ON_LINE = 2,
  PD_INTSEC_LINE_LINE = 3,
  PD_GLIDER_ON_CIRCLE = 4,
  PD_INTSEC_LINE_CIRCLE = 5,
  PD_INTSEC_CIRCLE_CIRCLE = 6,
} PointDefType;

typedef struct {
  Pos2D pos;
} LiteralPD;

typedef struct {
  PointDef *p1;
  PointDef *p2;
} MidpointPD;

typedef struct {
  LineDef *l;
  double prog; // ln.s * (1-p) + ln.e * p
} GliderOnLinePD;

typedef struct {
  LineDef *l1;
  LineDef *l2;
} IntsecLineLinePD;

typedef struct {
  CircleDef *c;
  double prog; // [0.0;1.0)
} GliderOnCirclePD;

typedef enum {
  ILC_PROG_LOWER = 0,
  ILC_PROG_HIGHER = 1,
} ILCProgType;

typedef struct {
  LineDef *l;
  CircleDef *c;
  // there are two intersection points, this chooses one of them
  ILCProgType prog_type;
} IntsecLineCirclePD;

typedef enum {
  ICC_RIGHT = 0,
  ICC_LEFT = 1,
} ICCSide;

typedef struct {
  CircleDef *c1;
  CircleDef *c2;
  // there are two intersection points, this chooses one of them
  ICCSide side;
} IntsecCircleCirclePD;

typedef struct {
  bool dirty;
  bool invalid;
  Pos2D pos;
} PointVal;

struct PointDef {
  PointDefType type;
  union {
    LiteralPD literal;
    MidpointPD midpoint;
    GliderOnLinePD glider_on_line;
    IntsecLineLinePD intsec_line_line;
    GliderOnCirclePD glider_on_circle;
    IntsecLineCirclePD intsec_line_circle;
    IntsecCircleCirclePD intsec_circle_circle;
  };
  PointVal val;
  DeletionFlag del_flag;
  // This value is only ever meant to be set/used inside save functions
  int save_id;
};

typedef enum {
  LD_POINT_TO_POINT = 0,
  LD_PARALLEL = 1,
  LD_PERPENDICULAR = 2,
} LineDefType;

typedef struct {
  PointDef *p1;
  PointDef *p2;
} PointToPointLD;

typedef struct {
  LineDef *l;
  PointDef *p;
} ParallelLD;

typedef struct {
  LineDef *l;
  PointDef *p;
} PerpendicularLD;

typedef struct {
  bool dirty;
  bool invalid;
  Pos2D start;
  Pos2D end;
} LineVal;

struct LineDef {
  LineExtMode ext_mode;
  LineDefType type;
  union {
    PointToPointLD point_to_point;
    ParallelLD parallel;
    PerpendicularLD perpendicular;
  };
  LineVal val;
  DeletionFlag del_flag;
  // This value is only ever meant to be set/used inside save functions
  int save_id;
};

typedef enum {
  CD_CENTER_POINT_OUTER_POINT = 0,
  CD_CENTER_POINT_RADIUS_SEG = 1,
} CircleDefType;

typedef struct {
  PointDef *center;
  PointDef *outer;
} CenterPointOuterPointCD;

typedef struct {
  PointDef *center;
  LineDef *rad_seg;
} CenterPointRadiusSegCD;

typedef struct {
  bool dirty;
  bool invalid;
  Pos2D center;
  double radius;
} CircleVal;

struct CircleDef {
  CircleDefType type;
  union {
    CenterPointOuterPointCD center_point_outer_point;
    CenterPointRadiusSegCD center_point_radius_seg;
  };
  CircleVal val;
  DeletionFlag del_flag;
  // This value is only ever meant to be set/used inside save functions
  int save_id;
};

PointDef make_point_literal(Pos2D pos);
PointDef make_point_midpoint(PointDef *p1, PointDef *p2);
PointDef make_point_glider_on_line(LineDef *l, float prog);
PointDef make_point_intsec_line_line(LineDef *l1, LineDef *l2);
PointDef make_point_glider_on_circle(CircleDef *c, float prog);
PointDef make_point_intsec_line_circle(LineDef *l, CircleDef *c,
                                       ILCProgType prog_type);
PointDef make_point_intsec_circle_circle(CircleDef *c1, CircleDef *c2,
                                         ICCSide side);

LineDef make_line_point_to_point(LineExtMode ext_mode, PointDef *p1,
                                 PointDef *p2);
LineDef make_line_parallel(LineDef *l, PointDef *p);
LineDef make_line_perpendicular(LineDef *l, PointDef *p);

CircleDef make_circle_center_point_outer_point(PointDef *center,
                                               PointDef *outer);
CircleDef make_circle_center_point_radius_seg(PointDef *center,
                                              LineDef *rad_seg);
