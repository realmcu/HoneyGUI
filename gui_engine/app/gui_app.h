/*
 * File      : gui_tabview.h
 */
#ifndef __GUI_APP_H__
#define __GUI_APP_H__
#include <guidef.h>
#include <gui_fb.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct gui_app gui_app_t;
struct gui_app
{
    gui_obj_t screen;
    const char *xml;
    uint32_t active_ms;
    void *thread_id;
    void (* thread_entry)(void *this);
    void (* ctor)(void *this);
    void (* dtor)(void *this);
    void (* ui_design)(gui_app_t *);

};
gui_app_t *gui_current_app(void);

void gui_app_exec(gui_app_t *app);

void gui_app_install(gui_app_t *app, void *ui_design, void *gui_app_entry);

void gui_app_startup(gui_app_t *app);

void gui_app_shutdown(gui_app_t *app);

void gui_app_uninstall(gui_app_t *app);

gui_app_t *gui_app_create(const char *app_name, void *ui_design, void *gui_app_entry);

void gui_app_delete(gui_app_t *app);

void gui_switch_app(gui_app_t *from, gui_app_t *to);


#ifdef __cplusplus
}
#endif
#endif

