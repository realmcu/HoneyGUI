/**
 * @file hml_button.c
 * @brief HML Button Component (hg_button)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_api.h"
// #include "gui_button.h"  // TODO: Add when button API is available

gui_obj_t *hml_create_button(gui_obj_t *parent, ezxml_t node)
{
    (void)parent;
    (void)node;
    // TODO: Implement when gui_button API is available
    gui_log("hml_button: not implemented yet\n");
    return NULL;
}
