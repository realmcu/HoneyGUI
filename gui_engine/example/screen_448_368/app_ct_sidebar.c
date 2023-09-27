#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_canvas.h"
#include "gui_seekbar.h"
#include "gui_page.h"
#include "gui_grid.h"
#include "gui_switch.h"
static void canvas_cb(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 368, 448);
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 0, 150));
    nvgFill(canvas->vg);
}

void page_ct_sidebar(void *parent)
{
    static void *array[] = {DOG4_BIN, DOG8_BIN, DOG12_BIN, DOG16_BIN, DOG20_BIN, DOG24_BIN, DOG28_BIN, DOG32_BIN, DOG36_BIN,
                            DOG40_BIN,
                            DOG44_BIN,
                            DOG48_BIN,
                            DOG52_BIN,
                            DOG56_BIN,
                            DOG60_BIN,
                            DOG64_BIN,
                            DOG68_BIN,
                            DOG72_BIN,
                            DOG76_BIN,

                            DOG80_BIN,
                            DOG84_BIN,
                            DOG88_BIN,
                            DOG92_BIN,
                            DOG96_BIN,
                            DOG100_BIN,
                            DOG104_BIN,
                            DOG108_BIN,
                            DOG112_BIN,
                            DOG116_BIN,

                            DOG120_BIN,
                            DOG124_BIN,
                            DOG128_BIN,
                            DOG132_BIN,
                            DOG136_BIN,
                            DOG140_BIN,
                            DOG144_BIN,
                            DOG148_BIN,
                            DOG152_BIN,
                            DOG156_BIN,

                            DOG160_BIN,
                            DOG164_BIN,
                            DOG168_BIN,
                            DOG172_BIN,
                            DOG176_BIN,
                            DOG180_BIN,
                            DOG184_BIN,
                            DOG188_BIN,
                            DOG192_BIN,
                            DOG196_BIN,

                            DOG200_BIN,
                            DOG204_BIN,
                            DOG208_BIN,
                            DOG212_BIN,
                            DOG216_BIN,
                            DOG220_BIN,
                            DOG224_BIN,
                            DOG228_BIN,
                            DOG232_BIN,
                            DOG236_BIN,

                            DOG240_BIN,
                            DOG244_BIN,
                            DOG248_BIN,
                            DOG252_BIN,
                            DOG256_BIN,
                            DOG260_BIN,
                            DOG264_BIN,
                            DOG268_BIN,
                            DOG272_BIN,
                            DOG276_BIN,

                            DOG280_BIN,
                            DOG284_BIN,
                            DOG288_BIN,
                            DOG292_BIN,
                            DOG296_BIN,
                            DOG300_BIN,
                            DOG304_BIN,
                            DOG308_BIN,
                            DOG312_BIN,
                            DOG316_BIN,

                            DOG320_BIN,
                            DOG324_BIN,
                            DOG328_BIN,
                            DOG332_BIN,
                            DOG336_BIN,
                            DOG340_BIN,
                            DOG344_BIN,
                            DOG348_BIN,
                            DOG352_BIN,
                            DOG356_BIN,

                            DOG360_BIN,


                           };
    gui_canvas_t *canvas = gui_canvas_create(parent, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb);
    gui_seekbar_create_movie_v(parent, array, 90, 80, 0);
    gui_page_t *page = gui_page_create(parent, "page", 0, 0, 368, 448);
    gui_grid_t *grid = gui_grid_create(page, 180, 0, 1, 20, 100, 150);
    gui_switch_t *sw_no_disturb  = gui_switch_create(grid, 10, 108, 169, 98, NO_DISTURB_OFF_BIN,
                                                     NO_DISTURB_ON_BIN);
    gui_switch_t *sw_notice      = gui_switch_create(grid, 190, 108, 169, 98, NOTICE_OFF_BIN,
                                                     NOTICE_ON_BIN);
    gui_switch_t *sw_call        = gui_switch_create(grid, 10, 220, 169, 98, CALL_OFF_BIN,
                                                     CALL_ON_BIN);
    gui_switch_t *sw_bright      = gui_switch_create(grid, 190, 220, 169, 98, BRIGHT_OFF_BIN,
                                                     BRIGHT_ON_BIN);
    gui_switch_t *sw_watch       = gui_switch_create(grid, 10, 332, 169, 98, WATCH_OFF_BIN,
                                                     WATCH_ON_BIN);
    gui_switch_t *sw_set         = gui_switch_create(grid, 190, 332, 169, 98, SET_OFF_BIN,
                                                     SET_ON_BIN);
    gui_switch_t *sw_vibrate     = gui_switch_create(grid, 10, 108, 169, 98, VIBRATE_OFF_BIN,
                                                     VIBRATE_ON_BIN);
    gui_switch_t *sw_emotion     = gui_switch_create(grid, 10, 220, 169, 98, EMOTION_OFF_BIN,
                                                     EMOTION_ON_BIN);
    gui_switch_t *sw_flashlight  = gui_switch_create(grid, 190, 220, 169, 98, FLASHLIGHT_OFF_BIN,
                                                     FLASHLIGHT_ON_BIN);
    gui_switch_t *sw_signal      = gui_switch_create(grid, 10, 332, 169, 98, SIGNAL_OFF_BIN,
                                                     SIGNAL_ON_BIN);
    gui_switch_t *sw_alarm_clock = gui_switch_create(grid, 190, 332, 169, 98,
                                                     ALARM_CLOCK_OFF_BIN, ALARM_CLOCK_ON_BIN);

}