#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"
#include "app_hongkong.h"
#include "gui_view.h"
#include "gui_list.h"

#define CURRENT_VIEW_NAME "weather_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
void weather_app(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = weather_app,
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
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void return_to_menu()
{
    gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }


static gui_list_t *weather_list;
static uint8_t current_list_index = 0;
static uint8_t last_list_index = 0;

static void update_panel_animation(void *param)
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();
    gui_list_note_t *tab = (gui_list_note_t *)param;

    int32_t diff = (weather_list->base.w / 2 + weather_list->base.x) - (tab->base.x +
                                                                        weather_list->note_length / 2);
    float scale = fabs((abs(diff) - dc->screen_width / 2.0f)) / (dc->screen_width / 2.0f);

    float panel_below_driftY = (1.0f - scale) * 50.0f;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(panel_img, "panel1", tab);
    gui_img_translate((gui_img_t *)panel_img, 0, panel_below_driftY);

}

// clear win child
static void clear_weather_icons(gui_win_t *win)
{
    if (win != NULL)
    {
        gui_obj_child_free((gui_obj_t *)win);
    }

}

// sunny
#define NUM_LIGHTS_OR_THUNDER 3
static gui_img_t *lights_or_thunder[NUM_LIGHTS_OR_THUNDER] = {NULL};
static int current_light_index = 0;
static void update_light_animation()
{
    for (int i = 0; i < NUM_LIGHTS_OR_THUNDER; i++)
    {
        if (i == current_light_index && lights_or_thunder[i]->opacity_value <= 250)
        {
            lights_or_thunder[i]->opacity_value += 5;
        }
        else if (i != current_light_index && lights_or_thunder[i]->opacity_value >= 5)
        {
            lights_or_thunder[i]->opacity_value -= 5;
        }
    }

    if (lights_or_thunder[current_light_index]->opacity_value == 255)
    {
        current_light_index = (current_light_index + 1) % NUM_LIGHTS_OR_THUNDER;
    }
}

// rainy
#define NUM_DYNAMIC_ICON 30
typedef struct
{
    float driftX;    // Horizontal drift
    float driftY;    // Vertical drift
    float scale;     // Scale factor, size is 32*scale
    gui_img_t *img;  // image object
} Dynamic_Icon;
static Dynamic_Icon dynamic[NUM_DYNAMIC_ICON];
static void update_raindrop_animation()
{
    gui_dispdev_t *dc = gui_get_dc();

    for (int i = 0; i < NUM_DYNAMIC_ICON; i++)
    {
        dynamic[i].driftY += 2.0f;
        if (dynamic[i].driftY > 0)
        {
            dynamic[i].driftX += 0.4f;
        }

        if (dynamic[i].driftY > dc->screen_height)
        {
            dynamic[i].driftX =  70 + rand() % 256;
            dynamic[i].driftY = -50;
        }

        gui_img_translate(dynamic[i].img, dynamic[i].driftX, dynamic[i].driftY);
    }
}

// snowy
static void update_snow_animation()
{
    for (int i = 0; i < NUM_DYNAMIC_ICON; i++)
    {
        dynamic[i].driftY ++;
        if (dynamic[i].driftY > 0)
        {
            dynamic[i].driftX ++;
        }

        if (dynamic[i].driftY > 300)
        {
            dynamic[i].driftX =  70 + rand() % 256;
            dynamic[i].driftY = -rand() % 300;
        }

        gui_img_translate(dynamic[i].img, dynamic[i].driftX, dynamic[i].driftY);
    }
}

// stormy
static void update_thunder_animation()
{
    for (int i = 0; i < NUM_LIGHTS_OR_THUNDER; i++)
    {
        if (i == current_light_index && lights_or_thunder[i]->opacity_value <= 250)
        {
            lights_or_thunder[i]->opacity_value += 5;
        }
        else if (i != current_light_index && lights_or_thunder[i]->opacity_value >= 5)
        {
            lights_or_thunder[i]->opacity_value -= 5;
        }
    }

    if (lights_or_thunder[current_light_index]->opacity_value == 255)
    {
        current_light_index = (current_light_index + 1) % NUM_LIGHTS_OR_THUNDER;
    }
}

static float leaf_rot_angle = 0.0f;
static void update_leaf_animation()
{
    gui_dispdev_t *dc = gui_get_dc();
    leaf_rot_angle += 5.0f;

    for (int i = 0; i < NUM_DYNAMIC_ICON; i++)
    {
        dynamic[i].driftX += 10.0f;
        if (dynamic[i].driftX > 0)
        {
            dynamic[i].img->opacity_value -= 5;
        }

        if (dynamic[i].driftX > dc->screen_width)
        {
            dynamic[i].driftX =  -rand() % 300;
            dynamic[i].driftY = 100 + rand() % 300;
            dynamic[i].img->opacity_value = 255;
        }

        gui_img_translate(dynamic[i].img, dynamic[i].driftX, dynamic[i].driftY);
        gui_img_rotation(dynamic[i].img, leaf_rot_angle);
    }
}

static void update_weather_animation(void *param)
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();
    gui_win_t *win = (gui_win_t *)param;


    if (weather_list->offset > -145) // center offset: -55
    {
        current_list_index = 1; //sunny tab
    }
    else if (weather_list->offset > -325) // center offset: -235
    {
        current_list_index = 2; //cloudy tab
    }
    else if (weather_list->offset > -505) // center offset: -415
    {
        current_list_index = 3; //rainy tab
    }
    else if (weather_list->offset > -685) // center offset: -595
    {
        current_list_index = 4; //snowy tab
    }
    else if (weather_list->offset > -865) // center offset: -775
    {
        current_list_index = 5; //stormy tab
    }
    else if (weather_list->offset > -1045) // center offset: -955
    {
        current_list_index = 6; //windy tab
    }


    if (current_list_index != 0 && current_list_index != last_list_index)
    {
        clear_weather_icons(win);

        if (current_list_index == 1) //sunny tab
        {
            lights_or_thunder[0] = gui_img_create_from_mem(win, 0, LIGHT0_BIN, 0, 0, 0, 0);
            gui_img_scale(lights_or_thunder[0], 1.5f, 1.5f);
            gui_img_translate(lights_or_thunder[0], 270, 0);
            gui_img_rotation(lights_or_thunder[0], 45.0f);
            lights_or_thunder[1] = gui_img_create_from_mem(win, 0, LIGHT1_BIN, 0, 0, 0, 0);
            gui_img_scale(lights_or_thunder[1], 1.5f, 2.0f);
            gui_img_translate(lights_or_thunder[1], 250, 30);
            gui_img_rotation(lights_or_thunder[1], 90.0f);
            lights_or_thunder[2] = gui_img_create_from_mem(win, 0, LIGHT2_BIN, 0, 0, 0, 0);
            gui_img_scale(lights_or_thunder[2], 1.5f, 3.0f);
            gui_img_translate(lights_or_thunder[2], 220, 60);
            gui_img_rotation(lights_or_thunder[2], 135.0f);

            gui_obj_create_timer(GUI_BASE(lights_or_thunder[0]), 10, true, update_light_animation);

            gui_img_t *sun = gui_img_create_from_mem(win, 0, SUN0_BIN, 0, 0, 0, 0);
            gui_img_scale(sun, 2.0f, 2.0f);
            gui_img_translate(sun, 77, -128);

        }
        else if (current_list_index == 2) // cloudy tab
        {
            gui_img_t *sun = gui_img_create_from_mem(win, 0, SUN0_BIN, 0, 0, 0, 0);
            gui_img_scale(sun, 2.0f, 2.0f);
            gui_img_translate(sun, 77, -128);
            gui_img_t *cloud = gui_img_create_from_mem(win, 0, CLOUD0_BIN, 0, 0, 0, 0);
            gui_img_scale(cloud, 2.0f, 2.0f);
            gui_img_translate(cloud, 77, -60);
        }
        else if (current_list_index == 3) // rainy tab
        {
            for (int i = 0; i < NUM_DYNAMIC_ICON; i++)
            {
                dynamic[i].driftX = 70 + rand() % 256;
                dynamic[i].driftY = -(i % 50) * 15;
                dynamic[i].scale = (rand() % 71) / 100.0f + 0.2f;

                dynamic[i].img = gui_img_create_from_mem(win, "rain", RAIN0_BIN, 0, 0, 0, 0);
                gui_img_scale(dynamic[i].img, dynamic[i].scale, dynamic[i].scale);
                gui_img_translate(dynamic[i].img, dynamic[i].driftX, dynamic[i].driftY);
            }

            gui_img_t *cloud = gui_img_create_from_mem(win, 0, CLOUD0_BIN, 0, 0, 0, 0);
            gui_img_scale(cloud, 2.0f, 2.0f);
            gui_img_translate(cloud, 77, -60);

            gui_obj_create_timer(GUI_BASE(cloud), 10, true, update_raindrop_animation);

        }
        else if (current_list_index == 4) // snowy tab
        {
            for (int i = 0; i < NUM_DYNAMIC_ICON; i++)
            {
                dynamic[i].driftX = 70 + rand() % 256;
                dynamic[i].driftY = -rand() % 300;
                dynamic[i].scale = (rand() % 31) / 100.0f + 0.1f;

                dynamic[i].img = gui_img_create_from_mem(win, "snow", SNOW0_BIN, 0, 0, 0, 0);
                gui_img_scale(dynamic[i].img, dynamic[i].scale, dynamic[i].scale);
                gui_img_translate(dynamic[i].img, dynamic[i].driftX, dynamic[i].driftY);
            }

            gui_img_t *cloud = gui_img_create_from_mem(win, 0, CLOUD0_BIN, 0, 0, 0, 0);
            gui_img_scale(cloud, 2.0f, 2.0f);
            gui_img_translate(cloud, 77, -60);

            gui_obj_create_timer(GUI_BASE(cloud), 10, true, update_snow_animation);

        }
        else if (current_list_index == 5) // stormy tab
        {
            lights_or_thunder[0] = gui_img_create_from_mem(win, 0, THUNDER00_BIN, 0, 0, 0, 0);
            gui_img_translate(lights_or_thunder[0], 173, 40);
            lights_or_thunder[1] = gui_img_create_from_mem(win, 0, THUNDER01_BIN, 0, 0, 0, 0);
            gui_img_translate(lights_or_thunder[1], 120, 40);
            gui_img_scale(lights_or_thunder[1], 1.0f, 1.2f);
            lights_or_thunder[2] = gui_img_create_from_mem(win, 0, THUNDER02_BIN, 0, 0, 0, 0);
            gui_img_translate(lights_or_thunder[2], 230, 40);
            gui_img_scale(lights_or_thunder[2], 1.0f, 1.5f);

            gui_obj_create_timer(GUI_BASE(lights_or_thunder[0]), 10, true, update_thunder_animation);

            gui_img_t *cloud = gui_img_create_from_mem(win, 0, CLOUD0_BIN, 0, 0, 0, 0);
            gui_img_scale(cloud, 2.0f, 2.0f);
            gui_img_translate(cloud, 77, -60);

        }
        else if (current_list_index == 6) // windy tab
        {
            for (int i = 0; i < NUM_DYNAMIC_ICON; i++)
            {
                dynamic[i].driftX = -rand() % 300;
                dynamic[i].driftY = 100 + rand() % 300;
                dynamic[i].scale = (rand() % 71) / 100.0f + 0.2f;

                int random_image_index = rand() % 3;
                void *images[3] = {(void *)WINDLEAF00_BIN, (void *)WINDLEAF01_BIN, (void *)WINDLEAF02_BIN};

                dynamic[i].img = gui_img_create_from_mem(win, "leaf", images[random_image_index], 0, 0,
                                                         0, 0);
                gui_img_set_focus(dynamic[i].img, 32, 32);
                gui_img_translate(dynamic[i].img, dynamic[i].driftX, dynamic[i].driftY);
                gui_img_scale(dynamic[i].img, dynamic[i].scale, dynamic[i].scale);

            }
            gui_obj_create_timer(GUI_BASE(dynamic[0].img), 10, true, update_leaf_animation);
        }


        last_list_index = current_list_index;
    }
}

static void add_panel(gui_list_note_t *tab)
{
    gui_img_create_from_mem(tab, "panel0", PANEL00_BIN, 0, 44, 0, 0);

    gui_img_t *panel_down = gui_img_create_from_mem(tab, "panel1", PANEL00_BIN, 0, 200, 0, 0);
    gui_img_set_focus(panel_down, 0, 400);
    gui_img_scale(panel_down, 1.0f, -1.0f);
    gui_img_set_opacity(panel_down, 125);

}

void weather_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    int length = 160;
    uint8_t space = 20;

    weather_list = gui_list_create(obj, "list", 0, 0, 0, 0, length, space, HORIZONTAL);
    gui_list_set_offset(weather_list, -55);

    gui_list_add_note(weather_list, false);
    gui_list_note_t *sunny_tab = gui_list_add_note(weather_list, false);
    add_panel(sunny_tab);
    gui_list_note_t *cloudy_tab = gui_list_add_note(weather_list, false);
    add_panel(cloudy_tab);
    gui_list_note_t *rainy_tab = gui_list_add_note(weather_list, false);
    add_panel(rainy_tab);
    gui_list_note_t *snowy_tab = gui_list_add_note(weather_list, false);
    add_panel(snowy_tab);
    gui_list_note_t *stormy_tab = gui_list_add_note(weather_list, false);
    add_panel(stormy_tab);
    gui_list_note_t *windy_tab = gui_list_add_note(weather_list, false);
    add_panel(windy_tab);

    gui_list_add_note(weather_list, false);
    gui_list_set_style(weather_list, LIST_ZOOM);

    gui_obj_create_timer(GUI_BASE(sunny_tab), 10, true, update_panel_animation);
    gui_obj_create_timer(GUI_BASE(cloudy_tab), 10, true, update_panel_animation);
    gui_obj_create_timer(GUI_BASE(rainy_tab), 10, true, update_panel_animation);
    gui_obj_create_timer(GUI_BASE(snowy_tab), 10, true, update_panel_animation);
    gui_obj_create_timer(GUI_BASE(stormy_tab), 10, true, update_panel_animation);
    gui_obj_create_timer(GUI_BASE(windy_tab), 10, true, update_panel_animation);

    gui_win_t *weather_win = gui_win_create(current_view, "weather_win", 0, 0, 0, 0);
    gui_obj_create_timer(GUI_BASE(weather_win), 10, true, update_weather_animation);

}