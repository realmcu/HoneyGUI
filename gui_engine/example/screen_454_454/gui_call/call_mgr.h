#ifndef __CALL_MGR_H__
#define __CALL_MGR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gui_app.h>
#include "gui_tabview.h"
#include "root_image/ui_resource.h"

#define GUI_DIAL_NUM_MAX 24
#define BUFFER_DIAL_NUM_MAX 64

typedef enum
{
    CALL_MAIN,
    CALL_DIAL,
    CALL_INCOMMING,
    CALL_OUTGOING,
    CALL_ACTIVE,
    CALL_MAX,
} call_ids;

//call mgr apis
int app_call_switch_tabs(call_ids id);

//call main apis
void design_tab_call_main(void *parent);

//call dail apis
void design_tab_call_dial(void *parent);
void set_dial_num(char *dial_num, uint8_t length);
char *get_dial_num(void);

//call incomming apis
void design_tab_call_incomming(void *parent);

//call outgoing apis
void design_tab_call_outgoing(void *parent);

//call active apis
void design_tab_call_active(void *parent);

#ifdef __cplusplus
}
#endif
#endif
