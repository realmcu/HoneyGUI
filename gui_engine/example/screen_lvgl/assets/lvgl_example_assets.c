#include "lvgl.h"

void load_font_bin_file(void)
{
    lv_font_t *my_font;
    my_font = lv_font_load("F:/lv_font_harmony_32.bin");
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label, my_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(label, "Hello LVGL! By bin file.");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 60, 15);
}
void load_img_bin_file(void)
{
    lv_obj_t *icon = lv_img_create(lv_scr_act());
    lv_img_set_src(icon, "F:/lvgl_logo.bin");
    lv_obj_set_pos(icon, 10, 10);
}


void load_font_c_file(void)
{
    lv_obj_t *label = lv_label_create(lv_scr_act());
    extern const lv_font_t lv_font_harmony_32;
    lv_obj_set_style_text_font(label, &lv_font_harmony_32, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(label, "Hello LVGL! By C file.");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 60, 105);
}

void load_img_c_file(void)
{
    LV_IMG_DECLARE(img_demo_lvgl_logo);
    lv_obj_t *icon = lv_img_create(lv_scr_act());
    lv_img_set_src(icon, &img_demo_lvgl_logo);
    lv_obj_set_pos(icon, 10, 100);
}

void lv_tool_file_test(void)
{
//C file
    load_img_c_file();
    load_font_c_file();

//bin file
    load_img_bin_file();
    load_font_bin_file();
}
