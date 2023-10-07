
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_magic_img.h>
#include <gui_curtain.h>
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_tab.h"

void page_down_message_app1_test(void *parent)
{
    gui_magic_img_t *figure1 =  gui_magic_img_create_from_mem(parent, "figure1", MESSAGE_APP1L_BIN,
                                                              0, 0, 0, 0);
    gui_img_set_opacity(figure1, 50);
}
void page_down_message_music_test(void *parent)
{
    gui_magic_img_t *figure2 =  gui_magic_img_create_from_mem(parent, "figure2", MESSAGE_MUS_BIN,
                                                              0, 0, 0, 0);
    gui_img_set_opacity(figure2, 50);
}
void page_down_message_app2_test(void *parent)
{
    gui_magic_img_t *figure3 =  gui_magic_img_create_from_mem(parent, "figure3", MESSAGE_3_BIN,
                                                              0, 0, 0, 0);
    gui_img_set_opacity(figure3, 50);
}
void page_down_message_weixin_test(void *parent)
{
    gui_magic_img_t *figure4 =  gui_magic_img_create_from_mem(parent, "figure4", MESSAGE_WEIXIN_BIN,
                                                              0, 0, 0, 0);
    gui_img_set_opacity(figure4, 50);
}
void page_down_message_close(void *parent)
{
    gui_magic_img_t *close =  gui_magic_img_create_from_mem(parent, "close", CLOSE_BIN,
                                                            35, 330, 0, 0);
    gui_img_set_opacity(close, 50);
}
void  page_down_message_design(void *parent)
{
    gui_magic_img_t *background =  gui_magic_img_create_from_mem(parent, "background",
                                                                 RECT_350_348_BIN,
                                                                 0, 0, 0, 0);
    background->base.draw_img.blend_mode = IMG_BYPASS_MODE;

    gui_cardview_t *cd =  gui_cardview_create(parent, "cardview",    0, 0, 0, 0);
    gui_cardview_set_style(cd, STACKING);

    gui_card_t *tb_music = gui_card_create(cd, "app1",     0, 0, 0, 0, 0, 0);
    gui_card_t *tb_weixin = gui_card_create(cd, "tb_weixin",   0, 0, 0, 0, 0, 1);
    gui_card_t *tb_weixin1 = gui_card_create(cd, "music",  0, 0, 0, 0, 0, 2);
    gui_card_t *tb_weixin2 = gui_card_create(cd, "app2",  0, 0, 0, 0, 0, 3);
    // gui_cardview_set_style(cd, STACKING);

    page_down_message_app1_test(tb_music);
    page_down_message_weixin_test(tb_weixin);
    page_down_message_music_test(tb_weixin1);
    page_down_message_app2_test(tb_weixin2);
    gui_magic_img_t *close =  gui_magic_img_create_from_mem(parent, "close",
                                                            CLOSE_BIN,
                                                            35, 300, 0, 0);
}