#include "state.h"
#include "defs.h"
#include "util.h"

#include <stdlib.h>

void clear_geometry_state(GeometryState *gs) {
  for (GenericElemList *curr = gs->pd_list; curr != NULL;) {
    GenericElemList *next = curr->next;
    free(curr->pd);
    free(curr);
    curr = next;
  }
  gs->pd_list = NULL;
  for (GenericElemList *curr = gs->ld_list; curr != NULL;) {
    GenericElemList *next = curr->next;
    free(curr->ld);
    free(curr);
    curr = next;
  }
  gs->ld_list = NULL;
  for (GenericElemList *curr = gs->cd_list; curr != NULL;) {
    GenericElemList *next = curr->next;
    free(curr->cd);
    free(curr);
    curr = next;
  }
  gs->cd_list = NULL;
}

bool register_point(GeometryState *gs, PointDef *pd) {
  GenericElemList *elem = (GenericElemList *)malloc(sizeof(GenericElemList));
  if (elem == NULL)
    return false;
  elem->pd = pd;
  elem->next = gs->pd_list;
  gs->pd_list = elem;
  return true;
}

bool register_line(GeometryState *gs, LineDef *ld) {
  GenericElemList *elem = (GenericElemList *)malloc(sizeof(GenericElemList));
  if (elem == NULL)
    return false;
  elem->ld = ld;
  elem->next = gs->ld_list;
  gs->ld_list = elem;
  return true;
}

bool register_circle(GeometryState *gs, CircleDef *cd) {
  GenericElemList *elem = (GenericElemList *)malloc(sizeof(GenericElemList));
  if (elem == NULL)
    return false;
  elem->cd = cd;
  elem->next = gs->cd_list;
  gs->cd_list = elem;
  return true;
}

PointDef *alloc_and_reg_point(GeometryState *gs, PointDef pd) {
  PointDef *alloc_pd = malloc(sizeof(PointDef));
  if (alloc_pd == NULL)
    return NULL;
  *alloc_pd = pd;
  if (!register_point(gs, alloc_pd)) {
    free(alloc_pd);
    return NULL;
  }
  return alloc_pd;
}

LineDef *alloc_and_reg_line(GeometryState *gs, LineDef ld) {
  LineDef *alloc_ld = malloc(sizeof(LineDef));
  if (alloc_ld == NULL)
    return NULL;
  *alloc_ld = ld;
  if (!register_line(gs, alloc_ld)) {
    free(alloc_ld);
    return NULL;
  }
  return alloc_ld;
}

CircleDef *alloc_and_reg_circle(GeometryState *gs, CircleDef cd) {
  CircleDef *alloc_cd = malloc(sizeof(CircleDef));
  if (alloc_cd == NULL)
    return NULL;
  *alloc_cd = cd;
  if (!register_circle(gs, alloc_cd)) {
    free(alloc_cd);
    return NULL;
  }
  return alloc_cd;
}

void mark_everyting_dirty(GeometryState *gs) {
  for (GenericElemList *curr = gs->pd_list; curr != NULL; curr = curr->next)
    curr->pd->val.dirty = true;
  for (GenericElemList *curr = gs->ld_list; curr != NULL; curr = curr->next)
    curr->ld->val.dirty = true;
  for (GenericElemList *curr = gs->cd_list; curr != NULL; curr = curr->next)
    curr->cd->val.dirty = true;
}

void mark_everything_df_dont_know(GeometryState *gs) {

  for (GenericElemList *curr = gs->pd_list; curr != NULL; curr = curr->next)
    curr->pd->del_flag = DF_DONT_KNOW;
  for (GenericElemList *curr = gs->ld_list; curr != NULL; curr = curr->next)
    curr->ld->del_flag = DF_DONT_KNOW;
  for (GenericElemList *curr = gs->cd_list; curr != NULL; curr = curr->next)
    curr->cd->del_flag = DF_DONT_KNOW;
}

void propagate_del_flag(GeometryState *gs) {
  for (GenericElemList *curr = gs->pd_list; curr != NULL; curr = curr->next)
    eval_point_del_flag(curr->pd);
  for (GenericElemList *curr = gs->ld_list; curr != NULL; curr = curr->next)
    eval_line_del_flag(curr->ld);
  for (GenericElemList *curr = gs->cd_list; curr != NULL; curr = curr->next)
    eval_circle_del_flag(curr->cd);
}

void delete_marked_cascading(GeometryState *gs) {
  propagate_del_flag(gs);
  {
    GenericElemList sentinel;
    sentinel.next = gs->pd_list;
    GenericElemList *prev = &sentinel;
    while (prev->next != NULL) {
      GenericElemList *curr = prev->next;
      if (curr->pd->del_flag == DF_YES) {
        prev->next = curr->next;
        free(curr->pd);
        free(curr);
      } else {
        prev = curr;
      }
    }
    gs->pd_list = sentinel.next;
  }
  {
    GenericElemList sentinel;
    sentinel.next = gs->ld_list;
    GenericElemList *prev = &sentinel;
    while (prev->next != NULL) {
      GenericElemList *curr = prev->next;
      if (curr->ld->del_flag == DF_YES) {
        prev->next = curr->next;
        free(curr->ld);
        free(curr);
      } else {
        prev = curr;
      }
    }
    gs->ld_list = sentinel.next;
  }
  {
    GenericElemList sentinel;
    sentinel.next = gs->cd_list;
    GenericElemList *prev = &sentinel;
    while (prev->next != NULL) {
      GenericElemList *curr = prev->next;
      if (curr->cd->del_flag == DF_YES) {
        prev->next = curr->next;
        free(curr->cd);
        free(curr);
      } else {
        prev = curr;
      }
    }
    gs->cd_list = sentinel.next;
  }
  mark_everything_df_dont_know(gs);
}

SDL_Color const delete_mark_color = {.r = 255, .g = 0, .b = 0, .a = 255};

void color_df_cascading_and_reset(GeometryState *gs) {
  propagate_del_flag(gs);
  for (GenericElemList *curr = gs->pd_list; curr != NULL; curr = curr->next)
    if (curr->pd->del_flag == DF_YES)
      curr->pd->color = delete_mark_color;
  for (GenericElemList *curr = gs->ld_list; curr != NULL; curr = curr->next)
    if (curr->ld->del_flag == DF_YES)
      curr->ld->color = delete_mark_color;
  for (GenericElemList *curr = gs->cd_list; curr != NULL; curr = curr->next)
    if (curr->cd->del_flag == DF_YES)
      curr->cd->color = delete_mark_color;
  mark_everything_df_dont_know(gs);
}
