/*
 * File      : draw_img.h
 */
#ifndef __DRAW_IMAGE_H__
#define __DRAW_IMAGE_H__

#include <guidef.h>
#include <gui_api.h>


#ifdef __cplusplus
extern "C" {
#endif

struct gui_rgb_data_head
{
    unsigned char scan : 1;
    unsigned char rsvd : 4;
    unsigned char compress : 3;
    char type;
    short w;
    short h;
    char version;
    char rsvd2;
};

typedef enum
{
    RGB565 = 0,
    RGB888 = 3,
    RGBA8888 = 4,
    BINARY = 5,
    ALPHAMASK = 9,
    RTZIP_COMPRESS = 10,
} GUI_FormatType;


typedef enum
{
    IMG_BYPASS_MODE = 0,
    IMG_FILTER_BLACK,
    IMG_MAGIC_MATRIX,
    IMG_TRANSPARENT_MODE,
} BLEND_MODE_TYPE;

typedef struct draw_img
{
    uint16_t img_x;
    uint16_t img_y;
    uint16_t img_w;
    uint16_t img_h;
    uint16_t target_w;
    uint16_t target_h;
    void *data;
    struct rtgui_matrix *matrix;
    struct rtgui_matrix *inverse;
    struct rtgui_image_engine *engine;
    unsigned char opacity_value;
    uint32_t blend_mode : 3;

} draw_img_t;

typedef struct rtgui_image_engine
{
    const char *name;
    gui_list_t list;

    /* image engine function */
    bool (*image_check)(draw_img_t *image);

    bool (*image_load)(draw_img_t *image, bool load);
    void (*image_unload)(draw_img_t *image);

    void (*image_blit)(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect);


} rtgui_image_engine_t;



/* init rtgui image system */
void rtgui_system_image_init(void);
void rtgui_image_load_scale(draw_img_t *img);
bool rtgui_image_create(draw_img_t *img, bool load);
void rtgui_image_destroy(draw_img_t *image);
void rtgui_image_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect);

/* register an image engine */
void rtgui_image_register_engine(struct rtgui_image_engine *engine);





#ifdef __cplusplus
}
#endif

#endif

