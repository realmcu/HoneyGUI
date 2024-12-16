#include "lvgl.h"

#if LV_USE_RTK_IDU

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef LV_USE_GPU_RTK_PPE
#include "hal_idu.h"
#endif

/**********************
 * GLOBAL VARIABLES
 **********************/

static uint8_t *decompress_flag = NULL;
static uint8_t *psram_img_cache = NULL;  // PSRAM image decode data
static uint32_t psram_img_cache_size = 0;  // cache size
static bool psram_img_cache_valid = false;  // cache flag

/**********************
 * TYPEDEFS
 **********************/
typedef struct
{
    lv_fs_file_t f;
    const uint8_t *data;
    uint32_t width;
    uint32_t height;
    uint8_t bytes_per_pixel;
} rle_dsc_t;

#pragma pack(1)
typedef struct
{
    uint8_t len;
    uint16_t pixel16;  //rgb565
} idu_rgb565_node_t;
typedef struct idu_argb8565_node
{
    uint8_t len;
    uint16_t pixel;  //argb8565
    uint8_t alpha;
} idu_argb8565_node_t;
typedef struct idu_rgb888_node
{
    uint8_t len;
    uint8_t pixel_b;  //rgb888
    uint8_t pixel_g;
    uint8_t pixel_r;
} idu_rgb888_node_t;
typedef struct idu_argb8888_node
{
    uint8_t len;
    uint32_t pixel32;    //argb8888
} idu_argb8888_node_t;
#pragma pack()

typedef struct gui_rgb_data_head
{
    unsigned char scan : 1;
    unsigned char align : 1;
    unsigned char resize: 2; //0-no resize;1-50%(x&y);2-70%;3-80%
    unsigned char compress: 1;
    unsigned char rsvd : 3;
    char type;
    short w;
    short h;
    char version;
    char rsvd2;
} gui_rgb_data_head_t;
typedef struct
{
    struct
    {
        uint8_t algorithm : 2;
        uint8_t feature_1 : 2;
        uint8_t feature_2 : 2;
        uint8_t pixel_bytes : 2;
    } algorithm_type;
    uint8_t reserved[3];
    uint32_t raw_pic_width;
    uint32_t raw_pic_height;
} idu_file_header_t;
typedef enum
{
    RGB565 = 0, //bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red, pls refs def_def.h
    ARGB8565 = 1, //bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red, bit[23:16] for Alpha
    RGB888 = 3, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red
    ARGB8888 = 4, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red, bit[24:31] for Alpha
} GUI_FormatType;
typedef struct
{
    idu_file_header_t header;
    uint32_t compressed_addr[1024]; // adjust size accordingly
} idu_file_t;

/**********************
 * UTILS
 **********************/
static void rle_memset16(uint16_t *addr, uint16_t pixel, uint32_t len) // RGB565
{
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
}
static void rle_memset32(uint32_t *addr, uint32_t pixel, uint32_t len) // ARGB8888
{
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
}
/**********************
 * STATIC PROTOTYPES
 **********************/
static bool is_rle(const lv_img_dsc_t *src_dsc);
static uint8_t get_bytes_per_pixel(uint8_t raw_bytes_per_pixel);
static lv_res_t decoder_info(lv_img_decoder_t *decoder, const void *src, lv_img_header_t *header);
static lv_res_t decoder_open(lv_img_decoder_t *decoder, lv_img_decoder_dsc_t *dsc);
static void decoder_close(lv_img_decoder_t *decoder, lv_img_decoder_dsc_t *dsc);
static lv_res_t decompress_rle_rgb565_data(const idu_file_t *file, uint8_t *img_data,
                                           uint16_t width, uint16_t height);
static lv_res_t decompress_rle_rgb888_data(const idu_file_t *file, uint8_t *img_data,
                                           uint16_t width, uint16_t height);
static lv_res_t decompress_rle_argb8565_data(const idu_file_t *file, uint8_t *img_data,
                                             uint16_t width, uint16_t height);
static lv_res_t decompress_rle_argb8888_data(const idu_file_t *file, uint8_t *img_data,
                                             uint16_t width, uint16_t height);

#ifdef LV_USE_GPU_RTK_PPE
static lv_res_t hw_acc_idu_decode(const uint8_t *image, uint8_t *output, uint16_t width,
                                  uint16_t height);
#endif

/**********************
 * GLOBAL FUNCTIONS
 **********************/
void lv_rtk_idu_init(void)
{
    lv_img_decoder_t *dec = lv_img_decoder_create();
    lv_img_decoder_set_info_cb(dec, decoder_info);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_close_cb(dec, decoder_close);

#ifdef LV_USE_GPU_RTK_PPE
    uint8_t channel1 = 1, channel2 = 3;
    hal_dma_channel_init(&channel1, &channel2);
#endif
    LV_LOG_INFO("RLE decoder initialized");
}

/**********************
 * STATIC FUNCTIONS
 **********************/
static bool is_rle(const lv_img_dsc_t *src_dsc)
{
    return (src_dsc->header.cf == LV_IMG_CF_RAW || src_dsc->header.cf == LV_IMG_CF_RAW_CHROMA_KEYED) &&
           src_dsc->data != NULL;
}
static uint8_t get_bytes_per_pixel(uint8_t raw_bytes_per_pixel)
{
    switch (raw_bytes_per_pixel)
    {
    case 0: return 2; // RGB565
    case 1: return 3; // RGB888
    case 2: return 4; // ARGB8888
    default: return 0; // ???
    }
}

static lv_res_t decoder_info(lv_img_decoder_t *decoder, const void *src, lv_img_header_t *header)
{
    LV_UNUSED(decoder);
    lv_img_src_t src_type = lv_img_src_get_type(src);          /*Get the source type*/

    /*If it's a RLE file...*/
    if (src_type == LV_IMG_SRC_FILE)
    {
        const char *fn = src;
        LV_ASSERT(strcmp(lv_fs_get_ext(fn), "rle") == 0);
        if (strcmp(lv_fs_get_ext(fn), "rle") == 0)               /*Check the extension*/
        {
            /*Save the data in the header*/
            lv_fs_file_t f;
            lv_fs_res_t res = lv_fs_open(&f, src, LV_FS_MODE_RD);
            if (res != LV_FS_RES_OK) { return LV_RES_INV; }
            uint8_t headers[8];

            lv_fs_read(&f, headers, 8, NULL);
            uint16_t width = headers[2] | (headers[3] << 8);
            uint16_t height = headers[4] | (headers[5] << 8);

            header->w = width;
            header->h = height;
            header->always_zero = 0;
            lv_fs_close(&f);
#if LV_COLOR_DEPTH == 32
            uint16_t bpp;
            memcpy(&bpp, headers + 8, 2);
            header->cf = bpp == 32 ? LV_IMG_CF_TRUE_COLOR_ALPHA : LV_IMG_CF_TRUE_COLOR;
#else
            header->cf = LV_IMG_CF_TRUE_COLOR;
#endif
            LV_LOG_INFO("decoder_info RLE %d %d %d", header->w, header->h, header->cf);
            return LV_RES_OK;
        }
    }
    else if (src_type == LV_IMG_SRC_VARIABLE)
    {
        const lv_img_dsc_t *src_dsc = (const lv_img_dsc_t *)src;
        if (!is_rle(src_dsc)) { return LV_RES_INV; }

        const uint8_t *data = src_dsc->data;
        uint16_t width = data[2] | (data[3] << 8);
        uint16_t height = data[4] | (data[5] << 8);

        uint8_t raw_bytes_per_pixel = data[8] & 0x03;
        uint8_t bytes_per_pixel = get_bytes_per_pixel(raw_bytes_per_pixel);

#if LV_COLOR_DEPTH == 32
        if (raw_bytes_per_pixel == 2)
        {
            header->cf = LV_IMG_CF_TRUE_COLOR;
        }
        else if (raw_bytes_per_pixel == 3)
        {
            header->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
        }
        else if (raw_bytes_per_pixel == 4)
        {
            header->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
        }
        else
        {
            header->cf = LV_IMG_CF_RAW_CHROMA_KEYED;
        }
#else
        header->cf = LV_IMG_CF_TRUE_COLOR;
#endif

        header->w = width;
        header->h = height;

        return LV_RES_OK;
    }
    return LV_RES_INV;
}

static lv_res_t decoder_open(lv_img_decoder_t *decoder, lv_img_decoder_dsc_t *dsc)
{
    if (dsc->src_type == LV_IMG_SRC_FILE)
    {
        const char *fn = dsc->src;
        if (strcmp(lv_fs_get_ext(fn), "rle") == 0)               /*Check the extension*/
        {

            uint8_t *rle_data;
            lv_fs_file_t f;
            lv_fs_res_t res = lv_fs_open(&f, fn, LV_FS_MODE_RD);
            if (res != LV_FS_RES_OK) { return LV_RES_INV; }
            uint8_t headers[20];

            lv_fs_ioctl(&f, (void **)&rle_data, 0);
            memcpy(headers, rle_data, 20);
            uint8_t idu_type = headers[8] & 0x03;// Lowest 2 bits
            uint16_t width = headers[2] | (headers[3] << 8);
            uint16_t height = headers[4] | (headers[5] << 8);
            uint8_t raw_bytes_per_pixel = headers[8] >> 6;
            uint8_t bytes_per_pixel = get_bytes_per_pixel(raw_bytes_per_pixel);

            lv_fs_close(&f);
            dsc->header.w = width;
            dsc->header.h = height;
            LV_ASSERT(idu_type == 0); //source RLE

            if (decompress_flag == (uint8_t *)rle_data)
            {
                dsc->img_data = psram_img_cache;
                return LV_RES_OK;
            }
            uint32_t img_size = width * height * bytes_per_pixel;
            uint8_t *img_data = lv_mem_alloc(img_size);

            psram_img_cache = (uint8_t *)img_data;

            idu_file_t *file = (idu_file_t *)(rle_data + 8);
            lv_res_t ret;

#ifdef LV_USE_GPU_RTK_PPE
            ret = hw_acc_idu_decode(rle_data, img_data, width, height);

            if (ret == LV_RES_OK)
            {
                decompress_flag = (uint8_t *)rle_data;
                dsc->img_data = (uint8_t *)img_data;
                LV_LOG_INFO("Hardware decode succeeded.");
                return LV_RES_OK;
            }
#endif
            char input_type = headers[1];
            switch (input_type)
            {
            case RGB565:
                ret = decompress_rle_rgb565_data(file, img_data, width, height);
                break;
            case RGB888:
                ret = decompress_rle_rgb888_data(file, img_data, width, height);
                break;
            case ARGB8565:
                ret = decompress_rle_argb8565_data(file, img_data, width, height);
                break;
            case ARGB8888:
                ret = decompress_rle_argb8888_data(file, img_data, width, height);
                break;
            default:
                LV_LOG_ERROR("Unsupported bytes per pixel: %u", bytes_per_pixel);
                return LV_RES_INV;
            }

            if (ret != LV_RES_OK)
            {
                LV_LOG_ERROR("Software decode failed.");
                lv_mem_free(img_data);
                return ret;
            }
            else
            {
                LV_LOG_INFO("Software decode succeeded.");
            }
            decompress_flag = (uint8_t *)rle_data;
            dsc->img_data = (uint8_t *)img_data;

            return LV_RES_OK;     /*The image is fully decoded. Return with its pointer*/
        }

    }
    else if (dsc->src_type == LV_IMG_SRC_VARIABLE)
    {
        const lv_img_dsc_t *src_dsc = (const lv_img_dsc_t *)dsc->src;
        LV_ASSERT(src_dsc != NULL);
        const uint8_t *data = src_dsc->data;
        uint16_t width = data[2] | (data[3] << 8);
        uint16_t height = data[4] | (data[5] << 8);

        uint8_t raw_bytes_per_pixel = data[8] >> 6;
        uint8_t idu_type = data[8] & 0x03;
        uint8_t bytes_per_pixel = get_bytes_per_pixel(raw_bytes_per_pixel);

        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)src_dsc->data;
        char input_type = head->type;

        const uint8_t *rle_header = data + 8;
        idu_file_t *file = (idu_file_t *)rle_header;

        uint32_t img_size = width * height * bytes_per_pixel;
        dsc->header.w = width;
        dsc->header.h = height;

        LV_ASSERT(idu_type == 0); //source RLE

        if (decompress_flag == (uint8_t *)src_dsc->data)
        {
            dsc->img_data = psram_img_cache;
            return LV_RES_OK;
        }
        uint8_t *img_data = lv_mem_alloc(img_size); //(uint8_t *)lv_psram_decoder_rle_addr;
        psram_img_cache = (uint8_t *)img_data;

        lv_res_t ret;

#ifdef LV_USE_GPU_RTK_PPE
        ret = hw_acc_idu_decode(data, img_data, width, height);

        if (ret == LV_RES_OK)
        {
            decompress_flag = (uint8_t *)data;
            dsc->img_data = (uint8_t *)img_data;
            LV_LOG_INFO("Hardware decode succeeded.");
            return LV_RES_OK;
        }
#endif
        switch (input_type)
        {
        case RGB565:
            ret = decompress_rle_rgb565_data(file, img_data, width, height);
            break;
        case RGB888:
            ret = decompress_rle_rgb888_data(file, img_data, width, height);
            break;
        case ARGB8565:
            ret = decompress_rle_argb8565_data(file, img_data, width, height);
            break;
        case ARGB8888:
            ret = decompress_rle_argb8888_data(file, img_data, width, height);
            break;
        default:
            LV_LOG_ERROR("Unsupported bytes per pixel: %u", bytes_per_pixel);
            return LV_RES_INV;
        }

        if (ret != LV_RES_OK)
        {
            lv_mem_free(img_data);
            LV_LOG_ERROR("Software decode failed.");
            return ret;
        }
        else
        {
            LV_LOG_INFO("Software decode succeeded.");
        }

        decompress_flag = (uint8_t *)src_dsc->data;
        dsc->img_data = (uint8_t *)img_data;

        return LV_RES_OK;
    }
    return LV_RES_INV;    /*If not returned earlier then it failed*/

}

static void decoder_close(lv_img_decoder_t *decoder, lv_img_decoder_dsc_t *dsc)
{
    LV_UNUSED(decoder);
    dsc->img_data = NULL;
    if (dsc->user_data)
    {
        lv_mem_free(dsc->user_data);
        dsc->user_data = NULL;
    }
    LV_LOG_INFO("Closed RLE image");
}
#if LV_USE_GPU_RTK_PPE
static lv_res_t hw_acc_idu_decode(const uint8_t *image, uint8_t *output, uint16_t width,
                                  uint16_t height)
{
    if (image == NULL || output == NULL)
    {
        LV_ASSERT(image != NULL && output != NULL);
        return LV_RES_INV;
    }

    hal_idu_decompress_info info;
    info.start_column = 0;
    info.end_column = width - 1;
    info.start_line = 0;
    info.end_line = height - 1;
    info.raw_data_address = (uint32_t)(image + 8);

    bool ret = hal_idu_decompress(&info, output);

    if (!ret)
    {
        return LV_RES_OK;
    }
    else
    {
        return LV_RES_INV;
    }
}
#endif
static lv_res_t decompress_rle_rgb565_data(const idu_file_t *file, uint8_t *img_data,
                                           uint16_t width, uint16_t height)
{
    LV_ASSERT(file != NULL && img_data != NULL);
    LV_ASSERT(width > 0 && height > 0);

    uint16_t *linebuf = (uint16_t *)img_data;
    for (uint32_t y = 0; y < height; y++)
    {
        uint32_t start = (uint32_t)(uintptr_t)file + file->compressed_addr[y];
        uint32_t end = (uint32_t)(uintptr_t)file + file->compressed_addr[y + 1];
        for (uint32_t addr = start; addr < end;)
        {
            idu_rgb565_node_t *node = (idu_rgb565_node_t *)(uintptr_t)addr;
            rle_memset16(linebuf, node->pixel16, node->len);
            addr += sizeof(idu_rgb565_node_t);
            linebuf += node->len;
        }
    }

    return LV_RES_OK;
}
static lv_res_t decompress_rle_argb8565_data(const idu_file_t *file, uint8_t *img_data,
                                             uint16_t width, uint16_t height)
{
    LV_ASSERT(file != NULL && img_data != NULL);
    LV_ASSERT(width > 0 && height > 0);

    uint8_t *linebuf = (uint8_t *)img_data;
    for (uint32_t line = 0; line < height; line++)
    {
        uint32_t start = (uint32_t)(uintptr_t)file + file->compressed_addr[line];
        uint32_t end = (uint32_t)(uintptr_t)file + file->compressed_addr[line + 1];
        for (uint32_t addr = start; addr < end;)
        {
            idu_argb8565_node_t *node = (idu_argb8565_node_t *)(uintptr_t)addr;
            for (uint32_t i = 0; i < node->len; i++)
            {
                linebuf[i * 3 + 0] = node->pixel & 0xff;
                linebuf[i * 3 + 1] = (node->pixel >> 8) & 0xff;
                linebuf[i * 3 + 2] = node->alpha;
            }
            addr += sizeof(idu_argb8565_node_t);
            linebuf += node->len * 3;
        }
    }

    return LV_RES_OK;
}


static lv_res_t decompress_rle_rgb888_data(const idu_file_t *file, uint8_t *img_data,
                                           uint16_t width, uint16_t height)
{
    LV_ASSERT(file != NULL && img_data != NULL);
    LV_ASSERT(width > 0 && height > 0);

    uint8_t *linebuf = (uint8_t *)img_data;
    for (uint32_t line = 0; line < height; line++)
    {
        uint32_t start = (uint32_t)(uintptr_t)file + file->compressed_addr[line];
        uint32_t end = (uint32_t)(uintptr_t)file + file->compressed_addr[line + 1];


        for (uint32_t addr = start; addr < end;)
        {
            idu_rgb888_node_t *node = (idu_rgb888_node_t *)(uintptr_t)addr;
            for (uint32_t i = 0; i < node->len; i++)
            {
                linebuf[i * 3]     = node->pixel_b;
                linebuf[i * 3 + 1] = node->pixel_g;
                linebuf[i * 3 + 2] = node->pixel_r;
            }
            addr += sizeof(idu_rgb888_node_t);
            linebuf += node->len * 3;
        }
    }
    return LV_RES_OK;
}

static lv_res_t decompress_rle_argb8888_data(const idu_file_t *file, uint8_t *img_data,
                                             uint16_t width, uint16_t height)
{
    LV_ASSERT(file != NULL && img_data != NULL);
    LV_ASSERT(width > 0 && height > 0);

    uint32_t *linebuf = (uint32_t *)img_data;
    for (uint32_t line = 0; line < height; line++)
    {
        uint32_t start = (uint32_t)(uintptr_t)file + file->compressed_addr[line];
        uint32_t end = (uint32_t)(uintptr_t)file + file->compressed_addr[line + 1];

        for (uint32_t addr = start; addr < end;)
        {
            idu_argb8888_node_t *node = (idu_argb8888_node_t *)(uintptr_t)addr;
            rle_memset32(linebuf, node->pixel32, node->len);

            addr += sizeof(idu_argb8888_node_t);
            linebuf += node->len;
        }
    }
    return LV_RES_OK;
}


#endif /* LV_USE_RTK_IDU */