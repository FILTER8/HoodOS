#pragma once
#include <lvgl.h>
#include "HoodWiFi.h"

namespace HoodUI {
  static constexpr uint32_t GREEN = 0xCCFF00;
  static lv_obj_t *statusLabel = nullptr;
  static lv_obj_t *pageLabel = nullptr;
  static uint8_t currentPage = 0;
  static const char *pages[] = {"HOOD", "BANKR", "SETUP"};

  inline void changePage(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    currentPage = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    if (pageLabel) {
      if (currentPage == 0) lv_label_set_text(pageLabel, "YOUR HOODIE\nCOMING SOON");
      if (currentPage == 1) lv_label_set_text(pageLabel, "BANKR\nCOMING SOON");
      if (currentPage == 2) lv_label_set_text(pageLabel, "WIFI STATUS\nSEE HEADER");
    }
  }

  inline void create() {
    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_color(screen, lv_color_hex(GREEN), 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "HOOD OS / 0.1");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 10, 10);

    statusLabel = lv_label_create(screen);
    lv_label_set_text(statusLabel, "CONNECTING...");
    lv_obj_set_width(statusLabel, 220);
    lv_label_set_long_mode(statusLabel, LV_LABEL_LONG_DOT);
    lv_obj_align(statusLabel, LV_ALIGN_TOP_LEFT, 10, 33);

    lv_obj_t *line = lv_obj_create(screen);
    lv_obj_set_size(line, 220, 2);
    lv_obj_align(line, LV_ALIGN_TOP_MID, 0, 58);
    lv_obj_set_style_bg_color(line, lv_color_hex(GREEN), 0);
    lv_obj_set_style_border_width(line, 0, 0);
    lv_obj_set_style_radius(line, 0, 0);
    lv_obj_set_style_pad_all(line, 0, 0);

    pageLabel = lv_label_create(screen);
    lv_label_set_text(pageLabel, "YOUR HOODIE\nCOMING SOON");
    lv_obj_set_style_text_align(pageLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(pageLabel, LV_ALIGN_CENTER, 0, -5);

    for (uint8_t i = 0; i < 3; ++i) {
      lv_obj_t *btn = lv_button_create(screen);
      lv_obj_set_size(btn, 70, 34);
      lv_obj_set_pos(btn, 10 + i * 75, 195);
      lv_obj_set_style_bg_color(btn, lv_color_hex(0x000000), 0);
      lv_obj_set_style_border_color(btn, lv_color_hex(GREEN), 0);
      lv_obj_set_style_border_width(btn, 1, 0);
      lv_obj_set_style_radius(btn, 0, 0);
      lv_obj_set_style_shadow_width(btn, 0, 0);
      lv_obj_add_event_cb(btn, changePage, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
      lv_obj_t *label = lv_label_create(btn);
      lv_label_set_text(label, pages[i]);
      lv_obj_set_style_text_color(label, lv_color_hex(GREEN), 0);
      lv_obj_center(label);
    }
  }

  inline void updateStatus() {
    if (statusLabel) lv_label_set_text(statusLabel, HoodWiFi::status().c_str());
  }
}
