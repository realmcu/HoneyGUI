#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "jerry_util.h"

#include "jerry_utf8.h"
#include "gui_api.h"
extern int js_console_init(void);
extern int js_module_init(void);
extern int js_buffer_init(void);
extern int js_buffer_cleanup(void);
extern void *gui_malloc(size_t n);
extern void *gui_realloc(void *ptr_old, size_t n);
extern void gui_free(void *rmem);
static void _js_value_dump(jerry_value_t value);

static js_util_user _user_init = NULL, _user_cleanup = NULL;

void js_set_property(const jerry_value_t obj, const char *name,
                     const jerry_value_t prop)
{
    jerry_value_t str = jerry_create_string((const jerry_char_t *)name);
    jerry_set_property(obj, str, prop);
    jerry_release_value(str);
}

jerry_value_t js_get_property(const jerry_value_t obj, const char *name)
{
    jerry_value_t ret;

    const jerry_value_t str = jerry_create_string((const jerry_char_t *)name);
    ret = jerry_get_property(obj, str);
    jerry_release_value(str);

    return ret;
}

void js_set_string_property(const jerry_value_t obj, const char *name,
                            char *value)
{
    jerry_value_t str       = jerry_create_string((const jerry_char_t *)name);
    jerry_value_t value_str = jerry_create_string((const jerry_char_t *)value);
    jerry_set_property(obj, str, value_str);
    jerry_release_value(str);
    jerry_release_value(value_str);
}

void js_set_boolean_property(const jerry_value_t obj, const char *name,
                             bool value)
{
    jerry_value_t str = jerry_create_string((const jerry_char_t *)name);
    jerry_value_t value_bool = jerry_create_boolean(value);
    jerry_set_property(obj, str, value_bool);
    jerry_release_value(str);
    jerry_release_value(value_bool);
}

void js_add_function(const jerry_value_t obj, const char *name,
                     jerry_external_handler_t func)
{
    jerry_value_t str = jerry_create_string((const jerry_char_t *)name);
    jerry_value_t jfunc = jerry_create_external_function(func);

    jerry_set_property(obj, str, jfunc);

    jerry_release_value(str);
    jerry_release_value(jfunc);
}

jerry_value_t js_string_to_value(const char *value)
{
    jerry_value_t str = 0;

    if (jerry_str_is_utf8(value, strlen(value)))
    {
        str = jerry_create_string((const jerry_char_t *)value);
    }
    else
    {
        char *utf8 = NULL;

        jerry_str2utf8((char *)value, strlen(value), &utf8);
        str = jerry_create_string((const jerry_char_t *)utf8);

        gui_free(utf8);
    }

    return str;
}

char *js_value_to_string(const jerry_value_t value)
{
    int len;
    char *str;

    len = jerry_get_utf8_string_size(value);

    str = (char *)gui_malloc(len + 1);
    if (str)
    {
        jerry_string_to_char_buffer(value, (jerry_char_t *)str, len);
        str[len] = '\0';
    }

    return str;
}

jerry_value_t js_call_func_obj(const jerry_value_t func_obj_val, /**< function object to call */
                               const jerry_value_t this_val, /**< object for 'this' binding */
                               const jerry_value_t args_p[], /**< function's call arguments */
                               jerry_size_t args_count) /**< number of the arguments */
{
    jerry_value_t ret;

    js_util_lock();
    ret = jerry_call_function(func_obj_val, this_val, args_p, args_count);
    js_util_unlock();

    return ret;
}

jerry_value_t js_call_function(const jerry_value_t obj, const char *name,
                               const jerry_value_t args[], jerry_size_t args_cnt)
{
    jerry_value_t ret;
    jerry_value_t function = js_get_property(obj, name);

    if (jerry_value_is_function(function))
    {
        ret = js_call_func_obj(function, obj, args, args_cnt);
    }
    else
    {
        ret = jerry_create_null();
    }

    jerry_release_value(function);
    return ret;
}

bool object_dump_foreach(const jerry_value_t property_name,
                         const jerry_value_t property_value, void *user_data_p)
{
    char *str;
    int str_size;
    int *first_property;

    first_property = (int *)user_data_p;

    if (*first_property) { *first_property = 0; }
    else
    {
        printf(", ");
    }

    if (jerry_value_is_string(property_name))
    {
        str_size = jerry_get_string_size(property_name);
        str = (char *) gui_malloc(str_size + 1);
        //RT_ASSERT(str != NULL);

        jerry_string_to_char_buffer(property_name, (jerry_char_t *)str, str_size);
        str[str_size] = '\0';
        printf("%s : ", str);
        gui_free(str);
    }
    _js_value_dump(property_value);

    return true;
}

static void _js_value_dump(jerry_value_t value)
{
    if (jerry_value_is_undefined(value))
    {
        printf("undefined");
    }
    else if (jerry_value_is_boolean(value))
    {
        printf("%s", jerry_get_boolean_value(value) ? "true" : "false");
    }
    else if (jerry_value_is_number(value))
    {
        printf("%f", jerry_get_number_value(value));
    }
    else if (jerry_value_is_null(value))
    {
        printf("null");
    }
    else if (jerry_value_is_string(value))
    {
        char *str;
        int str_size;

        str_size = jerry_get_string_size(value);
        str = (char *) gui_malloc(str_size + 1);
        //RT_ASSERT(str != NULL);

        jerry_string_to_char_buffer(value, (jerry_char_t *)str, str_size);
        str[str_size] = '\0';
        printf("\"%s\"", str);
        gui_free(str);
    }
    else if (jerry_value_is_promise(value))
    {
        printf("promise??");
    }
    else if (jerry_value_is_function(value))
    {
        printf("[function]");
    }
    else if (jerry_value_is_constructor(value))
    {
        printf("constructor");
    }
    else if (jerry_value_is_array(value))
    {
        int index;
        printf("[");
        for (index = 0; index < jerry_get_array_length(value); index ++)
        {
            jerry_value_t item = jerry_get_property_by_index(value, index);
            _js_value_dump(item);
            printf(", ");
            jerry_release_value(item);
        }
        printf("]");
    }
    else if (jerry_value_is_object(value))
    {
        int first_property = 1;
        printf("{");
        jerry_foreach_object_property(value, object_dump_foreach, &first_property);
        printf("}");
    }
    else
    {
        printf("what?");
    }
}

void js_value_dump(jerry_value_t value)
{
    _js_value_dump(value);
    printf("\n");
}
#ifdef __arm__
#include "romfs.h"
#else
#include <sys/stat.h>
#include <fcntl.h>
#endif
int js_read_file(const char *filename, char **script)
{
#ifdef __WIN32
    FILE *fp;
    int length = 0;
    struct stat statbuf;

    if (!filename || !script) { return 0; }

    stat(filename, &statbuf);
    length = statbuf.st_size;

    if (!length) { return 0; }

    if (!(*script)) { return 0; }

    (*script)[length] = '\0';

    fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("open %s failed!\n", filename);
        return 0;
    }

    if (fread(*script, length, 1, fp) != 1)
    {
        length = 0;
        printf("read %s failed!\n", filename);
    }
    fclose(fp);

    return length;
#else
    //FILE *fp;
    int length = 0;
    //struct stat statbuf;

    if (!filename || !script) { return 0; }

    //stat(filename, &statbuf);
    //length = statbuf.st_size;

    //if(!length) return 0;
    int fd = 0;
    fd = open(filename, 0);
    length = gui_fs_lseek(fd, 0, SEEK_END) - lseek(fd, 0, SEEK_SET);

    if (!(*script)) { return 0; }
    (*script)[length] = '\0';

    read(fd, *script, length);

    //rt_kprintf("length:%d",length);
    if (fd <= 0)
    {
        printf("open %s failed!\n", filename);
        return 0;
    }
    close(fd);

    return length;
#endif
}

int js_util_init(void)
{


    js_console_init();
    if (_user_init != NULL)
    {
        _user_init();
    }

    return 0;
}

int js_util_cleanup(void)
{
    if (_user_cleanup != NULL)
    {
        _user_cleanup();
    }

    return 0;
}

void js_util_user_init(js_util_user func)
{
    _user_init = func;
}

void js_util_user_cleanup(js_util_user func)
{
    _user_cleanup = func;
}

int js_util_lock(void)
{
    return 0;
}

int js_util_unlock(void)
{
    return 0;
}
