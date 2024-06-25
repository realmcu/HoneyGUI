#include "gui_app.h"
#include "gui_api.h"
#include "gui_win.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_cube.h"
#include "gui_perspective.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_page.h"
#include "gui_grid.h"
#include "font_mem.h"
#include "app_gui_main.h"
#include "benchmark_common.h"
#include "gui_common.h"
#include "root_image/ui_resource.h"

#ifndef _WIN32
#include "lcd_sh8601z_454_lcdc.h"
#include "fmc_api_ext.h"
#endif

#define BENCHMARK_MAX_FRAME 100

static void design_app_benchmark(gui_app_t *app);

uint32_t benchmark_frame_count = 0;

static gui_app_t app_benchmark =
{
    .screen =
    {
        .name = "app_benchmark",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = design_app_benchmark,
    .thread_entry = NULL,
    .active_ms = 0xFFFFFFFF,
};

void *get_app_benchmark(void)
{
    return &app_benchmark;
}

static void win_count_frame_cb(void *p)
{
    /*send msg to update frame cost*/
    if (benchmark_frame_count < BENCHMARK_MAX_FRAME)
    {
        gui_update_by_event(GUI_EVENT_BENCHMARK_UPDATE_FRAME_COST, NULL, false);
    }
    /*send msg to switch scenario*/
    else
    {
        benchmark_frame_count = 0;
        gui_update_by_event(GUI_EVENT_BENCHMARK_NEXT_SCENRAIO, NULL, false);
    }
    gui_fb_change();
}

static void win_count_frame_switch_scenario_cb(void *obj, gui_event_t event)
{
    gui_win_t *this = (gui_win_t *)obj;
    gui_app_t *app = get_app_benchmark();
    /*print current scenario cost*/
    scenario_benchmark_manager_current_cost();
    /*switch to next scenario*/
    if (scenario_benchmark_manager_switch_next(&(app->screen)) == -1)
    {
        this->animate->animate = false;
        app_benchmark_finish();
        gui_log("Benchmark Finish!");
    }
    else
    {
        gui_log("Benchmark Switch!");
        /*run on next scenario*/
    }
}

static void win_count_frame_update_frame_cost_cb(void *obj, gui_event_t event)
{
    benchmark_scenario_t *benchmark = get_current_benchmark_scenario();
    benchmark_cost_t *p_cost = NULL;
    uint32_t input_prepare_time = frame_monitor_benchmark.input_prepare - frame_monitor_benchmark.start;
    uint32_t draw_prepare_time = frame_monitor_benchmark.draw_prepare -
                                 frame_monitor_benchmark.input_prepare;
    uint32_t draw_time = frame_monitor_benchmark.draw - frame_monitor_benchmark.draw_prepare;
    uint32_t total_time = frame_monitor_benchmark.end - frame_monitor_benchmark.start;

    benchmark->current_frame = benchmark_frame_count;
    if (benchmark_frame_count == 0)
    {
        benchmark->input_prepare_cost_us.time_max = input_prepare_time;
        benchmark->input_prepare_cost_us.time_min = input_prepare_time;
        benchmark->draw_prepare_cost_us.time_max = draw_prepare_time;
        benchmark->draw_prepare_cost_us.time_min = draw_prepare_time;
        benchmark->draw_cost_us.time_max = draw_time;
        benchmark->draw_cost_us.time_min = draw_time;
        benchmark->total_cost_us.time_max = total_time;
        benchmark->total_cost_us.time_min = total_time;
    }

    p_cost = &benchmark->input_prepare_cost_us;
    if (input_prepare_time > p_cost->time_max)
    {
        p_cost->time_max = input_prepare_time;
        p_cost->time_max_frame = benchmark->current_frame;
    }
    if (input_prepare_time < p_cost->time_min)
    {
        p_cost->time_min = input_prepare_time;
        p_cost->time_min_frame = benchmark->current_frame;
    }

    p_cost = &benchmark->draw_prepare_cost_us;
    if (draw_prepare_time > p_cost->time_max)
    {
        p_cost->time_max = draw_prepare_time;
        p_cost->time_max_frame = benchmark->current_frame;
    }
    if (draw_prepare_time < p_cost->time_min)
    {
        p_cost->time_min = draw_prepare_time;
        p_cost->time_min_frame = benchmark->current_frame;
    }

    p_cost = &benchmark->draw_cost_us;
    if (draw_time > p_cost->time_max)
    {
        p_cost->time_max = draw_time;
        p_cost->time_max_frame = benchmark->current_frame;
    }
    if (draw_time < p_cost->time_min)
    {
        p_cost->time_min = draw_time;
        p_cost->time_min_frame = benchmark->current_frame;
    }

    p_cost = &benchmark->total_cost_us;
    if (total_time > p_cost->time_max)
    {
        p_cost->time_max = total_time;
        p_cost->time_max_frame = benchmark->current_frame;
    }
    if (total_time < p_cost->time_min)
    {
        p_cost->time_min = total_time;
        p_cost->time_min_frame = benchmark->current_frame;
    }

    benchmark->input_prepare_cost_us.time_avg = (benchmark->input_prepare_cost_us.time_avg *
                                                 benchmark->current_frame + input_prepare_time) / (benchmark->current_frame + 1);
    benchmark->draw_prepare_cost_us.time_avg = (benchmark->draw_prepare_cost_us.time_avg *
                                                benchmark->current_frame + draw_prepare_time) / (benchmark->current_frame + 1);
    benchmark->draw_cost_us.time_avg = (benchmark->draw_cost_us.time_avg * benchmark->current_frame +
                                        draw_time) / (benchmark->current_frame + 1);
    benchmark->total_cost_us.time_avg = (benchmark->total_cost_us.time_avg * benchmark->current_frame +
                                         total_time) / (benchmark->current_frame + 1);
    //gui_log("benchmark_frame_count %d", benchmark_frame_count);
    benchmark_frame_count ++;

}

static void gui_task_kick_wdg(void)
{
#ifndef _WIN32
    WDG_Kick();
#endif
}

/*benchmark app*/
static void design_app_benchmark(gui_app_t *app)
{
    gui_log("design_app_benchmark\n");

    gui_task_ext_execution_sethook(gui_task_kick_wdg);

    gui_font_mem_init(SIMKAI_SIZE32_BITS1_FONT_BIN);

    /*add switch scenario event*/
    gui_win_t *win_count_frame = gui_win_create(&(app->screen), "win_count_frame", 0, 0, 0, 0);
    gui_win_set_animate(win_count_frame, 1, -1, win_count_frame_cb, win_count_frame);
    gui_obj_add_event_cb(win_count_frame, (gui_event_cb_t)win_count_frame_update_frame_cost_cb,
                         (gui_event_t)GUI_EVENT_BENCHMARK_UPDATE_FRAME_COST, NULL);
    gui_obj_add_event_cb(win_count_frame, (gui_event_cb_t)win_count_frame_switch_scenario_cb,
                         (gui_event_t)GUI_EVENT_BENCHMARK_NEXT_SCENRAIO, NULL);

    /*start first scenario*/
    scenario_benchmark_manager_ctor(get_app_benchmark(), &benchmark_scenarios[0]);

}

/*benchmark scenario 0*/
void benchmark_single_img_design(void *parent)
{
    gui_img_t *img = gui_img_create_from_mem(parent, "img", CLOCK_BASE_BIN, 0, 0, 454, 454);
    //gui_log("benchmark_single_img_design img 0x%x", (uint32_t *)img);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
}

/*benchmark scenario 1*/
static float img_rotate_angle = 0.0f;
static void single_img_rotation_ani_cb(void *p)
{
    //gui_log("img_rotate_angle %f", img_rotate_angle);
    gui_img_t *this = (gui_img_t *)p;

    gui_img_rotation(this, img_rotate_angle, this->base.w / 2, this->base.h / 2);
    gui_img_translate(this, this->base.w / 2, this->base.h / 2);

    float step = 360.0f / BENCHMARK_MAX_FRAME;
    if (img_rotate_angle < 360.0f)
    {
        img_rotate_angle = img_rotate_angle + step;
    }
}

void benchmark_single_img_rotation_design(void *parent)
{
    gui_img_t *img_rotation = gui_img_create_from_mem(parent, "img_rotation", CLOCK_BASE_BIN, 0, 0, 454,
                                                      454);
    gui_img_set_mode(img_rotation, IMG_BYPASS_MODE);
    gui_img_set_animate(img_rotation, 0, -1, single_img_rotation_ani_cb, img_rotation);
}

/*benchmark scenario 2*/
static int16_t img_cube_slide = 0;
static void single_img_tab_cube_ani_cb(void *p)
{
    //gui_log("img_cube_slide %d", img_cube_slide);
    gui_img_t *this = (gui_img_t *)p;

    gui_obj_t *parent = this->base.parent;
    gui_tab_t *tab = (gui_tab_t *)parent;
    gui_obj_t *grandparent = tab->base.parent;
    gui_tabview_t *tabview = (gui_tabview_t *)grandparent;

    tabview->release_x = img_cube_slide;

    int16_t step = 454 / BENCHMARK_MAX_FRAME + 1;
    if (img_cube_slide > -400) //cube stop at -400
    {
        img_cube_slide = img_cube_slide - step;
    }

}
void benchmark_single_img_cube_design(void *parent)
{
    gui_tabview_t *tabview_cube = gui_tabview_create(parent, "tabview_cube", 0, 0, 0, 0);
    gui_tabview_set_style(tabview_cube, TAB_CUBE);

    gui_tab_t *tab1 = gui_tab_create(tabview_cube, "tab1",  0, 0, 0, 0, 0, 0);
    gui_tab_t *tab2 = gui_tab_create(tabview_cube, "tab2",  0, 1, 0, 0, 0, 0);

    gui_img_t *img_tab_cube = gui_img_create_from_mem(tab1, "img_tab_cube", CLOCK_BASE_BIN, 0, 0, 454,
                                                      454);
    gui_img_set_mode(img_tab_cube, IMG_SRC_OVER_MODE);
    gui_img_set_animate(img_tab_cube, 0, -1, single_img_tab_cube_ani_cb, img_tab_cube);
}

/*benchmark scenario 3*/
static int16_t img_reduction_slide = 0;
static void single_img_tab_reduction_ani_cb(void *p)
{
    //gui_log("img_reduction_slide %d", img_reduction_slide);
    gui_img_t *this = (gui_img_t *)p;

    gui_obj_t *parent = this->base.parent;
    gui_tab_t *tab = (gui_tab_t *)parent;
    gui_obj_t *grandparent = tab->base.parent;
    gui_tabview_t *tabview = (gui_tabview_t *)grandparent;

    tabview->release_x = img_reduction_slide;

    int16_t step = 454 / BENCHMARK_MAX_FRAME + 1;
    if (img_reduction_slide > -350)//reduction stop at 454 * 0.8
    {
        img_reduction_slide = img_reduction_slide - step;
    }

}
void benchmark_single_img_reduction_design(void *parent)
{
    gui_tabview_t *tabview_reduction = gui_tabview_create(parent, "tabview_reduction", 0, 0, 0, 0);
    gui_tabview_set_style(tabview_reduction, REDUCTION);

    gui_tab_t *tab1 = gui_tab_create(tabview_reduction, "tab1",  0, 0, 0, 0, 0, 0);
    gui_tab_t *tab2 = gui_tab_create(tabview_reduction, "tab2",  0, 1, 0, 0, 0, 0);

    gui_img_t *img_tab_reduction = gui_img_create_from_mem(tab1, "img_tab_reduction", CLOCK_BASE_BIN, 0,
                                                           0,
                                                           454, 454);
    gui_img_set_mode(img_tab_reduction, IMG_SRC_OVER_MODE);
    gui_img_set_animate(img_tab_reduction, 0, -1, single_img_tab_reduction_ani_cb, img_tab_reduction);
}

/*benchmark scenario 4*/
void benchmark_double_img_design(void *parent)
{
    gui_img_t *img1 = gui_img_create_from_mem(parent, "img1", CLOCK_BASE_BIN, 0, 0,
                                              454, 454);
    gui_img_set_mode(img1, IMG_BYPASS_MODE);

    /*should be an agrb 8565 pic*/
    gui_img_t *img2 = gui_img_create_from_mem(parent, "img2", WATCH_BASE_GREY_70_ALPHA_BIN, 0, 0,
                                              454, 454);
    gui_img_set_mode(img2, IMG_SRC_OVER_MODE);
}

/*benchmark scenario 5*/
void benchmark_cube(void *parent)
{
    gui_cube_imgfile_t image_file;
    memset(&image_file, 0, sizeof(gui_cube_imgfile_t));
    for (int i = 0; i < 6; i++)
    {
        image_file.src_mode[i] = IMG_SRC_MEMADDR;
    }
    image_file.data_addr.data_addr_up = B1_RLE_BIN;
    image_file.data_addr.data_addr_down = B2_RLE_BIN;
    image_file.data_addr.data_addr_front = B3_RLE_BIN;
    image_file.data_addr.data_addr_left = B4_RLE_BIN;
    image_file.data_addr.data_addr_right = B5_RLE_BIN;
    image_file.data_addr.data_addr_back = B6_RLE_BIN;
    gui_cube_t *ccc = gui_cube_create(parent, "ccc", &image_file, 0, 0);
    gui_cube_set_center(ccc, 227, 227);
    gui_cube_set_size(ccc, 75);
    gui_cube_auto_rotation_by_y(ccc, 100, 360 / BENCHMARK_MAX_FRAME);
}

/*benchmark scenario 6*/
static int16_t perspective_rotation = 0;
static void win_perspective_animate_cb(void *p)
{
    //gui_log("perspective_rotation %d", perspective_rotation);
    gui_win_t *this = (gui_win_t *)p;

    gui_obj_t *parent = this->base.parent;
    gui_perspective_t *perspective = (gui_perspective_t *)parent;

    perspective->release_x = perspective_rotation;

    int16_t step = 454 / BENCHMARK_MAX_FRAME + 1;
    if (perspective_rotation > -454)
    {
        perspective_rotation = perspective_rotation - step;
    }
}

void benchmark_perspective(void *parent)
{
    gui_perspective_imgfile_t image_list;
    memset(&image_list, 0, sizeof(gui_perspective_imgfile_t));

    for (int16_t i = 0; i < 6; i++)
    {
        image_list.src_mode[i] = IMG_SRC_MEMADDR;
    }
    image_list.data_addr[0] = A1_BIN;
    image_list.data_addr[1] = A2_BIN;
    image_list.data_addr[2] = A3_BIN;
    image_list.data_addr[3] = A4_BIN;
    image_list.data_addr[4] = A5_BIN;
    image_list.data_addr[5] = A6_BIN;
    gui_perspective_t *ppp = gui_perspective_create(parent, "ppp", &image_list, 0, 0);

    /*todo: add animate to rotate automaticlly*/
    gui_win_t *win = gui_win_create(ppp, "win_perspective_animate", 0, 0, 0, 0);
    gui_win_set_animate(win, 1, -1, win_perspective_animate_cb, win);


}

/*benchmark scenario 7*/
#define BENCHMARK_TEXT_NUM  14
static char *benchmark_text_display = "测测测测测测测测测测测测测测";
void benchmark_single_text(void *parent)
{
    gui_text_t *text[BENCHMARK_TEXT_NUM];
    int16_t y_offset = 32;
    for (uint8_t i = 0; i < BENCHMARK_TEXT_NUM; i++)
    {
        text[i] = gui_text_create(parent, "text_test", 0, y_offset * i, 454, 32);
        gui_text_set(text[i], benchmark_text_display, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(benchmark_text_display), 32);
        gui_text_type_set(text[i], SIMKAI_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    }
}

/*benchmark scenario 8*/
#define BENCHMARK_SCROLLTEXT_NUM    14
static char *benchmark_scrolltext_display = "测测测测测测测测测测测测测测";
void benchmark_single_scroll_text(void *parent)
{
    gui_scroll_text_t *scroll_text[BENCHMARK_SCROLLTEXT_NUM];
    int16_t y_offset = 32;
    for (uint8_t i = 0; i < BENCHMARK_SCROLLTEXT_NUM; i++)
    {
        scroll_text[i] = gui_scroll_text_create(parent, "text_test", 0, i * y_offset, 454, 32);
        gui_scroll_text_set(scroll_text[i], benchmark_scrolltext_display, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                            strlen(benchmark_scrolltext_display), 32);
        gui_scroll_text_scroll_set(scroll_text[i], SCROLL_X, 0, 0, 5000, 0);
    }
}

#if 0
/*show benchmark result*/
#define STRING_BUFFER_SIZE  512
static uint8_t index_show_scenario = 0;
static gui_text_t *text_scenario = NULL;
static int16_t print_benchmark_to_string(uint8_t scenario, char *string, int16_t string_size)
{
    uint8_t i = scenario;
    int16_t len = 0;
    benchmark_cost_t *cost = &benchmark_scenarios[i].input_prepare_cost_us;

    len = len + snprintf(string, string_size, "scenario %d\n%s\n",
                         benchmark_scenarios[i].benchmark_info->scenario, benchmark_scenarios[i].benchmark_info->name);

    len = len + snprintf(string + len, string_size - len,
                         "input pre (ms): cost-frame\nmax%u.%u-%u min%u.%u-%u avg%u.%u\n",
                         cost->time_max / 1000, cost->time_max % 1000, cost->time_max_frame,
                         cost->time_min / 1000, cost->time_min % 1000, cost->time_min_frame,
                         cost->time_avg / 1000, cost->time_avg % 1000);

    cost = &benchmark_scenarios[i].draw_prepare_cost_us;
    len = len + snprintf(string + len, string_size - len,
                         "draw pre (ms): cost-frame\nmax%u.%u-%u min%u.%u-%u avg%u.%u\n",
                         cost->time_max / 1000, cost->time_max % 1000, cost->time_max_frame,
                         cost->time_min / 1000, cost->time_min % 1000, cost->time_min_frame,
                         cost->time_avg / 1000, cost->time_avg % 1000);

    cost = &benchmark_scenarios[i].draw_cost_us;
    len = len + snprintf(string + len, string_size - len,
                         "draw (ms): cost-frame\nmax%u.%u-%u min%u.%u-%u avg%u.%u\n",
                         cost->time_max / 1000, cost->time_max % 1000, cost->time_max_frame,
                         cost->time_min / 1000, cost->time_min % 1000, cost->time_min_frame,
                         cost->time_avg / 1000, cost->time_avg % 1000);
    cost = &benchmark_scenarios[i].total_cost_us;
    len = len + snprintf(string + len, string_size - len,
                         "total (ms): cost-frame\nmax%u.%u-%u min%u.%u-%u avg%u.%u",
                         cost->time_max / 1000, cost->time_max % 1000, cost->time_max_frame,
                         cost->time_min / 1000, cost->time_min % 1000, cost->time_min_frame,
                         cost->time_avg / 1000, cost->time_avg % 1000);
    gui_log("%s", string);
    return len;
}

static void win_show_next_cb(void *p, uint16_t event)
{
    char *string_scenario = (char *)p;
    uint16_t size = STRING_BUFFER_SIZE;
    if (index_show_scenario <= BENCHMARK_SCENARIO_MAX - 1)
    {
        index_show_scenario ++;
        print_benchmark_to_string(index_show_scenario, string_scenario, size);
        gui_text_set(text_scenario, string_scenario, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(string_scenario), 32);
    }
}

static void win_show_prev_cb(void *p, uint16_t event)
{
    char *string_scenario = (char *)p;
    uint16_t size = STRING_BUFFER_SIZE;
    if (index_show_scenario > 0)
    {
        index_show_scenario --;
        print_benchmark_to_string(index_show_scenario, string_scenario, size);
        gui_text_set(text_scenario, string_scenario, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(string_scenario), 32);
    }
}

void design_benchmark_show(gui_app_t *app)
{
    uint16_t size = STRING_BUFFER_SIZE;
    int16_t len = 0;
    void *parent = &app->screen;
    char *string_scenario1 = os_mem_alloc(RAM_TYPE_DATA_ON, size);//gui_malloc(size);
    char *string_scenario2 = os_mem_alloc(RAM_TYPE_DATA_ON, size);//gui_malloc(size);
    memset(string_scenario1, 0, size);
    memset(string_scenario2, 0, size);

    gui_win_t *win_prev = gui_win_create(parent, "win_prev", 0, 0, 200, 454);
    gui_win_t *win_next = gui_win_create(parent, "win_next", 254, 0, 200, 454);
    gui_win_release(win_next, (gui_event_cb_t)win_show_next_cb, string_scenario2);
    gui_win_release(win_prev, (gui_event_cb_t)win_show_prev_cb, string_scenario1);

    len = print_benchmark_to_string(index_show_scenario, string_scenario1, size);

    text_scenario = gui_text_create(parent, "app", 0, 20, 454, 454);
    gui_text_set(text_scenario, string_scenario1, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_scenario1), 32);
    gui_text_mode_set(text_scenario, MULTI_CENTER);

}
#endif

void app_benchmark_start(void)
{
    /*enable frame monitor*/
    gui_dispdev_t *dc = gui_get_dc();
    dc->lcd_frame_monitor = &frame_monitor_benchmark;
#ifndef _WIN32
    /*enable flash dtr*/
    if (fmc_flash_try_high_speed_mode(FMC_FLASH_NOR_IDX0, FMC_FLASH_NOR_4_BIT_DTR_MODE))
    {
        DBG_DIRECT("flash switch 4bit success");
    }
    else
    {
        DBG_DIRECT("flash switch 4bit fail");
    }
    /*off te*/
    rtk_lcd_hal_transfer_done();
    gui_display_off();
    gui_display_on();
    extern bool first_frame_read_te_type;
    first_frame_read_te_type = true;
    rtk_lcd_hal_set_TE_type(LCDC_TE_TYPE_NO_TE);
#endif
    /*free all tabs before switch gui app*/
    benchmark_free_all_other_mgrs();
    //to do
    gui_switch_app(get_app_watch_ui(), get_app_benchmark());
}

void app_benchmark_finish(void)
{
    /*disable frame monitor*/
    gui_dispdev_t *dc = gui_get_dc();
    dc->lcd_frame_monitor = NULL;
    /*show cost ms on lcd*/
    //design_benchmark_show(get_app_benchmark());
    //to do
    gui_switch_app(get_app_benchmark(), get_app_watch_ui());
}

