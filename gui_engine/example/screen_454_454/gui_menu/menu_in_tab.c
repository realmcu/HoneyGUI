#include "root_image/ui_resource.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "app_gui_main.h"
#include "gui_tabview.h"
#include "gui_obj.h"
#include "gui_img_scope.h"
#include "gui_common.h"
#include "gui_page.h"
#include "gui_grid.h"
#include "gui_button.h"
#include "gui_app.h"
#include "gui_progressbar.h"
#include "tp_algo.h"
#include "gui_canvas_rect.h"
static void press_callback(gui_win_t *button);
static void press_animate_cb(gui_win_t *button);
static void release_callback(gui_win_t *button);
static void release_animate_callback(gui_win_t *button);
static void deal_win_in_page(gui_obj_t *object, int page_y);
static void page_callback(gui_page_t *page);
static void heart_rate_cb(void);
static void menu_cb(gui_obj_t *obj);
static void stopwatch_cb(void);
static void map_cb(void);
static void card_cb(void);
static void box2d_cb(void);
static void compass_cb(void);
static void volume_cb(void);
static void cycle_tracking_cb(void);
static void setting_cb(void);
static void web_cb(void);
static void block_cb(void);
static void fruit_ninja_cb(void);
static void calculator_cb(void);
static int page_y_recode;
static void page_dtor(gui_obj_t *obj);
static gui_progressbar_t *pro;
static int window_index;
#define MENU_GAP 74
#define SCROLLBAR_SIZE 14
const static void *scrollbar_array[SCROLLBAR_SIZE];

static void gui_page_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                extern void gui_page_input_prepare(gui_obj_t *obj);
                gui_page_input_prepare(obj);
            }
            break;

        case OBJ_PREPARE:
            {
                extern void gui_page_prepare_rebound(gui_obj_t *obj);
                gui_page_prepare_rebound(obj);
            }
            break;

        case OBJ_DESTORY:
            page_dtor(obj);
            break;

        default:
            break;
        }
    }
}
#include "math.h"
/* app swap animation callback of the first app*/
void app_watch_mune_win_ani_cb(void *args, gui_win_t *win)
{
    // Calculate the progress percentage of the animation
    float pro = gui_win_get_aniamtion_progress_percent(win);

    // Adjust the scale of the window based on sine function of the progress percentage
    // This will create a smooth scaling effect as the progress advances
    gui_win_set_scale_rate(win, 1 + sinf(pro * M_PI / 2), 1 + sinf(pro * M_PI / 2));
    // Ensure the scope property is set to 1 (used to indicate visibility)
    gui_win_set_scope(win, 1);
    // Set the opacity of the window's base GUI element, gradually decreasing as the animation progresses
    // This creates a fading effect by setting opacity inversely proportional to the progress percentage
    gui_win_set_opacity(win, (1.0f - pro) * UINT8_MAX);
    // Check if the animation has reached its last frame
    if (gui_win_is_animation_end_frame(win))
    {
        // Shutdown the GUI application
        gui_app_shutdown(get_app_watch_ui());
    }
}

#define MENU_WIN_NAME "menu win name"
void design_tab_menu(void *parent)
{
    gui_win_t *win = gui_win_create(parent, MENU_WIN_NAME, 0, 0, 0, 0);
    gui_win_set_animate(win, 2000, 0, app_watch_mune_win_ani_cb, 0);
    gui_win_stop_animation(win);//aniamtion start to play until button click event
    /* app swap animation configration of the first app*/
    gui_canvas_rect_create((void *)win, "canvas_rect", 0, 0, gui_get_screen_width(),
                           gui_get_screen_height(),
                           gui_rgba(UINT8_MAX,
                                    UINT8_MAX, UINT8_MAX, 100));
    gui_page_t *page = gui_page_create(win, 0, 0, 0, 0, 0);
    //gui_page_rebound(page, 1);
    GET_BASE(page)->obj_cb = gui_page_cb;
    GET_BASE(page)->has_input_prepare_cb = true;
    GET_BASE(page)->has_prepare_cb = true;
    GET_BASE(page)->has_destroy_cb = true;
    GET_BASE(page)->y = page_y_recode;
    page_y_recode = 0;
    gui_page_set_animate(page, 1000, -1, page_callback, page);
    pro = gui_progressbar_movie_create(win, (void *)scrollbar_array, SCROLLBAR_SIZE, 435, 200);
    gui_img_set_mode((void *)pro->c, IMG_SRC_OVER_MODE);
    //gui_page_rebound(page, true);
    gui_page_center_alignment(page, MENU_GAP);
    static void *array[] =
    {
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
    };
    static char *text_array[] =
    {
        "Heart Rate",
        "Menu",
        "Stopwatch",
        "Map",
        "CARD",
        "box2d",
        "Compass",
        "Volume",
        "Cycle tracking",
        "Setting",
        "Web",
        "Block",
        "Fruit Ninja",
        "Calculator",
        "Music",
        "Sport",
    };
    int array_size = sizeof(array) / sizeof(array[0]);
    static gui_win_t *button_array[sizeof(array) / sizeof(array[0])];
    gui_grid_t *grid = gui_grid_create(page, 27, 0, array_size, 1, 0, MENU_GAP);
    for (size_t i = 0; i < array_size; i++)
    {

        gui_win_t *button = gui_win_create(grid, 0, 0, 0, 400, 70);
        gui_win_press(button, press_animate_cb, button);
        gui_win_release(button, release_animate_callback, button);
        //
        {
            gui_img_t *img = gui_img_create_from_mem(button, 0, RECT1_BIN, 0, 0, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        }
        {
            gui_img_t *img = gui_img_create_from_mem(button, 0, array[i], 3, 3, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        }
        button_array[i] = button;
        char *text = text_array[i];
        int font_size = 16;
        gui_text_t *t = gui_text_create(button, "txt", 70, 27, gui_get_screen_width(), font_size);
        if (i < 14)
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(42, 5, 79), strlen(text), font_size);
        }
        else
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(200, 200, 200), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    gui_win_click(button_array[0],  heart_rate_cb,     button_array[0]);
    gui_win_click(button_array[1],  menu_cb,           button_array[1]);
    gui_win_click(button_array[2],  stopwatch_cb,      button_array[2]);
    gui_win_click(button_array[3],  map_cb,            button_array[3]);
    gui_win_click(button_array[4],  card_cb,           button_array[4]);
    gui_win_click(button_array[5],  box2d_cb,          button_array[5]);
    gui_win_click(button_array[6],  compass_cb,        button_array[6]);
    gui_win_click(button_array[7],  volume_cb,         button_array[7]);
    gui_win_click(button_array[8],  cycle_tracking_cb, button_array[8]);
    gui_win_click(button_array[9],  setting_cb,        button_array[9]);
    gui_win_click(button_array[10], web_cb,            button_array[10]);
    gui_win_click(button_array[11], block_cb,          button_array[11]);
    gui_win_click(button_array[12], fruit_ninja_cb,    button_array[12]);
    gui_win_click(button_array[13], calculator_cb,    button_array[13]);
}

static void press_callback(gui_win_t *button)
{
    touch_info_t *tp = tp_get_info();
    static bool first_flag;
    static float current_scale;
    if (!first_flag)
    {
        first_flag = true;
        current_scale = 1;
    }
    /**
     * @brief Shrunk current scale to 0.7 in animation duration
     *  y = (0.7-scale)x+scale;
     */
    float scale = (0.7f - current_scale) * button->animate->progress_percent + current_scale;
    if (tp->deltaX == 0)
    {
        matrix_translate(GET_BASE(button)->w / 2, GET_BASE(button)->h / 2, GET_BASE(button)->matrix);
        matrix_scale(scale, scale, GET_BASE(button)->matrix);
        matrix_translate(GET_BASE(button)->w / -2, GET_BASE(button)->h / -2, GET_BASE(button)->matrix);
    }
    if (button->animate->progress_percent == 1)
    {
        first_flag = false;
    }

}
static void press_animate_cb(gui_win_t *button)
{
    gui_win_set_animate(button, 1000, 0, press_callback, button);
}
static void release_callback(gui_win_t *button)
{
    static bool first_flag = false;
    static float current_scale;
    if (!first_flag)
    {
        first_flag = true;
        current_scale = GET_BASE(button)->matrix->m[0][0];
    }
    /**
     * @brief Shrunk current scale to 1 in animation duration
     *  y = (1-scale)x+scale;
     */
    float scale = (1 - current_scale) * button->animate->progress_percent + current_scale;
    {
        matrix_translate(GET_BASE(button)->w / 2, GET_BASE(button)->h / 2, GET_BASE(button)->matrix);
        matrix_scale(1, 1, GET_BASE(button)->matrix);
        matrix_translate(GET_BASE(button)->w / -2, GET_BASE(button)->h / -2, GET_BASE(button)->matrix);
    }
    if (button->animate->progress_percent == 1)
    {
        first_flag = false;
    }
}
static void release_animate_callback(gui_win_t *button)
{
    gui_win_set_animate(button, 1000, 0, release_callback, button);
}
static void deal_win_in_page(gui_obj_t *object, int page_y)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == WINDOW)
        {
            int ay = window_index * MENU_GAP + page_y;
            window_index++;
            if (ay >= (float)(0 - obj->h) &&
                ay < (float)gui_get_screen_height())
            {
                float scale = (1 - 0.5f * _UI_ABS(((float)(ay + (float)(obj->h / 2)) -
                                                   (float)gui_get_screen_height() / 2) / (float)(gui_get_screen_height() / 2)));
                GUI_TYPE(gui_win_t, obj)->scale = scale;
                obj->not_show = false;
            }
            else
            {
                obj->not_show = true;
            }
        }
        deal_win_in_page(obj, page_y);
    }
}
const static void *scrollbar_array_top[] =
{
    BARS1_BIN,
    BARS2_BIN,
    BARS3_BIN,
};
const static void *scrollbar_array_buttom[] =
{
    BAR40_BIN,
    BAR41_BIN,
    BAR42_BIN,
    BAR43_BIN,
};
const static void *scrollbar_array[SCROLLBAR_SIZE] =
{
    BAR1_BIN,  BAR4_BIN,

    BAR7_BIN,

    BAR10_BIN,

    BAR13_BIN,

    BAR16_BIN,

    BAR19_BIN,

    BAR22_BIN,

    BAR25_BIN,

    BAR28_BIN,

    BAR31_BIN,

    BAR34_BIN,

    BAR38_BIN,
    BAR39_BIN,
};
static void page_callback(gui_page_t *page)
{
    window_index = 0;
    deal_win_in_page((void *)page, GUI_BASE(page)->y);



    // gui_log("%f,%d,%d\n", ((float)(page->start_y - GET_BASE(page)->y)) / (float)(GET_BASE(
    //         page)->h - gui_get_screen_height()), page->start_y, GET_BASE(page)->y);
    float progress = ((float)(page->start_y - GET_BASE(page)->y)) / (float)(GET_BASE(
                                                                                page)->h - gui_get_screen_height());
    if (pro)
    {
        if (progress >= 0 && progress <= 1)
        {
            {
                pro->color_hl = (uint32_t)(uintptr_t)scrollbar_array;
                pro->max = sizeof(scrollbar_array) / sizeof(void *);
            }
            gui_progressbar_set_percentage(pro, progress) ;
        }
        else if (progress < 0)
        {
            {
                pro->color_hl = (uint32_t)(uintptr_t)scrollbar_array_top;
                pro->max = sizeof(scrollbar_array_top) / sizeof(void *);
            }
            float progress_top = -progress;
            gui_progressbar_set_percentage(pro, progress_top) ;
        }
        else if (progress > 1)
        {
            {
                pro->color_hl = (uint32_t)(uintptr_t)scrollbar_array_buttom;
                pro->max = sizeof(scrollbar_array_buttom) / sizeof(void *);
            }
            float progress_buttom = progress - 1;
            gui_progressbar_set_percentage(pro, progress_buttom) ;
        }
    }


}
static void heart_rate_cb()
{
    //GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_HEART_RATE))
    GUI_APP_STARTUP(APP_HEART_RATE) // cppcheck-suppress unknownMacro
    gui_app_layer_top();
}
/* The first app's nimation starts playing on click event*/
static void menu_cb(gui_obj_t *obj)
{
    // Retrieve the window object with the name specified by MENU_WIN_NAME from the root of the GUI tree
    gui_win_t *win = 0;
    gui_obj_tree_get_widget_by_name(gui_get_root(obj), MENU_WIN_NAME, (void *)&win);

    // If the window object is found, enable its animation by setting the animate flag to 1
    if (win)
    {
        win->animate->animate = 1;
    }
    GUI_APP_STARTUP(APP_MENU);  // Start up the menu application(NEXT APP)

    // Bring the next application layer to the top, ensuring it's displayed above the first app layer
    gui_app_layer_top();
}
/* In the button click callback, execute the next app startup.*/
static void stopwatch_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_STOPWATCH))
}
static void map_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_MAP))
}
static void card_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_CARDVIEW))
}
static void box2d_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_BOX2D))
}
static void compass_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_COMPASS))
}
static void volume_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_VOLUME))
}
static void cycle_tracking_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_CYCLE_TRACKING))
}
static void setting_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_SETTING))
}
static void web_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_WEB))
}
static void block_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_BLOCK))
}
static void page_dtor(gui_obj_t *obj)
{
    extern void gui_page_destory(gui_obj_t *obj);
    gui_page_destory(obj);
    page_y_recode = obj->y;
}
static void fruit_ninja_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_FRUIT_NINJA))
}
static void calculator_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_CALCULATOR))
}
// Define an array for window list, containing 15 windows
static gui_win_t *win_list_array[15];

// Define the gap between windows
static const int win_list_gap = 100;

// Define the length of the array
static const int array_length = 15;

// Define the speed threshold to stop
static const int end_speed = 3;

// Define an array to record the Y-axis delta from the last touch event
static int16_t recode[5];

// Define current speed and target speed
static int speed, target;

// Define a boolean to control alignment
static bool alien = true;
static int touch_y;

// Function to update touch speed
static void update_speed(gui_obj_t *obj)
{
    // Get touch information
    touch_info_t *tp = tp_get_info();
    int recode_num = 4;

    // Update recorded values
    for (size_t i = 0; i < recode_num; i++)
    {
        recode[i] = recode[i + 1];
    }

    // Record the current touch Y-axis delta
    recode[recode_num] = tp->deltaY;

    // Calculate speed
    speed = recode[recode_num] - recode[0];
    //gui_log("tp->deltaY:%d,speed:%d\n", tp->deltaY, speed);

    // Define maximum and minimum speed
    int max_speed = GUI_PAGE_MAX_SPEED;
    int min_speed = GUI_PAGE_MIN_SPEED;

    // Check if speed exceeds the maximum speed and limit it
    if (speed > max_speed)
    {
        speed = max_speed;
    }
    else if (speed < -max_speed)
    {
        speed = -max_speed;
    }

    // Check if speed is below the minimum speed and limit it
    if ((speed > 0) && (speed < min_speed))
    {
        speed = min_speed;
    }
    else if ((speed < 0) && (speed > -min_speed))
    {
        speed = -min_speed;
    }
}

// Define a flag for rendering
static bool render;

// Function to update inertial scrolling
static void gui_page_update_inertial(gui_obj_t *obj)
{
    // Import touchpad module
    IMPORT_GUI_TOUCHPAD
    //gui_log("speed:%d\n", speed);

    // If the speed is greater than the stop threshold
    if (speed > end_speed)
    {
        // Increase historical Y coordinate and decrease speed
        touch_y += speed;
        speed -= 1;
    }
    // If the speed is less than the negative stop threshold
    else if (speed < -end_speed)
    {
        // Increase historical Y coordinate and increase speed (towards 0)
        touch_y += speed;
        speed += 1;
    }
}

static void win_list_overwrite(void **param, gui_obj_t *win_aniamte)
{
    // Import touchpad and wheel modules
    IMPORT_GUI_TOUCHPAD
    IMPORT_GUI_WHEEL

    // Define static variables to record whether the wheel takes over, the offset for time array, and the historical Y coordinate
    static bool wheel_take_over;
    static char time_array_offset;
    static int history_y;

    // Get the window list object
    gui_obj_t *win_list = param[0];

    // If the window list is null, return immediately
    if (!win_list)
    {
        return;
    }

    // If the wheel has not taken over touch input
    if (!wheel_take_over)
    {
        // Check if the touch is pressed within the bounds of the animated window
        if (touch->pressed && touch->x > win_aniamte->x && touch->x < win_aniamte->x + win_aniamte->w)
        {
            // Record the initial historical Y coordinate, reset speed, and clear records
            history_y = touch_y;
            speed = 0;
            memset(recode, 0, sizeof(recode));
            static int i;

        }

        // Check if the touch is pressing within the bounds of the animated window
        if (touch->pressing &&
            touch->x > win_aniamte->x &&
            touch->x < win_aniamte->x + win_aniamte->w &&
            touch->type != TOUCH_HOLD_X &&
            touch->type != TOUCH_RIGHT_SLIDE &&
            touch->type != TOUCH_LEFT_SLIDE)
        {
            // Update historical Y coordinate
            touch_y = history_y + touch->deltaY;
            /**
             * Calculate offset index and window offset
             * Index = offset / gap % array_length
             * WidgetOffset = offset % gap
            */
            // Update scrolling speed
            update_speed(win_list);

            // Set render flag
            render = 1;
        }
        else
        {
            // If the touch is not within the window bounds, execute inertial scrolling update
            gui_page_update_inertial(win_list);
        }

        // If rendering is needed
        if (render)
        {
            // Calculate the offset for the time array and the widget offset
            time_array_offset = -(touch_y / win_list_gap % array_length);
            int widget_offset = touch_y % win_list_gap;

            // Set the Y coordinate of the window list
            GUI_BASE(win_list)->y = widget_offset;

            // Check if the condition for the animation stop is met
            if (alien && widget_offset == 0 && speed <= end_speed && speed >= -end_speed)
            {
                render = 0;
            }

            // If still needing animation adjustment
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
                        if (widget_offset > win_list_gap / 2)
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
                        if (-widget_offset > win_list_gap / 2)
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

            // Hide all items in the window list
            for (size_t i = 0; i < array_length; i++)
            {
                GUI_BASE(win_list_array[i])->not_show = 1;
            }

            // Calculate and display the currently visible window items
            for (size_t i = 0; i < gui_get_screen_height() / win_list_gap + 3; i++)
            {
                int index = time_array_offset + i;
                if (index >= array_length)
                {
                    index -= array_length;
                }
                if (index < 0)
                {
                    index += array_length;
                }

                GUI_BASE(win_list_array[index])->y = i * win_list_gap;
                GUI_BASE(win_list_array[index])->not_show = 0;
            }
        }
        else
        {
            // Record the current historical Y coordinate
            history_y = touch_y;
        }
    }
    static uint8_t checksum;
    uint8_t last = checksum;
    checksum = 0;
    checksum = gui_obj_checksum(0, (uint8_t *)win_list, sizeof(gui_win_t));

    if (last != checksum)
    {
        gui_fb_change();
    }
}

static void page_scroll(gui_obj_t *parent)
{
    // Define an array of icons containing pointers to different icons
    void *icon_array[] =
    {
        ICON_MIC_MUTE_OFF_BIN, ICON_MIC_MUTE_OFF_TOUCH_BIN, ICON_MIC_MUTE_ON_BIN, ICON_MIC_MUTE_ON_TOUCH_BIN,
        ICON_BLUETOOTH_OFF_BIN, ICON_BLUETOOTH_ON_BIN, ICON_BLUETOOTH_TOUCH_BIN, ICON_BUDS_OFF_BIN,
        ICON_BUDS_ON_BIN, ICON_BUDS_TOUCH_BIN, ICON_BUTTON_OFF_BIN, ICON_BUTTON_ON_BIN,
        ICON_PHONE_TOUCH_BIN, ICON_PHONE_ON_BIN, ICON_PHONE_OFF_BIN
    };

    // Define an array of texts corresponding to each window label
    char *win_list_text_array[] =
    {
        "ICON_MIC_MUTE_OFF_BIN", "ICON_MIC_MUTE_OFF_TOUCH_BIN", "ICON_MIC_MUTE_ON_BIN",
        "ICON_MIC_MUTE_ON_TOUCH_BIN", "ICON_BLUETOOTH_OFF_BIN", "ICON_BLUETOOTH_ON_BIN",
        "ICON_BLUETOOTH_TOUCH_BIN", "ICON_BUDS_OFF_BIN", "ICON_BUDS_ON_BIN", "ICON_BUDS_TOUCH_BIN",
        "ICON_BUTTON_OFF_BIN", "ICON_BUTTON_ON_BIN", "ICON_PHONE_TOUCH_BIN", "ICON_PHONE_ON_BIN",
        "ICON_PHONE_OFF_BIN"
    };

    // Get the length of the array
    const int array_length = sizeof(win_list_text_array) / sizeof(win_list_text_array[0]);

    // Create an animated window at the initial position (0, 100), without a specified size
    gui_win_t *win_animate = gui_win_create(parent, 0, 100, -100, 0, 0);

    // Create a window list inside the animated window
    gui_win_t *win_list = gui_win_create(win_animate, 0, 0, 0, 0, 0);

    // Static parameter array to pass the window list and window list array
    static void *param[2];
    param[0] = win_list;
    param[1] = win_list_array;

    // Set properties for the animated window: duration is 1000 ms, loop count is -1 (infinite loop), callback function is win_list_overwrite, with param as argument
    gui_win_set_animate(win_animate, 1000, -1, win_list_overwrite, param);

    // Define the gap between windows
    int gap = win_list_gap;

    // Iterate over each item in the text array, create corresponding windows and icons, and set texts
    for (size_t i = 0; i < array_length; i++)
    {
        // Create a window, location is calculated based on the iterator index and the gap
        gui_win_t *win = gui_win_create(win_list, 0, 0, i * gap, 200, gap);
        win_list_array[i] = win;

        // Create an icon inside the window, icon is loaded from memory, the initial position is set to 0
        gui_img_t *icon = gui_img_create_from_mem(win, 0, icon_array[i], 0, 0, 0, 0);

        {
            // Create text inside the window
            gui_text_t *t = gui_text_create(win, win_list_text_array[i], 100, 0, 100, 16);

            // Set text content, font, color, and length
            gui_text_set(t, win_list_text_array[i], GUI_FONT_SRC_BMP, APP_COLOR_BLACK,
                         strlen(win_list_text_array[i]), 16);

            // Define the font address
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;

            // Set text type to the font resource from memory address
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);

            // Set text mode to center
            gui_text_mode_set(t, CENTER);
        }
    }
}


void menu_loop_ui(gui_obj_t *parent)
{
    gui_canvas_rect_create(parent, 0, 0, 0, gui_get_screen_width(), gui_get_screen_height(),
                           APP_COLOR_SILVER);
    page_scroll(parent);


}





