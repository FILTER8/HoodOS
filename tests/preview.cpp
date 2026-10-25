#include <cstdio>
#include <cassert>
#include "../firmware/HoodOS/core/HoodNavigation.h"
void save(const char*path){
 FILE *f=fopen(path,"wb"); assert(f); fprintf(f,"P6\n240 240\n255\n");
 for(int i=0;i<240*240;i++) {
  uint16_t c=HoodTheme::pixels[i];
  uint8_t rgb[3]={uint8_t(((c>>11)&31)*255/31),uint8_t(((c>>5)&63)*255/63),uint8_t((c&31)*255/31)};
  fwrite(rgb,1,3,f);
 }
 fclose(f);
}
int main(){
 HoodState::begin(); HoodState::followedId=351;
 HoodAPI::reset(351);
 String raw="{\"tokenId\":351,\"width\":20,\"height\":20,\"encoding\":\"hex-msb-row-major\",\"pixels\":\"00000000000000007e000ffc01c02018110393883b908317f8388003880038ff83ae043c7e23f0323fa323eff23d70c3a880\",\"blackPixels\":140}";
 assert(HoodAPI::parse(raw,351)); HoodAPI::status=HoodAPI::READY;
 assert(HoodTheme::begin(lv_screen_active()));
 HoodState::screen=HoodState::HOME; HoodNavigation::render(); save("tests/home.ppm");
 HoodState::screen=HoodState::APPS; HoodNavigation::render(); save("tests/apps.ppm");
 HoodState::screen=HoodState::MORE; HoodNavigation::render(); save("tests/more.ppm");
 HoodState::screen=HoodState::HOODIE; HoodNavigation::render(); save("tests/hoodie.ppm");
}
