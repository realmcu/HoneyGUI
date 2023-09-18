
#include "gui_seekbar.h"
#include "root_image/resource_8772f.h"

void page_tb_carousel(void *parent)
{
    static  void *array[] =
    {
        CAROUSEL_0_BIN, CAROUSEL_3_BIN,       CAROUSEL_9_BIN,
        CAROUSEL_12_BIN,
        CAROUSEL_21_BIN,
        CAROUSEL_24_BIN,
        CAROUSEL_27_BIN,

        CAROUSEL_33_BIN,
        CAROUSEL_36_BIN,
        CAROUSEL_42_BIN,
        CAROUSEL_48_BIN,
        CAROUSEL_51_BIN,
        CAROUSEL_54_BIN,
        CAROUSEL_60_BIN,
        CAROUSEL_63_BIN,
        CAROUSEL_66_BIN,
        CAROUSEL_69_BIN,
        CAROUSEL_72_BIN,
        CAROUSEL_75_BIN,
        CAROUSEL_84_BIN,
        CAROUSEL_87_BIN,

        CAROUSEL_90_BIN,
    };

    gui_seekbar_create_movie_h(parent, array, 20, 180, 180);
}
