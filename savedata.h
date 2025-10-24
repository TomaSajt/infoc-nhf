#pragma once

#include "geom.h"
#include "stdio.h"

void save_to_file(FILE *handle, PointDef *pd_arr[], int pd_n, LineDef *ld_arr[],
                  int ld_n, CircleDef *cd_arr[], int cd_n);
