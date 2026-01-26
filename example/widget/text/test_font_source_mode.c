/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "test_font.h"
#include "gui_api_dc.h"
#include "font_mem.h"

/*============================================================================*
 *                           Constants
 *============================================================================*/
static const char *TEST_TEXT = "HoneyGUI";
#define FONT_SIZE 32
#define LABEL_SIZE 16
#define LABEL_OFFSET_Y 5

/*============================================================================*
 *                            Variables
 *============================================================================*/
static uint8_t *fs_font_path = (uint8_t *)"/pc/example/assets/font/HarmonyOS_size32_bits8_font.bin";
static uint8_t *fs_ttf_path = (uint8_t *)"/pc/example/assets/font/HarmonyOS_size32_bits4_vfont.bin";

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void create_label(void *parent, const char *text, int16_t x, int16_t y)
{
    gui_text_t *label = gui_text_create(parent, "label", x, y, 150, 20);
    gui_text_set(label, (void *)text, GUI_FONT_SRC_TTF, APP_COLOR_YELLOW,
                 strlen(text), LABEL_SIZE);
    gui_text_type_set(label, font32vb4, FONT_SRC_MEMADDR);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Test font source modes: MEMADDR / FTL / FILESYS
 * @details Layout: 3 columns (MEMADDR, FTL, FILESYS), 4 rows (BMP, IMG, MAT, TTF)
 */
void text_font_source_mode_test(void)
{
    gui_dispdev_t *dc = gui_get_dc();
    if (dc == NULL)
    {
        gui_log("Failed to get display device\n");
        return;
    }

    uint16_t screen_width = dc->screen_width;
    gui_log("Font source mode test - Screen: %dx%d\n", screen_width, dc->screen_height);

    /* Initialize filesystem font */
    uint8_t fs_font_idx = gui_font_mem_init_fs(fs_font_path);
    if (fs_font_idx == UINT8_MAX)
    {
        gui_log("Failed to init filesystem font\n");
    }

    /* Layout: 3 columns, 4 rows */
    int16_t col_width = screen_width / 3;
    int16_t row_height = 80;
    int16_t start_x = 5;
    int16_t start_y = 10;

    void *root = gui_obj_get_root();

    /* Row 0: BMP */
    {
        int16_t y = start_y;
        /* MEMADDR */
        gui_text_t *t1 = gui_text_create(root, "bmp_mem", start_x, y, 0, 0);
        gui_text_set(t1, (void *)TEST_TEXT, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(t1, font32b8, FONT_SRC_MEMADDR);
        create_label(root, "BMP+MEM", start_x, y + FONT_SIZE + LABEL_OFFSET_Y);

        /* FTL */
        gui_text_t *t2 = gui_text_create(root, "bmp_ftl", start_x + col_width, y, 0, 0);
        gui_text_set(t2, (void *)TEST_TEXT, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(t2, font32b8, FONT_SRC_FTL);
        create_label(root, "BMP+FTL", start_x + col_width, y + FONT_SIZE + LABEL_OFFSET_Y);

        /* FILESYS */
        if (fs_font_idx != UINT8_MAX)
        {
            gui_text_t *t3 = gui_text_create(root, "bmp_fs", start_x + col_width * 2, y, 0, 0);
            gui_text_set(t3, (void *)TEST_TEXT, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                         strlen(TEST_TEXT), FONT_SIZE);
            gui_text_type_set(t3, fs_font_path, FONT_SRC_FILESYS);
            create_label(root, "BMP+FS", start_x + col_width * 2, y + FONT_SIZE + LABEL_OFFSET_Y);
        }
    }

    /* Row 1: IMG */
    {
        int16_t y = start_y + row_height;
        /* MEMADDR */
        gui_text_t *t1 = gui_text_create(root, "img_mem", start_x, y, 0, 0);
        gui_text_set(t1, (void *)TEST_TEXT, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(t1, font32b8, FONT_SRC_MEMADDR);
        gui_text_convert_to_img(t1, RGB565);
        create_label(root, "IMG+MEM", start_x, y + FONT_SIZE + LABEL_OFFSET_Y);

        /* FTL */
        gui_text_t *t2 = gui_text_create(root, "img_ftl", start_x + col_width, y, 0, 0);
        gui_text_set(t2, (void *)TEST_TEXT, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(t2, font32b8, FONT_SRC_FTL);
        gui_text_convert_to_img(t2, RGB565);
        create_label(root, "IMG+FTL", start_x + col_width, y + FONT_SIZE + LABEL_OFFSET_Y);

        /* FILESYS */
        if (fs_font_idx != UINT8_MAX)
        {
            gui_text_t *t3 = gui_text_create(root, "img_fs", start_x + col_width * 2, y, 0, 0);
            gui_text_set(t3, (void *)TEST_TEXT, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                         strlen(TEST_TEXT), FONT_SIZE);
            gui_text_type_set(t3, fs_font_path, FONT_SRC_FILESYS);
            gui_text_convert_to_img(t3, RGB565);
            create_label(root, "IMG+FS", start_x + col_width * 2, y + FONT_SIZE + LABEL_OFFSET_Y);
        }
    }

    /* Row 2: MAT */
    {
        int16_t y = start_y + row_height * 2;
        /* MEMADDR */
        gui_text_t *t1 = gui_text_create(root, "mat_mem", start_x, y, 0, 0);
        gui_text_set(t1, (void *)TEST_TEXT, GUI_FONT_SRC_MAT, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(t1, font32b8, FONT_SRC_MEMADDR);
        create_label(root, "MAT+MEM", start_x, y + FONT_SIZE + LABEL_OFFSET_Y);

        /* FTL */
        gui_text_t *t2 = gui_text_create(root, "mat_ftl", start_x + col_width, y, 0, 0);
        gui_text_set(t2, (void *)TEST_TEXT, GUI_FONT_SRC_MAT, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(t2, font32b8, FONT_SRC_FTL);
        create_label(root, "MAT+FTL", start_x + col_width, y + FONT_SIZE + LABEL_OFFSET_Y);

        /* FILESYS */
        if (fs_font_idx != UINT8_MAX)
        {
            gui_text_t *t3 = gui_text_create(root, "mat_fs", start_x + col_width * 2, y, 0, 0);
            gui_text_set(t3, (void *)TEST_TEXT, GUI_FONT_SRC_MAT, APP_COLOR_WHITE,
                         strlen(TEST_TEXT), FONT_SIZE);
            gui_text_type_set(t3, fs_font_path, FONT_SRC_FILESYS);
            create_label(root, "MAT+FS", start_x + col_width * 2, y + FONT_SIZE + LABEL_OFFSET_Y);
        }
    }

    /* Row 3: TTF */
    {
        int16_t y = start_y + row_height * 3;
        /* MEMADDR */
        gui_text_t *t1 = gui_text_create(root, "ttf_mem", start_x, y, 0, 0);
        gui_text_set(t1, (void *)TEST_TEXT, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(t1, font32vb4, FONT_SRC_MEMADDR);
        create_label(root, "TTF+MEM", start_x, y + FONT_SIZE + LABEL_OFFSET_Y);

        /* FTL */
        gui_text_t *t2 = gui_text_create(root, "ttf_ftl", start_x + col_width, y, 0, 0);
        gui_text_set(t2, (void *)TEST_TEXT, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(t2, font32vb4, FONT_SRC_FTL);
        create_label(root, "TTF+FTL", start_x + col_width, y + FONT_SIZE + LABEL_OFFSET_Y);

        /* FILESYS */
        gui_text_t *t3 = gui_text_create(root, "ttf_fs", start_x + col_width * 2, y, 0, 0);
        gui_text_set(t3, (void *)TEST_TEXT, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(TEST_TEXT), FONT_SIZE);
        gui_text_type_set(t3, fs_ttf_path, FONT_SRC_FILESYS);
        create_label(root, "TTF+FS", start_x + col_width * 2, y + FONT_SIZE + LABEL_OFFSET_Y);
    }

    gui_log("Font source mode test completed\n");
}
