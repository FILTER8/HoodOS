#pragma once
#include "../core/HoodUICommon.h"
#include "../core/HoodAPI.h"
namespace SettingsApp {
  inline void draw() {
    HoodUI::header("SETTINGS");
    HoodFont::draw("WIFI",14,43,HoodFont::SMALL,2);
    HoodFont::draw(HoodWiFi::connected()? "ONLINE" : "OFFLINE",122,43,HoodFont::SMALL,2);
    String addr = HoodWiFi::ip();
    HoodFont::draw(addr.c_str(),14,64,HoodFont::TINY,2);

    // Diagnostic information visible even when USB Serial Monitor isn't set up.
    char ram[38];
    const unsigned freeKb = (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT) / 1024;
    const unsigned largestKb = (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT) / 1024;
    snprintf(ram,sizeof(ram),"RAM %uK MAX %uK",freeKb,largestKb);
    HoodFont::draw(ram,14,81,HoodFont::TINY,2);
    char diag[32];
    if (psramFound()) {
      snprintf(diag,sizeof(diag),"PSRAM OK  ERR %u",unsigned(HoodWiFi::disconnectReason()));
    } else {
      snprintf(diag,sizeof(diag),"NO PSRAM  ERR %u",unsigned(HoodWiFi::disconnectReason()));
    }
    HoodFont::draw(diag,14,96,HoodFont::TINY,2);
    HoodTheme::box(14,115,212,1);
    char token[25];
    snprintf(token,sizeof(token),"FOLLOWING #%u",unsigned(HoodState::followedId));
    HoodFont::draw(token,14,122,HoodFont::SMALL,2);
    HoodUI::navButton("CHANGE HOODIE",16,144,208,32);
    HoodUI::navButton("RETRY PIXELS",16,184,208,32);
    HoodFont::center("< BACK",120,230,HoodFont::TINY,1);
  }
  inline void tap(int,int y) {
    if (y<35 || y>=225) HoodState::back();
    else if (y>=144 && y<=176) HoodState::openFollow();
    else if (y>=184 && y<=216) HoodAPI::retry();
  }
}
