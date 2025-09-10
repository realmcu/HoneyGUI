#ifndef __DASHBOARD_ADAPT_H__
#define __DASHBOARD_ADAPT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct gui_dashboard
{
    /* BT & WIFI status */
    uint32_t bt_status         : 1; // bluetooth status, 1: on, 0: off
    uint32_t wifi_status       : 1; // wifi status, 1: on, 0: off

    /* LED status */
    uint32_t led0_status       : 1; // 1: on, 0: off
    uint32_t led1_status       : 1;
    uint32_t led2_status       : 1;
    uint32_t led3_status       : 1;
    uint32_t led4_status       : 1;
    uint32_t led5_status       : 1;
    uint32_t led_turn_l_status : 1;
    uint32_t led_turn_r_status : 1;

    /* Map status */
    uint32_t map_data_update   : 1;
    uint32_t map_data_index    : 1;
    void *map_data[2];     // map data pointer

    uint16_t speed_val; // km/h
    uint16_t power_val; // kW
    uint16_t odo_val;   // km
    uint16_t soc_val;   // %
    uint16_t temp_val;  // °C

    char location[16];

} gui_dashboard_t;



/*============================================================================*
 *                         Functions
 *============================================================================*/
void gui_dashboard_info_register(gui_dashboard_t *info);

gui_dashboard_t *get_dashboard_info(void);

#ifdef __cplusplus
}
#endif
#endif


