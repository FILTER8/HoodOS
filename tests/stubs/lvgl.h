#pragma once
#include <stdint.h>
struct lv_obj_t {};
struct lv_display_t{};
struct lv_indev_t{};
struct lv_event_t{};
struct lv_point_t{int16_t x,y;};
using lv_event_code_t=int;
#define LV_COLOR_FORMAT_RGB565 1
#define LV_OBJ_FLAG_CLICKABLE 1
#define LV_OBJ_FLAG_SCROLLABLE 2
#define LV_EVENT_PRESSED 1
#define LV_EVENT_PRESSING 2
#define LV_EVENT_RELEASED 3
#define LV_EVENT_CLICKED 4
inline lv_obj_t* lv_canvas_create(lv_obj_t*){static lv_obj_t x;return &x;}
inline void lv_canvas_set_buffer(lv_obj_t*,void*,int,int,int){}
inline void lv_obj_set_pos(lv_obj_t*,int,int){}
inline void lv_obj_set_size(lv_obj_t*,int,int){}
inline void lv_obj_set_style_pad_all(lv_obj_t*,int,int){}
inline void lv_obj_set_style_border_width(lv_obj_t*,int,int){}
inline void lv_obj_add_flag(lv_obj_t*,int){}
inline void lv_obj_clear_flag(lv_obj_t*,int){}
inline void lv_obj_invalidate(lv_obj_t*){}
inline lv_obj_t* lv_screen_active(){static lv_obj_t x;return &x;}
inline lv_event_code_t lv_event_get_code(lv_event_t*){return LV_EVENT_CLICKED;}
inline lv_indev_t* lv_indev_active(){static lv_indev_t x;return &x;}
inline void lv_indev_get_point(lv_indev_t*,lv_point_t*p){p->x=120;p->y=120;}
inline void lv_obj_add_event_cb(lv_obj_t*,void(*)(lv_event_t*),int,void*){}
