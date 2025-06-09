#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include "root_image_hongkong/ui_resource.h"
#include "gui_win.h"
#include "gui_server.h"
#include "app_hongkong.h"
#include "gui_menu_cellular.h"
#include "math.h"
#include "guidef.h"
#include "gui_list.h"
#include "tp_algo.h"
#include "gui_canvas_rect.h"
#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502


#define MENU_GAP 125
#define APP_NUM 30
#define CURRENT_VIEW_NAME "menu_view"
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
const static gui_view_descriptor_t *activity_view = NULL;
const static gui_view_descriptor_t *box2d_ring_view = NULL;
const static gui_view_descriptor_t *fruit_ninja_view = NULL;
const static gui_view_descriptor_t *heartrate_view = NULL;
const static gui_view_descriptor_t *soccer_view = NULL;
const static gui_view_descriptor_t *weather_view = NULL;
const static gui_view_descriptor_t *flower_view = NULL;
const static gui_view_descriptor_t *butterfly_view = NULL;
const static gui_view_descriptor_t *applist_view = NULL;
const static gui_view_descriptor_t *disc_view = NULL;
const static gui_view_descriptor_t *face_view = NULL;
const static gui_view_descriptor_t *prism_thick_view = NULL;
const static gui_view_descriptor_t *prism3d_mirror_view = NULL;
const static gui_view_descriptor_t *windmill_view = NULL;
const static gui_view_descriptor_t *energybox_view = NULL;
const static gui_view_descriptor_t *notification_view = NULL;
const static gui_view_descriptor_t *pandkoi_view = NULL;
const static gui_view_descriptor_t *seawater_view = NULL;
const static gui_view_descriptor_t *firefly_view = NULL;
const static gui_view_descriptor_t *rainbow_digital_view = NULL;
const static gui_view_descriptor_t *koiclock_view = NULL;
void app_menu_design(gui_view_t *view);
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = app_menu_design,
};

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    watchface_view = gui_view_descriptor_get("watchface_view");
    activity_view = gui_view_descriptor_get("activity_view");
    box2d_ring_view = gui_view_descriptor_get("box2d_ring_view");
    fruit_ninja_view = gui_view_descriptor_get("fruit_ninja_view");
    heartrate_view = gui_view_descriptor_get("heartrate_view");
    soccer_view = gui_view_descriptor_get("soccer_view");
    weather_view = gui_view_descriptor_get("weather_view");
    flower_view = gui_view_descriptor_get("flower_view");
    butterfly_view = gui_view_descriptor_get("butterfly_view");
    applist_view = gui_view_descriptor_get("applist_view");
    disc_view = gui_view_descriptor_get("disc_view");
    face_view = gui_view_descriptor_get("face_view");
    prism_thick_view = gui_view_descriptor_get("prism_thick_view");
    prism3d_mirror_view = gui_view_descriptor_get("prism3d_mirror_view");
    windmill_view = gui_view_descriptor_get("windmill_view");
    energybox_view = gui_view_descriptor_get("energybox_view");
    notification_view = gui_view_descriptor_get("notification_view");
    pandkoi_view = gui_view_descriptor_get("pandkoi_view");
    seawater_view = gui_view_descriptor_get("seawater_view");
    firefly_view = gui_view_descriptor_get("firefly_view");
    rainbow_digital_view = gui_view_descriptor_get("rainbow_digital_view");
    koiclock_view = gui_view_descriptor_get("koiclock_view");

    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

extern uint8_t menu_style;
static int16_t list_offset_his = 0;
// bool sidebar_flag = 0;

// extern void sidebar_app_array_fill(void *img_addr, gui_event_cb_t callback_function);

void switch_APP_BOX2D_RING(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, box2d_ring_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_FRUIT_NINJA(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, fruit_ninja_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_HEART_RATE(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, heartrate_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_ACTIVITY(void *obj, gui_event_t e, void *param)
{

    gui_view_switch_direct(current_view, activity_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void app_back2watchface_cb(void)
{
    gui_view_switch_direct(current_view, watchface_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_SOCCER(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, soccer_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_FLOWER(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, flower_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_WEATHER(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, weather_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_BUTTERFLY(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, butterfly_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_APPLIST(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, applist_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_DISC(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, disc_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_FACE(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, face_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_PRISM_THICK(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, prism_thick_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_PRISM_MIRROR(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, prism3d_mirror_view, SWITCH_IN_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
    gui_view_set_animate_step(gui_view_get_current(), 1000);
}

void switch_APP_WINDMILL(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, windmill_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_ENERGYBOX(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, energybox_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

void switch_APP_NOTIFICATION(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, notification_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
void switch_APP_PANDKOI(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, pandkoi_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
void switch_APP_SEAWATER(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, seawater_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
void switch_APP_FIREFLY(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, firefly_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
void switch_APP_RAINBOW_DIGITAL(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, rainbow_digital_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
void switch_APP_KOICLOCK(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, koiclock_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void return_timer_cb()
{
    touch_info_t *tp = tp_get_info();
    GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, app_back2watchface_cb)
}

static void list_timer_cb(void *obj)
{
    list_offset_his = ((gui_list_t *)obj)->offset;
}

void app_menu_design(gui_view_t *view)
{
    gui_win_t *win = gui_win_create(view, "win_app_menu", 0, 0, 0, 0);
    gui_obj_create_timer(GUI_BASE(win), 10, true, return_timer_cb);

    extern bool return_to_watchface_flag;
    return_to_watchface_flag = false;
    if (menu_style == 0)
    {
        gui_list_note_t *tab_array[APP_NUM];
        uint32_t *array[] =
        {
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            SOCCER_ICON_BIN,
            FLOWER_ICON_BIN,
            WEATHER_ICON_BIN,
            BUTTERFLY_ICON_BIN,
            APPLIST_ICON_BIN,
            DISC_ICON_BIN,
            FACE_ICON_BIN,
            PRISM_THICK_ICON_BIN,
            PRISM3D_ICON_BIN,
            WINDMILL_ICON_BIN,
            ENERGYBOX_ICON_BIN,
            NOTIFICATION_ICON_BIN,
            PANDKOI_ICON_BIN,
            SEAWATER_ICON_BIN,
            FIREFLY_ICON_BIN,
            RAINBOW_DIGITAL_ICON_BIN,
            KOI_CLOCK_ICON_BIN,
        };
        char *text_array[] =
        {
            "Heart Rate",
            "Fruit Ninja",
            "Box2d Ring",
            "Activity",
            "Soccer",
            "Flower",
            "Weather",
            "Butterfly",
            "App List",
            "Disc",
            "Face",
            "Prism Thick",
            "Prism Mirror",
            "Windmill",
            "Energybox",
            "Notification",
            "Pand Koi",
            "Sea Water",
            "Fire Fly",
            "Rainbow Digital",
            "koi a clock",
        };
        int array_size = sizeof(array) / sizeof(array[0]);

        uint8_t space = 5;

        int length = 100;
        gui_list_t *list = gui_list_create(win, "list", 0, 0, 0, 0, length, space,
                                           VERTICAL);
        gui_list_set_style(list, LIST_HELIX);
        gui_list_set_offset(list, list_offset_his);
        gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);

        gui_color_t color_t[] =
        {
            APP_COLOR_SILVER_OPACITY(150),
            APP_COLOR_WHITE_OPACITY,
        };

        for (uint8_t i = 0; i < array_size; i++)
        {
            gui_list_note_t *tab = gui_list_add_note(list, false);
            gui_canvas_rect_create(GUI_BASE(tab), "tab", 0, 0, 0, length, color_t[i % 2]);
            gui_img_t *img = gui_img_create_from_mem(tab, 0, array[i], 30, 0, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            tab_array[i] = tab;
            char *text = text_array[i];
            int font_size = 32;
            gui_text_t *t = gui_text_create(tab, "txt", 180, 27, gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 0, 0), strlen(text), font_size);
            gui_text_type_set(t, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        }
        gui_obj_add_event_cb(tab_array[0], (gui_event_cb_t)switch_APP_HEART_RATE,  GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[1], (gui_event_cb_t)switch_APP_FRUIT_NINJA, GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[2], (gui_event_cb_t)switch_APP_BOX2D_RING,  GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[3], (gui_event_cb_t)switch_APP_ACTIVITY,    GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[4], (gui_event_cb_t)switch_APP_SOCCER,      GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[5], (gui_event_cb_t)switch_APP_FLOWER,      GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[6], (gui_event_cb_t)switch_APP_WEATHER,     GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[7], (gui_event_cb_t)switch_APP_BUTTERFLY,   GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[8], (gui_event_cb_t)switch_APP_APPLIST,     GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[9], (gui_event_cb_t)switch_APP_DISC,       GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[10], (gui_event_cb_t)switch_APP_FACE,       GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[11], (gui_event_cb_t)switch_APP_PRISM_THICK, GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[12], (gui_event_cb_t)switch_APP_PRISM_MIRROR,
                             GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[13], (gui_event_cb_t)switch_APP_WINDMILL, GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[14], (gui_event_cb_t)switch_APP_ENERGYBOX, GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[15], (gui_event_cb_t)switch_APP_NOTIFICATION,
                             GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[16], (gui_event_cb_t)switch_APP_PANDKOI,
                             GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[17], (gui_event_cb_t)switch_APP_SEAWATER,
                             GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[18], (gui_event_cb_t)switch_APP_FIREFLY,
                             GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[19], (gui_event_cb_t)switch_APP_RAINBOW_DIGITAL,
                             GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_obj_add_event_cb(tab_array[20], (gui_event_cb_t)switch_APP_KOICLOCK,
                             GUI_EVENT_TOUCH_CLICKED,
                             NULL);
    }
    else
    {
        gui_canvas_rect_t *canvas_bg = gui_canvas_rect_create(GUI_BASE(win), "background", 0, 0,
                                                              SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgba(76, 76, 76, 255));
        uint32_t *array[] =
        {
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            SOCCER_ICON_BIN,
            FLOWER_ICON_BIN,
            WEATHER_ICON_BIN,
            BUTTERFLY_ICON_BIN,
            APPLIST_ICON_BIN,
            DISC_ICON_BIN,
            FACE_ICON_BIN,
            PRISM_THICK_ICON_BIN,
            PRISM3D_ICON_BIN,
            WINDMILL_ICON_BIN,
            ENERGYBOX_ICON_BIN,
            NOTIFICATION_ICON_BIN,
            PANDKOI_ICON_BIN,
            SEAWATER_ICON_BIN,
            FIREFLY_ICON_BIN,
            RAINBOW_DIGITAL_ICON_BIN,
            KOI_CLOCK_ICON_BIN,
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            SOCCER_ICON_BIN,
            FLOWER_ICON_BIN,
            WEATHER_ICON_BIN,
            BUTTERFLY_ICON_BIN,
            APPLIST_ICON_BIN,
            DISC_ICON_BIN,
            FACE_ICON_BIN,
            PRISM_THICK_ICON_BIN,
            PRISM3D_ICON_BIN,
            WINDMILL_ICON_BIN,
            ENERGYBOX_ICON_BIN,
            NOTIFICATION_ICON_BIN,
            PANDKOI_ICON_BIN,
            SEAWATER_ICON_BIN,
            FIREFLY_ICON_BIN,
            RAINBOW_DIGITAL_ICON_BIN,
            KOI_CLOCK_ICON_BIN,
        };
        gui_menu_cellular_t *menu = gui_menu_cellular_create(win, 100, array,
                                                             sizeof(array) / sizeof(uint32_t *));
        gui_menu_cellular_offset(menu, -200, 0);
        {
            struct gui_menu_cellular_gesture_parameter gesture_parameter_array[] =
            {
                {switch_APP_HEART_RATE, NULL}, {switch_APP_FRUIT_NINJA, NULL}, {switch_APP_BOX2D_RING, NULL}, {switch_APP_ACTIVITY, NULL},
                {switch_APP_SOCCER, NULL}, {switch_APP_FLOWER, NULL}, {switch_APP_WEATHER, NULL}, {switch_APP_BUTTERFLY, NULL},
                {switch_APP_APPLIST, NULL}, {switch_APP_DISC, NULL}, {switch_APP_FACE, NULL},
                {switch_APP_PRISM_THICK, NULL}, {switch_APP_PRISM_MIRROR, NULL}, {switch_APP_WINDMILL, NULL}, {switch_APP_ENERGYBOX, NULL},
                {switch_APP_NOTIFICATION, NULL}, {switch_APP_PANDKOI, NULL}, {switch_APP_SEAWATER, NULL}, {switch_APP_FIREFLY, NULL},
                {switch_APP_HEART_RATE, NULL}, {switch_APP_FRUIT_NINJA, NULL}, {switch_APP_BOX2D_RING, NULL}, {switch_APP_ACTIVITY, NULL},
                {switch_APP_SOCCER, NULL}, {switch_APP_FLOWER, NULL}, {switch_APP_WEATHER, NULL}, {switch_APP_BUTTERFLY, NULL},
                {switch_APP_APPLIST, NULL}, {switch_APP_DISC, NULL}, {switch_APP_FACE, NULL},
                {switch_APP_PRISM_THICK, NULL}, {switch_APP_PRISM_MIRROR, NULL}, {switch_APP_WINDMILL, NULL}, {switch_APP_ENERGYBOX, NULL},
                {switch_APP_NOTIFICATION, NULL}, {switch_APP_PANDKOI, NULL}, {switch_APP_SEAWATER, NULL}, {switch_APP_FIREFLY, NULL},
                {switch_APP_RAINBOW_DIGITAL, NULL}, {switch_APP_KOICLOCK, NULL},
            };
            gui_menu_cellular_on_click(menu, gesture_parameter_array,
                                       sizeof(gesture_parameter_array) / sizeof(gesture_parameter_array[0]));
        }
    }
}