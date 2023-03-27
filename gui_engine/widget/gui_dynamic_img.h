/*
 * File      : gui_dynamic.h
 */
#ifndef __GUI_DYNAMIC_IMG_H__
#define __GUI_DYNAMIC_IMG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include <gui_img.h>
#include <gui_magic_img.h>

typedef struct gui_dynamic_img
{
    gui_magic_img_t base;
    void *addr_entry;
    uint8_t total_cnt;
    uint32_t init_time_ms;
    uint32_t interval_time_ms;
    uint32_t duration_time_ms;
} gui_dynamic_img_t;

gui_dynamic_img_t *gui_dynamic_create_from_mem(gui_obj_t *parent, const char *name,
                                               void *addr_entry, int16_t x, int16_t y,  uint8_t total_cnt, uint32_t interval_time_ms,
                                               uint32_t duration_time_ms);

void gui_dynamic_img_stop(void);
void gui_dynamic_img_start(void);
void gui_dynamic_img_restart(gui_dynamic_img_t *this);

#ifdef __cplusplus
}
#endif

#endif

