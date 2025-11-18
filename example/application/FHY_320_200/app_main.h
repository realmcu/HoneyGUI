#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_view.h"
#include "gui_components_init.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SS_WITH_HEAD_TRACKING 1

#define QUICK_PAGE_NUM_MAX  4
#define SCROLL_BAR_S_H      25
#define SCROLL_BAR_M_H      50
#define SCROLL_BAR_L_H      75
#define SCROLL_BAR_BG_H     100



#define GUI_WIDGET_TRY_EXCEPT(obj) {GUI_ASSERT(obj != NULL);GUI_ASSERT((GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER))}

/**
* @brief Macro to get a widget pointer by name from the root of a GUI object tree.
*
* This macro assigns a widget pointer by searching for a widget with the specified name within the given root.
* If the widget is found, the pointer is set to it; otherwise, it remains NULL. Additionally, the macro handles
* exceptions if the widget retrieval fails.
*
* @param pointer The name of the pointer that will hold the widget.
* @param name The name of the widget to search for.
* @param root_widget The root widget of the GUI object tree to search within.
*/
#define GUI_WIDGET_POINTER_BY_NAME_ROOT(pointer, name, root_widget) gui_obj_t *pointer = 0;\
    extern void gui_obj_tree_get_widget_by_name(gui_obj_t *, const char *, gui_obj_t **);\
    gui_obj_tree_get_widget_by_name((void *)root_widget, name, &pointer);

/*============================================================================*
 *                            Variables
 *============================================================================*/
extern struct tm *timeinfo;

/* Detail view page design func */
extern void (*detail_page_design_func)(gui_obj_t *parent);

/* Quick view page design func */
extern int8_t quick_page_num;
extern void (*quick_page_design_func_array[QUICK_PAGE_NUM_MAX])(gui_obj_t *parent);

/* Battery */
extern char battery_earbuds_connect_l_str[4];
extern char battery_earbuds_connect_r_str[4];
extern char battery_tx_str[4];
/*============================================================================*
 *                           Punblic Functions
 *============================================================================*/
void update_icon_color(gui_obj_t *obj, uint8_t status);
void status_bar_design(gui_obj_t *parent);

/* Page design */
void page_playback_design(gui_obj_t *parent);
void page_volume_design(gui_obj_t *parent);
void page_equalizer_design(gui_obj_t *parent);
void page_ambient_sound_design(gui_obj_t *parent);
void page_smart_talk_design(gui_obj_t *parent);
#if SS_WITH_HEAD_TRACKING
void page_spatial_sound_with_head_tracking_design(gui_obj_t *parent);
#else
void page_spatial_sound_without_head_tracking_design(gui_obj_t *parent);
#endif
void page_hr_audio_design(gui_obj_t *parent);
void page_voice_aware_design(gui_obj_t *parent);
void page_auto_play_pause_design(gui_obj_t *parent);
void page_audio_source_design(gui_obj_t *parent);
void page_auracast_design(gui_obj_t *parent);

void page_tx_management_design(gui_obj_t *parent);
void page_screen_brightness_design(gui_obj_t *parent);
void page_dark_light_design(gui_obj_t *parent);
void page_lock_screen_design(gui_obj_t *parent);
void page_auto_dim_off_screen_design(gui_obj_t *parent);
void page_quick_wake_up_screen_design(gui_obj_t *parent);
void page_case_button_customize_design(gui_obj_t *parent);
void page_information_center_customize_design(gui_obj_t *parent);
void page_unlock_slider_design(gui_obj_t *parent);
void page_notification_design(gui_obj_t *parent);
void page_clock_settings_design(gui_obj_t *parent);
void page_time_format_design(gui_obj_t *parent);
void page_language_design(gui_obj_t *parent);
void page_reorder_quick_access_design(gui_obj_t *parent);
void page_support_design(gui_obj_t *parent);

void page_timer_design(gui_obj_t *parent);
void page_flashlight_design(gui_obj_t *parent);
void page_silentnow_design(gui_obj_t *parent);
void page_volume_unit_meter_design(gui_obj_t *parent);
void page_qrcode_design(gui_obj_t *parent);

#ifdef __cplusplus
}
#endif
#endif


