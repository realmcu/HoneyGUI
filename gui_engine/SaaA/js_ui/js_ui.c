
#include "js_user.h"


static void js_cb_with_args(gui_obj_t *obj, gui_event_t event_code)
{
    //gui_log("enter js_cb_with_args\n");
    cb_arg_t *args = NULL;
    gui_event_dsc_t *event = obj->event_dsc;

    // same event only handle the first register one
    for (size_t i = 0; i < obj->event_dsc_cnt; i++)
    {
        if (event[i].filter == event_code)
        {
            args = event[i].user_data;
        }
    }
    if (args == NULL)
    {
        return;
    }
    jerry_value_t res = jerry_call_function(args->func, jerry_create_undefined(), args->args_p,
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
    jerry_value_t res = jerry_call_function(args->func, jerry_create_undefined(), args->args_p,
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
    gui_tree_get_widget_by_name(&app->screen, a, &widget);
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
    gui_button_t *txtbox = NULL;
    jerry_get_object_native_pointer(this_value, (void *) &txtbox, NULL);
    if (txtbox)
    {
        char *strbuf1 = js_value_to_string(args[0]);
        gui_text_set(txtbox->text, strbuf1, GUI_FONT_SRC_TTF, txtbox->text->color, strlen(strbuf1),
                     txtbox->text->font_height);
        //jerry_release_value(s);
        jerry_value_t global_obj = jerry_get_global_object();
        jerry_value_t app_property = js_get_property(global_obj, "app");
        gui_app_t *app = NULL;
        jerry_get_object_native_pointer(app_property, (void *)&app, NULL);
        jerry_release_value(global_obj);
        jerry_release_value(app_property);
        //gui_exec(app->screen);
    }
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
        gui_app_t *app2 = (void *)get_app_xml();
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


DECLARE_HANDLER(rotation)
{
    if (args_cnt != 3 || !jerry_value_is_number(args[0]) || !jerry_value_is_number(args[1]) ||
        !jerry_value_is_number(args[2]))
    {
        return jerry_create_undefined();
    }
    gui_img_t *img = NULL;
    jerry_get_object_native_pointer(this_value, (void *)img, NULL);
    if (!img)
    {
        gui_img_rotation((void *)img, jerry_get_number_value(args[0]),
                         jerry_get_number_value(args[1]), jerry_get_number_value(args[2]));
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
    jerry_get_object_native_pointer(this_value, (void *)img, NULL);
    if (!img)
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
    jerry_get_object_native_pointer(this_value, (void *)img, NULL);
    if (!img)
    {
        gui_img_set_mode(img, jerry_get_number_value(args[0]));
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(setAttribute)
{
    if (args_cnt != 3 || !jerry_value_is_string(args[0]) || !jerry_value_is_number(args[1]) ||
        !jerry_value_is_number(args[2]))
    {
        return jerry_create_undefined();
    }
    gui_img_t *img = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&img, NULL);
    ////gui_log("jerry_get_object_native_pointer = %s",img->base.name);
    if (img)
    {
        jerry_value_t s = jerry_value_to_string(args[0]);////gui_log("jerryenter1\n");
        jerry_length_t length = jerry_get_utf8_string_length(s); ////gui_log("jerryenter2\n");
        jerry_char_t *strbuf1 = gui_malloc(length + 1); ////gui_log("jerryenter3\n");
        jerry_string_to_utf8_char_buffer(s, strbuf1, length + 1); ////gui_log("jerryenter4\n");
        strbuf1[length] = '\0';
        gui_img_set_attribute(img, (void *)strbuf1, NULL, jerry_get_number_value(args[1]),
                              jerry_get_number_value(args[2]));////gui_log("jerryenter5\n");
        jerry_release_value(s);
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
        gui_button_click((void *)obj, js_cb_with_args, (void *)(cb_arg));
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
        gui_button_press((void *)obj, js_cb_with_args, (void *)(cb_arg));
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
        gui_button_long((void *)obj, js_cb_with_args, (void *)(cb_arg));
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
        gui_button_release((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(onOn)
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
        GUI_TYPE(gui_switch_t, obj)->onOn((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onOff)
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
        GUI_TYPE(gui_switch_t, obj)->onOff((void *)obj, js_cb_with_args, (void *)(cb_arg));
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
        // GUI_TYPE(gui_switch_t, obj)->onOn((void *)obj, js_cb_with_args, (void *)(cb_arg));
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
    animate->callback = (void (*)(void *))callback;
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
    //gui_log("enter seekbar_getAttribute1\n");
    {
        {
            gui_obj_t *obj = NULL;
            jerry_get_object_native_pointer(this_value, (void *)&obj,
                                            NULL);//gui_log("enter seekbar_getAttribute4\n");

            return jerry_create_number(GUI_TYPE(gui_seekbar_t, obj)->animate->progress_percent);

        }
        ;//gui_log("enter seekbar_getAttribute7\n");
    }


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
            //gui_log("seekbar_setAttribute:%f\n", jerry_get_number_value(args[1]));
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
            (obj->not_show) = true;
            gui_fb_change();
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
            obj->not_show = false;

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
    sw->turn_off(sw);
    return jerry_create_undefined();
}

DECLARE_HANDLER(sw_open)
{
    //gui_log("enter sw_open\n");
    gui_switch_t *sw = NULL;
    jerry_get_object_native_pointer(this_value, (void *)(&sw), NULL);
    sw->turn_on(sw);
    return jerry_create_undefined();
}
struct timer_args
{
    jerry_value_t func;
    uint32_t timeout;
    bool one_short;
};
#if _WIN32
#include "stdio.h"
#include "stdlib.h"
#include <pthread.h>
static void *rtk_gui_timer(struct timer_args *arg)
{
    if (arg->one_short)
    {
        usleep((int)(arg->timeout) * 1000); //10ms
        gui_free(arg);
        jerry_value_t res = jerry_call_function(arg->func, jerry_create_undefined(), 0, 0);
        jerry_release_value(res);
    }
    else
    {
        while (1)
        {
            usleep((int)(arg->timeout) * 1000); //10ms
            jerry_value_t res = jerry_call_function(arg->func, jerry_create_undefined(), 0, 0);
            jerry_release_value(res);
        }

    }



}
#endif

DECLARE_HANDLER(setTimeout)
{
    if (args_cnt >= 2 && jerry_value_is_function(args[0]) && jerry_value_is_number(args[1]))
    {
#ifdef _WIN32
        pthread_t thread;
        struct timer_args *arg = gui_malloc(sizeof(struct timer_args));
        memset(arg, 0, sizeof(struct timer_args));
        arg->func = args[0];
        arg->timeout = (uint32_t)jerry_get_number_value(args[1]);
        arg->one_short = 1;
        pthread_create(&thread, NULL, rtk_gui_timer, arg);

#endif
    }
    return jerry_create_undefined();
}
DECLARE_HANDLER(setInterval)
{
    if (args_cnt >= 2 && jerry_value_is_function(args[0]) && jerry_value_is_number(args[1]))
    {
#ifdef _WIN32
        pthread_t thread;
        struct timer_args *arg = gui_malloc(sizeof(struct timer_args));
        memset(arg, 0, sizeof(struct timer_args));
        arg->func = args[0];
        arg->timeout = (uint32_t)jerry_get_number_value(args[1]);
        pthread_create(&thread, NULL, rtk_gui_timer, arg);

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
    REGISTER_METHOD(img, rotation);
    REGISTER_METHOD(img, scale);
    REGISTER_METHOD(img, setMode);
    REGISTER_METHOD(img, setAttribute);

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
    REGISTER_METHOD_NAME(icon, "write", icon_write);
    jerry_value_t progress = jerry_create_object();
    js_set_property(global_obj, "progressbar", progress);
    REGISTER_METHOD(progress, progress);
    REGISTER_METHOD(progress, getElementById);

    jerry_value_t seekbar = jerry_create_object();
    js_set_property(global_obj, "seekbar", seekbar);
    REGISTER_METHOD(seekbar, progress);
    REGISTER_METHOD(seekbar, getElementById);
    REGISTER_METHOD_NAME(seekbar, "onRelease", onrelease_seekbar);
    REGISTER_METHOD_NAME(seekbar, "onPress", onpress_seekbar);
    REGISTER_METHOD_NAME(seekbar, "onPressing", onpressing_seekbar);
    REGISTER_METHOD_NAME(seekbar, "setAnimate", setAnimate_seekbar);
    REGISTER_METHOD_NAME(seekbar, "setAttribute", seekbar_setAttribute);
    REGISTER_METHOD_NAME(seekbar, "getAttribute", seekbar_getAttribute);
    REGISTER_METHOD_NAME(seekbar, "palyAnimate", play_animate_seekbar);
    REGISTER_METHOD_NAME(seekbar, "animateProgress", seekbar_getAnimateProgress);
    jerry_value_t document = jerry_create_object();
    js_set_property(global_obj, "textbox", document);
    REGISTER_METHOD(document, write);
    REGISTER_METHOD(document, getElementById);
    REGISTER_METHOD(document, setPosition);
    REGISTER_METHOD_NAME(document, "palyAnimate", play_animate_text);
    REGISTER_METHOD_NAME(document, "setAnimate", setAnimate_text);

    REGISTER_METHOD_NAME(document, "pauseAnimate", pause_animate_text);

    jerry_value_t app = jerry_create_object();
    js_set_property(global_obj, "app", app);
    //jerry_set_object_native_pointer(app, app_p, NULL);
    REGISTER_METHOD(app, open);
    REGISTER_METHOD(app, reload);
    REGISTER_METHOD(app, close);
    jerry_value_t win = jerry_create_object();
    js_set_property(global_obj, "win", win);
    REGISTER_METHOD(win, getElementById);
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
    jerry_value_t sw = jerry_create_object();
    js_set_property(global_obj, "sw", sw);
    REGISTER_METHOD(sw, getElementById);
    REGISTER_METHOD(sw, onOn);
    REGISTER_METHOD(sw, onOff);
    REGISTER_METHOD_NAME(sw, "turnOn", sw_open);
    REGISTER_METHOD_NAME(sw, "turnOff", sw_close);
    REGISTER_METHOD_NAME(sw, "onPress", onPress_switch);
    REGISTER_METHOD(global_obj, setTimeout);
    REGISTER_METHOD(global_obj, setInterval);
    jerry_release_value(global_obj);
}


void js_run_file_on_server(const char *file, gui_app_t *app)
{
    if (file)
    {
        gui_msg_t msg;
        memset(&msg, 0, sizeof(gui_msg_t));
        msg.u.payload = (void *)file;
        msg.cb = app;
    }
}