#pragma once
#include <WiFi.h>
#include "secrets.h"

namespace HoodWiFi {
  inline void begin() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(HOOD_WIFI_SSID, HOOD_WIFI_PASSWORD);
  }
  inline bool connected() { return WiFi.status() == WL_CONNECTED; }
  inline String status() {
    if (connected()) return String("ONLINE ") + WiFi.localIP().toString();
    return "CONNECTING...";
  }
}
