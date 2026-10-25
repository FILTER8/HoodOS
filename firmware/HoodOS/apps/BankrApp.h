#pragma once
#include "../core/HoodUICommon.h"
namespace BankrApp {
  inline void draw() {
    HoodUI::header("BANKR");
    HoodIcons::draw(icon_wallet,92,68,7);
    HoodUI::note("COMING SOON",150);
    HoodFont::center("NO WALLET CONNECTED",120,174,HoodFont::TINY,2);
    HoodUI::bottomHome();
  }
  inline void tap(int,int y) { if (y<35 || y>208) HoodState::back(); }
}
