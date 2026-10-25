#pragma once
#include <Arduino.h>
#include <Preferences.h>

namespace HoodState {
  enum Screen : uint8_t { HOME, APPS, MORE, HOODIE, BANKR, JOURNEY, GAMES, GALLERY, SETTINGS, FOLLOW };
  static Screen screen = HOME;
  static Screen returnScreen = HOME;
  static Screen previousPage = SETTINGS;
  static uint16_t followedId = 351; // Default for a new installation; following is not ownership.
  static char draft[6] = "";
  static bool invalidId = false;

  inline bool isDashboard(Screen s) { return s == HOME || s == APPS || s == MORE; }
  inline void open(Screen destination) {
    if (isDashboard(screen)) returnScreen = screen;
    screen = destination;
  }
  inline void back() { screen = returnScreen; }
  inline void begin() {
    Preferences p;
    if (p.begin("hoodos", true)) {
      followedId = p.getUShort("follow", 351);
      p.end();
    }
    if (followedId > 5999) followedId = 351;
  }
  inline void openFollow() {
    previousPage = screen;
    draft[0] = 0;
    invalidId = false;
    screen = FOLLOW;
  }
  inline void digit(char d) {
    size_t n = strlen(draft);
    if (n == 1 && draft[0] == '0') { draft[0] = d; draft[1] = 0; }
    else if (n < sizeof(draft)-1) { draft[n] = d; draft[n+1] = 0; }
    invalidId = false;
  }
  inline void erase() {
    size_t n = strlen(draft);
    if (n > 0) draft[n-1] = 0;
    invalidId = false;
  }
  inline bool save() {
    if (!draft[0]) { invalidId = true; return false; }
    int val = atoi(draft);
    if (val < 0 || val > 5999) { invalidId = true; return false; }
    Preferences p;
    if (!p.begin("hoodos", false)) { invalidId = true; return false; }
    size_t written = p.putUShort("follow", uint16_t(val));
    p.end();
    if (!written) { invalidId = true; return false; }
    followedId = uint16_t(val);
    screen = HOME; // Show the new companion immediately.
    return true;
  }
}
