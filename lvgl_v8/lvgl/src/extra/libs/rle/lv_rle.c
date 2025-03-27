/**
 * @file lv_rle.c
 *
 */
/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

#if LV_USE_RTK_IDU

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#ifdef __arm__
#include "trace.h"
#endif

#if (LV_USE_GPU_RTK_PPE == 1)
#include "rtl_idu.h"
#include "lv_draw_rtk_ppe_utils.h"
#include "rtl_gdma.h"
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if (LV_USE_GPU_RTK_PPE == 1)
static uint8_t high_speed_dma_channel = 0xFF;
static uint8_t low_speed_dma_channel = 0xFF;
#endif
/**********************
 * GLOBAL VARIABLES
 **********************/

typedef struct
{
    union
    {
        const uint8_t *src_addr;
        const char src_file[256];
    };
    uint8_t *rgb_data;
    lv_img_src_t src_type;
    // bool valid;
} rle_cache_t;
static rle_cache_t rle_cache;


// static uint8_t *decompress_flag = NULL;
// static uint8_t *psram_img_cache = NULL;  // PSRAM image decode data
// static uint32_t psram_img_cache_size = 0;  // cache size
// static bool psram_img_cache_valid = false;  // cache flag

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

#if (LV_USE_GPU_RTK_PPE == 1)
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

#if (LV_USE_GPU_RTK_PPE == 1)
    uint8_t channel1 = 1, channel2 = 3;
    lv_ppe_register_dma_channel(channel1, channel2);
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
        // LV_ASSERT(strcmp(lv_fs_get_ext(fn), "rle") == 0);
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
            char input_type = headers[1];

            if (input_type == RGB565)
            {
                header->cf = LV_IMG_CF_TRUE_COLOR;
            }
            else if (input_type == RGB888)
            {
                header->cf = LV_IMG_CF_TRUE_COLOR;
            }
            else if (input_type == ARGB8565)
            {
                header->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
            }
            else if (input_type == ARGB8888)
            {
                header->cf = LV_IMG_CF_TRUE_COLOR;
            }
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

        uint8_t input_type = data[1];
        if (input_type == RGB565)
        {
            header->cf = LV_IMG_CF_TRUE_COLOR;
        }
        else if (input_type == RGB888)
        {
            header->cf = LV_IMG_CF_TRUE_COLOR;
        }
        else if (input_type == ARGB8565)
        {
            header->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
        }
        else if (input_type == ARGB8888)
        {
            header->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
        }

        header->w = width;
        header->h = height;

        return LV_RES_OK;
    }
    return LV_RES_INV;
}

static lv_res_t decoder_open(lv_img_decoder_t *decoder, lv_img_decoder_dsc_t *dsc)
{
    // search cache
    LV_LOG_INFO("rgb_data 0x%x.", rle_cache.rgb_data);
    if (rle_cache.rgb_data)
    {
        if (dsc->src_type == rle_cache.src_type)
        {
            if (((rle_cache.src_type == LV_IMG_SRC_VARIABLE) && (dsc->src == rle_cache.src_addr)) ||
                ((rle_cache.src_type == LV_IMG_SRC_FILE) && strcmp(dsc->src, rle_cache.src_file) == 0)
               )
            {
                dsc->img_data = rle_cache.rgb_data;
                LV_LOG_INFO("Get img from cache.");
                return LV_RES_OK;
            }
        }
        // clear cache immediately, if no enough memory for two decoded images
        LV_LOG_INFO("Clear img cache.");
        if (rle_cache.rgb_data)
        {
#if (LV_IDU_MEM_CUSTOM == 1)
#else
            lv_mem_free((void *)rle_cache.rgb_data);
#endif
        }
        memset((void *)&rle_cache, 0, sizeof(rle_cache));
    }

    if (dsc->src_type == LV_IMG_SRC_FILE)
    {
        void *file_data = NULL;
        bool file_ioctl = true;
        const char *fn = dsc->src;
        if (strcmp(lv_fs_get_ext(fn), "rle") == 0)               /*Check the extension*/
        {
            //DBG_DIRECT(" %s %d\n", __FUNCTION__, __LINE__);
            lv_fs_file_t f;
            uint8_t headers[20];
            lv_fs_res_t res = lv_fs_open(&f, fn, LV_FS_MODE_RD);
            if (res != LV_FS_RES_OK)
            {
                //DBG_DIRECT(" %s %d\n", __FUNCTION__, __LINE__);
                return LV_RES_INV;
            }

            res = lv_fs_ioctl(&f, (void **)&file_data, 0);
            if (res == LV_FS_RES_NOT_IMP)
            {
                file_ioctl = false;
                do
                {
                    LV_LOG_INFO("load img into ram! %s\n", fn);
                    uint32_t f_sz = 0;
                    uint32_t rd_sz = 0;
                    res = lv_fs_seek(&f, 0, LV_FS_SEEK_END);
                    if (res != LV_FS_RES_OK) {break;}
                    res = lv_fs_tell(&f, &f_sz);
                    if (res != LV_FS_RES_OK) {break;}

                    file_data = lv_mem_alloc(f_sz);
                    LV_ASSERT_MALLOC(file_data);
                    res = lv_fs_seek(&f, 0, LV_FS_SEEK_SET);
                    if (res != LV_FS_RES_OK)
                    {
                        lv_mem_free((void *)file_data);
                        file_data = NULL;
                        break;
                    }
                    res = lv_fs_read(&f, file_data, f_sz, &rd_sz);
                    if (res != LV_FS_RES_OK || f_sz != rd_sz)
                    {
                        lv_mem_free((void *)file_data);
                        file_data = NULL;
                        break;
                    }
                }
                while (0);
            }
            lv_fs_close(&f);
            if (res != LV_FS_RES_OK || !file_data)
            {
                //DBG_DIRECT(" %s %d\n", __FUNCTION__, __LINE__);
                return LV_RES_INV;
            }

            memcpy(headers, file_data, 20);
            uint8_t idu_type = headers[8] & 0x03;// Lowest 2 bits
            uint16_t width = headers[2] | (headers[3] << 8);
            uint16_t height = headers[4] | (headers[5] << 8);
            uint8_t raw_bytes_per_pixel = headers[8] >> 6;
            uint8_t bytes_per_pixel = get_bytes_per_pixel(raw_bytes_per_pixel);

            dsc->header.w = width;
            dsc->header.h = height;
            LV_ASSERT(idu_type == 0); //source RLE

            uint32_t img_size = width * height * bytes_per_pixel;
#if (LV_IDU_MEM_CUSTOM == 1)
            uint8_t *img_data = (uint8_t *)LV_IDU_MEM_ADR;
#else
            uint8_t *img_data = lv_mem_alloc(img_size);
#endif

            idu_file_t *file = (idu_file_t *)((uint8_t *)file_data + 8);
            lv_res_t ret;

#if (LV_USE_GPU_RTK_PPE == 1)
            ret = hw_acc_idu_decode(file_data, img_data, width, height);

            if (ret == LV_RES_OK)
            {
                dsc->img_data = (uint8_t *)img_data;
                if (false == file_ioctl && file_data)
                {
                    lv_mem_free((void *)file_data);
                }
                LV_LOG_INFO("Hardware decode succeeded.");

                // update rle cache
                LV_LOG_INFO("Update cache.");
                rle_cache.src_type = dsc->src_type;
                memset((void *)rle_cache.src_file, 0, sizeof(rle_cache.src_file));
                strncpy((char *)rle_cache.src_file, dsc->src, strlen(dsc->src));
                LV_LOG_INFO("rgb_data 0x%x img_data 0x%x.", rle_cache.rgb_data, img_data);
                if (rle_cache.rgb_data)
                {
#if (LV_IDU_MEM_CUSTOM == 1)
#else
                    lv_mem_free((void *)rle_cache.rgb_data);
#endif
                }
                rle_cache.rgb_data = img_data;
                return LV_RES_OK;
            }
#endif
            //DBG_DIRECT(" %s %d\n", __FUNCTION__, __LINE__);
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

            if (false == file_ioctl && file_data)
            {
                lv_mem_free((void *)file_data);
            }
            if (ret != LV_RES_OK)
            {
                LV_LOG_ERROR("Software decode failed.");
#if (LV_IDU_MEM_CUSTOM == 1)
#else
                lv_mem_free((void *)img_data);
#endif
                img_data = NULL;
                return ret;
            }
            else
            {
                LV_LOG_INFO("Software decode succeeded.");
            }
            dsc->img_data = (uint8_t *)img_data;

            // update rle cache
            LV_LOG_INFO("Update cache.");
            rle_cache.src_type = dsc->src_type;
            memset((void *)rle_cache.src_file, 0, sizeof(rle_cache.src_file));
            strncpy((char *)rle_cache.src_file, dsc->src, strlen(dsc->src));
            LV_LOG_INFO("rgb_data 0x%x img_data 0x%x.", rle_cache.rgb_data, img_data);
            if (rle_cache.rgb_data)
            {
#if (LV_IDU_MEM_CUSTOM == 1)
#else
                lv_mem_free((void *)rle_cache.rgb_data);
#endif
            }
            rle_cache.rgb_data = img_data;

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

        LV_LOG_INFO("0x%x,  w %d, h %d idu %d", data, width, height, idu_type);
        LV_ASSERT(idu_type == 0); //source RLE

#if (LV_IDU_MEM_CUSTOM == 1)
        uint8_t *img_data = (uint8_t *)LV_IDU_MEM_ADR;
#else
        uint8_t *img_data = lv_mem_alloc(img_size);
#endif

        lv_res_t ret;

#if (LV_USE_GPU_RTK_PPE == 1)
        ret = hw_acc_idu_decode(data, img_data, width, height);

        if (ret == LV_RES_OK)
        {
            dsc->img_data = (uint8_t *)img_data;
            LV_LOG_INFO("Hardware decode succeeded.");

            // update rle cache
            LV_LOG_INFO("Update cache.");
            rle_cache.src_type = dsc->src_type;
            rle_cache.src_addr = dsc->src;
            if (rle_cache.rgb_data)
            {
#if (LV_IDU_MEM_CUSTOM == 1)
#else
                lv_mem_free((void *)rle_cache.rgb_data);
#endif
            }
            rle_cache.rgb_data = img_data;
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
            LV_LOG_ERROR("Software decode failed.");
#if (LV_IDU_MEM_CUSTOM == 1)
#else
            lv_mem_free((void *)img_data);
#endif
            img_data = NULL;
            return ret;
        }
        else
        {
            LV_LOG_INFO("Software decode succeeded.");
        }

        dsc->img_data = (uint8_t *)img_data;

        // update rle cache
        LV_LOG_INFO("Update cache.");
        rle_cache.src_type = dsc->src_type;
        rle_cache.src_addr = dsc->src;
        if (rle_cache.rgb_data)
        {
#if (LV_IDU_MEM_CUSTOM == 1)
#else
            lv_mem_free((void *)rle_cache.rgb_data);
#endif
        }
        rle_cache.rgb_data = img_data;
        return LV_RES_OK;
    }
    return LV_RES_INV;    /*If not returned earlier then it failed*/

}

static void decoder_close(lv_img_decoder_t *decoder, lv_img_decoder_dsc_t *dsc)
{
    LV_UNUSED(decoder);
    LV_LOG_INFO("Close RLE decoder");
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
    idu_file_header_t *rle_headers = (idu_file_header_t *)image;
    IDU_decode_range info;
    info.start_column = 0;
    info.end_column = width - 1;
    info.start_line = 0;
    info.end_line = height - 1;
    info.target_stride = rle_headers->raw_pic_width * rle_headers->algorithm_type.pixel_bytes;
    IDU_DMA_config dma_cfg;
    dma_cfg.output_buf = (uint32_t *)output;
    dma_cfg.RX_DMA_channel_num = lv_ppe_get_high_speed_dma();
    dma_cfg.TX_DMA_channel_num = lv_ppe_get_low_speed_dma();

//    bool ret = hal_idu_decompress(&info, output);
    IDU_ERROR ret = IDU_Decode((uint8_t *)(image + 8), &info, &dma_cfg);

    if (!ret)
    {
        return LV_RES_INV;
    }
    else
    {
        return LV_RES_OK;
    }
}
#endif
static lv_res_t decompress_rle_rgb565_data(const idu_file_t *file, uint8_t *img_data,
                                           uint16_t width, uint16_t height)
{
    LV_ASSERT(file != NULL && img_data != NULL);
    LV_ASSERT(width > 0 && height > 0);

#if LV_COLOR_DEPTH == 16
    // For 16-bit color depth (RGB565), directly use the RGB565 data
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
#elif LV_COLOR_DEPTH == 24
    // For 24-bit color depth, convert RGB565 to RGB888
    uint8_t *linebuf = (uint8_t *)img_data;
    for (uint32_t y = 0; y < height; y++)
    {
        uint32_t start = (uint32_t)(uintptr_t)file + file->compressed_addr[y];
        uint32_t end = (uint32_t)(uintptr_t)file + file->compressed_addr[y + 1];
        for (uint32_t addr = start; addr < end;)
        {
            idu_rgb565_node_t *node = (idu_rgb565_node_t *)(uintptr_t)addr;
            for (uint32_t i = 0; i < node->len; i++)
            {
                uint16_t rgb565 = node->pixel16;
                uint8_t r = (rgb565 >> 11) & 0x1F;
                uint8_t g = (rgb565 >> 5) & 0x3F;
                uint8_t b = rgb565 & 0x1F;

                // Convert to RGB888
                linebuf[i * 3 + 0] = (r << 3) | (r >> 2); // Red
                linebuf[i * 3 + 1] = (g << 2) | (g >> 4); // Green
                linebuf[i * 3 + 2] = (b << 3) | (b >> 2); // Blue
            }
            addr += sizeof(idu_rgb565_node_t);
            linebuf += node->len * 3;
        }
    }
#elif LV_COLOR_DEPTH == 32
    // For 32-bit color depth, convert RGB565 to RGB888 and add a fully opaque alpha channel
    uint8_t *linebuf = (uint8_t *)img_data;
    for (uint32_t y = 0; y < height; y++)
    {
        uint32_t start = (uint32_t)(uintptr_t)file + file->compressed_addr[y];
        uint32_t end = (uint32_t)(uintptr_t)file + file->compressed_addr[y + 1];
        for (uint32_t addr = start; addr < end;)
        {
            idu_rgb565_node_t *node = (idu_rgb565_node_t *)(uintptr_t)addr;
            for (uint32_t i = 0; i < node->len; i++)
            {
                uint16_t rgb565 = node->pixel16;
                uint8_t r = (rgb565 >> 11) & 0x1F;
                uint8_t g = (rgb565 >> 5) & 0x3F;
                uint8_t b = rgb565 & 0x1F;

                // Convert to RGB888 with full opacity in alpha channel
                linebuf[i * 4 + 0] = (r << 3) | (r >> 2); // Red
                linebuf[i * 4 + 1] = (g << 2) | (g >> 4); // Green
                linebuf[i * 4 + 2] = (b << 3) | (b >> 2); // Blue
                linebuf[i * 4 + 3] = 255;                // Alpha (fully opaque)
            }
            addr += sizeof(idu_rgb565_node_t);
            linebuf += node->len * 4;
        }
    }
#else
    LV_LOG_ERROR("Unsupported LV_COLOR_DEPTH");
#endif

    return LV_RES_OK;
}


static lv_res_t decompress_rle_argb8565_data(const idu_file_t *file, uint8_t *img_data,
                                             uint16_t width, uint16_t height)
{
    LV_ASSERT(file != NULL && img_data != NULL);
    LV_ASSERT(width > 0 && height > 0);

#if LV_COLOR_DEPTH == 16
    // For 16-bit color depth (RGB565), directly use the RGB565 part and ignore alpha
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
#elif LV_COLOR_DEPTH == 24
    // For 24-bit color depth, convert RGB565 to RGB888 without alpha
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
                uint16_t rgb565 = node->pixel;
                uint8_t r = (rgb565 >> 11) & 0x1F;
                uint8_t g = (rgb565 >> 5) & 0x3F;
                uint8_t b = rgb565 & 0x1F;

                // Convert to RGB888
                linebuf[i * 3 + 0] = (r << 3) | (r >> 2); // Red
                linebuf[i * 3 + 1] = (g << 2) | (g >> 4); // Green
                linebuf[i * 3 + 2] = (b << 3) | (b >> 2); // Blue
            }
            addr += sizeof(idu_argb8565_node_t);
            linebuf += node->len * 3;
        }
    }
#elif LV_COLOR_DEPTH == 32
    // For 32-bit color depth, convert RGB565 to RGB888 and handle alpha channel
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
                uint16_t rgb565 = node->pixel;
                uint8_t r = (rgb565 >> 11) & 0x1F;
                uint8_t g = (rgb565 >> 5) & 0x3F;
                uint8_t b = rgb565 & 0x1F;

                // Convert to RGB888 and include alpha
                linebuf[i * 4 + 0] = (r << 3) | (r >> 2); // Red
                linebuf[i * 4 + 1] = (g << 2) | (g >> 4); // Green
                linebuf[i * 4 + 2] = (b << 3) | (b >> 2); // Blue
                linebuf[i * 4 + 3] = node->alpha;         // Alpha
            }
            addr += sizeof(idu_argb8565_node_t);
            linebuf += node->len * 4;
        }
    }
#else
    LV_LOG_ERROR("Unsupported LV_COLOR_DEPTH");
#endif

    return LV_RES_OK;
}


static lv_res_t decompress_rle_rgb888_data(const idu_file_t *file, uint8_t *img_data,
                                           uint16_t width, uint16_t height)
{
    LV_ASSERT(file != NULL && img_data != NULL);
    LV_ASSERT(width > 0 && height > 0);

#if LV_COLOR_DEPTH == 16
    // For 16-bit color depth, convert RGB888 to RGB565
    uint16_t *linebuf = (uint16_t *)img_data;
    for (uint32_t line = 0; line < height; line++)
    {
        uint32_t start = (uint32_t)(uintptr_t)file + file->compressed_addr[line];
        uint32_t end = (uint32_t)(uintptr_t)file + file->compressed_addr[line + 1];

        for (uint32_t addr = start; addr < end;)
        {
            idu_rgb888_node_t *node = (idu_rgb888_node_t *)(uintptr_t)addr;
            for (uint32_t i = 0; i < node->len; i++)
            {
                uint8_t r = node->pixel_r;
                uint8_t g = node->pixel_g;
                uint8_t b = node->pixel_b;

                // Convert RGB888 to RGB565
                uint16_t rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                linebuf[i] = rgb565;
            }
            addr += sizeof(idu_rgb888_node_t);
            linebuf += node->len;
        }
    }
#elif LV_COLOR_DEPTH == 24
    // For 24-bit color depth, directly copy RGB888 data
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
                linebuf[0] = node->pixel_r;
                linebuf[1] = node->pixel_g;
                linebuf[2] = node->pixel_b;
                linebuf += 3;
            }
            addr += sizeof(idu_rgb888_node_t);
        }
    }
#elif LV_COLOR_DEPTH == 32
    // For 32-bit color depth, copy RGB888 data and set alpha to fully opaque
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
                linebuf[0] = node->pixel_r;
                linebuf[1] = node->pixel_g;
                linebuf[2] = node->pixel_b;
                linebuf[3] = 0xFF; // Alpha channel set to fully opaque
                linebuf += 4;
            }
            addr += sizeof(idu_rgb888_node_t);
        }
    }
#else
    LV_LOG_ERROR("Unsupported LV_COLOR_DEPTH");
#endif

    return LV_RES_OK;
}

static lv_res_t decompress_rle_argb8888_data(const idu_file_t *file, uint8_t *img_data,
                                             uint16_t width, uint16_t height)
{
    LV_ASSERT(file != NULL && img_data != NULL);
    LV_ASSERT(width > 0 && height > 0);

#if LV_COLOR_DEPTH == 32
    // For 32-bit depth, directly copy ARGB8888 data as it's a perfect match
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
#elif LV_COLOR_DEPTH == 24
    // For 24-bit depth, convert ARGB8888 to RGB888, ignoring the alpha channel
    uint8_t *linebuf = (uint8_t *)img_data;
    for (uint32_t line = 0; line < height; line++)
    {
        uint32_t start = (uint32_t)(uintptr_t)file + file->compressed_addr[line];
        uint32_t end = (uint32_t)(uintptr_t)file + file->compressed_addr[line + 1];

        for (uint32_t addr = start; addr < end;)
        {
            idu_argb8888_node_t *node = (idu_argb8888_node_t *)(uintptr_t)addr;
            for (uint32_t i = 0; i < node->len; i++)
            {
                // Extract RGB components from ARGB8888
                linebuf[0] = (node->pixel32 >> 16) & 0xFF; // Red
                linebuf[1] = (node->pixel32 >> 8) & 0xFF;  // Green
                linebuf[2] = node->pixel32 & 0xFF;         // Blue
                linebuf += 3;
            }
            addr += sizeof(idu_argb8888_node_t);
        }
    }
#elif LV_COLOR_DEPTH == 16
    // For 16-bit depth, convert ARGB8888 to RGB565, ignoring the alpha channel
    uint16_t *linebuf = (uint16_t *)img_data;
    for (uint32_t line = 0; line < height; line++)
    {
        uint32_t start = (uint32_t)(uintptr_t)file + file->compressed_addr[line];
        uint32_t end = (uint32_t)(uintptr_t)file + file->compressed_addr[line + 1];

        for (uint32_t addr = start; addr < end;)
        {
            idu_argb8888_node_t *node = (idu_argb8888_node_t *)(uintptr_t)addr;
            for (uint32_t i = 0; i < node->len; i++)
            {
                // Extract RGB components from ARGB8888
                uint8_t r = (node->pixel32 >> 16) & 0xFF;
                uint8_t g = (node->pixel32 >> 8) & 0xFF;
                uint8_t b = node->pixel32 & 0xFF;

                // Convert ARGB8888 to RGB565, discarding the alpha channel
                uint16_t rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                linebuf[i] = rgb565;
            }
            addr += sizeof(idu_argb8888_node_t);
            linebuf += node->len;
        }
    }
#else
    LV_LOG_ERROR("Unsupported LV_COLOR_DEPTH");
#endif

    return LV_RES_OK;
}


#endif /* LV_USE_RTK_IDU */
