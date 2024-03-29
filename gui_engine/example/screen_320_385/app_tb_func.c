#include "root_image_chargebox/ui_resource.h"
#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>

void page_tb_func(void *parent)
{
    gui_img_create_from_mem(parent, "table2", TABLE2_BLANK_BIN, 10, 10, 0, 0);

    char *text1 = "Name:     RTL8763ESE";
    gui_text_t *t1 = gui_text_create(parent, "txt", 20, 15, gui_get_screen_width(), 24);
    gui_text_set(t1, text1, GUI_FONT_SRC_BMP, gui_obj_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text1),
                 24);
    //gui_text_type_set(t1, SIMKAI_SIZE24_BITS4_FONT_BIN);
    char *text2 = "Battery:  Case 96%,L 100%,R 100%";
    gui_text_t *t2 = gui_text_create(parent, "txt", 20, 55, gui_get_screen_width(), 24);
    gui_text_set(t2, text2, GUI_FONT_SRC_BMP, gui_obj_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text2),
                 24);
    //gui_text_type_set(t2, SIMKAI_SIZE24_BITS4_FONT_BIN);
    char *text3 = "BT:       Connection";
    gui_text_t *t3 = gui_text_create(parent, "txt", 20, 95, gui_get_screen_width(), 24);
    gui_text_set(t3, text3, GUI_FONT_SRC_BMP, gui_obj_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text3),
                 24);
    //gui_text_type_set(t3, SIMKAI_SIZE24_BITS4_FONT_BIN);
    char *text4 = "Volume:    50%";
    gui_text_t *t4 = gui_text_create(parent, "txt", 20, 135, gui_get_screen_width(), 24);
    gui_text_set(t4, text4, GUI_FONT_SRC_BMP, gui_obj_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text4),
                 24);
    // gui_text_type_set(t4, SIMKAI_SIZE24_BITS4_FONT_BIN);
    char *text5 = "ANC:      ON";
    gui_text_t *t5 = gui_text_create(parent, "txt", 20, 175, gui_get_screen_width(), 24);
    gui_text_set(t5, text5, GUI_FONT_SRC_BMP, gui_obj_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text5),
                 24);
    //gui_text_type_set(t5, SIMKAI_SIZE24_BITS4_FONT_BIN);

    gui_button_t *button1 = gui_button_create(parent, 80, 250, 100, 100, VOL_INC_BIN, VOL_INC_BLUE_BIN,
                                              "",
                                              0, 0);
    gui_button_t *button2 = gui_button_create(parent, 220, 250, 100, 100, VOL_DEC_BIN, VOL_DEC_BLUE_BIN,
                                              "",
                                              0, 0);

}
