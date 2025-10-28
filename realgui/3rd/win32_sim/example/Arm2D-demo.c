#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "components_init.h"
#include "fb_sdl.h"
#include "nanovg.h"
#include "nanovg_agge.h"
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include "arm_2d.h"



static int ARM2D_example(void)
{
    printf("ARM2D example test code:\n");

    uint8_t *pixel = malloc(DRV_LCD_WIDTH * DRV_LCD_HIGHT * DRV_PIXEL_BITS / 8);

    memset(pixel, 0x0, DRV_LCD_WIDTH * DRV_LCD_HIGHT * DRV_PIXEL_BITS / 8);

    const arm_2d_tile_t ptTile = {    
        .tRegion = {        
            .tSize = {            
                .iWidth = DRV_LCD_WIDTH,            
                .iHeight = DRV_LCD_HIGHT,        
            },    
        },    
        .tInfo = {        
            .bIsRoot = true,        
            .bHasEnforcedColour = true,        
            .tColourInfo = {            
                .chScheme = ARM_2D_COLOUR_8BIT,        
            },    
        },    
        .pchBuffer = (uint8_t *)pixel,
    };
    //arm_2d_rgb32_fill_colour(&ptTile, NULL, 0xFF0000FF);
    extern const arm_2d_tile_t c_tilearm2d_testCCCN888;
    // arm_2d_rgb32_tile_copy(&c_tilearm2d_testCCCN888, &ptTile, 
    //                         NULL,
    //                         ARM_2D_CP_MODE_XY_MIRROR);


    arm_2d_location_t ptTileLoc = {
        .iX = 0,
        .iY = 0,
    };
    arm_2d_cccn888_tile_rotation(
        &c_tilearm2d_testCCCN888,
        &ptTile,
        NULL,
        ptTileLoc,
        45.0f,
        0x0
    );


    port_direct_draw_bitmap_to_lcd(0, 0, DRV_LCD_WIDTH, DRV_LCD_HIGHT, (uint8_t *)pixel);

    return 0;
}

GUI_INIT_APP_EXPORT(ARM2D_example);