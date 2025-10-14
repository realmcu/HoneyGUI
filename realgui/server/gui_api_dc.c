/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_dc.c
  * @brief Application Programming Interface for UI
  * @details Display Device
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/5
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */


/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include "guidef.h"
#include "gui_api_dc.h"
#include "gui_api_os.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/



/*============================================================================*
 *                            Variables
 *============================================================================*/

static struct gui_dispdev *dc = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_dc_info_register(struct gui_dispdev *info)
{
    dc = info;

    if (dc->fb_height == dc->screen_height)
    {
        dc->pfb_type = PFB_X_DIRECTION;
        uint32_t last_section = (dc->screen_width % dc->fb_width) ? 1 : 0;
        dc->section_total = dc->screen_width / dc->fb_width + last_section;
    }
    else if (dc->fb_width == dc->screen_width)
    {
        dc->pfb_type = PFB_Y_DIRECTION;
        uint32_t last_section = (dc->screen_height % dc->fb_height) ? 1 : 0;
        dc->section_total = dc->screen_height / dc->fb_height + last_section;
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
    }
}

gui_dispdev_t *gui_get_dc(void)
{
    return dc;
}

uint32_t gui_get_screen_width(void)
{
    return dc->screen_width;
}
uint32_t gui_get_screen_height(void)
{
    return dc->screen_height;
}
int gui_flash_boost(void)
{
    if (dc->flash_seq_trans_enable)
    {
        return dc->flash_seq_trans_enable();
    }
    return 1;

}
int gui_flash_boost_disable(void)
{
    if (dc->flash_seq_trans_disable)
    {
        return dc->flash_seq_trans_disable();
    }
    return 1;

}

void gui_display_on(void)
{
    gui_log("gui_display_on \n");
    if (dc->lcd_power_on)
    {
        dc->lcd_power_on();
    }
}

void gui_display_off(void)
{
    gui_log("gui_display_off \n");
    if (dc->lcd_power_off)
    {
        dc->lcd_power_off();
    }
}
