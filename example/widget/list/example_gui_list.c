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
// #define RUN_VERTICAL_CIRCLE_STYLE
// #define RUN_VERTICAL_CARD_STYLE
// #define RUN_HORIZONTAL_ZOOM_STYLE
// #define RUN_VERTICAL_FAN_STYLE
#define RUN_VERTICAL_HELIX_STYLE
// #define RUN_VERTICAL_CURL_STYLE
// #define RUN_VERTICAL_CLASSIC_STYLE
// #define RUN_VERTICAL_FADE_STYLE

#define NOTE_NUM 20

int length = 100;

static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
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
    uint16_t index = ((gui_list_note_t *)obj)->index % (sizeof(color) / sizeof(gui_color_t));
#ifdef RUN_VERTICAL_CIRCLE_STYLE
    gui_canvas_rect_t *canvas = gui_canvas_rect_create(obj, "note", 0, 0, 480, 100, color[index]);
#endif

#ifdef RUN_HORIZONTAL_ZOOM_STYLE
    gui_canvas_rect_t *canvas = gui_canvas_rect_create(obj, "note", 0, 0, 100, 480, color[index]);
#endif

#ifdef RUN_VERTICAL_CARD_STYLE
    gui_canvas_rect_t *canvas = gui_canvas_rect_create(obj, "note", 50, 0, 380, 150, color[index]);
#endif

#ifdef RUN_VERTICAL_FAN_STYLE
    gui_color_t color2[] =
    {
        APP_COLOR_SILVER_OPACITY(150),
        APP_COLOR_WHITE_OPACITY,
    };
    gui_canvas_rect_t *canvas = gui_canvas_rect_create(obj, "note", 50, 0, 380, 100, color2[index % 2]);
#endif

#ifdef RUN_VERTICAL_HELIX_STYLE
    gui_color_t color2[] =
    {
        APP_COLOR_SILVER_OPACITY(150),
        APP_COLOR_WHITE_OPACITY,
    };
    gui_canvas_rect_t *canvas = gui_canvas_rect_create(obj, "note", 50, 0, 380, 100, color2[index % 2]);
#endif

#ifdef RUN_VERTICAL_CURL_STYLE
    gui_color_t color2[] =
    {
        APP_COLOR_SILVER_OPACITY(150),
        APP_COLOR_WHITE_OPACITY,
    };
    gui_canvas_rect_t *canvas = gui_canvas_rect_create(obj, "note", 0, 0, 0, 100, color2[index % 2]);
#endif

#ifdef RUN_VERTICAL_CLASSIC_STYLE
    gui_canvas_rect_t *canvas = gui_canvas_rect_create(obj, "note", 0, 0, 480, 480, color[index]);
#endif

#ifdef RUN_VERTICAL_FADE_STYLE
    gui_canvas_rect_t *canvas = gui_canvas_rect_create(obj, "note", 0, 0, 480, 480, color[index]);
#endif
    GUI_UNUSED(canvas);
}

static int app_init(void)
{
    uint16_t space = 0;

    /* VERTICAL CIRCLE STYLE */
#ifdef RUN_VERTICAL_CIRCLE_STYLE

    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       note_design, NULL, false);
    gui_list_set_note_num(list, NOTE_NUM);
    gui_list_set_style(list, LIST_CIRCLE);
    gui_list_set_out_scope(list, 100);
    return 0;
#endif


    /* VERTICAL CARD STYLE */
#ifdef RUN_VERTICAL_CARD_STYLE
    int length = 150;
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       note_design, NULL,
                                       false);
    gui_list_set_style(list, LIST_CARD); //if LIST_CARD style, must set style before set note num
    gui_list_set_note_num(list, NOTE_NUM);
    gui_list_set_card_stack_location(list, 20);
    return 0;
#endif

    /* HORIZONTAL ZOOM STYLE */
#ifdef RUN_HORIZONTAL_ZOOM_STYLE

    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space,
                                       HORIZONTAL, note_design, NULL, false);
    gui_list_set_style(list, LIST_ZOOM);
    gui_list_set_note_num(list, NOTE_NUM);
    return 0;
#endif

    /* VERTICAL FAN STYLE */
#ifdef RUN_VERTICAL_FAN_STYLE

    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       note_design, NULL, false);
    gui_list_set_note_num(list, NOTE_NUM);
    gui_list_set_style(list, LIST_FAN);
    return 0;
#endif

    /* VERTICAL HELIX STYLE */
#ifdef RUN_VERTICAL_HELIX_STYLE
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       note_design, NULL, false);
    gui_list_set_note_num(list, NOTE_NUM);
    gui_list_set_style(list, LIST_HELIX);
    gui_list_set_auto_align(list, true);
    gui_list_enable_loop(list, true);
    return 0;
#endif

    /* VERTICAL CURL STYLE */
#ifdef RUN_VERTICAL_CURL_STYLE
    space = 0;

    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       note_design, NULL, false);
    gui_list_set_note_num(list, NOTE_NUM);
    gui_list_set_style(list, LIST_CURL);
    return 0;
#endif

    /* VERTICAL CLASSIC STYLE */
#ifdef RUN_VERTICAL_CLASSIC_STYLE
    int length = 480;
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       note_design, NULL,
                                       false);
    gui_list_set_style(list, LIST_CLASSIC); //if LIST_CARD style, must set style before set note num
    gui_list_set_note_num(list, 10);
    gui_list_set_auto_align(list, true);
    gui_list_set_offset(list, -length * 5);
    gui_list_set_inertia(list, false);
    return 0;
#endif

    /* VERTICAL FADE STYLE */
#ifdef RUN_VERTICAL_FADE_STYLE
    int length = 480;
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       note_design, NULL,
                                       false);
    gui_list_set_style(list, LIST_FADE);
    gui_list_set_note_num(list, 10);
    gui_list_set_auto_align(list, true);
    gui_list_set_inertia(list, false);
    gui_list_set_out_scope(list, 480 / 2);
    gui_list_set_offset(list, -length * 5);
    return 0;
#endif
}

GUI_INIT_APP_EXPORT(app_init);
/* gui list widget example end*/


