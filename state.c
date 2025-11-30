#include "state.h"

void zoom(ViewInfo *view_info, Pos2D fp, double mul) {
  *view_info = (ViewInfo){
      .scale = view_info->scale * mul,
      .center =
          {
              .x = fp.x + (view_info->center.x - fp.x) / mul,
              .y = fp.y + (view_info->center.y - fp.y) / mul,
          },
  };
}
