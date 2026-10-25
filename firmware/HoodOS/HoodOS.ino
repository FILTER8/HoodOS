// HoodOS v0.3.2 — modular pixel-art companion firmware
// Waveshare ESP32-S3-Touch-LCD-1.54 (240x240 ST7789 + CST816)
// Display/touch initialization preserved from the successful v0.1 sketch.
#include <Arduino.h>
#include <esp_system.h>
#include <Wire.h>
#include <SPI.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include "TouchDrvCSTXXX.hpp"
// Diagnostic switch: set to 0 if Wi-Fi still vanishes, then re-upload.
// 0 = Wi-Fi connects normally, but HTTPS artwork fetch is disabled.
// 1 = fetch Hoodie pixels after 10 seconds of stable Wi-Fi.
// First confirm Wi-Fi stability with 0, then change to 1 and upload.
#define HOODOS_ENABLE_PIXEL_FETCH 1

#include "core/HoodWiFi.h"
#include "core/HoodNavigation.h"

#define SENSOR_SDA 42
#define SENSOR_SCL 41
#define SENSOR_IRQ 48
#define SENSOR_RST 47
#define GFX_BL 46

TouchDrvCSTXXX touch;
volatile bool touchIRQ = false;
bool touchReady = false;
Arduino_DataBus *bus = new Arduino_ESP32SPI(45, 21, 38, 39, -1);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 40, 0, true, 240, 240);
static lv_display_t *display;
static lv_color_t *drawBuffer;

void IRAM_ATTR onTouchIRQ() { touchIRQ = true; }

void displayFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *pxMap) {
  uint32_t width = lv_area_get_width(area);
  uint32_t height = lv_area_get_height(area);
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)pxMap, width, height);
  lv_display_flush_ready(disp);
}

void readTouch(lv_indev_t *indev, lv_indev_data_t *data) {
  // The vendor example only samples when the IRQ fires. That works for taps,
  // but swipes need continuous coordinates while the finger is touching.
  static bool pressed = false;
  static lv_point_t last = {0, 0};
  int16_t x[1], y[1];
  if (touchIRQ || pressed) {
    touchIRQ = false;
    const uint8_t n = touch.getPoint(x, y, 1);
    pressed = n > 0;
    if (pressed) {
      last.x = constrain(x[0], 0, 239);
      last.y = constrain(y[0], 0, 239);
    }
  }
  data->state = pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
  data->point = last;
}

void setup() {
  Serial.begin(115200);
  Serial.println("HoodOS v0.3.2 boot");
  Serial.printf("HoodOS: reset reason=%d, free heap=%u\n",
                (int)esp_reset_reason(), (unsigned)ESP.getFreeHeap());
  Serial.printf("PSRAM found: %s, total=%u, free=%u\n",
                psramFound() ? "YES" : "NO",
                (unsigned)ESP.getPsramSize(), (unsigned)ESP.getFreePsram());
  if (!gfx->begin()) { Serial.println("Display init failed"); while (true) delay(1000); }
  gfx->fillScreen(RGB565_BLACK);
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
  // A brief lime flash proves the LCD/backlight work before LVGL starts.
  gfx->fillScreen(0xCFE0);
  delay(200);
  gfx->fillScreen(RGB565_BLACK);

  touch.setPins(SENSOR_RST, SENSOR_IRQ);
  Wire.begin(SENSOR_SDA, SENSOR_SCL);
  touchReady = touch.begin(Wire, CST816_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL);
  if (touchReady) attachInterrupt(digitalPinToInterrupt(SENSOR_IRQ), onTouchIRQ, FALLING);
  else Serial.println("Touch not detected - UI display still available");

  lv_init();
  lv_tick_set_cb([]() -> uint32_t { return millis(); });
  // The ST7789 driver consumes 16-bit RGB565 pixels. Explicitly set LVGL's
  // output format instead of relying on lv_conf.h defaults.
  // PARTIAL refresh only: 30 rows is plenty for LVGL and saves 43,200
  // bytes of scarce INTERNAL RAM compared with the old 120-row buffer.
  // Full-screen graphics remain in PSRAM; this is just a flush buffer.
  const uint32_t bufferPixels = 240 * 30;
  const size_t bufferBytes = bufferPixels * 2;
  drawBuffer = (lv_color_t *)heap_caps_malloc(bufferBytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!drawBuffer) drawBuffer = (lv_color_t *)heap_caps_malloc(bufferBytes, MALLOC_CAP_8BIT);
  if (!drawBuffer) { Serial.println("ERROR: LVGL draw buffer allocation failed"); gfx->fillScreen(0xCFE0); while (true) delay(1000); }
  display = lv_display_create(240, 240);
  lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
  lv_display_set_flush_cb(display, displayFlush);
  lv_display_set_buffers(display, drawBuffer, nullptr, bufferBytes, LV_DISPLAY_RENDER_MODE_PARTIAL);
  Serial.printf("LVGL display ready: RGB565, buffer=%u bytes\n", (unsigned)bufferBytes);
  Serial.printf("HoodOS RAM after LVGL: free=%u, largest=%u, PSRAM=%u\n",
                (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                (unsigned)ESP.getFreePsram());
  if (touchReady) {
    lv_indev_t *input = lv_indev_create();
    lv_indev_set_type(input, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(input, readTouch);
  }
  if (!HoodNavigation::begin()) {
    Serial.println("ERROR: HoodOS canvas allocation failed. Check free RAM / PSRAM settings.");
    gfx->fillScreen(0xCFE0); // Full lime = startup error, black = still rendering.
    while(true) delay(1000);
  }
  Serial.printf("HoodOS RAM before Wi-Fi: free=%u, largest=%u, PSRAM=%u\n",
                (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                (unsigned)ESP.getFreePsram());
  Serial.println("HoodOS home screen prepared; starting Wi-Fi");
  lv_timer_handler(); // Force first frame before asynchronous Wi-Fi startup.
  HoodWiFi::begin();
  Serial.println("HoodOS Wi-Fi initiated; entering main loop");
}

void loop() {
  lv_timer_handler();
  HoodNavigation::tick();
  delay(5);
}
