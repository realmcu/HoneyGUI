#include "gui_combo.h"
#include "gui_button.h"
#include "gui_switch.h"
#include "gui_canvas_rect.h"

static void input_prepare()
{

}
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
        case OBJ_INPUT_PREPARE:
            input_prepare();
            break;
        case OBJ_PREPARE:
            prepare(obj);
            break;
        default:
            break;
        }
    }
}




static void ctor(gui_combo_t *widget, gui_obj_t *parent, int x, int y, int w, int item_count,
                 int item_height, const char *items[], const uint8_t *item_background_image,
                 const uint8_t *item_background_highlight_image,
                 const uint8_t *item_background_selection_image,
                 gui_color_t font_color,

                 gui_color_t font_color_selection,
                 gui_color_t font_color_highlight,
                 const uint8_t *font_file_pointer,
                 const uint8_t *font_file_pointer_selection
                )
{
    gui_obj_ctor(GUI_BASE(widget), parent, "COMBO", x, y, w, item_height * (item_count + 1));
    GET_BASE(widget)->type = MACRO_COMBO;
    GET_BASE(widget)->obj_cb = pipeline;
    GET_BASE(widget)->has_prepare_cb = true;
    GET_BASE(widget)->has_input_prepare_cb = true;
    widget->fold = 1;
    widget->items = items;
    widget->item_background_highlight_image = item_background_highlight_image;
    widget->item_background_image = item_background_image;
    widget->item_background_selection_image = item_background_selection_image;
    widget->font_color = font_color;
    widget->font_color_highlight = font_color_highlight;
    widget->font_color_selection = font_color_selection;
    widget->font = font_file_pointer;
    widget->font_selection = font_file_pointer_selection;
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
            extern void gui_obj_tree_get_widget_by_type_and_index(gui_obj_t *root, T_OBJ_TYPE type,
                                                                  gui_obj_t **output, int index);
            gui_button_t *last_selection = 0;
            gui_obj_tree_get_widget_by_type_and_index((void *)combo, BUTTON, (void *)&last_selection,
                                                      combo->current_item);
            last_selection->off_pic_addr = (void *)combo->item_background_image;
            last_selection->img->data = last_selection->off_pic_addr;

            gui_text_content_set(combo->button_selector_text, (void *)combo->items[(size_t)param],
                                 strlen(combo->items[(size_t)param]));

            GUI_TYPE(gui_button_t, button)->off_pic_addr = (void *)combo->item_background_selection_image;
            GUI_TYPE(gui_button_t, button)->img->data = GUI_TYPE(gui_button_t, button)->off_pic_addr;
            _gui_api_for_gui_switch_t.turn_off(combo->button_selector);
            {
                gui_text_t *last_text = 0;
                gui_obj_tree_get_widget_by_type_and_index((void *)combo, TEXTBOX, (void *)&last_text,
                                                          combo->current_item + 1);
                last_text->color = combo->font_color;
                last_text->path = (void *)combo->font;
                last_text->char_width_sum = 0;
                last_text->char_line_sum = 0;
                last_text->content_refresh = true;
                last_text->layout_refresh = true;
            }
            {
                gui_text_t *last_text = 0;
                gui_obj_tree_get_widget_by_type_and_index((void *)combo, TEXTBOX, (void *)&last_text,
                                                          (size_t)param + 1);
                last_text->color = combo->font_color_selection;
                last_text->path = (void *)combo->font_selection;
                last_text->char_width_sum = 0;
                last_text->char_line_sum = 0;
                last_text->content_refresh = true;
                last_text->layout_refresh = true;
            }
            combo->current_item = (size_t)param;
            combo->on_change_function ? combo->on_change_function(combo, (gui_event_t)0,
                                                                  combo->on_change_function_parameter) : (void)0;

        }
        break;
    default:
        break;
    }

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
gui_combo_t *gui_combo_create(gui_obj_t *parent,
                              int x,
                              int y,
                              int w,
                              int item_count,
                              int item_height,
                              const char *items[],
                              const uint8_t *item_background_image,
                              const uint8_t *item_background_highlight_image,
                              const uint8_t *item_background_selection_image,
                              const uint8_t *selector_background_image,
                              const uint8_t *selector_background_highlight_image,
                              const uint8_t *selector_background_collapsed_image,
                              const uint8_t *selector_background_collapsed_highlight_image,
                              gui_color_t background_color,
                              int font_size,
                              gui_color_t font_color,
                              const uint8_t *font_file_pointer,
                              int font_size_selection,
                              gui_color_t font_color_selection,
                              const uint8_t *font_file_pointer_selection,
                              gui_color_t font_color_highlight,
                              BLEND_MODE_TYPE image_blend_mode
                             )
{
#define _GUI_NEW_GUI_COMBO_PARAM this, parent, x, y, w, item_count, item_height, items, item_background_image, item_background_highlight_image, item_background_selection_image,font_color,font_color_selection,font_color_highlight, font_file_pointer, font_file_pointer_selection
    GUI_CREATE_HELPER(gui_combo_t, ctor, _GUI_NEW_GUI_COMBO_PARAM)
    // gui_button_t *button_selector = gui_button_create(this, 0, 0, w, item_height,
    //                                                   item_background_image, item_background_highlight_image, 0, 0, 0);
    // GUI_API(gui_button_t).on_click(button_selector, selector_button_cb, this);

    gui_switch_t *button_selector = gui_switch_create(this, 0, 0, w, item_height,
                                                      (void *)selector_background_collapsed_image, (void *)selector_background_image);
    button_selector->on_hl_pic_addr = (void *)selector_background_highlight_image;
    button_selector->off_hl_pic_addr = (void *)selector_background_collapsed_highlight_image;
    GUI_API(gui_switch_t).on_turn_on(button_selector, selector_button_cb, this);
    GUI_API(gui_switch_t).on_turn_off(button_selector, selector_button_cb, this);
    gui_img_set_mode(button_selector->switch_picture, image_blend_mode);
    this->button_selector = button_selector;
    {
        const char *text = items[0];
        gui_text_t *t = gui_text_create(button_selector, text, 0, 0, w, item_height);
        gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, font_color, strlen(text), font_size);
        const void *addr1 = font_file_pointer;
        gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
        this->button_selector_text = t;
    }
    int vertical_gap = 8;
    gui_win_t *window_select = gui_win_create(this, 0, 0, item_height + vertical_gap, w,
                                              item_height * item_count);
    this->window_select = window_select;
    gui_canvas_rect_create((void *)window_select, 0, 0, 0, w, item_height * item_count,
                           background_color);
    for (size_t i = 0; i < item_count; i++)
    {
        gui_button_t *button_selector = gui_button_create(window_select, 0, (i) * item_height, w,
                                                          item_height, (void *)item_background_image, (void *)item_background_highlight_image, 0,
                                                          (T_BUTTON_BG_TYPE)0, 0);
        GUI_API(gui_button_t).on_click(button_selector, item_button_cb, (void *)i);
        gui_button_img_move(button_selector, vertical_gap / 2, vertical_gap / 2);
        gui_img_set_mode(button_selector->img, image_blend_mode);
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

_gui_api_gui_combo_t _gui_api_for_gui_combo_t =
{
    .on_change = on_change,
    .get_current_item = current_item,
};
