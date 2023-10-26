

//#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jerry_util.h>
#include <gui_app.h>
#include <gui_obj.h>
#include <gui_text.h>
#include "gui_server.h"
#include "gui_win.h"
#include "gui_button.h"
#if 1
#ifdef OS_FREERTOS
#include "os_mem.h"
#include "trace.h"
#include "string.h"
#endif





struct ezjs
{
    int a;
    int b;
};
typedef struct cb_arg
{
    jerry_value_t func;
    jerry_value_t *args_p;
    jerry_size_t args_count;
} cb_arg_t;
void *bt_sim_timer;
DECLARE_HANDLER(print1)
{
    gui_log("print1\n");
    return jerry_create_undefined();
}
DECLARE_HANDLER(print2)
{
    gui_log("print2\n");
    return jerry_create_undefined();
}
jerry_value_t js_rect;
DECLARE_HANDLER(get_js_rect)
{
    return js_rect;
}
void ezjsinit()
{
    js_rect = jerry_create_object();
    jerry_value_t a = jerry_create_number(1);
    jerry_value_t b = jerry_create_number(2);
    js_set_property(js_rect, "a", a);
    js_set_property(js_rect, "b", b);
    jerry_release_value(a);
    jerry_release_value(b);
    REGISTER_METHOD(js_rect, print1);
    REGISTER_METHOD(js_rect, print2);
    jerry_value_t global_obj = jerry_get_global_object();
    js_set_property(global_obj, "js_rect", js_rect);
    REGISTER_METHOD(global_obj, get_js_rect);
    jerry_release_value(global_obj);
    jerry_release_value(js_rect);
}
void js_close_on_server(void);
static void js_cb(jerry_value_t func);
static void js_cb_with_args(gui_obj_t *obj);
#include "gui_magic_img.h"
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
    //gui_log("jerry_get_object_native_pointer = %s",img->base.name);
    if (img)
    {
        jerry_value_t s = jerry_value_to_string(args[0]);//gui_log("jerryenter1\n");
        jerry_length_t length = jerry_get_utf8_string_length(s); //gui_log("jerryenter2\n");
        jerry_char_t *strbuf1 = gui_malloc(length + 1); //gui_log("jerryenter3\n");
        jerry_string_to_utf8_char_buffer(s, strbuf1, length + 1); //gui_log("jerryenter4\n");
        strbuf1[length] = '\0';
        gui_img_set_attribute(img, (void *)strbuf1, NULL, jerry_get_number_value(args[1]),
                              jerry_get_number_value(args[2]));//gui_log("jerryenter5\n");
        jerry_release_value(s);
    }
    return jerry_create_undefined();
}


DECLARE_HANDLER(onClick)
{
    ////gui_log("enter onclick\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_button_api.onClick((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onPress)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_button_api.onPress((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onHold)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_button_api.onLong((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}



DECLARE_HANDLER(onRelease_win)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_win_api.onRelease((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onClick_win)
{
    ////gui_log("enter onclick\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_win_api.onClick((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onPress_win)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_win_api.onPress((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onHold_win)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_win_api.onLong((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}



DECLARE_HANDLER(onLeft_win)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_win_api.onLeft((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onRight_win)
{
    ////gui_log("enter onclick\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_win_api.onRight((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onUp_win)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *) &obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_win_api.onUp((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onDown_win)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_win_api.onDown((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(onRelease)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_button_api.onRelease((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(write)
{
    if (args_cnt != 1 || !jerry_value_is_string(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_text_t *txtbox = NULL;
    jerry_get_object_native_pointer(this_value, (void *) &txtbox, NULL);
    //gui_log("jerry_get_object_native_pointer = %s",txtbox->base.name);
    if (txtbox)
    {
        jerry_value_t s = jerry_value_to_string(args[0]);//gui_log("jerryenter1\n");
        jerry_length_t length = jerry_get_utf8_string_length(s); //gui_log("jerryenter2\n");
        jerry_char_t *strbuf1 = gui_malloc(length + 1); //gui_log("jerryenter3\n");
        //gui_log("strbuf1=%p\n",strbuf1);//gui_log("txtbox->text_type:%s,\n",txtbox->text_type);
        jerry_string_to_utf8_char_buffer(s, strbuf1, length + 1); //gui_log("jerryenter4\n");
        strbuf1[length] = '\0';
        //gui_log("txtbox->text_type:%s,\n",txtbox->text_type);
        gui_text_set(txtbox, (void *)strbuf1, txtbox->text_type, txtbox->color, length,
                     txtbox->font_height);
        jerry_release_value(s);
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
#include "gui_tabview.h"
DECLARE_HANDLER(jump)
{
    if (args_cnt != 1 || !jerry_value_is_number(args[0]))
    {
        return jerry_create_undefined();
    }
    gui_obj_t *txtbox = NULL;
    jerry_get_object_native_pointer(this_value, (void *) &txtbox, NULL);
    //gui_log("jerry_get_object_native_pointer = %s",txtbox->base.name);
    if (txtbox)
    {
        //gui_tab_jump(txtbox, jerry_get_number_value(args[0]), 0);
    }
    return jerry_create_undefined();
}
/*DECLARE_HANDLER(getElementById)
{

    if(args_cnt !=1 || !jerry_value_is_string(args[0]))
    return jerry_create_undefined();
    jerry_value_t s = jerry_value_to_string(args[0]);
    jerry_length_t length = jerry_get_utf8_string_length (s);
    jerry_char_t str_buf[length+1];
    jerry_string_to_utf8_char_buffer(s, str_buf, length+1);
    jerry_release_value(s);
    str_buf[length] = '\0';
    //gui_log(" enter getElementById:%s,length:%d\n",str_buf, length);
    gui_app_t *cur_app = gui_get_current_app();
    gui_obj_t *widget = gui_tree_get_widget_by_name(cur_app->screen, str_buf);
    //gui_log(" enter getElementById widget:%d,%s\n",widget, widget->name);
    //gui_log(" enter getElementById this_value:%d\n",this_value);
    jerry_set_object_native_pointer(this_value, widget, NULL);
    //gui_log(" enter getElementById this_value2:%d\n",this_value);
    return this_value;
}*/
void gui_tree_get_widget_by_name(gui_obj_t *obj, const char *name, gui_obj_t **output)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        //gui_log("obj name = %s, name =%s\n", obj->name, name);
        if ((strlen(name) == strlen(obj->name)) && (strcmp(name, obj->name) == 0))
        {
            //gui_log("obj 1 = %s\n", obj->name);
            //gui_log("obj 1 = %p\n", obj);
            *output = obj;
            return;
        }
        gui_tree_get_widget_by_name(obj, name, output);
    }
}
DECLARE_HANDLER(getElementById)
{
    gui_log("enter getElementById\n");
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
    gui_tree_get_widget_by_name(&app->screen, a, &widget); gui_log("name:%s,widget:%p\n", a, widget);
    gui_free(a);
    //gui_log("getElementById:%x\n",widget);
    jerry_set_object_native_pointer(this_value, widget, NULL);
    jerry_release_value(global_obj);
    jerry_release_value(app_property);
    return jerry_create_undefined();
}
#include "ezxml.h"
DECLARE_HANDLER(getAttribute)
{
    if (args_cnt != 1 || !jerry_value_is_string(args[0]))
    {
        return jerry_create_undefined();
    }
    ezxml_t title;
    jerry_get_object_native_pointer(this_value, (void *)&title, NULL);
    //printf("ezxmltitle:%x", title);
    char *a = js_value_to_string(args[0]);
    const char *widget_type = ezxml_attr(title, a);
    gui_free(a);
    return jerry_create_string((void *)widget_type);
}

extern const char *get_tag_by_widget_type(int type);
extern const char *get_child_ele_attr(char *xml, char *ele, char *txt, char *chile_ele, char *attr);
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
    //printf("app:%s,%s\n", app->xml, name);
    char *widget_type = (void *)get_tag_by_widget_type(type);
    ezxml_t c = get_child_ele((void *)app->xml, widget_type, name, js_value_to_string(args[0]));
    //printf("ezxmlc:%x", c);
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
    //gui_log("jerry_get_object_native_pointer = %s",txtbox->base.name);
    if (txtbox)
    {
        /*jerry_value_t s = jerry_value_to_string(args[0]);//gui_log("jerryenter1\n");
        jerry_length_t length = jerry_get_utf8_string_length(s); //gui_log("jerryenter2\n");
        jerry_char_t *strbuf1 = gui_malloc(length + 1); //gui_log("jerryenter3\n");
        //gui_log("strbuf1=%p\n",strbuf1);//gui_log("txtbox->text_type:%s,\n",txtbox->text_type);
        jerry_string_to_utf8_char_buffer(s, strbuf1, length + 1); //gui_log("jerryenter4\n");
        strbuf1[length] = '\0';
        //gui_log("txtbox->text_type:%s,\n",txtbox->text_type);
        txtbox->text->utf_8 = strbuf1;*/
        char *strbuf1 = js_value_to_string(args[0]);
        //printf("iconwrite:%s\n", strbuf1);
        gui_text_set(txtbox->text, strbuf1, "rtk_font_stb", txtbox->text->color, strlen(strbuf1),
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
#if 0
void js_timeout(void *p)
{
    if (p)
    {
        ////gui_log("enter js_timeout\n");
        rtgui_msg_t msg;
        memset(&msg, 0, sizeof(rtgui_msg_t));
        msg.type = GUI_SRV_CB;
        msg.cb = js_cb;
        msg.u.payload = (void *)(*((uint32_t *)p));
        //gui_tree_get_widget_by_name(&msg);
    }
}
DECLARE_HANDLER(setTimeout)
{
    ////gui_log("enter setTimeout\n");
    static size_t p;
    if (args_cnt >= 2 && jerry_value_is_function(args[0]) && jerry_value_is_number(args[1]))
    {
        p = args[0];

        void *timer = gui_timer_create("ttt", js_timeout, &p,  jerry_get_number_value(args[1]), false);
        size_t a = (size_t)timer;
        jerry_value_t v = jerry_create_number((double)a);
        gui_timer_start(timer);
        return v;
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(clearTimeout)
{
    ////gui_log("enter setTimeout\n");
    if (args_cnt >= 1 && jerry_value_is_number(args[0]))
    {
        void *timer = (void *)(size_t)(jerry_get_number_value(args[0]));
        gui_timer_delete(timer);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(setInterval)
{
    ////gui_log("enter setInterval\n");
    static size_t p;
    if (args_cnt >= 2 && jerry_value_is_function(args[0]) && jerry_value_is_number(args[1]))
    {
        p = args[0];
        void *timer = gui_timer_create("ttt", js_timeout, &p,  jerry_get_number_value(args[1]), true);
        size_t a = (size_t)timer;
        jerry_value_t v = jerry_create_number((double)a);
        gui_timer_start(timer);
        return v;
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(clearInterval)
{
    ////gui_log("enter setTimeout\n");
    if (args_cnt >= 1 && jerry_value_is_number(args[0]))
    {
        void *timer = (void *)(size_t)(jerry_get_number_value(args[0]));
        gui_timer_delete(timer);
    }

    return jerry_create_undefined();
}
#endif
DECLARE_HANDLER(reload)
{
    ////gui_log("enter setTimeout\n");
    gui_app_t *app = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&app, NULL);
    //gui_exec(app->screen);

    return jerry_create_undefined();
}
DECLARE_HANDLER(close)
{
    ////gui_log("enter close\n");
    gui_app_t *app = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&app, NULL);
    gui_app_shutdown(app);
    memset(app, 0, sizeof(gui_app_t));
    //gui_free(app);
    //js_close_on_server();
    return jerry_create_undefined();
}

DECLARE_HANDLER(open)
{
    ////gui_log("enter open\n");
    if (args_cnt >= 1 && jerry_value_is_string(args[0]))
    {
        gui_app_t *app = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&app, NULL);
        //if (app->installed)
        {
            //gui_app_shutdown(app);
            //memset(app, 0, sizeof(gui_app_t));
            //gui_free(app);
            //js_close_on_server();
        }


        char *new_xml = js_value_to_string(args[0]);
        //open_xml_app(new_xml);
        gui_app_t *app2 = (void *)get_app_xml();
        app2->xml = new_xml;
        gui_switch_app(app, app2);
    }
//gui_log("before jerry_create_undefined\n");
    return jerry_create_undefined();
}
/*DECLARE_HANDLER(JumpToTab)
{
    ////gui_log("enter JumpToTab\n");
    if (args_cnt >= 1 && jerry_value_is_string(args[0]))
    {
        gui_app_t *app = NULL;
        jerry_get_object_native_pointer(this_value, &app, NULL);
        char *tabname = js_value_to_string(args[0]);
        //gui_log("tabname =%s,app=%s",tabname, app->app_name);
        if (app)
        {
            gui_jump_to_tab(app->screen, tabname);
        }
        //gui_free(tabname);
    }

    return jerry_create_undefined();
}*/
/*DECLARE_HANDLER(alert)
{
    ////gui_log("enter JumpToTab\n");
    if (args_cnt >= 1 && jerry_value_is_string(args[0]))
    {
        gui_alert(js_value_to_string(args[0]));
    }

    return jerry_create_undefined();
}*/
static void js_cb(jerry_value_t func)
{
    //gui_log("case GUI_SRV_CB_JS:\n");
    jerry_value_t res = jerry_call_function(func, jerry_create_undefined(), NULL, 0);
    jerry_release_value(res);
    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t app_property = js_get_property(global_obj, "app");
    gui_app_t *app = NULL;
    jerry_get_object_native_pointer(app_property, (void *)&app, NULL);
}

static void js_cb_with_args(gui_obj_t *obj)
{
    gui_log("enter js_cb_with_args\n");
    cb_arg_t *args = NULL;
    gui_event_dsc_t *event = obj->event_dsc;
    int event_code = 0;
    for (size_t i = 0; i < obj->event_dsc_cnt; i++)
    {
        if (event[i].event_code)
        {
            event_code = event[i].event_code;
            break;
        }

    }
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
    gui_log("enter js_cb_with_args_animate\n");
    if (args == NULL)
    {
        return;
    }
    jerry_value_t res = jerry_call_function(args->func, jerry_create_undefined(), args->args_p,
                                            args->args_count);
    jerry_release_value(res);

}

static void js_cb_with_arg(cb_arg_t *arg)
{
    //gui_log("case js_cb_with_arg:\n");
    jerry_value_t res = jerry_call_function(arg->func,
                                            jerry_create_undefined(), arg->args_p, arg->args_count);
    jerry_release_value(res);
}
/*DECLARE_HANDLER(onLeft)
{
    ////gui_log("enter onSlideLeft\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, &obj, NULL);

        onClick(obj, js_cb, (void *)(args[0]));
        ////gui_log("enter onSlideLeft %s,%p,obj->callback.cb_type;%d\n",obj->name,obj, obj->callback.cb_type);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onSlideRight)
{
    //gui_log("enter onSlideRight\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, &obj, NULL);

        obj->callback.cb_type = RIGHT_SLIDE_CB;
        obj->callback.link_cb = js_cb;
        obj->callback.param = (void *)(args[0]);
        //gui_log("enter onSlideLeft %s,%p,obj->callback.cb_type;%d\n", obj->name, obj, obj->callback.cb_type);
    }

    return jerry_create_undefined();
}*/
/*8DECLARE_HANDLER(append)
{
    ////gui_log("enter append\n");
    if (args_cnt >= 1 && jerry_value_is_string(args[0]))
    {
        gui_iconlist_t *iconlist = NULL;
        jerry_get_object_native_pointer(this_value, &iconlist, NULL);
        char *icon_name = js_value_to_string(args[0]);
        char *icon_text = "_DEFAULT";
        if (args_cnt >= 2 && jerry_value_is_string(args[1]))
        {
            char *txt = js_value_to_string(args[1]);
            if (strlen(txt))
            {
                icon_text = txt;
            }
        }
        gui_icon_theme1_t *icon = gui_icon_create_theme1(iconlist, icon_name, 0, 0, 320, 60,
        iconlist->icon_cnt, NULL, icon_text, LEFT, 0, NULL, NULL, NULL, 0xffff);


        jerry_value_t global_obj = jerry_get_global_object();
        jerry_value_t app_property = js_get_property(global_obj, "app");
        gui_app_t *app = NULL;
        jerry_get_object_native_pointer(app_property, &app, NULL);
        gui_exec(app->screen);
    }

    return jerry_create_undefined();
}*/
static void screen_reload()
{
    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t app_property = js_get_property(global_obj, "app");
    gui_app_t *app = NULL;
    jerry_get_object_native_pointer(app_property, (void *)&app, NULL);
    //gui_exec(app->screen);
    jerry_release_value(global_obj);
    jerry_release_value(app_property);
}
/*DECLARE_HANDLER(iconlist_clear)
{
    gui_iconlist_t *iconlist = NULL;
    jerry_get_object_native_pointer(this_value, &iconlist, NULL);
    gui_child_tree_free(iconlist);
    iconlist->icon_cnt = 0;
    iconlist->cur_id = 0;
    screen_reload();
    return jerry_create_undefined();
}*/
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
/*DECLARE_HANDLER(ic_write)
{
    ////gui_log("enter ic_write\n");
    if (args_cnt >= 1 && jerry_value_is_string(args[0]))
    {
        gui_icon_theme1_t *icon = NULL;
        jerry_get_object_native_pointer(this_value, &icon, NULL);
        char *text = js_value_to_string(args[0]);
        if (icon->icon_text == NULL)
        {
            icon->icon_text = gui_text_create(icon, "txt_name", 0,0,gui_get_screen_width(), gui_get_screen_height());
            icon->icon_text->set(icon->icon_text, text, "rtk_font_fs32", 0xffff, strlen(text));

        }
        else
        {
            icon->icon_text->set(icon->icon_text, text, "rtk_font_fs32", 0xffff, strlen(text));
        }
    }

    return jerry_create_undefined();
}*/
DECLARE_HANDLER(bluetooth_open)
{
    ////gui_log("enter bluetooth_open\n");
    return jerry_create_undefined();
}
DECLARE_HANDLER(bluetooth_scan)
{
    ////gui_log("enter bluetooth_scan\n");
    //gui_timer_start(bt_sim_timer);
    return jerry_create_undefined();
}
DECLARE_HANDLER(bluetooth_stopscan)
{
    ////gui_log("enter bluetooth_stopscan\n");
    //gui_timer_stop(bt_sim_timer);
    return jerry_create_undefined();
}
DECLARE_HANDLER(bluetooth_close)
{
    ////gui_log("enter bluetooth_stopscan\n");
    return jerry_create_undefined();
}
void found_device()
{
    /*
    char buffer[32];
    jerry_value_t js_ssid, js_strength, js_bssid, js_secure, js_ret;

    js_return = jerry_create_array(info->wifi_list.num);

    for (i = 0; i < info->wifi_list.num; i++)
    {
    struct rt_wlan_info *wifi_info = &(info->wifi_list.info[i]);
    jerry_value_t js_wifi_info = jerry_create_object();

    js_ssid = js_string_to_value((const char*)wifi_info->ssid.val);
    js_set_property(js_wifi_info, "ssid", js_ssid);
    jerry_release_value(js_ssid);

    js_strength = jerry_create_number(wifi_info->rssi);
    js_set_property(js_wifi_info, "strength", js_strength);
    jerry_release_value(js_strength);

    rt_sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
    wifi_info->bssid[0],
    wifi_info->bssid[1],
    wifi_info->bssid[2],
    wifi_info->bssid[3],
    wifi_info->bssid[4],
    wifi_info->bssid[5]);

    js_bssid = jerry_create_string((const jerry_char_t*)buffer);
    js_set_property(js_wifi_info, "bssid", js_bssid);
    jerry_release_value(js_bssid);

    js_secure = jerry_create_boolean(wifi_info->security);
    js_set_property(js_wifi_info, "secure", js_secure);
    jerry_release_value(js_secure);

    js_ret = jerry_set_property_by_index(js_return, i, js_wifi_info);
    jerry_release_value(js_ret);

    jerry_release_value(js_wifi_info);
    }
    }
    */
}
int sim_loop = 0;
void sim_bt_timeout(void *p)
{
    /*  char buffer[32];
        jerry_value_t js_ssid, js_mac, js_secure, js_ret;
        static jerry_value_t js_return;

         js_return = jerry_create_array(3);
        char name[3][10] = { "btdevice1", "btdevice2", "btdevice3" };
        char mac[3][20] = { "11:11:11:11:11:11", "22:22:22:22:22:22", "33:33:33:33:33:33" };
        int i = (sim_loop++)%3;
        {
            jerry_value_t bt_info = jerry_create_object();

            js_ssid = js_string_to_value(name[i]);
            js_set_property(bt_info, "ssid", js_ssid);
            jerry_release_value(js_ssid);

            js_mac = js_string_to_value(mac[i]);
            js_set_property(bt_info, "mac", js_mac);
            jerry_release_value(js_mac);
            js_ret = jerry_set_property_by_index(js_return, i, bt_info);
            jerry_release_value(js_ret);

            jerry_release_value(bt_info);
        }
        rtgui_msg_t msg;
        memset(&msg, 0, sizeof(rtgui_msg_t));
        msg.type = GUI_SRV_CB;
        msg.cb = js_cb_with_arg;
        static cb_arg_t args;
        args.func = p;
        args.args_p = &js_return;
        args.args_count = 1;
        msg.u.payload = &args;
        //gui_tree_get_widget_by_name(&msg);
        jerry_release_value(js_return);*/
}

void sim_bt_found(const jerry_value_t value)
{
    //bt_sim_timer = gui_timer_create("bt", sim_bt_timeout, value, 100, true);

}
DECLARE_HANDLER(bluetooth_onFound)
{
    ////gui_log("enter bluetooth_onFound\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        sim_bt_found(args[0]);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(opendir)
{
    if (args_cnt >= 2 && jerry_value_is_string(args[0]) && jerry_value_is_function(args[1]))
    {
        char *pathname = js_value_to_string(args[0]);
        gui_fs_DIR *dir = gui_fs_opendir(pathname);
        jerry_value_t Dir = js_get_property(this_value, "Dir");
        jerry_set_object_native_pointer(Dir, dir, NULL);
        free(pathname);
        jerry_value_t err = jerry_create_boolean(true);
        jerry_value_t args_g[2] = {err, Dir};
        jerry_release_value(Dir);
        jerry_release_value(err);
        jerry_length_t args_c = 2;
        jerry_value_t res = jerry_call_function(args[1],
                                                this_value, args_g, args_c);
        jerry_release_value(res);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(readSync)
{
    struct gui_fs_dirent *ptr  = NULL;
    gui_fs_DIR *dir = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&dir, NULL);
    ptr = gui_fs_readdir(dir);
    if (ptr)
    {
        jerry_value_t global_obj = jerry_get_global_object();
        jerry_value_t fs = js_get_property(global_obj, "fs");
        jerry_value_t dirent = js_get_property(fs, "Dirent");
        jerry_value_t direntname = js_string_to_value(gui_strdup(ptr->d_name));
        js_set_property(dirent, "name", direntname);
        jerry_set_object_native_pointer(dirent, ptr, NULL);
        jerry_release_value(global_obj);
        jerry_release_value(fs);
        return dirent;
    }
    else
    {
        return jerry_create_boolean(false);
    }



}
DECLARE_HANDLER(fs_close)
{
    gui_fs_DIR *dir = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&dir, NULL);
    gui_fs_closedir(dir);
    return jerry_create_undefined();
}
DECLARE_HANDLER(isFile)
{
    struct gui_fs_dirent *ptr  = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&ptr, NULL);
    if (ptr->d_type == 1)
    {
        return jerry_create_boolean(true);
    }

    return jerry_create_boolean(false);
}
DECLARE_HANDLER(isDirectory)
{
    struct gui_fs_dirent *ptr  = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&ptr, NULL);
    if (ptr->d_type == 2)
    {
        return jerry_create_boolean(true);
    }

    return jerry_create_boolean(false);
}
#include "gui_switch.h"
DECLARE_HANDLER(onOn)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        GUI_TYPE(gui_switch_t, obj)->onOn((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onOff)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        GUI_TYPE(gui_switch_t, obj)->onOff((void *)obj, js_cb_with_args, (void *)(cb_arg));
    }

    return jerry_create_undefined();
}
#include "gui_seekbar.h"
DECLARE_HANDLER(onpress_seekbar)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_obj_add_event_cb(obj, (gui_event_cb_t)js_cb_with_args, GUI_EVENT_1, cb_arg);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onpressing_seekbar)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_obj_add_event_cb(obj, (gui_event_cb_t)js_cb_with_args, GUI_EVENT_3, cb_arg);
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onrelease_seekbar)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);
        gui_obj_t *obj = NULL;
        jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
        ////gui_log("enter onclick %s\n",obj->name);

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
        gui_obj_add_event_cb(obj, (gui_event_cb_t)js_cb_with_args, GUI_EVENT_2, cb_arg);
    }

    return jerry_create_undefined();
}
static void gui_seekbar_set_animate(gui_seekbar_t *o, uint32_t dur, int repeatCount, void *callback,
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
    animate->repeatCount = repeatCount;
    animate->p = p;
    ((gui_seekbar_t *)o)->animate = animate;
}
DECLARE_HANDLER(play_animate_seekbar)
{
    gui_log("enter play_animate_seekbar\n");

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
DECLARE_HANDLER(setAnimate_seekbar)
{
    gui_log("enter setAnimate_seekbar\n");
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
    //gui_log("enter onPress\n");
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
DECLARE_HANDLER(seekbar_setAttribute)
{
    //gui_log("enter onPress\n");
    if (args_cnt >= 2 && jerry_value_is_string(args[0]) && jerry_value_is_number(args[1]))
    {
        char *a = js_value_to_string(args[0]);
        if (strcmp(a, "progress") == 0)
        {
            gui_obj_t *obj = NULL;
            jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
            gui_log("seekbar_setAttribute:%f\n", jerry_get_number_value(args[1]));
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
#include "gui_progressbar.h"
DECLARE_HANDLER(progress)
{
    //gui_log("enter progress\n");
    gui_obj_t *obj = NULL;
    jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
    if (args_cnt >= 1 && jerry_value_is_number(args[0]))
    {
        //js_add_event_listener(this_value, "onclick", args[0]);

        ////gui_log("enter onclick %s\n",obj->name);
        gui_progressbar_set_percentage((void *)obj, jerry_get_number_value(args[0]));
    }
    float per = gui_progressbar_get_percentage((void *)obj);
    return jerry_create_number(per);
    //return jerry_create_undefined();
}
DECLARE_HANDLER(sw_close)
{
    gui_log("enter sw_close\n");
    gui_switch_t *sw = NULL;
    jerry_get_object_native_pointer(this_value, (void *)(&sw), NULL);
    sw->turn_off(sw);
    return jerry_create_undefined();
}

DECLARE_HANDLER(sw_open)
{
    gui_log("enter sw_open\n");
    gui_switch_t *sw = NULL;
    jerry_get_object_native_pointer(this_value, (void *)(&sw), NULL);
    sw->turn_on(sw);
    return jerry_create_undefined();
}
#ifdef RTL87x2G
//#include "rtl_gpio.h"
//#include "rtl_rcc.h"
//#include "drv_gpio.h"
//#include "drv_i2c.h"
//#include "drv_touch.h"
//#include "drv_lcd.h"
//#include "touch_gt911.h"
//#include "string.h"
//#include "trace.h"
//#include "utils.h"
//#endif
//#ifdef RTL87x2G
//#include "app_msg.h"
//T_IO_MSG led_msg = {.type = IO_MSG_TYPE_LED_ON};
//T_IO_MSG led_off_msg = {.type = IO_MSG_TYPE_LED_OFF};
#endif
DECLARE_HANDLER(writeSync)
{
    gui_log("enter writeSync:%d\n", args[0]);
    if (args_cnt >= 1 && jerry_value_is_number(args[0]))
    {

        int write_value = jerry_get_number_value(args[0]);
        int gpio = -1;
        jerry_value_t v1;
        jerry_value_t v2;
        v1 = js_get_property(this_value, "gpio");
        v2 = js_get_property(this_value, "direction");
        gpio = jerry_get_number_value(v1);
        jerry_release_value(v1);
        char *direction = js_value_to_string(v2);
        jerry_release_value(v2);
        int mode = 0;
#ifdef RTL87x2G

        //if (!strcmp(direction, "out"))
        //{
        //    mode = PIN_MODE_OUTPUT;
        //}
        //else if (!strcmp(direction, "in"))
        //{
        //    mode = PIN_MODE_INPUT;
        //}
        if (gpio >= 0)
        {
            gui_log("gpio%d, %d, %d", gpio, mode, write_value);
            //drv_pin_mode(gpio, mode);
            //drv_pin_write(gpio, write_value);
//extern bool app_send_msg_to_apptask(T_IO_MSG *p_msg);
//                  if(write_value == 0){
//                  led_msg.u.param = 0x64+gpio;
//                  app_send_msg_to_apptask(&led_msg);}
//                  else
//                  {
//                                      led_off_msg.u.param = 0x64+gpio;
//                  app_send_msg_to_apptask(&led_off_msg);
//                  }
        }


#endif
        gui_free(direction);
    }
    return jerry_create_undefined();
}

DECLARE_HANDLER(Gpio)
{
    gui_log("enter GPIO\n");
    REGISTER_METHOD(this_value, writeSync);
    //REGISTER_METHOD(this_value, gpioWrite);
    char *mode_string = "in";
    int pin = -1;
    if (args_cnt >= 1)
    {
        if (jerry_value_is_number(args[0]))
        {
            pin = jerry_get_number_value(args[0]);

        }
        if (args_cnt >= 2)
        {
            if (jerry_value_is_string(args[1]))
            {
                mode_string = js_value_to_string(args[1]);
            }
        }
    }
    jerry_value_t pin_js = jerry_create_number(pin);
    js_set_property(this_value, "gpio", pin_js);
    js_set_string_property(this_value, "direction", mode_string);
    /*{
    #ifdef RTL87x2G
    #include "drv_gpio.h"
        if (mode ==1)
        {
            mode = PIN_MODE_OUTPUT;
        }
        else
        {
            mode = PIN_MODE_INPUT;
        }
        if (pin>=0)
        {
            drv_pin_mode(pin, mode);
        }
    #endif
    }*/


    return jerry_create_undefined();
}
void gpio_init()
{
    /*var LED1 = new Gpio('P1_0', 'out');
    var LED2 = new Gpio('P1_1', 'out');
    var LED3 = new Gpio('P2_5', 'out');
    LED1.writeSync(1)
    LED2.writeSync(1)
    LED3.writeSync(1)*/
    jerry_value_t global_obj = jerry_get_global_object();
    REGISTER_METHOD(global_obj, Gpio);
}

jerry_value_t fs_init()
{
    jerry_value_t fs = jerry_create_object();

    REGISTER_METHOD(fs, opendir);
    jerry_value_t Dir = jerry_create_object();
    REGISTER_METHOD(Dir, readSync);
    REGISTER_METHOD_NAME(Dir, "close", fs_close);
    jerry_value_t Dirent = jerry_create_object();
    REGISTER_METHOD(Dirent, isFile);
    REGISTER_METHOD(Dirent, isDirectory);
    js_set_property(fs, "Dir", Dir);
    js_set_property(fs, "Dirent", Dirent);
    jerry_value_t global_obj = jerry_get_global_object();
    js_set_property(global_obj, "fs", fs);
    return fs;
}
//JS_MODULE(fs, fs_init)
void gui_js_init()
{
    //gui_log(" enter gui_js_init\n");
    jerry_value_t global_obj = jerry_get_global_object();
//  REGISTER_METHOD(global_obj, setTimeout);
//  REGISTER_METHOD(global_obj, setInterval);
    //REGISTER_METHOD(global_obj, alert);
//  REGISTER_METHOD(global_obj, clearInterval);
//  REGISTER_METHOD(global_obj, clearTimeout);
//    REGISTER_METHOD(global_obj, alert);
    //REGISTER_METHOD(global_obj, confirm);
    //
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



//    REGISTER_METHOD(img, setGpu);
//    REGISTER_METHOD(img, setNogpu);
//    REGISTER_METHOD(img, show);
//    REGISTER_METHOD(img, notShow);
    //
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
//    REGISTER_METHOD_NAME(icon, "write", ic_write);
//    REGISTER_METHOD(icon, show);
//    REGISTER_METHOD(icon, notShow);
    //
    jerry_value_t document = jerry_create_object();
    js_set_property(global_obj, "textbox", document);
    REGISTER_METHOD(document, write);
    REGISTER_METHOD(document, getElementById);
//    REGISTER_METHOD(document, show);
//    REGISTER_METHOD(document, notShow);
    //
    jerry_value_t app = jerry_create_object();
    js_set_property(global_obj, "app", app);
    //jerry_set_object_native_pointer(app, app_p, NULL);
    REGISTER_METHOD(app, open);
    REGISTER_METHOD(app, reload);
//  REGISTER_METHOD(app, JumpToTab);
    REGISTER_METHOD(app, close);
    //
    jerry_value_t win = jerry_create_object();
    js_set_property(global_obj, "win", win);
    REGISTER_METHOD(win, getElementById);
//    REGISTER_METHOD(win, onSlideLeft);
//  REGISTER_METHOD(win, onSlideRight);
//    REGISTER_METHOD(win, onClick);
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
//    REGISTER_METHOD(sw, show);
//    REGISTER_METHOD(sw, notShow);
    REGISTER_METHOD_NAME(sw, "turnOn", sw_open);
    REGISTER_METHOD_NAME(sw, "turnOff", sw_close);
    //
    //




    jerry_release_value(global_obj);
    //gui_log(" end gui_js_init\n");
}
static void *context_alloc(size_t size, void *cb_data_p)
{
#ifdef OS_FREERTOS
#ifdef RTL87x2G
#include "mem_config.h"
    return (void *)(SPIC1_ADDR + 0x200000);
    //return os_mem_alloc(RAM_TYPE_EXT_DATA_SRAM, size);//
#elif defined RTL8763EP
    //return (void *)(0x4000000 + 0x200000);
    return malloc(size);
#else
    return malloc(size);
#endif
#endif
    return malloc(size);
}
#include <jerryscript.h>
#include <jerry_util.h>
#include "..\Third_Party\jerryscript-latest\rtthread-port\config.h"
extern int js_console_init(void);
extern void jerry_port_set_default_context(jerry_context_t *context);
jerry_value_t parsed_code;
jerry_value_t ret_value;
void js_init(void)
{
    /* JERRY_ENABLE_EXTERNAL_CONTEXT */
    jerry_port_set_default_context(jerry_create_context(PKG_JMEM_HEAP_SIZE * 1024, context_alloc,
                                                        NULL));

    /* Initialize engine */
    jerry_init(JERRY_INIT_EMPTY);

    /* Register 'print' function from the extensions */
    jerryx_handler_register_global((const jerry_char_t *) "print",
                                   jerryx_handler_print);
    js_console_init();
    gui_js_init();
    gpio_init();
    //fs_init();
    extern gui_app_t *get_app_launcher_frontend(void);
    gui_app_startup(get_app_launcher_frontend());
}
void js_run_file(const char *file, gui_app_t  *app)
{
    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t app_property = js_get_property(global_obj, "app");
    jerry_set_object_native_pointer(app_property, app, NULL);
    jerry_release_value(global_obj);
    jerry_release_value(app_property);
    jerry_char_t *script;
    gui_log("js_run_file1");
    size_t length = js_read_file(file, (void *)(&script));
    if (length > 0)
    {
        gui_log("js_run_file2:%s", script);
        /* Setup Global scope code */
        parsed_code = jerry_parse(NULL, 0, script, length, JERRY_PARSE_NO_OPTS);
        gui_log("js_run_file3");
        if (!jerry_value_is_error(parsed_code))
        {
            /* Execute the parsed source code in the Global scope */
            ret_value = jerry_run(parsed_code);
            /* Returned value must be freed */
            jerry_release_value(ret_value);
        }
        /* Parsed source code must be freed */
        jerry_release_value(parsed_code);
        free(script);
    }



}
void js_close()
{
    ////gui_log("enter js_close\n");
    /* Cleanup engine */
    jerry_cleanup();

    free((void *)jerry_port_get_current_context());
}
void js_close_on_server()
{
    rtgui_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.type = GUI_SRV_CB;
    msg.cb = js_close;
    //gui_tree_get_widget_by_name(&msg);
}
#include "gui_server.h"
void js_run_file_on_server(const char *file, gui_app_t *app)
{
    //gui_log("js_run_file_on_server\n");
    if (file)
    {
        rtgui_msg_t msg;
        memset(&msg, 0, sizeof(rtgui_msg_t));
        msg.type = GUI_SRV_RUN_JS;
        msg.u.payload = (void *)file;
        msg.cb = app;
        //gui_tree_get_widget_by_name(&msg);
    }


}
#endif


