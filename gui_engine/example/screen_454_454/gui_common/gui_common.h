#ifndef __GUI_COMMON_H__
#define __GUI_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_obj.h"
#include "gui_win.h"
#include "def_list.h"
#include "gui_tab.h"
#include "gui_interface.h"
#ifndef _WIN32
//for bt status
#include "app_main.h"
#include "app_cfg.h"
#endif

typedef void (*pConFunc)(void *parent);
typedef void (*pDeConFunc)();

typedef struct
{
    uint16_t tab_id;
    int idx;
    int idy;
    char *tab_name;
    pConFunc ctor;
    pDeConFunc dector;
} tab_info_t;

typedef struct
{
    gui_tab_t *p_tab;
    const tab_info_t *tab_info;
} app_tab_t;

typedef enum
{
    AUDIO_SUPPORT_INVALID,
    AUDIO_SUPPORT_SINK,
    AUDIO_SUPPORT_SOURCE,
    AUDIO_SUPPORT_BOTH
} T_AUDIO_SUPPORT;


#ifndef _WIN32
//for curtain_menu
T_STATE gui_get_bt_status(void);
T_AUDIO_SUPPORT gui_get_audio_support(void);

//for tab_buds
void gui_set_create_connect_by_user(bool flag);
bool gui_get_create_connect_by_user(void);
void gui_set_create_connect_by_user_addr(uint8_t *addr);
uint8_t *gui_get_create_connect_by_user_addr(void);

#endif

int app_tab_manager_ctor(void *parent, app_tab_t *tab);
int app_tab_manager_free(app_tab_t *tab);
int app_tab_manager_switch(app_tab_t *tab);
void app_gui_main_free(void);


#ifdef __cplusplus
}
#endif
#endif
