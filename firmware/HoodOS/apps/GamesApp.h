#pragma once
#include "../core/HoodUICommon.h"
namespace GamesApp {
  inline void draw() {
    HoodUI::header("GAMES");
    HoodIcons::draw(icon_play,92,68,7);
    HoodUI::note("COMING SOON",150);
    HoodFont::center("HOOD GAMES",120,174,HoodFont::TINY,2);
    HoodUI::bottomHome();
  }
  inline void tap(int,int y) { if (y<35 || y>208) HoodState::back(); }
}
