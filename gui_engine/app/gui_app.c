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
#define GUI_APP_MAX UINT8_MAX
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

void gui_app_startup(gui_app_t *app)
{
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
    app->ui_design(app);
    gui_obj_tree_print_mmd(&(app->screen));
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
#ifdef ENABLE_GUI_CONTAIN_FUNCTION
    app->screen.range_w = gui_get_screen_width();
    app->screen.range_h = gui_get_screen_height();
#endif
    app->screen.create_done = true;
    app->start_ms = gui_ms_get();
}

void gui_app_shutdown(gui_app_t *app)
{

    if (app->next)
    {
        app->close = true;
    }
    else
    {
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

gui_app_t *gui_app_create(const char *app_name, void *ui_design, void *gui_app_entry)
{
    gui_app_t *app = gui_malloc(sizeof(gui_app_t));
    memset(app, 0, sizeof(gui_app_t));
    app->screen.name = app_name;

    gui_app_install(app, ui_design, gui_app_entry);

    return app;
}

void gui_switch_app(gui_app_t *from, gui_app_t *to)
{
    gui_app_shutdown(from);

    gui_app_startup(to);
    gui_log("gui_switch_app from %s to %s\n", from->screen.name, to->screen.name);
}

void gui_set_app_active_time(gui_app_t *app, uint32_t active_ms)
{
    app->active_ms = active_ms;
}
gui_app_t *gui_obj_tree_get_app(gui_obj_t *obj)
{
    gui_obj_t *child = obj;

    while (true)
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
