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
#include "romfs/hg_romfs.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define FONT_MEM_POOL_SIZE (1024 * 1024 * 100)

/* Path to the packed emoji ROM filesystem (built by mkromfs_emoji.bat). */
#define EMOJI_PACK_PATH    "./example/assets/font/emoji/emoji_pack.bin"

/* mkromfs on-disk directory entry: four little-endian uint32 fields
 * (type, name offset, data offset, size), 16 bytes total. */
#define EMOJI_ROMFS_ENTRY_SIZE 16

#if ENABLE_FONT_V3_TYPO
#define FONT_DIR           "./example/assets/font/v3/"
#else
#define FONT_DIR           "./example/assets/font/"
#endif
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

/*
 * Rebuild a mkromfs image (example/assets/font/emoji/emoji_pack.bin, built with
 * "-b -a 0x0") into native struct romfs_dirent nodes.
 *
 * mkromfs packs every directory entry as four little-endian uint32 fields
 * (type / name offset / data offset / size), 16 bytes, with all offsets
 * relative to the image start. On a 32-bit target that layout is byte-identical
 * to struct romfs_dirent, so firmware just points hg_romfs_mount() at the flash
 * image. This simulator is built 64-bit, where the two pointers inside struct
 * romfs_dirent widen it to 32 bytes -- the raw image can no longer be walked in
 * place. So parse it once: name and file payload point back into the resident
 * image buffer, while a directory's data points at a freshly built child array.
 */
static uint32_t emoji_romfs_u32(const uint8_t *p)
{
    return (uint32_t)p[0]         | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static struct romfs_dirent *emoji_romfs_build_children(const uint8_t *base,
                                                       uint32_t table_off,
                                                       uint32_t count)
{
    struct romfs_dirent *arr = gui_malloc(count * sizeof(struct romfs_dirent));
    if (arr == NULL)
    {
        return NULL;
    }

    for (uint32_t i = 0; i < count; i++)
    {
        const uint8_t *raw = base + table_off + i * EMOJI_ROMFS_ENTRY_SIZE;
        uint32_t type     = emoji_romfs_u32(raw);
        uint32_t name_off = emoji_romfs_u32(raw + 4);
        uint32_t data_off = emoji_romfs_u32(raw + 8);
        uint32_t size     = emoji_romfs_u32(raw + 12);

        arr[i].type = type;
        arr[i].name = (const char *)(base + name_off);
        arr[i].size = size;
        arr[i].data = (type == ROMFS_DIRENT_DIR)
                      ? (const uint8_t *)emoji_romfs_build_children(base, data_off, size)
                      : (base + data_off);
    }
    return arr;
}

/* Parse an in-RAM mkromfs image and return a root dirent ready for mounting. */
static struct romfs_dirent *emoji_romfs_from_image(const uint8_t *base)
{
    struct romfs_dirent *root = gui_malloc(sizeof(struct romfs_dirent));
    if (root == NULL)
    {
        return NULL;
    }

    /* The root entry occupies offset 0: {type, name off, data off, size}. */
    root->type = emoji_romfs_u32(base);
    root->name = (const char *)(base + emoji_romfs_u32(base + 4));
    root->size = emoji_romfs_u32(base + 12);
    root->data = (const uint8_t *)emoji_romfs_build_children(base,
                                                             emoji_romfs_u32(base + 8),
                                                             root->size);
    return root;
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

    /* Load the packed emoji ROM filesystem into RAM and mount it under /emoji.
     * The pack (built by mkromfs_emoji.bat) indexes every emoji bitmap by
     * filename, mirroring how the emoji set is burned to flash on target. The
     * text engine's emoji lookup then resolves "/emoji/emoji_u<hex>.bin"
     * through the ROMFS backend with no per-frame file IO. On 32-bit targets
     * the flash image is mounted directly; here (64-bit sim) it is parsed into
     * native dirent nodes first -- see emoji_romfs_from_image(). */
    void *emoji_pack = load_file_to_memory(EMOJI_PACK_PATH, NULL);
    if (emoji_pack != NULL)
    {
        struct romfs_dirent *emoji_root = emoji_romfs_from_image(emoji_pack);
        if (emoji_root != NULL)
        {
            gui_vfs_mount_romfs("/emoji", emoji_root, 0);
        }
    }

#define LOAD_FONT(file) load_file_to_memory(FONT_PATH(file), NULL)

    /* HarmonyOS Sans SC - bitmap (1/2/4/8-bit) + vector */
    font32b1       = LOAD_FONT("HarmonyOS_Sans_SC_Regular_size32_bits1_bitmap.bin");
    font32b2       = LOAD_FONT("HarmonyOS_Sans_SC_Regular_size32_bits2_bitmap.bin");
    font32b4       = LOAD_FONT("HarmonyOS_Sans_SC_Regular_size32_bits4_bitmap.bin");
    font32b8       = LOAD_FONT("HarmonyOS_Sans_SC_Regular_size32_bits8_bitmap.bin");
    font32vb4      = LOAD_FONT("HarmonyOS_Sans_SC_Regular_vector.bin");
    fontharmonysc  = font32b2;

    /* NotoSans - bitmap + vector (Latin/basic) */
    fontnoto       = LOAD_FONT("NotoSans_Regular_size32_bits4_bitmap.bin");
    fontnotovec    = LOAD_FONT("NotoSans_Regular_vector.bin");

    /* NotoSans - multi-language bitmap */
    fontnotoarabic = LOAD_FONT("NotoSansArabic_Regular_size32_bits2_bitmap.bin");
    fontnotojp     = LOAD_FONT("NotoSansJP_Regular_size32_bits2_bitmap.bin");
    fontnotokr     = LOAD_FONT("NotoSansKR_Regular_size32_bits2_bitmap.bin");
    fontnotothai   = LOAD_FONT("NotoSansThai_Regular_size32_bits2_bitmap.bin");
    fontnotohebrew = LOAD_FONT("NotoSansHebrew_Regular_size32_bits2_bitmap.bin");

    /* NotoSans - multi-language vector */
    fontnotoscvec  = LOAD_FONT("NotoSansSC_Regular_vector.bin");
    fontnotojpvec  = LOAD_FONT("NotoSansJP_Regular_vector.bin");
    fontnotokrvec  = LOAD_FONT("NotoSansKR_Regular_vector.bin");

#undef LOAD_FONT

    gui_font_mem_init(font32b2);
}
