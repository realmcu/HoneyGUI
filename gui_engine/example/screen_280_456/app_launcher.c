#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include "gui_canvas.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_grid.h"
#include "tp_algo.h"
#include <math.h>
#include "gui_cube.h"
#include "font_mem.h"
#include "font_stb.h"
#include "font_freetype.h"
#include "gui_scroll_text.h"
#include "gui_page.h"
#include "gui_components_init.h"
#include "app_launcher.h"
#include "gui_wheel_list.h"
#ifndef _WIN32
#include "os_mem.h"
#endif // !_WIN32

static void app_launcher_ui_design(gui_app_t *app);
static gui_app_t app_launcher =
{
    .screen =
    {
        .name = "app_launcher",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_launcher_ui_design,
    .active_ms = 1000 * 5,
};

gui_app_t *get_launcher_app(void)
{
    return &app_launcher;
}

static void app_wheel_ui_design(gui_app_t *app);
static gui_app_t app_wheel =
{
    .screen =
    {
        .name = "app_wheel",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_wheel_ui_design,
    .active_ms = 1000 * 5,
};

gui_app_t *get_wheel_app(void)
{
    return &app_wheel;
}

int xscale = 64;
int yscale = 64;
#define CHANGE_VALUE 8
void change_size(gui_img_t *obj)
{
    // gui_log("change_size xscale : %d , yscale : %d \n",xscale,yscale);
    if (xscale <= 256 - CHANGE_VALUE && yscale < xscale)
    {
        xscale += CHANGE_VALUE;
    }
    else if (xscale == 256 && yscale < 256)
    {
        yscale += CHANGE_VALUE;
    }
    else if (xscale == yscale && xscale > CHANGE_VALUE * 6)
    {
        xscale -= CHANGE_VALUE;
        yscale -= CHANGE_VALUE;
    }
    else if (xscale == CHANGE_VALUE * 6 && yscale == CHANGE_VALUE * 6)
    {
        xscale = CHANGE_VALUE * 7;
    }
    int a = (xscale + yscale) / 2 - 1;
    gui_img_scale(obj, (float)xscale / 256, (float)yscale / 256);
    if (a <= 255 && a >= 0)
    {
        gui_img_set_opacity(obj, a);;
    }
    else
    {
        gui_img_set_opacity(obj, 0);;
    }
}
static void tab_img(void *tab)
{
    gui_img_t *bg = gui_img_create_from_mem(tab, "bg", LAKE_BIN, 0, 0, 0, 0);
    gui_img_set_opacity(bg, 128);
    gui_img_t *ppe_test = gui_img_create_from_mem(tab, "ppe", TREE_BIN, 0, 0, 0, 0);
    gui_img_scale(ppe_test, (float)xscale / 256, (float)yscale / 256);
    // gui_img_set_animate(ppe_test, 1000, -1, change_size, ppe_test);
}
#define FONT_NUM 4
char *text_string[FONT_NUM] =
{
    "abcdefg",
    "1234567",
    "!@#$%^&",
    "QWERTYU",
};
static uint16_t uutf_16[] = {0x30, 0x31, 0x32, 0x33, 0X34};
static void tab_text(void *tab)
{
    gui_img_t *tree = gui_img_create_from_mem(tab, "tree", TREE_BIN, 0, 0, 0, 0);
#if 1
    gui_text_t *text2 = gui_text_create(tab,  "text2",  0, 35, 280, 50);
    gui_text_set(text2, "abcdefg", GUI_FONT_SRC_BMP, APP_COLOR_GREEN, 7, 48);
    gui_text_mode_set(text2, LEFT);

    gui_text_t *text3 = gui_text_create(tab,  "text3",  0, 85, 280, 50);
    gui_text_set(text3, uutf_16, GUI_FONT_SRC_BMP, APP_COLOR_BLUE, 10, 48);
    gui_text_mode_set(text3, CENTER);
    gui_text_encoding_set(text3, UTF_16);

    gui_text_t *text4 = gui_text_create(tab,  "text4",  0, 145, 280, 50);
    gui_text_set(text4, "!@#$", GUI_FONT_SRC_BMP, APP_COLOR_RED, 4, 64);
    gui_text_mode_set(text4, RIGHT);

    gui_text_t *text5 = gui_text_create(tab,  "text5",  0, 205, 280, 48 * 3);
    gui_text_set(text5, "12345678901234567890", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 20, 48);
    gui_text_mode_set(text5, MULTI_LEFT);

    gui_scroll_text_t *scrool_text1 = gui_scroll_text_create(tab,  "scrool_text1",  80, 300, 100, 50);
    gui_scroll_text_scroll_set(scrool_text1, SCROLL_X, 100, 0, 3000, 1500000);
    gui_scroll_text_set(scrool_text1, "0123456789", GUI_FONT_SRC_BMP, gui_rgb(0, 0xff, 0), 10, 32);

    gui_scroll_text_t *scrool_text2 = gui_scroll_text_create(tab,  "scrool_text2",  80, 360, 100, 60);
    gui_scroll_text_scroll_set(scrool_text2, SCROLL_Y, 100, 0, 3000, 1500000);
    gui_scroll_text_set(scrool_text2, "123456789012345678901234567890", GUI_FONT_SRC_BMP,
                        APP_COLOR_WHITE, 30, 32);
#else
    gui_color_t color[FONT_NUM];
    color[0] = APP_COLOR_WHITE;
    color[1] = APP_COLOR_RED;
    color[2] = APP_COLOR_GREEN;
    color[3] = APP_COLOR_BLUE;
    gui_text_t *text1[FONT_NUM];
    gui_text_t *text2[FONT_NUM];
    gui_text_t *text4[FONT_NUM];
    gui_text_t *text8[FONT_NUM];
#if 0
    for (int i = 0; i < FONT_NUM; i++)
    {
        text1[i] = gui_text_create(tab, "text1", 32 * i, 32 * i, 280, 32);
        gui_text_set(text1[i], text_string[i], GUI_FONT_SRC_TTF, color[i], strlen(text_string[i]), 32);
        gui_text_type_set(text1[i], QUICKSAND_MEDIUM_TTF, FONT_SRC_MEMADDR);
    }
#else
    for (int i = 0; i < FONT_NUM; i++)
    {
        text1[i] = gui_text_create(tab, "text1", 0, 32 * i + 32 * FONT_NUM * 0, 300, 32);
        gui_text_set(text1[i], text_string[i], GUI_FONT_SRC_BMP, color[i], strlen(text_string[i]), 32);
        gui_text_type_set(text1[i], HARMONYOS_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    }
    for (int i = 0; i < FONT_NUM; i++)
    {
        text2[i] = gui_text_create(tab, "text2", 0, 32 * i + 32 * FONT_NUM * 1, 300, 32);
        gui_text_set(text2[i], text_string[i], GUI_FONT_SRC_BMP, color[i], strlen(text_string[i]), 32);
        gui_text_type_set(text2[i], HARMONYOS_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    }
    for (int i = 0; i < FONT_NUM; i++)
    {
        text4[i] = gui_text_create(tab, "text4", 0, 32 * i + 32 * FONT_NUM * 2, 300, 32);
        gui_text_set(text4[i], text_string[i], GUI_FONT_SRC_BMP, color[i], strlen(text_string[i]), 32);
        gui_text_type_set(text4[i], HARMONYOS_SIZE32_BITS4_FONT_BIN, FONT_SRC_MEMADDR);
    }
    for (int i = 0; i < FONT_NUM; i++)
    {
        text8[i] = gui_text_create(tab, "text8", 0, 32 * i + 32 * FONT_NUM * 3, 300, 32);
        gui_text_set(text8[i], text_string[i], GUI_FONT_SRC_BMP, color[i], strlen(text_string[i]), 32);
        gui_text_type_set(text8[i], HARMONYOS_SIZE32_BITS8_FONT_BIN, FONT_SRC_MEMADDR);
    }
#endif
#endif
}
#define ICON_NUM 6
static void tab_iconlist(void *tab)
{
    gui_page_t *page = gui_page_create(tab, "page", 0, 0, 280,
                                       40 + 65 + 140 * (ICON_NUM - 1) + 10 + 40 + 20);
    uint8_t index = 0;
    gui_img_t *icon[ICON_NUM];
    gui_text_t *text[ICON_NUM];
    char *text_str[ICON_NUM] =
    {
        "alarm",
        "weather",
        "heart rate",
        "music",
        "sleep",
        "setting",
    };
    void *icon_addr[ICON_NUM] =
    {
        ICMENUALARM_BIN,
        ICMENUWEATHER_BIN,
        ICMENUHEARTRATE_BIN,
        ICMENUMUSIC_BIN,
        ICMENUSLEEP_BIN,
        ICMENUSETTING_BIN,
    };
    for (uint8_t i = 0; i < ICON_NUM; i++)
    {
        icon[i] = gui_img_create_from_mem(page, "icon0", icon_addr[i], 107, 40 + 140 * i, 65, 65);
        // text[i] = gui_text_create(page, "text0", 0, 40 + 65 + 140 * i + 10, 280, 40);
        // gui_text_set(text[i], text_str[i], GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(text_str[i]), 32);
        // gui_text_mode_set(text[i], CENTER);
    }
}
static void tab_two(void *tab)
{
    gui_img_t *lake = gui_img_create_from_mem(tab, "lake", LAKE_BIN, 0, 0, 0, 0);
    gui_img_scale(lake, 0.5, 0.5);
    gui_img_t *tree = gui_img_create_from_mem(tab, "tree", TREE_BIN, 140, 228, 0, 0);
    gui_img_scale(tree, 0.5, 0.5);
    gui_img_set_opacity(tree, 128);
    gui_img_set_opacity(lake, 128);
}
static void tab_lake(void *tab)
{
    gui_img_t *lake = gui_img_create_from_mem(tab, "lake", LAKE_BIN, 0, 0, 0, 0);
}
static void tab_tree(void *tab)
{
    gui_img_t *tree = gui_img_create_from_mem(tab, "tree", TREE_BIN, 0, 0, 0, 0);
}
static void curtain_center(gui_curtainview_t *curtainview)
{
    gui_curtain_t *curtain_center = gui_curtain_create(curtainview, "curtain_center", 0, 0, 280, 456,
                                                       CURTAIN_MIDDLE, 1);
    gui_img_t *tree = gui_img_create_from_mem(curtain_center, "tree", TREE_BIN, 0, 0, 0, 0);
    gui_img_set_opacity((gui_img_t *)tree, 96);

    gui_text_t *time = gui_text_create(curtain_center, "time",  0, 80, 280, 128);
    gui_text_set(time, "15:30", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, 5, 128);
    gui_text_type_set(time, QUICKSAND_MEDIUM_TTF, FONT_SRC_MEMADDR);
    gui_text_mode_set(time, CENTER);

    int w_offset = gui_get_mem_utf8_char_width("Sun 6/6/2023", HARMONYOS_SIZE32_BITS4_FONT_BIN);
    gui_text_t *date = gui_text_create(curtain_center, "date", (280 - w_offset) / 2, 180, 280, 100);
    gui_text_set(date, "Sun 6/6/2023", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12, 32);
    gui_text_mode_set(date, LEFT);
    gui_text_type_set(date, HARMONYOS_SIZE32_BITS8_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_convert_to_img(date, ARGB8888);

    gui_img_t *bt = gui_img_create_from_mem(curtain_center, "bt", ICBLECONNECT24_BIN, 150, 3, 0, 0);
    gui_img_t *bat = gui_img_create_from_mem(curtain_center, "bat", ICBATTERYLVL4_BIN, 185, 6, 0, 0);

    gui_text_t *electric_quantity = gui_text_create(curtain_center, "electric_quantity", 228, 0, 52,
                                                    20);
    gui_text_set(electric_quantity, "80", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 2, 32);
}
static void switch_to_wheel(void *obj, gui_event_t event, void *param)
{
    gui_switch_app(get_launcher_app(), get_wheel_app());
}
static void curtain_up(gui_curtainview_t *curtainview)
{
    gui_curtain_t *curtain_up = gui_curtain_create(curtainview, "curtain_up", 0, 0, 0, 0, CURTAIN_UP,
                                                   0.5f);
    gui_img_t *bg = gui_img_create_from_mem(curtain_up, "bg", CURTAIN_HALF_BACKGROUND_90_BIN, -20, 228,
                                            0, 0);
    gui_img_t *app1 = gui_img_create_from_mem(curtain_up, "app1", APP_GOOGLE_BIN, 5, 238, 0, 0);
    gui_obj_add_event_cb(app1, switch_to_wheel, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *app2 = gui_img_create_from_mem(curtain_up, "app2", APP_FACEBOOK_BIN, 74, 238, 0, 0);
    gui_img_t *app3 = gui_img_create_from_mem(curtain_up, "app3", APP_GMAIL_BIN, 142, 238, 0, 0);
    gui_img_t *app4 = gui_img_create_from_mem(curtain_up, "app4", APP_INSTAGRAM_BIN, 211, 238, 0, 0);
    gui_img_t *app5 = gui_img_create_from_mem(curtain_up, "app5", APP_LINKEDIN_BIN, 5, 310, 0, 0);
    gui_img_t *app6 = gui_img_create_from_mem(curtain_up, "app6", APP_TWITTER_BIN, 74, 310, 0, 0);
    gui_img_t *app7 = gui_img_create_from_mem(curtain_up, "app7", APP_WHATSAPP_BIN, 142, 310, 0, 0);
    gui_img_t *app8 = gui_img_create_from_mem(curtain_up, "app8", APP_YOUTUBE_BIN, 211, 310, 0, 0);
}
#include "gui_turn_table.h"
const gui_turn_table_coordinate_t point[12] =
{
    //left
    {50, 250},
    {50, 150},
    //up
    {100, 50},
    {205, 50},
    {310, 50},
    //right
    {360, 150},
    {360, 250},
    {360, 350},
    //down
    {310, 450},
    {205, 450},
    {100, 450},
    //left
    {50, 350}
};

static void tab_tt(void *tab)
{
    gui_turn_table_t *tt = gui_turn_table_create(tab, 24, 32, 3, 3);
    gui_turn_table_set_icon_scale(tt, 1.0f, 2.0f);
    gui_turn_table_set_highlight_index(tt, 9);
    gui_turn_table_generate_layout_by_array(tt, (gui_turn_table_coordinate_t *)point, 12);

    gui_turn_table_add_icon_default(tt, APP_FACEBOOK_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_FACEBOOK_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_FACEBOOK_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_INSTAGRAM_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_TWITTER_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_WHATSAPP_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_YOUTUBE_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_GMAIL_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_INSTAGRAM_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_TWITTER_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_WHATSAPP_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_YOUTUBE_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_GMAIL_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_INSTAGRAM_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_TWITTER_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_WHATSAPP_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_YOUTUBE_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_GMAIL_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_INSTAGRAM_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_TWITTER_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_WHATSAPP_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_YOUTUBE_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_GMAIL_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_GOOGLE_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_GOOGLE_BIN, NULL);
    gui_turn_table_add_icon_default(tt, APP_GOOGLE_BIN, NULL);

    gui_turn_table_add_icon_static(tt, ICMENUHEARTRATE_BIN);
}
static void tab_home(void *tab)
{
    gui_curtainview_t *curtainview = gui_curtainview_create(tab, "curtainview", 0, 0, 0, 0);
    curtain_center(curtainview);
    curtain_up(curtainview);
}
static void app_launcher_ui_design(gui_app_t *app)
{
    gui_log("app_launcher_ui_design\n");
#ifndef _WIN32
    size_t heap;
    gui_log("before ui unused mem head is %d", heap = os_mem_peek(RAM_TYPE_DATA_ON));
#endif
    gui_font_stb_init(QUICKSAND_MEDIUM_TTF);
    gui_font_mem_init(HARMONYOS_SIZE32_BITS1_FONT_BIN);
    gui_font_mem_init(HARMONYOS_SIZE32_BITS2_FONT_BIN);
    gui_font_mem_init(HARMONYOS_SIZE32_BITS4_FONT_BIN);
    gui_font_mem_init(HARMONYOS_SIZE32_BITS8_FONT_BIN);
    gui_font_mem_init(HARMONYOS_SIZE48_BITS4_FONT_BIN);
    gui_font_mem_init(HARMONYOS_SIZE64_BITS4_FONT_BIN);

    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tabview_set_style(tv, REDUCTION);
    gui_tab_t *tabn = gui_tab_create(tv, "tab0", 0, 0, 0, 0, -1, 0);
    gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);
    gui_tab_t *tab1 = gui_tab_create(tv, "tab1", 0, 0, 0, 0, 1, 0);
    gui_tab_t *tab2 = gui_tab_create(tv, "tab2", 0, 0, 0, 0, 2, 0);
    gui_tab_t *tab3 = gui_tab_create(tv, "tab3", 0, 0, 0, 0, 3, 0);
    gui_tab_t *tab4 = gui_tab_create(tv, "tab4", 0, 0, 0, 0, 4, 0);
    gui_tab_t *tab5 = gui_tab_create(tv, "tab5", 0, 0, 0, 0, 5, 0);

    tab_tt(tabn);
    tab_home(tab0);
    tab_text(tab1);
    tab_tree(tab2);
    tab_img(tab3);
    tab_lake(tab4);
    tab_iconlist(tab5);
#ifndef _WIN32
    gui_log("after ui unused mem head is %d", heap = os_mem_peek(RAM_TYPE_DATA_ON));
#endif
}

static void wheel_cb(void *obj, gui_event_t event, void *param)
{
    gui_img_t *img = (gui_img_t *)obj;
    gui_log("img p %p, x %d, y %d \n", obj, img->base.x, img->base.y);
}
static void switch_to_launcher(void *obj, gui_event_t event, void *param)
{
    gui_switch_app(get_wheel_app(), get_launcher_app());
}
static void app_wheel_ui_design(gui_app_t *app)
{
    gui_wheel_list_t *hc = gui_wheel_list_create(&app->screen, 2, 32, 80);
    gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, switch_to_launcher);

    gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, wheel_cb);
    gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, wheel_cb);
    gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, wheel_cb);
    gui_wheel_list_add_icon_default(hc, ICMENUMUSIC_BIN, wheel_cb);
    gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, wheel_cb);
    gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, wheel_cb);
    gui_wheel_list_set_icon(hc, ICMENUALARM_BIN, wheel_cb, 1, 0);

    gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUMUSIC_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUMUSIC_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, NULL);
    gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, NULL);
}

#if defined _WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static int app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./gui_engine/example/screen_280_456/root_image_8762g/root(0x4400000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x4400000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }
#endif
    gui_server_init();
    gui_app_startup(get_launcher_app());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
