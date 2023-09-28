
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_magic_img.h>
#include <gui_curtain.h>
void  page_down_message(void *parent)
{
    gui_magic_img_t *background =  gui_magic_img_create_from_mem(parent, "background",
                                                                 RECT_350_348_BIN,
                                                                 0, 0, 0, 0);
    background->base.draw_img.blend_mode = IMG_BYPASS_MODE;
    gui_img_set_opacity(background, 128);

#if 1
    gui_magic_img_t *figure_bk0 =  gui_magic_img_create_from_mem(parent, "figure_bk0", RECT_300_127_BIN,
                                                                 10, 76, 0, 0);
    gui_img_set_opacity(figure_bk0, 200);
    gui_img_t *text_0 =  gui_img_create_from_mem(parent, "text_0", TEXT_0_BIN, 33, 119, 0, 0);
    gui_img_t *text_1 =  gui_img_create_from_mem(parent, "text_1", TEXT_1_BIN, 33, 157, 0, 0);
    gui_img_t *time_0 =  gui_img_create_from_mem(parent, "time_0", TEXT_2_BIN, 237, 84, 0, 0);
    gui_img_t *pic_app =  gui_img_create_from_mem(parent, "pic_app", INSTAGRAM_BIN, 33, 55, 0, 0);
#else //delete message
    gui_magic_img_t *figure_bk0 =  gui_magic_img_create_from_mem(parent, "figure_bk0", RECT_300_127_BIN,
                                                                 -71, 76, 0, 0);
    gui_img_set_opacity(figure_bk0, 128);
    gui_img_t *text_0 =  gui_img_create_from_mem(parent, "text_0", TEXT_0_BIN, -64, 119, 0, 0);
    gui_img_t *text_1 =  gui_img_create_from_mem(parent, "text_1", TEXT_1_BIN, -64, 157, 0, 0);
    gui_img_t *time_0 =  gui_img_create_from_mem(parent, "time_0", TEXT_2_BIN, 140, 84, 0, 0);
    gui_img_t *pic_app =  gui_img_create_from_mem(parent, "pic_app", INSTAGRAM_BIN, -64, 55, 0, 0);
    gui_magic_img_t *figure_bk0_1 =  gui_magic_img_create_from_mem(parent, "figure_bk0_1",
                                                                   RECT_100_127_BIN,
                                                                   226, 76, 0, 0);
    gui_img_t *pic_delete =  gui_img_create_from_mem(parent, "pic_delete", DELETE_BIN, 258, 118, 0, 0);
#endif

    gui_magic_img_t *figure_bk1 =  gui_magic_img_create_from_mem(parent, "figure_bk1", RECT_300_127_BIN,
                                                                 10, 240, 0, 0);
    gui_img_set_opacity(figure_bk1, 200);
    gui_img_t *text_2 =  gui_img_create_from_mem(parent, "text_2", TEXT_0_BIN, 33, 283, 0, 0);
    gui_img_t *text_3 =  gui_img_create_from_mem(parent, "text_3", TEXT_1_BIN, 33, 321, 0, 0);
    gui_img_t *time_1 =  gui_img_create_from_mem(parent, "time_1", TEXT_2_BIN, 237, 248, 0, 0);
    gui_img_t *pic_weixin =  gui_img_create_from_mem(parent, "pic_weixin", WEIXIN_BIN, 33, 219, 0, 0);

}