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
#include "gui_view.h"
#include "test_font.h"
/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
void *font32b1;
void *font32b2;
void *font32b4;
void *font32b8;
void *font32vb4;

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

    void *buffer = malloc(file_size);
    if (!buffer)
    {
        fclose(fp);
        return NULL;
    }

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
    font32b1 = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits1_font.bin", NULL);
    font32b2 = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits2_font.bin", NULL);
    font32b4 = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits4_font.bin", NULL);
    font32b8 = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits8_font.bin", NULL);
    font32vb4 = load_file_to_memory("./example/assets/font/HarmonyOS_size32_bits4_vfont.bin", NULL);
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
    gui_log("Event test obj name = %s, e = 0x%x !\n", this->name, e);
}


/**
 * @brief Timer callback
 * @param param Timer parameters
 */
static void test_timer_cb(void *param)
{
    gui_log("timer cb test!\n");
}

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
    // text_widget_example();
    text_font_test();
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

