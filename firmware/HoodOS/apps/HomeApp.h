#pragma once
#include "../core/HoodUICommon.h"
#include "../core/HoodAPI.h"
namespace HomeApp {
  inline void draw() {
    HoodFont::draw("HOOD OS",10,9,HoodFont::LARGE,2);
    HoodFont::draw("0.3",173,13,HoodFont::TINY,2);
    HoodIcons::draw(HoodWiFi::connected() ? icon_wifi : icon_wifi_slash,218,9,2);
    HoodTheme::box(8,30,224,1);

    if (HoodAPI::ready(HoodState::followedId)) {
      HoodAPI::drawPortrait(50,39,7); // Real 20x20 artwork at 7x = 140x140.
    } else {
      HoodTheme::outline(50,39,140,140);
      HoodIcons::draw(icon_shirt,88,75,8);
      HoodFont::center("LOADING",120,159,HoodFont::TINY,2);
    }
    char token[25];
    snprintf(token,sizeof(token),"HOODIE #%u",unsigned(HoodState::followedId));
    HoodFont::center(token,120,188,HoodFont::LARGE,2);
    const char *status = HoodAPI::ready(HoodState::followedId) ? "YOUR COMPANION" : HoodAPI::problem;
    HoodFont::center(status,120,210,HoodFont::TINY,2);
    HoodUI::dots(0);
  }
  inline void tap(int x,int y) {
    if (y>=224) { HoodState::screen=HoodState::APPS; return; }
    if (x>=37 && x<=203 && y>=35 && y<=224) HoodState::open(HoodState::HOODIE);
  }
}
