#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "../secrets.h"

namespace HoodWiFi {
  static volatile uint8_t lastDisconnectReason = 0;
  inline void onEvent(arduino_event_id_t event, arduino_event_info_t info) {
    switch (event) {
      case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("HoodWiFi: associated with hotspot / router");
        break;
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        lastDisconnectReason = 0;
        Serial.printf("HoodWiFi: GOT IP %s\n", WiFi.localIP().toString().c_str());
        break;
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        lastDisconnectReason = info.wifi_sta_disconnected.reason;
        Serial.printf("HoodWiFi: DISCONNECTED reason=%u\n",
                      (unsigned)info.wifi_sta_disconnected.reason);
        break;
      default: break;
    }
  }

  inline void begin() {
    WiFi.onEvent(onEvent);
    WiFi.mode(WIFI_STA);
    // Keep the ESP32 radio awake: some iPhone hotspots are sensitive to STA sleep.
    WiFi.setSleep(false);
    WiFi.setAutoReconnect(true);
    Serial.printf("HoodWiFi: connecting to SSID '%s' (2.4 GHz)\n", HOOD_WIFI_SSID);
    WiFi.begin(HOOD_WIFI_SSID, HOOD_WIFI_PASSWORD);
  }
  inline bool connected() { return WiFi.status() == WL_CONNECTED; }
  inline uint8_t disconnectReason() { return lastDisconnectReason; }
  inline String ip() { return connected() ? WiFi.localIP().toString() : String("OFFLINE"); }
}
