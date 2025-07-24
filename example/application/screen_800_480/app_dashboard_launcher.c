#include "app_dashboard_launcher.h"
#include "app_dashboard_main_display.h"
#include "gui_components_init.h"
#include "app_dashboard_connected_display.h"
#include "app_dashboard_data.h"
#ifndef _WIN32
#include <trace.h>
#include "gap.h"
#include "profile_server.h"
#include "gap_msg.h"
#include <simple_ble_service.h>
#include "bas.h"
#include "gap_bond_le.h"
#include "app_msg.h"
#include "gap_conn_le.h"
#include "watch_msg.h"
#include "os_mem.h"
#endif // !_WIN32

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "dashboard_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_dashboard_launcher_ui_design(gui_view_t *view);

/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @addtogroup  PERIPH_SEVER_CALLBACK Profile Server Callback Event Handler
    * @brief Handle profile server callback event
    * @{
    */
#ifndef _WIN32
T_SERVER_ID simp_srv_id; /**< Simple ble service id*/
T_SERVER_ID bas_srv_id;  /**< Battery service id */
/** @} */ /* End of group PERIPH_SEVER_CALLBACK */
/** @defgroup  PERIPH_GAP_MSG GAP Message Handler
    * @brief Handle GAP Message
    * @{
    */
T_GAP_DEV_STATE gap_dev_state = {0, 0, 0, 0};                 /**< GAP device state */
T_GAP_CONN_STATE gap_conn_state = GAP_CONN_STATE_DISCONNECTED; /**< GAP connection state */
#endif

/* VIEW */
const static gui_view_descriptor_t *main_view = NULL;
const static gui_view_descriptor_t *connect_view = NULL;

#if defined _WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    main_view = gui_view_descriptor_get("main_view");
    connect_view = gui_view_descriptor_get("connect_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


void app_dashboard_launcher_update_thread(void *p)
{
    app_dashboard_auto_refresh_data_demo();
}

static int app_init(void)
{
#if defined _WIN32
    int fd;
    extern int open(const char *file, int flags, ...);
    extern int read(int fd, void *buf, size_t len);
    extern int close(int fd);
    fd = open("example\\application\\screen_800_480\\root_image_800_480\\root(0x4400000).bin", 0);
    if (fd < 0)
    {
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }
    printf("open root(0x4400000).bin Successful!\n");
    ssize_t bytes_read = read(fd, resource_root, 1024 * 1024 * 20);
    if (bytes_read < 0)
    {
        printf("read bin file failed!\n");
        close(fd);
        return 0;
    }
    close(fd);
#endif
    gui_win_t *win = gui_win_create(gui_obj_get_root(), "app_dashboard_win", 0, 0, 0, 0);
    gui_obj_create_timer(GUI_BASE(win), 25, true, app_dashboard_launcher_update_thread);
    gui_view_create(win, main_view, 0, 0, 0, 0);
    app_dashboard_initialize_data();
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
