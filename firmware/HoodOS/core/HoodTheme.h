#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include <esp_heap_caps.h>

// All screen artwork is rendered with exactly two RGB565 colors.
// Prefers PSRAM for its full-screen canvas; falls back to internal RAM if needed.
namespace HoodTheme {
  static constexpr int WIDTH = 240;
  static constexpr int HEIGHT = 240;
  static constexpr uint16_t HOOD_BLACK = 0x0000;
  static constexpr uint16_t HOOD_LIME = 0xCFE0; // #CCFF00 encoded as RGB565

  static uint16_t *pixels = nullptr;
  static lv_obj_t *canvas = nullptr;

  inline bool begin(lv_obj_t *parent) {
    const size_t bytes = WIDTH * HEIGHT * sizeof(uint16_t);
    Serial.printf("HoodTheme: canvas requested %u bytes, free PSRAM=%u\n",
                  (unsigned)bytes, (unsigned)ESP.getFreePsram());
    if (psramFound()) pixels = (uint16_t *)heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (pixels) Serial.println("HoodTheme: using PSRAM");
    else {
      Serial.println("HoodTheme: PSRAM allocation unavailable, trying normal RAM");
      pixels = (uint16_t *)heap_caps_malloc(bytes, MALLOC_CAP_8BIT);
    }
    if (!pixels) { Serial.println("ERROR: Unable to allocate canvas buffer"); return false; }
    canvas = lv_canvas_create(parent);
    if (!canvas) { Serial.println("ERROR: lv_canvas_create failed"); return false; }
    lv_canvas_set_buffer(canvas, pixels, WIDTH, HEIGHT, LV_COLOR_FORMAT_RGB565);
    lv_obj_set_pos(canvas, 0, 0);
    lv_obj_set_size(canvas, WIDTH, HEIGHT);
    lv_obj_set_style_pad_all(canvas, 0, 0);
    lv_obj_set_style_border_width(canvas, 0, 0);
    lv_obj_add_flag(canvas, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(canvas, LV_OBJ_FLAG_SCROLLABLE);
    Serial.println("HoodTheme: full-screen RGB565 canvas created");
    return true;
  }

  inline void clear(uint16_t color = HOOD_BLACK) {
    if (!pixels) return;
    for (int i = 0; i < WIDTH * HEIGHT; i++) pixels[i] = color;
  }

  inline void box(int x, int y, int w, int h, uint16_t color = HOOD_LIME) {
    if (!pixels || w <= 0 || h <= 0) return;
    int x0 = max(0, x), y0 = max(0, y);
    int x1 = min(WIDTH, x+w), y1 = min(HEIGHT, y+h);
    for (int yy = y0; yy < y1; yy++) {
      uint16_t *row = pixels + yy * WIDTH;
      for (int xx = x0; xx < x1; xx++) row[xx] = color;
    }
  }
  inline void outline(int x, int y, int w, int h, uint16_t color = HOOD_LIME) {
    box(x,y,w,1,color); box(x,y+h-1,w,1,color);
    box(x,y,1,h,color); box(x+w-1,y,1,h,color);
  }
  inline void present() { if (canvas) lv_obj_invalidate(canvas); }
}
