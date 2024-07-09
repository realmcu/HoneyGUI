/**
 * @file auto_test_cmd.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "auto_test_cmd.h"

void print_pass(void *msg)
{
    AUTO_TEST_LOG("PASS!");
}
void cmd_print_pass(void)
{
    gui_server_exec_cb(print_pass);
}
GUI_EXPORT_CMD(pass, cmd_print_pass);


void print_fail(void *msg)
{
    AUTO_TEST_LOG("FAIL!");
}
void cmd_print_fail(void)
{
    gui_server_exec_cb(print_fail);
}
GUI_EXPORT_CMD(fail, cmd_print_fail);

void print_all_widget_size(void *msg)
{
    AUTO_TEST_LOG("gui_test_case_obj_000 START!");
    LOG_SIZEOF(obj);
    LOG_SIZEOF(img);
    LOG_SIZEOF(win);
    LOG_SIZEOF(page);
    LOG_SIZEOF(tab);
    LOG_SIZEOF(tabview);
    LOG_SIZEOF(button);
    LOG_SIZEOF(text);
    LOG_SIZEOF(scroll_text);
    LOG_SIZEOF(app);
    LOG_SIZEOF(canvas_arc);
    LOG_SIZEOF(canvas_rect);
    LOG_SIZEOF(canvas);
    LOG_SIZEOF(card);
    LOG_SIZEOF(cardview);
    LOG_SIZEOF(colorwheel);
    LOG_SIZEOF(cube);
    LOG_SIZEOF(curtain);
    LOG_SIZEOF(curtainview);
    LOG_SIZEOF(gallery);
    LOG_SIZEOF(grid);
    LOG_SIZEOF(img_live);
    LOG_SIZEOF(img_scope);
    LOG_SIZEOF(stb_img);
    LOG_SIZEOF(kb);
    LOG_SIZEOF(map);
    LOG_SIZEOF(menu_cellular);
    LOG_SIZEOF(multi_level);
    LOG_SIZEOF(pagelist);
    LOG_SIZEOF(pagelistview);
    LOG_SIZEOF(perspective);
    LOG_SIZEOF(progressbar);
    LOG_SIZEOF(qbcode);
    LOG_SIZEOF(scroll_wheel);
    LOG_SIZEOF(seekbar);
    LOG_SIZEOF(progressbar);
    LOG_SIZEOF(simple_img);
    LOG_SIZEOF(svg);
    LOG_SIZEOF(turn_table);
    LOG_SIZEOF(watch_gradient_spot);
    LOG_SIZEOF(wave);
    LOG_SIZEOF(wheel_list);
    AUTO_TEST_LOG("gui_test_case_obj_000 PASS!");
}
void gui_test_case_obj_000(void)
{
    gui_server_exec_cb(print_all_widget_size);
}
GUI_EXPORT_CMD(gui_test_case_obj_000, gui_test_case_obj_000);

void compare_img_szie(void *msg)
{
    AUTO_TEST_LOG("gui_test_case_img_000 START!");
    LOG_SIZEOF(img);
    if (sizeof(gui_img_t) > GUI_IMG_T_SIZE)
    {
        AUTO_TEST_LOG("gui_test_case_img_000 PASS!");
    }
    else
    {
        AUTO_TEST_LOG("gui_test_case_img_000 FAIL!");
    }
}
void gui_test_case_img_000(void)
{
    gui_server_exec_cb(compare_img_szie);
}

void auto_test(void *msg)
{
    gui_test_case_obj_000();
    gui_test_case_img_000();
}
void cmd_auto_test_start(void)
{
    gui_server_exec_cb(auto_test);
}
GUI_EXPORT_CMD(auto_test_start, cmd_auto_test_start);

