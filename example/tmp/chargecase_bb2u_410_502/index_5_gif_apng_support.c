#include "gui_view_macro.h"

#include "gui_img.h"
#define SWITCH_DUR_MS (100)
#define SWITCH_DUR_MS2 (100*2*2)
#define SWITCH_DUR_MS3 (100*3*2)
#define SWITCH_DUR_MS4 (100*4*2)
#define SWITCH_DUR_MS5 (100*5*2)
#define SWITCH_DUR_MS6 (100*6*2)
#define SWITCH_DUR_MS7 (100*7*2)
#define SWITCH_DUR_MS8 (100*8*2)


#define WATCHFACE_DUR_MS (5000)
#define CARD_MARGIN (20+25)
static void switch_widget_play(void *p);
static void switch_widget_play2(void *p);
static  uint32_t switch_array[] =
{
    FRAME_0024_BIN,
    FRAME_0025_BIN,
    FRAME_0026_BIN,
    FRAME_0027_BIN,
    FRAME_0028_BIN,
    FRAME_0029_BIN,
    FRAME_0030_BIN,
    FRAME_0031_BIN,
    FRAME_0032_BIN,
    FRAME_0033_BIN,
    FRAME_0034_BIN,
    FRAME_0035_BIN,
    FRAME_0036_BIN,
    FRAME_0037_BIN,
    FRAME_0038_BIN,
    FRAME_0040_BIN,
    FRAME_0041_BIN,
    FRAME_0043_BIN,
    FRAME_0045_BIN,
    FRAME_0047_BIN,
    FRAME_0053_BIN,
    FRAME_0061_BIN,
    FRAME_0069_BIN,
    FRAME_0077_BIN,
    FRAME_0085_BIN,
    FRAME_0093_BIN,
    FRAME_0097_BIN,
    FRAME_0103_BIN,
};
#define SWW (120+20)
#define SWH (66+20)
static void switch_widget_play_2(void *p);
static void switch_widget_play_3(void *p);
static void switch_widget_play_4(void *p);
static void switch_widget_play_5(void *p);
static void switch_widget_play_6(void *p);
static void switch_widget_play_7(void *p);
static void switch_widget_play_8(void *p);
static void switch_widget_play_9(void *p);
static void switch_widget_play_10(void *p);
static void *func_array[] =
{
    switch_widget_play,

    switch_widget_play_2,
    switch_widget_play_3,
    switch_widget_play_4,
    switch_widget_play_5,
    switch_widget_play_6,
    switch_widget_play_7,
    switch_widget_play_8,

};
static bool index5_play_flag = 1;
static uint64_t ms_start  = 0;
static void click_switch_widget_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    //gui_obj_t *o = GUI_BASE(obj);
    ms_start  = 0;
    if (index5_play_flag)
    {
        gui_obj_create_timer(obj, 1, 1, switch_widget_play_9);
    }
    else
    {
        gui_obj_create_timer(obj, 1, 1, switch_widget_play_10);
    }




}
static void gif_apng_support_switch_in(gui_view_t *view)
{
    gui_view_switch_on_event(view, gui_view_descriptor_get("blur_glow_effect_view"),
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_switch_on_event(view, gui_view_descriptor_get("video_support_view"),
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    GUI_UNUSED(view);
    gui_log("gif_apng_support_view switch in\n");
    {
        gui_img_t *switch_widget =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(FRAME_0024_BIN), 150, 50, 0, 0);
        GUI_UNUSED(switch_widget);
        gui_obj_add_event_cb(switch_widget, click_switch_widget_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    for (size_t i = 0; i < 8; i++)
    {
        int x = i % 2 * SWW + 75;
        int y = i / 2 * SWH + 150;
        {
            gui_img_t *switch_widget =
                gui_img_create_from_mem(view,
                                        0, (void *)FILE_POINTER(FRAME_0024_BIN), x, y, 0, 0);

            gui_obj_create_timer((void *)switch_widget, 1, true, func_array[i]);
        }
    }


}

static void gif_apng_support_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("gif_apng_support_view switch out\n");
}

GUI_VIEW_INSTANCE("gif_apng_support_view", 1, gif_apng_support_switch_in,
                  gif_apng_support_switch_out);
static void switch_widget_play(void *p)
{
    if (!index5_play_flag)
    {
        return;
    }

    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / SWITCH_DUR_MS;

    if (index >= array_size || index < 0 || dur >= SWITCH_DUR_MS)
    {
        gui_obj_start_timer(obj);
        ms_start = ms_cur;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);



    }
}
static void switch_widget_play_2(void *p)
{
    if (!index5_play_flag)
    {
        return;
    }
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / SWITCH_DUR_MS2;

    if (index >= array_size || index < 0 || dur >= SWITCH_DUR_MS2)
    {
        gui_obj_start_timer(obj);
        ms_start = ms_cur;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);



    }
}
static void switch_widget_play_3(void *p)
{
    if (!index5_play_flag)
    {
        return;
    }
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / SWITCH_DUR_MS3;

    if (index >= array_size || index < 0 || dur >= SWITCH_DUR_MS3)
    {
        gui_obj_start_timer(obj);
        ms_start = ms_cur;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);



    }
}
static void switch_widget_play_4(void *p)
{
    if (!index5_play_flag)
    {
        return;
    }
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / SWITCH_DUR_MS4;

    if (index >= array_size || index < 0 || dur >= SWITCH_DUR_MS4)
    {
        gui_obj_start_timer(obj);
        ms_start = ms_cur;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);



    }
}
static void switch_widget_play_5(void *p)
{
    if (!index5_play_flag)
    {
        return;
    }
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / SWITCH_DUR_MS5;

    if (index >= array_size || index < 0 || dur >= SWITCH_DUR_MS5)
    {
        gui_obj_start_timer(obj);
        ms_start = ms_cur;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);



    }
}
static void switch_widget_play_6(void *p)
{
    if (!index5_play_flag)
    {
        return;
    }
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / SWITCH_DUR_MS6;

    if (index >= array_size || index < 0 || dur >= SWITCH_DUR_MS6)
    {
        gui_obj_start_timer(obj);
        ms_start = ms_cur;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);



    }
}
static void switch_widget_play_7(void *p)
{
    if (!index5_play_flag)
    {
        return;
    }
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / SWITCH_DUR_MS7;

    if (index >= array_size || index < 0 || dur >= SWITCH_DUR_MS7)
    {
        gui_obj_start_timer(obj);
        ms_start = ms_cur;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);



    }
}
static void switch_widget_play_8(void *p)
{
    if (!index5_play_flag)
    {
        return;
    }
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / SWITCH_DUR_MS8;

    if (index >= array_size || index < 0 || dur >= SWITCH_DUR_MS8)
    {
        gui_obj_start_timer(obj);
        ms_start = ms_cur;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);



    }
}
static void switch_widget_play_9(void *p)
{
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();

    if (ms_start == 0)
    {
        ms_start = ms_cur;
    }

    uint64_t dur = ms_cur - ms_start;
    gui_log("dui:%d\n", dur);
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / 600;

    if (index >= array_size || index < 0 || dur >= 600)
    {

        gui_obj_delete_timer(p);
        index5_play_flag = 0;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);



    }
}
static void switch_widget_play_10(void *p)
{
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();

    if (ms_start == 0)
    {
        ms_start = ms_cur;
    }

    uint64_t dur = ms_cur - ms_start;
    gui_log("dui:%d\n", dur);
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / 600;

    if (index >= array_size || index < 0 || dur >= 600)
    {

        gui_obj_delete_timer(p);
        index5_play_flag = 1;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[array_size - 1 - index]));
        gui_img_refresh_size(img);



    }
}
static void switch_widget_play2(void *p)
{
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / (SWITCH_DUR_MS / 2);

    if (index >= array_size || index < 0 || dur >= (SWITCH_DUR_MS / 2))
    {
        gui_obj_start_timer(obj);
        ms_start = ms_cur;
        return;
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);
        gui_log("switch index:%d\n", index);


    }
}
