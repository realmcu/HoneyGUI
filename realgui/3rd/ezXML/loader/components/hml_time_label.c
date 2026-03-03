/**
 * @file hml_time_label.c
 * @brief HML Time Label Component (hg_time_label)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_text.h"
#include "gui_api.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "gui_obj_type.h"  // For T_OBJ_CB_TYPE, OBJ_DESTROY

/**
 * @brief State structure for time label widget
 */
typedef struct
{
    gui_text_t *text;
    char format[32];
    int font_size;
    gui_color_t color;
} hml_time_label_state_t;

/**
 * @brief Update time display callback
 */
static void time_update_callback(void *obj_ptr)
{
    gui_obj_t *obj = (gui_obj_t *)obj_ptr;
    hml_time_label_state_t *state = (hml_time_label_state_t *)obj->user_data;

    if (!state) { return; }

    // Get current time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    // Format time string
    char buffer[64];
    if (strcmp(state->format, "HH:mm") == 0)
    {
        snprintf(buffer, sizeof(buffer), "%02d:%02d",
                 tm_info->tm_hour, tm_info->tm_min);
    }
    else if (strcmp(state->format, "HH:mm:ss") == 0)
    {
        snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d",
                 tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    }
    else if (strcmp(state->format, "HH:mm-split") == 0)
    {
        // Split format - vertical layout with newline
        snprintf(buffer, sizeof(buffer), "%02d\n%02d",
                 tm_info->tm_hour, tm_info->tm_min);
    }
    else
    {
        // Default fallback
        snprintf(buffer, sizeof(buffer), "%02d:%02d",
                 tm_info->tm_hour, tm_info->tm_min);
    }

    // Update text content
    gui_text_set(state->text, buffer, GUI_FONT_SRC_BMP, state->color,
                 strlen(buffer), state->font_size);
}

/**
 * @brief Lifecycle callback for time label cleanup
 */
static void time_label_lifecycle_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (cb_type == OBJ_DESTROY)
    {
        hml_time_label_state_t *state = (hml_time_label_state_t *)obj->user_data;
        if (state)
        {
            gui_free(state);
            obj->user_data = NULL;
        }
        // Free timer if allocated
        if (obj->timer)
        {
            gui_free(obj->timer);
            obj->timer = NULL;
        }
    }
}

/**
 * @brief Create time label widget
 *
 * @param parent Parent widget
 * @param node XML node containing attributes
 * @return gui_obj_t* Pointer to created widget
 */
gui_obj_t *hml_create_time_label(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "time_label");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 50);
    int font_size = hml_attr_int(node, "fontSize", 32);
    gui_color_t color = hml_attr_color(node, "color", 0xFFFFFFFF);
    const char *format = hml_attr_str(node, "timeFormat", "HH:mm");

    // Parse text alignment (copy from hml_label.c)
    const char *h_align = hml_attr_str(node, "hAlign", "LEFT");
    const char *v_align = hml_attr_str(node, "vAlign", "TOP");

    TEXT_MODE mode = LEFT;
    if (strcmp(h_align, "CENTER") == 0)
    {
        if (strcmp(v_align, "MIDDLE") == 0) { mode = MID_CENTER; }
        else { mode = CENTER; }
    }
    else if (strcmp(h_align, "RIGHT") == 0)
    {
        if (strcmp(v_align, "MIDDLE") == 0) { mode = MID_RIGHT; }
        else { mode = RIGHT; }
    }
    else // LEFT
    {
        if (strcmp(v_align, "MIDDLE") == 0) { mode = MID_LEFT; }
        else { mode = LEFT; }
    }

    // Create text widget
    gui_text_t *text = gui_text_create(parent, id, x, y, w, h);
    gui_text_mode_set(text, mode);

    // Allocate state
    hml_time_label_state_t *state = gui_malloc(sizeof(hml_time_label_state_t));
    if (!state) { return (gui_obj_t *)text; }

    memset(state, 0, sizeof(hml_time_label_state_t));
    state->text = text;
    state->font_size = font_size;
    state->color = color;
    strncpy(state->format, format, sizeof(state->format) - 1);
    state->format[sizeof(state->format) - 1] = '\0';

    GUI_BASE(text)->user_data = state;

    // Register periodic timer for auto-update
    uint32_t interval_ms = (strstr(format, "ss") != NULL ||
                            strstr(format, ":ss") != NULL) ? 1000 : 60000;

    gui_obj_timer_t *timer = gui_malloc(sizeof(gui_obj_timer_t));
    if (timer)
    {
        timer->interval_ms = interval_ms;
        timer->reload = true;  // Periodic timer
        timer->p_timer_callback = time_update_callback;
        timer->expire_time = gui_ms_get() + interval_ms;
        GUI_BASE(text)->timer = timer;
    }

    // Register destroy callback
    GUI_BASE(text)->obj_cb = time_label_lifecycle_cb;
    GUI_BASE(text)->has_destroy_cb = 1;

    // Perform initial update
    time_update_callback(GUI_BASE(text));

    return (gui_obj_t *)text;
}
