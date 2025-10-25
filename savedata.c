#include "savedata.h"
#include "geom.h"
#include <stdio.h>
#include <stdlib.h>

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

void save_to_file(FILE *handle, PointDef *pd_arr[], int pd_n, LineDef *ld_arr[],
                  int ld_n, CircleDef *cd_arr[], int cd_n) {

  // we assign an ID to each element to be saved
  // the IDs only have to be unique type-wise
  for (int i = 0; i < pd_n; i++)
    pd_arr[i]->save_id = i;
  for (int i = 0; i < ld_n; i++)
    ld_arr[i]->save_id = i;
  for (int i = 0; i < cd_n; i++)
    cd_arr[i]->save_id = i;

  for (int i = 0; i < pd_n; i++)
    save_point(handle, pd_arr[i]);
  for (int i = 0; i < ld_n; i++)
    save_line(handle, ld_arr[i]);
  for (int i = 0; i < cd_n; i++)
    save_circle(handle, cd_arr[i]);
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

bool parse_point(char *data, PointDef *pd, SaveData *sd,
                 ReadResult **sorted_p_results, ReadResult **sorted_l_results,
                 ReadResult **sorted_c_results) {
  int type_i;
  if (sscanf(data, "%d", &type_i) != 1)
    return false;
  pd->type = (PointDefType)type_i;
  switch (pd->type) {
  case PD_LITERAL: {
    if (sscanf(data, "%*d %lf %lf", &pd->literal.pos.x, &pd->literal.pos.y) !=
        2)
      return false;
    break;
  }
  case PD_MIDPOINT: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_p_results, sd->p_n);
    int ind2 = id_to_index(id2, sorted_p_results, sd->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    pd->midpoint.p1 = sd->point_defs[ind1];
    pd->midpoint.p2 = sd->point_defs[ind2];
    break;
  }
  case PD_GLIDER_ON_LINE: {
    int id;
    if (sscanf(data, "%*d %d %lf", &id, &pd->glider_on_line.prog) != 2)
      return false;
    int ind = id_to_index(id, sorted_l_results, sd->l_n);
    if (ind == -1)
      return false;
    pd->glider_on_line.l = sd->line_defs[ind];
    break;
  }
  case PD_INTSEC_LINE_LINE: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_l_results, sd->l_n);
    int ind2 = id_to_index(id2, sorted_l_results, sd->l_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    pd->intsec_line_line.l1 = sd->line_defs[ind1];
    pd->intsec_line_line.l2 = sd->line_defs[ind2];
    break;
  }
  case PD_GLIDER_ON_CIRCLE: {
    int id;
    if (sscanf(data, "%*d %d %lf", &id, &pd->glider_on_circle.prog) != 2)
      return false;
    int ind = id_to_index(id, sorted_c_results, sd->c_n);
    if (ind == -1)
      return false;
    pd->glider_on_circle.c = sd->circle_defs[ind];
    break;
  }
  case PD_INTSEC_LINE_CIRCLE: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_l_results, sd->l_n);
    int ind2 = id_to_index(id2, sorted_c_results, sd->c_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    pd->intsec_line_circle.l = sd->line_defs[ind1];
    pd->intsec_line_circle.c = sd->circle_defs[ind2];
    break;
  }
  case PD_INTSEC_CIRCLE_CIRCLE: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_c_results, sd->c_n);
    int ind2 = id_to_index(id2, sorted_c_results, sd->c_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    pd->intsec_circle_circle.c1 = sd->circle_defs[ind1];
    pd->intsec_circle_circle.c2 = sd->circle_defs[ind2];
    break;
  }
  }
  return true;
}

bool parse_line(char *data, LineDef *ld, SaveData *sd,
                ReadResult **sorted_p_results, ReadResult **sorted_l_results,
                ReadResult **sorted_c_results) {

  int ext_mode_i;
  int type_i;
  if (sscanf(data, "%d %d", &ext_mode_i, &type_i) != 2)
    return false;
  ld->ext_mode = (LineExtMode)ext_mode_i;
  ld->type = (LineDefType)type_i;

  switch (ld->type) {
  case LD_POINT_TO_POINT: {
    int id1, id2;
    if (sscanf(data, "%*d %*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_p_results, sd->p_n);
    int ind2 = id_to_index(id2, sorted_p_results, sd->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    ld->point_to_point.p1 = sd->point_defs[ind1];
    ld->point_to_point.p2 = sd->point_defs[ind2];
    break;
  }
  case LD_PARALLEL: {
    int id1, id2;
    if (sscanf(data, "%*d %*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_l_results, sd->l_n);
    int ind2 = id_to_index(id2, sorted_p_results, sd->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    ld->parallel.l = sd->line_defs[ind1];
    ld->parallel.p = sd->point_defs[ind2];
    break;
  }
  case LD_PERPENDICULAR: {
    int id1, id2;
    if (sscanf(data, "%*d %*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_l_results, sd->l_n);
    int ind2 = id_to_index(id2, sorted_p_results, sd->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    ld->perpendicular.l = sd->line_defs[ind1];
    ld->perpendicular.p = sd->point_defs[ind2];
    break;
  }
  }
  return true;
}

bool parse_circle(char *data, CircleDef *cd, SaveData *sd,
                  ReadResult **sorted_p_results, ReadResult **sorted_l_results,
                  ReadResult **sorted_c_results) {
  int type_i;
  if (sscanf(data, "%d", &type_i) != 1)
    return false;
  cd->type = (CircleDefType)type_i;

  switch (cd->type) {
  case CD_CENTER_POINT_OUTER_POINT: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_p_results, sd->p_n);
    int ind2 = id_to_index(id2, sorted_p_results, sd->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    cd->center_point_outer_point.center = sd->point_defs[ind1];
    cd->center_point_outer_point.outer = sd->point_defs[ind2];
    break;
  }
  case CD_CENTER_POINT_RADIUS_SEG: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, sorted_p_results, sd->p_n);
    int ind2 = id_to_index(id2, sorted_l_results, sd->l_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    cd->center_point_radius_seg.center = sd->point_defs[ind1];
    cd->center_point_radius_seg.rad_seg = sd->line_defs[ind2];
    break;
  }
  }
  return true;
}

bool load_from_file(FILE *handle, SaveData *sd) {
  ReadResult p_results[100], l_results[100], c_results[100];
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
    ReadResult *read_result;
    switch (type) {
    case 'p':
      read_result = &p_results[p_n++];
      break;
    case 'l':
      read_result = &l_results[l_n++];
      break;
    case 'c':
      read_result = &c_results[c_n++];
      break;
    default:
      printf("Unknown elem type\n");
      return false;
    }
    read_result->id = id;
    char *ptr = fgets(read_result->rest, 256, handle);
    // TODO: handle if line was too long!
    if (ptr == NULL) {
      printf("Error: fgets read nothing\n");
      return false;
    }
  }
  for (int i = 0; i < p_n; i++) {
    ReadResult *rr = &p_results[i];
    printf("Point: id: %d, rest: \"%s\"\n", rr->id, rr->rest);
  }
  for (int i = 0; i < l_n; i++) {
    ReadResult *rr = &l_results[i];
    printf("Line: id: %d, rest: \"%s\"\n", rr->id, rr->rest);
  }
  for (int i = 0; i < c_n; i++) {
    ReadResult *rr = &c_results[i];
    printf("Circle: id: %d, rest: \"%s\"\n", rr->id, rr->rest);
  }

  ReadResult *sorted_p_results[100];
  ReadResult *sorted_l_results[100];
  ReadResult *sorted_c_results[100];

  for (int i = 0; i < p_n; i++)
    sorted_p_results[i] = &p_results[i];
  for (int i = 0; i < l_n; i++)
    sorted_l_results[i] = &l_results[i];
  for (int i = 0; i < c_n; i++)
    sorted_c_results[i] = &c_results[i];

  sort_by_id(sorted_p_results, p_n);
  sort_by_id(sorted_l_results, l_n);
  sort_by_id(sorted_c_results, c_n);

  *sd = (SaveData){.point_defs = {},
                   .line_defs = {},
                   .circle_defs = {},
                   .p_n = p_n,
                   .l_n = l_n,
                   .c_n = c_n};

  for (int i = 0; i < p_n; i++)
    sd->point_defs[i] = (PointDef *)malloc(sizeof(PointDef));
  for (int i = 0; i < l_n; i++)
    sd->line_defs[i] = (LineDef *)malloc(sizeof(LineDef));
  for (int i = 0; i < c_n; i++)
    sd->circle_defs[i] = (CircleDef *)malloc(sizeof(CircleDef));

  bool ok = true;
  for (int i = 0; ok && i < p_n; i++) {
    printf("p i:%d\n", i);
    ok = parse_point(sorted_p_results[i]->rest, sd->point_defs[i], sd,
                     sorted_p_results, sorted_l_results, sorted_c_results);
  }
  for (int i = 0; ok && i < l_n; i++) {
    printf("l i:%d\n", i);
    ok = parse_line(sorted_l_results[i]->rest, sd->line_defs[i], sd,
                    sorted_p_results, sorted_l_results, sorted_c_results);
  }

  for (int i = 0; ok && i < c_n; i++) {
    printf("c i:%d\n", i);
    ok = parse_circle(sorted_c_results[i]->rest, sd->circle_defs[i], sd,
                      sorted_p_results, sorted_l_results, sorted_c_results);
  }

  if (!ok) {
    for (int i = 0; i < p_n; i++) {
      free(sd->point_defs[i]);
    }
    for (int i = 0; i < l_n; i++) {
      free(sd->line_defs[i]);
    }
    for (int i = 0; i < c_n; i++) {
      free(sd->circle_defs[i]);
    }
    return false;
  }

  return true;
}
