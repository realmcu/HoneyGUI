#ifndef __RECORD_MGR_H__
#define __RECORD_MGR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gui_app.h>
#include "gui_tabview.h"
#include "root_image/ui_resource.h"
#include "gui_curtain.h"
#ifndef _WIN32
#include "fs_if.h"
#endif

typedef enum
{
    RECORD_MAIN,
    RECORD_FILES,
    RECORD_PLAY,
    RECORD_MAX
} record_ids;

//record mgr apis
int app_record_switch_tabs(record_ids id);

//record main apis
void design_tab_record_main(void *parent);

//record files apis
void design_tab_record_files(void *parent);

//record play apis
void design_tab_record_play(void *parent);
void switch_record_play_pause_touch_cb(void *obj, uint16_t event);
void free_record_mgr(void);

#ifdef __cplusplus
}
#endif
#endif
