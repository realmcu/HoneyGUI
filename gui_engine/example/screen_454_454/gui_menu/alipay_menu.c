#include "./root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_canvas_rect.h"
#include "gui_qbcode.h"
#include "gui_multi_level.h"
#include "gui_text.h"
#include "gui_obj.h"
#include "tp_algo.h"
#include "gui_app.h"


#define ROW_COUNT 10
#define TICKET_COUNT 24
#define ROW_GAP 50
#define UPDATE_TEXT_NAME "update text"
#define CLICK_FUNCTION_ARRAY func_array
#define TICKET_NAME_ARRAY string_array
#define BACKGROUND_IMAGE ICON_PROCESS_BAR_BIN
#define FONT ARIALBD_SIZE16_BITS4_FONT_BIN
#define FONT_SIZE 16

static const int gap = ROW_GAP;
static const int count = ROW_COUNT;
static const int font_size = FONT_SIZE;
static int time_array_size;
static const void *func_array[];
static const char *string_array[];
static gui_obj_t *widget_array[ROW_COUNT];
static void qrcode_ui(gui_obj_t *parent)
{
    gui_qbcode_t *qbcode = gui_qbcode_create(parent, "qbcode", 454 / 2 - 320 / 2, 454 / 2 - 320 / 2,
                                             320, 320, QRCODE_DISPLAY_SECTION, QRCODE_ENCODE_TEXT);
    char *str = "https://www.wechat.com/";
    gui_qbcode_config(qbcode, str, strlen(str), 2);
}

static void render(int index, gui_obj_t *obj)
{
    GUI_WIDGET_POINTER_BY_TYPE(t, TEXTBOX, obj)
    gui_text_content_set((void *)t, (void *)TICKET_NAME_ARRAY[index], strlen(TICKET_NAME_ARRAY[index]));
    //change obj callback
    obj->event_dsc->event_cb = CLICK_FUNCTION_ARRAY[index];
    obj->event_dsc->user_data = (void *)(size_t)index;

}
static void override(gui_win_t *timer1, gui_win_t *win)
{
    IMPORT_GUI_TOUCHPAD
    IMPORT_GUI_WHEEL

    static unsigned char render_mode, alien, render_flag, end_speed;

    static int touch_y, speed, text_image_map_length, col_offset, index_offset;
    static GUI_SPEED_RECODE_DEFINE
    static bool wheel_take_over;
    static char time_array_offset;
    static int history_y;
    static bool event5_flag;
    end_speed = 3;
    alien = 1;
    if (wheel->pressed)
    {
        wheel_take_over = 1;
        wheel->history_x = touch_y;
    }
    if (wheel->released)
    {
        wheel_take_over = 0;
    }

    if (!wheel_take_over)
    {
        int ax, ay;
        gui_obj_absolute_xy(GUI_BASE(win), &ax, &ay);
        if (touch->pressed && touch->x > ax && touch->x < ax + GUI_BASE(win)->w)
        {
            history_y = touch_y;
            speed = 0;
            memset(speed_recode, 0, sizeof(speed_recode));
            event5_flag = 0;
        }
        if (touch->released && time_array_offset < 0)
        {
            GUI_BASE(timer1)->y = 0;
            time_array_offset = 0;
            render_flag = 0;
            history_y = 0;
            touch_y = 0;
            GUI_WIDGET_POINTER_BY_NAME(text, UPDATE_TEXT_NAME)
            gui_obj_hidden(text, 1);
        }
        if (touch->pressing && touch->x > ax && touch->x < ax + GUI_BASE(win)->w)
        {
            touch_y = history_y + touch->deltaY;
            /**
             * Index = offset / gap % array length
               WidgetOffset = offset % gap
            */
            gui_update_speed(&speed, speed_recode);
            render_flag = 1;


        }
        else
        {
            gui_inertial(&speed, end_speed, &touch_y);
            if (touch_y > 0)
            {
                touch_y = 0;
            }
        }

        if (touch_y < -gap * (time_array_size) + (int)gui_get_screen_height())
        {
            touch_y = -gap * (time_array_size) + (int)gui_get_screen_height();
        }
        if (render_flag)
        {
            time_array_offset = -(touch_y / gap % time_array_size);
            int widget_offset = touch_y % gap;
            if (time_array_offset < 0)
            {
                widget_offset = touch_y;
                time_array_offset = -1;

                if (!event5_flag)
                {
                    win->event5_flag = 1;
                }
                else
                {
                    win->event5_flag = 0;
                }

                event5_flag = 1;
            }


            GUI_BASE(timer1)->y = widget_offset;
            if (alien && widget_offset == 0 && speed <= end_speed &&
                speed >= -end_speed)
            {
                render_flag = 0;
            }
            if (alien && widget_offset != 0)
            {
                if (speed > 0 && speed <= end_speed)
                {

                    if (_UI_ABS(widget_offset) <= end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        touch_y += end_speed;
                    }


                }
                else if (speed < 0 && speed >= -end_speed)
                {

                    if (_UI_ABS(widget_offset) <= end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        touch_y -= end_speed;
                    }

                }
                else if (speed == 0)
                {
                    if (_UI_ABS(widget_offset) <= end_speed)
                    {
                        widget_offset = 0;
                    }
                    if (widget_offset > 0)
                    {
                        if (widget_offset > gap / 2)
                        {
                            touch_y += end_speed;
                        }
                        else
                        {
                            touch_y -= end_speed;
                        }
                    }
                    else if (widget_offset < 0)
                    {
                        if (-widget_offset > gap / 2)
                        {
                            touch_y -= end_speed;
                        }
                        else
                        {
                            touch_y += end_speed;
                        }
                    }
                }
            }
            if (time_array_offset >= 0)
            {
                for (size_t i = 0; i < count; i++)
                {

                    int index = time_array_offset + i;
                    if (index >= time_array_size)
                    {
                        index -= time_array_size;
                    }
                    if (index < 0)
                    {
                        index += time_array_size;
                    }
                    const char *text = TICKET_NAME_ARRAY[index];
                    if (i == (count) / 2)
                    {
                        index_offset = index;
                    }

                    render(index, (void *)widget_array[i]);
                }
            }
        }
        else
        {
            history_y = touch_y;
        }
    }
    else
    {
        touch_y = wheel->history_y;
        {
            time_array_offset = -(touch_y / gap % time_array_size);
            int widget_offset = touch_y % gap;
            GUI_BASE(timer1)->y = widget_offset;
            if (alien && widget_offset == 0 && speed <= end_speed &&
                speed >= -end_speed)
            {
                render_flag = 0;
            }
            if (alien && widget_offset != 0)
            {
                if (speed > 0 && speed <= end_speed)
                {
                    if (_UI_ABS(widget_offset) <= end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        touch_y += end_speed;
                    }
                }
                else if (speed < 0 && speed >= -end_speed)
                {
                    if (_UI_ABS(widget_offset) <= end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        touch_y -= end_speed;
                    }
                }
                else if (speed == 0)
                {
                    if (_UI_ABS(widget_offset) <= end_speed)
                    {
                        widget_offset = 0;
                    }
                    if (widget_offset > 0)
                    {
                        if (widget_offset > gap / 2)
                        {
                            touch_y += end_speed;
                        }
                        else
                        {
                            touch_y -= end_speed;
                        }
                    }
                    else if (widget_offset < 0)
                    {
                        if (-widget_offset > gap / 2)
                        {
                            touch_y -= end_speed;
                        }
                        else
                        {
                            touch_y += end_speed;
                        }
                    }
                }
            }
            {
                for (size_t i = 0; i < count; i++)
                {

                    int index = time_array_offset + i;
                    if (index >= time_array_size)
                    {
                        index -= time_array_size;
                    }
                    if (index < 0)
                    {
                        index += time_array_size;
                    }
                    const char *text = TICKET_NAME_ARRAY[index];
                    if (i == (count) / 2)
                    {
                        index_offset = index;
                    }

                    render(index, (void *)widget_array[i]);
                }
            }
        }
    }
}
static void ticket(void *obj, gui_event_t e, void *param)
{
    gui_log(TICKET_NAME_ARRAY[(size_t)param]); gui_log("\r");
}
static const void *func_array[TICKET_COUNT] =
{
    ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket,
    ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket,
};
static const char *string_array[TICKET_COUNT] =
{
    "TICKET 1",
    "TICKET 2",
    "TICKET 3",
    "TICKET 4",
    "TICKET 5",
    "TICKET 6",
    "TICKET 7",
    "TICKET 8",
    "TICKET 9",
    "TICKET 10",
    "TICKET 11",
    "TICKET 12",
    "TICKET 13",
    "TICKET 14",
    "TICKET 15",
    "TICKET 16",
    "TICKET 17",
    "TICKET 18",
    "TICKET 19",
    "TICKET 20",
    "TICKET 21",
    "TICKET 22",
    "TICKET 23",
    "TICKET 24",
};
static void update_callback(void *obj, gui_event_t e, void *param)
{
    gui_log(param);
    GUI_WIDGET_POINTER_BY_NAME(text, UPDATE_TEXT_NAME)
    gui_obj_hidden(text, 0);
}
static void menu_ui(gui_obj_t *parent)
{
    {
        const char *text = "update";
        gui_text_t *t = gui_text_create(parent, UPDATE_TEXT_NAME, 0, 20, gui_get_screen_width(), font_size);
        gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
        const void *addr1 = FONT;
        gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, LEFT);
        gui_obj_hidden(GUI_BASE(t), 1);
    }
    time_array_size = TICKET_COUNT;
    gui_win_t *win = gui_win_create(parent, 0, 0, 0, gui_get_screen_width(), gui_get_screen_height());
    gui_win_t *timer1 = gui_win_create(win, 0, 0, 0, 0, 0);
    gui_win_set_animate(win, 1000, -1, override, timer1);
    for (size_t i = 0; i < count; i++)
    {
        gui_win_t *win = gui_win_create(timer1, 0, 0, i * gap, gui_get_screen_width(), gap);
        widget_array[i] = GUI_BASE(win);
        gui_img_t *img = gui_img_create_from_mem(win, 0, BACKGROUND_IMAGE, 0, 0, 0, 0);
        const char *text = TICKET_NAME_ARRAY[i];
        gui_text_t *t = gui_text_create(win, 0, 0, 20, GUI_BASE(win)->w, font_size);
        gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
        const void *addr1 = FONT;
        gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, LEFT);

        gui_win_click(win, (void *)CLICK_FUNCTION_ARRAY[i], (void *)i);
    }
    gui_obj_add_event_cb(win, update_callback, GUI_EVENT_5, "update\n");


}
static void jump(void *obj, gui_event_t e, gui_multi_level_t *param)
{
    GUI_API(gui_multi_level_t).jump(param, 1, 0);
}

void alipay_menu_ui(gui_obj_t *parent)
{

    gui_canvas_rect_create(parent, 0, 0, 0, gui_get_screen_width(), gui_get_screen_height(),
                           APP_COLOR_SILVER);
    gui_multi_level_t *qrcode = gui_multi_level_create(parent, 0, qrcode_ui);
    gui_multi_level_t *menu = gui_multi_level_create(qrcode, 0, menu_ui);
    gui_win_t *win = gui_win_create(qrcode, 0, 0, 0, gui_get_screen_width(), gui_get_screen_height());
    gui_win_click(win, jump, qrcode);

}





