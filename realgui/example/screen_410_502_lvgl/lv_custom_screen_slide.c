#include "lv_custom_screen_slide.h"
#include "lv_img_dsc_list.h"


lv_obj_t *screen1;
lv_obj_t *screen2;
lv_coord_t initial_x = 0;
lv_coord_t delta_x = 0;
lv_coord_t screen_width = 0;

static void touch_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *scr_act = lv_scr_act();
    struct _lv_display_t *disp = lv_disp_get_default();

    if (code == LV_EVENT_PRESSED)
    {
        LV_LOG("Event: PRESSED\n");
        // Record initial touch position
        lv_indev_t *indev = lv_event_get_indev(e);
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        initial_x = p.x;
        screen_width = lv_obj_get_width(scr_act);
        LV_LOG("Initial touch position: x=%d\n", initial_x);

        if (lv_layer_bottom())
        {
            lv_obj_delete(lv_layer_bottom());
        }

        if (scr_act == screen1)
        {
            disp->bottom_layer = screen2;
        }
        if (scr_act == screen2)
        {
            disp->bottom_layer = screen1;
        }
    }
    else if (code == LV_EVENT_PRESSING)
    {
        LV_LOG("Event: PRESSING\n");
        // Calculate current sliding distance
        lv_indev_t *indev = lv_event_get_indev(e);
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        delta_x = p.x - initial_x;
        LV_LOG("Delta x: %d\n", delta_x);

        // If on screen1 and sliding left, show screen2
        if (scr_act == screen1 && delta_x < 0)
        {
            lv_obj_set_x(screen1, delta_x);
            lv_obj_set_x(screen2, screen_width + delta_x);  // Move screen2 simultaneously
            LV_LOG("Moving Screen1 and Screen2: Screen1 x=%d, Screen2 x=%d\n", delta_x, screen_width + delta_x);
        }
        // If on screen2 and sliding right, show screen1
        else if (scr_act == screen2 && delta_x > 0)
        {
            lv_obj_set_x(screen2, delta_x);
            lv_obj_set_x(screen1, delta_x - screen_width);
            LV_LOG("Moving Screen2 and Screen1: Screen2 x=%d, Screen1 x=%d\n", delta_x, delta_x - screen_width);
        }
    }
    else if (code == LV_EVENT_RELEASED)
    {
        LV_LOG("Event: RELEASED\n");
        // Determine whether to switch screens or reset positions
        disp->bottom_layer = lv_obj_create(NULL);
        if (scr_act == screen1 && delta_x < -screen_width / 2)
        {
            LV_LOG("Switching to Screen 2\n");
            lv_scr_load_anim(screen2, LV_SCR_LOAD_ANIM_OVER_RIGHT, 0, 0, false);
        }
        else if (scr_act == screen2 && delta_x > screen_width / 2)
        {
            LV_LOG("Switching to Screen 1\n");
            lv_scr_load_anim(screen1, LV_SCR_LOAD_ANIM_OVER_LEFT, 0, 0, false);
        }
        else
        {
            LV_LOG("Restoring screens\n");
            if (scr_act == screen1)
            {
                lv_obj_set_x(screen1, 0);
                lv_obj_set_x(screen2, screen_width);  // Ensure screen2 resets to right side
            }
            else if (scr_act == screen2)
            {
                lv_obj_set_x(screen2, 0);
                lv_obj_set_x(screen1, -screen_width); // Ensure screen1 resets to left side
            }
        }
    }
}

void create_screens(void)
{
    // Initialize first screen
    screen1 = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen1, lv_color_hex(0x000000), 0); // Black background
    lv_obj_set_size(screen1, lv_pct(100), lv_pct(100));  // Full-screen size

    // Add label to first screen
    lv_obj_t *label1 = lv_label_create(screen1);
    lv_label_set_text(label1, "Screen 1");
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0); // Center alignment
    lv_obj_set_style_text_color(label1, lv_color_hex(0xFFFFFF), 0); // White text color

    // Create red rectangle on first screen
    lv_obj_t *rect1 = lv_obj_create(screen1);
    lv_obj_set_size(rect1, lv_pct(100), lv_pct(25)); // Quarter screen height
    lv_obj_set_style_bg_color(rect1, lv_color_hex(0xFF0000), 0); // Red color
    lv_obj_align(rect1, LV_ALIGN_BOTTOM_MID, 0, 0); // Align at bottom center

    // Initialize second screen
    screen2 = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0x000000), 0); // Black background
    lv_obj_set_size(screen2, lv_pct(100), lv_pct(100)); // Full-screen size

    // Add label to second screen
    lv_obj_t *label2 = lv_label_create(screen2);
    lv_label_set_text(label2, "Screen 2");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0); // Center alignment
    lv_obj_set_style_text_color(label2, lv_color_hex(0xFFFFFF), 0); // White text color

    // Create blue rectangle on second screen
    lv_obj_t *rect2 = lv_obj_create(screen2);
    lv_obj_set_size(rect2, lv_pct(100), lv_pct(25)); // Quarter screen height
    lv_obj_set_style_bg_color(rect2, lv_color_hex(0x0000FF), 0); // Blue color
    lv_obj_align(rect2, LV_ALIGN_BOTTOM_MID, 0, 0); // Align at bottom center

    // Add touch events to screens
    lv_obj_add_event_cb(screen1, touch_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(screen2, touch_event_handler, LV_EVENT_ALL, NULL);

    // Initial position setup
    lv_obj_set_x(screen2, lv_obj_get_width(screen1));

    // Load initial screen
    lv_screen_load(screen1);

    lv_obj_set_x(screen2, lv_obj_get_width(screen1) / 2);
    lv_obj_t *scr_act = lv_scr_act();
    struct _lv_display_t *disp = lv_disp_get_default();

    LV_LOG("Initial Screen Loaded: Screen1\n");
}

void change_screen(void)
{
    LV_LOG("change_screen \n");
    if (lv_scr_act() != screen1)
    {
        lv_screen_load(screen1);
    }
}
