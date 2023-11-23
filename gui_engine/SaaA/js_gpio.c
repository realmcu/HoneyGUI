#include "js_user.h"

DECLARE_HANDLER(writeSync)
{
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
            //if(write_value == 0){
            //led_msg.u.param = 0x64+gpio;
            //app_send_msg_to_apptask(&led_msg);}
            //else
            //{
            //    led_off_msg.u.param = 0x64+gpio;
            //    app_send_msg_to_apptask(&led_off_msg);
            // }
#ifdef ENABLE_MATTER_SWITCH
            if (gpio >= 0)
            {
                extern bool matter_send_msg_to_app(uint16_t sub_type, uint32_t param);

                uint32_t param = gpio << 8 | mode;
                if (gpio != 49052)
                {
                    //single
                    matter_send_msg_to_app(0, param);
                }
                else
                {
                    //group
                    matter_send_msg_to_app(1, param);
                }

                gui_log("gpio%d, %d, %d param %d", gpio, mode, write_value, param);
            }
#endif

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

    return jerry_create_undefined();
}

void js_gpio_init()
{
    jerry_value_t global_obj = jerry_get_global_object();
    REGISTER_METHOD(global_obj, Gpio);

}