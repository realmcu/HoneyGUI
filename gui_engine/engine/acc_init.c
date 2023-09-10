#include <draw_img.h>
#include <stdio.h>
//#include <gui_matrix.h>
#include "acc_engine.h"


extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect);

extern void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect);

static struct acc_engine acc = {0};

void gui_acc_init(void)
{
#ifdef RTK_MODULE_VG_LITE
    acc.blit = hw_acc_blit;
#elif defined (RTK_MODULE_RTK_PPE) || defined(RTK_MODULE_RTK_PPEV2)
    acc.blit = hw_acc_blit;
#else
    acc.blit = sw_acc_blit;
#endif
}



struct acc_engine *gui_get_acc(void)
{
    return &acc;
}

