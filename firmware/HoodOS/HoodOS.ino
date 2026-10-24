// HoodOS v0.1 — ESP32-S3, ST7789 240x240, CST816 touch, LVGL 9
// Hardware wiring follows the provided working 09_lvgl_arduino_v9.ino sketch.
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include "TouchDrvCSTXXX.hpp"
#include "HoodWiFi.h"
#include "HoodUI.h"

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
  static bool pressed = false;
  static lv_point_t last = {0, 0};
  int16_t x[1], y[1];
  // CST816 IRQ is used by the manufacturer's working example.
  if (touchIRQ) {
    touchIRQ = false;
    pressed = touch.getPoint(x, y, 1) > 0;
    if (pressed) { last.x = x[0]; last.y = y[0]; }
  } else {
    pressed = false;
  }
  data->state = pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
  data->point = last;
}

void setup() {
  Serial.begin(115200);
  if (!gfx->begin()) { Serial.println("Display init failed"); while (true) delay(1000); }
  gfx->fillScreen(RGB565_BLACK);
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  touch.setPins(SENSOR_RST, SENSOR_IRQ);
  Wire.begin(SENSOR_SDA, SENSOR_SCL);
  touchReady = touch.begin(Wire, CST816_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL);
  if (touchReady) attachInterrupt(digitalPinToInterrupt(SENSOR_IRQ), onTouchIRQ, FALLING);

  lv_init();
  lv_tick_set_cb([]() -> uint32_t { return millis(); });
  const uint32_t bufferPixels = 240 * 120;
  drawBuffer = (lv_color_t *)heap_caps_malloc(bufferPixels * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!drawBuffer) { Serial.println("LVGL buffer allocation failed"); while (true) delay(1000); }
  display = lv_display_create(240, 240);
  lv_display_set_flush_cb(display, displayFlush);
  lv_display_set_buffers(display, drawBuffer, nullptr, bufferPixels * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
  if (touchReady) {
    lv_indev_t *input = lv_indev_create();
    lv_indev_set_type(input, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(input, readTouch);
  }
  HoodUI::create();
  HoodWiFi::begin();
  HoodUI::updateStatus();
}

void loop() {
  lv_timer_handler();
  static uint32_t lastUpdate = 0;
  if (millis() - lastUpdate >= 1500) {
    lastUpdate = millis();
    HoodUI::updateStatus();
  }
  delay(5);
}
