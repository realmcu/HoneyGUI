/*
 * File      : engine_gray.h
 */
#ifndef __ENGINE_GRAY_H__
#define __ENGINE_GRAY_H__

#include <guidef.h>
#include <gui_api.h>
#include <gui_matrix.h>
#include <draw_img.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct engine_gray
{
    int16_t x; //display start X
    int16_t y;  //display start Y
    uint16_t w;
    uint16_t h;
    void *data;
    GUI_FormatType format;
    // gui_matrix_t matrix; //seems can remve by howie
    gui_matrix_t inverse;
} engine_gray_t;




void engine_gray16_blit_to_dc(engine_gray_t *gray16, struct gui_dispdev *dc, gui_rect_t *rect);
void engine_gray256_blit_to_dc(engine_gray_t *gray256, struct gui_dispdev *dc, gui_rect_t *rect);


#ifdef __cplusplus
}
#endif

#endif

