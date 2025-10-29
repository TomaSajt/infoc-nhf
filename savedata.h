#pragma once

#include <stdio.h>

#include "geom_state.h"

void save_to_file(FILE *handle, GeometryState const *gs);

bool load_from_file(FILE *handle, GeometryState *gs);
