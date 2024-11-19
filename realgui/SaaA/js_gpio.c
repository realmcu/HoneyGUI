#include "js_user.h"
#if defined ENABLE_GPIO_SWITCH
#ifdef RTL87x2G
#include "rtl_gpio.h"
#include "rtl_rcc.h"
#include "drv_gpio.h"
#include "drv_i2c.h"
#include "drv_touch.h"
#include "drv_lcd.h"
#include "touch_gt911.h"
#include "string.h"
#include "trace.h"
#include "utils.h"
#endif
#endif

#if defined ENABLE_MESH_SWITCH
#include "app_msg.h"

#if (defined RTL87x2G)||(defined RTL8762D)
T_IO_MSG led_msg = {.type = IO_MSG_TYPE_MESH_LIGHT, .subtype = IO_MSG_MESH_LIGHT_ON_GUI};
#endif

#endif

#if defined ENABLE_MATTER_SWITCH
#endif

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
        uint8_t wr_mode = (write_value >> 4) & 0x0F;
        uint8_t wr_val = write_value & 0x0F;


        if (gpio >= 0)
        {
            gui_log("gpio %d, IOmode %d, value 0x%x, wr_mode %d, wr_val %d\n", gpio, mode, write_value, wr_mode,
                    wr_val);
            /**
             * GPIO
            */
#ifdef ENABLE_GPIO_SWITCH
#ifdef RTL87x2G
            if (!strcmp(direction, "out"))
            {
                mode = PIN_MODE_OUTPUT;
            }
            else if (!strcmp(direction, "in"))
            {
                mode = PIN_MODE_INPUT;
            }
            drv_pin_mode(gpio, mode);
            drv_pin_write(gpio, wr_val);
#endif
#endif
            /**
             * MESH
            */
#ifdef ENABLE_MESH_SWITCH
#if (defined RTL87x2G)||(defined RTL8762D)
            extern bool app_send_msg_to_apptask(T_IO_MSG * p_msg);

            led_msg.u.param = 0x64 + gpio;
            if (wr_val == 0)
            {
                led_msg.subtype = IO_MSG_MESH_LIGHT_ON_GUI;
            }
            else
            {
                led_msg.subtype = IO_MSG_MESH_LIGHT_OFF_GUI;
            }
            app_send_msg_to_apptask(&led_msg);
#endif
#endif
            /**
             * MATTER
            */
#ifdef ENABLE_MATTER_SWITCH
#ifdef RTL87x2G
            // wr_mode: 0 (singal),  1 (group), 2 (subscribe), 3 (shutdown subscribe), 4 (gui ready)
            if (gpio >= 0)
            {
                extern bool matter_send_msg_to_app(uint16_t sub_type, uint32_t param);

                uint32_t param = (gpio << 8) | wr_val;
                matter_send_msg_to_app(wr_mode, param);

                gui_log("->Matter: gpio %d, wr_mode %d, wr_val %d param 0x%x\n", gpio, wr_mode, wr_val, param);
            }
#endif
#endif
        }

        gui_free(direction);
    }
    return jerry_create_undefined();
}

DECLARE_HANDLER(Gpio)
{
    // gui_log("enter GPIO\n");
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
    // js_set_string_property(this_value, "state", jerry_create_number(0));

    return jerry_create_undefined();
}

void js_gpio_init()
{
    jerry_value_t global_obj = jerry_get_global_object();
    REGISTER_METHOD(global_obj, Gpio);

}