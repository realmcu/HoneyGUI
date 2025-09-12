#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "components_init.h"
#include "fb_sdl.h"
#include "l3.h"
#include "desc_earth.txt"

#define MODEL_CANVAS_WIDTH 480
#define MODEL_CANVAS_HEIGHT 480

static float rot_x_angle = 0.0f;
static float rot_y_angle = 0.0f;

static void earth_global_cb(l3_model_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 10), 1, 32767,
                             90,  this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 0, 10, rot_x_angle, rot_y_angle, 0, 5);
}

static int lite3D_example(void)
{
    printf("Lite3D example test code!\n");

    uint8_t *pixel = malloc(DRV_LCD_WIDTH * DRV_LCD_HIGHT * sizeof(uint16_t));
    memset(pixel, 0x0, DRV_LCD_WIDTH * DRV_LCD_HIGHT * sizeof(uint16_t));

    l3_model_t *face_3d = l3_create_model((void *)(_acdesc), L3_DRAW_FRONT_AND_SORT, 0, 0,
                                          MODEL_CANVAS_WIDTH, MODEL_CANVAS_HEIGHT);

    l3_set_global_transform(face_3d, (l3_global_transform_cb)earth_global_cb);

    l3_set_target_canvas(face_3d, 0, 0, MODEL_CANVAS_WIDTH, MODEL_CANVAS_HEIGHT, 16/*LITE_RGB565*/,
                         pixel);

    l3_push(face_3d);
    l3_draw(face_3d);


    port_direct_draw_bitmap_to_lcd(0, 0, DRV_LCD_WIDTH, DRV_LCD_HIGHT, (uint8_t *)pixel);




    return 0;
}

// GUI_INIT_APP_EXPORT(lite3D_example);