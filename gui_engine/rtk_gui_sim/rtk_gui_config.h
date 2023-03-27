/*
 * @Author: howie_wang
 * @Date: 2022-03-31 13:33:29
 * @LastEditTime: 2022-04-02 15:50:00
 * @LastEditors: Please set LastEditors
 */
#ifndef RTK_GUI_CONFIG_H__
#define RTK_GUI_CONFIG_H__
// <<< Use Configuration Wizard in Context Menu >>>\n
/* Automatically generated file; DO NOT EDIT. */
/* rtk guiConfiguration */


// <c> 8762F_DEVICE
#define WS_USING_8762F_DEVICE
// </c>

// <c> 8762F_CMSIS
#define WS_USING_CMSIS
// </c>

// <h> 8762F_LIB

// <c> rom_ns_lib
#define WS_USING_ROM_NS_LIB
// </c>

// <c> bee3pro_sdk_lib
#define WS_USING_BEE3PRO_SDK_LIB
// </c>

// <c> cmse_lib
#define WS_USING_CMSE_LIB
// </c>

// </h>

// <e> OSIF Enabled
#define WS_USING_OSIF 1
#if (WS_USING_OSIF == 1)
// <q> freertos  - Enable freertos
#define WS_USING_FREERTOS 1
#endif
// </e>

// <h> Hardware Drivers Config
// <e> RTK_GPIO_ENABLED - GPIO peripheral driver
//==========================================================
#define BSP_USING_GPIO 1
// </e>

// <e> RTK_UART_ENABLED - UART peripheral driver
//==========================================================
#define BSP_USING_UART 1
#if (BSP_USING_UART == 1)
// <q> BSP_USING_UART0
#define BSP_USING_UART0 0
#endif
#if (BSP_USING_UART == 1)
// <q> BSP_USING_UART1
#define BSP_USING_UART1 0
#endif
#if (BSP_USING_UART == 1)
// <q> BSP_USING_UART2
#define BSP_USING_UART2 0
#endif
#if (BSP_USING_UART == 1)
// <q> BSP_USING_UART3
#define BSP_USING_UART3 0
#endif
#if (BSP_USING_UART == 1)
// <e> BSP_USING_UART4
#define BSP_USING_UART4 1
#endif

// <o> UART_CONFIG_BAUDRATE  -  Baudrate

// <0=> 9600 baud
// <1=> 19200 baud
// <2=> 115200 baud
// <3=> 230400 baud
// <4=> 460800 baud
// <5=> 921600 baud
// <6=> 2000000 baud
// <7=> 3000000 baud
// <8=> 4000000 baud
// <9=> 6000000 baud

#define UART4_CONFIG_BAUDRATE 2

// <q> BSP_USING_UART4_INT
#define BSP_USING_UART4_INT 1

// </e>

#if (BSP_USING_UART == 1)
// <e> BSP_USING_UART5
#define BSP_USING_UART5 0
#endif

// <o> UART_CONFIG_BAUDRATE  -  Baudrate

// <0=> 9600 baud
// <1=> 19200 baud
// <2=> 115200 baud
// <3=> 230400 baud
// <4=> 460800 baud
// <5=> 921600 baud
// <6=> 2000000 baud
// <7=> 3000000 baud
// <8=> 4000000 baud
// <9=> 6000000 baud

#define UART5_CONFIG_BAUDRATE 2

// <q> BSP_USING_UART5_INT
#define BSP_USING_UART5_INT 1

// </e>

// </e>

// <e> RTK_I2C_ENABLED - I2C peripheral driver
//==========================================================
#define BSP_USING_I2C 1
#if (BSP_USING_I2C == 1)
// <e> BSP_USING_I2C0
#define BSP_USING_I2C0 0
#endif
// <o> I2C_CONFIG_CLOCK  -  Clock

// <0=> 100k
// <1=> 200k
// <2=> 400k

#define I2C0_CONFIG_CLOCK 2
// </e>
#if (BSP_USING_I2C == 1)
// <e> BSP_USING_I2C1
#define BSP_USING_I2C1 1
#endif
// <o> I2C_CONFIG_CLOCK  -  Clock

// <0=> 100k
// <1=> 200k
// <2=> 400k

#define I2C1_CONFIG_CLOCK 2
// </e>
// </e>

// <e> RTK_RTC_ENABLED - RTC peripheral driver
//==========================================================
#define BSP_USING_RTC 1
#if (BSP_USING_RTC == 1)
// <q> BSP_USING_RTC_COMP0
#define BSP_USING_RTC_COMP0 1
#endif
#if (BSP_USING_RTC == 1)
// <q> BSP_USING_RTC_COMP1
#define BSP_USING_RTC_COMP1 0
#endif
#if (BSP_USING_RTC == 1)
// <q> BSP_USING_RTC_COMP2
#define BSP_USING_RTC_COMP2 0
#endif
#if (BSP_USING_RTC == 1)
// <q> BSP_USING_RTC_COMP3
#define BSP_USING_RTC_COMP3 0
#endif
// </e>

// <e> RTK_ADC_ENABLED - ADC peripheral driver
//==========================================================
#define BSP_USING_ADC 1
#if (BSP_USING_ADC == 1)
// <q> BSP_USING_ADC_CH0
#define BSP_USING_ADC_CH0 1
#endif
#if (BSP_USING_ADC == 1)
// <q> BSP_USING_ADC_CH1
#define BSP_USING_ADC_CH1 1
#endif
#if (BSP_USING_ADC == 1)
// <q> BSP_USING_ADC_CH2
#define BSP_USING_ADC_CH2 0
#endif
#if (BSP_USING_ADC == 1)
// <q> BSP_USING_ADC_CH3
#define BSP_USING_ADC_CH3 0
#endif
// </e>


// <e> RTK_LCD_ENABLED - LCD peripheral driver
#define BSP_USING_LCD 1
#if (BSP_USING_LCD == 1)
// <o> SELECT_LCD_INTERFACE

// <0=> DBIB
// <1=> DBIC
// <2=> EDPI
// <3=> MIPI

#define BSP_USING_LCDC
#define LCD_CONFIG_INTERFACE 3
#if (LCD_CONFIG_INTERFACE == 0)
#define BSP_USING_DBIB
#elif (LCD_CONFIG_INTERFACE == 1)
#define BSP_USING_DBIC
#elif (LCD_CONFIG_INTERFACE == 2)
#define BSP_USING_EDPI
#elif (LCD_CONFIG_INTERFACE == 3)
#define BSP_USING_DSI
#endif
// <o> SELECT_LCD_DRIVER

// <0=> NT35510
// <7=> ST7701S

#define LCD_CONFIG_DRIVER 7

#if (LCD_CONFIG_DRIVER == 0)
#define NT35510_480800_DBIB
#elif (LCD_CONFIG_DRIVER == 7)
#define ST7701S_480480_MIPI_VIDEO
#endif
#endif
// </e>

// <e> RTK_TOUCH_ENABLED - TOUCH peripheral driver
#define BSP_USING_TOUCH 1
#if (BSP_USING_TOUCH == 1)
// <o> I2C_CONFIG  -  0-1

// <0=> I2C0
// <1=> I2C1

#define TOUCH_I2C_CONFIG 1



// <o> SELECT_TOUCH_DRIVER

// <0=> TOUCH_GT911

#define TOUCH_CONFIG_DRIVER 0

#if (TOUCH_CONFIG_DRIVER == 0)
#define TOUCH_GT911
#endif
#endif
// </e>

// </h>


// <e> WS BLE
#define RTK_BT_TASK 1
#if (RTK_BT_TASK == 1)

// <e> WS BLE SDK Select
#define WS_USING_BLE_SDK 1
#if (WS_USING_BLE_SDK == 1)
// <o> BLE SDK

// <0=> TUYA SDK
// <1=> XIAOMI SDK

#define BLE_SDK_CONFIG 0
#if (BLE_SDK_CONFIG == 0)
#define TUYA_BLE_SDK
#endif
#endif
// </e>
// <e> BLE_SERVICE_ENABLED
#define WS_USING_BLE_SERVICE 1
#endif
#if (WS_USING_BLE_SERVICE == 1)
// <q> BAS_SERVICE
#define BAS_SERVICE 1
#endif
#if (WS_USING_BLE_SERVICE == 1)
// <q> IAS_SERVICE
#define IAS_SERVICE 0
#endif
#if (WS_USING_BLE_SERVICE == 1)
// <q> SIMPLE_BLE_SERVICE
#define SIMPLE_BLE_SERVICE 1
#endif
#if (WS_USING_BLE_SERVICE == 1)
// <q> WRISTBAND_PRIVATE_SERVICE
#define RTK_BLE_WRISTBAND 0
#endif
// </e>
// </e>

// <e> WS BT
#define RTK_BT_TASK 1
#if (RTK_BT_TASK == 1)
// <e> BT_PROFILE_ENABLED
#define WS_USING_BT_PROFILE 1
#endif
#if (WS_USING_BT_PROFILE == 1)
// <c> SDP
#define BT_PROFILE_SDP
// </c>
#endif
#if (WS_USING_BT_PROFILE == 1)
// <c> SPP
#define BT_PROFILE_SPP
// </c>
#endif
#if (WS_USING_BT_PROFILE == 1)
// <c> HFP
#define BT_PROFILE_HFP
// </c>
#endif
#if (WS_USING_BT_PROFILE == 1)
// <c> A2DP
#define BT_PROFILE_A2DP
// </c>
#endif
#if (WS_USING_BT_PROFILE == 1)
// <c> AVRCP
#define BT_PROFILE_AVRCP
// </c>
#endif
#if (WS_USING_BT_PROFILE == 1)
// <c> PBAP
#define BT_PROFILE_PBAP
// </c>
#endif
// </e>
// </e>

// <c> RTK GUI Config
#define MODULE_USING_RTK_GUI
// </c>

// <c> RTK GUI Demo
#define MODULE_USING_RTK_GUI_DEMO
// </c>

// <c> RTK GUI frame buffer type
#define RTK_GUI_FB_RAM_SINGLE
// </c>

// <e> Console Enabled
#define WS_USING_CONSOLE 1
#if (WS_USING_CONSOLE == 1)
// <q> LETTER_SHELL  - Enable Letter Shell
#define WS_USING_LETTER_SHELL 1
#endif
// <o> UART_CONFIG  -  3-4


// <3=> UART3
// <4=> UART4

#define LETTER_SHELL_UART_CONFIG 4
// </e>

// <e> Watch Clock Enabled
#define MODULE_USING_WATCH_CLOCK 1
// <o> RTC_COMP_CONFIG  -  0-3


// <0=> RTC_COMP0
// <1=> RTC_COMP1
// <2=> RTC_COMP2
// <3=> RTC_COMP3

#define WATCH_CLOCK_RTC_COMP 0
// </e>

// <e> Battery Check Enabled
#define MODULE_USING_BATTERY_CHECK 0
// <o> ADC_CH_CONFIG  -  0-7


// <0=> ADC_CH0
// <1=> ADC_CH1
// <2=> ADC_CH2
// <3=> ADC_CH3

#define BATTERY_CHECK_ADC_CH 0
// </e>

// <e> Battery Check Compare Enabled
#define MODULE_USING_BATTERY_CHECK_COMPARE 0
// <o> ADC_CH_CONFIG  -  0-7


// <0=> ADC_CH0
// <1=> ADC_CH1
// <2=> ADC_CH2
// <3=> ADC_CH3

#define BATTERY_CHECK_COMPARE_ADC_CH 1
// </e>

// <c> RTK DataBase
#define RTK_USING_DATABASE
// </c>
// <e> fal Enabled
#define PKG_USING_FAL 1
#if (PKG_USING_FAL == 1)
// <e> FLASHDB  - Enable flashDB
#define PKG_USING_FLASHDB 1
#if (PKG_USING_FLASHDB == 1)
// <q> FLASHDB_SAMPLE  - Enable flashDB sample test
#define FLASHDB_USING_SAMPLES 0
#endif
#endif

// </e>

// <<< end of configuration section >>>
#endif//RTK_GUI_CONFIG_H__
