#pragma once
#include "../assets/HoodFont.h"
#include "../assets/HoodIcons.h"
#include "HoodState.h"
#include "HoodWiFi.h"

namespace HoodUI {
  inline void header(const char *title, bool back=true) {
    HoodFont::draw(back ? "<" : "*", 9, 10, HoodFont::LARGE, 2);
    HoodFont::draw(title, 30, 11, HoodFont::SMALL, 2);
    HoodIcons::draw(HoodWiFi::connected() ? icon_wifi : icon_wifi_slash, 218, 9, 2);
    HoodTheme::box(8, 30, 224, 1);
  }
  inline void note(const char *s, int y) { HoodFont::center(s, 120, y, HoodFont::SMALL, 2); }
  inline void navButton(const char *s, int x, int y, int w, int h) {
    HoodTheme::outline(x,y,w,h);
    HoodFont::center(s, x+w/2, y+(h-10)/2, HoodFont::SMALL, 2);
  }
  inline void bottomHome() { HoodFont::center("< BACK",120,222,HoodFont::SMALL,2); }
  inline void dots(int current) {
    for (int i=0;i<3;i++) {
      int x = 101 + i*16;
      if (i==current) HoodTheme::box(x,230,8,5);
      else HoodTheme::outline(x,230,8,5);
    }
  }
}
