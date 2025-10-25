#pragma once

#include "geom.h"
#include "stdio.h"

typedef struct {
  PointDef *point_defs[100];
  LineDef *line_defs[100];
  CircleDef *circle_defs[100];
  int p_n;
  int l_n;
  int c_n;
} SaveData;

void save_to_file(FILE *handle, PointDef *pd_arr[], int pd_n, LineDef *ld_arr[],
                  int ld_n, CircleDef *cd_arr[], int cd_n);

bool load_from_file(FILE *handle, SaveData *save_data);
