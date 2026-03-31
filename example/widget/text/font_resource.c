/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "guidef.h"
#include "gui_api_os.h"

#include "font_mem.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define FONT_MEM_POOL_SIZE (1024 * 1024 * 100)
#define FONT_DIR           "./example/assets/font/"
#define FONT_PATH(name)    FONT_DIR name

/*============================================================================*
 *                            Variables
 *============================================================================*/
static uint8_t s_font_mem_pool[FONT_MEM_POOL_SIZE];
static size_t s_font_mem_offset = 0;

void *font32b1;
void *font32b2;
void *font32b4;
void *font32b8;
void *font32vb4;

void *font32b4index1;
void *font32vb4index1;

void *fontnoto;
void *fontnotoarabic;
void *fontnotojp;
void *fontnotokr;
void *fontnotothai;
void *fontnotohebrew;

void *fontnotovec;
void *fontnotoscvec;
void *fontnotojpvec;
void *fontnotokrvec;

void *fontharmonysc;

void *fontnotov2;


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
        // Note: buffer is from static memory pool, no need to free
        // Rollback the offset instead
        s_font_mem_offset -= file_size;
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

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Initialize font files
 */
void font_file_init(void)
{
    s_font_mem_offset = 0;

#define LOAD_FONT(file) load_file_to_memory(FONT_PATH(file), NULL)

    font32b1       = LOAD_FONT("HarmonyOS_size32_bits1_font.bin");
    font32b2       = LOAD_FONT("HarmonyOS_size32_bits2_font.bin");
    font32b4       = LOAD_FONT("HarmonyOS_size32_bits4_font.bin");
    font32b8       = LOAD_FONT("HarmonyOS_size32_bits8_font.bin");
    font32vb4      = LOAD_FONT("HarmonyOS_size32_bits4_vfont.bin");

    font32b4index1  = LOAD_FONT("NotoSans_Regular_size32_bits4_font.bin");
    font32vb4index1 = LOAD_FONT("NotoSans_Regular_size32_bits4_vfont.bin");

    fontnoto       = LOAD_FONT("NotoSans_Regular_size32_bits4_bitmap.bin");
    fontnotoarabic = LOAD_FONT("NotoSansArabic_size32_bits2_font.bin");
    fontnotojp     = LOAD_FONT("NotoSansJP_regular_size32_bits2_bitmap.bin");
    fontnotokr     = LOAD_FONT("NotoSansKR_regular_size32_bits2_bitmap.bin");
    fontnotothai   = LOAD_FONT("NotoSansThai_size32_bits2_font.bin");
    fontnotohebrew = LOAD_FONT("NotoSansHebrew_size32_bits2_font.bin");

    fontnotovec    = LOAD_FONT("NotoSans_Regular_vector.bin");
    fontnotoscvec  = LOAD_FONT("NotoSansSC_Regular_vector.bin");
    fontnotojpvec  = LOAD_FONT("NotoSansJP_Regular_vector.bin");
    fontnotokrvec  = LOAD_FONT("NotoSansKR_Regular_vector.bin");

    fontharmonysc  = LOAD_FONT("HarmonyOS_size32_bits2_font.bin");
    fontnotov2     = LOAD_FONT("NotoSans_Regular_size32_bits4_v3_font.bin");

#undef LOAD_FONT

    gui_font_mem_init(font32b2);
}
