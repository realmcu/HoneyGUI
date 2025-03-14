/**
 * @file app_music.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "app_music.h"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if APP_MUSIC_AUTO_PLAY
static void auto_step_cb(lv_timer_t *timer);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *ctrl;
static lv_obj_t *list;

static const char *title_list[] =
{
    "Waiting for true love",
    "Need a Better Future",
    "Vibrations",
    "Why now?",
    "Never Look Back",
    "It happened Yesterday",
    "Feeling so High",
    "Go Deeper",
    "Find You There",
    "Until the End",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
};

static const char *artist_list[] =
{
    "The John Smith Band",
    "My True Name",
    "Robotics",
    "John Smith",
    "My True Name",
    "Robotics",
    "Robotics",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
};

static const char *genre_list[] =
{
    "Rock - 1997",
    "Drum'n bass - 2016",
    "Psy trance - 2020",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
};

static const uint32_t time_list[] =
{
    1 * 60 + 14,
    2 * 60 + 26,
    1 * 60 + 54,
    2 * 60 + 24,
    2 * 60 + 37,
    3 * 60 + 33,
    1 * 60 + 56,
    3 * 60 + 31,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
};

/**********************
 *      MACROS
 **********************/

static lv_obj_t *app_music_list_create(lv_obj_t *parent);
static void app_music_list_button_check(uint32_t track_id, bool state);
static lv_obj_t *app_music_main_create(lv_obj_t *parent);
static void app_music_play(uint32_t id);
static void app_music_resume(void);
static void app_music_pause(void);
static void app_music_album_next(bool next);


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void app_music(lv_obj_t *parent)
{
    // lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x343247), 0);

    list = app_music_list_create(parent);
    ctrl = app_music_main_create(parent);

#if APP_MUSIC_AUTO_PLAY
    lv_timer_create(auto_step_cb, 1000, NULL);
#endif
}

const char *app_music_get_title(uint32_t track_id)
{
    if (track_id >= sizeof(title_list) / sizeof(title_list[0])) { return NULL; }
    return title_list[track_id];
}

const char *app_music_get_artist(uint32_t track_id)
{
    if (track_id >= sizeof(artist_list) / sizeof(artist_list[0])) { return NULL; }
    return artist_list[track_id];
}

const char *app_music_get_genre(uint32_t track_id)
{
    if (track_id >= sizeof(genre_list) / sizeof(genre_list[0])) { return NULL; }
    return genre_list[track_id];
}

uint32_t app_music_get_track_length(uint32_t track_id)
{
    if (track_id >= sizeof(time_list) / sizeof(time_list[0])) { return 0; }
    return time_list[track_id];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if APP_MUSIC_AUTO_PLAY
static void auto_step_cb(lv_timer_t *t)
{
    LV_UNUSED(t);
    static uint32_t state = 0;

    switch (state)
    {
    case 5:
        app_music_album_next(true);
        break;

    case 6:
        app_music_album_next(true);
        break;
    case 7:
        app_music_album_next(true);
        break;
    case 8:
        app_music_play(0);
        break;
#if APP_MUSIC_SQUARE || APP_MUSIC_ROUND
    case 11:
        lv_obj_scroll_by(ctrl, 0, -LV_VER_RES, LV_ANIM_ON);
        break;
    case 13:
        lv_obj_scroll_by(ctrl, 0, -LV_VER_RES, LV_ANIM_ON);
        break;
#else
    case 12:
        lv_obj_scroll_by(ctrl, 0, -LV_VER_RES, LV_ANIM_ON);
        break;
#endif
    case 15:
        lv_obj_scroll_by(list, 0, -300, LV_ANIM_ON);
        break;
    case 16:
        lv_obj_scroll_by(list, 0, 300, LV_ANIM_ON);
        break;
    case 18:
        app_music_play(1);
        break;
    case 19:
        lv_obj_scroll_by(ctrl, 0, LV_VER_RES, LV_ANIM_ON);
        break;
#if APP_MUSIC_SQUARE || APP_MUSIC_ROUND
    case 20:
        lv_obj_scroll_by(ctrl, 0, LV_VER_RES, LV_ANIM_ON);
        break;
#endif
    case 30:
        app_music_play(2);
        break;
    case 40:
        {
            lv_obj_t *bg = lv_layer_top();
            lv_obj_set_style_bg_color(bg, lv_color_hex(0x6f8af6), 0);
            lv_obj_set_style_text_color(bg, lv_color_white(), 0);
            lv_obj_set_style_bg_opa(bg, LV_OPA_COVER, 0);
            lv_obj_fade_in(bg, 400, 0);
            lv_obj_t *dsc = lv_label_create(bg);
            lv_obj_set_style_text_font(dsc, font_small, 0);
            lv_label_set_text(dsc, "The average FPS is");
            lv_obj_align(dsc, LV_ALIGN_TOP_MID, 0, 90);

            lv_obj_t *num = lv_label_create(bg);
            lv_obj_set_style_text_font(num, font_large, 0);
#if LV_USE_PERF_MONITOR
            lv_display_t *disp = lv_display_get_default();
            const lv_sysmon_perf_info_t *info = lv_subject_get_pointer(&disp->perf_sysmon_backend.subject);
            lv_label_set_text_fmt(num, "%" LV_PRIu32, info->calculated.fps_avg_total);
#endif
            lv_obj_align(num, LV_ALIGN_TOP_MID, 0, 120);

            lv_obj_t *attr = lv_label_create(bg);
            lv_obj_set_style_text_align(attr, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_set_style_text_font(attr, font_small, 0);
#if APP_MUSIC_SQUARE || APP_MUSIC_ROUND
            lv_label_set_text(attr, "Copyright 2020 LVGL Kft.\nwww.lvgl.io | lvgl@lvgl.io");
#else
            lv_label_set_text(attr, "Copyright 2020 LVGL Kft. | www.lvgl.io | lvgl@lvgl.io");
#endif
            lv_obj_align(attr, LV_ALIGN_BOTTOM_MID, 0, -10);
            break;
        }
    case 41:
        lv_screen_load(lv_obj_create(NULL));
        app_music_pause();
        break;
    }
    state++;
}

#endif /*APP_MUSIC_AUTO_PLAY*/

/*APP_MUSIC_LIST*/

/**********************
 *      DEFINES
 **********************/
#define TRACK_COUNT 14  /* The number of tracks in the list */

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t *add_list_button(lv_obj_t *parent, uint32_t track_id);
static void btn_click_event_cb(lv_event_t *e);
static void list_delete_event_cb(lv_event_t *e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *list;
static const lv_font_t *font_small;
static const lv_font_t *font_medium;
static const lv_font_t *font_large;
static lv_style_t style_scrollbar;
static lv_style_t style_btn;
static lv_style_t style_button_pr;
static lv_style_t style_button_chk;
static lv_style_t style_button_dis;
static lv_style_t style_title;
static lv_style_t style_artist;
static lv_style_t style_time;


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static lv_obj_t *app_music_list_create(lv_obj_t *parent)
{
    font_small = &SourceHanSansSC_size12_bits1_font;
    font_medium = &SourceHanSansSC_size24_bits1_font;
    font_large = &SourceHanSansSC_size32_bits1_font;

    lv_style_init(&style_scrollbar);
    lv_style_set_width(&style_scrollbar, 4);
//    lv_style_set_bg_opa(&style_scrollbar, LV_OPA_COVER);
    lv_style_set_bg_color(&style_scrollbar, lv_color_hex3(0xeee));
    lv_style_set_radius(&style_scrollbar, LV_RADIUS_CIRCLE);
    lv_style_set_pad_right(&style_scrollbar, 4);

    static const int32_t grid_cols[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
#if APP_MUSIC_LARGE
    static const int32_t grid_rows[] = {35, 30, LV_GRID_TEMPLATE_LAST};
#else
    static const int32_t grid_rows[] = {22, 17, LV_GRID_TEMPLATE_LAST};
#endif
    lv_style_init(&style_btn);
    lv_style_set_bg_opa(&style_btn, LV_OPA_TRANSP);
    lv_style_set_grid_column_dsc_array(&style_btn, grid_cols);
    lv_style_set_grid_row_dsc_array(&style_btn, grid_rows);
    lv_style_set_grid_row_align(&style_btn, LV_GRID_ALIGN_CENTER);
    lv_style_set_layout(&style_btn, LV_LAYOUT_GRID);
#if APP_MUSIC_LARGE
    lv_style_set_pad_right(&style_btn, 30);
#else
    lv_style_set_pad_right(&style_btn, 20);
#endif
    lv_style_init(&style_button_pr);
    lv_style_set_bg_opa(&style_button_pr, LV_OPA_COVER);
    lv_style_set_bg_color(&style_button_pr, lv_color_hex(0x4c4965));

    lv_style_init(&style_button_chk);
    lv_style_set_bg_opa(&style_button_chk, LV_OPA_COVER);
    lv_style_set_bg_color(&style_button_chk, lv_color_hex(0x4c4965));

    lv_style_init(&style_button_dis);
    lv_style_set_text_opa(&style_button_dis, LV_OPA_40);
    lv_style_set_image_opa(&style_button_dis, LV_OPA_40);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_medium);
    lv_style_set_text_color(&style_title, lv_color_hex(0xffffff));

    lv_style_init(&style_artist);
    lv_style_set_text_font(&style_artist, font_small);
    lv_style_set_text_color(&style_artist, lv_color_hex(0xb1b0be));

    lv_style_init(&style_time);
    lv_style_set_text_font(&style_time, font_medium);
    lv_style_set_text_color(&style_time, lv_color_hex(0xffffff));

    /*Create an empty transparent container*/
    list = lv_obj_create(parent);
    lv_obj_add_event_cb(list, list_delete_event_cb, LV_EVENT_DELETE, NULL);
    lv_obj_remove_style_all(list);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - APP_MUSIC_HANDLE_SIZE);
    lv_obj_set_y(list, APP_MUSIC_HANDLE_SIZE);
    lv_obj_add_style(list, &style_scrollbar, LV_PART_SCROLLBAR);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

    uint32_t track_id;
    for (track_id = 0; track_id < TRACK_COUNT; track_id++)
    {
        add_list_button(list, track_id);
    }

#if APP_MUSIC_ROUND
    lv_obj_set_scroll_snap_y(list, LV_SCROLL_SNAP_CENTER);
#endif

    app_music_list_button_check(0, true);

    return list;
}

static void app_music_list_button_check(uint32_t track_id, bool state)
{
    lv_obj_t *btn = lv_obj_get_child(list, track_id);
    lv_obj_t *icon = lv_obj_get_child(btn, 0);

    if (state)
    {
        lv_obj_add_state(btn, LV_STATE_CHECKED);
        lv_image_set_src(icon, &app_music_btn_list_pause);
        lv_obj_scroll_to_view(btn, LV_ANIM_ON);
    }
    else
    {
        lv_obj_remove_state(btn, LV_STATE_CHECKED);
        lv_image_set_src(icon, &app_music_btn_list_play);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t *add_list_button(lv_obj_t *parent, uint32_t track_id)
{
    uint32_t t = app_music_get_track_length(track_id);
    char time[32];
    lv_snprintf(time, sizeof(time), "%"LV_PRIu32":%02"LV_PRIu32, t / 60, t % 60);
    const char *title = app_music_get_title(track_id);
    const char *artist = app_music_get_artist(track_id);

    lv_obj_t *btn = lv_obj_create(parent);
    lv_obj_remove_style_all(btn);
#if APP_MUSIC_LARGE
    lv_obj_set_size(btn, lv_pct(100), 110);
#else
    lv_obj_set_size(btn, lv_pct(100), 60);
#endif

    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_add_style(btn, &style_button_pr, LV_STATE_PRESSED);
    lv_obj_add_style(btn, &style_button_chk, LV_STATE_CHECKED);
    lv_obj_add_style(btn, &style_button_dis, LV_STATE_DISABLED);
    lv_obj_add_event_cb(btn, btn_click_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *icon = lv_image_create(btn);
    lv_image_set_src(icon, &app_music_btn_list_play);
    lv_obj_set_grid_cell(icon, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 2);

    lv_obj_t *title_label = lv_label_create(btn);
    lv_label_set_text(title_label, title);
    lv_obj_set_grid_cell(title_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_add_style(title_label, &style_title, 0);

    lv_obj_t *artist_label = lv_label_create(btn);
    lv_label_set_text(artist_label, artist);
    lv_obj_add_style(artist_label, &style_artist, 0);
    lv_obj_set_grid_cell(artist_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_obj_t *time_label = lv_label_create(btn);
    lv_label_set_text(time_label, time);
    lv_obj_add_style(time_label, &style_time, 0);
    lv_obj_set_grid_cell(time_label, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 0, 2);

    lv_obj_t *border = lv_image_create(btn);
    lv_image_set_src(border, &app_music_list_border);
    lv_image_set_inner_align(border, LV_IMAGE_ALIGN_TILE);
    lv_obj_set_width(border, lv_pct(120));
    lv_obj_align(border, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(border, LV_OBJ_FLAG_IGNORE_LAYOUT);

    return btn;
}

static void btn_click_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    uint32_t idx = lv_obj_get_index(btn);
    app_music_play(idx);
}

static void list_delete_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_DELETE)
    {
        lv_style_reset(&style_scrollbar);
        lv_style_reset(&style_btn);
        lv_style_reset(&style_button_pr);
        lv_style_reset(&style_button_chk);
        lv_style_reset(&style_button_dis);
        lv_style_reset(&style_title);
        lv_style_reset(&style_artist);
        lv_style_reset(&style_time);
    }
}

/*APP_MUSIC_MAIN*/

/*********************
 *      DEFINES
 *********************/
#define ACTIVE_TRACK_CNT    3
#define INTRO_TIME          2000


/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t *create_cont(lv_obj_t *parent);
static void create_wave_images(lv_obj_t *parent);
static lv_obj_t *create_title_box(lv_obj_t *parent);
static lv_obj_t *create_icon_box(lv_obj_t *parent);
static lv_obj_t *create_spectrum_obj(lv_obj_t *parent);
static lv_obj_t *create_ctrl_box(lv_obj_t *parent);
static lv_obj_t *create_handle(lv_obj_t *parent);

static void scale_anim_cb(void *a, int32_t v);
static void del_counter_timer_cb(lv_event_t *e);
static void spectrum_draw_event_cb(lv_event_t *e);
static lv_obj_t *album_image_create(lv_obj_t *parent);
static void album_gesture_event_cb(lv_event_t *e);
static void play_event_click_cb(lv_event_t *e);
static void prev_click_event_cb(lv_event_t *e);
static void next_click_event_cb(lv_event_t *e);
static void timer_cb(lv_timer_t *t);
static void track_load(uint32_t id);
static void album_fade_anim_cb(void *var, int32_t v);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *main_cont;
static lv_obj_t *spectrum_obj;
static lv_obj_t *title_label;
static lv_obj_t *artist_label;
static lv_obj_t *genre_label;
static lv_obj_t *time_obj;
static lv_obj_t *album_image_obj;
static lv_obj_t *slider_obj;
static uint32_t time_act;
static lv_timer_t *sec_counter_timer;
static uint32_t track_id;
static bool playing;
static lv_obj_t *play_obj;

// (LV_SCALE_NONE = 256)
static const int32_t scale_values[] =
{
    256, 258, 256, 258, 256, 258, 262, 266, 270, 266, 262, 258
};
static const uint32_t scale_values_len = sizeof(scale_values) / sizeof(scale_values[0]);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void _image_set_scale_anim_cb(void *obj, int32_t scale)
{
    lv_image_set_scale((lv_obj_t *)obj, (uint16_t)scale);
}

static void _obj_set_x_anim_cb(void *obj, int32_t x)
{
    lv_obj_set_x((lv_obj_t *)obj, (int32_t)x);
}

static lv_obj_t *app_music_main_create(lv_obj_t *parent)
{
    lv_obj_t *cont = create_cont(parent);

    create_wave_images(cont);
    lv_obj_t *title_box = create_title_box(cont);
    lv_obj_t *icon_box = create_icon_box(cont);
    lv_obj_t *ctrl_box = create_ctrl_box(cont);
    spectrum_obj = create_spectrum_obj(cont);
    lv_obj_t *handle_box = create_handle(cont);

#if APP_MUSIC_ROUND
    lv_obj_set_style_pad_hor(cont, LV_HOR_RES / 6, 0);
#endif

#if APP_MUSIC_SQUARE || APP_MUSIC_ROUND
    static const int32_t grid_cols[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_rows[] = {APP_MUSIC_HANDLE_SIZE, 0, LV_GRID_CONTENT, LV_GRID_FR(3), LV_GRID_CONTENT, LV_GRID_FR(3), LV_GRID_CONTENT, LV_GRID_FR(3), LV_GRID_CONTENT, APP_MUSIC_HANDLE_SIZE, LV_GRID_TEMPLATE_LAST};
    grid_rows[1] = LV_VER_RES;
    lv_obj_set_grid_dsc_array(cont, grid_cols, grid_rows); //  personally to grid_rows
    lv_obj_set_style_grid_row_align(cont, LV_GRID_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_grid_cell(spectrum_obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(title_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(icon_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(ctrl_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(handle_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 8, 1);
#elif APP_MUSIC_LANDSCAPE == 0
    static const int32_t grid_cols[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static const int32_t grid_rows[] = {APP_MUSIC_HANDLE_SIZE, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_FR(3), LV_GRID_CONTENT, LV_GRID_FR(3), 250, LV_GRID_FR(3), LV_GRID_CONTENT, LV_GRID_FR(3), LV_GRID_CONTENT, LV_GRID_FR(1), APP_MUSIC_HANDLE_SIZE, LV_GRID_TEMPLATE_LAST};
#if APP_MUSIC_LARGE
    grid_rows[6] = 480;
#endif
    lv_obj_set_grid_dsc_array(cont, grid_cols, grid_rows);
    lv_obj_set_style_grid_row_align(cont, LV_GRID_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_grid_cell(title_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(icon_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(spectrum_obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(ctrl_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 8, 1);
    lv_obj_set_grid_cell(handle_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 10, 1);
#else
    static const int32_t grid_cols[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static const int32_t grid_rows[] = {APP_MUSIC_HANDLE_SIZE, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_FR(3), LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_FR(1), APP_MUSIC_HANDLE_SIZE, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(cont, grid_cols, grid_rows);
    lv_obj_set_style_grid_row_align(cont, LV_GRID_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_grid_cell(title_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(icon_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(ctrl_box, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(handle_box, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 8, 1);
    lv_obj_set_grid_cell(spectrum_obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 9);
#endif

    sec_counter_timer = lv_timer_create(timer_cb, 1000, NULL);
    lv_timer_pause(sec_counter_timer);

    lv_anim_t a;
    lv_anim_init(&a);

    lv_obj_fade_in(title_box, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(icon_box, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(ctrl_box, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(handle_box, 1000, INTRO_TIME + 1000);
    lv_obj_fade_in(album_image_obj, 800, INTRO_TIME + 1000);
    lv_obj_fade_in(spectrum_obj, 0, INTRO_TIME);

    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_var(&a, album_image_obj);
    lv_anim_set_duration(&a, 1000);
    lv_anim_set_delay(&a, INTRO_TIME + 1000);
    lv_anim_set_values(&a, 1, LV_SCALE_NONE);
    lv_anim_set_exec_cb(&a, _image_set_scale_anim_cb);
    lv_anim_set_completed_cb(&a, NULL);
    lv_anim_start(&a);

    lv_obj_t *logo = lv_image_create(lv_screen_active());
    lv_image_set_src(logo, &logo);
    lv_obj_move_foreground(logo);
    lv_obj_align_to(logo, spectrum_obj, LV_ALIGN_CENTER, 0, 0);

#if APP_MUSIC_SQUARE == 0 && APP_MUSIC_ROUND == 0
    lv_obj_t *title = lv_label_create(lv_screen_active());
    lv_label_set_text(title, "LVGL Demo\nMusic player");
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(title, font_large, 0);
    lv_obj_set_style_text_line_space(title, 8, 0);
    lv_obj_fade_out(title, 500, INTRO_TIME);
    lv_obj_align_to(title, logo, LV_ALIGN_OUT_LEFT_MID, -20, 0);
#endif

    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_set_var(&a, logo);
    lv_anim_set_duration(&a, 400);
    lv_anim_set_delay(&a, INTRO_TIME + 800);
    lv_anim_set_values(&a, LV_SCALE_NONE, 10);
    lv_anim_set_completed_cb(&a, lv_obj_delete_anim_completed_cb);
    lv_anim_start(&a);

    lv_obj_update_layout(main_cont);

    return main_cont;
}

static void app_music_album_next(bool next)
{
    uint32_t id = track_id;
    if (next)
    {
        id++;
        if (id >= ACTIVE_TRACK_CNT) { id = 0; }
    }
    else
    {
        if (id == 0)
        {
            id = ACTIVE_TRACK_CNT - 1;
        }
        else
        {
            id--;
        }
    }

    if (playing)
    {
        app_music_play(id);
    }
    else
    {
        track_load(id);
    }
}

static void app_music_play(uint32_t id)
{
    track_load(id);
    app_music_resume();
}

static void app_music_resume(void)
{
    playing = true;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_values(&a, 0, scale_values_len - 1);
    lv_anim_set_exec_cb(&a, scale_anim_cb);
    lv_anim_set_var(&a, album_image_obj);
    lv_anim_set_duration(&a, 2000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

    if (sec_counter_timer) { lv_timer_resume(sec_counter_timer); }
    lv_slider_set_range(slider_obj, 0, app_music_get_track_length(track_id));
    lv_obj_add_state(play_obj, LV_STATE_CHECKED);
}

static void app_music_pause(void)
{
    playing = false;
    lv_anim_delete(album_image_obj, scale_anim_cb);
    lv_image_set_scale(album_image_obj, LV_SCALE_NONE);
    if (sec_counter_timer) { lv_timer_pause(sec_counter_timer); }
    lv_obj_remove_state(play_obj, LV_STATE_CHECKED);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t *create_cont(lv_obj_t *parent)
{
    main_cont = lv_obj_create(parent);
    lv_obj_remove_flag(main_cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(main_cont, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_remove_style_all(main_cont);
    lv_obj_set_size(main_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_scroll_snap_y(main_cont, LV_SCROLL_SNAP_CENTER);

    lv_obj_t *player = lv_obj_create(main_cont);
    lv_obj_set_y(player, -APP_MUSIC_HANDLE_SIZE);
#if APP_MUSIC_SQUARE || APP_MUSIC_ROUND
    lv_obj_set_size(player, LV_HOR_RES, 2 * LV_VER_RES + APP_MUSIC_HANDLE_SIZE * 2);
#else
    lv_obj_set_size(player, LV_HOR_RES, LV_VER_RES + APP_MUSIC_HANDLE_SIZE * 2);
#endif
    lv_obj_remove_flag(player, LV_OBJ_FLAG_SNAPPABLE);
    lv_obj_set_style_bg_color(player, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_width(player, 0, 0);
    lv_obj_set_style_pad_all(player, 0, 0);
    lv_obj_set_scroll_dir(player, LV_DIR_VER);

    lv_obj_t *placeholder1 = lv_obj_create(main_cont);
    lv_obj_remove_style_all(placeholder1);
    lv_obj_remove_flag(placeholder1, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *placeholder2 = lv_obj_create(main_cont);
    lv_obj_remove_style_all(placeholder2);
    lv_obj_remove_flag(placeholder2, LV_OBJ_FLAG_CLICKABLE);

#if APP_MUSIC_SQUARE || APP_MUSIC_ROUND
    lv_obj_t *placeholder3 = lv_obj_create(main_cont);
    lv_obj_remove_style_all(placeholder3);
    lv_obj_remove_flag(placeholder3, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(placeholder1, lv_pct(100), LV_VER_RES);
    lv_obj_set_y(placeholder1, 0);
    lv_obj_set_size(placeholder2, lv_pct(100), LV_VER_RES);
    lv_obj_set_y(placeholder2, LV_VER_RES);
    lv_obj_set_size(placeholder3, lv_pct(100), LV_VER_RES - 2 * APP_MUSIC_HANDLE_SIZE);
    lv_obj_set_y(placeholder3, 2 * LV_VER_RES + APP_MUSIC_HANDLE_SIZE);
#else
    lv_obj_set_size(placeholder1, lv_pct(100), LV_VER_RES);
    lv_obj_set_y(placeholder1, 0);
    lv_obj_set_size(placeholder2, lv_pct(100), LV_VER_RES - 2 * APP_MUSIC_HANDLE_SIZE);
    lv_obj_set_y(placeholder2, LV_VER_RES + APP_MUSIC_HANDLE_SIZE);
#endif

    lv_obj_update_layout(main_cont);
    return player;
}

static void create_wave_images(lv_obj_t *parent)
{

    lv_obj_t *wave_top = lv_image_create(parent);
    lv_image_set_src(wave_top, &app_music_wave_top);
    lv_image_set_inner_align(wave_top, LV_IMAGE_ALIGN_TILE);
    lv_obj_set_width(wave_top, LV_HOR_RES);
    lv_obj_align(wave_top, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_flag(wave_top, LV_OBJ_FLAG_IGNORE_LAYOUT);

    lv_obj_t *wave_bottom = lv_image_create(parent);
    lv_image_set_src(wave_bottom, &app_music_wave_bottom);
    lv_image_set_inner_align(wave_bottom, LV_IMAGE_ALIGN_TILE);
    lv_obj_set_width(wave_bottom, LV_HOR_RES);
    lv_obj_align(wave_bottom, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(wave_bottom, LV_OBJ_FLAG_IGNORE_LAYOUT);


    lv_obj_t *wave_corner = lv_image_create(parent);
    lv_image_set_src(wave_corner, &app_music_corner_left);
#if APP_MUSIC_ROUND == 0
    lv_obj_align(wave_corner, LV_ALIGN_BOTTOM_LEFT, 0, 0);
#else
    lv_obj_align(wave_corner, LV_ALIGN_BOTTOM_LEFT, -LV_HOR_RES / 6, 0);
#endif
    lv_obj_add_flag(wave_corner, LV_OBJ_FLAG_IGNORE_LAYOUT);

    wave_corner = lv_image_create(parent);
    lv_image_set_src(wave_corner, &app_music_corner_right);
#if APP_MUSIC_ROUND == 0
    lv_obj_align(wave_corner, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
#else
    lv_obj_align(wave_corner, LV_ALIGN_BOTTOM_RIGHT, LV_HOR_RES / 6, 0);
#endif
    lv_obj_add_flag(wave_corner, LV_OBJ_FLAG_IGNORE_LAYOUT);
}

static lv_obj_t *create_title_box(lv_obj_t *parent)
{
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_height(cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    title_label = lv_label_create(cont);
    lv_obj_set_style_text_font(title_label, font_large, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0x504d6d), 0);
    lv_label_set_text(title_label, app_music_get_title(track_id));
    lv_obj_set_height(title_label, lv_font_get_line_height(font_large) * 3 / 2);

    artist_label = lv_label_create(cont);
    lv_obj_set_style_text_font(artist_label, font_small, 0);
    lv_obj_set_style_text_color(artist_label, lv_color_hex(0x504d6d), 0);
    lv_label_set_text(artist_label, app_music_get_artist(track_id));

    genre_label = lv_label_create(cont);
    lv_obj_set_style_text_font(genre_label, font_small, 0);
    lv_obj_set_style_text_color(genre_label, lv_color_hex(0x8a86b8), 0);
    lv_label_set_text(genre_label, app_music_get_genre(track_id));

    return cont;
}

static lv_obj_t *create_icon_box(lv_obj_t *parent)
{
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_height(cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *icon;
    icon = lv_image_create(cont);
    lv_image_set_src(icon, &app_music_icn_heart);
    icon = lv_image_create(cont);
    lv_image_set_src(icon, &app_music_icn_chart);
    icon = lv_image_create(cont);
    lv_image_set_src(icon, &app_music_icn_download);
    icon = lv_image_create(cont);
    lv_image_set_src(icon, &app_music_icn_chat);

    return cont;
}

static lv_obj_t *create_spectrum_obj(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
#if APP_MUSIC_LARGE
    lv_obj_set_height(obj, 500);
#else
    lv_obj_set_height(obj, 250);
#endif
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(obj, spectrum_draw_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_refresh_ext_draw_size(obj);
    album_image_obj = album_image_create(obj);
    return obj;
}

static lv_obj_t *create_ctrl_box(lv_obj_t *parent)
{
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_height(cont, LV_SIZE_CONTENT);
    lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
#if APP_MUSIC_LARGE
    lv_obj_set_style_pad_bottom(cont, 17, 0);
#else
    lv_obj_set_style_pad_bottom(cont, 8, 0);
#endif
    static const int32_t grid_col[] = {LV_GRID_FR(2), LV_GRID_FR(3), LV_GRID_FR(5), LV_GRID_FR(5), LV_GRID_FR(5), LV_GRID_FR(3), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    static const int32_t grid_row[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(cont, grid_col, grid_row);

    lv_obj_t *icon;
    icon = lv_image_create(cont);
    lv_image_set_src(icon, &app_music_btn_rnd);
    lv_obj_set_grid_cell(icon, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    icon = lv_image_create(cont);
    lv_image_set_src(icon, &app_music_btn_loop);
    lv_obj_set_grid_cell(icon, LV_GRID_ALIGN_END, 5, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    icon = lv_image_create(cont);
    lv_image_set_src(icon, &app_music_btn_prev);
    lv_obj_set_grid_cell(icon, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_add_event_cb(icon, prev_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_CLICKABLE);

    play_obj = lv_imagebutton_create(cont);
    lv_imagebutton_set_src(play_obj, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &app_music_btn_play, NULL);
    lv_imagebutton_set_src(play_obj, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, NULL, &app_music_btn_pause,
                           NULL);
    lv_obj_add_flag(play_obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_grid_cell(play_obj, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_add_event_cb(play_obj, play_event_click_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(play_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_width(play_obj, app_music_btn_play.header.w);

    icon = lv_image_create(cont);
    lv_image_set_src(icon, &app_music_btn_next);
    lv_obj_set_grid_cell(icon, LV_GRID_ALIGN_CENTER, 4, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_add_event_cb(icon, next_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_CLICKABLE);

    slider_obj = lv_slider_create(cont);
    lv_obj_set_style_anim_duration(slider_obj, 100, 0);
    lv_obj_add_flag(slider_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(slider_obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
#if APP_MUSIC_LARGE == 0
    lv_obj_set_height(slider_obj, 3);
#else
    lv_obj_set_height(slider_obj, 6);
#endif
    lv_obj_set_grid_cell(slider_obj, LV_GRID_ALIGN_STRETCH, 1, 4, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_style_bg_image_src(slider_obj, &app_music_icn_slider, LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider_obj, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_pad_all(slider_obj, 20, LV_PART_KNOB);
    lv_obj_set_style_bg_grad_dir(slider_obj, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_obj, lv_color_hex(0x569af8), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(slider_obj, lv_color_hex(0xa666f1), LV_PART_INDICATOR);
    lv_obj_set_style_outline_width(slider_obj, 0, 0);
    lv_obj_add_event_cb(slider_obj, del_counter_timer_cb, LV_EVENT_DELETE, NULL);

    time_obj = lv_label_create(cont);
    lv_obj_set_style_text_font(time_obj, font_small, 0);
    lv_obj_set_style_text_color(time_obj, lv_color_hex(0x8a86b8), 0);
    lv_label_set_text(time_obj, "0:00");
    lv_obj_set_grid_cell(time_obj, LV_GRID_ALIGN_END, 5, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_add_event_cb(time_obj, del_counter_timer_cb, LV_EVENT_DELETE, NULL);

    return cont;
}

static lv_obj_t *create_handle(lv_obj_t *parent)
{
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 8, 0);

    lv_obj_t *handle_label = lv_label_create(cont);
    lv_label_set_text(handle_label, "ALL TRACKS");
    lv_obj_set_style_text_font(handle_label, font_small, 0);
    lv_obj_set_style_text_color(handle_label, lv_color_hex(0x8a86b8), 0);

    lv_obj_t *handle_rect = lv_obj_create(cont);
#if APP_MUSIC_LARGE
    lv_obj_set_size(handle_rect, 40, 3);
#else
    lv_obj_set_size(handle_rect, 20, 2);
#endif
    lv_obj_set_style_bg_color(handle_rect, lv_color_hex(0x8a86b8), 0);
    lv_obj_set_style_border_width(handle_rect, 0, 0);

    return cont;
}

static void track_load(uint32_t id)
{
    time_act = 0;
    lv_slider_set_value(slider_obj, 0, LV_ANIM_OFF);
    lv_label_set_text(time_obj, "0:00");

    if (id == track_id) { return; }
    bool next = false;
    if ((track_id + 1) % ACTIVE_TRACK_CNT == id) { next = true; }

    app_music_list_button_check(track_id, false);
    track_id = id;
    app_music_list_button_check(id, true);

    lv_label_set_text(title_label, app_music_get_title(track_id));
    lv_label_set_text(artist_label, app_music_get_artist(track_id));
    lv_label_set_text(genre_label, app_music_get_genre(track_id));

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, album_image_obj);
    lv_anim_set_values(&a, lv_obj_get_style_image_opa(album_image_obj, 0), LV_OPA_TRANSP);
    lv_anim_set_exec_cb(&a, album_fade_anim_cb);
    lv_anim_set_duration(&a, 500);
    lv_anim_start(&a);

    lv_anim_init(&a);
    lv_anim_set_var(&a, album_image_obj);
    lv_anim_set_duration(&a, 500);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
#if APP_MUSIC_LANDSCAPE
    if (next)
    {
        lv_anim_set_values(&a, 0, -LV_HOR_RES / 7);
    }
    else
    {
        lv_anim_set_values(&a, 0, LV_HOR_RES / 7);
    }
#else
    if (next)
    {
        lv_anim_set_values(&a, 0, -LV_HOR_RES / 2);
    }
    else
    {
        lv_anim_set_values(&a, 0, LV_HOR_RES / 2);
    }
#endif
    lv_anim_set_exec_cb(&a, _obj_set_x_anim_cb);
    lv_anim_set_completed_cb(&a, lv_obj_delete_anim_completed_cb);
    lv_anim_start(&a);

    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_var(&a, album_image_obj);
    lv_anim_set_duration(&a, 500);
    lv_anim_set_values(&a, LV_SCALE_NONE, LV_SCALE_NONE / 2);
    lv_anim_set_exec_cb(&a, _image_set_scale_anim_cb);
    lv_anim_set_completed_cb(&a, NULL);
    lv_anim_start(&a);

    album_image_obj = album_image_create(spectrum_obj);

    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_var(&a, album_image_obj);
    lv_anim_set_duration(&a, 500);
    lv_anim_set_delay(&a, 100);
    lv_anim_set_values(&a, LV_SCALE_NONE / 4, LV_SCALE_NONE);
    lv_anim_set_exec_cb(&a, _image_set_scale_anim_cb);
    lv_anim_set_completed_cb(&a, NULL);
    lv_anim_start(&a);

    lv_anim_init(&a);
    lv_anim_set_var(&a, album_image_obj);
    lv_anim_set_values(&a, 0, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a, album_fade_anim_cb);
    lv_anim_set_duration(&a, 500);
    lv_anim_set_delay(&a, 100);
    lv_anim_start(&a);
}

static void del_counter_timer_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_DELETE && sec_counter_timer)
    {
        lv_timer_delete(sec_counter_timer);
        sec_counter_timer = NULL;
    }
}

static void spectrum_draw_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_REFR_EXT_DRAW_SIZE)
    {
#if APP_MUSIC_LANDSCAPE
        lv_event_set_ext_draw_size(e, LV_HOR_RES);
#else
        lv_event_set_ext_draw_size(e, LV_VER_RES);
#endif
    }
    else if (code == LV_EVENT_COVER_CHECK)
    {
        lv_event_set_cover_res(e, LV_COVER_RES_NOT_COVER);
    }
}

static void scale_anim_cb(void *a, int32_t v)
{
    lv_obj_t *obj = a;
    lv_image_set_scale(obj, scale_values[v % scale_values_len]);
}

static lv_obj_t *album_image_create(lv_obj_t *parent)
{
    lv_obj_t *img = lv_image_create(parent);

    switch (track_id)
    {
    case 2:
        lv_image_set_src(img, &app_music_cover_3);
        break;
    case 1:
        lv_image_set_src(img, &app_music_cover_2);
        break;
    case 0:
        lv_image_set_src(img, &app_music_cover_1);
        break;
    }
    lv_image_set_antialias(img, false);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(img, album_gesture_event_cb, LV_EVENT_GESTURE, NULL);
    lv_obj_remove_flag(img, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);

    return img;
}

static void album_gesture_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
    if (dir == LV_DIR_LEFT) { app_music_album_next(true); }
    if (dir == LV_DIR_RIGHT) { app_music_album_next(false); }
}

static void play_event_click_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    if (lv_obj_has_state(obj, LV_STATE_CHECKED))
    {
        app_music_resume();
    }
    else
    {
        app_music_pause();
    }
}

static void prev_click_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    app_music_album_next(false);
}

static void next_click_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        app_music_album_next(true);
    }
}

static void timer_cb(lv_timer_t *t)
{
    LV_UNUSED(t);
    time_act++;
    lv_label_set_text_fmt(time_obj, "%"LV_PRIu32":%02"LV_PRIu32, time_act / 60, time_act % 60);
    lv_slider_set_value(slider_obj, time_act, LV_ANIM_ON);
    if (time_act >= app_music_get_track_length(track_id))
    {
        app_music_album_next(true);
    }
}

static void album_fade_anim_cb(void *var, int32_t v)
{
    lv_obj_set_style_image_opa(var, v, 0);
}
