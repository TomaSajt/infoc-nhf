#include "debugmalloc.h"
#include "geom_defs.h"

#include <stdio.h>
#include <stdlib.h>

#include "savedata.h"

void save_point(FILE *handle, PointDef *pd) {
  fprintf(handle, "p %d %d ", pd->save_id, pd->type);
  switch (pd->type) {
  case PD_LITERAL:
    fprintf(handle, "%.17lf %.17lf", /**/
            pd->literal.pos.x, pd->literal.pos.y);
    break;
  case PD_MIDPOINT:
    fprintf(handle, "%d %d", /**/
            pd->midpoint.p1->save_id, pd->midpoint.p2->save_id);
    break;
  case PD_GLIDER_ON_LINE:
    fprintf(handle, "%d %.17lf", /**/
            pd->glider_on_line.l->save_id, pd->glider_on_line.prog);
    break;
  case PD_INTSEC_LINE_LINE:
    fprintf(handle, "%d %d", /**/
            pd->intsec_line_line.l1->save_id, pd->intsec_line_line.l2->save_id);
    break;
  case PD_GLIDER_ON_CIRCLE:
    fprintf(handle, "%d %.17lf", /**/
            pd->glider_on_circle.c->save_id, pd->glider_on_line.prog);
    break;
  case PD_INTSEC_LINE_CIRCLE:
    fprintf(handle, "%d %d", /**/
            pd->intsec_line_circle.l->save_id,
            pd->intsec_line_circle.c->save_id);
    break;
  case PD_INTSEC_CIRCLE_CIRCLE:
    fprintf(handle, "%d %d", /**/
            pd->intsec_circle_circle.c1->save_id,
            pd->intsec_circle_circle.c2->save_id);
    break;
  }
  fprintf(handle, "\n");
}

void save_line(FILE *handle, LineDef *ld) {
  fprintf(handle, "l %d %d %d ", ld->save_id, ld->ext_mode, ld->type);
  switch (ld->type) {
  case LD_POINT_TO_POINT:
    fprintf(handle, "%d %d", /**/
            ld->point_to_point.p1->save_id, ld->point_to_point.p2->save_id);
    break;
  case LD_PARALLEL:
    fprintf(handle, "%d %d", /**/
            ld->parallel.l->save_id, ld->parallel.p->save_id);
    break;
  case LD_PERPENDICULAR:
    fprintf(handle, "%d %d", /**/
            ld->parallel.l->save_id, ld->parallel.p->save_id);
    break;
  }
  fprintf(handle, "\n");
}

void save_circle(FILE *handle, CircleDef *cd) {
  fprintf(handle, "c %d %d ", cd->save_id, cd->type);
  switch (cd->type) {
  case CD_CENTER_POINT_OUTER_POINT:
    fprintf(handle, "%d %d", /**/
            cd->center_point_outer_point.center->save_id,
            cd->center_point_outer_point.outer->save_id);
    break;
  case CD_CENTER_POINT_RADIUS_SEG:
    fprintf(handle, "%d %d", /**/
            cd->center_point_radius_seg.center->save_id,
            cd->center_point_radius_seg.rad_seg->save_id);
    break;
  }
  fprintf(handle, "\n");
}

void save_to_file(FILE *handle, GeometryState *gs) {
  // we assign an ID to each element to be saved
  // the IDs only have to be unique type-wise
  for (int i = 0; i < gs->p_n; i++)
    gs->point_defs[i]->save_id = i;
  for (int i = 0; i < gs->l_n; i++)
    gs->line_defs[i]->save_id = i;
  for (int i = 0; i < gs->c_n; i++)
    gs->circle_defs[i]->save_id = i;

  for (int i = 0; i < gs->p_n; i++)
    save_point(handle, gs->point_defs[i]);
  for (int i = 0; i < gs->l_n; i++)
    save_line(handle, gs->line_defs[i]);
  for (int i = 0; i < gs->c_n; i++)
    save_circle(handle, gs->circle_defs[i]);
}

typedef struct {
  int id;
  char rest[256];
} ReadResult;

int id_cmp(const void *a, const void *b) {
  ReadResult *aa = *(ReadResult **)a;
  ReadResult *bb = *(ReadResult **)b;
  return aa->id - bb->id;
}

void sort_by_id(ReadResult **sorted_results, int n) {
  qsort(sorted_results, n, sizeof(ReadResult *), id_cmp);
}

int id_rr_cmp(const void *key, const void *elem) {
  int id = *(const int *)key;            // key is int*
  ReadResult *rr = *(ReadResult **)elem; // elem is Person*
  return id - rr->id;
}

int id_to_index(int id, ReadResult **sorted_results, int n) {
  ReadResult **rr =
      bsearch(&id, sorted_results, n, sizeof(ReadResult **), id_rr_cmp);
  if (rr == NULL)
    return -1;
  return rr - sorted_results;
}

bool parse_point(char *data, PointDef *pd, GeometryState *gs,
                 ReadResult **sorted_p_results, ReadResult **sorted_l_results,
                 ReadResult **sorted_c_results) {
  int type_i;
  if (sscanf(data, "%d", &type_i) != 1)
    return false;

  switch ((PointDefType)type_i) {
  case PD_LITERAL: {
    Pos2D pos;
    if (sscanf(data, "%*d %lf %lf", &pos.x, &pos.y) != 2)
      return false;
    *pd = make_point_literal(pos);
    return true;
  }
  case PD_MIDPOINT: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_p_results, gs->p_n);
    int ind2 = id_to_index(id2, sorted_p_results, gs->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *pd = make_point_midpoint(gs->point_defs[ind1], gs->point_defs[ind2]);
    return true;
  }
  case PD_GLIDER_ON_LINE: {
    int id;
    double prog;
    if (sscanf(data, "%*d %d %lf", &id, &prog) != 2)
      return false;
    int ind = id_to_index(id, sorted_l_results, gs->l_n);
    if (ind == -1)
      return false;
    *pd = make_point_glider_on_line(gs->line_defs[ind], prog);
    return true;
  }
  case PD_INTSEC_LINE_LINE: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_l_results, gs->l_n);
    int ind2 = id_to_index(id2, sorted_l_results, gs->l_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *pd = make_point_intsec_line_line(gs->line_defs[ind1], gs->line_defs[ind2]);
    return true;
  }
  case PD_GLIDER_ON_CIRCLE: {
    int id;
    double prog;
    if (sscanf(data, "%*d %d %lf", &id, &prog) != 2)
      return false;
    int ind = id_to_index(id, sorted_c_results, gs->c_n);
    if (ind == -1)
      return false;

    *pd = make_point_glider_on_circle(gs->circle_defs[ind], prog);
    return true;
  }
  case PD_INTSEC_LINE_CIRCLE: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_l_results, gs->l_n);
    int ind2 = id_to_index(id2, sorted_c_results, gs->c_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *pd = make_point_intsec_line_circle(gs->line_defs[ind1],
                                        gs->circle_defs[ind2]);
    return true;
  }
  case PD_INTSEC_CIRCLE_CIRCLE: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_c_results, gs->c_n);
    int ind2 = id_to_index(id2, sorted_c_results, gs->c_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *pd = make_point_intsec_circle_circle(gs->circle_defs[ind1],
                                          gs->circle_defs[ind2]);
    return true;
  }
  }
  // type_i was not any of the expected enum values
  return false;
}

bool parse_line(char *data, LineDef *ld, GeometryState *gs,
                ReadResult **sorted_p_results, ReadResult **sorted_l_results,
                ReadResult **sorted_c_results) {
  int ext_mode_i;
  int type_i;
  if (sscanf(data, "%d %d", &ext_mode_i, &type_i) != 2)
    return false;

  LineExtMode ext_mode = (LineExtMode)ext_mode_i;

  switch ((LineDefType)type_i) {
  case LD_POINT_TO_POINT: {
    if (ext_mode != L_EXT_SEGMENT && ext_mode != L_EXT_RAY &&
        ext_mode != L_EXT_LINE)
      return false;

    int id1, id2;
    if (sscanf(data, "%*d %*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_p_results, gs->p_n);
    int ind2 = id_to_index(id2, sorted_p_results, gs->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;

    *ld = make_line_point_to_point(ext_mode, gs->point_defs[ind1],
                                   gs->point_defs[ind2]);
    return true;
  }
  case LD_PARALLEL: {
    if (ext_mode != L_EXT_LINE)
      return false;

    int id1, id2;
    if (sscanf(data, "%*d %*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_l_results, gs->l_n);
    int ind2 = id_to_index(id2, sorted_p_results, gs->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *ld = make_line_parallel(gs->line_defs[ind1], gs->point_defs[ind2]);
    return true;
  }
  case LD_PERPENDICULAR: {
    if (ext_mode != L_EXT_LINE)
      return false;

    int id1, id2;
    if (sscanf(data, "%*d %*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_l_results, gs->l_n);
    int ind2 = id_to_index(id2, sorted_p_results, gs->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *ld = make_line_perpendicular(gs->line_defs[ind1], gs->point_defs[ind2]);
    return true;
  }
  }
  // type_i was not any of the expected enum values
  return false;
}

bool parse_circle(char *data, CircleDef *cd, GeometryState *gs,
                  ReadResult **sorted_p_results, ReadResult **sorted_l_results,
                  ReadResult **sorted_c_results) {
  int type_i;
  if (sscanf(data, "%d", &type_i) != 1)
    return false;

  switch ((CircleDefType)type_i) {
  case CD_CENTER_POINT_OUTER_POINT: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_p_results, gs->p_n);
    int ind2 = id_to_index(id2, sorted_p_results, gs->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *cd = make_circle_center_point_outer_point(gs->point_defs[ind1],
                                               gs->point_defs[ind2]);
    return true;
  }
  case CD_CENTER_POINT_RADIUS_SEG: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_p_results, gs->p_n);
    int ind2 = id_to_index(id2, sorted_l_results, gs->l_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    cd->center_point_radius_seg.center = gs->point_defs[ind1];
    cd->center_point_radius_seg.rad_seg = gs->line_defs[ind2];
    *cd = make_circle_center_point_radius_seg(gs->point_defs[ind1],
                                              gs->line_defs[ind2]);
    return true;
  }
  }
  // type_i was not any of the expected enum values
  return false;
}

bool load_from_file(FILE *handle, GeometryState *gs) {
  ReadResult p_rrs[100], l_rrs[100], c_rrs[100];
  int p_n = 0, l_n = 0, c_n = 0;
  while (true) {
    char type;
    int id;
    int res = fscanf(handle, "%c %d ", &type, &id);
    if (res == EOF)
      break;
    if (res != 2) {
      printf("Error: res was: %d\n", res);
      return false;
    }
    ReadResult *rr;
    switch (type) {
    case 'p':
      rr = &p_rrs[p_n++];
      break;
    case 'l':
      rr = &l_rrs[l_n++];
      break;
    case 'c':
      rr = &c_rrs[c_n++];
      break;
    default:
      printf("Unknown elem type\n");
      return false;
    }
    rr->id = id;
    char *ptr = fgets(rr->rest, 256, handle);
    // TODO: handle if line was too long!
    if (ptr == NULL) {
      printf("Error: fgets read nothing\n");
      return false;
    }
  }

  ReadResult *srt_p_rrs[100];
  ReadResult *srt_l_rrs[100];
  ReadResult *srt_c_rrs[100];

  for (int i = 0; i < p_n; i++)
    srt_p_rrs[i] = &p_rrs[i];
  for (int i = 0; i < l_n; i++)
    srt_l_rrs[i] = &l_rrs[i];
  for (int i = 0; i < c_n; i++)
    srt_c_rrs[i] = &c_rrs[i];

  sort_by_id(srt_p_rrs, p_n);
  sort_by_id(srt_l_rrs, l_n);
  sort_by_id(srt_c_rrs, c_n);

  *gs = (GeometryState){
      .point_defs = {},
      .line_defs = {},
      .circle_defs = {},
      .p_n = p_n,
      .l_n = l_n,
      .c_n = c_n,
  };

  for (int i = 0; i < p_n; i++) {
    gs->point_defs[i] = (PointDef *)malloc(sizeof(PointDef));
  }
  for (int i = 0; i < l_n; i++) {
    gs->line_defs[i] = (LineDef *)malloc(sizeof(LineDef));
  }
  for (int i = 0; i < c_n; i++) {
    gs->circle_defs[i] = (CircleDef *)malloc(sizeof(CircleDef));
  }

  bool ok = true;
  for (int i = 0; ok && i < p_n; i++) {
    ok = parse_point(srt_p_rrs[i]->rest, gs->point_defs[i], gs, srt_p_rrs,
                     srt_l_rrs, srt_c_rrs);
  }
  for (int i = 0; ok && i < l_n; i++) {
    ok = parse_line(srt_l_rrs[i]->rest, gs->line_defs[i], gs, srt_p_rrs,
                    srt_l_rrs, srt_c_rrs);
  }

  for (int i = 0; ok && i < c_n; i++) {
    ok = parse_circle(srt_c_rrs[i]->rest, gs->circle_defs[i], gs, srt_p_rrs,
                      srt_l_rrs, srt_c_rrs);
  }

  if (!ok) {
    for (int i = 0; i < p_n; i++) {
      free(gs->point_defs[i]);
    }
    for (int i = 0; i < l_n; i++) {
      free(gs->line_defs[i]);
    }
    for (int i = 0; i < c_n; i++) {
      free(gs->circle_defs[i]);
    }
    return false;
  }

  return true;
}
