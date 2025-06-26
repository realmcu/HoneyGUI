/* gui list widget example start*/

#include "guidef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_canvas_rect.h"
#include "gui_list.h"

/* Use the following macro to checkout the different style lists */
#define RUN_VERTICAL_CIRCLE_STYLE
// #define RUN_VERTICAL_CARD_STYLE
// #define RUN_HORIZONTAL_ZOOM_STYLE
// #define RUN_VERTICAL_FAN_STYLE
// #define RUN_VERTICAL_HELIX_STYLE
// #define RUN_VERTICAL_CURL_STYLE

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
#ifdef RUN_VERTICAL_CIRCLE_STYLE
    int length = 100;
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space,
                                       VERTICAL, false);
    for (uint8_t i = 0; i < 8; i++)
    {
        gui_list_note_t *tab = gui_list_add_note(list, false);
        gui_canvas_rect_t *canvas = gui_canvas_rect_create(GUI_BASE(tab), "tab", 0, 0, 480, length,
                                                           color[i]);
    }
    gui_list_set_style(list, LIST_CIRCLE);
    return 0;
#endif


    /* VERTICAL CARD STYLE */
#ifdef RUN_VERTICAL_CARD_STYLE
    int length = 150;
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       false);
    for (uint8_t i = 0; i < 8; i++)
    {
        gui_list_note_t *tab = gui_list_add_note(list, false);
        gui_canvas_rect_t *canvas = gui_canvas_rect_create(GUI_BASE(tab), "tab", 50, 0, 380, length,
                                                           color[i]);
    }
    gui_list_set_style(list, LIST_CARD);
    return 0;
#endif

    /* HORIZONTAL ZOOM STYLE */
#ifdef RUN_HORIZONTAL_ZOOM_STYLE
    int length = 100;
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space,
                                       HORIZONTAL, false);
    for (uint8_t i = 0; i < 8; i++)
    {
        gui_list_note_t *tab = gui_list_add_note(list, false);
        gui_canvas_rect_t *canvas = gui_canvas_rect_create(GUI_BASE(tab), "tab", 0, 0, length, 480,
                                                           color[i]);
    }
    gui_list_set_style(list, LIST_ZOOM);
    return 0;
#endif

    /* VERTICAL FAN STYLE */
#ifdef RUN_VERTICAL_FAN_STYLE
    int length = 100;
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       false);
    for (uint8_t i = 0; i < 20; i++)
    {
        gui_list_note_t *tab = gui_list_add_note(list, false);
        gui_canvas_rect_t *canvas = gui_canvas_rect_create(GUI_BASE(tab), "tab", 50, 0, 380, length,
                                                           APP_COLOR_SILVER_OPACITY(150));
    }
    gui_list_set_style(list, LIST_FAN);
    return 0;
#endif

    /* VERTICAL HELIX STYLE */
#ifdef RUN_VERTICAL_HELIX_STYLE
    int length = 100;
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       true);
    for (uint8_t i = 0; i < 20; i++)
    {
        gui_list_note_t *tab = gui_list_add_note(list, false);
        gui_canvas_rect_t *canvas = gui_canvas_rect_create(GUI_BASE(tab), "tab", 50, 0, 380, length,
                                                           APP_COLOR_SILVER_OPACITY(150));
    }
    gui_list_set_style(list, LIST_HELIX);
    return 0;
#endif

    /* VERTICAL CURL STYLE */
#ifdef RUN_VERTICAL_CURL_STYLE
    int length = 100;
    space = 0;
    gui_color_t color_t[] =
    {
        APP_COLOR_SILVER_OPACITY(150),
        APP_COLOR_WHITE_OPACITY,
    };
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       false);
    for (uint8_t i = 0; i < 20; i++)
    {
        gui_list_note_t *tab = gui_list_add_note(list, false);
        gui_canvas_rect_t *canvas = gui_canvas_rect_create(GUI_BASE(tab), "tab", 0, 0, 0, length,
                                                           color_t[i % 2]);
    }
    gui_list_set_style(list, LIST_CURL);
    return 0;
#endif
}

GUI_INIT_APP_EXPORT(app_init);
/* gui list widget example end*/


