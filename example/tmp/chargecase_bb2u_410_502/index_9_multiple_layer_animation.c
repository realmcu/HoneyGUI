#include "gui_view_macro.h"
#include "gui_img.h"
static  uint32_t watchface_array[] =
{

    WATCHFACE_FRAME_0000_BIN,
    WATCHFACE_FRAME_0005_BIN,
    WATCHFACE_FRAME_0008_BIN,
    WATCHFACE_FRAME_0009_BIN,
    WATCHFACE_FRAME_0014_BIN,
    WATCHFACE_FRAME_0018_BIN,
    WATCHFACE_FRAME_0019_BIN,
    WATCHFACE_FRAME_0024_BIN,
    WATCHFACE_FRAME_0027_BIN,
    WATCHFACE_FRAME_0028_BIN,
    WATCHFACE_FRAME_0033_BIN,
    WATCHFACE_FRAME_0037_BIN,
    WATCHFACE_FRAME_0038_BIN,
    WATCHFACE_FRAME_0043_BIN,
    WATCHFACE_FRAME_0045_BIN,
    WATCHFACE_FRAME_0047_BIN,
    WATCHFACE_FRAME_0052_BIN,
    WATCHFACE_FRAME_0055_BIN,
    WATCHFACE_FRAME_0057_BIN,
    WATCHFACE_FRAME_0062_BIN,
    WATCHFACE_FRAME_0064_BIN,
    WATCHFACE_FRAME_0065_BIN,
    WATCHFACE_FRAME_0070_BIN,
    WATCHFACE_FRAME_0074_BIN,
    WATCHFACE_FRAME_0075_BIN,
    WATCHFACE_FRAME_0080_BIN,
    WATCHFACE_FRAME_0083_BIN,
    WATCHFACE_FRAME_0084_BIN,
    WATCHFACE_FRAME_0089_BIN,
    WATCHFACE_FRAME_0093_BIN,
    WATCHFACE_FRAME_0094_BIN,
    WATCHFACE_FRAME_0099_BIN,
    WATCHFACE_FRAME_0102_BIN,
    WATCHFACE_FRAME_0103_BIN,
    WATCHFACE_FRAME_0108_BIN,
    WATCHFACE_FRAME_0112_BIN,
    WATCHFACE_FRAME_0113_BIN,
    WATCHFACE_FRAME_0118_BIN,
};
static  uint32_t a1_array[] =
{
    A1FRAME_000_BIN,
    A1FRAME_003_BIN,
    A1FRAME_006_BIN,
    A1FRAME_009_BIN,
    A1FRAME_012_BIN,
    A1FRAME_015_BIN,
    A1FRAME_018_BIN,
    A1FRAME_021_BIN,
    A1FRAME_024_BIN,
    A1FRAME_027_BIN,
    A1FRAME_030_BIN,
    A1FRAME_033_BIN,
    A1FRAME_036_BIN,
    A1FRAME_039_BIN,
    A1FRAME_042_BIN,
    A1FRAME_045_BIN,
    A1FRAME_048_BIN,
    A1FRAME_051_BIN,
    A1FRAME_054_BIN,
    A1FRAME_057_BIN,
    A1FRAME_059_BIN,
};
static  uint32_t a2_array[] =
{
    A2FUEL_000_BIN,
    A2FUEL_005_BIN,
    A2FUEL_010_BIN,
    A2FUEL_015_BIN,
    A2FUEL_020_BIN,
    A2FUEL_025_BIN,
    A2FUEL_030_BIN,
    A2FUEL_035_BIN,
    A2FUEL_040_BIN,
    A2FUEL_045_BIN,
    A2FUEL_050_BIN,
    A2FUEL_055_BIN,
    A2FUEL_060_BIN,
    A2FUEL_065_BIN,
    A2FUEL_070_BIN,
    A2FUEL_075_BIN,
    A2FUEL_080_BIN,
    A2FUEL_085_BIN,
    A2FUEL_090_BIN,
    A2FUEL_095_BIN,
    A2FUEL_100_BIN,
};
static  uint32_t a3_array[] =
{
    RPM_000_BIN,
    RPM_003_BIN,
    RPM_006_BIN,
    RPM_009_BIN,
    RPM_012_BIN,
    RPM_015_BIN,
    RPM_018_BIN,
    RPM_021_BIN,
    RPM_024_BIN,
    RPM_027_BIN,
    RPM_030_BIN,
    RPM_033_BIN,
    RPM_036_BIN,
    RPM_039_BIN,
    RPM_042_BIN,
    RPM_045_BIN,
    RPM_048_BIN,
    RPM_051_BIN,
    RPM_054_BIN,
    RPM_057_BIN,
    RPM_060_BIN,
};
static  uint32_t a4_array[] =
{
    SPEED_000_BIN,
    SPEED_003_BIN,
    SPEED_006_BIN,
    SPEED_009_BIN,
    SPEED_012_BIN,
    SPEED_015_BIN,
    SPEED_018_BIN,
    SPEED_021_BIN,
    SPEED_024_BIN,
    SPEED_027_BIN,
    SPEED_030_BIN,
    SPEED_033_BIN,
    SPEED_036_BIN,
    SPEED_039_BIN,
    SPEED_042_BIN,
    SPEED_045_BIN,
    SPEED_048_BIN,
    SPEED_051_BIN,
    SPEED_054_BIN,
    SPEED_057_BIN,
    SPEED_060_BIN,
};

#define WATCHFACE_DUR_MS (5000)
static void switch_widget_play_watchface(void *p)
{
    GUI_UNUSED(watchface_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(watchface_array);
    int index = (array_size - 1) * dur / WATCHFACE_DUR_MS;
    index = index % array_size;


    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(watchface_array[index]));
        //  gui_img_refresh_size(img);
        gui_log("switch index:%d\n", index);


    }
}
static void switch_widget_play_watchface2(void *p)
{
    GUI_UNUSED(a1_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(a1_array);
    int index = (array_size - 1) * dur / WATCHFACE_DUR_MS;
    index = index % array_size;


    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(a1_array[index]));
        //  gui_img_refresh_size(img);
        gui_log("switch index:%d\n", index);


    }
}
static void switch_widget_play_watchface3(void *p)
{
    GUI_UNUSED(a2_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(a2_array);
    int index = (array_size - 1) * dur / WATCHFACE_DUR_MS;
    index = index % array_size;


    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(a2_array[index]));
        //  gui_img_refresh_size(img);
        gui_log("switch index:%d\n", index);


    }
}
static void switch_widget_play_watchface4(void *p)
{
    GUI_UNUSED(a3_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(a3_array);
    int index = (array_size - 1) * dur / WATCHFACE_DUR_MS;
    index = index % array_size;


    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(a3_array[index]));
        //  gui_img_refresh_size(img);
        gui_log("switch index:%d\n", index);


    }
}
static void switch_widget_play_watchface5(void *p)
{
    GUI_UNUSED(a4_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(a4_array);
    int index = (array_size - 1) * dur / WATCHFACE_DUR_MS;
    index = index % array_size;


    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(a4_array[index]));
        //  gui_img_refresh_size(img);
        gui_log("switch index:%d\n", index);


    }
}
static void multiple_layer_animation_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("multiple_layer_animation_view switch in\n");
    gui_view_switch_on_event(view, "radial_gradient_view",
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, "low_latency_view",
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(WATCHFACE_FRAME_0000_BIN), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_BYPASS_MODE);
        gui_obj_create_timer((void *)img, 10, true, switch_widget_play_watchface);

    }
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(a1_array[0]), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_create_timer((void *)img, 10, true, switch_widget_play_watchface2);

    }
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(a2_array[0]), 100, 100, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_create_timer((void *)img, 10, true, switch_widget_play_watchface3);

    }
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(a3_array[0]), 200, 200, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_create_timer((void *)img, 10, true, switch_widget_play_watchface4);

    }
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(a4_array[0]), 100, 300, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_create_timer((void *)img, 10, true, switch_widget_play_watchface5);

    }
}

static void multiple_layer_animation_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("multiple_layer_animation_view switch out\n");

}

GUI_VIEW_INSTANCE("multiple_layer_animation_view", 1, multiple_layer_animation_switch_in,
                  multiple_layer_animation_switch_out);

