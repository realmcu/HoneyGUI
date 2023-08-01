#include "gui_button.h"
#include "gui_obj.h"
#include <tp_algo.h>
#include "gui_magic_img.h"
#include "gui_dynamic_img.h"
#include "draw_font.h"
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
void gui_button_press(gui_button_t *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_PRESSED, parameter);
}
void gui_button_long(gui_button_t *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_LONG, parameter);
}
void gui_button_release(gui_button_t *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_RELEASED, parameter);
}
static void (onPress)(gui_button_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_PRESSED, parameter);
}
static void (onRelease)(gui_button_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_RELEASED, parameter);
}
static void (onLong)(gui_button_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_LONG, parameter);
}
static void (onClick)(gui_button_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED, parameter);
}
static void button_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();

    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {
        if (tp->type != 271)
        {
            ////gui_log("type2:%d,%d\n", tp->type, tp->released);
        }
        gui_button_t *b = (void *)obj;
        switch (tp->type)
        {
        case TOUCH_SHORT:
            {
                //////gui_log("%s\n", "TOUCH_SHORT");
                //
//                bool callback = false;
                for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
                {
                    gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                    if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                    {
                        //callback = true;
                    }
                } ////gui_log("%d\n", __LINE__);
                //if (callback)
                {
                    ////gui_log("%d\n", __LINE__);
                    if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                        (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                    {
                        ////gui_log("%d\n", __LINE__);
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
                            gui_obj_event_set(obj, GUI_EVENT_TOUCH_LONG);
                        }
                    }
                }
            }
            break;

        default:
            break;
        }


        {
            if (tp->pressed)
            {

                if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                    (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                {

                    //gui_send_callback_p_to_server(b->press_cb, b->press_cb_p);
                    if (b->on_pic_addr && b->style == 0)
                    {
#ifdef RTK_GUI_SCRIPT_AS_A_APP
                        gui_img_set_attribute(b->img, b->img->base.name, b->on_pic_addr, b->img->base.x, b->img->base.y);
#endif
                    }
                    ////gui_log("%d\n", __LINE__);
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSED);  ////gui_log("%d\n", __LINE__);
                    b->long_flag = false;
                    b->press_flag = true;
                }
            }

            if (b->release_flag)
            {

                {
                    b->press_flag = false;
                    b->release_flag = false;
                    //gui_send_callback_p_to_server(b->press_cb, b->press_cb_p);
                    ////gui_log("%d\n", __LINE__);
                    if (b->off_pic_addr && b->style == 0)
                    {
#ifdef RTK_GUI_SCRIPT_AS_A_APP
                        gui_img_set_attribute(b->img, b->img->base.name, b->off_pic_addr, b->img->base.x, b->img->base.y);
#endif
                    }
                    // bool callback = false;
                    for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
                    {
                        gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                        if (event_dsc->filter == GUI_EVENT_TOUCH_RELEASED)
                        {
                            // callback = true;
                        }
                    }
                    //if (callback)
                    {
                        gui_obj_event_set(obj, GUI_EVENT_TOUCH_RELEASED);
                    }  ////gui_log("%d\n", __LINE__);
                    b->long_flag = false;
                }
            }
            if (tp->released && b->press_flag)
            {
                b->release_flag = true;
            }
        }

    }
}
static void (obj_update_att)(struct _gui_obj_t *o)
{
    gui_button_t *obj = (void *)o;
    if (obj->animate && obj->animate->animate)
    {
        size_t frame_count = obj->animate->dur * (1000 / 15) / (1000);
        obj->animate->callback(obj->animate->p);
        obj->animate->current_frame++;

        if (obj->animate->current_frame > frame_count)
        {
            if (obj->animate->repeatCount == 0)
            {
                obj->animate->animate = false;
            }
            else if (obj->animate->repeatCount < 0)
            {
                obj->animate->current_frame = 0;
            }
            else if (obj->animate->repeatCount > 0)
            {
                obj->animate->current_repeat_count++;
                if (obj->animate->current_repeat_count >= obj->animate->repeatCount)
                {
                    obj->animate->animate = false;
                }
                else
                {
                    obj->animate->current_frame = 0;
                }
            }
        }
        obj->animate->progress_percent = ((float)(obj->animate->current_frame)) / ((float)(
                                                                                       frame_count));

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
    root->obj_update_att = obj_update_att;

    // for self
    this->off_pic_addr = background_pic;
    this->on_pic_addr = highlight_pic;
}

static void set_animate(gui_button_t *o, uint32_t dur, int repeatCount, void *callback, void *p)
{
    gui_animate_t *animate = ((gui_button_t *)o)->animate;
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }
    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *))callback;
    animate->repeatCount = repeatCount;
    animate->p = p;
    ((gui_button_t *)o)->animate = animate;
}
gui_api_button_t gui_button_api =
{
    .onClick = onClick,
    .onLong = onLong,
    .onPress = onPress,
    .onRelease = onRelease,
    .set_animate = set_animate,
};
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
    if (background_pic)
    {
        /* code */


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
            //button->img = (void *)gui_svg_create_from_mem((void *)button, background_pic, count, 0, 0, w, h);
            break;
        default:
            break;
        }
    }
    if (text)
    {
        gui_img_get_height(button->img);
#ifdef RTK_GUI_SCRIPT_AS_A_APP
        gui_set_font_mem_resourse(RTK_GUI_DEFAULT_FONT_SIZE,
                                  gui_get_file_address("app/system/resource/font/gbk_32_32_dot.bin"),
                                  gui_get_file_address("app/system/resource/font/gbk_unicode_table.bin"));
#endif
        button->text = gui_text_create(button, "icon_text", 0, button->img->draw_img.img_h + 40, w + 10, h);
        gui_text_set(button->text, text, "rtk_font_mem", BUTTON_COLOR, strlen(text),
                     RTK_GUI_DEFAULT_FONT_SIZE);
        gui_text_mode_set(button->text, LEFT);
    }

    GET_BASE(button)->create_done = true;
    return button;
}
