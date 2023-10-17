/**
 * @file gui_widget_template.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-10-17
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __GUI_WIDGET_TEMPLATE_H__
#define __GUI_WIDGET_TEMPLATE_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>



typedef struct gui_widget_template
{
    gui_obj_t base;
    void *user_data;
} gui_widget_template_t;




gui_widget_template_t *gui_widget_template_create(void *parent,  const char *name, void *addr,
                                                  int16_t x, int16_t y, int16_t w, int16_t h);


#ifdef __cplusplus
}
#endif

#endif

