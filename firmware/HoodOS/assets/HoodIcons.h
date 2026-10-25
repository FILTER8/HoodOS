#pragma once
#include "../core/HoodTheme.h"
#include "smallbits_icons.h"

// Original Smallbits 8x8 icon patterns. No smoothing, integer scaling only.
namespace HoodIcons {
  inline void draw(const char *bitmap, int x, int y, int scale=3,
                   uint16_t ink=HoodTheme::HOOD_LIME) {
    if (!bitmap) return;
    for (int row=0; row<8; row++) {
      for (int col=0; col<8; col++) {
        if (bitmap[row*8+col] == '1')
          HoodTheme::box(x+col*scale,y+row*scale,scale,scale,ink);
      }
    }
  }
}
