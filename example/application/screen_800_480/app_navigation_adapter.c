//app_nanigation_adapter.c
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "dashboard_adapt.h"
#include "gui_api_os.h"
#include <stdio.h>
#include "app_navigation_adapter.h"
#include "root_image_800_480/ui_resource.h"
#include "app_map.h"
#if defined(CONFIG_REALTEK_APP_DASHBOARD_DEMO_SUPPORT) && CONFIG_REALTEK_APP_DASHBOARD_DEMO_SUPPORT
#include "communicate_protocol.h"
#include "communicate_parse_navigation.h"
#include "communicate_parse.h"
#endif

/*============================================================================*
 *                           Constants
 *============================================================================*/
static char str_def1[] = {0xe5, 0x85, 0xac, 0xe9, 0x87, 0x8c, '\0'};
static char str_def2[] = {0xe8, 0xbf, 0x9b, 0xe5, 0x85, 0xa5, '\0'};
static char str_def3[] = {0xe7, 0xb1, 0xb3, '\0'};
static char str_def4[] = {0x20, 0xe8, 0xbf, 0x9b, 0xe5, 0x85, 0xa5, '\0'};

/*============================================================================*
 *                           Navigation Data Structure
 *============================================================================*/
typedef struct
{
    uint8_t navigation_msg[40];
    uint8_t navigation_num_len;
    uint8_t navigation_unit[40];
    uint8_t navigation_unit_len;
    uint8_t road_names[40];
    uint8_t road_num_len;
    uint8_t navi_icon_type; // 1: straight, 2: left, 3: right, 4: u-turn
} navi_data_t;

static navi_data_t current_navi_data = {0};

/*============================================================================*
 *                           Map Adapter Functions
 *============================================================================*/

/**
 * @brief Update map display buffers
 * @param buffer Pointer to map buffer data
 * @param length Length of buffer data
 */
void map_update_buffers_display(void *buffer, uint32_t length)
{
    gui_dashboard_t *info = get_dashboard_info();
    if (info != NULL && buffer != NULL)
    {
        // TODO: Implement map buffer update logic
        gui_log("Dashboard: Update map buffers, length=%d\n", length);
    }
}

void map_display_normal_navi_stop(void)
{
    // TODO: Stop normal navigation display
    gui_log("Dashboard: Stop normal navigation display\n");
}

void map_display_buffer_stop(void)
{
    // TODO: Stop buffer navigation display
    gui_log("Dashboard: Stop buffer navigation display\n");
}

/**
 * @brief Send ITP (Image Transfer Protocol) ACK
 */
void app_dashboard_data_send_itp_ack(void)
{
#if defined(CONFIG_REALTEK_APP_DASHBOARD_DEMO_SUPPORT) && CONFIG_REALTEK_APP_DASHBOARD_DEMO_SUPPORT
    struct protocol_pack p = {0};
    p.l2_cmd_id = NAVIGATION_INFORMATION_ID;
    p.l2_key = KEY_NAVIGTOR_IMAGE_TRANSFER;
    p.l2_length = 2;
    p.l2_payload[0] = MESSGAE_IMAGE_TRANSFER_HEADER_TYPE_ACK;
    p.l2_payload[1] = 0x00; // SUCCESS

    package_prepare_send(&p);
    gui_log("Dashboard: Send ITP ACK\n");
#endif
}

void app_navigation_update_icon(void)
{
    const uint8_t *image_data = NULL;
    const char *icon_name = "Unknown";

    switch (current_navi_data.navi_icon_type)
    {
    case 1:
        image_data = NAVI_C1_BIN;
        icon_name = "Straight";
        break;
    case 2:
        image_data = NAVI_C2_BIN;
        icon_name = "Left Turn";
        break;
    case 3:
        image_data = NAVI_C3_BIN;
        icon_name = "Right Turn";
        break;
    case 4:
        image_data = NAVI_C4_BIN;
        icon_name = "U-Turn";
        break;
    default:
        image_data = NAVI_C1_BIN;
        icon_name = "Default(Straight)";
        break;
    }

    if (image_data != NULL)
    {
        app_map_update_navi_icon(image_data);
        gui_log("Navigation: Updated icon to %s (type=%d)\n",
                icon_name, current_navi_data.navi_icon_type);
    }
    else
    {
        gui_log("Navigation: ERROR - image_data is NULL for type=%d\n",
                current_navi_data.navi_icon_type);
    }
}
void app_dashboard_data_update_navi_status(const uint8_t *pValue, uint16_t length)
{
    if (pValue == NULL || length < 3)
    {
        gui_log("Dashboard: Invalid navigation data\n");
        return;
    }

    gui_dashboard_t *info = get_dashboard_info();
    if (info == NULL)
    {
        gui_log("Dashboard: Dashboard info not initialized\n");
        return;
    }

    int32_t distance = 0;
    uint8_t navi_type = pValue[0];

    // Clear buffers
    memset(&current_navi_data, 0, sizeof(navi_data_t));

    // Determine navigation icon type and update turn signal based on turn direction
// #if defined(CONFIG_REALTEK_APP_DASHBOARD_DEMO_SUPPORT) && CONFIG_REALTEK_APP_DASHBOARD_DEMO_SUPPORT
    switch (navi_type)
    {
    case SIM_TURN_LEFT:
    case SIM_LEFT_FRONT:
    case SIM_LEFT_BACK:
        current_navi_data.navi_icon_type = 2; // Left turn - NAVI_C2_BIN
        info->led_turn_l_status = 1; // Turn on left turn signal
        info->led_turn_r_status = 0; // Turn off right turn signal
        break;
    case SIM_LEFT_TURN_AROUND:
        current_navi_data.navi_icon_type = 4; // U-turn - NAVI_C4_BIN
        info->led_turn_l_status = 1; // Turn on left turn signal for U-turn
        info->led_turn_r_status = 0;
        break;
    case SIM_TURN_RIGHT:
    case SIM_RIGHT_FRONT:
    case SIM_RIGHT_BACK:
        current_navi_data.navi_icon_type = 3; // Right turn - NAVI_C3_BIN
        info->led_turn_l_status = 0; // Turn off left turn signal
        info->led_turn_r_status = 1; // Turn on right turn signal
        break;
    default:
        current_navi_data.navi_icon_type = 1; // Straight - NAVI_C1_BIN
        info->led_turn_l_status = 0; // Turn off both turn signals
        info->led_turn_r_status = 0;
        break;
    }

    gui_log("Dashboard: Navigation direction=%d, turn_left=%d, turn_right=%d\n",
            navi_type, info->led_turn_l_status, info->led_turn_r_status);

    // Calculate distance (big-endian)
    distance = (pValue[1] << 8) + pValue[2];

    // Format navigation message
    if (distance >= 1000)
    {
        // Display in kilometers
        snprintf((char *)current_navi_data.navigation_msg,
                 sizeof(current_navi_data.navigation_msg),
                 "%d.%d", distance / 1000, (distance % 1000) / 100);
        current_navi_data.navigation_num_len = strlen((char *)current_navi_data.navigation_msg);

        // Set unit
        size_t def_len = strlen(str_def1);
        if (def_len < sizeof(current_navi_data.navigation_unit))
        {
            strncpy((char *)current_navi_data.navigation_unit, str_def1,
                    sizeof(current_navi_data.navigation_unit) - 1);
            current_navi_data.navigation_unit[sizeof(current_navi_data.navigation_unit) - 1] = '\0';
        }
        current_navi_data.navigation_unit_len = def_len;

        // Set road name prefix
        def_len = strlen(str_def2);
        if (def_len < sizeof(current_navi_data.road_names))
        {
            strncpy((char *)current_navi_data.road_names, str_def2,
                    sizeof(current_navi_data.road_names) - 1);
            current_navi_data.road_names[sizeof(current_navi_data.road_names) - 1] = '\0';
        }
        current_navi_data.road_num_len = def_len;

        // Append road name if available
        if (length > 3 && ((size_t)(length - 3) <= (sizeof(current_navi_data.road_names) - def_len)))
        {
            memcpy(current_navi_data.road_names + def_len, pValue + 3, length - 3);
            current_navi_data.road_num_len += length - 3;
        }
    }
    else
    {
        // Display in meters
        snprintf((char *)current_navi_data.navigation_msg,
                 sizeof(current_navi_data.navigation_msg),
                 "%d", distance);
        current_navi_data.navigation_num_len = strlen((char *)current_navi_data.navigation_msg);

        // Set unit
        size_t def_len = strlen(str_def3);
        if (def_len < sizeof(current_navi_data.navigation_unit))
        {
            strncpy((char *)current_navi_data.navigation_unit, str_def3,
                    sizeof(current_navi_data.navigation_unit) - 1);
            current_navi_data.navigation_unit[sizeof(current_navi_data.navigation_unit) - 1] = '\0';
        }
        current_navi_data.navigation_unit_len = def_len;

        // Set road name prefix
        def_len = strlen(str_def4);
        if (def_len < sizeof(current_navi_data.road_names))
        {
            strncpy((char *)current_navi_data.road_names, str_def4,
                    sizeof(current_navi_data.road_names) - 1);
            current_navi_data.road_names[sizeof(current_navi_data.road_names) - 1] = '\0';
        }
        current_navi_data.road_num_len = def_len;

        // Append road name if available
        if (length > 3 && ((size_t)(length - 3) <= (sizeof(current_navi_data.road_names) - def_len)))
        {
            memcpy(current_navi_data.road_names + def_len, pValue + 3, length - 3);
            current_navi_data.road_num_len += length - 3;
        }
    }
    app_navigation_update_icon();
    gui_log("Dashboard: Navigation updated - type=%d, distance=%d, icon=%d\n",
            navi_type, distance, current_navi_data.navi_icon_type);
}

/**
 * @brief Get navigation distance message
 */
void app_navigation_get_distance_msg(uint8_t **msg, uint8_t *len)
{
    if (msg != NULL && len != NULL)
    {
        *msg = current_navi_data.navigation_msg;
        *len = current_navi_data.navigation_num_len;
    }
}

/**
 * @brief Get navigation unit
 */
void app_navigation_get_unit(uint8_t **unit, uint8_t *len)
{
    if (unit != NULL && len != NULL)
    {
        *unit = current_navi_data.navigation_unit;
        *len = current_navi_data.navigation_unit_len;
    }
}

/**
 * @brief Get navigation road name
 */
void app_navigation_get_road_name(uint8_t **road, uint8_t *len)
{
    if (road != NULL && len != NULL)
    {
        *road = current_navi_data.road_names;
        *len = current_navi_data.road_num_len;
    }
}

/**
 * @brief Get navigation icon type
 */
uint8_t app_navigation_get_icon_type(void)
{
    return current_navi_data.navi_icon_type;
}
