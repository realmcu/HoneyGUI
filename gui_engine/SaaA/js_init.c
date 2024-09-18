
#include "js_user.h"
#include "jcontext.h"

extern int js_console_init(void);
extern void jerry_port_set_default_context(jerry_context_t *context);
extern void *gui_malloc(size_t n);
extern void *gui_realloc(void *ptr_old, size_t n);
extern void gui_free(void *rmem);
jerry_value_t parsed_code;
jerry_value_t ret_value;

static void *context_alloc(size_t size, void *cb_data_p)
{
    void *js_buffer = NULL;
#ifdef RTL8763EP
    return malloc(size);
#elif defined RTL87x2G
    js_buffer = gui_lower_malloc((PKG_JMEM_HEAP_SIZE + 100) * 1024);
#elif defined RTL8762D
    js_buffer = (void *)(0x6900000);

#elif defined __WIN32
    static uint8_t buffer[(PKG_JMEM_HEAP_SIZE + 100) * 1024] = {0};
    js_buffer = buffer;
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
    scipt_buff = gui_lower_malloc(JS_SCRIPT_BUFF_SIZE);
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
    gui_lower_free(scipt_buff);
#elif defined RTL8762D
    // static
#else
    // WIN
    gui_free(scipt_buff);
#endif
}


void js_isReady(void)
{
    // inform other tasks
#ifdef ENABLE_MATTER_SWITCH
#ifdef RTL87x2G
    // wr_mode: 0 (signal),  1 (group), 2 (subscribe), 3 (shutdown subscribe), 4 (gui ready)
    extern bool matter_send_msg_to_app(uint16_t sub_type, uint32_t param);
    matter_send_msg_to_app(4, 0);
    gui_log("->Matter: js_isReady\n");
#endif
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
    extern void js_gui_init(void);
    js_gui_init();
    js_gpio_init();
    //js_fs_init();
    js_wifi_init();
    js_key_init();
    js_smarthome_init();
    js_matter_init();
    js_mesh_init();
    extern gui_app_t *get_app_launcher_frontend(void);
    gui_app_startup(get_app_launcher_frontend());
}
