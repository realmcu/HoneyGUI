
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_img.h>
#include <gui_text.h>
#include <gui_curtain.h>

void  page_left_figure(void *parent)
{

#ifdef RTL8762G
    gui_img_t *background =  gui_img_create_from_mem(parent, "background_left",
                                                     ZIP_RECT_248_384_BIN,
                                                     72, 0, 0, 0);
#else
    gui_img_t *background =  gui_img_create_from_mem(parent, "background_left",
                                                     RECT_248_384_BIN,
                                                     72, 0, 0, 0);
#endif // RTL8762G
    gui_img_set_opacity(background, 128);

    gui_img_t *time =  gui_img_create_from_mem(parent, "time", TIME_BIN, 161, 20, 0, 0);
    gui_img_t *data =  gui_img_create_from_mem(parent, "data", DATA_BIN, 132, 64, 0, 0);
    gui_img_t *week =  gui_img_create_from_mem(parent, "week", SUN_BIN, 213, 64, 0, 0);

#ifdef RTL8762G
    gui_img_t *figure_bk0 =  gui_img_create_from_mem(parent, "figure_bk0", ZIP_RECT_110_108_BIN,
                                                     82, 98, 0, 0);
#else
    gui_img_t *figure_bk0 =  gui_img_create_from_mem(parent, "figure_bk0", RECT_110_108_BIN,
                                                     82, 98, 0, 0);
#endif // RTL8762G
    gui_img_set_opacity(figure_bk0, 200);
    gui_img_t *sky =  gui_img_create_from_mem(parent, "sky", SKY1_BIN, 123, 112, 0, 0);
    gui_img_t *centigrade0 =  gui_img_create_from_mem(parent, "centigrade", ASSEMBLY_BIN,
                                                      106, 183, 0, 0);
    gui_img_t *centigrade1 =  gui_img_create_from_mem(parent, "centigrade", ASSEMBLY_BIN,
                                                      124, 183, 0, 0);
    gui_img_t *centigrade2 =  gui_img_create_from_mem(parent, "centigrade", CENTIGRADE_BIN,
                                                      132, 166, 0, 0);

    gui_img_t *map =  gui_img_create_from_mem(parent, "siri", MAP_BIN, 202, 98, 0, 0);

#ifdef RTL8762G
    gui_img_t *figure_bk1 =  gui_img_create_from_mem(parent, "figure_bk1", ZIP_RECT_228_64_BIN,
                                                     82, 214, 0, 0);
#else
    gui_img_t *figure_bk1 =  gui_img_create_from_mem(parent, "figure_bk1", RECT_228_64_BIN,
                                                     82, 214, 0, 0);
#endif // RTL8762G
    gui_img_set_opacity(figure_bk1, 200);
    gui_img_t *heart_rate =  gui_img_create_from_mem(parent, "heart_rate", HEART_RATE_BIN,
                                                     98, 225, 0, 0);
    gui_img_t *sound =  gui_img_create_from_mem(parent, "sound", SOUND_RECORDING_BIN, 150,
                                                225, 0, 0);
    gui_img_t *sound_left =  gui_img_create_from_mem(parent, "sound", SOUND_LINE_BIN, 122,
                                                     315, 0, 0);
    // gui_img_t *sound_right =  gui_img_create_from_mem(parent, "sound", SOUND_RIGHT_BIN, 206,
    //                                                   317, 0, 0);
    gui_img_t *sport =  gui_img_create_from_mem(parent, "sport", SPORT_BIN, 202, 225, 0, 0);
    gui_img_t *ring =  gui_img_create_from_mem(parent, "ring", RING_BIN, 254, 225, 0, 0);

    gui_img_t *siri =  gui_img_create_from_mem(parent, "siri", SIRI_60_60_BIN, 161, 297, 0, 0);
}
