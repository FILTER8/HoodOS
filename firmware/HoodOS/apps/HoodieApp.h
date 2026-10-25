#pragma once
#include "../core/HoodUICommon.h"
#include "../core/HoodAPI.h"
namespace HoodieApp {
  inline void draw() {
    HoodUI::header("HOODIE");
    if (HoodAPI::ready(HoodState::followedId)) HoodAPI::drawPortrait(60,39,6);
    else {
      HoodTheme::outline(60,39,120,120);
      HoodIcons::draw(icon_shirt,96,68,6);
    }
    char token[25];
    snprintf(token,sizeof(token),"HOODIE #%u",unsigned(HoodState::followedId));
    HoodFont::center(token,120,166,HoodFont::LARGE,2);
    HoodUI::navButton("CHANGE HOODIE",24,189,192,34);
    HoodFont::center("< BACK",120,230,HoodFont::TINY,1);
  }
  inline void tap(int,int y) {
    if (y<34 || y>=226) HoodState::back();
    else if (y>=184 && y<225) HoodState::openFollow();
  }
}
