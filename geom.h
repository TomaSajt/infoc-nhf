typedef struct {
  double x;
  double y;
} Pos2D;

typedef struct PointProvider PointProvider;
typedef struct LineProvider LineProvider;

typedef struct {
  Pos2D pos;
} Point;

typedef enum {
  PP_LITERAL = 0,
  PP_MIDPOINT = 1,
  PP_ON_LINE = 2,
  PP_LINE_LINE_INTERSECTION = 3,
} PointProviderType;

typedef struct {
  Pos2D pos;
} LiteralPP;

typedef struct {
  PointProvider *pt1;
  PointProvider *pt2;
} MidpointPP;

typedef struct {
  LineProvider *ln;
  double progress;
} OnLinePP;

typedef struct {
  LineProvider *ln1;
  LineProvider *ln2;
} LineLineIntersectionPP;

struct PointProvider {
  int type;
  union {
    LiteralPP literal;
    MidpointPP midpoint;
    OnLinePP on_line;
    LineLineIntersectionPP line_line_intersection;
  };
};

typedef enum {
  LP_POINT_POINT = 0,
  LP_PARALLEL = 1,
  LP_PERPENDICULAR = 2
} LineProviderType;

typedef struct {
  PointProvider *pt1;
  PointProvider *pt2;
} PointPointLP;

typedef struct {
  LineProvider *ln;
  PointProvider *pt;
} ParallelLP;

typedef struct {
  LineProvider *ln;
  PointProvider *pt;
} PerpendicularLP;

struct LineProvider {
  int type;
  union {
    PointPointLP point_point;
    ParallelLP parallel;
    PerpendicularLP PerpendicularLP;
  };
};
