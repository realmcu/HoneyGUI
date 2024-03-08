#include <gui_tabview.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "resource_8762g.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "draw_font.h"
#include "acc_engine.h"
#include "nanosvg.h"
#include "gui_img.h"
#include "time.h"
#include "gui_button.h"
#include "app_dashboard_launcher.h"
#include "app_dashboard_main_display.h"
#include "app_dashboard_connected_display.h"
#include "app_dashboard_data.h"
#include <trace.h>
#include "gap.h"
#include "profile_server.h"
#include "gap_msg.h"
#include <simple_ble_service.h>
#include "bas.h"
#include "gap_bond_le.h"
#include "app_msg.h"
#include "gap_conn_le.h"
#include "app_dashboard_data.h"
#include "watch_msg.h"
#include "gui_win.h"
//#include "app_lvgl.h"
/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @addtogroup  PERIPH_SEVER_CALLBACK Profile Server Callback Event Handler
    * @brief Handle profile server callback event
    * @{
    */
T_SERVER_ID simp_srv_id; /**< Simple ble service id*/
T_SERVER_ID bas_srv_id;  /**< Battery service id */
/** @} */ /* End of group PERIPH_SEVER_CALLBACK */
/** @defgroup  PERIPH_GAP_MSG GAP Message Handler
    * @brief Handle GAP Message
    * @{
    */
T_GAP_DEV_STATE gap_dev_state = {0, 0, 0, 0};                 /**< GAP device state */
T_GAP_CONN_STATE gap_conn_state = GAP_CONN_STATE_DISCONNECTED; /**< GAP connection state */

gui_app_t app_dashboard_launcher =
{
    .screen =
    {
        .name = "app_dashboard_launcher",
        .x    = 0,
        .y    = 0,
    },
    .thread_entry = app_dashboard_launcher_update_thread,
    .ui_design = app_dashboard_launcher_ui_design,
};

gui_app_t *get_app_dashboard_launcher(void)
{
    return &app_dashboard_launcher;
}

extern gui_win_t *win_connected_display;
extern gui_win_t *win_main_display;
void app_dashboard_launcher_update_thread(void *this)
{
    while (1)
    {
        app_dashboard_auto_refresh_data_demo();
        gui_thread_mdelay(25);
    }
}
#ifdef CONFIG_REALTEK_BUILD_LVGL_GUI
static void kb_button_cb(void *obj, gui_event_t e)
{
    gui_log("gui_switch_app_to lvgl line = %d \n", __LINE__);
    extern gui_app_t *get_app_lvgl(void);
    gui_switch_app(gui_current_app(), get_app_lvgl());
}
#endif
void app_dashboard_launcher_ui_design(gui_app_t *app)
{
    win_connected_display = gui_win_create(&(app->screen), "win_connected_display", 0, 0, 800, 480);
    win_main_display = gui_win_create(&(app->screen), "win_main_display", 0, 0, 800, 480);
#ifdef CONFIG_REALTEK_BUILD_LVGL_GUI
    gui_obj_add_event_cb(win_main_display, (gui_event_cb_t)kb_button_cb, GUI_EVENT_KB_DOWN_PRESSED,
                         NULL);//switch app
#endif
    gui_set_font_mem_resourse(28, QUICKSAND_SEMIBOLD_28_BIN, CP500_TABLE_BIN);
    gui_set_font_mem_resourse(32, GBK_32_32_DOT_BIN, GBK_UNICODE_TABLE_BIN);
    gui_set_font_mem_resourse(56, QUICKSAND_SEMIBOLD_56_BIN, CP500_TABLE_BIN);

    app_dashboard_create_main_display(win_main_display);
    app_dashboard_create_connected_display(win_connected_display);
    win_main_display->base.not_show = false;
    win_connected_display->base.not_show = true;
    app_dashboard_initialize_data();
}
