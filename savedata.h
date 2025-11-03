#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <stdio.h>

#include "geom/state.h"

void save_to_file(FILE *handle, GeometryState const *gs);

bool load_from_file(FILE *handle, GeometryState *gs);

#endif
