#include "js_user.h"

DECLARE_HANDLER(opendir)
{
    if (args_cnt >= 2 && jerry_value_is_string(args[0]) && jerry_value_is_function(args[1]))
    {
        char *pathname = js_value_to_string(args[0]);
        gui_fs_dir *dir = gui_fs_opendir(pathname);
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
    gui_fs_dir *dir = NULL;
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
    gui_fs_dir *dir = NULL;
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

jerry_value_t js_fs_init()
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