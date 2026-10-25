#pragma once
#include "../core/HoodUICommon.h"
namespace JourneyApp {
  inline void draw() {
    HoodUI::header("JOURNEY");
    HoodIcons::draw(icon_compass,92,68,7);
    HoodUI::note("COMING SOON",150);
    HoodFont::center("HOOD ACTIVITY",120,174,HoodFont::TINY,2);
    HoodUI::bottomHome();
  }
  inline void tap(int,int y) { if (y<35 || y>208) HoodState::back(); }
}
