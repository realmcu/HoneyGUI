#include "gui_button.h"
#include "gui_obj.h"
#include <tp_algo.h>
#include "gui_magic_img.h"
#include "gui_dynamic_img.h"

void gui_button_text_move(gui_button_t *this, int16_t text_x, int16_t text_y)
{
    this->text->base.x = text_x;
    this->text->base.y = text_y;
}
void gui_button_text_color(gui_button_t *this, uint32_t color)
{
    this->text->color = color;
}
void gui_button_img_move(gui_button_t *this, int16_t img_x, int16_t img_y)
{
    this->img->base.x = img_x;
    this->img->base.y = img_y;
}

void gui_button_click(gui_button_t *this, gui_event_cb_t event_cb)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
}
static void press_ani(gui_button_t *this)
{
    gui_img_scale((void *)(this->img), this->win->animate->progress_percent * (-0.5f) + 1.0f,
                  this->win->animate->progress_percent * (-0.5f) + 1.0f);
}
static void button_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();

    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {

        gui_button_t *b = (void *)obj;//gui_log("%d ",tp->type);
        switch (tp->type)
        {
        case TOUCH_SHORT:
            {
                // if (obj->callback.link_cb)
                {
                    if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                        (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                    {
                        gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
                    }
                }
            }
            break;
        case TOUCH_LONG:
            {
                if (b->long_flag == false)
                {

                    //if (b->long_click_cb)
                    {
                        if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                            (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                        {
                            b->long_flag = true;
                            //gui_send_callback_p_to_server(b->long_click_cb, b->long_click_cb_p);
                        }
                    }
                }
            }
            break;
        case TOUCH_INVALIDE:
            {
                if (b->press_flag == false)
                {

                    if (b->on_pic_addr)
                        // printf("tp->x %d, tp->y %d, obj->dx %d, obj->dy %d \n",tp->x,tp->y,obj->dx,obj->dy);
                    {
                        if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                            (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                        {
                            b->press_flag = true;
                            b->long_flag = false;
                            //gui_send_callback_p_to_server(b->press_cb, b->press_cb_p);
                            gui_img_set_attribute(b->img, b->img->base.name, b->on_pic_addr, b->img->base.x, b->img->base.y);
                        }
                    }
                }
            }
            break;
        case TOUCH_INIT:
            {
                if (b->press_flag)
                {
                    b->press_flag = false;
                    b->long_flag = false;
                    if (b->off_pic_addr)
                    {
                        //gui_send_callback_p_to_server(b->release_cb, b->release_cb_p);
                        //gui_img_set_attribute(b->img, b->img->base.name, b->off_pic_addr, b->img->base.x, b->img->base.y);
                    }

                    b->win->set_animate(b->win, 1000, 0, press_ani, b);
                }
            }
            break;
        default:
            break;
        }
    }
}

static void gui_button_ctor(
    gui_button_t *this,
    gui_obj_t *parent,
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    void *background_pic,
    void *highlight_pic,
    char *text
)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, "DEFAULT", x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = BUTTON;
    root->obj_prepare = button_prepare;

    // for self
    this->off_pic_addr = background_pic;
    this->on_pic_addr = highlight_pic;
}

gui_button_t *gui_button_create(
    void *parent,
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    void *background_pic,
    void *highlight_pic,
    char *text,
    char image_type,
    int count
)
{
    GUI_ASSERT(parent != NULL);
    gui_button_t *button = gui_malloc(sizeof(gui_button_t));
    GUI_ASSERT(button != NULL);
    memset(button, 0, sizeof(gui_button_t));

    gui_button_ctor(button, parent, x, y, w, h, background_pic, highlight_pic, text);
    gui_list_init(&(GET_BASE(button)->child_list));
    if ((GET_BASE(button)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(button)->parent)->child_list),
                               &(GET_BASE(button)->brother_list));
    }
    button->win = gui_win_create(button, "button_win", 0, 0, 0, 0);
    switch (image_type)
    {
    case 0:
        button->img = (void *)gui_magic_img_create_from_mem(button, "icon_img", background_pic, 0, 0, 0, 0);
        break;
    case 1:
        button->img = (void *)gui_dynamic_create_from_mem((void *)button, "g", background_pic, count, 0, 0,
                                                          30, INT32_MAX - 1);
        break;
    case 2:
        button->img = (void *)gui_svg_create_from_mem((void *)button, background_pic, count, 0, 0,
                                                      w, h);
        break;
    default:
        break;
    }

    if (text)
    {
        gui_img_get_height(button->img);
        button->text = gui_text_create(button, "icon_text", 0, button->img->draw_img.img_h + 40, w + 10, h);
        gui_text_set(button->text, text, "rtk_font_stb", BUTTON_COLOR, strlen(text),
                     RTK_GUI_DEFAULT_FONT_SIZE);
        gui_text_mode_set(button->text, LEFT);
    }

    GET_BASE(button)->create_done = true;
    return button;
}
