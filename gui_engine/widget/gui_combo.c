#include "gui_combo.h"
#include "gui_button.h"




static void prepare(gui_obj_t *obj)
{
    gui_combo_t *combo = (void *)obj;
    IMPORT_GUI_TOUCHPAD
    int x, y;
    gui_obj_absolute_xy(obj, &x, &y);
    if (touch->pressed && !combo->fold && !(touch->x > x && touch->x < x + obj->w && touch->y > y &&
                                            touch->y < y + obj->h))
    {
        gui_obj_hidden((void *)combo->window_select, 1);
        combo->fold = !combo->fold;
    }

}

static void pipeline(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            prepare(obj);
            break;
        default:
            break;
        }
    }
}




static void ctor(gui_combo_t *widget, gui_obj_t *parent, int x, int y, int w, int item_count,
                 int item_height, const char *items[], uint8_t *item_background_image,
                 uint8_t *item_background_highlight_image)
{
    gui_obj_ctor(GUI_BASE(widget), parent, "COMBO", x, y, w, item_height * (item_count + 1));
    GET_BASE(widget)->type = MACRO_COMBO;
    GET_BASE(widget)->obj_cb = pipeline;
    GET_BASE(widget)->has_prepare_cb = true;
    widget->fold = 1;
    widget->items = items;
}
static void selector_button_cb(void *obj, gui_event_t e, void *param)
{
    gui_combo_t *combo = param;

    switch ((int)combo->fold)
    {
    case true:
        {
            gui_obj_hidden((void *)combo->window_select, 0);
        }
        break;
    case false:
        {
            gui_obj_hidden((void *)combo->window_select, 1);
        }
        break;
    default:
        break;
    }
    combo->fold = !combo->fold;
}
static void item_button_cb(void *obj, gui_event_t e, void *param)
{
    gui_obj_t *button = obj;
    gui_combo_t *combo = (void *)button->parent->parent;
    GUI_WIDGET_TYPE_TRY_EXCEPT(combo, MACRO_COMBO)
    switch ((int)combo->fold)
    {
    case false:
        {
            gui_obj_hidden((void *)combo->window_select, 1);
            gui_text_content_set(combo->button_selector_text, (void *)combo->items[(size_t)param],
                                 strlen(combo->items[(size_t)param]));
            combo->current_item = (size_t)param;
            combo->on_change_function ? combo->on_change_function(combo, 0,
                                                                  combo->on_change_function_parameter) : (void)0;
        }
        break;
    default:
        break;
    }
    combo->fold = !combo->fold;
}
static void current_item(gui_combo_t *combo, int *item_index, const char **item_text)
{
    GUI_WIDGET_TYPE_TRY_EXCEPT(combo, MACRO_COMBO)
    *item_index = combo->current_item;
    *item_text = combo->items[combo->current_item];
}
static void on_change(gui_combo_t *combo, gui_event_cb_t on_change_function, void *parameter)
{
    combo->on_change_function = on_change_function;
    combo->on_change_function_parameter = parameter;
}
gui_combo_t *gui_combo_create(gui_obj_t *parent, int x, int y, int w, int item_count,
                              int item_height,
                              const char *items[], uint8_t *item_background_image, uint8_t *item_background_highlight_image,
                              int font_size, gui_color_t font_color, uint8_t *font_file_pointer)
{
#define _GUI_NEW_GUI_COMBO_PARAM this, parent, x, y, w, item_count, item_height, items, item_background_image, item_background_highlight_image
    GUI_CREATE_HELPER(gui_combo_t, ctor, _GUI_NEW_GUI_COMBO_PARAM)
    gui_button_t *button_selector = gui_button_create(parent, 0, 0, w, item_height,
                                                      item_background_image, item_background_highlight_image, 0, 0, 0);
    GUI_API(gui_button_t).on_click(button_selector, selector_button_cb, this);

    gui_img_set_mode(button_selector->img, IMG_BYPASS_MODE);
    {
        const char *text = items[0];
        gui_text_t *t = gui_text_create(button_selector, text, 0, 0, w, item_height);
        gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, font_color, strlen(text), font_size);
        const void *addr1 = font_file_pointer;
        gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
        this->button_selector_text = t;
    }
    gui_win_t *window_select = gui_win_create(this, 0, 0, item_height, w, item_height * item_count);
    this->window_select = window_select;
    for (size_t i = 0; i < item_count; i++)
    {
        gui_button_t *button_selector = gui_button_create(window_select, 0, (i) * item_height, w,
                                                          item_height, item_background_image, item_background_highlight_image, 0, 0, 0);
        GUI_API(gui_button_t).on_click(button_selector, item_button_cb, (void *)i);
        gui_img_set_mode(button_selector->img, IMG_BYPASS_MODE);
        {
            const char *text = items[i];
            gui_text_t *t = gui_text_create(button_selector, text, 0, 0, w,
                                            item_height);
            gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, font_color, strlen(text), font_size);
            const void *addr1 = font_file_pointer;
            gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MID_CENTER);
        }
    }
    gui_obj_hidden((void *)window_select, 1);
    return this;
}
_GUI_API_ASSIGN(gui_combo_t)
.on_change = on_change,
 .get_current_item = current_item,
};
