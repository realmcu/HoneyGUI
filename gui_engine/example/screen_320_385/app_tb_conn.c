#include "root_image_chargebox/ui_resource.h"
#include <gui_img.h>
#include "gui_win.h"
#include <gui_button.h>
#include <gui_page.h>

void page_tb_conn(void *parent)
{
    gui_page_t *pg = gui_page_create(parent, "BT", 0, 40, 385, 200);
    gui_img_create_from_mem(parent, "overlap", OVERLAP_BIN, 0, -30, 0, 0);
    gui_img_create_from_mem(parent, "AURACAST", AURACAST_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(parent, "overlap", OVERLAP_BIN, 0, 240, 0, 0);

    gui_button_t *button1 = gui_button_create(pg, 10, 40, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, NULL,
                                              0, 0);
    char *text1 = "1    RTL8773ESL";
    gui_text_t *button_text1 = gui_text_create(button1, "button_text", 10, 0, 367, 38);
    gui_text_set(button_text1, text1, GUI_FONT_SOURCE_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text1), 24);

    gui_button_t *button2 = gui_button_create(pg, 10, 80, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, NULL,
                                              0, 0);
    char *text2 = "2    RTL8763EAU";
    gui_text_t *button_text2 = gui_text_create(button2, "button_text", 10, 0, 367, 38);
    gui_text_set(button_text2, text2, GUI_FONT_SOURCE_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text2), 24);

    gui_button_t *button3 = gui_button_create(pg, 10, 120, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, NULL,
                                              0, 0);
    char *text3 = "3    TV1";
    gui_text_t *button_text3 = gui_text_create(button3, "button_text", 10, 0, 367, 38);
    gui_text_set(button_text3, text3, GUI_FONT_SOURCE_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text3), 24);

    gui_button_t *button4 = gui_button_create(pg, 10, 160, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, "",
                                              0, 0);
    char *text4 = "4    TV2";
    gui_text_t *button_text4 = gui_text_create(button4, "button_text", 10, 0, 367, 38);
    gui_text_set(button_text4, text4, GUI_FONT_SOURCE_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text4), 24);

    gui_button_t *button5 = gui_button_create(pg, 10, 200, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, "",
                                              0, 0);
    char *text5 = "5    TV3";
    gui_text_t *button_text5 = gui_text_create(button5, "button_text", 10, 0, 367, 38);
    gui_text_set(button_text5, text5, GUI_FONT_SOURCE_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text5), 24);

    gui_button_t *button6 = gui_button_create(pg, 10, 240, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, "",
                                              0, 0);
    char *text6 = "6    TV4";
    gui_text_t *button_text6 = gui_text_create(button6, "button_text", 10, 0, 367, 38);
    gui_text_set(button_text6, text6, GUI_FONT_SOURCE_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text6), 24);

    gui_button_t *button7 = gui_button_create(pg, 10, 280, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, "",
                                              0, 0);
    char *text7 = "7    TV5";
    gui_text_t *button_text7 = gui_text_create(button7, "button_text", 10, 0, 367, 38);
    gui_text_set(button_text7, text7, GUI_FONT_SOURCE_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text7), 24);



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