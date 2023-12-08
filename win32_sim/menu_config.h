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
#define BUILD_USING_RTK_GUI     1

#if (BUILD_USING_RTK_GUI == 1)

// <c> RTK GUI Demo
//#define BUILD_USING_RTK_GUI_DEMO
// </c>

// <c> RTK GUI Demo 368 448
#define BUILD_USING_RTK_GUI_448_368_DEMO
// </c>


// <c> RTK GUI Demo 320 384
// #define BUILD_USING_RTK_GUI_320_384_DEMO
// </c>

// <c> RTK GUI Demo 320 385
// #define BUILD_USING_RTK_GUI_320_385_DEMO
// </c>

// <c> RTK GUI Use OS Heap
#define BUILD_USING_RTK_GUI_OS_HEAP
// </c>

// <c> RTK GUI Font Enable STB
#define BUILD_USING_RTK_GUI_FONT_STB
// </c>

// <c> RTK GUI Font Enable FREETYPE
//#define BUILD_USING_RTK_GUI_FONT_FREETYPE
// </c>

// <c> RTK GUI Font Enable RTK MEM
#define BUILD_USING_RTK_GUI_FONT_RTK_MEM
// </c>

// <c> RTK GUI Font Enable TTF SVG
//#define BUILD_USING_RTK_GUI_FONT_TTF_SVG
// </c>

// <c> RTK GUI Enable VGLITE GPU
//#define BUILD_USING_VG_LITE
// </c>

// <c> RTK GUI Enable PPE1.0
//#define BUILD_USING_RTK_PPE
// </c>

// <c> MODULE NANOVG VGLITE
//#define USING_NANOVG_HW_VGLITE
// </c>

// <c> RTK GUI Enable SasA
//#define BUILD_USING_SCRIPT_AS_A_APP
// </c>


#endif

// </e>

// <e> HoneyGUI Enable LVGL
#define BUILD_USING_LVGL_GUI     1

#if (BUILD_USING_LVGL_GUI == 1)

// <c> HoneyGUI Enable LVGL EXAMPLES
#define BUILD_USING_LVGL_EXAMPLES
// </c>

// <c> HoneyGUI Enable LVGL RLOTTIE
//#define BUILD_USING_LVGL_RLOTTIE
// </c>

#endif

// </e>


// </h>

// <<< end of configuration section >>>
#endif//RTK_GUI_CONFIG_H__
