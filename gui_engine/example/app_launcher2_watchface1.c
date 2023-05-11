
#include <gui_obj.h>

#ifdef MODULE_VG_LITE
#include "gui_vg_lite_clock.h"
#endif
void watchface(void *parent)
{
#ifdef MODULE_VG_LITE
    gui_vg_lite_clock_t *path_clock = gui_vg_lite_clock_create(parent,  "path_clock", NULL, 0, 0,
                                                               454, 454);
#else
    //gui_button_create(parent, 252, 57, 100, 100, ACTIVITY_SVG, NULL, NULL, 2, 5134);
#endif
}
