
#include "js_user.h"
extern int js_console_init(void);
extern void jerry_port_set_default_context(jerry_context_t *context);
extern void *gui_malloc(uint32_t n);
extern void *gui_realloc(void *ptr_old, uint32_t n);
extern void gui_free(void *rmem);
jerry_value_t parsed_code;
jerry_value_t ret_value;

static void *context_alloc(size_t size, void *cb_data_p)
{
#ifdef RTL8763EP
    return malloc(size);
#elif defined RTL87x2G
#include "app_section.h"
    APP_PSRAM_SECTION static uint8_t js_buffer[(PKG_JMEM_HEAP_SIZE + 100) * 1024] = {0};
    return (void *)js_buffer;
#elif defined RTL8762D
    return (void *)(0x6900000);

#elif defined __WIN32
    static uint8_t js_buffer[(PKG_JMEM_HEAP_SIZE + 100) * 1024] = {0};
#else
    js_buffer = malloc(size);
#endif

    return (void *)js_buffer;
}

// Only for js script loading !
static void *script_malloc(void)
{
#define JS_SCRIPT_BUFF_SIZE (100 * 1024)
    void *scipt_buff = NULL;

#ifdef RTL8763EP
    scipt_buff =  malloc(JS_SCRIPT_BUFF_SIZE);
#elif defined RTL87x2G
#include "app_section.h"
    APP_PSRAM_SECTION static uint8_t js_script_buffer[JS_SCRIPT_BUFF_SIZE] = {0};
    scipt_buff = (void *)js_script_buffer;
#elif defined RTL8762D
    scipt_buff = (void *)(0x6900000 + PKG_JMEM_HEAP_SIZE * 1024);
#else
    // WIN
    scipt_buff = gui_malloc(JS_SCRIPT_BUFF_SIZE);
#endif

    return scipt_buff;
}

// Only for js script loading !
static void script_free(void *scipt_buff)
{
#ifdef RTL8763EP
    free(scipt_buff);
#elif defined RTL87x2G
    // static
#elif defined RTL8762D
    // static
#else
    // WIN
    gui_free(scipt_buff);
#endif
}

void js_run_file(const char *file, gui_app_t  *app)
{
    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t app_property = js_get_property(global_obj, "app");
    jerry_set_object_native_pointer(app_property, app, NULL);
    jerry_release_value(global_obj);
    jerry_release_value(app_property);
    jerry_char_t *script = script_malloc();
    size_t length = js_read_file(file, (void *)(&script));
    if (length > 0)
    {
        /* Setup Global scope code */
        parsed_code = jerry_parse(NULL, 0, script, length, JERRY_PARSE_NO_OPTS);
        if (!jerry_value_is_error(parsed_code))
        {
            /* Execute the parsed source code in the Global scope */
            ret_value = jerry_run(parsed_code);
            /* Returned value must be freed */
            jerry_release_value(ret_value);
        }
        /* Parsed source code must be freed */
        jerry_release_value(parsed_code);
        script_free(script);
    }
}

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
    js_gui_init();
    js_gpio_init();
    //js_fs_init();
    wifi_init();
    js_smarthome_init();
    js_matter_init();
    extern gui_app_t *get_app_launcher_frontend(void);
    gui_app_startup(get_app_launcher_frontend());
}
