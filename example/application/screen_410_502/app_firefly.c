/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_gray.h"
#include "gui_obj.h"
#include "gui_view.h"
#include "root_image/ui_resource.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "firefly_view"

/*============================================================================*
*                             Types
*============================================================================*/
typedef struct
{
    float x;// Current x position of the firefly
    float y;// Current y position of the firefly
    float speed;
    float angle;
    float scale;
    gui_img_t *img;//Gradient image for the firefly
    gui_img_t *fill;//fill image for the firefly
    uint32_t last_change; //last time the firefly changed direction or speed
} Firefly;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_ui_firefly_design(gui_view_t *view);
static void cleanup_resources(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_firefly_design,
    .on_switch_out = cleanup_resources,

    .keep = false,
};

#define MAX_FIREFLY 20
static Firefly fireflys[MAX_FIREFLY];
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
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

// static void return_to_menu()
// {
//     gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
//                            SWITCH_IN_ANIMATION_FADE);
// }

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }



static void update_all_fireflys(void *param)
{
    uint32_t current_time = gui_ms_get();
    gui_dispdev_t *dc = gui_get_dc();
    (void)param;

    for (int i = 0; i < MAX_FIREFLY; i++)
    {
        if (current_time - fireflys[i].last_change > 1000U + xorshift16() % 2000U)
        {
            fireflys[i].angle = (xorshift16() % 360) * M_PI_F / 180.0f;
            fireflys[i].speed = 0.5f + (xorshift16() % 20) / 20.0f;
            fireflys[i].last_change = current_time;
        }

        fireflys[i].x += cosf(fireflys[i].angle) * fireflys[i].speed;
        fireflys[i].y += sinf(fireflys[i].angle) * fireflys[i].speed;

        if (fireflys[i].x < 0) { fireflys[i].x = dc->screen_width; }
        if (fireflys[i].x > dc->screen_width) { fireflys[i].x = 0; }
        if (fireflys[i].y < 0) { fireflys[i].y = dc->screen_height; }
        if (fireflys[i].y > dc->screen_height) { fireflys[i].y = 0; }

        gui_img_translate(fireflys[i].img, fireflys[i].x, fireflys[i].y);
        gui_img_translate(fireflys[i].fill, fireflys[i].x, fireflys[i].y);

        if (xorshift16() % 100 < 5)
        {
            uint8_t opacity = 150 + xorshift16() % 105;
            gui_img_set_opacity(fireflys[i].img, opacity);
            gui_img_set_opacity(fireflys[i].fill, opacity);
        }
    }
}

static void create_firefly(gui_win_t *firefly_win)
{
    void *firefly_img[] =
    {
        FIREFLY_1_BIN, FIREFLY_2_BIN, FIREFLY_3_BIN, FIREFLY_4_BIN, FIREFLY_5_BIN,
        FIREFLY_6_BIN, FIREFLY_7_BIN, FIREFLY_8_BIN, FIREFLY_9_BIN, FIREFLY_10_BIN,
        FIREFLY_11_BIN, FIREFLY_12_BIN, FIREFLY_13_BIN, FIREFLY_14_BIN, FIREFLY_15_BIN,
        FIREFLY_16_BIN, FIREFLY_17_BIN, FIREFLY_18_BIN, FIREFLY_19_BIN, FIREFLY_20_BIN
    };
    void *firefly_fill[] =
    {
        FIREFLY_FILL1_BIN, FIREFLY_FILL2_BIN, FIREFLY_FILL3_BIN, FIREFLY_FILL4_BIN,
        FIREFLY_FILL5_BIN, FIREFLY_FILL6_BIN, FIREFLY_FILL7_BIN, FIREFLY_FILL8_BIN,
        FIREFLY_FILL9_BIN, FIREFLY_FILL10_BIN, FIREFLY_FILL11_BIN, FIREFLY_FILL12_BIN,
        FIREFLY_FILL13_BIN, FIREFLY_FILL14_BIN, FIREFLY_FILL15_BIN, FIREFLY_FILL16_BIN,
        FIREFLY_FILL17_BIN, FIREFLY_FILL18_BIN, FIREFLY_FILL19_BIN, FIREFLY_FILL20_BIN
    };
    uint16_t img_array_size = sizeof(firefly_img) / sizeof(void *);
    gui_dispdev_t *dc = gui_get_dc();
    for (int i = 0; i < MAX_FIREFLY; i++)
    {
        fireflys[i].x = xorshift16() % dc->screen_width;
        fireflys[i].y = xorshift16() % dc->screen_height;
        fireflys[i].speed = 0.5f + (xorshift16() % 20) / 20.0f;
        fireflys[i].angle = (xorshift16() % 360) * M_PI_F / 180.0f;
        fireflys[i].scale = 0.5f + (xorshift16() % 8) / 10.0f;
        fireflys[i].last_change = gui_ms_get();

        fireflys[i].fill = gui_img_create_from_mem(firefly_win, "fire_fly_fill",
                                                   firefly_fill[i % img_array_size], 0, 0, 0, 0);
        fireflys[i].img = gui_img_create_from_mem(firefly_win, "fire_fly",
                                                  firefly_img[i % img_array_size], 0, 0, 0, 0);


        gui_img_scale(fireflys[i].img, fireflys[i].scale, fireflys[i].scale);
        gui_img_scale(fireflys[i].fill, fireflys[i].scale, fireflys[i].scale);

        uint8_t opacity = 200 + xorshift16() % 55;
        gui_img_set_opacity(fireflys[i].img, opacity);
        gui_img_set_opacity(fireflys[i].fill, opacity);
    }
}

static void cleanup_resources(gui_view_t *view)
{
    (void)view;
    for (int i = 0; i < MAX_FIREFLY; i++)
    {
        if (fireflys[i].img)
        {
            fireflys[i].img = NULL;
        }
        if (fireflys[i].fill)
        {
            fireflys[i].fill = NULL;
        }
    }
}

static void app_ui_firefly_design(gui_view_t *view)
{
    srand((uint32_t)gui_ms_get());
    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_dispdev_t *dc = gui_get_dc();
    gui_win_t *firefly_win = gui_win_create(obj, "firefly_win", 0, 0, dc->screen_width,
                                            dc->screen_height);
    create_firefly(firefly_win);
    gui_obj_create_timer(&(firefly_win->base), 17, true, update_all_fireflys);
    gui_obj_start_timer(&(firefly_win->base));
}
