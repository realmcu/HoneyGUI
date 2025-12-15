/*============================================================================*
 *                        Dashboard View Header
 *============================================================================*/
#ifndef __APP_DASHBOARD_H__
#define __APP_DASHBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                           Function Declarations
 *============================================================================*/

/**
 * @brief Switch dashboard display mode between dail and digital
 *
 * This function switches the dashboard display mode:
 * - From dail mode to digital mode
 * - From digital mode to dail mode
 *
 * Can be called from external key event handlers.
 */
void app_dashboard_switch_display_mode(void);

/**
 * @brief Exit dashboard and return to start view
 *
 * This function triggers the exit animation and switches
 * from the dashboard view back to the start engine view.
 *
 * Can be called from external key event handlers.
 */
void app_dashboard_exit_to_start(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_DASHBOARD_H__ */
