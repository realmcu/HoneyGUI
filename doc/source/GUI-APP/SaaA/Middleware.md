# Middleware

This page shows how the  UI switch corresponds to the peripheral switch.
## Light control
### Light swtich data

|data|value type|brief|
|---|---|---|
|gpio|number|index of light|
|direction|out / in|direction of signal|
|write value|number|0 for turning off / 1 for turning on |

- Refer to [https://www.npmjs.com/package/onoff#usage](https://www.npmjs.com/package/onoff#usage) for more information.
### GPIO light Switch
- Get gpio index , direction, and write value;
- Use gpio driver ```drv_pin_mode()``` & ```drv_pin_write()``` to operate it;
### MATTER light Switch
- Get gpio index, and write value;
- Tramsform data to matter protocol;
- Use ```matter_send_msg_to_app()``` to operate lights;
### MESH light Switch
- Get gpio index, and write value;
- Tramsform data to mesh protocol;
- Use ```matter_send_msg_to_app()``` to operate lights;


The following code example is the ```writeSync``` 's control light implementation for RTL87X2G. First get gpio value and direction value, then use specify driver api to operate light.
```c
#ifdef RTL87x2G
#define ENABLE_MATTER_SWITCH
#define ENABLE_MESH_SWITCH
#define ENABLE_GPIO_SWITCH
#endif

#if defined ENABLE_GPIO_SWITCH
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

#if defined ENABLE_MESH_SWITCH
#include "app_msg.h"
T_IO_MSG led_msg = {.type = IO_MSG_TYPE_LED_ON};
T_IO_MSG led_off_msg = {.type = IO_MSG_TYPE_LED_OFF};
#endif

#if defined ENABLE_MATTER_SWITCH
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


        if (gpio >= 0)
        {
            gui_log("gpio%d, %d, %d", gpio, mode, write_value);
/**
 * GPIO
*/
#ifdef ENABLE_GPIO_SWITCH
            if (!strcmp(direction, "out"))
            {
                mode = PIN_MODE_OUTPUT;
            }
            else if (!strcmp(direction, "in"))
            {
                mode = PIN_MODE_INPUT;
            }
            drv_pin_mode(gpio, mode);
            drv_pin_write(gpio, write_value);
#endif

/**
 * MESH
*/
#ifdef ENABLE_MESH_SWITCH					
            extern bool app_send_msg_to_apptask(T_IO_MSG *p_msg);
            if(write_value == 0){
            led_msg.u.param = 0x64+gpio;
            app_send_msg_to_apptask(&led_msg);}
            else
            {
                led_off_msg.u.param = 0x64+gpio;
                app_send_msg_to_apptask(&led_off_msg);
            }
#endif

/**
 * MATTER
*/								
#ifdef ENABLE_MATTER_SWITCH
            if (gpio >= 0)
            {
                extern bool matter_send_msg_to_app(uint16_t sub_type, uint32_t param);
                uint32_t param = gpio << 8 | write_value;
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
#endif								
									
        }

        gui_free(direction);
    }
    return jerry_create_undefined();
}
```





