#include <draw_img.h>
#include <stdio.h>
#include "acc_sw_rle.h"
#include "acc_init.h"


extern void sw_acc_init(void);
extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);


void gui_acc_blit_to_dc(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    gui_get_acc()->blit(image, dc, rect);
}
