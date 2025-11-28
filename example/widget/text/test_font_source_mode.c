/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file test_font_source_mode.c
  * @brief Test different font source types and data source modes
  * @details Tests combinations of GUI_FONT_SRC_BMP/IMG/MAT/TTF with FONT_SRC_MEMADDR/FTL
  * @author luke_sun
  * @date 2025/11/28
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "test_font.h"
#include "gui_api_dc.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/
static const char *TEST_TEXT = "HoneyGUI";
#define FONT_SIZE 32
#define LABEL_OFFSET_Y 10  // Offset for label below text

/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Create a label text widget for test identification
 * @param parent Parent widget
 * @param label_text Label text content
 * @param x X coordinate
 * @param y Y coordinate
 */
static void create_test_label(void *parent, const char *label_text, int16_t x, int16_t y)
{
    gui_text_t *label = gui_text_create(parent, "label", x, y, 200, 20);
    gui_text_set(label, (void *)label_text, GUI_FONT_SRC_BMP, APP_COLOR_YELLOW,
                 strlen(label_text), 16);
    gui_text_type_set(label, font32b2, FONT_SRC_MEMADDR);
}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Test different font source types and data source modes
 * @details Creates text widgets to test GUI_FONT_SRC_BMP/IMG/MAT/TTF with FONT_SRC_MEMADDR/FTL
 */
void text_font_source_mode_test(void)
{
    // Get screen device information for layout calculation
    gui_dispdev_t *dc = gui_get_dc();
    if (dc == NULL)
    {
        gui_log("Failed to get display device\n");
        return;
    }

    uint16_t screen_width = dc->screen_width;
    uint16_t screen_height = dc->screen_height;

    gui_log("Font source mode test started - Screen: %dx%d\n", screen_width, screen_height);

    // Calculate layout positions
    // Grid layout: rows = font types (BMP, IMG, MAT, TTF), columns = data source modes (MEMADDR, FTL)
    int16_t col_spacing = screen_width / 2;  // Two columns
    int16_t row_spacing = 100;  // Space for text + label with clear separation
    int16_t start_x = 10;
    int16_t start_y = 10;

    // Row 0: BMP font type tests
    int16_t row = 0;

    // Test 1: GUI_FONT_SRC_BMP + FONT_SRC_MEMADDR
    {
        int16_t x = start_x;
        int16_t y = start_y + row * row_spacing;

        gui_text_t *text = gui_text_create(gui_obj_get_root(), "bmp_memaddr", x, y, 0, 0);
        gui_text_set(text, (void *)TEST_TEXT, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(text, font32b2, FONT_SRC_MEMADDR);

        create_test_label(gui_obj_get_root(), "BMP+MEMADDR", x, y + FONT_SIZE + LABEL_OFFSET_Y);

        gui_log("Created BMP+MEMADDR test at (%d, %d)\n", x, y);
    }

    // Test 2: GUI_FONT_SRC_BMP + FONT_SRC_FTL
    {
        int16_t x = start_x + col_spacing;
        int16_t y = start_y + row * row_spacing;

        gui_text_t *text = gui_text_create(gui_obj_get_root(), "bmp_ftl", x, y, 0, 0);
        gui_text_set(text, (void *)TEST_TEXT, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(text, font32b2, FONT_SRC_FTL);

        create_test_label(gui_obj_get_root(), "BMP+FTL", x, y + FONT_SIZE + LABEL_OFFSET_Y);

        gui_log("Created BMP+FTL test at (%d, %d)\n", x, y);
    }

    // Row 1: IMG font type tests
    row = 1;

    // Test 3: GUI_FONT_SRC_IMG + FONT_SRC_MEMADDR
    {
        int16_t x = start_x;
        int16_t y = start_y + row * row_spacing;

        gui_text_t *text = gui_text_create(gui_obj_get_root(), "img_memaddr", x, y, 0, 0);
        gui_text_set(text, (void *)TEST_TEXT, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(text, font32b4, FONT_SRC_MEMADDR);
        gui_text_convert_to_img(text, RGB565);

        create_test_label(gui_obj_get_root(), "IMG+MEMADDR", x, y + FONT_SIZE + LABEL_OFFSET_Y);

        gui_log("Created IMG+MEMADDR test at (%d, %d)\n", x, y);
    }

    // Test 4: GUI_FONT_SRC_IMG + FONT_SRC_FTL
    {
        int16_t x = start_x + col_spacing;
        int16_t y = start_y + row * row_spacing;

        gui_text_t *text = gui_text_create(gui_obj_get_root(), "img_ftl", x, y, 0, 0);
        gui_text_set(text, (void *)TEST_TEXT, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(text, font32b4, FONT_SRC_FTL);
        gui_text_convert_to_img(text, RGB565);

        create_test_label(gui_obj_get_root(), "IMG+FTL", x, y + FONT_SIZE + LABEL_OFFSET_Y);

        gui_log("Created IMG+FTL test at (%d, %d)\n", x, y);
    }

    // Row 2: MAT font type tests
    row = 2;

    // Test 5: GUI_FONT_SRC_MAT + FONT_SRC_MEMADDR
    {
        int16_t x = start_x;
        int16_t y = start_y + row * row_spacing;

        gui_text_t *text = gui_text_create(gui_obj_get_root(), "mat_memaddr", x, y, 0, 0);
        gui_text_set(text, (void *)TEST_TEXT, GUI_FONT_SRC_MAT, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(text, font32b4, FONT_SRC_MEMADDR);

        create_test_label(gui_obj_get_root(), "MAT+MEMADDR", x, y + FONT_SIZE + LABEL_OFFSET_Y);

        gui_log("Created MAT+MEMADDR test at (%d, %d)\n", x, y);
    }

    // Test 6: GUI_FONT_SRC_MAT + FONT_SRC_FTL
    {
        int16_t x = start_x + col_spacing;
        int16_t y = start_y + row * row_spacing;

        gui_text_t *text = gui_text_create(gui_obj_get_root(), "mat_ftl", x, y, 0, 0);
        gui_text_set(text, (void *)TEST_TEXT, GUI_FONT_SRC_MAT, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(text, font32b4, FONT_SRC_FTL);

        create_test_label(gui_obj_get_root(), "MAT+FTL", x, y + FONT_SIZE + LABEL_OFFSET_Y);

        gui_log("Created MAT+FTL test at (%d, %d)\n", x, y);
    }

    // Row 3: TTF font type tests
    row = 3;

    // Test 7: GUI_FONT_SRC_TTF + FONT_SRC_MEMADDR
    {
        int16_t x = start_x;
        int16_t y = start_y + row * row_spacing;

        gui_text_t *text = gui_text_create(gui_obj_get_root(), "ttf_memaddr", x, y, 0, 0);
        gui_text_set(text, (void *)TEST_TEXT, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(text, font32vb4, FONT_SRC_MEMADDR);

        create_test_label(gui_obj_get_root(), "TTF+MEMADDR", x, y + FONT_SIZE + LABEL_OFFSET_Y);

        gui_log("Created TTF+MEMADDR test at (%d, %d)\n", x, y);
    }

    // Test 8: GUI_FONT_SRC_TTF + FONT_SRC_FTL
    {
        int16_t x = start_x + col_spacing;
        int16_t y = start_y + row * row_spacing;

        gui_text_t *text = gui_text_create(gui_obj_get_root(), "ttf_ftl", x, y, 0, 0);
        gui_text_set(text, (void *)TEST_TEXT, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(text, font32vb4, FONT_SRC_FTL);

        create_test_label(gui_obj_get_root(), "TTF+FTL", x, y + FONT_SIZE + LABEL_OFFSET_Y);

        gui_log("Created TTF+FTL test at (%d, %d)\n", x, y);
    }

    gui_log("Font source mode test completed - All 8 combinations created\n");
}
