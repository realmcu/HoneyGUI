/*
 * File      : gui_app.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <gui_app.h>
#include <string.h>
#include <string.h>
#include <stdio.h>
#include "gui_obj.h"
#include "gui_matrix.h"

gui_app_t *current_app;
gui_app_t *next_app;
static bool app_layer;
static void app_shutdown(gui_app_t *app);
void gui_app_layer_top()
{
    app_layer = true;
}
void gui_app_layer_buttom()
{
    app_layer = false;
}
bool gui_app_get_layer()
{
    return app_layer;
}
gui_app_t *gui_current_app(void)
{
    return current_app;
}
gui_app_t *gui_next_app(void)
{
    return next_app;
}

static void ctor(void *this)
{
    GUI_ASSERT(this != NULL);
}

static void dtor(void *this)
{
    GUI_ASSERT(this != NULL);
}
#define GUI_APP_MAX 32
static gui_app_t *app_array[GUI_APP_MAX];
static int app_array_count;
void gui_app_append(gui_app_t *app)
{


    if (app && app_array_count < GUI_APP_MAX - 1)
    {
        app_array[app_array_count] = app;
        app_array_count++;
    }
    else if (!app)
    {
        gui_log("gui_app_append NULL\n");
    }
    else if (!(app_array_count < GUI_APP_MAX - 1))
    {
        gui_log("gui_app_append app_array_count too much\n");
    }

}
gui_app_t *gui_app_get_by_name(const char *name)
{
    if (name)
    {
        for (size_t i = 0; i < app_array_count; i++)
        {
            if (strcmp(app_array[i]->screen.name, name) == 0)
            {
                return app_array[i];
            }
        }
    }
    return 0;

}
void gui_app_install(gui_app_t *app, void *ui_design, void *gui_app_entry)
{

    app->ctor = ctor;
    app->dtor = dtor;

    app->thread_entry = (void (*)(void *))gui_app_entry;
    app->ui_design = (void (*)(gui_app_t *))ui_design;
    ctor(app);
    gui_app_append(app);
}
#include "gui_win.h"
#define APP_WINDOW_NAME "__APP_WINDOW~!@#$_"
#define APP_TRANSITION_DURATION_MS 1000
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(app_transition_animation_function);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(app_shutdowm_transition_animation_function);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(app_shutdowm_transition_animation_function_custom);
void gui_app_startup(gui_app_t *app)
{
    app->close_sync = 0;
    if (current_app == app)
    {
        app_shutdown(current_app);
        current_app = 0;
    }

    gui_list_init(&app->screen.child_list);
    gui_list_init(&app->screen.brother_list);
    app->screen.opacity_value = UINT8_MAX;
    app->screen.magic = GUI_MAGIC_NUMBER;
    if (current_app && !current_app->close_sync)
    {
        next_app = app;
        current_app->next = true;
    }
    else
    {
        current_app = app;
    }
    app->window = GUI_TYPE(gui_win_t, &(app->screen));
    if (app->startup_animation_flag != GUI_APP_ANIMATION_NULL ||
        app->shutdown_animation_flag != GUI_APP_ANIMATION_NULL)
    {
        gui_win_t *win = gui_win_create(&(app->screen), APP_WINDOW_NAME, 0, 0, 0, 0);
        app->window = win;
        if (app->startup_animation_flag != GUI_APP_ANIMATION_NULL)
        {
            gui_win_set_animate(win, APP_TRANSITION_DURATION_MS, 0,  app_transition_animation_function, app);
        }
    }
    else if (app->startup_animation || app->shutdown_animation)
    {
        gui_win_t *win = gui_win_create(&(app->screen), APP_WINDOW_NAME, 0, 0, 0, 0);
        app->window = win;
        if (app->startup_animation)
        {
            gui_win_set_animate(win, APP_TRANSITION_DURATION_MS, 0,  app->startup_animation, app);
        }
    }



    app->ui_design(app);
    //gui_obj_tree_print_mmd(&(app->screen));
    if (app->active_ms == 0)
    {
        app->active_ms = 10000;
    }
    if (app->thread_entry != NULL)
    {
        app->thread_id = gui_thread_create(app->screen.name,
                                           app->thread_entry, app,
                                           1024 * 3, 25);
    }
    app->screen.opacity_value = UINT8_MAX;
    app->screen.matrix = gui_malloc(sizeof(struct gui_matrix));
    matrix_identity(app->screen.matrix);
    app->screen.w = gui_get_screen_width();
    app->screen.h = gui_get_screen_height();

    app->screen.create_done = true;
    app->start_ms = gui_ms_get();
}
static void app_shutdown(gui_app_t *app)
{
    if (app->next)
    {
        app->close = true;
        app->next = false;
        gui_log("app_shutdown async\n");
    }
    else
    {
        gui_log("app_shutdown sync\n");
        if (app->dtor != NULL)
        {
            app->dtor(app);
        }
        if (app->thread_entry != NULL)
        {
            gui_thread_delete(app->thread_id);
        }
        app->screen.parent = 0;
        gui_obj_tree_free(&app->screen);
        if (app->screen.matrix != NULL)
        {
            gui_free(app->screen.matrix);
            app->screen.matrix = NULL;
        }

        app->close_sync = true;
    }
}
void gui_app_shutdown(gui_app_t *app)
{
    app->close_sync = 0;
    if (app->shutdown_animation_flag != GUI_APP_ANIMATION_NULL)
    {
        if (app->window != GUI_TYPE(gui_win_t, &(app->screen)))
        {
            gui_win_set_animate(app->window, APP_TRANSITION_DURATION_MS, 0,
                                app_shutdowm_transition_animation_function, app);
            return;
        }
        else
        {
            gui_log("[APP ANIMATION]Please use GUI_APP_DEFINE_NAME_ANIMATION to make a APP");
        }

    }
    else if (app->shutdown_animation)
    {
        if (app->window != GUI_TYPE(gui_win_t, &(app->screen)))
        {
            gui_win_append_animate(app->window, APP_TRANSITION_DURATION_MS, 0,
                                   app->shutdown_animation, app, "customShutdowm");
            gui_win_append_animate(app->window, APP_TRANSITION_DURATION_MS, 0,
                                   app_shutdowm_transition_animation_function_custom, app, "customShutdowm");
            return;
        }
        else
        {
            gui_log("[APP ANIMATION]Please use GUI_APP_DEFINE_NAME_ANIMATION_FUNC_CUSTOM to make a APP");
        }

    }
    app_shutdown(app);

}

gui_app_t *gui_app_create(const char *app_name, void *ui_design, void *gui_app_entry)
{
    gui_app_t *app = gui_malloc(sizeof(gui_app_t));
    memset(app, 0, sizeof(gui_app_t));
    app->screen.name = app_name;

    gui_app_install(app, ui_design, gui_app_entry);

    return app;
}

void gui_app_switch(gui_app_t *from, gui_app_t *to)
{
    if (from == NULL)
    {
        from = current_app;
    }
    gui_app_shutdown(from);

    gui_app_startup(to);
    gui_log("gui_app_switch from %s to %s\n", from->screen.name, to->screen.name);
}

void gui_set_app_active_time(gui_app_t *app, uint32_t active_ms)
{
    app->active_ms = active_ms;
}
gui_app_t *gui_obj_tree_get_app(gui_obj_t *obj)
{
    gui_obj_t *child = obj;
    if (!child)
    {
        return (gui_app_t *)0;
    }
    while (true)
    {
        if (child)
        {
            if (child->parent == NULL && child->type == SCREEN)
            {
                gui_log("gui_obj_tree_get_root = %s\n", child->name);
                return (gui_app_t *)child;
            }
            else
            {
                child = child->parent;
            }
        }
    }
}

static float ease_in_out(float t)
{
    return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
}
static float bounce_ease_out(float t)
{
    if (t < 1 / 2.75f)
    {
        return 7.5625f * t * t;
    }
    else if (t < 2 / 2.75f)
    {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
    }
    else if (t < 2.5f / 2.75f)
    {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
    }
    else
    {
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
}
static void app_transition_animation(float pro, gui_win_t *win)
{
    // Ensure progress is between 0.0 and 1.0
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function to transform progress
    float eased_pro = (pro);

    // Calculate and set scale rate
    float scale_rate_horizontal = 1.0f + (0.5f * (1.0f - eased_pro));  // Scale from 1.5 to 1.0
    float scale_rate_vertical = scale_rate_horizontal;    // Scale from 1.5 to 1.0
    gui_win_set_scale_rate(win, scale_rate_horizontal, scale_rate_vertical);

    // Calculate and set opacity
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);  // Opacity from 0 to 255
    gui_win_set_opacity(win, opacity_value);

    // Calculate and set position (assume window moves along the X-axis, height remains the same)
    int x_start = 100;  // Start position
    int x_end = 0;      // End position
    int y_position = gui_win_get_y(win);  // Maintain Y position
    int new_x_position = x_start + (int)((x_end - x_start) * eased_pro);
    gui_win_move(win, new_x_position, y_position);
}

static void app_transition_animation_from_right(float pro, gui_win_t *win)
{
    // Ensure progress is between 0.0 and 1.0
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function to transform progress
    float eased_pro = (pro);

    // Get screen width
    uint32_t screen_width = gui_get_screen_width();

    // Set initial and final positions of the window
    int start_x = screen_width; // Right side of the screen
    int end_x = 0; // Left side position

    // Get window's Y-coordinate to keep it unchanged
    int y_position = gui_win_get_y(win);

    // Calculate current position
    int new_x_position = start_x - (int)((start_x - end_x) * eased_pro);

    // Move window
    gui_win_move(win, new_x_position, y_position);

    // Calculate and set opacity
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    gui_win_set_opacity(win, opacity_value);

    // Optionally add scale animation, assume no scale animation here
    float scale_rate_horizontal = 1.0f;
    float scale_rate_vertical = 1.0f;
    gui_win_set_scale_rate(win, scale_rate_horizontal, scale_rate_vertical);
}
static void app_transition_animation_to_opacity0(float eased_pro, gui_win_t *win)
{
    // Ensure progress is between 0.0 and 1.0
    if (eased_pro < 0.0f) { eased_pro = 0.0f; }
    if (eased_pro > 1.0f) { eased_pro = 1.0f; }
    // Calculate and set opacity
    unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    gui_win_set_opacity(win, opacity_value);
}
static void app_transition_animation_from_opacity0(float eased_pro, gui_win_t *win)
{
    // Ensure progress is between 0.0 and 1.0
    if (eased_pro < 0.0f) { eased_pro = 0.0f; }
    if (eased_pro > 1.0f) { eased_pro = 1.0f; }
    // Calculate and set opacity
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    gui_win_set_opacity(win, opacity_value);
}
static void app_transition_animation_to_right(float pro, gui_win_t *win)
{
    // Ensure progress is between 0.0 and 1.0
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function to transform progress
    float eased_pro = (pro);

    // Get screen width
    uint32_t screen_width = gui_get_screen_width();

    // Set initial and final positions of the window
    int start_x = gui_win_get_x(win); // Current window position
    int end_x = screen_width; // Right side of the screen

    // Get window's Y-coordinate to keep it unchanged
    int y_position = gui_win_get_y(win);

    // Calculate current position
    int new_x_position = start_x + (int)((end_x - start_x) * eased_pro);

    // Move window
    gui_win_move(win, new_x_position, y_position);

    // Calculate and set opacity
    unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    gui_win_set_opacity(win, opacity_value);

    // Optionally add scale animation, assume no scale animation here
    float scale_rate_horizontal = 1.0f;
    float scale_rate_vertical = 1.0f;
    gui_win_set_scale_rate(win, scale_rate_horizontal, scale_rate_vertical);
}

static void app_transition_animation_zoom_center(float pro, gui_win_t *win)
{
    // Ensure progress is between 0.0f and 1.0f
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use an easing function to transform progress
    float eased_pro = pro; // Replace this with an actual easing function if desired


    // Calculate scaling factor (scale from 0.5 to 1.0)
    float scale_rate_horizontal = 0.5f + 0.5f * eased_pro;
    float scale_rate_vertical = scale_rate_horizontal;
    gui_win_set_scale_rate(win, scale_rate_horizontal, scale_rate_vertical);

    // Calculate and set opacity (from 0 to 255)
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    gui_win_set_opacity(win, opacity_value);


}
static void app_transition_animation_collapse_to_center(float pro, gui_win_t *win)
{
    // Ensure progress is between 0.0f and 1.0f
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use an easing function to transform progress
    float eased_pro = pro; // Replace this with an actual easing function if desired



    // Calculate scaling factor (scale from 1.0 to 0.5)
    float scale_rate_horizontal = 1.0f - 0.5f * eased_pro;
    float scale_rate_vertical = scale_rate_horizontal;
    gui_win_set_scale_rate(win, scale_rate_horizontal, scale_rate_vertical);

    // Calculate and set opacity (from 255 to 0)
    unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    gui_win_set_opacity(win, opacity_value);


}

static gui_error_t animation_case(gui_app_animation_t flag, gui_app_t *app, gui_win_t *win,
                                  float pro)
{
    gui_error_t e = GUI_ERROR;
    switch (flag)
    {
    case GUI_APP_ANIMATION_NULL:
        {app_shutdown(app);}
        return e;
    case GUI_APP_ANIMATION_1:
        {
            app_transition_animation_zoom_center(ease_in_out(pro), win);
        }
        break;
    case GUI_APP_ANIMATION_2:
        {
            app_transition_animation(ease_in_out(pro), win);
        }
        break;
    case GUI_APP_ANIMATION_3:
        {
            app_transition_animation_from_right(ease_in_out(pro), win);
        }
        break;
    case GUI_APP_ANIMATION_4:
        {
            app_transition_animation_from_right(bounce_ease_out(pro), win);
        }
        break;
    case GUI_APP_ANIMATION_5:
        {
            app_transition_animation_collapse_to_center(ease_in_out(pro), win);
        }
        break;
    case GUI_APP_ANIMATION_6:
        {
            //app_transition_animation(ease_in_out(pro), win);
        }
        break;
    case GUI_APP_ANIMATION_7:
        {
            app_transition_animation_to_right(ease_in_out(pro), win);
        }
        break;
    case GUI_APP_ANIMATION_8:
        {
            //app_transition_animation_from_right(bounce_ease_out(pro), win);
        }
    case GUI_APP_ANIMATION_9:
        {
            app_transition_animation_from_opacity0(ease_in_out(pro), win);
        }
        break;
    case GUI_APP_ANIMATION_10:
        {
            app_transition_animation_to_opacity0(ease_in_out(pro), win);
        }
        break;
    default:
        break;
    }
    e = GUI_SUCCESS;
    return e;
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(app_transition_animation_function)
{
    gui_app_t *app = p;
    gui_app_animation_t flag = app->startup_animation_flag;
    gui_win_t *win = GUI_TYPE(gui_win_t, this_widget);
    float pro = gui_win_get_animation_progress_percent(win); gui_log("startup%f\n", pro);
    gui_error_t e = animation_case(flag, app, win, pro);
    if (e == GUI_SUCCESS)
    {
        if (gui_win_is_animation_end_frame(win))
        {
            gui_win_set_scale_rate(win, 0, 0);//reset scale
            gui_win_set_opacity(win, UINT8_MAX);
            gui_win_move(win, 0, 0);
        }
    }
    else
    {
        return;
    }
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(app_shutdowm_transition_animation_function)
{
    gui_app_t *app = p;
    gui_app_animation_t flag = app->shutdown_animation_flag;
    gui_win_t *win = GUI_TYPE(gui_win_t, this_widget);
    float pro = gui_win_get_animation_progress_percent(win); gui_log("shutdowm%f\n", pro);
    gui_error_t e = animation_case(flag, app, win, pro);
    if (e == GUI_SUCCESS)
    {
        if (gui_win_is_animation_end_frame(win))
        {
            //gui_win_set_scale_rate(win, 0, 0);//reset scale
            //gui_win_set_opacity(win, UINT8_MAX);
            //gui_win_move(win, 0,0);
            app_shutdown(app);
        }
    }
    else
    {
        return;
    }
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(app_shutdowm_transition_animation_function_custom)
{
    gui_app_t *app = p;
    if (animate->end_frame)
    {
        app_shutdown(app);
    }
}
