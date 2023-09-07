/*
 * File      : gui_intelligent_stacking.h
 */
#ifndef __GUI_INTELLIGENT_STACKING_H__
#define __GUI_INTELLIGENT_STACKING_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>



typedef struct gui_intelligent_stacking
{
    gui_obj_t base;
    gui_list_t  tab_list;
} gui_intelligent_stacking_t;




gui_intelligent_stacking_t *gui_intelligent_stacking_create(void *parent,  const char *name,
                                                            int16_t x, int16_t y, int16_t w, int16_t h);

void gui_intelligent_stacking_add_tab(void *addr);

void gui_intelligent_default(void);


#ifdef __cplusplus
}
#endif

#endif

