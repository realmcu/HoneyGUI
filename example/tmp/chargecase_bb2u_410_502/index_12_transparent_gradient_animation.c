#include "gui_view_macro.h"
#include "gui_img.h"
#include "math.h"
#define WATCHFACE_DUR_MS (5000)
#define SCREEN_W 410
#define SCREEN_H 502

static void switch_widget_play_watchface(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    gui_img_t *img = (gui_img_t *)obj;

    static uint64_t ms_start = 0;
    uint64_t ms_cur = gui_ms_get();
    if (ms_start == 0)
    {
        ms_start = ms_cur;
    }
    uint64_t elapsed = ms_cur - ms_start;
    if (elapsed >= WATCHFACE_DUR_MS)
    {
        gui_obj_move((void *)img, 50, SCREEN_H - 150);

        ms_start = 0;
        return;
    }
    float t = (float)elapsed / WATCHFACE_DUR_MS;
    float t_ease = t * t * (3.0f - 2.0f * t);
    float start_x = 0;
    float start_y = 0;
    float end_x   = SCREEN_W - 150;
    float end_y   = 80;
    float mid_y_offset = 60.0f * sinf(t * 3.14159f);
    float x = start_x + (end_x - start_x) * t_ease;
    float y = start_y + (end_y - start_y) * t_ease - mid_y_offset;
    gui_obj_move((void *)img, (int)x, (int)y);

}
static void switch_widget_play_watchface2(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    gui_img_t *img = (gui_img_t *)obj;

    static uint64_t ms_start = 0;
    uint64_t ms_cur = gui_ms_get();
    if (ms_start == 0)
    {
        ms_start = ms_cur;
    }
    uint64_t elapsed = ms_cur - ms_start;
    if (elapsed >= WATCHFACE_DUR_MS)
    {
        gui_obj_move((void *)img, 50, SCREEN_H - 150);

        ms_start = 0;
        return;
    }
    float t = (float)elapsed / WATCHFACE_DUR_MS;
    float t_ease = t * t * (3.0f - 2.0f * t);
    float start_x = 100;
    float start_y = 100;
    float end_x   = SCREEN_W - 150;
    float end_y   = SCREEN_H - 150;
    float mid_y_offset = 60.0f * sinf(t * 3.14159f);
    float x = start_x + (end_x - start_x) * t_ease;
    float y = start_y + (end_y - start_y) * t_ease - mid_y_offset;
    gui_obj_move((void *)img, (int)x, (int)y);

}
static void switch_widget_play_watchface3(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    gui_img_t *img = (gui_img_t *)obj;

    static uint64_t ms_start = 0;
    uint64_t ms_cur = gui_ms_get();
    if (ms_start == 0)
    {
        ms_start = ms_cur;
    }
    uint64_t elapsed = ms_cur - ms_start;
    if (elapsed >= WATCHFACE_DUR_MS)
    {
        gui_obj_move((void *)img, 50, SCREEN_H - 150);

        ms_start = 0;
        return;
    }
    float t = (float)elapsed / WATCHFACE_DUR_MS;
    float t_ease = t * t * (3.0f - 2.0f * t);
    float start_x = SCREEN_W - 150;
    float start_y = SCREEN_H - 150;
    float end_x   = 100;
    float end_y   = 100 ;
    float mid_y_offset = 60.0f * sinf(t * 3.14159f);
    float x = start_x + (end_x - start_x) * t_ease;
    float y = start_y + (end_y - start_y) * t_ease - mid_y_offset;
    gui_obj_move((void *)(void *)img, (int)x, (int)y);
}

static void transparent_gradient_animation_switch_in(gui_view_t *view)
{

    gui_view_switch_on_event(view, "vector_graphic_view",
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_switch_on_event(view, "smooth_gradient_image_view",
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    GUI_UNUSED(view);
    gui_log("transparent_gradient_animation_view switch in\n");
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(BOTTOM_BG_BIN), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);


    }
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(BOTTOM_BG_BIN), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_create_timer((void *)img, 10, true, switch_widget_play_watchface);

    }
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(MODE_CONT_BIN), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_create_timer((void *)img, 10, true, switch_widget_play_watchface2);

    }
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(RETURN_BIN), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_create_timer((void *)img, 10, true, switch_widget_play_watchface3);

    }

}

static void transparent_gradient_animation_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("transparent_gradient_animation_view switch out\n");
}

GUI_VIEW_INSTANCE("transparent_gradient_animation_view", 1,
                  transparent_gradient_animation_switch_in, transparent_gradient_animation_switch_out);

