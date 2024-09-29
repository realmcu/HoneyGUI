
#include "js_user.h"
#ifdef __WIN32
#include "unistd.h"
#include <semaphore.h>
sem_t sem_timer;
#endif

jerry_value_t jerry_call_func_sem(const jerry_value_t func_obj_val,  /**< function object to call */
                                  const jerry_value_t this_val, /**< object for 'this' binding */
                                  const jerry_value_t args_p[], /**< function's call arguments */
                                  jerry_size_t args_count) /**< number of the arguments */
{
#ifdef __WIN32
    sem_wait(&sem_timer);
#endif

    jerry_value_t res = jerry_call_function(func_obj_val, this_val, args_p, args_count);

#ifdef __WIN32
    sem_post(&sem_timer);
#endif
    return res;
}

static void js_cb_with_args(gui_obj_t *obj, gui_event_t event_code, void *param)
{
    //gui_log("enter js_cb_with_args\n");
    cb_arg_t *args = param;
    gui_event_dsc_t *event = obj->event_dsc;

    if (args == NULL)
    {
        return;
    }
    jerry_value_t res = jerry_call_func_sem(args->func, jerry_create_undefined(), args->args_p,
                                            args->args_count);
    jerry_release_value(res);
}

static void js_cb_with_args_animate(cb_arg_t *args)
{
    //gui_log("enter js_cb_with_args_animate\n");
    if (args == NULL)
    {
        return;
    }
    jerry_value_t res = jerry_call_func_sem(args->func, jerry_create_undefined(), args->args_p,
                                            args->args_count);
    jerry_release_value(res);

}

static void screen_reload()
{
    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t app_property = js_get_property(global_obj, "app");
    gui_app_t *app = NULL;
    jerry_get_object_native_pointer(app_property, (void *)&app, NULL);
    jerry_release_value(global_obj);
    jerry_release_value(app_property);
}

DECLARE_HANDLER(write)
{
    if (args_cnt != 1 || !jerry_value_is_string(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_text_t *txtbox = NULL;
    jerry_get_object_native_pointer(this_value, (void *) &txtbox, NULL);
    if (txtbox)
    {
        jerry_value_t s = jerry_value_to_string(args[0]);////gui_log("jerryenter1\n");
        jerry_length_t char_length = jerry_get_utf8_string_length(s); ////gui_log("jerryenter2\n");
        jerry_size_t byte_size = jerry_get_utf8_string_size(s);
        jerry_char_t *strbuf1 = gui_malloc(byte_size + 1); ////gui_log("jerryenter3\n");
        jerry_string_to_utf8_char_buffer(s, strbuf1, byte_size + 1); ////gui_log("jerryenter4\n");
        strbuf1[byte_size] = '\0';
        gui_free(txtbox->content);
        gui_text_content_set(txtbox, (void *)strbuf1, byte_size);
        jerry_release_value(s);
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(setInputable)
{
    if (args_cnt != 1 || !jerry_value_is_boolean(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_text_t *txtbox = NULL;
    jerry_get_object_native_pointer(this_value, (void *) &txtbox, NULL);
    if (txtbox)
    {
        bool enable = jerry_get_boolean_value(args[0]);
        gui_text_input_set(txtbox, enable);
        if (enable)
        {
            gui_text_click(txtbox, gui_keyboard_launch_by_widget, txtbox);
        }
        // jerry_release_value(enable);
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(setPosition)
{
    if (args_cnt != 1 || !jerry_value_is_object(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_text_t *txtbox = NULL;
    jerry_get_object_native_pointer(this_value, (void *) &txtbox, NULL);
    if (txtbox)
    {
        int x = jerry_get_number_value(js_get_property(args[0], "x"));
        int y = jerry_get_number_value(js_get_property(args[0], "y"));
        // gui_log("setPosition %d %d\n", x, y);
        gui_text_move(txtbox, x, y);
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(jump)
{
    // gui_log("enter jump\n");
    if (args_cnt != 1 || !jerry_value_is_number(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_obj_t *obj = NULL;
    jerry_get_object_native_pointer(this_value, (void *) &obj, NULL);
    ////gui_log("jerry_get_object_native_pointer = %s",txtbox->base.name);
    if (obj)
    {
        // gui_log("jump %f\n", jerry_get_number_value(args[0]));
        gui_tabview_jump_tab((void *)obj, jerry_get_number_value(args[0]), 0);
    }
    return jerry_create_undefined();
}


DECLARE_HANDLER(getElementById)
{
    if (args_cnt != 1 || !jerry_value_is_string(args[0]))
    {
        return jerry_create_undefined();
    }
    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t app_property = js_get_property(global_obj, "app");
    gui_app_t *app = NULL;
    jerry_get_object_native_pointer(app_property, (void *)&app, NULL);
    gui_obj_t *widget = NULL;
    char *a = js_value_to_string(args[0]);
    gui_obj_tree_get_widget_by_name(&app->screen, a, &widget);
    gui_free(a);
    jerry_set_object_native_pointer(this_value, widget, NULL);
    jerry_release_value(global_obj);
    jerry_release_value(app_property);
    return jerry_create_undefined();
}

DECLARE_HANDLER(getAttribute)
{
    if (args_cnt != 1 || !jerry_value_is_string(args[0]))
    {
        return jerry_create_undefined();
    }
    ezxml_t title;
    jerry_get_object_native_pointer(this_value, (void *)&title, NULL);
    char *a = js_value_to_string(args[0]);
    const char *widget_type = ezxml_attr(title, a);
    gui_free(a);
    return jerry_create_string((void *)widget_type);
}

extern const char *get_tag_by_widget_type(int type);

extern ezxml_t get_child_ele(char *xml, char *ele, char *txt, char *chile_ele);
DECLARE_HANDLER(getChildElementByTag)
{
    if (args_cnt != 1 || !jerry_value_is_string(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_obj_t *widget = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&widget, NULL);
    char *name = (void *)widget->name;
    int type = widget->type;
    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t app_property = js_get_property(global_obj, "app");
    gui_app_t *app = NULL;
    jerry_get_object_native_pointer(app_property, (void *)&app, NULL);
    char *widget_type = (void *)get_tag_by_widget_type(type);
    ezxml_t c = get_child_ele((void *)app->xml, widget_type, name, js_value_to_string(args[0]));
    jerry_value_t v = jerry_create_object();
    jerry_set_object_native_pointer(v, c, NULL);
    REGISTER_METHOD(v, getAttribute);
    return v;
}
DECLARE_HANDLER(icon_write)
{
    if (args_cnt != 1 || !jerry_value_is_string(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_button_t *btn = NULL;
    jerry_get_object_native_pointer(this_value, (void *) &btn, NULL);
    if (btn)
    {
        // char *strbuf1 = js_value_to_string(args[0]);
        // gui_text_set(btn->text, strbuf1, GUI_FONT_SRC_TTF, btn->text->color, strlen(strbuf1),
        //              btn->text->font_height);
        // //jerry_release_value(s);
        // jerry_value_t global_obj = jerry_get_global_object();
        // jerry_value_t app_property = js_get_property(global_obj, "app");
        // gui_app_t *app = NULL;
        // jerry_get_object_native_pointer(app_property, (void *)&app, NULL);
        // jerry_release_value(global_obj);
        // jerry_release_value(app_property);
        //gui_exec(app->screen);

        jerry_value_t s = jerry_value_to_string(args[0]);////gui_log("jerryenter1\n");
        jerry_length_t char_length = jerry_get_utf8_string_length(s); ////gui_log("jerryenter2\n");
        jerry_size_t byte_size = jerry_get_utf8_string_size(s);
        jerry_char_t *strbuf1 = gui_malloc(byte_size + 1); ////gui_log("jerryenter3\n");
        jerry_string_to_utf8_char_buffer(s, strbuf1, byte_size + 1); ////gui_log("jerryenter4\n");
        strbuf1[byte_size] = '\0';
        gui_free(btn->text->content);
        gui_text_content_set(btn->text, (void *)strbuf1, byte_size);
        jerry_release_value(s);
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(icon_txtPos)
{
    if (args_cnt != 1 || !jerry_value_is_object(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_button_t *btn = NULL;
    jerry_get_object_native_pointer(this_value, (void *) &btn, NULL);
    if (btn)
    {
        int x = jerry_get_number_value(js_get_property(args[0], "x"));
        int y = jerry_get_number_value(js_get_property(args[0], "y"));
        // gui_log("setPosition %d %d\n", x, y);
        gui_button_text_move(btn, x, y);
    }
    return jerry_create_undefined();
}

DECLARE_HANDLER(icon_setEnable)
{
    if (args_cnt == 1 || jerry_value_is_number(args[0]))
    {
        gui_button_t *btn = NULL;
        jerry_get_object_native_pointer(this_value, (void *) &btn, NULL);
        if (btn)
        {
            bool enable = (bool)jerry_get_number_value(args[0]);

            gui_button_set_enable(btn, enable);
        }
    }
    return jerry_create_undefined();
}

DECLARE_HANDLER(icon_setImg)
{
#if 0
    if (args_cnt != 2 || !jerry_value_is_string(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_img_t *img = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&img, NULL);
    ////gui_log("jerry_get_object_native_pointer = %s",img->base.name);
    if (img)
    {
        jerry_value_t s = jerry_value_to_string(args[0]);
        jerry_length_t length = jerry_get_utf8_string_length(s);

        jerry_char_t *path_buf = gui_malloc(length + 1);
        jerry_string_to_utf8_char_buffer(s, path_buf, length + 1);
        path_buf[length] = '\0';
        void *img_addr = gui_get_file_address((const char *)path_buf);
        gui_free(path_buf);

        if (img_addr)
        {
            gui_img_set_attribute(img, img->base.name, img_addr, img->base.x, img->base.y);
        }
        jerry_release_value(s);
        gui_fb_change();
    }
#endif
    return jerry_create_undefined();
}
DECLARE_HANDLER(reload)
{
    gui_app_t *app = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&app, NULL);
    return jerry_create_undefined();
}
DECLARE_HANDLER(close)
{
    gui_app_t *app = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&app, NULL);
    gui_app_shutdown(app);
    memset(app, 0, sizeof(gui_app_t));
    return jerry_create_undefined();
}

DECLARE_HANDLER(open)
{
    if (args_cnt >= 1 && jerry_value_is_string(args[0]))
    {
        gui_app_t *app = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&app, NULL);

        char *new_xml = js_value_to_string(args[0]);
        extern void *get_app_xml(void);
        gui_app_t *app2 = (gui_app_t *)get_app_xml();
        app2->xml = new_xml;
        gui_switch_app(app, app2);
    }
    return jerry_create_undefined();
}


DECLARE_HANDLER(show)
{
    gui_obj_t *obj = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
    obj->not_show = false;
    screen_reload();
    return jerry_create_undefined();
}
DECLARE_HANDLER(notShow)
{
    gui_obj_t *obj = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
    obj->not_show = true;
    screen_reload();
    return jerry_create_undefined();
}

DECLARE_HANDLER(setLocation)
{
    if (args_cnt != 2 || !jerry_value_is_number(args[0]) || !jerry_value_is_number(args[1]))
    {
        return jerry_create_undefined();
    }
    gui_obj_t *obj = NULL;
    float x = 0.f;
    float y = 0.f;

    jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
    x = jerry_get_number_value(args[0]);
    y = jerry_get_number_value(args[1]);

    if (obj)
    {
        obj->x = x;
        obj->y = y;
    }
    return jerry_create_undefined();
}

DECLARE_HANDLER(rotation)
{
    if (args_cnt != 3 || !jerry_value_is_number(args[0]) || !jerry_value_is_number(args[1]) ||
        !jerry_value_is_number(args[2]))
    {
        return jerry_create_undefined();
    }
    gui_img_t *img = NULL;
    float degree = 0.f;
    float c_x = 0.f;
    float c_y = 0.f;

    jerry_get_object_native_pointer(this_value, (void *)&img, NULL);
    degree = jerry_get_number_value(args[0]);
    c_x = jerry_get_number_value(args[1]);
    c_y = jerry_get_number_value(args[2]);

    if (img)
    {
        gui_img_rotation((void *)img, degree, c_x, c_y);
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(scale)
{
    if (args_cnt != 2 || !jerry_value_is_number(args[0]) || !jerry_value_is_number(args[1]))
    {
        return jerry_create_undefined();
    }
    gui_img_t *img = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&img, NULL);
    if (img)
    {
        gui_img_scale((void *)img, jerry_get_number_value(args[0]),
                      jerry_get_number_value(args[1]));
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(setMode)
{
    if (args_cnt != 1 || !jerry_value_is_number(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_img_t *img = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&img, NULL);
    if (img)
    {
        gui_img_set_mode(img, jerry_get_number_value(args[0]));
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(setShow)
{
    if (args_cnt != 1 || !jerry_value_is_number(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_obj_t *obj = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
    if (obj)
    {
        bool show = (bool)jerry_get_number_value(args[0]);
        gui_obj_show(obj, show);
        gui_fb_change();
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(setImg)
{
    if (args_cnt != 1 || !jerry_value_is_string(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_img_t *img = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&img, NULL);
    ////gui_log("jerry_get_object_native_pointer = %s",img->base.name);
    if (img)
    {
        jerry_value_t s = jerry_value_to_string(args[0]);
        jerry_length_t length = jerry_get_utf8_string_length(s);

        jerry_char_t *path_buf = gui_malloc(length + 1);
        jerry_string_to_utf8_char_buffer(s, path_buf, length + 1);
        path_buf[length] = '\0';
        void *img_addr = gui_get_file_address((const char *)path_buf);
        gui_free(path_buf);

        if (img_addr)
        {
            gui_img_set_attribute(img, img->base.name, img_addr, img->base.x, img->base.y);
        }
        jerry_release_value(s);
        gui_fb_change();
    }
    return jerry_create_undefined();
}

DECLARE_HANDLER(setRebound)
{
    if (args_cnt != 1 || !jerry_value_is_boolean(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_page_t *page = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&page, NULL);
    if (page)
    {
        gui_page_rebound(page, jerry_get_boolean_value(args[0]));
    }
    return jerry_create_undefined();
}

DECLARE_HANDLER(onClick)
{
    //////gui_log("enter onclick\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = args[i + 1];
        }

        cb_arg->func = args[0];
        gui_button_click((void *)obj, (gui_event_cb_t)js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onPress)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_button_press((void *)obj, (gui_event_cb_t)js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onHold)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_button_long((void *)obj, (gui_event_cb_t)js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}


DECLARE_HANDLER(onClick_text)
{
    //////gui_log("enter onclick\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = args[i + 1];
        }

        cb_arg->func = args[0];
        gui_text_click((void *)obj, (gui_event_cb_t)js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
// get passwd from kb
DECLARE_HANDLER(setPasswd)
{
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        gui_text_t *txt = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&txt, NULL);

        if (txt)
        {
            txt->ispasswd = true;
            cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
            memset(cb_arg, 0, sizeof(cb_arg_t));
            cb_arg->args_count = args_cnt - 1;
            if (cb_arg->args_count)
            {
                cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
            }
            for (size_t i = 0; i < cb_arg->args_count; i++)
            {
                cb_arg->args_p[i] = args[i + 1];
            }

            cb_arg->func = args[0];
            gui_text_pswd_done((void *)txt, (gui_event_cb_t)js_cb_with_args, (void *)(cb_arg));
        }
    }

    return jerry_create_undefined();
}


DECLARE_HANDLER(onRelease_win)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_win_release((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onClick_win)
{
    //////gui_log("enter onclick\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }

        cb_arg->func = args[0];
        gui_win_click((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onPress_win)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_win_press((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onPress_switch)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_obj_add_event_cb((void *)obj, (gui_event_cb_t)js_cb_with_args, GUI_EVENT_TOUCH_PRESSED,
                             (void *)(cb_arg));

    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onHold_win)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_win_long((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}



DECLARE_HANDLER(onLeft_win)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_win_left((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onRight_win)
{
    //////gui_log("enter onclick\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }

        cb_arg->func = args[0];
        gui_win_right((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onUp_win)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *) &obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_win_up((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onDown_win)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_win_down((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(onRelease)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_button_release((void *)obj, (gui_event_cb_t)js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(switch_on)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = args[i + 1];
        }
        cb_arg->func = args[0];
        GUI_API(gui_switch_t).on_turn_on((void *)obj, (gui_event_cb_t)js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(switch_off)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = args[i + 1];
        }
        cb_arg->func = args[0];
        GUI_API(gui_switch_t).on_turn_off((void *)obj, (gui_event_cb_t)js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}

// tabview
DECLARE_HANDLER(OnChange)
{
    gui_log("enter OnChange\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = args[i + 1];
            // gui_log("param: %u", cb_arg->args_p[i]);
        }
        cb_arg->func = args[0];
        gui_tabview_tab_change((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }
    return jerry_create_undefined();
}
// tabview
DECLARE_HANDLER(getCurTab)
{
    gui_obj_t *obj = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
    jerry_value_t x = GUI_TYPE(gui_tabview_t, obj)->cur_id.x;
    jerry_value_t y = GUI_TYPE(gui_tabview_t, obj)->cur_id.y;

    jerry_value_t tab_cur_id = jerry_create_object();
    js_set_property(tab_cur_id, "x", jerry_create_number((double) x));
    js_set_property(tab_cur_id, "y", jerry_create_number((double) y));
    return tab_cur_id;
}


#include "gui_seekbar.h"
DECLARE_HANDLER(onpress_seekbar)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_obj_add_event_cb(obj, (gui_event_cb_t)js_cb_with_args, GUI_EVENT_1, cb_arg);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onpressing_seekbar)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_obj_add_event_cb(obj, (gui_event_cb_t)js_cb_with_args, GUI_EVENT_3, cb_arg);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onrelease_seekbar)
{
    ////gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //////gui_log("enter onclick %s\n",obj->name);

        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        ////gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];
        gui_obj_add_event_cb(obj, (gui_event_cb_t)js_cb_with_args, GUI_EVENT_2, cb_arg);
    }

    return jerry_create_undefined();
}
static void gui_seekbar_set_animate(gui_seekbar_t *o, uint32_t dur, int repeat_count,
                                    void *callback,
                                    void *p)
{
    gui_animate_t *animate = ((gui_seekbar_t *)o)->animate;
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }
    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (gui_animate_callback_t)callback;
    animate->repeat_count = repeat_count;
    animate->p = p;
    ((gui_seekbar_t *)o)->animate = animate;
}
DECLARE_HANDLER(play_animate_seekbar)
{
    //gui_log("enter play_animate_seekbar\n");

    {
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //js_add_event_listener(this_value, "onclick", args[0]);
        GUI_TYPE(gui_seekbar_t, obj)->animate->animate = true;
        GUI_TYPE(gui_seekbar_t, obj)->animate->current_frame = 0;
        GUI_TYPE(gui_seekbar_t, obj)->animate->progress_percent = 0;
        GUI_TYPE(gui_seekbar_t, obj)->animate->current_repeat_count = 0;
        GUI_TYPE(gui_seekbar_t, obj)->animate->init = false;
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(play_animate_text)
{
    //gui_log("enter play_animate_text\n");

    {
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //js_add_event_listener(this_value, "onclick", args[0]);
        GUI_TYPE(gui_text_t, obj)->animate->animate = true;
        GUI_TYPE(gui_text_t, obj)->animate->current_frame = 0;
        GUI_TYPE(gui_text_t, obj)->animate->progress_percent = 0;
        GUI_TYPE(gui_text_t, obj)->animate->current_repeat_count = 0;
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(setAnimate_seekbar)
{
    //gui_log("enter setAnimate_seekbar\n");
    if (args_cnt >= 2 && jerry_value_is_function(args[0]) && jerry_value_is_object(args[1]))
    {
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //js_add_event_listener(this_value, "onclick", args[0]);
        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        //gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];

        float from = jerry_get_number_value(js_get_property(args[1], "from"));
        float to = jerry_get_number_value(js_get_property(args[1], "to"));
        int repeat = jerry_get_number_value(js_get_property(args[1], "iterations"));
        int duration = jerry_get_number_value(js_get_property(args[1], "duration"));
        gui_seekbar_set_animate((void *)obj, duration, repeat, js_cb_with_args_animate,
                                (void *)(cb_arg));
        GUI_TYPE(gui_seekbar_t, obj)->animate->animate = false;
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(setAnimate_text)
{
    //gui_log("enter setAnimate_text\n");
    if (args_cnt >= 2 && jerry_value_is_function(args[0]) && jerry_value_is_object(args[1]))
    {
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //js_add_event_listener(this_value, "onclick", args[0]);
        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        //gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];

        float from = jerry_get_number_value(js_get_property(args[1], "from"));
        float to = jerry_get_number_value(js_get_property(args[1], "to"));
        int repeat = jerry_get_number_value(js_get_property(args[1], "iterations"));
        int duration = jerry_get_number_value(js_get_property(args[1], "duration"));
        gui_text_set_animate((void *)obj, duration, repeat, js_cb_with_args_animate,
                             (void *)(cb_arg));
        GUI_TYPE(gui_text_t, obj)->animate->animate = false;
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(pause_animate_text)
{
    //gui_log("enter pause_animate_text\n");

    gui_obj_t *obj = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
    GUI_TYPE(gui_text_t, obj)->animate->animate = false;

    return jerry_create_undefined();
}
DECLARE_HANDLER(pause_animate_img)
{
    gui_img_t *obj = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
    obj->animate->animate = false;

    return jerry_create_undefined();
}
DECLARE_HANDLER(play_animate_img)
{
    gui_img_t *obj = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
    obj->animate->animate = true;
    obj->animate->current_frame = 0;
    obj->animate->progress_percent = 0;
    obj->animate->current_repeat_count = 0;

    return jerry_create_undefined();
}
DECLARE_HANDLER(setAnimate_img)
{
    if (args_cnt >= 2 && jerry_value_is_function(args[0]) && jerry_value_is_object(args[1]))
    {
        gui_img_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        //js_add_event_listener(this_value, "onclick", args[0]);
        cb_arg_t *cb_arg = gui_malloc(sizeof(cb_arg_t));
        //gui_log("cb_arg:%x\n", cb_arg);
        memset(cb_arg, 0, sizeof(cb_arg_t));
        cb_arg->args_count = args_cnt - 1;
        if (cb_arg->args_count)
        {
            cb_arg->args_p = gui_malloc(sizeof(jerry_value_t) * cb_arg->args_count);
        }
        for (size_t i = 0; i < cb_arg->args_count; i++)
        {
            cb_arg->args_p[i] = js_string_to_value(js_value_to_string(args[i + 1]));
        }
        cb_arg->func = args[0];

        float from = jerry_get_number_value(js_get_property(args[1], "from"));
        float to = jerry_get_number_value(js_get_property(args[1], "to"));
        int repeat = jerry_get_number_value(js_get_property(args[1], "iterations"));
        int duration = jerry_get_number_value(js_get_property(args[1], "duration"));
        gui_img_set_animate((void *)obj, duration, repeat, js_cb_with_args_animate,
                            (void *)(cb_arg));
        obj->animate->animate = false;
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(setBlock)
{
    if (args_cnt == 1 && jerry_value_is_number(args[0]))
    {
        gui_img_t *img = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&img, NULL);

        if (img)
        {
            bool block = (bool)jerry_get_number_value(args[0]);
            gui_img_set_tp_block(img, block);
        }
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(win_getAttribute)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_string(args[0]))
    {
        char *a = js_value_to_string(args[0]);
        if (strcmp(a, "hidden") == 0)
        {
            gui_obj_t *obj = NULL;
            jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
            if (obj->not_show)
            {
                return js_string_to_value("hidden");
            }
            else
            {
                return jerry_create_undefined();
            }


        }
        gui_free(a);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(seekbar_getAttribute)
{
    //gui_log("enter seekbar_getAttribute1\n");
    if (args_cnt >= 1 && jerry_value_is_string(args[0]))
    {
        char *a = js_value_to_string(args[0]);
        if (strcmp(a, "animate") == 0)
        {
            gui_obj_t *obj = NULL;
            jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
            jerry_value_t v = jerry_create_object();
            js_set_property(v, "progress", jerry_create_number(GUI_TYPE(gui_seekbar_t,
                                                                        obj)->animate->progress_percent));
            gui_free(a);
            return v;

        }
        gui_free(a);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(seekbar_getAnimateProgress)
{
    gui_obj_t *obj = NULL;
    float percent = 0;
    jerry_get_object_native_pointer(this_value, (void *)&obj,
                                    NULL);
    percent = GUI_TYPE(gui_seekbar_t, obj)->animate->progress_percent;
    // gui_log("seekbar_getAnimateProgress %f\n", percent);
    return jerry_create_number(percent);

}
DECLARE_HANDLER(seekbar_setAttribute)
{
    //gui_log("enter seekbar_setAttribute\n");
    if (args_cnt >= 2 && jerry_value_is_string(args[0]) && jerry_value_is_number(args[1]))
    {
        char *a = js_value_to_string(args[0]);
        if (strcmp(a, "progress") == 0)
        {
            gui_obj_t *obj = NULL;
            jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
            // gui_log("seekbar_setAttribute:%f\n", jerry_get_number_value(args[1]));
            gui_progressbar_set_percentage((void *)obj, jerry_get_number_value(args[1]));

        }
        gui_free(a);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(win_setAttribute)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_string(args[0]))
    {
        char *a = js_value_to_string(args[0]);
        if (strcmp(a, "hidden") == 0)
        {
            gui_obj_t *obj = NULL;
            jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
            if (obj)
            {
                (obj->not_show) = true;
                gui_fb_change();
            }
        }
        gui_free(a);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(win_removeAttribute)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_string(args[0]))
    {
        char *a = js_value_to_string(args[0]);
        if (strcmp(a, "hidden") == 0)
        {
            gui_obj_t *obj = NULL;
            jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
            if (obj)
            {
                obj->not_show = false;
                gui_fb_change();
            }
        }
        gui_free(a);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(progress)
{
    gui_obj_t *obj = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
    if (args_cnt >= 1 && jerry_value_is_number(args[0]))
    {
        gui_progressbar_set_percentage((void *)obj, jerry_get_number_value(args[0]));
    }
    float per = gui_progressbar_get_percentage((void *)obj);
    return jerry_create_number(per);
}
DECLARE_HANDLER(sw_close)
{
    //gui_log("enter sw_close\n");
    gui_switch_t *sw = NULL;
    jerry_get_object_native_pointer(this_value, (void *)(&sw), NULL);
    GUI_API(gui_switch_t).turn_off(sw);
    return jerry_create_undefined();
}

DECLARE_HANDLER(sw_open)
{
    //gui_log("enter sw_open\n");
    gui_switch_t *sw = NULL;
    jerry_get_object_native_pointer(this_value, (void *)(&sw), NULL);
    GUI_API(gui_switch_t).turn_on(sw);
    return jerry_create_undefined();
}

DECLARE_HANDLER(sw_setState)
{
    if (args_cnt >= 1 && jerry_value_is_number(args[0]))
    {
        gui_switch_t *sw = NULL;
        double val = jerry_get_number_value(args[0]);

        jerry_get_object_native_pointer(this_value, (void *)(&sw), NULL);
        if (sw)
        {
            gui_switch_change_state(sw, (bool)val);
        }
    }

    return jerry_create_undefined();
}


#include "js_extern_io.h"
void gui_extern_event_timer_handler(gui_msg_js_t *js_msg)
{
    gui_msg_t msg;
    jerry_value_t js_cb = 0;
    jerry_value_t res = 0;

    memset(&msg, 0, sizeof(gui_msg_t));
    memcpy(&(msg.cb), js_msg, sizeof(gui_msg_js_t));
    js_cb = (jerry_value_t)(uintptr_t)msg.payload;
    // gui_log("timer msg cb 0x%x\n", js_cb);

    res = jerry_call_func_sem(js_cb, jerry_create_undefined(), 0, 0);
    jerry_release_value(res);
}

#if _WIN32
#include <pthread.h>

typedef struct
{
    pthread_t *hdl;
    uint32_t time_ms;
    bool isPeriodic;
    void *js_cb;
} JS_TIMER_T;

void js_ui_timerThread(void *param)
{
    JS_TIMER_T *js_timer = (JS_TIMER_T *)param;
    const jerry_value_t js_cb = (jerry_value_t)(uintptr_t)js_timer->js_cb;
    uint32_t time_ms = js_timer->time_ms;
    gui_msg_t msg = {.event = GUI_EVENT_EXTERN_IO_JS, .payload = (void *)(uintptr_t)EXTERN_EVENT_TIMER, .cb = (gui_msg_cb)(uintptr_t)js_cb};

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    do
    {
        usleep(time_ms * 1000); // us

        pthread_testcancel();
        // gui_send_msg_to_server(&msg);

        int res = jerry_call_func_sem(js_cb, jerry_create_undefined(), 0, 0);
        jerry_release_value(res);

        pthread_testcancel();
    }
    while (js_timer->isPeriodic);

    pthread_exit(NULL);
}
#else
#include "os_timer.h"
#include "trace.h"

#if defined (RTL87x2G)
void arm_js_timercb(void *p_handle)
{
    // DBG_DIRECT("Timercb 0x%x 0x%x\n", &p_handle, p_handle);
    uint32_t timer_id;
    void *js_cb;

    os_timer_id_get(&p_handle, &timer_id);
    // DBG_DIRECT("timer_id 0x%x\n", timer_id);

    js_cb = (void *)timer_id;
    // DBG_DIRECT("js_cb 0x%x\n", js_cb);

    gui_msg_t msg = {.event = GUI_EVENT_EXTERN_IO_JS, .cb = (gui_msg_cb)EXTERN_EVENT_TIMER, .payload = js_cb};
    gui_send_msg_to_server(&msg);
}
#endif

#endif


DECLARE_HANDLER(setTimeout)
{
    // gui_log("setTimeout \n");
    if (args_cnt >= 2 && jerry_value_is_function(args[0]) && jerry_value_is_number(args[1]))
    {
        uint32_t time_ms = (uint32_t)jerry_get_number_value(args[1]);
        jerry_value_t js_cb = args[0];
        double js_timerID = 0;
        gui_log("js_cb 0x%x\n", (uint32_t)js_cb);

#ifdef _WIN32
        pthread_t *thread_hdl;
        JS_TIMER_T *js_timer = gui_malloc(sizeof(JS_TIMER_T));
        memset(js_timer, 0, sizeof(JS_TIMER_T));
        js_timer->js_cb = (void *)(uintptr_t)js_cb;
        js_timer->time_ms = (uint32_t)jerry_get_number_value(args[1]);
        js_timer->isPeriodic = false;

        thread_hdl = gui_thread_create("Timer", js_ui_timerThread, js_timer,
                                       0, 0);
        js_timer->hdl = thread_hdl;

        // gui_log("hdl 0x%x, timerID 0x%x\n", thread_hdl, js_timer);

        js_timerID = (uint32_t)(uintptr_t)js_timer;
        gui_log("timerID 0x%x\n", (uint32_t)js_timerID);

        return jerry_create_number(js_timerID);
#else
#if defined (RTL87x2G)
        void *p_handle = NULL;

        if (os_timer_create(&p_handle, "timer", (uint32_t)js_cb,
                            time_ms, false, arm_js_timercb) == true)
        {
            // Timer created successfully, start the timer.
            os_timer_start(&p_handle);
            js_timerID = (uint32_t)p_handle;
        }
        else
        {
            // Timer failed to create.
            js_timerID = 0;
        }
        gui_log("timerID 0x%x\n", (uint32_t)js_timerID);

        return jerry_create_number(js_timerID);
#endif
#endif
    }
    return jerry_create_undefined();
}

DECLARE_HANDLER(clearTimeout)
{
    // gui_log("clearTimeout \n");
    if (args_cnt == 1 && jerry_value_is_number(args[0]))
    {
        uint32_t js_timerID = (uint32_t)jerry_get_number_value(args[0]);
        gui_log("js_timerID 0x%x\n", js_timerID);

#ifdef _WIN32
        JS_TIMER_T *js_timer = (JS_TIMER_T *)(uintptr_t)js_timerID;
        int res = 0;

        // gui_log("hdl 0x%x js_timer 0x%x\n", js_timer->hdl, js_timer);
        res = gui_thread_delete(js_timer->hdl);
        if (0 == res)
        {
            gui_free(js_timer);
        }
        else
        {
            gui_log("res %d %s\n", res, strerror(res));
        }

#else
#if defined (RTL87x2G)
        os_timer_delete((void **)&js_timerID);
#endif
#endif
    }
    return jerry_create_undefined();
}


DECLARE_HANDLER(setInterval)
{
    // gui_log("setInterval \n");
    if (args_cnt >= 2 && jerry_value_is_function(args[0]) && jerry_value_is_number(args[1]))
    {
        uint32_t time_ms = (uint32_t)jerry_get_number_value(args[1]);
        jerry_value_t js_cb = args[0];
        double js_timerID = 0;
        gui_log("js_cb 0x%x\n", (uint32_t)js_cb);

#ifdef _WIN32
        pthread_t *thread_hdl;
        JS_TIMER_T *js_timer = gui_malloc(sizeof(JS_TIMER_T));
        memset(js_timer, 0, sizeof(JS_TIMER_T));
        js_timer->js_cb = (void *)(uintptr_t)args[0];
        js_timer->time_ms = (uint32_t)jerry_get_number_value(args[1]);
        js_timer->isPeriodic = true;

        thread_hdl = gui_thread_create("Timer", js_ui_timerThread, js_timer,
                                       0, 0);
        js_timer->hdl = thread_hdl;

        // gui_log("hdl 0x%x, timerID 0x%x\n", thread_hdl, js_timer);

        js_timerID = (uint32_t)(uintptr_t)js_timer;
        gui_log("timerID 0x%x\n", (uint32_t)js_timerID);

        return jerry_create_number(js_timerID);
#else
#if defined (RTL87x2G)
        void *p_handle = NULL;

        if (os_timer_create(&p_handle, "timer", (uint32_t)js_cb,
                            time_ms, true, arm_js_timercb) == true)
        {
            // Timer created successfully, start the timer.
            os_timer_start(&p_handle);
            js_timerID = (uint32_t)p_handle;
        }
        else
        {
            // Timer failed to create.
            js_timerID = 0;
        }
        gui_log("timerID 0x%x\n", (uint32_t)js_timerID);

        return jerry_create_number(js_timerID);
#endif
#endif
    }
    return jerry_create_undefined();
}


DECLARE_HANDLER(clearInterval)
{
    // gui_log("clearInterval \n");
    if (args_cnt == 1 && jerry_value_is_number(args[0]))
    {
        uint32_t js_timerID = (uint32_t)jerry_get_number_value(args[0]);
        gui_log("js_timerID 0x%x\n", js_timerID);

#ifdef _WIN32
        JS_TIMER_T *js_timer = (JS_TIMER_T *)(uintptr_t)js_timerID;
        int res = 0;

        // gui_log("hdl 0x%x\n", js_timer->hdl);
        res = gui_thread_delete(js_timer->hdl);
        if (0 == res)
        {
            gui_free(js_timer);
        }
        else
        {
            gui_log("res %d %s\n", res, strerror(res));
        }
#else
#if defined (RTL87x2G)
        os_timer_delete((void **)&js_timerID);
#endif
#endif
    }
    return jerry_create_undefined();
}


void js_gui_init()
{
    jerry_value_t global_obj = jerry_get_global_object();
    REGISTER_METHOD(global_obj, getAttribute);
    jerry_value_t img = jerry_create_object();
    js_set_property(global_obj, "img", img);
    REGISTER_METHOD(img, getElementById);
    REGISTER_METHOD(img, setLocation);
    REGISTER_METHOD(img, rotation);
    REGISTER_METHOD(img, scale);
    REGISTER_METHOD(img, setMode);
    REGISTER_METHOD(img, setImg);
    REGISTER_METHOD(img, setShow);
    REGISTER_METHOD_NAME(img,  "setAnimate", setAnimate_img);
    REGISTER_METHOD_NAME(img,  "playAnimate", play_animate_img);
    REGISTER_METHOD_NAME(img,  "pauseAnimate", pause_animate_img);
    REGISTER_METHOD_NAME(img,  "setBlock", setBlock);


    jerry_value_t tab = jerry_create_object();
    js_set_property(global_obj, "tab", tab);
    REGISTER_METHOD(tab, getElementById);
    REGISTER_METHOD(tab, jump);
    REGISTER_METHOD_NAME(tab, "onChange", OnChange);
    REGISTER_METHOD_NAME(tab, "getCurTab", getCurTab);

    jerry_value_t icon = jerry_create_object();
    js_set_property(global_obj, "icon", icon);
    REGISTER_METHOD(icon, getElementById);
    REGISTER_METHOD(icon, onClick);
    REGISTER_METHOD(icon, onPress);
    REGISTER_METHOD(icon, onRelease);
    REGISTER_METHOD(icon, onHold);
    REGISTER_METHOD(icon, getChildElementByTag);
    REGISTER_METHOD(icon, setShow);
    REGISTER_METHOD_NAME(icon, "write", icon_write);
    REGISTER_METHOD_NAME(icon, "setTxtPos", icon_txtPos);
    REGISTER_METHOD_NAME(icon, "setEnable", icon_setEnable);
    REGISTER_METHOD_NAME(icon, "setImg", icon_setImg);


    jerry_value_t progress = jerry_create_object();
    js_set_property(global_obj, "progressbar", progress);
    REGISTER_METHOD(progress, progress);
    REGISTER_METHOD(progress, getElementById);

    jerry_value_t seekbar = jerry_create_object();
    js_set_property(global_obj, "seekbar", seekbar);
    REGISTER_METHOD(seekbar, progress);
    REGISTER_METHOD(seekbar, getElementById);
    REGISTER_METHOD(seekbar, setShow);
    REGISTER_METHOD_NAME(seekbar, "onRelease", onrelease_seekbar);
    REGISTER_METHOD_NAME(seekbar, "onPress", onpress_seekbar);
    REGISTER_METHOD_NAME(seekbar, "onPressing", onpressing_seekbar);
    REGISTER_METHOD_NAME(seekbar, "setAnimate", setAnimate_seekbar);
    REGISTER_METHOD_NAME(seekbar, "setAttribute", seekbar_setAttribute);
    REGISTER_METHOD_NAME(seekbar, "getAttribute", seekbar_getAttribute);
    REGISTER_METHOD_NAME(seekbar, "playAnimate", play_animate_seekbar);
    REGISTER_METHOD_NAME(seekbar, "animateProgress", seekbar_getAnimateProgress);


    jerry_value_t document = jerry_create_object();
    js_set_property(global_obj, "textbox", document);
    REGISTER_METHOD(document, write);
    REGISTER_METHOD(document, getElementById);
    REGISTER_METHOD(document, setPosition);
    REGISTER_METHOD(document, setShow);
    REGISTER_METHOD_NAME(document, "playAnimate", play_animate_text);
    REGISTER_METHOD_NAME(document, "setAnimate", setAnimate_text);
    REGISTER_METHOD_NAME(document, "pauseAnimate", pause_animate_text);
    REGISTER_METHOD_NAME(document, "setInputable", setInputable);
    REGISTER_METHOD_NAME(document, "onClick", onClick_text);
    REGISTER_METHOD_NAME(document, "setPasswd", setPasswd);


    jerry_value_t app = jerry_create_object();
    js_set_property(global_obj, "app", app);
    //jerry_set_object_native_pointer(app, app_p, NULL);
    REGISTER_METHOD(app, open);
    REGISTER_METHOD(app, reload);
    REGISTER_METHOD(app, close);
    jerry_value_t win = jerry_create_object();
    js_set_property(global_obj, "win", win);
    REGISTER_METHOD(win, getElementById);
    REGISTER_METHOD(win, setShow);
    REGISTER_METHOD_NAME(win, "onRelease", onRelease_win);
    REGISTER_METHOD_NAME(win, "onClick", onClick_win);
    REGISTER_METHOD_NAME(win, "onPress", onPress_win);
    REGISTER_METHOD_NAME(win, "onHold", onHold_win);
    REGISTER_METHOD_NAME(win, "onLeft", onLeft_win);
    REGISTER_METHOD_NAME(win, "onRight", onRight_win);
    REGISTER_METHOD_NAME(win, "onDown", onUp_win);
    REGISTER_METHOD_NAME(win, "onUp", onDown_win);
    REGISTER_METHOD_NAME(win, "getAttribute", win_getAttribute);
    REGISTER_METHOD_NAME(win, "removeAttribute", win_removeAttribute);
    REGISTER_METHOD_NAME(win, "setAttribute", win_setAttribute);

    jerry_value_t page = jerry_create_object();
    js_set_property(global_obj, "page", page);
    REGISTER_METHOD(page, getElementById);
    REGISTER_METHOD_NAME(page, "setRebound", setRebound);

    jerry_value_t sw = jerry_create_object();
    js_set_property(global_obj, "sw", sw);
    REGISTER_METHOD(sw, getElementById);
    REGISTER_METHOD(sw, setShow);
    REGISTER_METHOD_NAME(sw, "onOn", switch_on);
    REGISTER_METHOD_NAME(sw, "onOff", switch_off);
    REGISTER_METHOD_NAME(sw, "turnOn", sw_open);
    REGISTER_METHOD_NAME(sw, "turnOff", sw_close);
    REGISTER_METHOD_NAME(sw, "onPress", onPress_switch);
    REGISTER_METHOD_NAME(sw, "setState", sw_setState);

    // timer
    REGISTER_METHOD(global_obj, setTimeout);
    REGISTER_METHOD(global_obj, clearTimeout);
    REGISTER_METHOD(global_obj, setInterval);
    REGISTER_METHOD(global_obj, clearInterval);

    jerry_release_value(global_obj);

#ifdef __WIN32
    sem_init(&sem_timer, 0, 1);
#endif

}


void js_run_file_on_server(const char *file, gui_app_t *app)
{
    if (file)
    {
        gui_msg_t msg;
        memset(&msg, 0, sizeof(gui_msg_t));
        msg.payload = (void *)file;
        msg.cb = (gui_msg_cb)app;
    }
}