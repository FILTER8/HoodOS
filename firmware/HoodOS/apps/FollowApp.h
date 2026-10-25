#pragma once
#include "../core/HoodUICommon.h"
namespace FollowApp {
  inline void draw() {
    HoodUI::header("FOLLOW HOODIE");
    char current[21];
    snprintf(current,sizeof(current),"CURRENT #%u",unsigned(HoodState::followedId));
    HoodFont::center(current,120,42,HoodFont::TINY,2);
    HoodFont::center(HoodState::draft[0] ? HoodState::draft : "_",120,63,HoodFont::LARGE,2);
    if (HoodState::invalidId) HoodFont::center("INVALID ID",120,82,HoodFont::TINY,1);
    const char *keys[12] = {"1","2","3","4","5","6","7","8","9","DEL","0","SAVE"};
    for(int i=0; i<12; i++) {
      int x=9+(i%3)*75;
      int y=91+(i/3)*36;
      HoodTheme::outline(x,y,71,32);
      HoodFont::center(keys[i],x+35,y+11,HoodFont::SMALL,2);
    }
  }
  inline void tap(int x,int y) {
    if(y<34) { HoodState::screen=HoodState::previousPage; return; }
    if(y<91 || y>=235 || x<9 || x>=234) return;
    int col=(x-9)/75;
    int row=(y-91)/36;
    int insideX=(x-9)%75, insideY=(y-91)%36;
    if(col>2 || row>3 || insideX>=71 || insideY>=32) return;
    int idx = row*3+col;
    const char *keys="123456789";
    if(idx<9) HoodState::digit(keys[idx]);
    else if(idx==9) HoodState::erase();
    else if(idx==10) HoodState::digit('0');
    else HoodState::save();
  }
}
