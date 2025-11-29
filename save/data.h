#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include <stdio.h>

#include "../geom/state.h"

void save_to_file(FILE *handle, GeometryState2 const *gs);

bool load_from_file(FILE *handle, GeometryState2 *gs);

#endif
