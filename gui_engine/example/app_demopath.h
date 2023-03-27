/*
 * File      : app_demopath.h
 */
#ifndef __APP_DEMOPATH_H__
#define __APP_DEMOPATH_H__

#ifdef __cpluscplus
extern "C" {
#endif



#include "vg_lite.h"
#include <gui_img.h>
    static char path_data[] =
    {
        2, 95, 90, // moveto   -5,-10
        4, 105, 90,  // lineto    5,-10
        4, 110, 95,  // lineto   10, -5
        4, 100, 100,    // lineto    0,  0
        4, 110, 105,   // lineto   10,  5
        4, 105, 110,   // lineto    5, 10
        4, 95, 110,  // lineto   -5, 10
        4, 90, 105,  // lineto  -10,  5
        4, 90, 95, // lineto  -10, -5
        0, // end
    };

    static vg_lite_path_t path1 =
    {
        {
            90, 90, // left,top
            110, 110
        }, // right,bottom
        VG_LITE_HIGH, // quality
        VG_LITE_S8, // -128 to 127 coordinate range
        {0}, // uploaded
        sizeof(path_data), // path length
        path_data, // path data
        1
    };
    gui_demo_path_t path_demo_1 =
    {
        &path1,
        0xffffff00
    };

#ifdef __cplusplus
}
#endif


#endif


