/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_win.h"
#include "gui_text.h"
#include "root_image/ui_resource.h"
#include "gui_win.h"
#include "app_main_watch.h"
#include "gui_menu_cellular.h"
#include "guidef.h"
#include "gui_list.h"
#include "gui_canvas_rect.h"


/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct note_design_param
{
    void **click_cb;
    uint32_t **img_data_array;
} note_design_param_t;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "menu_view"
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_menu_design(gui_view_t *view);
static void clear_menu(gui_view_t *view);
static void switch_app_heart_rate(void *obj, gui_event_t e, void *param);
static void switch_app_fruit_ninja(void *obj, gui_event_t e, void *param);
static void switch_app_box2d_ring(void *obj, gui_event_t e, void *param);
static void switch_app_activity(void *obj, gui_event_t e, void *param);
static void switch_app_soccer(void *obj, gui_event_t e, void *param);
static void switch_app_weather(void *obj, gui_event_t e, void *param);
static void switch_app_flower(void *obj, gui_event_t e, void *param);
static void switch_app_butterfly(void *obj, gui_event_t e, void *param);
static void switch_app_applist(void *obj, gui_event_t e, void *param);
static void switch_app_disc(void *obj, gui_event_t e, void *param);
static void switch_app_face(void *obj, gui_event_t e, void *param);
static void switch_app_prism_thick(void *obj, gui_event_t e, void *param);
static void switch_app_prism_mirror(void *obj, gui_event_t e, void *param);
static void switch_app_windmill(void *obj, gui_event_t e, void *param);
static void switch_app_pandkoi(void *obj, gui_event_t e, void *param);
static void switch_app_seawater(void *obj, gui_event_t e, void *param);
static void switch_app_firefly(void *obj, gui_event_t e, void *param);
static void switch_app_rainbow_digital(void *obj, gui_event_t e, void *param);
static void switch_app_koiclock(void *obj, gui_event_t e, void *param);
static void switch_app_digital_clock(void *obj, gui_event_t e, void *param);
static void switch_app_countdown(void *obj, gui_event_t e, void *param);
static void switch_app_firework(void *obj, gui_event_t e, void *param);
static void switch_app_heart_particle(void *obj, gui_event_t e, void *param);
static void switch_app_butterfly_particle(void *obj, gui_event_t e, void *param);
static void switch_app_butterflys(void *obj, gui_event_t e, void *param);
static void switch_app_earth_clock(void *obj, gui_event_t e, void *param);
static void switch_app_labubu_digital(void *obj, gui_event_t e, void *param);
static void switch_app_flower_clock(void *obj, gui_event_t e, void *param);
static void switch_app_rainbow_analog(void *obj, gui_event_t e, void *param);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
const static gui_view_descriptor_t *activity_view = NULL;
const static gui_view_descriptor_t *box2d_ring_view = NULL;
const static gui_view_descriptor_t *fruit_ninja_view = NULL;
const static gui_view_descriptor_t *heartrate_view = NULL;
const static gui_view_descriptor_t *music_view = NULL;
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
const static gui_view_descriptor_t *pandkoi_view = NULL;
const static gui_view_descriptor_t *seawater_view = NULL;
const static gui_view_descriptor_t *firefly_view = NULL;
const static gui_view_descriptor_t *rainbow_digital_view = NULL;
const static gui_view_descriptor_t *koiclock_view = NULL;
const static gui_view_descriptor_t *digital_clock_view = NULL;
const static gui_view_descriptor_t *box2d_countdown_view = NULL;
const static gui_view_descriptor_t *fireworks_clock_view = NULL;
const static gui_view_descriptor_t *heart_particle_view = NULL;
const static gui_view_descriptor_t *butterfly_particle_view = NULL;
const static gui_view_descriptor_t *butterflys_view = NULL;
const static gui_view_descriptor_t *earth_clock_view = NULL;
const static gui_view_descriptor_t *labubu_digital_view = NULL;
const static gui_view_descriptor_t *flower_clock_view = NULL;
const static gui_view_descriptor_t *rainbow_analog_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = app_menu_design,
    .on_switch_out = clear_menu,
};

extern uint8_t menu_style;
static int16_t list_offset_his = 0;
static gui_menu_cellular_t *menu_cellular = NULL;
static int16_t cellular_offset_x = 0;
static int16_t cellular_offset_y = 0;

static gui_color_t color_t[2] = {0};
static note_design_param_t *design_p = NULL;

static const char *text_array[] =
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
    "Pand Koi",
    "Sea Water",
    "Fire Fly",
    "Rainbow Digital",
    "Koi a Clock",
    "Digital Clock",
    "Count Down Time",
    "Fireworks",
    "Heart Particle",
    "Butterfly Shadow",
    "Butterflys",
    "Earth Clock",
    "Labubu Digital",
    "Flower Clock ",
    "Rainbow Analog",
    "Music",
    "",
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
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
    music_view = gui_view_descriptor_get("music_view");
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
    pandkoi_view = gui_view_descriptor_get("pandkoi_view");
    seawater_view = gui_view_descriptor_get("seawater_view");
    firefly_view = gui_view_descriptor_get("firefly_view");
    rainbow_digital_view = gui_view_descriptor_get("rainbow_digital_view");
    koiclock_view = gui_view_descriptor_get("koiclock_view");
    digital_clock_view = gui_view_descriptor_get("digital_clock_view");
    box2d_countdown_view = gui_view_descriptor_get("box2d_countdown_view");
    fireworks_clock_view = gui_view_descriptor_get("fireworks_clock_view");
    heart_particle_view = gui_view_descriptor_get("heart_particle_view");
    butterfly_particle_view = gui_view_descriptor_get("butterfly_particle_view");
    butterflys_view = gui_view_descriptor_get("butterflys_view");
    earth_clock_view = gui_view_descriptor_get("earth_clock_view");
    labubu_digital_view = gui_view_descriptor_get("labubu_digital_view");
    flower_clock_view = gui_view_descriptor_get("flower_clock_view");
    rainbow_analog_view = gui_view_descriptor_get("rainbow_analog_view");

    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void switch_app_box2d_ring(void *obj, gui_event_t e, void *param)
{
    (void)obj;//not used
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, box2d_ring_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_fruit_ninja(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, fruit_ninja_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_heart_rate(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, heartrate_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_music(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, music_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_activity(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, activity_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_soccer(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, soccer_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_flower(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, flower_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_weather(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, weather_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_butterfly(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, butterfly_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_applist(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, applist_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_disc(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, disc_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_face(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, face_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
    gui_view_set_animate_step(gui_view_get_current(), 1000);
    gui_obj_hidden(GUI_BASE(current_view), true);
}

static void switch_app_prism_thick(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, prism_thick_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_prism_mirror(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, prism3d_mirror_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
    gui_view_set_animate_step(gui_view_get_current(), 1000);
}

static void switch_app_windmill(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, windmill_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_pandkoi(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, pandkoi_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void switch_app_seawater(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, seawater_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void switch_app_firefly(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, firefly_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void switch_app_rainbow_digital(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, rainbow_digital_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void switch_app_koiclock(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, koiclock_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void switch_app_digital_clock(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, digital_clock_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void switch_app_countdown(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, box2d_countdown_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void switch_app_firework(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, fireworks_clock_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_heart_particle(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, heart_particle_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_butterfly_particle(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, butterfly_particle_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void switch_app_butterflys(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, butterflys_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_earth_clock(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, earth_clock_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
    gui_view_set_animate_step(gui_view_get_current(), 1000);
}

static void switch_app_labubu_digital(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, labubu_digital_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
    gui_view_set_animate_step(gui_view_get_current(), 1000);
}
static void switch_app_flower_clock(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, flower_clock_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
    gui_view_set_animate_step(gui_view_get_current(), 1000);
}

static void switch_app_rainbow_analog(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, rainbow_analog_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
    gui_view_set_animate_step(gui_view_get_current(), 1000);
}

static void switch_menu_style_cb(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    if (gui_view_get_next() != NULL) { return; }

    gui_obj_t *parent = GUI_BASE(current_view)->parent;
    gui_obj_tree_free(GUI_BASE(current_view));
    clear_menu(current_view);

    menu_style = (menu_style + 1) % 2;
    gui_view_create(parent, &descriptor, 0, 0, 0, 0);
}

static void list_timer_cb(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    list_offset_his = list->offset;
}

static void clear_menu(gui_view_t *view)
{
    (void)view;
    if (design_p)
    {
        gui_free(design_p->click_cb);
        gui_free(design_p->img_data_array);
        gui_free(design_p);
        design_p = NULL;
    }
}

static void note_design(gui_obj_t *obj, void *p)
{
    note_design_param_t *design_p = (note_design_param_t *)p;
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint16_t index = note->index;
    if (index < sizeof(text_array) / sizeof(text_array[0]) - 1)
    {
        gui_canvas_rect_create(GUI_BASE(note), "note", 0, 0, 0, 100, color_t[index % 2]);
        gui_img_t *img = gui_img_create_from_mem(note, 0, design_p->img_data_array[index], 30, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        char *text = (char *)text_array[index];
        int font_size = 32;
        gui_text_t *t = gui_text_create(note, "txt", 180, 27, gui_get_screen_width(), font_size);
        // gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 0, 0), strlen(text), font_size);
        // gui_text_type_set(t, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, gui_rgb(0, 0, 0), strlen(text), font_size);
        gui_text_type_set(t, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
        gui_obj_add_event_cb(note, (gui_event_cb_t)design_p->click_cb[index], GUI_EVENT_TOUCH_CLICKED,
                             NULL);
    }
    else
    {
        // gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(note), 0, 72, 0, 262, 56, 30,
        //                                                                gui_rgb(100, 100, 100));
        // gui_img_t *img = gui_img_create_from_mem(canvas, 0, APP_MENU_ICON_BIN, 33, 10, 0, 0);
        // gui_img_set_mode(img, IMG_SRC_OVER_MODE);

        // gui_text_t *text = gui_text_create(canvas, 0, 90, 10, 0, 0);
        // gui_text_set(text, (void *)"CELLULAR", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, 8, 32);
        // gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        // gui_text_rendermode_set(text, 2);

        gui_img_t *img = gui_img_create_from_mem(note, 0, CELLULAR_MENU_CARD_BIN, 87, 0, 0, 0);
        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_menu_style_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
}

static void cellular_timer_cb(void)
{
    cellular_offset_x = menu_cellular->hor_offset;
    cellular_offset_y = menu_cellular->ver_offset;
}

static void canvas_timer_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    matrix_translate((float)menu_cellular->hor_offset, (float)menu_cellular->ver_offset, obj->matrix);

    cellular_timer_cb();
}

static void app_menu_design(gui_view_t *view)
{
    gui_win_t *win = gui_win_create(view, "win_app_menu", 0, 0, 0, 0);
    gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    extern bool return_to_watchface_flag;
    return_to_watchface_flag = false;

    if (menu_style == 0)
    {
        void *click_cb[] =
        {
            switch_app_heart_rate,
            switch_app_fruit_ninja,
            switch_app_box2d_ring,
            switch_app_activity,
            switch_app_soccer,
            switch_app_flower,
            switch_app_weather,
            switch_app_butterfly,
            switch_app_applist,
            switch_app_disc,
            switch_app_face,
            switch_app_prism_thick,
            switch_app_prism_mirror,
            switch_app_windmill,
            switch_app_pandkoi,
            switch_app_seawater,
            switch_app_firefly,
            switch_app_rainbow_digital,
            switch_app_koiclock,
            switch_app_digital_clock,
            switch_app_countdown,
            switch_app_firework,
            switch_app_heart_particle,
            switch_app_butterfly_particle,
            switch_app_butterflys,
            switch_app_earth_clock,
            switch_app_labubu_digital,
            switch_app_flower_clock,
            switch_app_rainbow_analog,
            switch_app_music,

            switch_menu_style_cb, //must last
        };

        uint32_t *img_data_array[] =
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
            PANDKOI_ICON_BIN,
            SEAWATER_ICON_BIN,
            FIREFLY_ICON_BIN,
            RAINBOW_DIGITAL_ICON_BIN,
            KOI_CLOCK_ICON_BIN,
            DIGITAL_CLOCK_ICON_BIN,
            COUNT_DOWN_TIME_ICON_BIN,
            FIREWORKS_CLOCK_ICON_BIN,
            HEART_PARTICLE_ICON_BIN,
            BUTTERFLY_PARTICLE_ICON_BIN,
            BUTTERFLYS_ICON_BIN,
            EARTH_DIGITAL_ICON_BIN,
            LABUBU_DIGITAL_ICON_BIN,
            FLOWER_CLOCK_ICON_BIN,
            RAINBOW_ANALOG_ICON_BIN,
            MUSIC_ICON_BIN,

            CELLULAR_MENU_CARD_BIN, //must last
        };
        int array_size = sizeof(text_array) / sizeof(text_array[0]);

        design_p = gui_malloc(sizeof(note_design_param_t));
        void **func_cb = gui_malloc(array_size * sizeof(void *));
        memcpy(func_cb, click_cb, array_size * sizeof(void *));
        uint32_t **data_array = gui_malloc(array_size * sizeof(uint32_t *));
        memcpy(data_array, img_data_array, array_size * sizeof(void *));
        design_p->click_cb = func_cb;
        design_p->img_data_array = data_array;

        color_t[0] = APP_COLOR_SILVER_OPACITY(150);
        color_t[1] = APP_COLOR_WHITE_OPACITY;

        int length = 100;
        uint8_t space = 5;
        gui_list_t *list = gui_list_create(win, "list", 0, 0, 0, 0, length, space,
                                           VERTICAL, note_design, design_p, 1);
        gui_list_set_style(list, LIST_HELIX);
        gui_list_set_note_num(list, array_size);
        gui_list_set_offset(list, list_offset_his);
        gui_list_set_bar_color(list, APP_COLOR_SILVER);
        gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);
    }
    else
    {
        uint32_t *cellilar_img_data_array[] =
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
            PANDKOI_ICON_BIN,
            SEAWATER_ICON_BIN,
            FIREFLY_ICON_BIN,
            RAINBOW_DIGITAL_ICON_BIN,
            KOI_CLOCK_ICON_BIN,
            DIGITAL_CLOCK_ICON_BIN,
            COUNT_DOWN_TIME_ICON_BIN,
            FIREWORKS_CLOCK_ICON_BIN,
            HEART_PARTICLE_ICON_BIN,
            BUTTERFLY_PARTICLE_ICON_BIN,
            BUTTERFLYS_ICON_BIN,
            EARTH_DIGITAL_ICON_BIN,
            LABUBU_DIGITAL_ICON_BIN,
            FLOWER_CLOCK_ICON_BIN,
            RAINBOW_ANALOG_ICON_BIN,
            MUSIC_ICON_BIN,

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
            PANDKOI_ICON_BIN,
            SEAWATER_ICON_BIN,
            FIREFLY_ICON_BIN,
            RAINBOW_DIGITAL_ICON_BIN,
            KOI_CLOCK_ICON_BIN,
            DIGITAL_CLOCK_ICON_BIN,
            COUNT_DOWN_TIME_ICON_BIN,
            FIREWORKS_CLOCK_ICON_BIN,
            HEART_PARTICLE_ICON_BIN,
            BUTTERFLY_PARTICLE_ICON_BIN,
            BUTTERFLYS_ICON_BIN,
            EARTH_DIGITAL_ICON_BIN,
            LABUBU_DIGITAL_ICON_BIN,
            FLOWER_CLOCK_ICON_BIN,
            RAINBOW_ANALOG_ICON_BIN,
            MUSIC_ICON_BIN,
        };
        int array_size = sizeof(cellilar_img_data_array) / sizeof(cellilar_img_data_array[0]);
        int16_t icon_size = 100;
        gui_canvas_rect_create(GUI_BASE(win), "background", 0, 0,
                               SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgba(76, 76, 76, 255));
        gui_menu_cellular_t *menu = gui_menu_cellular_create(win, icon_size, cellilar_img_data_array,
                                                             array_size);
        menu_cellular = menu;
        gui_menu_cellular_offset(menu, cellular_offset_x, cellular_offset_y);
        {
            struct gui_menu_cellular_gesture_parameter gesture_parameter_array[] =
            {
                {switch_app_heart_rate, NULL}, {switch_app_fruit_ninja, NULL}, {switch_app_box2d_ring, NULL}, {switch_app_activity, NULL},
                {switch_app_soccer, NULL}, {switch_app_flower, NULL}, {switch_app_weather, NULL}, {switch_app_butterfly, NULL},
                {switch_app_applist, NULL}, {switch_app_disc, NULL}, {switch_app_face, NULL}, {switch_app_prism_thick, NULL},
                {switch_app_prism_mirror, NULL}, {switch_app_windmill, NULL}, {switch_app_pandkoi, NULL}, {switch_app_seawater, NULL},
                {switch_app_firefly, NULL}, {switch_app_rainbow_digital, NULL}, {switch_app_koiclock, NULL}, {switch_app_digital_clock, NULL},
                {switch_app_countdown, NULL}, {switch_app_firework, NULL}, {switch_app_heart_particle, NULL}, {switch_app_butterfly_particle, NULL},
                {switch_app_butterflys, NULL}, {switch_app_earth_clock, NULL}, {switch_app_labubu_digital, NULL}, {switch_app_flower_clock, NULL},
                {switch_app_rainbow_analog, NULL}, {switch_app_music, NULL},

                {switch_app_heart_rate, NULL}, {switch_app_fruit_ninja, NULL}, {switch_app_box2d_ring, NULL}, {switch_app_activity, NULL},
                {switch_app_soccer, NULL}, {switch_app_flower, NULL}, {switch_app_weather, NULL}, {switch_app_butterfly, NULL},
                {switch_app_applist, NULL}, {switch_app_disc, NULL}, {switch_app_face, NULL}, {switch_app_prism_thick, NULL},
                {switch_app_prism_mirror, NULL}, {switch_app_windmill, NULL}, {switch_app_pandkoi, NULL}, {switch_app_seawater, NULL},
                {switch_app_firefly, NULL}, {switch_app_rainbow_digital, NULL}, {switch_app_koiclock, NULL}, {switch_app_digital_clock, NULL},
                {switch_app_countdown, NULL}, {switch_app_firework, NULL}, {switch_app_heart_particle, NULL}, {switch_app_butterfly_particle, NULL},
                {switch_app_butterflys, NULL}, {switch_app_earth_clock, NULL}, {switch_app_labubu_digital, NULL}, {switch_app_flower_clock, NULL},
                {switch_app_rainbow_analog, NULL}, {switch_app_music, NULL},
            };
            gui_menu_cellular_on_click(menu, gesture_parameter_array,
                                       sizeof(gesture_parameter_array) / sizeof(gesture_parameter_array[0]));
        }
        {
            int h = array_size / 7 * 100 * 2 + (array_size % 7) / 3 * 100 + 100;
            gui_win_t *win_canvas = gui_win_create(view, "win_canvas", 125, h, 200, 100);
            // gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(win_canvas), 0, 0, 0, 170,
            //                                                                56, 30, gui_rgb(100, 100, 100));
            // gui_img_t *img = gui_img_create_from_mem(canvas, 0, APP_MENU_ICON_BIN, 33, 10, 0, 0);
            // gui_img_set_mode(img, IMG_SRC_OVER_MODE);

            // gui_text_t *text = gui_text_create(canvas, 0, 90, 10, 0, 0);
            // gui_text_set(text, (void *)"LIST", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, 4, 32);
            // gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
            // gui_text_rendermode_set(text, 2);
            gui_img_t *img = gui_img_create_from_mem(win_canvas, 0, LIST_MENU_CARD_BIN, 0, 0, 0, 0);
            gui_obj_create_timer(GUI_BASE(win_canvas), 10, true, canvas_timer_cb);
            gui_obj_add_event_cb(img, (gui_event_cb_t)switch_menu_style_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
    }
}