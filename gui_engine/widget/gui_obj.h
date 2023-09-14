/*
 * File      : gui_tabview.h
 */
#ifndef __GUI_OBJ_H__
#define __GUI_OBJ_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif


void gui_obj_ctor(struct _gui_obj_t *this, struct _gui_obj_t *parent, const char *filename,
                  int16_t x, int16_t y, int16_t w, int16_t h);

void gui_obj_dtor(struct _gui_obj_t *this);

void gui_obj_add_event_cb(void *obj, gui_event_cb_t event_cb, gui_event_t filter, void *user_data);

void gui_obj_event_set(gui_obj_t *obj, gui_event_t event_code);

void gui_tree_free(void *obj);

void gui_tree_print(gui_obj_t *obj);

void gui_tree_not_show(gui_obj_t *obj);

void gui_tree_show(gui_obj_t *obj, bool enable);

gui_obj_t *gui_tree_get_root(gui_obj_t *obj);

gui_obj_t *gui_get_child_handle(gui_obj_t *obj, obj_type_t child_type);

void gui_tree_get_cover(gui_obj_t *obj, obj_type_t target, bool *rst);

#ifdef __cplusplus
}
#endif

#endif

