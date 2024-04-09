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
#define CONFIG_REALTEK_BUILD_GUI     1

#if (CONFIG_REALTEK_BUILD_GUI == 1)

// <c> RTK GUI Demo
// #define CONFIG_REALTEK_BUILD_GUI_DEMO
// </c>

// <c> RTK GUI Demo 368 448
//#define CONFIG_REALTEK_BUILD_GUI_448_368_DEMO
// </c>

// <c> RTK GUI Demo 454 454
#define CONFIG_REALTEK_BUILD_GUI_454_454_DEMO
// </c>

// <c> RTK GUI Demo 280 456
// #define CONFIG_REALTEK_BUILD_GUI_280_456_DEMO
// </c>

// <c> RTK GUI Demo 368 448 FileSystem
// #define CONFIG_REALTEK_BUILD_GUI_448_368_FILESYSTEM_DEMO
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

// <c> RTK GUI Use OS Heap
// #define CONFIG_REALTEK_BUILD_GUI_OS_HEAP
// </c>

// <c> RTK GUI Font Enable STB
#define CONFIG_REALTEK_BUILD_GUI_FONT_STB
// </c>

// <c> RTK GUI Font Enable FREETYPE
//#define CONFIG_REALTEK_BUILD_GUI_FONT_FREETYPE
// </c>

// <c> RTK GUI Font Enable RTK MEM
#define CONFIG_REALTEK_BUILD_GUI_FONT_RTK_MEM
// </c>

// <c> RTK GUI Font Enable TTF SVG
//#define CONFIG_REALTEK_BUILD_GUI_FONT_TTF_SVG
// </c>

// <c> RTK GUI Enable VGLITE GPU
//#define CONFIG_REALTEK_BUILD_VG_LITE
// </c>

// <c> RTK GUI Enable PPE1.0
//#define CONFIG_REALTEK_BUILD_PPE
// </c>

// <c> RTK GUI Enable PPE2.0
// #define CONFIG_REALTEK_BUILD_PPEV2
// </c>

// <c> MODULE NANOVG VGLITE
//#define CONFIG_REALTEK_NANOVG_HW_VGLITE
// </c>

// <c> RTK GUI Enable SasA
//#define CONFIG_REALTEK_BUILD_SCRIPT_AS_A_APP
// </c>

// <c> RTK GUI Enable Painter Engine only enable for Bee3Pro and simulation
// #define CONFIG_REALTEK_BUILD_PAINTER_ENGINE
// </c>

#endif

// </e>

// <e> HoneyGUI Enable LVGL
#define CONFIG_REALTEK_BUILD_LVGL_GUI     0

#if (CONFIG_REALTEK_BUILD_LVGL_GUI == 1)

// <c> HoneyGUI Enable LVGL EXAMPLES
#define CONFIG_REALTEK_BUILD_LVGL_EXAMPLES
// </c>

// <c> HoneyGUI Enable LVGL RLOTTIE
//#define BUILD_USING_LVGL_RLOTTIE
// </c>

#endif
// <c> RTK GUI Enable SasA
//#define CONFIG_REALTEK_BUILD_WATCHFACE_UPDATE
// </c>
// </e>


// </h>

// <<< end of configuration section >>>
#endif//RTK_GUI_CONFIG_H__
