#pragma once
#include "../core/HoodUICommon.h"

// 2x2 tiles, each with a genuine Smallbits 8x8 icon at 5x scale (40x40).
// Unlike v0.2, every tile is >= 104x84 physical pixels for touch accuracy.
namespace AppsApp {
  struct Tile { const char *name; const char *icon; HoodState::Screen dest; };
  static const Tile first[4] = {
    {"HOODIE",icon_shirt,HoodState::HOODIE},
    {"BANKR",icon_wallet,HoodState::BANKR},
    {"JOURNEY",icon_compass,HoodState::JOURNEY},
    {"GAMES",icon_play,HoodState::GAMES},
  };
  static const Tile second[4] = {
    {"GALLERY",icon_images,HoodState::GALLERY},
    {"SETTINGS",icon_wrench,HoodState::SETTINGS},
    {"FOLLOW",icon_heart,HoodState::FOLLOW},
    {"HOME",icon_shirt,HoodState::HOME},
  };
  inline void draw(bool more) {
    HoodFont::draw(more ? "MORE" : "APPS",10,9,HoodFont::LARGE,2);
    HoodIcons::draw(HoodWiFi::connected() ? icon_wifi : icon_wifi_slash,218,9,2);
    HoodTheme::box(8,30,224,1);
    const Tile *tiles = more ? second : first;
    for(int i=0;i<4;i++) {
      int x=10+(i%2)*116;
      int y=40+(i/2)*92;
      HoodTheme::outline(x,y,104,84);
      HoodIcons::draw(tiles[i].icon,x+32,y+8,5);
      HoodFont::center(tiles[i].name,x+52,y+60,HoodFont::SMALL,2);
    }
    HoodUI::dots(more?2:1);
  }
  inline void tap(bool more,int x,int y) {
    if(y>=224) { HoodState::screen = more ? HoodState::APPS : HoodState::MORE; return; }
    if(y<40 || y>=216) return;
    int col=(x>=126)?1:0;
    int row=(y>=132)?1:0;
    int sx=10+col*116,sy=40+row*92;
    if(x<sx || x>=sx+104 || y<sy || y>=sy+84) return;
    const Tile &item = (more?second:first)[row*2+col];
    if (item.dest==HoodState::FOLLOW) HoodState::openFollow();
    else if (item.dest==HoodState::HOME) HoodState::screen=HoodState::HOME;
    else HoodState::open(item.dest);
  }
}
