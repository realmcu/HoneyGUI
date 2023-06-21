/*
 * File      : gui_config.h
 */
#ifndef __GUI_CONFIG_H__
#define __GUI_CONFIG_H__
#ifdef __cplusplus
extern "C" {
#endif

#define RTK_GUI_IMAGE_FS
#define RTK_GUI_IMAGE_MEM
// #define RTK_GUI_FONT_FS
#define RTK_GUI_FONT_MEM                3  //size of font_lib
#define RTK_GUI_MEMHEAP_OS              1
#define RTK_GUI_FONT_FREETYPE           0
#define RTK_GUI_FONT_STB                1
// #define RTK_GUI_TTF_SVG
#define RTK_GUI_DEFAULT_FONT_SIZE       24

#define RTK_GUI_GPU_SW                  1
#define RTK_GUI_GPU_ARM2D               0
#define RTK_GUI_GPU_DMA2D               0
#define RTK_GUI_GPU_VGLITE              0
#define RTK_GUI_GPU_OPENGL              0
#define RTK_GUI_GPU_OPENVG              0
#define RTK_GUI_GPU_NANOVG              0
#define MODULE_GPU_NANOVG
//#define RTK_GUI_SCRIPT_APP
#ifdef __cplusplus
}
#endif

#endif
