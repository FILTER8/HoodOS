#pragma once
#include "../core/HoodUICommon.h"
namespace GalleryApp {
  inline void draw() {
    HoodUI::header("GALLERY");
    HoodIcons::draw(icon_images,92,68,7);
    HoodUI::note("COMING SOON",150);
    HoodFont::center("ART AND BUILDS",120,174,HoodFont::TINY,2);
    HoodUI::bottomHome();
  }
  inline void tap(int,int y) { if (y<35 || y>208) HoodState::back(); }
}
