#include <draw_img.h>
#include "gui_api.h"


extern void sw_acc_init(void);
extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);
extern void arm_2d_blur(struct gui_dispdev *dc, gui_rect_t *rect, uint8_t blur_degree,
                        void *cache_mem);
extern void arm_2d_pointer_init(void);
static acc_engine_t acc =
{
    .blit = sw_acc_blit,
    .blur = arm_2d_blur,
};


void gui_port_acc_init(void)
{
    sw_acc_init();
    acc.blit = sw_acc_blit;
    arm_2d_pointer_init();
    gui_acc_info_register(&acc);
}
