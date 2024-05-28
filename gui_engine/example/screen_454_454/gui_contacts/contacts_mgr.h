#ifndef __CONTACTS_MGR_H__
#define __CONTACTS_MGR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gui_app.h>
#include "gui_tabview.h"
#include "root_image/ui_resource.h"
#ifndef _WIN32
#include "fs_if.h"
#endif

typedef enum
{
    CONTACTS_MAIN,
    CONTACTS_LIST,
    CONTACTS_DETAILS,
    CONTACTS_MAX
} contacts_ids;

//contacts mgr apis
int app_contacts_switch_tabs(contacts_ids id);
void free_contacts_mgr(void);

//contacts main apis
void design_tab_contacts_main(void *parent);

//contacts list apis
void design_tab_contacts_list(void *parent);
void design_tab_contacts_details(void *parent);


#ifdef __cplusplus
}
#endif
#endif
