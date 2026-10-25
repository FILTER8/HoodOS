#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <esp_heap_caps.h>
#include "HoodTheme.h"

// Public, read-only pixel data endpoint.
// 20 x 20, row-major, black=1, lime=0, most significant bit first.
// Each 100-hex-character response decodes to precisely 50 bytes (400 pixels).
// There are no wallet/private-key operations in this module.
// Set to 0 in HoodOS.ino for a Wi-Fi-only isolation test.
#ifndef HOODOS_ENABLE_PIXEL_FETCH
#define HOODOS_ENABLE_PIXEL_FETCH 1
#endif

namespace HoodAPI {
  enum Status : uint8_t { WAIT_WIFI, FETCHING, READY, FAILED };
  static uint16_t tokenId = 351;
  static Status status = WAIT_WIFI;
  static uint8_t bitmap[50] = {};
  static uint16_t blackPixels = 0;
  static uint32_t nextAttempt = 0;
  static char problem[28] = "WAITING FOR WIFI";

  inline bool validTokenId(uint16_t id) { return id < 6000; }
  inline bool ready(uint16_t id) { return status == READY && tokenId == id; }
  inline void reset(uint16_t id) {
    tokenId = id;
    memset(bitmap, 0, sizeof(bitmap));
    blackPixels = 0;
    status = WAIT_WIFI;
    nextAttempt = 0;
    strcpy(problem, "WAITING FOR WIFI");
    Serial.printf("HoodAPI: following #%u\n", (unsigned)id);
  }
  inline void retry() {
    status = WAIT_WIFI;
    nextAttempt = 0;
  }
  inline bool hexValue(char c, uint8_t &out) {
    if (c >= '0' && c <= '9') out = c - '0';
    else if (c >= 'a' && c <= 'f') out = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') out = c - 'A' + 10;
    else return false;
    return true;
  }
  // Parse only the documented flat JSON fields: no additional ArduinoJson dependency.
  inline bool fieldValue(const String &body, const char *key, String &value) {
    String needle = String('"') + key + '"';
    int pos = body.indexOf(needle);
    if (pos < 0) return false;
    pos = body.indexOf(':', pos + needle.length());
    if (pos < 0) return false;
    ++pos;
    while (pos < (int)body.length() && (body[pos] == ' ' || body[pos] == '\r' || body[pos] == '\n')) ++pos;
    if (pos >= (int)body.length()) return false;
    if (body[pos] == '"') {
      int end = body.indexOf('"', pos+1);
      if (end < 0) return false;
      value = body.substring(pos+1, end);
      return true;
    }
    int end = pos;
    while (end < (int)body.length() && body[end] >= '0' && body[end] <= '9') ++end;
    if (end == pos) return false;
    value = body.substring(pos, end);
    return true;
  }
  inline bool parse(const String &body, uint16_t expectedId) {
    String v;
    if (!fieldValue(body, "tokenId", v) || v.toInt() != expectedId) return false;
    if (!fieldValue(body, "width", v) || v != "20") return false;
    if (!fieldValue(body, "height", v) || v != "20") return false;
    if (!fieldValue(body, "encoding", v) || v != "hex-msb-row-major") return false;
    if (!fieldValue(body, "blackPixels", v)) return false;
    int claimed = v.toInt();
    if (claimed < 0 || claimed > 400) return false;
    if (!fieldValue(body, "pixels", v) || v.length() != 100) return false;
    uint8_t decoded[50];
    uint16_t black = 0;
    for (int i = 0; i < 50; ++i) {
      uint8_t hi, lo;
      if (!hexValue(v[2*i], hi) || !hexValue(v[2*i+1], lo)) return false;
      decoded[i] = uint8_t((hi << 4) | lo);
      black += __builtin_popcount((unsigned)decoded[i]);
    }
    if (black != claimed) return false;
    memcpy(bitmap, decoded, sizeof(bitmap));
    blackPixels = black;
    return true;
  }
  inline void connectionStats(const char *phase) {
    Serial.printf("HoodAPI [%s]: wifi=%d ip=%s heap=%u internal=%u largest=%u psram=%u\n",
                  phase, (int)WiFi.status(), WiFi.localIP().toString().c_str(),
                  (unsigned)ESP.getFreeHeap(),
                  (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                  (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                  (unsigned)ESP.getFreePsram());
  }

  inline bool fetch(uint16_t id) {
    // A negative HTTPClient code is a connection failure, not an HTTP response.
    // Keep this request read-only; never reuse insecure TLS for wallet actions.
    const char *host = "api.onchainhoodies.xyz";
    const String url = String("https://") + host + "/v1/token/" + id + "/pixels";
    connectionStats("before request");

    if (WiFi.status() != WL_CONNECTED || WiFi.localIP() == IPAddress(0,0,0,0)) {
      strcpy(problem, "WIFI NO IP");
      Serial.println("HoodAPI: Wi-Fi is not ready for HTTPS");
      return false;
    }

    IPAddress address;
    if (WiFi.hostByName(host, address) != 1) {
      strcpy(problem, "DNS FAILED");
      Serial.println("HoodAPI: DNS lookup FAILED");
      return false;
    }
    Serial.printf("HoodAPI: DNS %s -> %s\n", host, address.toString().c_str());

    WiFiClientSecure client;
    client.setInsecure(); // Public, read-only artwork only (prototype).
    client.setHandshakeTimeout(20); // ESP32 TLS handshake seconds.

    HTTPClient http;
    http.setConnectTimeout(12000); // Phone hotspots can take several seconds.
    http.setTimeout(12000);
    http.setReuse(false);
    if (!http.begin(client, url)) {
      strcpy(problem, "HTTPS BEGIN FAILED");
      Serial.println("HoodAPI: http.begin() FAILED");
      return false;
    }
    http.addHeader("Accept", "application/json");
    Serial.printf("HoodAPI: GET Hoodie #%u ...\n", (unsigned)id);
    const int code = http.GET();
    Serial.printf("HoodAPI: GET result=%d (%s)\n",
                  code, HTTPClient::errorToString(code).c_str());

    if (code != HTTP_CODE_OK) {
      // Capture TLS details before the HTTP client releases the socket.
      char tlsError[144] = {0};
      const int tlsCode = client.lastError(tlsError, sizeof(tlsError));
      Serial.printf("HoodAPI: TLS lastError=%d, detail=%s\n", tlsCode, tlsError);
      connectionStats("after failed GET");
      if (code < 0) {
        // DNS succeeded, but TCP/TLS could not finish the request.
        snprintf(problem, sizeof(problem), "CONNECT ERROR %d", code);
      } else {
        snprintf(problem, sizeof(problem), "HTTP ERROR %d", code);
      }
      http.end();
      return false;
    }
    const String body = http.getString();
    Serial.printf("HoodAPI: HTTP 200, received %u bytes\n", (unsigned)body.length());
    http.end();
    if (body.length() > 1024 || !parse(body, id)) {
      strcpy(problem, "INVALID PIXEL DATA");
      Serial.printf("HoodAPI: invalid response for #%u (%u bytes)\n",
                    (unsigned)id, (unsigned)body.length());
      return false;
    }
    Serial.printf("HoodAPI: Hoodie #%u loaded, %u black pixels\n",
                  (unsigned)id, (unsigned)blackPixels);
    return true;
  }
  // Called from the normal Arduino loop (not from the LVGL rendering callback).
  // Returns true when the visible state changes. HTTP calls may briefly block UI.
  inline bool tick(bool wifiConnected, uint16_t followedId) {
    // A true connection is not merely Wi-Fi association: wait for IP stability
    // before doing any HTTPS handshake. This also isolates hotspot reconnects.
    static bool wasConnected = false;
    static uint32_t connectedAt = 0;
    if (wifiConnected != wasConnected) {
      wasConnected = wifiConnected;
      connectedAt = wifiConnected ? millis() : 0;
      Serial.printf("HoodAPI: Wi-Fi state changed: %s\n", wifiConnected ? "ONLINE" : "OFFLINE");
    }
    if (tokenId != followedId) { reset(followedId); return true; }
    if (!wifiConnected) {
      if (status != READY && status != WAIT_WIFI) {
        status = WAIT_WIFI;
        strcpy(problem, "WAITING FOR WIFI");
        return true;
      }
      return false; // Keep a previously loaded portrait during a Wi-Fi dropout.
    }
    if (status == READY) return false;
    if (millis() - connectedAt < 10000UL) return false;
#if !HOODOS_ENABLE_PIXEL_FETCH
    static bool warned = false;
    if (!warned) {
      warned = true;
      Serial.println("HoodAPI: pixel fetch DISABLED (Wi-Fi diagnostic mode)");
      strcpy(problem, "PIXEL FETCH PAUSED");
      status = FAILED;
      return true;
    }
    return false;
#else
    if ((int32_t)(millis() - nextAttempt) < 0) return false;
    status = FETCHING;
    Serial.printf("HoodAPI: fetching Hoodie #%u\n", unsigned(tokenId));
    bool ok = fetch(tokenId);
    status = ok ? READY : FAILED;
    nextAttempt = millis() + (ok ? 0 : 15000);
    return true;
#endif
  }
  inline void drawPortrait(int x, int y, int scale) {
    if (status != READY) return;
    // The API bitmap encodes BLACK ink. Unset pixels are lime.
    HoodTheme::box(x, y, 20*scale, 20*scale, HoodTheme::HOOD_LIME);
    for (int pixel = 0; pixel < 400; ++pixel) {
      if (bitmap[pixel >> 3] & (0x80 >> (pixel & 7))) {
        HoodTheme::box(x+(pixel%20)*scale, y+(pixel/20)*scale,
                       scale, scale, HoodTheme::HOOD_BLACK);
      }
    }
  }
}
