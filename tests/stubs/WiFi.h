#pragma once
#include "Arduino.h"
#define WL_CONNECTED 3
#define WIFI_STA 1
struct IPAddress { String toString() const {return "127.0.0.1";} };
using arduino_event_id_t = int;
struct arduino_event_info_t { struct { uint8_t reason; } wifi_sta_disconnected = {0}; };
#define ARDUINO_EVENT_WIFI_STA_CONNECTED 1
#define ARDUINO_EVENT_WIFI_STA_GOT_IP 2
#define ARDUINO_EVENT_WIFI_STA_DISCONNECTED 3
struct WiFiClass { template<typename F> void onEvent(F){} void setSleep(bool){} void mode(int){} void setAutoReconnect(bool){} void begin(const char*,const char*){} int status(){return WL_CONNECTED;} IPAddress localIP(){return {};}};
inline WiFiClass WiFi;
