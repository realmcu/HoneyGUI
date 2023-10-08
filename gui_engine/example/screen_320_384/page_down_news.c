
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_magic_img.h>
#include <gui_curtain.h>
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_tab.h"
#include "gui_canvas.h"

static void canvas_cb(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 320, 384);
    nvgFillColor(canvas->vg, nvgRGBA(30, 30, 30, 255));
    nvgFill(canvas->vg);
}
void page_down_message_app1(void *parent)
{
    gui_magic_img_t *app1_message =  gui_magic_img_create_from_mem(parent, "app1_message",
                                                                   RECT_300_127_BIN,
                                                                   0, 0, 0, 0);
    gui_img_set_opacity(app1_message, 128);
    gui_magic_img_t *text_0 =  gui_magic_img_create_from_mem(parent, "text_0", TEXT_0_BIN, 22, 43, 0,
                                                             0);
    gui_magic_img_t *text_1 =  gui_magic_img_create_from_mem(parent, "text_1", TEXT_1_BIN, 22, 81, 0,
                                                             0);
    gui_magic_img_t *time_0 =  gui_magic_img_create_from_mem(parent, "time_0", TEXT_2_BIN, 226, 8, 0,
                                                             0);
    gui_magic_img_t *pic_app =  gui_magic_img_create_from_mem(parent, "pic_app", INSTAGRAM_BIN, 22, -21,
                                                              0, 0);
}
void page_down_message_music(void *parent)
{
    gui_magic_img_t *muisc =  gui_magic_img_create_from_mem(parent, "muisc_message", MESSAGE_MUS_BIN,
                                                            0, 0, 0, 0);

}
void page_down_message_app2(void *parent)
{
    gui_magic_img_t *app2_message =  gui_magic_img_create_from_mem(parent, "app2_message",
                                                                   MESSAGE_3_BIN,
                                                                   0, 0, 0, 0);
}
void page_down_message_weixin(void *parent)
{
    gui_magic_img_t *weixin_message =  gui_magic_img_create_from_mem(parent, "weixin_message",
                                                                     RECT_300_127_BIN,
                                                                     0, 0, 0, 0);
    gui_img_set_opacity(weixin_message, 128);
    gui_magic_img_t *text_2 =  gui_magic_img_create_from_mem(parent, "text_2", TEXT_0_BIN, 22, 43, 0,
                                                             0);
    gui_magic_img_t *text_3 =  gui_magic_img_create_from_mem(parent, "text_3", TEXT_1_BIN, 22, 81, 0,
                                                             0);
    gui_magic_img_t *time_1 =  gui_magic_img_create_from_mem(parent, "time_1", TEXT_2_BIN, 226, 8, 0,
                                                             0);
    gui_magic_img_t *pic_weixin =  gui_magic_img_create_from_mem(parent, "pic_weixin", WEIXIN_BIN, 22,
                                                                 -21, 0, 0);
}
void page_down_message_close(void *parent)
{
    gui_magic_img_t *close =  gui_magic_img_create_from_mem(parent, "close", CLOSE_BIN,
                                                            35, 330, 0, 0);
    //gui_img_set_opacity(close, 50);
}
void  page_down_message_design(void *parent)
{
    gui_canvas_t *canvas = gui_canvas_create(parent, "canvas", 0, 0, 0, 320, 384);
    gui_canvas_set_canvas_cb(canvas, canvas_cb);
    gui_cardview_t *cd =  gui_cardview_create(parent, "cardview",    0, 0, 0, 0);
    gui_cardview_set_style(cd, STACKING);

    gui_card_t *tb_music = gui_card_create(cd, "app1",     0, 0, 0, 0, 0, 0);
    gui_card_t *tb_weixin = gui_card_create(cd, "tb_weixin",   0, 0, 0, 0, 0, 1);
    gui_card_t *tb_weixin1 = gui_card_create(cd, "music",  0, 0, 0, 0, 0, 2);
    gui_card_t *tb_weixin2 = gui_card_create(cd, "app2",  0, 0, 0, 0, 0, 3);
    // gui_cardview_set_style(cd, STACKING);

    page_down_message_app1(tb_music);
    page_down_message_weixin(tb_weixin);
    page_down_message_music(tb_weixin1);
    page_down_message_app2(tb_weixin2);
    gui_magic_img_t *close =  gui_magic_img_create_from_mem(parent, "close",
                                                            CLOSE_BIN,
                                                            35, 300, 0, 0);
}