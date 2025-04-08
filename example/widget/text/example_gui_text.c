#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "guidef.h"
#include "gui_obj.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_text.h"

#include "../../assets/font/HarmonyOS_size32_bits2_font.c"

/* gui text widget example start*/
void test_event_cb(void *obj, gui_event_t e, void *param)
{
    gui_obj_t *this = (gui_obj_t *)obj;
    gui_log("Event test obj name = %s, e = 0x%x !\n", this->name, e);
}

void test_timer_cb(void *param)
{
    gui_log("timer cb test!\n");
}


static int app_init(void)
{
    gui_text_t *text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(text, "HoneyGUI", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 8, 32);
    gui_text_type_set(text, _acHarmonyOS_size32_bits2_font, FONT_SRC_MEMADDR);

    gui_obj_add_event_cb(text, (gui_event_cb_t)test_event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_obj_create_timer(&(text->base), 1000, true, test_timer_cb);
    gui_obj_start_timer(&(text->base));

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* gui text widget example end*/


