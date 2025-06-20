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
/* rtk gui Configuration */




// <h> HoneyGUI Framework Config


// <e> HoneyGUI Enable RTK Real GUI
#define CONFIG_REALTEK_HONEYGUI     1

#if (CONFIG_REALTEK_HONEYGUI == 1)

// <h> HoneyGUI Demo Select
// <c> RTK GUI Base Demo
#define CONFIG_REALTEK_BUILD_REAL_BASE
// </c>

// <c> RTK GUI 3D Demo
// #define CONFIG_REALTEK_BUILD_REAL_3D
// </c>

// <c> RTK GUI Paint Engine Demo
// #define CONFIG_REALTEK_BUILD_REAL_PAINTENGINE
// </c>

// <c> RTK GUI NanoVG Demo
// #define CONFIG_REALTEK_BUILD_REAL_NANOVG
// </c>

// <c> RTK GUI Box2D Demo
// #define CONFIG_REALTEK_BUILD_REAL_BOX2D
// </c>

// <c> RTK GUI LiteGFX Demo
// #define CONFIG_REALTEK_BUILD_REAL_LITEGFX_DEMO
// </c>

// <c> RTK GUI Demo 368 448
// #define CONFIG_REALTEK_BUILD_GUI_448_368_DEMO
// </c>

// <c> RTK GUI Demo 454 454
//#define CONFIG_REALTEK_BUILD_GUI_454_454_DEMO
// </c>

// <c> RTK GUI TEST
// #define CONFIG_REALTEK_BUILD_TEST
// </c>

// <c> RTK GUI AUTO TEST
// #define CONFIG_REALTEK_BUILD_HONEYGUI_AUTO_TEST
// </c>

// <c> RTK GUI Demo 280 456
// #define CONFIG_REALTEK_BUILD_GUI_280_456_DEMO
// </c>

// <c> RTK GUI Demo 240 240
// #define CONFIG_REALTEK_BUILD_GUI_240_240_DEMO
// </c>

// <c> RTK GUI Demo 240 320
// #define CONFIG_REALTEK_BUILD_GUI_240_320_DEMO
// </c>

// <c> RTK GUI Demo 320 384
// #define CONFIG_REALTEK_BUILD_GUI_320_384_DEMO
// </c>

// <c> RTK GUI Demo 320 385
// #define CONFIG_REALTEK_BUILD_GUI_320_385_DEMO
// </c>

// <c> RTK GUI Demo 800 480
// #define CONFIG_REALTEK_BUILD_GUI_800_480_DEMO
// </c>

// </h>


// <h> HoneyGUI Config Function

// <c> RTK GUI Font Enable FREETYPE
// #define CONFIG_REALTEK_BUILD_GUI_FONT_FREETYPE
// </c>

// <c> RTK GUI Enable VGLITE GPU
// #define CONFIG_REALTEK_BUILD_VG_LITE
// </c>

// <c> RTK GUI Enable PPE1.0
// #define CONFIG_REALTEK_BUILD_PPE
// </c>

// <c> RTK GUI Enable PPE2.0
// #define CONFIG_REALTEK_BUILD_PPEV2
// </c>

// <c> RTK GUI Enable SasA
// #define CONFIG_REALTEK_BUILD_SCRIPT_AS_A_APP
// </c>

// <c> RTK GUI Enable cJSON
#define CONFIG_REALTEK_BUILD_CJSON
// </c>
// <c> RTK GUI Enable web
// #define CONFIG_REALTEK_BUILD_WEB
// </c>
// <c> RTK GUI Enable KeyBoard And Pinyin
#define CONFIG_REALTEK_BUILD_PINYIN
// </c>

// <c> RTK GUI Enable u8g2
// #define CONFIG_REALTEK_BUILD_U8G2
// </c>

// <c> RTK GUI Enable LiteGFX
// #define CONFIG_REALTEK_BUILD_LITE_GFX
// </c>

// <c> RTK GUI Enable LetterShell
//#define CONFIG_REALTEK_BUILD_LETTER_SHELL
// </c>

// <c> RTK GUI Enable Monkey Test Log
// #define CONFIG_REALTEK_BUILD_MONKEY_TEST
// </c>

// <c> RTK GUI BOX2D
//#define CONFIG_REALTEK_BUILD_GUI_BOX2D
// </c>


// <c> RTK GUI Enable SasA
// #define CONFIG_REALTEK_BUILD_WATCHFACE_UPDATE
// </c>

#endif
// </h>

// </e>

// <e> HoneyGUI Enable LVGL
#define CONFIG_REALTEK_BUILD_LVGL_GUI     0

#if (CONFIG_REALTEK_BUILD_LVGL_GUI == 1)

// <c> HoneyGUI Enalbe LVGL APP
#define CONFIG_REALTEK_BUILD_LVGL_DEMO_APP
// </c>

// <c> HoneyGUI Enable LVGL EXAMPLES
#define CONFIG_REALTEK_BUILD_LVGL_EXAMPLES
// </c>

// <c> HoneyGUI Enable LVGL RLOTTIE
#define CONFIG_REALTEK_BUILD_LVGL_RLOTTIE
// </c>

#endif

// </e>

// <e> HoneyGUI Enable Arm-2D
#define CONFIG_REALTEK_BUILD_ARM_2D     0

#if (CONFIG_REALTEK_BUILD_ARM_2D == 1)

// <c> HoneyGUI Enalbe ARM2D APP
#define CONFIG_REALTEK_BUILD_ARM2D_DEMO_APP
// </c>

#endif

// </e>

// <e> HoneyGUI Enable Legacy RTK GUI
#define CONFIG_REALTEK_BUILD_LEGACY_RTK_GUI     0

#if (CONFIG_REALTEK_BUILD_LEGACY_RTK_GUI == 1)
#endif

// </e>


// </h>

// <<< end of configuration section >>>
#endif//RTK_GUI_CONFIG_H__
