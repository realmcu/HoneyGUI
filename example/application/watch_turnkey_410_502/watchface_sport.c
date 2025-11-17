/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "gui_win.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_video.h"
#include "gui_text.h"
#include "app_main_watch.h"
#include "gui_canvas.h"
#include "gui_view_instance.h"
#include "cJSON.h"
/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "watchface_sport_view"

#define RADIUS 210
/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
static char time_text_content[10] = {0};

static uint8_t *img_data_activity = NULL;
static char *move_content = NULL;
static char *ex_content = NULL;
static char *stand_content = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void clear_watchface_sport_view(gui_view_t *view)
{
    (void)view;
    if (img_data_activity)
    {
        gui_lower_free(img_data_activity);
        img_data_activity = NULL;
    }
    if (move_content)
    {
        gui_free(move_content);
        move_content = NULL;
    }
    if (ex_content)
    {
        gui_free(ex_content);
        ex_content = NULL;
    }
    if (stand_content)
    {
        gui_free(stand_content);
        stand_content = NULL;
    }
}
static void time_update_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    if (!timeinfo)
    {
        return;
    }

    sprintf(time_text_content, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "t_time", obj);
    gui_text_content_set((gui_text_t *)t_time, time_text_content, strlen(time_text_content));
}
static int steps_to_calories(int steps)
{
    return (int)(steps * 0.04f);
}
static void arc_activity_cb(NVGcontext *vg)
{
    cJSON *root;
    extern char *cjson_content;
    if (!cjson_content)
    {
        return;
    }
    else
    {
        root = cJSON_Parse(cjson_content);
        if (!root)
        {
            gui_log("Error parsing JSON!\r\n");
            return;
        }
    }
    // parse activity array
    cJSON *activity_array = cJSON_GetObjectItemCaseSensitive(root, "activity");
    if (cJSON_IsArray(activity_array))
    {
        cJSON *act = cJSON_GetArrayItem(activity_array, 0);
        if (!act)
        {
            gui_log("get activity_array unsuccessful\n");
        }
        else
        {
            cJSON *move = cJSON_GetObjectItemCaseSensitive(act, "move");
            cJSON *ex = cJSON_GetObjectItemCaseSensitive(act, "exercise");
            cJSON *stand = cJSON_GetObjectItemCaseSensitive(act, "stand");

            int calories = steps_to_calories(move->valueint);
            nvgBeginPath(vg);
            nvgArc(vg, RADIUS / 2, RADIUS / 2, 100 - 8, 3 * M_PI_F / 2,
                   M_PI_F * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 20);
            nvgStrokeColor(vg, nvgRGB(58, 23, 29));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, RADIUS / 2, RADIUS / 2, 100 - 8, 3 * M_PI_F / 2,
                   M_PI_F * (1.5f + 2.0f * calories / 500.0f), NVG_CW);  // cap 500 kcol
            nvgStrokeWidth(vg, 20);
            nvgStrokeColor(vg, nvgRGB(230, 67, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, RADIUS / 2, RADIUS / 2, 100 - 33, 3 * M_PI_F / 2,
                   M_PI_F * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 20);
            nvgStrokeColor(vg, nvgRGB(30, 55, 25));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, RADIUS / 2, RADIUS / 2, 100 - 33, 3 * M_PI_F / 2,
                   M_PI_F * (1.5f + 2.0f * ex->valueint / 60.0f), NVG_CW);  // cap 60 min.
            nvgStrokeWidth(vg, 20);
            nvgStrokeColor(vg, nvgRGB(186, 253, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, RADIUS / 2, RADIUS / 2, 100 - 57, 3 * M_PI_F / 2,
                   M_PI_F * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 20);
            nvgStrokeColor(vg, nvgRGB(22, 50, 47));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, RADIUS / 2, RADIUS / 2, 100 - 57, 3 * M_PI_F / 2,
                   M_PI_F * (1.5f + 2.0f * stand->valueint / 6.0f), NVG_CW); // cap 30 times
            nvgStrokeWidth(vg, 20);
            nvgStrokeColor(vg, nvgRGB(117, 230, 229));
            nvgStroke(vg);

            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(move_text, "ac_move", current_view);
                sprintf(move_content, "%d",  calories);
                gui_text_content_set((gui_text_t *)move_text, move_content, strlen(move_content));
            }
            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(ex_text, "ac_ex", current_view);
                sprintf(ex_content, "%d", ex->valueint);
                gui_text_content_set((gui_text_t *)ex_text, ex_content, strlen(ex_content));
            }
            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(stand_text, "ac_stand", current_view);
                sprintf(stand_content, "%d", stand->valueint);
                gui_text_content_set((gui_text_t *)stand_text, stand_content, strlen(stand_content));
            }
        }
    }
    // clear
    cJSON_Delete(root);
}
static void img_sport_press_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    if (!obj)
    {
        return;
    }
    gui_log("To be developed\n");
}
static void img_heart_press_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    if (!obj)
    {
        return;
    }
    gui_view_switch_direct(gui_view_get_current(), gui_view_descriptor_get("heartrate_view"),
                           SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
void create_watchface_sport(gui_view_t *view)
{

    current_view = view;
    gui_obj_t *parent = GUI_BASE(view);
    gui_win_t *win = gui_win_create(parent, "win", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


    sprintf(time_text_content, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    gui_text_t *t_time = gui_text_create(win, "t_time", -25, 120, 0, 0);
    gui_text_set(t_time, time_text_content, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_text_content),
                 80);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, RIGHT);
    gui_text_rendermode_set(t_time, 2);


    {
        //text sport
        {
            if (move_content == NULL)
            {
                move_content = (char *)gui_malloc(30);
                ex_content = (char *)gui_malloc(30);
                stand_content = (char *)gui_malloc(30);
            }
            sprintf(move_content, "0");
            gui_text_t *move_text = gui_text_create(win, "ac_move", -25, 20 + 180, 0, 0);
            gui_text_set(move_text, (void *)move_content, GUI_FONT_SRC_TTF, gui_rgb(230, 67, 79),
                         strlen(move_content), 80);
            gui_text_type_set(move_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(move_text, RIGHT);

            sprintf(ex_content, "0");
            gui_text_t *ex_text = gui_text_create(win, "ac_ex", -25, 57 + 200, 0, 0);
            gui_text_set(ex_text, (void *)ex_content, GUI_FONT_SRC_TTF, gui_rgb(186, 253, 79),
                         strlen(ex_content), 80);
            gui_text_type_set(ex_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(ex_text, RIGHT);


            sprintf(stand_content, "0");
            gui_text_t *stand_text = gui_text_create(win, "ac_stand", -25, 94 + 220, 0, 0);
            gui_text_set(stand_text, (void *)stand_content, GUI_FONT_SRC_TTF, gui_rgb(117, 230, 229),
                         strlen(stand_content), 80);
            gui_text_type_set(stand_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(stand_text, RIGHT);
        }
        int image_h = 210;
        int image_w = 210;
        int pixel_bytes = 4;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (!img_data_activity)
        {
            img_data_activity = gui_lower_malloc(buffer_size);
        }
        memset(img_data_activity, 0, buffer_size);
        gui_img_t *img = gui_img_create_from_mem(win, "activity", img_data_activity, 20, 210, 0, 0);
        img->base.w = image_w;
        img->base.h = image_h;
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, arc_activity_cb,
                                          img_data_activity);



    }
    {
        gui_img_t *img_sport = gui_img_create_from_mem(win, "sport_icon", APP_WORKOUT_ICON_BIN, 310, 30, 0,
                                                       0);
        gui_img_set_mode(img_sport, IMG_SRC_OVER_MODE);
        gui_img_scale(img_sport, 0.7, 0.7);
        gui_obj_add_event_cb(GUI_BASE(img_sport), (gui_event_cb_t)img_sport_press_cb,
                             GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    //icon weather
    {
        gui_img_t *img_weather = gui_img_create_from_mem(win, "weather_icon", APP_WEATHER_ICON_BIN, 20, 30,
                                                         0, 0);
        gui_img_set_mode(img_weather, IMG_SRC_OVER_MODE);
        gui_img_scale(img_weather, 0.7, 0.7);
        // gui_obj_add_event_cb(GUI_BASE(img_weather), (gui_event_cb_t)img_weather_press_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    //heart text
    {
        gui_win_t *win_heart = gui_win_create(win, "win_heart", 45, 440, 375, 60);
        gui_text_t *heart_text = gui_text_create(win_heart, "heart_text", 45, 0, 0, 0);
        gui_text_set(heart_text, "64 bpm, 1m ago", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("64 bpm, 1m ago"), 32);
        gui_text_type_set(heart_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(heart_text, LEFT);
        gui_img_t *img_heart = gui_img_create_from_mem(win_heart, "heart_icon", APP_HEART_RATE_ICON_BIN, 0,
                                                       0, 0, 0);
        gui_img_set_mode(img_heart, IMG_SRC_OVER_MODE);
        gui_img_scale(img_heart, 0.3, 0.3);
        gui_obj_add_event_cb(GUI_BASE(win_heart), (gui_event_cb_t)img_heart_press_cb,
                             GUI_EVENT_TOUCH_CLICKED, NULL);


    }
    gui_obj_create_timer(GUI_BASE(win), 500, true, time_update_cb);
}
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, (gui_view_t *)create_watchface_sport,
                  clear_watchface_sport_view);
