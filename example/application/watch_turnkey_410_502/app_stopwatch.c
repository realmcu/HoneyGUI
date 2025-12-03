/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "gui_list.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_canvas.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
#include "gui_message.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef enum
{
    DEFAULT = 0,
    START,
    STOP,
} STOPWATCH_STATUS;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "stopwatch_view"

#define SCREEN_WIDTH  (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()

#define ICON_START_X    25
#define ICON_START_Y    21
#define ICON_STOP_X     23
#define ICON_STOP_Y     23
#define ICON_MARK_X     21
#define ICON_MARK_Y     21
#define ICON_RESET_X    14
#define ICON_RESET_Y    14

#define COLOR_START  GUI_COLOR_ARGB8888(0xFF, 0x65, 0xDC, 0x7B) //65DC7B
#define COLOR_STOP   GUI_COLOR_ARGB8888(0xFF, 0xFE, 0x37, 0x2C) //FE372C
#define COLOR_RESET  GUI_COLOR_ARGB8888(0xFF, 0xB7, 0xB7, 0xB7) //B7B7B7
#define COLOR_MARK   GUI_COLOR_ARGB8888(0xFF, 0xFF, 0xFF, 0xFF) //FFFFFF
#define COLOR_HAND   GUI_COLOR_ARGB8888(0xFF, 0xEC, 0x60, 0x2A) //EC602A

#define TIME_CNT_NUM 20
#define DRAW_DOT_NUM 6

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void stopwatch_design(gui_view_t *view);
static void clear_mem(gui_view_t *view);

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, stopwatch_design, clear_mem);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static STOPWATCH_STATUS status = DEFAULT;

static uint32_t time_count = 0; //milsec
static uint32_t time_count_array[TIME_CNT_NUM] = {0}; //milsec
static uint8_t time_count_index = 0;
static char count_str[] = "00:00.00";
static char count_str_array[TIME_CNT_NUM][9] = {0};
static const char *lap_str_array[TIME_CNT_NUM] =
{
    "Lap_1",
    "Lap_2",
    "Lap_3",
    "Lap_4",
    "Lap_5",
    "Lap_6",
    "Lap_7",
    "Lap_8",
    "Lap_9",
    "Lap_10",
    "Lap_11",
    "Lap_12",
    "Lap_13",
    "Lap_14",
    "Lap_15",
    "Lap_16",
    "Lap_17",
    "Lap_18",
    "Lap_19",
    "Lap_20",
};
static char base_str[4] = {0};
static gui_text_t *t_lap_array[DRAW_DOT_NUM] = {0};
static uint32_t count_base = 0;

static void *img_data_rect = NULL;
static void *img_data_line = NULL;

static int8_t page_index = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void clear_mem(gui_view_t *view)
{
    GUI_UNUSED(view);
    if (img_data_rect)
    {
        gui_free(img_data_rect);
        img_data_rect = NULL;
    }
    if (img_data_line)
    {
        gui_free(img_data_line);
        img_data_line = NULL;
    }
}

static void update_page_1_lap(void)
{
    int8_t index = time_count_index + 1;
    int8_t start_index = (index > DRAW_DOT_NUM - 1) ? (index - (DRAW_DOT_NUM - 1)) : 0;
    for (int8_t i = start_index; i <= index && index != TIME_CNT_NUM; i++)
    {
        gui_color_t font_color = gui_rgb(0xB7, 0xB7, 0xB7);
        if (i == index)
        {
            font_color = gui_rgb(0xFF, 0xFF, 0xFF);
        }
        gui_text_t *t_lap = t_lap_array[i - start_index];
        gui_text_content_set(t_lap, (char *)lap_str_array[i] + 4, strlen(lap_str_array[i] + 4));
        gui_text_color_set(t_lap, font_color);
        gui_obj_hidden((void *)t_lap, false);
    }

    gui_obj_t *t_base = gui_list_entry(t_lap_array[0]->base.brother_list.prev, gui_obj_t, brother_list);
    gui_obj_hidden(t_base, false);
    if (time_count_index == 0)
    {
        uint32_t cnt = time_count_array[0];
        if (cnt >= 30000)
        {
            uint8_t min = cnt / 60000 + (cnt % 60000) / 30000;
            sprintf(base_str, "%dm", min);
            count_base = min * 60000;
        }
        else
        {
            uint8_t sec = cnt / 1000 + (cnt % 1000) / 500;
            sprintf(base_str, "%ds", sec);
            count_base = sec * 1000;
        }
        gui_text_content_set((void *)t_base, base_str, strlen(base_str));
    }
}

static void update_page_2_lap(gui_obj_t *parent)
{
    int8_t index = time_count_index;
    gui_obj_t *win = gui_list_entry(parent->child_list.prev, gui_obj_t, brother_list);
    while (index)
    {
        gui_obj_t *circle = gui_list_entry(win->child_list.next, gui_obj_t, brother_list);
        gui_img_a8_recolor((void *)circle, COLOR_START);
        gui_color_t font_color = gui_rgb(0xB7, 0xB7, 0xB7);
        gui_text_t *t_lap_count = (void *)gui_list_entry(win->child_list.prev, gui_obj_t, brother_list);
        gui_text_t *t_lap = (void *)gui_list_entry(t_lap_count->base.brother_list.prev, gui_obj_t,
                                                   brother_list);
        gui_text_color_set(t_lap_count, font_color);
        gui_text_color_set(t_lap, font_color);

        win->y += 50;
        win = gui_list_entry(win->brother_list.prev, gui_obj_t, brother_list);
        index--;
    }
    index = time_count_index;

    win = (void *)gui_win_create(parent, 0, 30, 165, 50, 350);
    gui_img_t *circle = gui_img_create_from_mem(win, 0, PAGE_INDEX_ICON_BIN, 247, 20, 0, 0);
    gui_img_a8_recolor((void *)circle, COLOR_STOP);
    gui_img_t *separator = gui_img_create_from_mem(win, 0, img_data_rect, 0, 0, 0, 0);
    gui_img_set_mode(separator, IMG_COVER_MODE);
    gui_text_t *t_lap = gui_text_create(win, 0, 0, 0, 100, 50);
    gui_text_set(t_lap, (void *)lap_str_array[index], GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen((void *)lap_str_array[index]), 22);
    gui_text_type_set(t_lap, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_lap, MID_LEFT);
    gui_text_rendermode_set(t_lap, 2);

    sprintf(count_str_array[index], "%02u:%02u:%02u",
            (time_count_array[index] / 60000),
            (time_count_array[index] % 60000) / 1000,
            (time_count_array[index] % 1000) / 10);
    gui_text_t *t_lap_count = gui_text_create(win, 0, 275, 0, 350, 50);
    gui_text_set(t_lap_count, count_str_array[index], GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(count_str_array[index]), 22);
    gui_text_type_set(t_lap_count, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_lap_count, MID_LEFT);
    gui_text_rendermode_set(t_lap_count, 2);
}

static void click_button_l(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_obj_t *bg_l = obj;
    gui_obj_t *list = gui_list_entry(bg_l->parent->child_list.next, gui_obj_t, brother_list);
    if (list->child_list.next != list->child_list.prev) { return; }
    gui_obj_t *note = gui_list_entry(list->child_list.next, gui_obj_t, brother_list);
    switch (status)
    {
    case DEFAULT:
        return;
        break;
    case START:
        {
            if (time_count_index != TIME_CNT_NUM)
            {
                time_count_array[time_count_index] = time_count;
                switch (page_index)
                {
                case 0:
                    {
                        if (time_count_index == 0) //create new hand
                        {
                            gui_obj_t *sec_container_b = gui_list_entry(note->child_list.next, gui_obj_t, brother_list);
                            int16_t focus_x = 7;
                            int16_t focus_y = 170;
                            gui_img_t *sec_hand = gui_img_create_from_mem(sec_container_b, 0, STOPWATCH_HAND_BIG_BIN,
                                                                          166  + focus_x, 3 + focus_y, 0, 0);
                            gui_img_set_focus(sec_hand, focus_x, focus_y);
                            gui_img_set_quality(sec_hand, true);
                            gui_img_a8_recolor((void *)sec_hand, 0xFF7B81DD);
                        }
                    }
                    break;
                case 1:
                    {
                        update_page_1_lap();
                    }
                    break;
                case 2:
                    {

                        update_page_2_lap(note);
                    }
                    break;
                default:
                    break;
                }
                time_count_index++;
            }
            // gui_log("time_count_array[%d] = %d\n", time_count_index - 1, time_count_array[time_count_index - 1]);
        }
        break;
    case STOP:
        msg_2_regenerate_current_view();
        break;

    default:
        break;
    }
}

static void click_button_r(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *bg_r = obj;
    gui_obj_t *list = gui_list_entry(bg_r->parent->child_list.next, gui_obj_t, brother_list);
    if (list->child_list.next != list->child_list.prev) { return; }

    gui_obj_t *bg_l = gui_list_entry(bg_r->brother_list.prev, gui_obj_t, brother_list);
    gui_obj_t *icon_l = gui_list_entry(bg_l->child_list.next, gui_obj_t, brother_list);
    gui_obj_t *icon_r = gui_list_entry(bg_r->child_list.next, gui_obj_t, brother_list);
    gui_obj_t *note = gui_list_entry(list->child_list.next, gui_obj_t, brother_list);

    switch (status)
    {
    case DEFAULT:
        {
            status = START;
            gui_img_a8_recolor((void *)bg_r, COLOR_STOP);
            gui_img_set_image_data((void *)icon_r, STOPWATCH_BUTTON_STOP_BIN);
            gui_img_refresh_size((void *)icon_r);
            gui_obj_move(icon_r, ICON_STOP_X, ICON_STOP_Y);
            gui_img_a8_recolor((void *)bg_l, COLOR_MARK);
            gui_obj_hidden((void *)icon_l, false);
            gui_obj_start_timer((void *)note);
        }
        break;
    case START:
        {
            status = STOP;
            gui_img_a8_recolor((void *)bg_r, COLOR_START);
            gui_img_set_image_data((void *)icon_r, STOPWATCH_BUTTON_START_BIN);
            gui_img_refresh_size((void *)icon_r);
            gui_obj_move(icon_r, ICON_START_X, ICON_START_Y);
            gui_img_a8_recolor((void *)bg_l, COLOR_RESET);
            gui_img_set_image_data((void *)icon_l, STOPWATCH_BUTTON_RESET_BIN);
            gui_img_refresh_size((void *)icon_l);
            gui_obj_move(icon_l, ICON_RESET_X, ICON_RESET_Y);

            gui_obj_stop_timer((void *)note);
        }
        break;
    case STOP:
        {
            status = START;
            gui_img_a8_recolor((void *)bg_r, COLOR_STOP);
            gui_img_set_image_data((void *)icon_r, STOPWATCH_BUTTON_STOP_BIN);
            gui_img_refresh_size((void *)icon_r);
            gui_obj_move(icon_r, ICON_STOP_X, ICON_STOP_Y);
            gui_img_a8_recolor((void *)bg_l, COLOR_MARK);
            gui_img_set_image_data((void *)icon_l, STOPWATCH_BUTTON_MARK_BIN);
            gui_img_refresh_size((void *)icon_l);
            gui_obj_move(icon_l, ICON_MARK_X, ICON_MARK_Y);

            gui_obj_start_timer((void *)note);
        }
        break;
    default:
        break;
    }
}

static void draw_separator(NVGcontext *vg)
{
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, 350, 3);
    nvgFillColor(vg, nvgRGBA(0xB7, 0xB7, 0xB7, 0xFF));
    nvgFill(vg);
}

static void draw_line(NVGcontext *vg)
{
    if (time_count_index == 0) { return; }

    float interval = 48.f;
    float line_width = 2.f;
    float area_half_w = 24.f;

    nvgBeginPath(vg);
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGBA(0xEC, 0x60, 0x2A, 0xFF));
    nvgLineCap(vg, NVG_BUTT);
    float dotted_line_w = 2.f + (time_count_index >= 5 ? interval * 5 : interval * (time_count_index));
    float dash_len = 2.0f;
    float gap_len = 2.0f;
    float current_x = 0.0f;

    while (current_x < dotted_line_w)
    {
        float end_x = current_x + dash_len;
        if (end_x > dotted_line_w)
        {
            end_x = dotted_line_w;
        }
        nvgMoveTo(vg, current_x, area_half_w);
        nvgLineTo(vg, end_x, area_half_w);
        current_x += dash_len + gap_len;
    }
    nvgStroke(vg);

    nvgBeginPath(vg);
    int8_t start_index = (time_count_index > DRAW_DOT_NUM - 1) ? (time_count_index -
                                                                  (DRAW_DOT_NUM - 1)) : 0;
    for (int8_t i = start_index; i <= time_count_index; i++)
    {
        int32_t cnt;
        if (i == time_count_index)
        {
            cnt = time_count - time_count_array[i - 1];
        }
        else if (i == 0)
        {
            cnt = time_count_array[i];
        }
        else
        {
            cnt = time_count_array[i] - time_count_array[i - 1];
        }
        float px = (i - start_index) * interval;
        float py = area_half_w - (cnt - (int32_t)count_base) * area_half_w / (int32_t)count_base;
        if (py < 1)
        {
            py = 1;
        }
        else if (py > area_half_w * 2)
        {
            py = area_half_w * 2;
        }

        if (i == start_index)
        {
            nvgMoveTo(vg, px, py);
        }
        else
        {
            nvgLineTo(vg, px, py);
        }
    }
    nvgStrokeColor(vg, nvgRGBA(0xFF, 0xFF, 0xFF, 255));
    nvgStrokeWidth(vg, line_width);
    nvgLineCap(vg, NVG_SQUARE);
    nvgLineJoin(vg, NVG_SQUARE);
    nvgStroke(vg);
}

static void update_line_cb(void *obj)
{
    GUI_UNUSED(obj);
    if (status == START && time_count_index != 0 && time_count_index != TIME_CNT_NUM)
    {
        int image_h = 48;
        int image_w = 240;
        int pixel_bytes = 2;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        memset(img_data_line, 0, buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, image_w, image_h, draw_line,
                                          img_data_line);
    }
}

static void page_0_timer_cb(void *obj)
{
    gui_obj_t *sec_container_b = gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                brother_list);
    gui_obj_t *min_container = gui_list_entry(sec_container_b->child_list.next, gui_obj_t,
                                              brother_list);
    gui_obj_t *t_count = gui_list_entry(sec_container_b->brother_list.next, gui_obj_t, brother_list);
    gui_obj_t *sec_hand = gui_list_entry(min_container->brother_list.next, gui_obj_t, brother_list);
    gui_obj_t *min_hand = gui_list_entry(min_container->child_list.next, gui_obj_t, brother_list);
    float degree_sec = ((time_count % 60000) / 1000.f) * 6;
    float degree_min = ((time_count % 1800000) / 60000.f) * 12;
    gui_img_rotation((void *)sec_hand, degree_sec);
    gui_img_rotation((void *)min_hand, degree_min);
    sprintf(count_str, "%02u:%02u.%02u", (time_count / 60000), (time_count % 60000) / 1000,
            (time_count % 1000) / 10);
    gui_text_content_set((void *)t_count, count_str, strlen(count_str));

    if (time_count_index)
    {
        gui_obj_t *sec_hand_new = gui_list_entry(sec_hand->brother_list.next, gui_obj_t, brother_list);
        uint32_t milsec = time_count - time_count_array[time_count_index - 1];
        float degree_sec_new = ((milsec % 60000) / 1000.f) * 6;
        gui_img_rotation((void *)sec_hand_new, degree_sec_new);
    }
}

static void page_1_timer_cb(void *obj)
{
    gui_obj_t *min_container = gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t, brother_list);
    gui_obj_t *sec_container = gui_list_entry(min_container->brother_list.next, gui_obj_t,
                                              brother_list);
    gui_obj_t *milsec_container = gui_list_entry(sec_container->brother_list.next, gui_obj_t,
                                                 brother_list);
    gui_obj_t *t_count = gui_list_entry(milsec_container->brother_list.next, gui_obj_t, brother_list);
    gui_obj_t *min_hand = gui_list_entry(min_container->child_list.next, gui_obj_t, brother_list);
    gui_obj_t *sec_hand = gui_list_entry(sec_container->child_list.next, gui_obj_t, brother_list);
    gui_obj_t *milsec_hand = gui_list_entry(milsec_container->child_list.next, gui_obj_t, brother_list);
    float degree_sec = ((time_count % 60000) / 1000.f) * 6;
    float degree_min = ((time_count % 1800000) / 60000.f) * 12;
    float degree_milsec = ((time_count % 1000) / 100.f) * 36;
    gui_img_rotation((void *)sec_hand, degree_sec);
    gui_img_rotation((void *)min_hand, degree_min);
    gui_img_rotation((void *)milsec_hand, degree_milsec);
    sprintf(count_str, "%02u:%02u.%02u", (time_count / 60000), (time_count % 60000) / 1000,
            (time_count % 1000) / 10);
    gui_text_content_set((void *)t_count, count_str, strlen(count_str));
}

static void page_2_timer_cb(void *obj)
{
    gui_obj_t *t_count = gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t, brother_list);
    sprintf(count_str, "%02u:%02u.%02u", (time_count / 60000), (time_count % 60000) / 1000,
            (time_count % 1000) / 10);
    gui_text_content_set((void *)t_count, count_str, strlen(count_str));
}

static void page_0_design(gui_obj_t *parent)
{
    gui_img_t *sec_container_b = gui_img_create_from_mem(parent, 0, STOPWATCH_SEC_CONTAINER_BIG_BIN, 30,
                                                         76, 0, 0);
    gui_img_set_mode(sec_container_b, IMG_SRC_OVER_MODE);
    gui_img_t *min_container = gui_img_create_from_mem(sec_container_b, 0, STOPWATCH_MIN_CONTAINER_BIN,
                                                       124, 52, 0, 0);
    gui_img_set_mode(min_container, IMG_SRC_OVER_MODE);

    int16_t focus_x = 7;
    int16_t focus_y = 170;
    gui_img_t *sec_hand = gui_img_create_from_mem(sec_container_b, 0, STOPWATCH_HAND_BIG_BIN,
                                                  166  + focus_x, 3 + focus_y, 0, 0);
    gui_img_set_focus(sec_hand, focus_x, focus_y);
    gui_img_set_quality(sec_hand, true);
    gui_img_a8_recolor((void *)sec_hand, COLOR_HAND);
    focus_x = 3;
    focus_y = 46;
    gui_img_t *min_hand = gui_img_create_from_mem(min_container, 0, STOPWATCH_HAND_S_BIN, 46 + focus_x,
                                                  4 + focus_y, 0, 0);
    gui_img_set_focus(min_hand, focus_x, focus_y);
    gui_img_set_quality(min_hand, true);
    gui_img_a8_recolor((void *)min_hand, COLOR_HAND);

    sprintf(count_str, "00:00.00");
    gui_text_t *t_count = gui_text_create(parent, 0, 125, 282, SCREEN_WIDTH, 48);
    gui_text_set(t_count, count_str, GUI_FONT_SRC_TTF, gui_rgb(0, 0, 0),
                 strlen(count_str), 48);
    gui_text_type_set(t_count, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_count, MID_LEFT);
    gui_text_rendermode_set(t_count, 2);

    if (time_count_index != 0) //create new hand
    {
        int16_t focus_x = 7;
        int16_t focus_y = 170;
        gui_img_t *sec_hand = gui_img_create_from_mem(sec_container_b, 0, STOPWATCH_HAND_BIG_BIN,
                                                      166  + focus_x, 3 + focus_y, 0, 0);
        gui_img_set_focus(sec_hand, focus_x, focus_y);
        gui_img_set_quality(sec_hand, true);
        gui_img_a8_recolor((void *)sec_hand, 0xFF7B81DD);
    }
}

static void page_1_design(gui_obj_t *parent)
{
    gui_img_t *min_container = gui_img_create_from_mem(parent, 0, STOPWATCH_MIN_CONTAINER_BIN, 24, 100,
                                                       0, 0);
    gui_img_set_mode(min_container, IMG_SRC_OVER_MODE);
    gui_img_t *sec_container = gui_img_create_from_mem(parent, 0, STOPWATCH_SEC_CONTAINER_BIN, 155, 100,
                                                       0, 0);
    gui_img_set_mode(sec_container, IMG_SRC_OVER_MODE);
    gui_img_t *milsec_container = gui_img_create_from_mem(parent, 0, STOPWATCH_MILSEC_CONTAINER_BIN,
                                                          286, 100, 0, 0);
    gui_img_set_mode(milsec_container, IMG_SRC_OVER_MODE);

    int16_t focus_x = 3;
    int16_t focus_y = 46;
    gui_img_t *min_hand = gui_img_create_from_mem(min_container, 0, STOPWATCH_HAND_S_BIN, 46 + focus_x,
                                                  4 + focus_y, 0, 0);
    gui_img_set_focus(min_hand, focus_x, focus_y);
    gui_img_set_quality(min_hand, true);
    gui_img_a8_recolor((void *)min_hand, COLOR_HAND);
    gui_img_t *sec_hand = gui_img_create_from_mem(sec_container, 0, STOPWATCH_HAND_S_BIN, 46 + focus_x,
                                                  4 + focus_y, 0, 0);
    gui_img_set_focus(sec_hand, focus_x, focus_y);
    gui_img_set_quality(sec_hand, true);
    gui_img_a8_recolor((void *)sec_hand, COLOR_HAND);
    gui_img_t *milsec_hand = gui_img_create_from_mem(milsec_container, 0, STOPWATCH_HAND_S_BIN,
                                                     46 + focus_x, 4 + focus_y, 0, 0);
    gui_img_set_focus(milsec_hand, focus_x, focus_y);
    gui_img_set_quality(milsec_hand, true);
    gui_img_a8_recolor((void *)milsec_hand, COLOR_HAND);

    gui_text_t *t_count = gui_text_create(parent, 0, 60, 230, SCREEN_WIDTH, 80);
    gui_text_set(t_count, count_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(count_str), 80);
    gui_text_type_set(t_count, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_count, MID_LEFT);
    gui_text_rendermode_set(t_count, 2);

    {
        int image_h = 48;
        int image_w = 240;
        int pixel_bytes = 2;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (!img_data_line)
        {
            img_data_line = gui_malloc(buffer_size);
        }
        memset(img_data_line, 0, buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, image_w, image_h, draw_line,
                                          img_data_line);
    }

    gui_img_t *img_line = gui_img_create_from_mem(parent, 0, img_data_line, 47, 309, 0, 0);
    gui_img_set_mode(img_line, IMG_COVER_MODE);
    gui_obj_create_timer((void *)img_line, 50, -1, update_line_cb);
    gui_obj_start_timer((void *)img_line);

    gui_text_t *t_base = gui_text_create(parent, 0, 336, 322, 50, 34);
    gui_text_set(t_base, base_str, GUI_FONT_SRC_TTF, gui_rgb(0xEC, 0x60, 0x2A),
                 strlen(base_str), 34);
    gui_text_type_set(t_base, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_base, MID_LEFT);
    gui_text_rendermode_set(t_base, 2);
    int16_t x = 37;
    for (int8_t i = 0; i < DRAW_DOT_NUM; i++)
    {
        t_lap_array[i] = gui_text_create(parent, 0, x, 370, 24, 24);
        gui_text_type_set(t_lap_array[i], SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t_lap_array[i], MID_CENTER);
        gui_text_rendermode_set(t_lap_array[i], 2);
        gui_text_set(t_lap_array[i], (char *)lap_str_array[0] + 4, GUI_FONT_SRC_TTF, gui_rgb(0xB7, 0xB7,
                     0xB7),
                     strlen(lap_str_array[0] + 4), 24);
        gui_obj_hidden((void *)t_lap_array[i], true);
        x += 48;
    }

    int8_t start_index = (time_count_index > DRAW_DOT_NUM - 1) ? (time_count_index -
                                                                  (DRAW_DOT_NUM - 1)) : 0;
    for (int8_t i = start_index; i <= time_count_index && time_count_index != TIME_CNT_NUM; i++)
    {
        gui_color_t font_color = gui_rgb(0xB7, 0xB7, 0xB7);
        if (i == time_count_index)
        {
            font_color = gui_rgb(0xFF, 0xFF, 0xFF);
        }
        gui_text_t *t_lap = t_lap_array[i - start_index];
        gui_text_content_set(t_lap, (char *)lap_str_array[i] + 4, strlen(lap_str_array[i] + 4));
        gui_text_color_set(t_lap, font_color);

        gui_obj_hidden((void *)t_lap, false);
    }

    if (time_count_index != 0)
    {
        uint32_t cnt = time_count_array[0];
        if (cnt >= 30000)
        {
            uint8_t min = cnt / 60000 + (cnt % 60000) / 30000;
            sprintf(base_str, "%dm", min);
            count_base = min * 60000;
        }
        else
        {
            uint8_t sec = cnt / 1000 + (cnt % 1000) / 500;
            sprintf(base_str, "%ds", sec);
            count_base = sec * 1000;
        }
        gui_text_content_set((void *)t_base, base_str, strlen(base_str));
    }
    else
    {
        gui_obj_hidden((void *)t_base, true);
    }
}

static void page_2_design(gui_obj_t *parent)
{
    gui_text_t *t_count = gui_text_create(parent, 0, 60, 89, SCREEN_WIDTH, 80);
    gui_text_set(t_count, count_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(count_str), 80);
    gui_text_type_set(t_count, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_count, MID_LEFT);
    gui_text_rendermode_set(t_count, 2);

    {
        int image_h = 3;
        int image_w = 350;
        int pixel_bytes = 2;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (!img_data_rect)
        {
            img_data_rect = gui_malloc(buffer_size);
        }
        memset(img_data_rect, 0, buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, image_w, image_h, draw_separator,
                                          img_data_rect);
    }

    if (time_count_index != 0)
    {
        int16_t y = 165;
        for (int8_t i = time_count_index - 1; i >= 0; i--)
        {
            gui_win_t *win = gui_win_create(parent, 0, 30, y, 50, 350);
            y += 50;
            gui_color_t font_color = gui_rgb(0xB7, 0xB7, 0xB7);
            gui_img_t *circle = gui_img_create_from_mem(win, 0, PAGE_INDEX_ICON_BIN, 247, 20, 0, 0);
            if (i == time_count_index - 1)
            {
                gui_img_a8_recolor((void *)circle, COLOR_STOP);
                font_color = gui_rgb(0xFF, 0xFF, 0xFF);
            }
            else
            {
                gui_img_a8_recolor((void *)circle, COLOR_START);
            }
            gui_img_t *separator = gui_img_create_from_mem(win, 0, img_data_rect, 0, 0, 0, 0);
            gui_img_set_mode(separator, IMG_COVER_MODE);

            gui_text_t *t_lap = gui_text_create(win, 0, 0, 0, 100, 50);
            gui_text_set(t_lap, (void *)lap_str_array[i], GUI_FONT_SRC_TTF, font_color,
                         strlen((void *)lap_str_array[i]), 22);
            gui_text_type_set(t_lap, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(t_lap, MID_LEFT);
            gui_text_rendermode_set(t_lap, 2);

            sprintf(count_str_array[i], "%02u:%02u:%02u",
                    (time_count_array[i] / 60000),
                    (time_count_array[i] % 60000) / 1000,
                    (time_count_array[i] % 1000) / 10);
            gui_text_t *t_lap_count = gui_text_create(win, 0, 275, 0, 350, 50);
            gui_text_set(t_lap_count, count_str_array[i], GUI_FONT_SRC_TTF, font_color,
                         strlen(count_str_array[i]), 22);
            gui_text_type_set(t_lap_count, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(t_lap_count, MID_LEFT);
            gui_text_rendermode_set(t_lap_count, 2);
        }
    }
}

static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    int16_t index = ((gui_list_note_t *)obj)->index;

    switch (index)
    {
    case 0:
        {
            page_0_design(obj);
            gui_obj_create_timer(obj, 10, -1, page_0_timer_cb);
            page_0_timer_cb(obj);
        }
        break;
    case 1:
        {
            page_1_design(obj);
            gui_obj_create_timer(obj, 10, -1, page_1_timer_cb);
            page_1_timer_cb(obj);
        }
        break;
    case 2:
        {
            page_2_design(obj);
            gui_obj_create_timer(obj, 10, -1, page_2_timer_cb);
            page_2_timer_cb(obj);
        }
        break;

    default:
        break;
    }
    if (status == START)
    {
        gui_obj_start_timer(obj);
    }
    else
    {
        gui_obj_stop_timer(obj);
    }
}

static void list_timer_cb(void *obj)
{
    gui_list_t *list = obj;
    int16_t offset = list->offset;

    gui_obj_t *page_indicator_0 = gui_list_entry(list->base.brother_list.next, gui_obj_t, brother_list);
    gui_obj_t *page_indicator_1 = gui_list_entry(page_indicator_0->brother_list.next, gui_obj_t,
                                                 brother_list);
    gui_obj_t *page_indicator_2 = gui_list_entry(page_indicator_1->brother_list.next, gui_obj_t,
                                                 brother_list);
    gui_img_a8_recolor((void *)page_indicator_0, 0xFF808080);
    gui_img_a8_recolor((void *)page_indicator_1, 0xFF808080);
    gui_img_a8_recolor((void *)page_indicator_2, 0xFF808080);
    if (offset >= -(SCREEN_HEIGHT / 2))
    {
        page_index = 0;
        gui_img_a8_recolor((void *)page_indicator_0, 0xFFFFFFFF);
    }
    else if (offset >= -(SCREEN_HEIGHT / 2) * 3)
    {
        page_index = 1;
        gui_img_a8_recolor((void *)page_indicator_1, 0xFFFFFFFF);
    }
    else
    {
        page_index = 2;
        gui_img_a8_recolor((void *)page_indicator_2, 0xFFFFFFFF);
    }

    if (status == START) { time_count += 20; }
}

static void time_update_cb(void *obj)
{
    gui_text_content_set(obj, time_str, strlen(time_str));
}

static void stopwatch_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, "menu_view",
                             SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    status = DEFAULT;
    time_count = 0;
    time_count_index = 0;
    memset(time_count_array, 0, sizeof(time_count_array));

    gui_list_t *list = gui_list_create(view, 0, 0, 0, 0, 0, SCREEN_HEIGHT, 0,
                                       VERTICAL, note_design, NULL, 0);
    gui_list_set_style(list, LIST_FADE);
    gui_list_set_note_num(list, 3);
    gui_list_set_inertia(list, false);
    gui_list_set_auto_align(list, true);
    gui_list_set_factor(list, 0.1f);
    gui_obj_create_timer((void *)list, 10, -1, list_timer_cb);
    if (!gui_view_get_current())
    {
        gui_list_set_offset(list, -(page_index * SCREEN_HEIGHT));
    }

    gui_img_t *page_indicator_0 = gui_img_create_from_mem(view, 0, PAGE_INDEX_ICON_BIN, 394, 56, 0, 0);
    gui_img_t *page_indicator_1 = gui_img_create_from_mem(view, 0, PAGE_INDEX_ICON_BIN, 394, 72, 0, 0);
    gui_img_t *page_indicator_2 = gui_img_create_from_mem(view, 0, PAGE_INDEX_ICON_BIN, 394, 88, 0, 0);
    gui_img_a8_recolor(page_indicator_0, 0xFFFFFFFF);
    gui_img_a8_recolor(page_indicator_1, 0xFF808080);
    gui_img_a8_recolor(page_indicator_2, 0xFF808080);

    gui_text_t *t_time = gui_text_create(view, 0, 0, 0, 390, 42);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(time_str), 34);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_RIGHT);
    gui_text_rendermode_set(t_time, 2);
    gui_obj_create_timer((void *)t_time, 30000, -1, time_update_cb);

    gui_img_t *bg_l = gui_img_create_from_mem(view, 0, STOPWATCH_BUTTON_BG_BIN, 18, 414, 0, 0);
    gui_img_t *bg_r = gui_img_create_from_mem(view, 0, STOPWATCH_BUTTON_BG_BIN, 320, 414, 0, 0);
    gui_img_t *icon_l = gui_img_create_from_mem(bg_l, 0, STOPWATCH_BUTTON_MARK_BIN, ICON_MARK_X,
                                                ICON_MARK_Y, 0, 0);
    gui_img_t *icon_r = gui_img_create_from_mem(bg_r, 0, STOPWATCH_BUTTON_START_BIN, ICON_START_X,
                                                ICON_START_Y, 0, 0);
    gui_img_a8_recolor(bg_l, COLOR_RESET);
    gui_img_a8_recolor(bg_r, COLOR_START);
    gui_img_a8_recolor(icon_l, GUI_COLOR_ARGB8888(0xFF, 0x00, 0x00, 0x00));
    gui_img_a8_recolor(icon_r, GUI_COLOR_ARGB8888(0xFF, 0x00, 0x00, 0x00));
    gui_obj_hidden((void *)icon_l, true);
    gui_obj_add_event_cb((void *)bg_l, click_button_l, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb((void *)bg_r, click_button_r, GUI_EVENT_TOUCH_CLICKED, NULL);
}
