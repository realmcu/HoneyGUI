
#include "gui_seekbar.h"
#include "rtk_gui_resource.h"

void page_tb_carousel(void *parent)
{
    static  void *array[] =
    {
        CAROUSEL_0_BIN, CAROUSEL_2_BIN, CAROUSEL_4_BIN, CAROUSEL_6_BIN, CAROUSEL_8_BIN,
        CAROUSEL_10_BIN,
        CAROUSEL_12_BIN,
        CAROUSEL_14_BIN,
        CAROUSEL_16_BIN,
        CAROUSEL_18_BIN,

        CAROUSEL_20_BIN,
        CAROUSEL_22_BIN,
        CAROUSEL_24_BIN,
        CAROUSEL_26_BIN,
        CAROUSEL_28_BIN,
        CAROUSEL_30_BIN,
        CAROUSEL_32_BIN,
        CAROUSEL_34_BIN,
        CAROUSEL_35_BIN,
        CAROUSEL_37_BIN,
        CAROUSEL_39_BIN,

        CAROUSEL_41_BIN,
        CAROUSEL_42_BIN,
        CAROUSEL_44_BIN,
        CAROUSEL_45_BIN,
        CAROUSEL_47_BIN,
        CAROUSEL_49_BIN,
        CAROUSEL_51_BIN,
        CAROUSEL_53_BIN,
        CAROUSEL_54_BIN,
        CAROUSEL_55_BIN,
        CAROUSEL_57_BIN,
        CAROUSEL_59_BIN,

        CAROUSEL_60_BIN,
        CAROUSEL_61_BIN,
        CAROUSEL_62_BIN,
        CAROUSEL_64_BIN,
        CAROUSEL_66_BIN,
        CAROUSEL_67_BIN,
        CAROUSEL_69_BIN,
        CAROUSEL_71_BIN,
        CAROUSEL_72_BIN,
        CAROUSEL_73_BIN,
        CAROUSEL_74_BIN,
        CAROUSEL_76_BIN,
        CAROUSEL_78_BIN,

        CAROUSEL_80_BIN,
        CAROUSEL_81_BIN,
        CAROUSEL_83_BIN,
        CAROUSEL_84_BIN,
        CAROUSEL_86_BIN,
        CAROUSEL_88_BIN,
        CAROUSEL_89_BIN,
        CAROUSEL_90_BIN,


    };

    gui_seekbar_create_movie_h(parent, array, 50, 180, 180);
}
