#include <cassert>
#include <cstdio>
#include "../firmware/HoodOS/core/HoodNavigation.h"
int main(){
  HoodAPI::reset(351);
  String raw="{\"version\":\"1.0\",\"tokenId\":351,\"width\":20,\"height\":20,\"encoding\":\"hex-msb-row-major\",\"pixels\":\"00000000000000007e000ffc01c02018110393883b908317f8388003880038ff83ae043c7e23f0323fa323eff23d70c3a880\",\"blackPixels\":140}";
  assert(HoodAPI::parse(raw,351));
  assert(HoodAPI::blackPixels==140);
  assert(!HoodAPI::parse(raw,0));
  String corrupt=raw;
  corrupt.s.replace(corrupt.s.find("140}"),4,"139}");
  assert(!HoodAPI::parse(corrupt,351));
  HoodAPI::status=HoodAPI::READY;
  assert(HoodTheme::begin(lv_screen_active()));
  HoodTheme::clear();
  HoodAPI::drawPortrait(50,39,7);
  int black=0, green=0;
  for(int y=39;y<179;y+=7)for(int x=50;x<190;x+=7){
    if(HoodTheme::pixels[y*240+x]==HoodTheme::HOOD_BLACK) ++black;
    if(HoodTheme::pixels[y*240+x]==HoodTheme::HOOD_LIME) ++green;
  }
  assert(black==140 && green==260);
  assert(HoodFont::measure("HOODIE #351",HoodFont::LARGE,2)<=210);
  HoodState::screen=HoodState::HOME;
  HoodNavigation::swipe(-80);
  assert(HoodState::screen==HoodState::APPS);
  HoodNavigation::swipe(-80);
  assert(HoodState::screen==HoodState::MORE);
  HoodNavigation::swipe(80);
  assert(HoodState::screen==HoodState::APPS);
  HoodNavigation::render();
  std::printf("PASS: pixel parser validates #351 (140 black); scales 20x20; UI headers compile\n");
}
