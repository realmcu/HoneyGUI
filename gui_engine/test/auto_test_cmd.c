/**
 * @file auto_test_cmd.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 1.0
 * @date 2024-08-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "auto_test_cmd.h"
#include "auto_test_file.h"

/*Demo test.Print pass.*/
void print_pass(void *msg)
{
    AUTO_TEST_LOG("PASS!");
}
void cmd_print_pass(void)
{
    gui_server_exec_cb(print_pass);
}
GUI_EXPORT_CMD(pass, cmd_print_pass);

/*Demo test.Print FAIL.*/
void print_fail(void *msg)
{
    AUTO_TEST_LOG("FAIL!");
}
void cmd_print_fail(void)
{
    gui_server_exec_cb(print_fail);
}
GUI_EXPORT_CMD(fail, cmd_print_fail);

/*obj test case 000: Print the size of the structures of all widgets*/
void print_all_widget_size(void *msg)
{
    test_case_name = "gui_test_case_obj_000";
    AUTO_TEST_LOG("%s START!", test_case_name);
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
    AUTO_TEST_LOG("%s PASS!", test_case_name);
}
void gui_test_case_obj_000(void)
{
    gui_server_exec_cb(print_all_widget_size);
}

/*img test case 000: Determine whether the structure size of the img widget has increased*/
void compare_img_widget_szie(void *msg)
{
    test_case_name = "gui_test_case_img_000";
    AUTO_TEST_LOG("%s START !", test_case_name);
    LOG_SIZEOF(img);
    if (sizeof(gui_img_t) <= GUI_IMG_T_SIZE)
    {
        AUTO_TEST_LOG("%s PASS!", test_case_name);
    }
    else
    {
        AUTO_TEST_LOG("%s FAIL!", test_case_name);
    }
}
void gui_test_case_img_000(void)
{
    gui_server_exec_cb(compare_img_widget_szie);
}

/*img test case 001: Test using the img widget to draw an image in RGB565 format*/
#include "image565.c"
void test_img_RGB565_filter(void *msg)
{
    test_case_name = "gui_test_case_img_001";
    AUTO_TEST_LOG("%s START !", test_case_name);
    gui_app_t *app = gui_current_app();
    gui_obj_t *screen = &app->screen;
    gui_obj_tree_free(screen);
    gui_img_t *img = gui_img_create_from_mem(screen, "img", _acimage565, 0, 0, 0, 0);
    gui_fb_change();
    gui_fb_disp(screen, false);
    save_dc(0);
    compare_dc(0);
    AUTO_TEST_LOG("%s PASS!", test_case_name);
}
void gui_test_case_img_001(void)
{
    gui_server_exec_cb(test_img_RGB565_filter);
}

void auto_test_finish(void *msg)
{
    AUTO_TEST_LOG("auto_test_finish!");
}
void gui_auto_test_finish(void)
{
    gui_server_exec_cb(auto_test_finish);
}

void auto_test(void *msg)
{
    gui_test_case_obj_000();
    gui_test_case_img_000();
    gui_test_case_img_001();
    gui_auto_test_finish();
}
GUI_EXPORT_CMD(auto_test_start, auto_test);
