#include "savedata.h"

void save_point(FILE *handle, PointDef *pd) {
  fprintf(handle, "p %d %d ", pd->id, pd->type);
  switch (pd->type) {
  case PD_LITERAL:
    fprintf(handle, "%.17lf %.17lf", /**/
            pd->literal.pos.x, pd->literal.pos.y);
    break;
  case PD_MIDPOINT:
    fprintf(handle, "%d %d", /**/
            pd->midpoint.p1->id, pd->midpoint.p2->id);
    break;
  case PD_GLIDER_ON_LINE:
    fprintf(handle, "%d %.17lf", /**/
            pd->glider_on_line.l->id, pd->glider_on_line.prog);
    break;
  case PD_INTSEC_LINE_LINE:
    fprintf(handle, "%d %d", /**/
            pd->intsec_line_line.l1->id, pd->intsec_line_line.l2->id);
    break;
  case PD_GLIDER_ON_CIRCLE:
    fprintf(handle, "%d %.17lf", /**/
            pd->glider_on_circle.c->id, pd->glider_on_line.prog);
    break;
  case PD_INTSEC_LINE_CIRCLE:
    fprintf(handle, "%d %d", /**/
            pd->intsec_line_circle.l->id, pd->intsec_line_circle.c->id);
    break;
  case PD_INTSEC_CIRCLE_CIRCLE:
    fprintf(handle, "%d %d", /**/
            pd->intsec_circle_circle.c1->id, pd->intsec_circle_circle.c2->id);
    break;
  }
  fprintf(handle, "\n");
}

void save_line(FILE *handle, LineDef *ld) {
  fprintf(handle, "l %d %d ", ld->id, ld->type);
  switch (ld->type) {
  case LD_POINT_TO_POINT:
    fprintf(handle, "%d %d", /**/
            ld->point_to_point.p1->id, ld->point_to_point.p2->id);
    break;
  case LD_PARALLEL:
    fprintf(handle, "%d %d", /**/
            ld->parallel.l->id, ld->parallel.p->id);
    break;
  case LD_PERPENDICULAR:
    fprintf(handle, "%d %d", /**/
            ld->parallel.l->id, ld->parallel.p->id);
    break;
  }
  fprintf(handle, "\n");
}

void save_circle(FILE *handle, CircleDef *cd) {
  fprintf(handle, "c %d %d ", cd->id, cd->type);
  switch (cd->type) {
  case CD_CENTER_POINT_OUTER_POINT:
    fprintf(handle, "%d %d", /**/
            cd->center_point_outer_point.center->id,
            cd->center_point_outer_point.outer->id);
    break;
  case CD_CENTER_POINT_RADIUS_SEG:
    fprintf(handle, "%d %d", /**/
            cd->center_point_radius_seg.center->id,
            cd->center_point_radius_seg.rad_seg->id);
    break;
  }
  fprintf(handle, "\n");
}

void save_to_file(FILE *handle, PointDef *pd_arr[], int pd_n, LineDef *ld_arr[],
                  int ld_n, CircleDef *cd_arr[], int cd_n) {
  for (int i = 0; i < pd_n; i++) {
    save_point(handle, pd_arr[i]);
  }
  for (int i = 0; i < ld_n; i++) {
    save_line(handle, ld_arr[i]);
  }
  for (int i = 0; i < cd_n; i++) {
    save_circle(handle, cd_arr[i]);
  }
}
