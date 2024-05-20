#ifndef __WATCHFACE_MGR_H__
#define __WATCHFACE_MGR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gui_app.h>
#include "gui_tabview.h"
#include "root_image/ui_resource.h"

#define DOUBLE_CLICK_INTERVAL 300

typedef enum
{
    WATCHFACE_MAIN,
    WATCHFACE_BUDS,
    WATCHFACE_BUDS_DEVICE,
    WATCHFACE_BUDS_SEARCH,
    WATCHFACE_PHONE,
    WATCHFACE_PHONE_PAIR,
    WATCHFACE_SETTING,
    WATCHFACE_CONFIRM,
    WATCHFACE_CUBE,
    WATCHFACE_PERSPECTIVE,
    WATCHFACE_MAX
} watchface_ids;

//watchface mgr apis
int app_watchface_switch_tabs(watchface_ids id);

//watchface main apis
void design_tab_watchface_main(void *parent);

//watchface buds apis
void design_tab_buds(void *parent);

//watchface buds device apis
void design_tab_buds_device(void *parent);

//watchface buds search apis
void design_tab_buds_search(void *parent);

//watchface phone apis
void design_tab_phone(void *parent);

//watchface phone pair apis
void design_tab_phone_pair(void *parent);

//watchface setting apis
void design_tab_setting(void *parent);

//watchface confirm apis
void design_tab_confirm(void *parent);
void set_confirm_yes(void yes_action(void *), void *obj);
void set_confirm_no(void no_action(void *), void *obj);
void set_confirm_text(char *confirm_text_display, int16_t x, int16_t y, uint16_t length);

//watchface cube apis
void design_tab_cube(void *parent);

//watchface perspective apis
void design_tab_perspective(void *parent);
void free_watchface_mgr(void);



#ifdef __cplusplus
}
#endif
#endif


