#include "root_image_chargebox/ui_resource.h"
#include <gui_img.h>
#include "gui_win.h"
#include <gui_button.h>
#include <gui_page.h>

void page_tb_conn(void *parent)
{
    gui_img_create_from_mem(parent, "page0", BACKGROUND_BIN, 0, 0, 0, 0);

    gui_page_t *pg = gui_page_create(parent, "BT", 0, 40, 385, 200);
    gui_img_create_from_mem(parent, "overlap", OVERLAP_BIN, 0, -30, 0, 0);
    gui_img_create_from_mem(parent, "AURACAST", AURACAST_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(parent, "overlap", OVERLAP_BIN, 0, 240, 0, 0);
    void *addr1 = SIMKAI_SIZE24_BITS4_FONT_BIN;
    gui_font_mem_init(addr1);


    gui_button_t *button1 = gui_button_create(pg, 10, 40, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, "",
                                              0, 0);
    char *text1 = "1    RTL8773ESL";
    GUI_TYPE(gui_button_t, button1)->text->font_height = 24;
    GUI_TYPE(gui_button_t, button1)->text->path = 0;
    gui_text_type_set(GUI_TYPE(gui_button_t, button1)->text, addr1);
    gui_text_mode_set(GUI_TYPE(gui_button_t, button1)->text, LEFT);
    gui_text_move(GUI_TYPE(gui_button_t, button1)->text, 20, 2);
    GUI_TYPE(gui_button_t, button1)->text->content = (void *)text1;
    GUI_TYPE(gui_button_t, button1)->text->len = strlen(text1);


    gui_button_t *button2 = gui_button_create(pg, 10, 80, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, "",
                                              0, 0);
    char *text2 = "2    RTL8763EAU";
    GUI_TYPE(gui_button_t, button2)->text->font_height = 24;
    GUI_TYPE(gui_button_t, button2)->text->path = 0;
    gui_text_type_set(GUI_TYPE(gui_button_t, button2)->text, addr1);
    gui_text_mode_set(GUI_TYPE(gui_button_t, button2)->text, LEFT);
    gui_text_move(GUI_TYPE(gui_button_t, button2)->text, 20, 2);
    GUI_TYPE(gui_button_t, button2)->text->content = (void *)text2;
    GUI_TYPE(gui_button_t, button2)->text->len = strlen(text2);

    gui_button_t *button3 = gui_button_create(pg, 10, 120, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, "",
                                              0, 0);
    char *text3 = "3    TV1";
    GUI_TYPE(gui_button_t, button3)->text->font_height = 24;
    GUI_TYPE(gui_button_t, button3)->text->path = 0;
    gui_text_type_set(GUI_TYPE(gui_button_t, button3)->text, addr1);
    gui_text_mode_set(GUI_TYPE(gui_button_t, button3)->text, LEFT);
    gui_text_move(GUI_TYPE(gui_button_t, button3)->text, 20, 2);
    GUI_TYPE(gui_button_t, button3)->text->content = (void *)text3;
    GUI_TYPE(gui_button_t, button3)->text->len = strlen(text3);

    gui_button_t *button4 = gui_button_create(pg, 10, 160, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, "",
                                              0, 0);
    char *text4 = "4    TV2";
    GUI_TYPE(gui_button_t, button4)->text->font_height = 24;
    GUI_TYPE(gui_button_t, button4)->text->path = 0;
    gui_text_type_set(GUI_TYPE(gui_button_t, button4)->text, addr1);
    gui_text_mode_set(GUI_TYPE(gui_button_t, button4)->text, LEFT);
    gui_text_move(GUI_TYPE(gui_button_t, button4)->text, 20, 2);
    GUI_TYPE(gui_button_t, button4)->text->content = (void *)text4;
    GUI_TYPE(gui_button_t, button4)->text->len = strlen(text4);

    gui_button_t *button5 = gui_button_create(pg, 10, 200, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, "",
                                              0, 0);
    char *text5 = "5    TV3";
    GUI_TYPE(gui_button_t, button5)->text->font_height = 24;
    GUI_TYPE(gui_button_t, button5)->text->path = 0;
    gui_text_type_set(GUI_TYPE(gui_button_t, button5)->text, addr1);
    gui_text_mode_set(GUI_TYPE(gui_button_t, button5)->text, LEFT);
    gui_text_move(GUI_TYPE(gui_button_t, button5)->text, 20, 2);
    GUI_TYPE(gui_button_t, button5)->text->content = (void *)text5;
    GUI_TYPE(gui_button_t, button5)->text->len = strlen(text5);

    gui_button_t *button6 = gui_button_create(pg, 10, 240, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, "",
                                              0, 0);
    char *text6 = "6    TV4";
    GUI_TYPE(gui_button_t, button6)->text->font_height = 24;
    GUI_TYPE(gui_button_t, button6)->text->path = 0;
    gui_text_type_set(GUI_TYPE(gui_button_t, button6)->text, addr1);
    gui_text_mode_set(GUI_TYPE(gui_button_t, button6)->text, LEFT);
    gui_text_move(GUI_TYPE(gui_button_t, button6)->text, 20, 2);
    GUI_TYPE(gui_button_t, button6)->text->content = (void *)text6;
    GUI_TYPE(gui_button_t, button6)->text->len = strlen(text6);



    gui_button_t *button_scan = gui_button_create(parent, 20, 250, 100, 100, SCAN_BIN, SCAN_BLUE_BIN,
                                                  "",
                                                  0, 0);
    gui_button_t *button_conn = gui_button_create(parent, 140, 250, 100, 100, CONN_BIN, CONN_BLUE_BIN,
                                                  "",
                                                  0, 0);
    gui_button_t *button_disc = gui_button_create(parent, 260, 250, 100, 100, DISC_BIN, DISC_BLUE_BIN,
                                                  "",
                                                  0, 0);
}