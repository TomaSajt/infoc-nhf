#include "data.h"
#include "../geom/defs.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    fprintf(handle, "%d %d %d", /**/
            pd->intsec_line_circle.l->save_id,
            pd->intsec_line_circle.c->save_id,
            pd->intsec_line_circle.prog_type);
    break;
  case PD_INTSEC_CIRCLE_CIRCLE:
    fprintf(handle, "%d %d %d", /**/
            pd->intsec_circle_circle.c1->save_id,
            pd->intsec_circle_circle.c2->save_id,
            pd->intsec_circle_circle.side);
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

void save_to_file(FILE *handle, GeometryState const *gs) {
  // we assign an ID to each element to be saved
  // the IDs only have to be unique type-wise
  {
    int id = 0;
    for (GenericElemList *curr = gs->pd_list; curr != NULL; curr = curr->next)
      curr->pd->save_id = id++;
  }
  {
    int id = 0;
    for (GenericElemList *curr = gs->ld_list; curr != NULL; curr = curr->next)
      curr->ld->save_id = id++;
  }
  {
    int id = 0;
    for (GenericElemList *curr = gs->cd_list; curr != NULL; curr = curr->next)
      curr->cd->save_id = id++;
  }

  for (GenericElemList *curr = gs->pd_list; curr != NULL; curr = curr->next)
    save_point(handle, curr->pd);
  for (GenericElemList *curr = gs->ld_list; curr != NULL; curr = curr->next)
    save_line(handle, curr->ld);
  for (GenericElemList *curr = gs->cd_list; curr != NULL; curr = curr->next)
    save_circle(handle, curr->cd);
}

typedef struct ReadResult ReadResult;
struct ReadResult {
  int id;
  char rest[256]; // a line in the save file is not allowed to be longer
  ReadResult *next;
};

int id_cmp(void const *a, void const *b) {
  ReadResult *aa = *(ReadResult **)a;
  ReadResult *bb = *(ReadResult **)b;
  return aa->id - bb->id;
}

void sort_by_id(ReadResult **sorted_results, int n) {
  qsort(sorted_results, n, sizeof(ReadResult *), id_cmp);
}

bool make_pointer_array_from_list(ReadResult *list, int n,
                                  ReadResult ***array_out) {
  *array_out = NULL;

  ReadResult **array = (ReadResult **)malloc(sizeof(ReadResult *) * n);
  if (n > 0 && array == NULL) {
    return false;
  }

  ReadResult *curr = list;
  for (int i = 0; i < n; i++) {
    assert(curr != NULL);
    array[i] = curr;
    curr = curr->next;
  }
  assert(curr == NULL);

  *array_out = array;
  return true;
}

// creates an array of list nodes of a length n list
// the contents of the list nodes are not initialized, only `next` is set
bool make_elem_holder(int n, GenericElemList ***array_out) {
  *array_out = NULL;

  GenericElemList **array =
      (GenericElemList **)malloc(sizeof(GenericElemList *) * n);
  if (n > 0 && array == NULL) {
    return false;
  }

  bool ok = true;
  for (int i = 0; i < n; i++) {
    array[i] = (GenericElemList *)malloc(sizeof(GenericElemList));
    if (array[i] == NULL)
      ok = false;
  }

  if (!ok) {
    for (int i = 0; i < n; i++)
      free(array[i]);
    free(array);
    return false;
  }

  if (n > 0) {
    for (int i = 0; i < n - 1; i++)
      array[i]->next = array[i + 1];
    array[n - 1]->next = NULL;
  }

  *array_out = array;
  return true;
}

int id_rr_cmp(void const *key, void const *elem) {
  int id = *(int *)key;                  // key is int*
  ReadResult *rr = *(ReadResult **)elem; // elem is ReadResult*
  return id - rr->id;
}

int id_to_index(int id, ReadResult **sorted_results, int n) {
  ReadResult **rr =
      bsearch(&id, sorted_results, n, sizeof(ReadResult **), id_rr_cmp);
  if (rr == NULL)
    return -1;
  return rr - sorted_results;
}

typedef struct {
  GenericElemList **point_defs_holder;
  GenericElemList **line_defs_holder;
  GenericElemList **circle_defs_holder;
  ReadResult **ord_p_results;
  ReadResult **ord_l_results;
  ReadResult **ord_c_results;
  int p_n;
  int l_n;
  int c_n;
} ParseContext;

bool parse_point(char *data, PointDef *pd, ParseContext const *ctx) {
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
    int ind1 = id_to_index(id1, ctx->ord_p_results, ctx->p_n);
    int ind2 = id_to_index(id2, ctx->ord_p_results, ctx->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *pd = make_point_midpoint(ctx->point_defs_holder[ind1]->pd,
                              ctx->point_defs_holder[ind2]->pd);
    return true;
  }
  case PD_GLIDER_ON_LINE: {
    int id;
    double prog;
    if (sscanf(data, "%*d %d %lf", &id, &prog) != 2)
      return false;
    int ind = id_to_index(id, ctx->ord_l_results, ctx->l_n);
    if (ind == -1)
      return false;
    *pd = make_point_glider_on_line(ctx->line_defs_holder[ind]->ld, prog);
    return true;
  }
  case PD_INTSEC_LINE_LINE: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, ctx->ord_l_results, ctx->l_n);
    int ind2 = id_to_index(id2, ctx->ord_l_results, ctx->l_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *pd = make_point_intsec_line_line(ctx->line_defs_holder[ind1]->ld,
                                      ctx->line_defs_holder[ind2]->ld);
    return true;
  }
  case PD_GLIDER_ON_CIRCLE: {
    int id;
    double prog;
    if (sscanf(data, "%*d %d %lf", &id, &prog) != 2)
      return false;
    int ind = id_to_index(id, ctx->ord_c_results, ctx->c_n);
    if (ind == -1)
      return false;

    *pd = make_point_glider_on_circle(ctx->circle_defs_holder[ind]->cd, prog);
    return true;
  }
  case PD_INTSEC_LINE_CIRCLE: {
    int id1, id2;
    int prog_type_i;
    if (sscanf(data, "%*d %d %d %d", &id1, &id2, &prog_type_i) != 3)
      return false;
    ILCProgType prog_type = (ILCProgType)prog_type_i;
    if (prog_type != ILC_PROG_LOWER && prog_type != ILC_PROG_HIGHER)
      return false;
    int ind1 = id_to_index(id1, ctx->ord_l_results, ctx->l_n);
    int ind2 = id_to_index(id2, ctx->ord_c_results, ctx->c_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *pd = make_point_intsec_line_circle(ctx->line_defs_holder[ind1]->ld,
                                        ctx->circle_defs_holder[ind2]->cd,
                                        prog_type);
    return true;
  }
  case PD_INTSEC_CIRCLE_CIRCLE: {
    int id1, id2;
    int side_i;
    if (sscanf(data, "%*d %d %d %d", &id1, &id2, &side_i) != 3)
      return false;

    ICCSide side = (ICCSide)side_i;
    if (side != ICC_LEFT && side != ICC_RIGHT)
      return false;
    int ind1 = id_to_index(id1, ctx->ord_c_results, ctx->c_n);
    int ind2 = id_to_index(id2, ctx->ord_c_results, ctx->c_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *pd = make_point_intsec_circle_circle(ctx->circle_defs_holder[ind1]->cd,
                                          ctx->circle_defs_holder[ind2]->cd,
                                          side);
    return true;
  }
  }
  // type_i was not any of the expected enum values
  return false;
}

bool parse_line(char *data, LineDef *ld, ParseContext const *ctx) {
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
    int ind1 = id_to_index(id1, ctx->ord_p_results, ctx->p_n);
    int ind2 = id_to_index(id2, ctx->ord_p_results, ctx->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;

    *ld = make_line_point_to_point(ext_mode, ctx->point_defs_holder[ind1]->pd,
                                   ctx->point_defs_holder[ind2]->pd);
    return true;
  }
  case LD_PARALLEL: {
    if (ext_mode != L_EXT_LINE)
      return false;

    int id1, id2;
    if (sscanf(data, "%*d %*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, ctx->ord_l_results, ctx->l_n);
    int ind2 = id_to_index(id2, ctx->ord_p_results, ctx->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *ld = make_line_parallel(ctx->line_defs_holder[ind1]->ld,
                             ctx->point_defs_holder[ind2]->pd);
    return true;
  }
  case LD_PERPENDICULAR: {
    if (ext_mode != L_EXT_LINE)
      return false;

    int id1, id2;
    if (sscanf(data, "%*d %*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, ctx->ord_l_results, ctx->l_n);
    int ind2 = id_to_index(id2, ctx->ord_p_results, ctx->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *ld = make_line_perpendicular(ctx->line_defs_holder[ind1]->ld,
                                  ctx->point_defs_holder[ind2]->pd);
    return true;
  }
  }
  // type_i was not any of the expected enum values
  return false;
}

bool parse_circle(char *data, CircleDef *cd, ParseContext const *ctx) {
  int type_i;
  if (sscanf(data, "%d", &type_i) != 1)
    return false;

  switch ((CircleDefType)type_i) {
  case CD_CENTER_POINT_OUTER_POINT: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, ctx->ord_p_results, ctx->p_n);
    int ind2 = id_to_index(id2, ctx->ord_p_results, ctx->p_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *cd = make_circle_center_point_outer_point(
        ctx->point_defs_holder[ind1]->pd, ctx->point_defs_holder[ind2]->pd);
    return true;
  }
  case CD_CENTER_POINT_RADIUS_SEG: {
    int id1, id2;
    if (sscanf(data, "%*d %d %d", &id1, &id2) != 2)
      return false;
    int ind1 = id_to_index(id1, ctx->ord_p_results, ctx->p_n);
    int ind2 = id_to_index(id2, ctx->ord_l_results, ctx->l_n);
    if (ind1 == -1 || ind2 == -1)
      return false;
    *cd = make_circle_center_point_radius_seg(ctx->point_defs_holder[ind1]->pd,
                                              ctx->line_defs_holder[ind2]->ld);
    return true;
  }
  }
  // type_i was not any of the expected enum values
  return false;
}

int get_rr_list_len(ReadResult const *rr_list) {
  int cnt = 0;
  while (rr_list != NULL) {
    cnt++;
    rr_list = rr_list->next;
  }
  return cnt;
}

bool process_rr_lists(ReadResult *p_rr_list, ReadResult *l_rr_list,
                      ReadResult *c_rr_list, GeometryState *gs) {
  bool ret = true;

  int p_n = get_rr_list_len(p_rr_list);
  int l_n = get_rr_list_len(l_rr_list);
  int c_n = get_rr_list_len(c_rr_list);
  printf("%d %d %d\n", p_n, l_n, c_n);

  ReadResult **ord_p_results, **ord_l_results, **ord_c_results;
  ret &= make_pointer_array_from_list(p_rr_list, p_n, &ord_p_results);
  ret &= make_pointer_array_from_list(l_rr_list, l_n, &ord_l_results);
  ret &= make_pointer_array_from_list(c_rr_list, c_n, &ord_c_results);
  if (!ret) {
    goto ord_results_cleanup;
  }

  sort_by_id(ord_p_results, p_n);
  sort_by_id(ord_l_results, l_n);
  sort_by_id(ord_c_results, c_n);

  GenericElemList **point_defs_holder, **line_defs_holder, **circle_defs_holder;
  ret &= make_elem_holder(p_n, &point_defs_holder);
  ret &= make_elem_holder(l_n, &line_defs_holder);
  ret &= make_elem_holder(c_n, &circle_defs_holder);
  if (!ret) {
    goto holder_cleanup;
  }

  for (int i = 0; i < p_n; i++) {
    point_defs_holder[i]->pd = (PointDef *)malloc(sizeof(PointDef));
    if (point_defs_holder[i]->pd == NULL)
      ret = false;
  }
  for (int i = 0; i < l_n; i++) {
    line_defs_holder[i]->ld = (LineDef *)malloc(sizeof(LineDef));
    if (line_defs_holder[i]->ld == NULL)
      ret = false;
  }
  for (int i = 0; i < c_n; i++) {
    circle_defs_holder[i]->cd = (CircleDef *)malloc(sizeof(CircleDef));
    if (circle_defs_holder[i]->cd == NULL)
      ret = false;
  }

  if (!ret) {
    goto defs_cleanup;
  }

  ParseContext ctx = (ParseContext){
      .point_defs_holder = point_defs_holder,
      .line_defs_holder = line_defs_holder,
      .circle_defs_holder = circle_defs_holder,
      .ord_p_results = ord_p_results,
      .ord_l_results = ord_l_results,
      .ord_c_results = ord_c_results,
      .p_n = p_n,
      .l_n = l_n,
      .c_n = c_n,
  };

  for (int i = 0; ret && i < p_n; i++) {
    ret = parse_point(ord_p_results[i]->rest, point_defs_holder[i]->pd, &ctx);
  }
  for (int i = 0; ret && i < l_n; i++) {
    ret = parse_line(ord_l_results[i]->rest, line_defs_holder[i]->ld, &ctx);
  }
  for (int i = 0; ret && i < c_n; i++) {
    ret = parse_circle(ord_c_results[i]->rest, circle_defs_holder[i]->cd, &ctx);
  }

  if (!ret) {
    goto defs_cleanup;
  }

  *gs = (GeometryState){
      .pd_list = p_n > 0 ? point_defs_holder[0] : NULL,
      .ld_list = l_n > 0 ? line_defs_holder[0] : NULL,
      .cd_list = c_n > 0 ? circle_defs_holder[0] : NULL,
  };

defs_cleanup:
  if (!ret) {
    for (int i = 0; i < p_n; i++)
      free(point_defs_holder[i]->pd);
    for (int i = 0; i < l_n; i++)
      free(line_defs_holder[i]->ld);
    for (int i = 0; i < c_n; i++)
      free(circle_defs_holder[i]->cd);
  }

holder_cleanup:
  if (!ret) {
    if (point_defs_holder != NULL) {
      for (int i = 0; i < p_n; i++)
        free(point_defs_holder[i]);
    }
    if (line_defs_holder != NULL) {
      for (int i = 0; i < l_n; i++)
        free(line_defs_holder[i]);
    }
    if (circle_defs_holder != NULL) {
      for (int i = 0; i < c_n; i++)
        free(circle_defs_holder[i]);
    }
  }
  free(point_defs_holder);
  free(line_defs_holder);
  free(circle_defs_holder);

ord_results_cleanup:
  free(ord_p_results);
  free(ord_l_results);
  free(ord_c_results);

  return ret;
}

void free_rr_list(ReadResult *list) {
  while (list != NULL) {
    ReadResult *next = list->next;
    free(list);
    list = next;
  }
}

bool load_from_file(FILE *handle, GeometryState *gs) {
  bool ret = true;

  ReadResult *p_rr_list = NULL, *l_rr_list = NULL, *c_rr_list = NULL;

  while (true) {
    char type;
    int id;
    int scan_res = fscanf(handle, "%c %d ", &type, &id);
    if (scan_res == EOF)
      break;
    if (scan_res != 2) {
      printf("Error: scan_res was: %d\n", scan_res);
      ret = false;
      goto rr_list_cleanup;
    }

    ReadResult **target_list = type == 'p'   ? &p_rr_list
                               : type == 'l' ? &l_rr_list
                               : type == 'c' ? &c_rr_list
                                             : NULL;
    if (target_list == NULL) {
      ret = false;
      goto rr_list_cleanup;
    }

    ReadResult *rr = (ReadResult *)malloc(sizeof(ReadResult));
    if (rr == NULL) {
      ret = false;
      goto rr_list_cleanup;
    }

    rr->id = id;
    rr->next = *target_list;
    *target_list = rr;

    char *ptr = fgets(rr->rest, 256, handle);
    if (ptr == NULL) {
      printf("Error: fgets read nothing\n");
      ret = false;
      goto rr_list_cleanup;
    }

    int len = strlen(rr->rest);
    if (rr->rest[len - 1] != '\n') {
      printf("Error: fgets tried to read a line longer than 256 chars\n");
      ret = false;
      goto rr_list_cleanup;
    }
  }

  ret = process_rr_lists(p_rr_list, l_rr_list, c_rr_list, gs);

rr_list_cleanup:
  free_rr_list(p_rr_list);
  free_rr_list(l_rr_list);
  free_rr_list(c_rr_list);

  return ret;
}
