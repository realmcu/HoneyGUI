#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <rtthread.h>
#include <finsh.h>

#include "jerry_util.h"
#include "jerry_module.h"

#include <jerryscript-ext/module.h>
#include <ecma-globals.h>

#ifdef RT_USING_MODULE
#include <dlfcn.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX    256
#endif

char *getcwd(char *buf, size_t size);

typedef jerry_value_t (*module_init_func_t)(void);

#ifdef RT_USING_DFS
char *js_module_dirname(char *path)
{
    size_t i;
    char *s = NULL;

    if (!path || !*path) { return NULL; }

    s = rt_strdup(path);
    if (!s) { return NULL; }

    i = strlen(s) - 1;
    for (; s[i] == '/'; i--) if (!i)
        {
            s[0] = '/';
            goto __exit;
        }
    for (; s[i] != '/'; i--) if (!i)
        {
            s[0] = '.';
            goto __exit;
        }
    for (; s[i] == '/'; i--) if (!i)
        {
            s[0] = '/';
            goto __exit;
        }

__exit:
    s[i + 1] = 0;
    return s;
}

char *js_module_normalize_path(const char *directory, const char *filename)
{
    char *fullpath;
    char *dst0, *dst, *src;
    char *cwd = NULL;

    /* check parameters */
    if (filename == NULL) { return NULL; }

    if (directory == NULL && filename[0] != '/')
    {
        cwd = (char *) malloc(PATH_MAX);
        if (cwd == NULL) { return NULL; }

        /* get current working directory */
        getcwd(cwd, PATH_MAX);
        directory = cwd;
    }

    if (filename[0] != '/') /* it's a absolute path, use it directly */
    {
        fullpath = malloc(strlen(directory) + strlen(filename) + 2);

        if (fullpath == NULL)
        {
            free(cwd);
            return NULL;
        }

        /* join path and file name */
        snprintf(fullpath, strlen(directory) + strlen(filename) + 2,
                 "%s/%s", directory, filename);
    }
    else
    {
        fullpath = rt_strdup(filename); /* copy string */

        if (fullpath == NULL)
        {
            return NULL;
        }
    }

    src = fullpath;
    dst = fullpath;

    dst0 = dst;
    while (1)
    {
        char c = *src;

        if (c == '.')
        {
            if (!src[1]) { src ++; } /* '.' and ends */
            else if (src[1] == '/')
            {
                /* './' case */
                src += 2;

                while ((*src == '/') && (*src != '\0'))
                {
                    src ++;
                }
                continue;
            }
            else if (src[1] == '.')
            {
                if (!src[2])
                {
                    /* '..' and ends case */
                    src += 2;
                    goto up_one;
                }
                else if (src[2] == '/')
                {
                    /* '../' case */
                    src += 3;

                    while ((*src == '/') && (*src != '\0'))
                    {
                        src ++;
                    }
                    goto up_one;
                }
            }
        }

        /* copy up the next '/' and erase all '/' */
        while ((c = *src++) != '\0' && c != '/')
        {
            *dst ++ = c;
        }

        if (c == '/')
        {
            *dst ++ = '/';
            while (c == '/')
            {
                c = *src++;
            }

            src --;
        }
        else if (!c)
        {
            break;
        }

        continue;

up_one:
        dst --;
        if (dst < dst0)
        {
            free(cwd);
            free(fullpath);
            return NULL;
        }
        while (dst0 < dst && dst[-1] != '/')
        {
            dst --;
        }
    }

    *dst = '\0';

    /* remove '/' in the end of path if exist */
    dst --;
    if ((dst != fullpath) && (*dst == '/'))
    {
        *dst = '\0';
    }

    /* final check fullpath is not empty, for the special path of lwext "/.." */
    if ('\0' == fullpath[0])
    {
        fullpath[0] = '/';
        fullpath[1] = '\0';
    }

    free(cwd);

    return fullpath;
}

/* load module from file system */
static bool load_module_from_filesystem(const jerry_value_t module_name, jerry_value_t *result)
{
    bool ret = false;
    char *str = NULL;
    char *module = js_value_to_string(module_name);

    char *dirname  = NULL;

    jerry_value_t dirname_value  = ECMA_VALUE_UNDEFINED;
    jerry_value_t filename_value = ECMA_VALUE_UNDEFINED;
    jerry_value_t global_obj     = ECMA_VALUE_UNDEFINED;

    char *full_path = NULL;
    char *full_dir  = NULL;

    global_obj  = jerry_get_global_object();
    dirname_value = js_get_property(global_obj, "__dirname");
    if (jerry_value_is_string(dirname_value))
    {
        dirname = js_value_to_string(dirname_value);
    }
    else
    {
        dirname = NULL;
    }

    if (module[0] != '/') /* is a relative path */
    {
        full_path = js_module_normalize_path(dirname, module);
    }
    else
    {
        full_path = js_module_normalize_path(NULL, module);
    }
    free(dirname);

    uint32_t len = js_read_file(full_path, &str);
    if (len == 0) { goto __exit; }

    filename_value = js_get_property(global_obj, "__filename");

    /* set new __filename and __dirname */
    full_dir = js_module_dirname(full_path);

    js_set_string_property(global_obj, "__dirname",  full_dir);
    js_set_string_property(global_obj, "__filename", full_path);

    (*result) = jerry_eval((jerry_char_t *)str, len, false);
    if (jerry_value_is_error(*result))
    {
        printf("failed to evaluate JS\n");
    }
    else
    {
        ret = true;
    }

    /* restore __filename and __dirname */
    js_set_property(global_obj, "__dirname",  dirname_value);
    js_set_property(global_obj, "__filename", filename_value);

__exit:
    if (full_dir) { free(full_dir); }
    if (full_path) { free(full_path); }

    jerry_release_value(global_obj);
    jerry_release_value(dirname_value);
    jerry_release_value(filename_value);

    free(module);
    free(str);

    return ret;
}
#endif

/* load builtin module */
static bool load_module_from_builtin(const jerry_value_t module_name,
                                     jerry_value_t *result)
{
    bool ret = false;
    module_init_func_t module_init;

    char *module = js_value_to_string(module_name);
#ifdef HOST_BUILD
    {
        extern jerry_value_t js_module_rtthread_init(void);

        if (strcmp(module, "os") == 0)
        {
            module_init = js_module_rtthread_init;
            *result = module_init();
            ret = true;
        }
    }
#elif defined(RT_USING_FINSH)
    int len = strlen(module) + 7;
    char module_fullname[len];

    snprintf(module_fullname, len, "__jsm_%s", module);
    module_fullname[len - 1] = '\0';

    /* find syscall in shell symbol section */
    struct finsh_syscall *syscall;

    for (syscall = _syscall_table_begin; syscall < _syscall_table_end; FINSH_NEXT_SYSCALL(syscall))
    {
        if (strcmp(syscall->name, module_fullname) == 0)
        {
            break;
        }
    }
    if (syscall < _syscall_table_end)
    {
        module_init = (module_init_func_t)syscall->func;
        *result = module_init();
        ret = true;
    }
#endif

    free(module);

    return ret;
}

#ifdef RT_USING_DFS
static jerryx_module_resolver_t load_filesystem_resolver =
{
    NULL,
    load_module_from_filesystem
};
#endif

static jerryx_module_resolver_t load_builtin_resolver =
{
    NULL,
    load_module_from_builtin
};

static const jerryx_module_resolver_t *resolvers[] =
{
    &load_builtin_resolver,
#ifdef RT_USING_DFS
    &load_filesystem_resolver
#endif
};

#ifdef RT_USING_MODULE

typedef jerry_value_t (*FFI_INIT)(void);

static void js_module_info_free(void *native)
{
    dlclose(native);
}

static const jerry_object_native_info_t js_module_info =
{
    .free_cb = js_module_info_free
};

#endif

DECLARE_HANDLER(require)
{
    jerry_value_t result;
    char *module = NULL;

    if (args_cnt == 0)
    {
        printf("No module name supplied\n");
        return jerry_create_undefined();
    }

    if (jerry_value_is_string(args[0]) == 0)
    {
        printf("No module name supplied as string\n");
        return jerry_create_undefined();
    }

    module = js_value_to_string(args[0]);
    /* Try each of the resolvers to see if we can find the requested module */
#ifdef RT_USING_MODULE
    if (strstr(module, ".so"))
    {
        void *handle;
        FFI_INIT ffi_init;
        jerry_value_t info;

        do
        {
            handle = dlopen(module, RTLD_LAZY);
            if (!handle)
            {
                printf("dlopen %s failed!\n", module);
                result = jerry_create_undefined();
                break;
            }

            ffi_init = (FFI_INIT)dlsym(handle, "ffi_init");
            if (!ffi_init)
            {
                printf("ffi_init not found!\n");
                result = jerry_create_undefined();
                dlclose(handle);
                break;
            }

            result = ffi_init();
            info = jerry_create_object();
            jerry_set_object_native_pointer(info, handle, &js_module_info);
            js_set_property(result, "info", info);
            jerry_release_value(info);
        }
        while (0);
    }
    else
#endif
    {
        /* make new module.exports */
        jerry_value_t global_obj  = jerry_get_global_object();
        jerry_value_t modules_obj = ECMA_VALUE_UNDEFINED;
        jerry_value_t exports_obj = ECMA_VALUE_UNDEFINED;

        modules_obj = js_get_property(global_obj, "module");
        exports_obj = js_get_property(modules_obj, "exports");

        jerry_value_t module_exports_obj = jerry_create_object();
        js_set_property(modules_obj, "exports", module_exports_obj);
        jerry_release_value(module_exports_obj);

        result = jerryx_module_resolve(args[0], resolvers, sizeof(resolvers) / sizeof(resolvers[0]));
        if (jerry_value_is_error(result))
        {
            printf("Couldn't load module %s\n", module);

            jerry_release_value(result);

            /* create result with error */
            result = jerry_create_error(JERRY_ERROR_TYPE,
                                        (const jerry_char_t *) "Module not found");
        }

        /* restore the parent module.exports */
        js_set_property(modules_obj, "exports", exports_obj);

        jerry_release_value(global_obj);
        jerry_release_value(modules_obj);
        jerry_release_value(exports_obj);
    }

    free(module);

    return result;
}

int js_module_init(void)
{
    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t modules_obj = jerry_create_object();
    jerry_value_t exports_obj = jerry_create_object();

    js_set_property(modules_obj, "exports", exports_obj);
    js_set_property(global_obj, "module", modules_obj);

    REGISTER_HANDLER(require);

    jerry_release_value(global_obj);
    jerry_release_value(modules_obj);
    jerry_release_value(exports_obj);

    return 0;
}
