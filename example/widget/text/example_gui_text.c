/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file example_gui_text.c
  * @brief Text widget example
  * @details Demonstrate text widget with font loading and event handling
  * @author luke_sun@realsil.com.cn
  * @date 2025/04/16
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "guidef.h"
#include "gui_obj.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_view.h"
#include "test_font.h"

#include "font_mem.h"
#include "font_custom.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define FONT_MEM_POOL_SIZE (1024 * 1024 * 100)
uint8_t s_font_mem_pool[FONT_MEM_POOL_SIZE];
static size_t s_font_mem_offset = 0;

/*============================================================================*
 *                            Variables
 *============================================================================*/
void *font32b1;
void *font32b2;
void *font32b4;
void *font32b8;
void *font32vb4;

void *fontnoto;
void *fontnotoarabic;
void *fontnotojp;
void *fontnotokr;
void *fontnotothai;
void *fontnotohebrew;
void *fontharmonysc;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Load file into memory
 * @param path File path
 * @param out_size [out] Optional output parameter for file size
 * @return Pointer to allocated memory, NULL on failure
 */
static void *load_file_to_memory(const char *path, size_t *out_size)
{
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        gui_log("Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    if (file_size <= 0)
    {
        fclose(fp);
        return NULL;
    }

    if (s_font_mem_offset + file_size > FONT_MEM_POOL_SIZE)
    {
        gui_log("Font memory pool overflow!\n");
        fclose(fp);
        return NULL;
    }

    void *buffer = &s_font_mem_pool[s_font_mem_offset];
    s_font_mem_offset += file_size;

    if (fread(buffer, 1, file_size, fp) != (size_t)file_size)
    {
        free(buffer);
        fclose(fp);
        return NULL;
    }

    fclose(fp);

    if (out_size)
    {
        *out_size = (size_t)file_size;
    }

    return buffer;
}


/**
 * @brief Initialize font files
 */
static void font_file_init(void)
{
    s_font_mem_offset = 0;

    font32b1 = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits1_font.bin", NULL);
    font32b2 = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits2_font.bin", NULL);
    font32b4 = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits4_font.bin", NULL);
    font32b8 = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits8_font.bin", NULL);
    font32vb4 = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits4_vfont.bin", NULL);

    fontnoto = load_file_to_memory("./example/assets/font/NotoSans_size32_bits2_font.bin", NULL);
    fontnotoarabic = load_file_to_memory("./example/assets/font/NotoSansArabic_size32_bits2_font.bin",
                                         NULL);
    fontnotojp = load_file_to_memory("./example/assets/font/NotoSansJP_size32_bits2_font.bin", NULL);
    fontnotokr = load_file_to_memory("./example/assets/font/NotoSansKR_size32_bits2_font.bin", NULL);
    fontnotothai = load_file_to_memory("./example/assets/font/NotoSansThai_size32_bits2_font.bin",
                                       NULL);
    fontnotohebrew = load_file_to_memory("./example/assets/font/NotoSansHebrew_size32_bits2_font.bin",
                                         NULL);
    fontharmonysc = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits2_font.bin", NULL);

    gui_font_mem_init(font32b2);
}


/**
 * @brief Test event callback
 * @param obj Event source object
 * @param e Event type
 * @param param Event parameters
 */
static void test_event_cb(void *obj, gui_event_t e, void *param)
{
    gui_obj_t *this = (gui_obj_t *)obj;
    GUI_UNUSED(param);
    gui_log("Event test obj name = %s, e = 0x%x !\n", this->name, e);
}


/**
 * @brief Timer callback
 * @param param Timer parameters
 */
static void test_timer_cb(void *param)
{
    gui_log("timer cb test!\n");
    GUI_UNUSED(param);
}

/**
 * @brief Create and configure a horizontal and a vertical scroll text widget example
 */
/* gui scroll text widget example start*/
static void scroll_text_widget_example(void)
{
    char *test_text =
        "Everyone has the right to freedom of thought, conscience and religion; this right includes freedom to change his religion or belief, and freedom, either alone or in community with others and in public or private, to manifest his religion or belief in teaching, practice, worship and observance. ";

    gui_scroll_text_t *scroll_text_x = gui_scroll_text_create(gui_obj_get_root(), "scroll_text", 0, 200,
                                                              300, 200);
    gui_scroll_text_set(scroll_text_x, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text),
                        32);
    gui_scroll_text_scroll_set(scroll_text_x, SCROLL_X, 300, 100, 40000, 0);
    gui_scroll_text_type_set(scroll_text_x, font32b2, FONT_SRC_MEMADDR);

    gui_scroll_text_t *scroll_text_y = gui_scroll_text_create(gui_obj_get_root(), "scroll_text", 50,
                                                              250, 300, 100);
    gui_scroll_text_set(scroll_text_y, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text),
                        32);
    gui_scroll_text_scroll_set(scroll_text_y, SCROLL_Y, 100, 100, 10000, 0);
    gui_scroll_text_type_set(scroll_text_y, font32b2, FONT_SRC_MEMADDR);
    gui_text_wordwrap_set(&scroll_text_y->base, true);
}
/* gui scroll text widget example end*/

/* gui custom font rendiring demo start*/
static void font_load(gui_text_t *text, gui_text_rect_t *rect)
{
    GUI_UNUSED(text);
    GUI_UNUSED(rect);

    gui_log("font_load\n");
    gui_font_mem_load(text, rect);
}

static void font_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    GUI_UNUSED(text);
    GUI_UNUSED(rect);

    gui_log("font_draw\n");
    gui_font_mem_draw(text, rect);
}

static void font_unload(gui_text_t *text)
{
    GUI_UNUSED(text);

    gui_log("font_unload\n");
    gui_font_mem_unload(text);
}

static void font_destroy(gui_text_t *text)
{
    GUI_UNUSED(text);

    gui_log("font_destroy\n");
    gui_font_mem_destroy(text);
}

static void free_widget_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_tree_free((gui_obj_t *)param);
}

static void custom_font_rendering_demo(void)
{
    gui_log("custom_font_rendering_demo\n");
    gui_font_custom_set_callbacks(font_load, font_draw, font_unload, font_destroy);

    gui_text_t *text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(text, "HoneyGUI", GUI_FONT_SRC_CUS, APP_COLOR_WHITE, 8, 32);
    gui_text_type_set(text, font32b1, FONT_SRC_MEMADDR);

    gui_text_t *free_text = gui_text_create(gui_obj_get_root(), "free_text", 0, 430, 0, 48);
    gui_text_set(free_text, "DESTROY", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, 7, 48);
    gui_text_type_set(free_text, font32vb4, FONT_SRC_MEMADDR);
    gui_text_mode_set(free_text, RIGHT);
    gui_obj_add_event_cb(free_text, free_widget_cb, GUI_EVENT_TOUCH_CLICKED, text);
}
/* gui custom font rendiring demo end*/

/**
 * @brief Create and configure a text widget example
 */
/* gui text widget example start*/
static void text_widget_example(void)
{
    gui_text_t *text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(text, "HoneyGUI", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 8, 32);
    gui_text_type_set(text, font32b1, FONT_SRC_MEMADDR);

    gui_obj_add_event_cb(text, (gui_event_cb_t)test_event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_obj_create_timer(&(text->base), 1000, true, test_timer_cb);
    gui_obj_start_timer(&(text->base));
}
/* gui text widget example end*/

static int app_init(void)
{
    font_file_init();

    /* gui text widget example */
    text_widget_example();

    /* gui scroll text widget example */
    // scroll_text_widget_example();

    /* gui custom font rendiring demo */
    // custom_font_rendering_demo();

    /* gui text font rendering test */
    // text_font_test();

    /* gui text multi language test */
    // text_multi_language_test();

    /* gui text font scroll function test */
    // text_font_scroll_function_test();
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

/*============================================================================*
 *                           Public Functions
 *============================================================================*/