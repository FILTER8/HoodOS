#pragma once
#include "../core/HoodTheme.h"
#include "HoodFontData.h"

// Integer-scaled bitmap fonts, no antialiasing; direct pixels on the canvas.
namespace HoodFont {
  enum Size : uint8_t { TINY, SMALL, LARGE };
  inline const HoodFontData::Glyph *table(Size size) {
    if (size == TINY) return HoodFontData::TINY;
    if (size == LARGE) return HoodFontData::LARGE;
    return HoodFontData::SMALL;
  }
  inline int spacing(Size size) {
    if (size == TINY) return HoodFontData::TINY_SPACING;
    if (size == LARGE) return HoodFontData::LARGE_SPACING;
    return HoodFontData::SMALL_SPACING;
  }
  inline int measure(const char *text, Size size = SMALL, int scale = 2) {
    const auto *font = table(size);
    int w = 0;
    while (*text) {
      unsigned char c = (unsigned char)*text++;
      if (c >= 'a' && c <= 'z') c -= 32;
      if (c < 32 || c > 126) c = '?';
      w += (font[c-32].width + spacing(size)) * scale;
    }
    return max(0, w - spacing(size)*scale);
  }
  inline void draw(const char *text, int x, int y, Size size=SMALL, int scale=2,
                   uint16_t ink=HoodTheme::HOOD_LIME) {
    const auto *font = table(size);
    for (; *text; text++) {
      unsigned char c = (unsigned char)*text;
      if (c >= 'a' && c <= 'z') c -= 32;
      if (c < 32 || c > 126) c = '?';
      const auto &g = font[c-32];
      for (int yy=0; yy<g.height; yy++) {
        for (int xx=0; xx<g.width; xx++) {
          if (g.rows[yy] & (1 << (7-xx)))
            HoodTheme::box(x + xx*scale, y + yy*scale, scale, scale, ink);
        }
      }
      x += (g.width + spacing(size))*scale;
    }
  }
  inline void center(const char *text, int centerX, int y, Size size=SMALL, int scale=2,
                     uint16_t ink=HoodTheme::HOOD_LIME) {
    draw(text, centerX-measure(text,size,scale)/2,y,size,scale,ink);
  }
}
