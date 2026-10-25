#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include <stdlib.h>
#include "HoodTheme.h"
#include "HoodState.h"
#include "HoodWiFi.h"
#include "HoodAPI.h"
#include "../apps/HomeApp.h"
#include "../apps/AppsApp.h"
#include "../apps/HoodieApp.h"
#include "../apps/BankrApp.h"
#include "../apps/JourneyApp.h"
#include "../apps/GamesApp.h"
#include "../apps/GalleryApp.h"
#include "../apps/SettingsApp.h"
#include "../apps/FollowApp.h"

namespace HoodNavigation {
  static lv_point_t startPoint = {0, 0};
  static lv_point_t endPoint = {0, 0};
  static bool swiped = false;

  inline void render() {
    HoodTheme::clear();
    switch(HoodState::screen) {
      case HoodState::HOME: HomeApp::draw(); break;
      case HoodState::APPS: AppsApp::draw(false); break;
      case HoodState::MORE: AppsApp::draw(true); break;
      case HoodState::HOODIE: HoodieApp::draw(); break;
      case HoodState::BANKR: BankrApp::draw(); break;
      case HoodState::JOURNEY: JourneyApp::draw(); break;
      case HoodState::GAMES: GamesApp::draw(); break;
      case HoodState::GALLERY: GalleryApp::draw(); break;
      case HoodState::SETTINGS: SettingsApp::draw(); break;
      case HoodState::FOLLOW: FollowApp::draw(); break;
    }
    HoodTheme::present();
  }
  inline void swipe(int dx) {
    if (!HoodState::isDashboard(HoodState::screen)) return;
    if (dx < 0) { // Finger moving left => next dashboard page.
      if (HoodState::screen == HoodState::HOME) HoodState::screen=HoodState::APPS;
      else if (HoodState::screen == HoodState::APPS) HoodState::screen=HoodState::MORE;
    } else { // Finger moving right => previous dashboard page.
      if (HoodState::screen == HoodState::MORE) HoodState::screen=HoodState::APPS;
      else if (HoodState::screen == HoodState::APPS) HoodState::screen=HoodState::HOME;
    }
  }
  inline void event(lv_event_t *e) {
    lv_event_code_t code=lv_event_get_code(e);
    if (code != LV_EVENT_PRESSED && code != LV_EVENT_PRESSING &&
        code != LV_EVENT_RELEASED && code != LV_EVENT_CLICKED) return;
    lv_indev_t *input=lv_indev_active();
    if (!input) return;
    lv_point_t p;
    lv_indev_get_point(input,&p);
    if (code == LV_EVENT_PRESSED) {
      startPoint=p;
      endPoint=p;
      swiped=false;
      return;
    }
    if (code == LV_EVENT_PRESSING) {
      endPoint=p;
      return;
    }
    if (code == LV_EVENT_RELEASED) {
      endPoint=p;
      const int dx=int(endPoint.x)-int(startPoint.x);
      const int dy=int(endPoint.y)-int(startPoint.y);
      if (HoodState::isDashboard(HoodState::screen) &&
          abs(dx)>=42 && abs(dx)>abs(dy)*13/10) {
        swiped=true;
        swipe(dx);
        render();
      }
      return;
    }
    // LVGL may also emit CLICKED after RELEASED. Ignore it after a gesture.
    if (swiped) { swiped=false; return; }
    if (p.x<0 || p.x>=240 || p.y<0 || p.y>=240) return;
    switch(HoodState::screen) {
      case HoodState::HOME: HomeApp::tap(p.x,p.y); break;
      case HoodState::APPS: AppsApp::tap(false,p.x,p.y); break;
      case HoodState::MORE: AppsApp::tap(true,p.x,p.y); break;
      case HoodState::HOODIE: HoodieApp::tap(p.x,p.y); break;
      case HoodState::BANKR: BankrApp::tap(p.x,p.y); break;
      case HoodState::JOURNEY: JourneyApp::tap(p.x,p.y); break;
      case HoodState::GAMES: GamesApp::tap(p.x,p.y); break;
      case HoodState::GALLERY: GalleryApp::tap(p.x,p.y); break;
      case HoodState::SETTINGS: SettingsApp::tap(p.x,p.y); break;
      case HoodState::FOLLOW: FollowApp::tap(p.x,p.y); break;
    }
    render();
  }
  inline bool begin() {
    HoodState::begin();
    HoodAPI::reset(HoodState::followedId);
    if (!HoodTheme::begin(lv_screen_active())) return false;
    lv_obj_add_event_cb(HoodTheme::canvas,event,LV_EVENT_PRESSED,nullptr);
    lv_obj_add_event_cb(HoodTheme::canvas,event,LV_EVENT_PRESSING,nullptr);
    lv_obj_add_event_cb(HoodTheme::canvas,event,LV_EVENT_RELEASED,nullptr);
    lv_obj_add_event_cb(HoodTheme::canvas,event,LV_EVENT_CLICKED,nullptr);
    render();
    return true;
  }
  inline void tick() {
    static uint32_t nextUiTick=0;
    static bool lastConnected=false;
    const bool connected=HoodWiFi::connected();
    bool changed=HoodAPI::tick(connected,HoodState::followedId);
    if (connected != lastConnected) {
      lastConnected=connected;
      Serial.printf("HoodOS Wi-Fi: %s\n",connected ? "ONLINE" : "OFFLINE");
      changed=true;
    }
    if (changed) render();
    if ((int32_t)(millis()-nextUiTick)>=0) {
      nextUiTick=millis()+2200;
      if (HoodState::screen == HoodState::HOME) render();
    }
  }
}
