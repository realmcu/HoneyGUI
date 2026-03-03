/**
 * @file hml_button.c
 * @brief HML Button Component (hg_button)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_api.h"
#include <string.h>
#include "gui_obj_type.h"  // For T_OBJ_CB_TYPE, OBJ_DESTROY

typedef struct
{
    gui_img_t *img_on;
    gui_img_t *img_off;
    bool current_state;
    bool toggle_mode;
} hml_button_state_t;

static void button_click_handler(void *obj_ptr, gui_event_t *event)
{
    (void)event;
    gui_obj_t *obj = (gui_obj_t *)obj_ptr;
    hml_button_state_t *state = (hml_button_state_t *)obj->user_data;

    if (!state || !state->toggle_mode) { return; }

    // Toggle state
    state->current_state = !state->current_state;
    GUI_BASE(state->img_on)->not_show = !state->current_state;
    GUI_BASE(state->img_off)->not_show = state->current_state;
}

/**
 * @brief Lifecycle callback for button cleanup
 */
static void button_lifecycle_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (cb_type == OBJ_DESTROY)
    {
        hml_button_state_t *state = (hml_button_state_t *)obj->user_data;
        if (state)
        {
            gui_free(state);
            obj->user_data = NULL;
        }
    }
}

gui_obj_t *hml_create_button(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "button");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 100);

    const char *toggle_str = hml_attr_str(node, "toggleMode", "false");
    const char *init_state = hml_attr_str(node, "initialState", "off");
    const char *img_on_path = ezxml_attr(node, "imageOn");
    const char *img_off_path = ezxml_attr(node, "imageOff");

    if (!img_on_path || !img_off_path)
    {
        gui_log("[HML] Button missing imageOn/imageOff\n");
        return NULL;
    }

    // Resolve paths (assets/icon.png → /hml/icon.bin)
    char on_path[128], off_path[128];
    hml_resolve_asset_path(img_on_path, on_path, sizeof(on_path));
    hml_resolve_asset_path(img_off_path, off_path, sizeof(off_path));

    // Create container
    gui_win_t *container = gui_win_create(parent, id, x, y, w, h);

    // Create both images at same position
    gui_img_t *img_on = gui_img_create_from_fs(GUI_BASE(container), "btn_on", (void *)on_path, 0, 0, 0,
                                               0);
    gui_img_t *img_off = gui_img_create_from_fs(GUI_BASE(container), "btn_off", (void *)off_path, 0, 0,
                                                0, 0);

    // Set initial visibility
    bool is_on = (strcmp(init_state, "on") == 0);
    GUI_BASE(img_on)->not_show = !is_on;
    GUI_BASE(img_off)->not_show = is_on;

    // Allocate and store state
    hml_button_state_t *state = (hml_button_state_t *)gui_malloc(sizeof(hml_button_state_t));
    state->img_on = img_on;
    state->img_off = img_off;
    state->current_state = is_on;
    state->toggle_mode = (strcmp(toggle_str, "true") == 0);

    GUI_BASE(container)->user_data = state;

    // Register destroy callback
    GUI_BASE(container)->obj_cb = button_lifecycle_cb;
    GUI_BASE(container)->has_destroy_cb = 1;

    // Register click handler
    gui_obj_add_event_cb(GUI_BASE(container), (gui_event_cb_t)button_click_handler,
                         GUI_EVENT_TOUCH_CLICKED, state);

    return GUI_BASE(container);
}
