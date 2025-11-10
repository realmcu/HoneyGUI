#include "gui_view_macro.h"

#include "gui_img.h"
#define SWITCH_DUR_MS (600)
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
                                    0, (void *)FILE_POINTER(FRAME_0024_BIN), 100, 100, 0, 0);

        gui_obj_create_timer((void *)switch_widget, 1, true, switch_widget_play);
    }
    {
        gui_img_t *switch_widget =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(FRAME_0024_BIN), 100, 300, 0, 0);

        gui_obj_create_timer((void *)switch_widget, 1, true, switch_widget_play2);
    }
}

static void gif_apng_support_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("gif_apng_support_view switch out\n");
}

GUI_VIEW_INSTANCE("gif_apng_support_view", false, gif_apng_support_switch_in,
                  gif_apng_support_switch_out);
static void switch_widget_play(void *p)
{
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
        gui_log("switch index:%d\n", index);


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
