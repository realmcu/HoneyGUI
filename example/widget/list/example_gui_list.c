/* gui list widget example start*/

#include "guidef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_canvas_rect.h"
#include "gui_list.h"

static int app_init(void)
{
    gui_color_t color[] =
    {
        APP_COLOR_WHITE_OPACITY,
        APP_COLOR_GRAY,
        APP_COLOR_SILVER_OPACITY(150),
        APP_COLOR_WHITE,
        APP_COLOR_GRAY_OPACITY(150),
        APP_COLOR_RED,
        APP_COLOR_GREEN,
        APP_COLOR_BLUE,
    };

    uint8_t space = 5;

    /* VERTICAL CIRCLE STYLE */
    int length = 100;
    gui_list_widget_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space,
                                              VERTICAL);
    for (uint8_t i = 0; i < 8; i++)
    {
        gui_list_tab_t *tab = gui_list_add_tab(list);
        gui_canvas_rect_t *canvas = gui_canvas_rect_create(GUI_BASE(tab), "tab", 0, 0, 480, length,
                                                           color[i]);
    }
    gui_list_set_style(list, LIST_CIRCLE);


    /* VERTICAL CARD STYLE */
    // int length = 150;
    // gui_list_widget_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL);
    // for (uint8_t i = 0; i < 8; i++)
    // {
    //     gui_list_tab_t *tab = gui_list_add_tab(list);
    //     gui_canvas_rect_t *canvas = gui_canvas_rect_create(GUI_BASE(tab), "tab", 50, 0, 380, length, color[i]);
    // }
    // gui_list_set_style(list, LIST_CARD);


    /*HORIZONTAL ZOOM STYLE*/
    // int length = 100;
    // gui_list_widget_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, HORIZONTAL);
    // for (uint8_t i = 0; i < 8; i++)
    // {
    //     gui_list_tab_t *tab = gui_list_add_tab(list);
    //     gui_canvas_rect_t *canvas = gui_canvas_rect_create(GUI_BASE(tab), "tab", 0, 0, length, 480, color[i]);
    // }
    // gui_list_set_style(list, LIST_ZOOM);

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* gui list widget example end*/


