#pragma once

#include <stdbool.h>

typedef struct {
  double x;
  double y;
} Pos2D;

typedef enum {
  L_EXT_SEGMENT = 0, //    start -> end
  L_EXT_RAY = 1,     //    start -> end ->
  L_EXT_LINE = 2,    // -> start -> end ->
} LineExtMode;

typedef struct {
  bool invalid;
  LineExtMode mode;
  Pos2D start;
  Pos2D end;
} LineEvaled;

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
  PD_INTSEC_CIRCLE_CIRCLE = 6
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
  int id;
};

typedef enum {
  LD_POINT_TO_POINT = 0,
  LD_PARALLEL = 1,
  LD_PERPENDICULAR = 2
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
  int id;
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
  int id;
};

void eval_point(PointDef *pd);
void eval_line(LineDef *ld);
void eval_circle(CircleDef *cd);

double pos_distance(Pos2D *pos1, Pos2D *pos2);
double distance_from_line(Pos2D *pos, Pos2D *start, Pos2D *end);
double distance_from_circle(Pos2D *pos, Pos2D *center, double radius);
