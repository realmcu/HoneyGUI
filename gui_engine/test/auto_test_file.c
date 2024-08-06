/**
 * @file auto_test_file.c
 * @author luke_sun (luke_sun@realtek.com.cn)
 * @brief
 * @version 1.0
 * @date 2024-08-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "auto_test_file.h"

int compareFiles(const char *file1, const char *file2)
{
    FILE *fp1 = fopen(file1, "rb");
    FILE *fp2 = fopen(file2, "rb");

    if (fp1 == NULL || fp2 == NULL)
    {
        AUTO_TEST_LOG("Error opening file");
        if (fp1) { fclose(fp1); }
        if (fp2) { fclose(fp2); }
        return 0; // Consider files not equal if either can't be opened
    }

    int isEqual = 1; // Assume files are equal

    while (1)
    {
        int ch1 = fgetc(fp1);
        int ch2 = fgetc(fp2);

        if (ch1 != ch2)
        {
            isEqual = 0; // Files differ
            break;
        }

        if (ch1 == EOF || ch2 == EOF)
        {
            break;
        }
    }

    if (fgetc(fp1) != EOF || fgetc(fp2) != EOF)
    {
        isEqual = 0;    // Files have different lengths
    }

    fclose(fp1);
    fclose(fp2);

    return isEqual;
}

void compare_dc()
{
    char filename[256];
    snprintf(filename, sizeof(filename), "gui_engine/test/dc_test/%s.bin", test_case_name);

    char golden_filename[256];
    snprintf(golden_filename, sizeof(golden_filename), "gui_engine/test/dc_test/%s_golden.bin",
             test_case_name);

    if (true == compareFiles(filename, golden_filename))
    {
        AUTO_TEST_LOG("%s Compare Pass!", test_case_name);
    }
    else
    {
        AUTO_TEST_LOG("%s Compare Fail!", test_case_name);
    }
    gui_fb_change();
}
void gui_test_compare_dc(void)
{
    gui_server_exec_cb(compare_dc);
}
GUI_EXPORT_CMD(compare, gui_test_compare_dc);

static void save_buffer_to_file(const uint8_t *buffer, size_t size)
{
    char filename[256 + 4]; // 4 for ".BIN"
    snprintf(filename, sizeof(filename), "gui_engine/test/dc_test/%s.bin", test_case_name);

    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        AUTO_TEST_LOG("Failed to open file");
        return;
    }
    size_t written = fwrite(buffer, 1, size, file);
    if (written != size)
    {
        AUTO_TEST_LOG("%s Save Fail!", test_case_name);
    }
    else
    {
        AUTO_TEST_LOG("Save Pass! Data successfully written to gui_engine/test/dc_test/%s", test_case_name,
                      filename);
    }
    fclose(file);
}
void save_dc(void *msg)
{
    gui_dispdev_t *dc = gui_get_dc();
    save_buffer_to_file(dc->lcd_gram, dc->screen_height * dc->screen_width * dc->bit_depth / 8);
    gui_fb_change();
}
void gui_test_save_dc(void)
{
    gui_server_exec_cb(save_dc);
}
GUI_EXPORT_CMD(save, gui_test_save_dc);

