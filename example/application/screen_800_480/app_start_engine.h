/*============================================================================*
 *                        Start Engine View Header
 *============================================================================*/
#ifndef __APP_START_ENGINE_H__
#define __APP_START_ENGINE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                           Function Declarations
 *============================================================================*/

/**
 * @brief Switch from start_view to dashboard_view
 *
 * This function triggers the engine start animation and switches
 * from the start engine view to the main dashboard view.
 * Can be called from external key event handlers.
 */
void app_dashboard_start_view_to_main(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_START_ENGINE_H__ */
