/**
 * @file lv_idu.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../draw/lv_image_decoder_private.h"
#include "../../../lvgl.h"

#if LV_USE_RTK_IDU

// #include <string.h>
#include "../../core/lv_global.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#ifdef LV_USE_GPU_RTK_PPE
#include "hal_idu.h"
#endif
/*********************
 *      DEFINES
 *********************/
#define DECODER_NAME    "IDU"
#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


/**********************
 * GLOBAL VARIABLES
 **********************/

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
 *  STATIC PROTOTYPES
 **********************/
static bool is_rle(const lv_image_dsc_t *src_dsc);
static uint8_t get_bytes_per_pixel(uint8_t raw_bytes_per_pixel);
static lv_result_t idu_decoder_info(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t * src, lv_image_header_t *header);
static lv_result_t idu_decoder_open(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc);
void idu_decoder_close(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc);
lv_result_t decompress_rle_rgb565_data(const idu_file_t *file, uint8_t *img_data,
                                           uint16_t width, uint16_t height);
lv_result_t decompress_rle_rgb888_data(const idu_file_t *file, uint8_t *img_data,
                                           uint16_t width, uint16_t height);
lv_result_t decompress_rle_argb8565_data(const idu_file_t *file, uint8_t *img_data,
                                             uint16_t width, uint16_t height);
lv_result_t decompress_rle_argb8888_data(const idu_file_t *file, uint8_t *img_data,
                                             uint16_t width, uint16_t height);
lv_result_t decompress_rle_data(char input_type, idu_file_t *file, uint8_t *img_data, uint16_t width, uint16_t height);
#ifdef LV_USE_GPU_RTK_PPE
static lv_result_t hw_acc_idu_decode(const uint8_t *image, uint8_t *output, uint16_t width,
                                  uint16_t height);
#endif

/**********************
 * GLOBAL FUNCTIONS
 **********************/
void lv_rtk_idu_init(void)
{
    lv_image_decoder_t *dec = lv_image_decoder_create();
    LV_ASSERT_MALLOC(dec);
    if(dec == NULL) {
        LV_LOG_WARN("Out of memory");
        return;
    }
    lv_image_decoder_set_info_cb(dec, idu_decoder_info);
    lv_image_decoder_set_open_cb(dec, idu_decoder_open);
    lv_image_decoder_set_close_cb(dec, idu_decoder_close);
    dec->name = DECODER_NAME;
#ifdef LV_USE_GPU_RTK_PPE
    uint8_t channel1 = 1, channel2 = 3;
    hal_dma_channel_init(&channel1, &channel2);
#endif
    // LV_LOG_INFO("RLE decoder initialized");
}
/**********************
 * STATIC FUNCTIONS
 **********************/
static bool is_rle(const lv_image_dsc_t *src_dsc)
{
    return (src_dsc->header.cf == LV_COLOR_FORMAT_RAW) &&
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

static lv_result_t idu_decoder_info(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc, lv_image_header_t *header)
{
    LV_UNUSED(decoder);
    const void * src = dsc->src;
    lv_image_src_t src_type = dsc->src_type;          /*Get the source type*/

    /*If it's a RLE file...*/
    if (src_type == LV_IMAGE_SRC_FILE)
    {
        LV_LOG_ERROR("Failed to LV_IMAGE_SRC_FILE");
        return LV_RESULT_INVALID;
        // const char *fn = dsc->src;
        // LV_ASSERT(strcmp(lv_fs_get_ext(fn), "rle") == 0);
        // if (strcmp(lv_fs_get_ext(fn), "rle") == 0)               /*Check the extension*/
        // {
        //     /*Save the data in the header*/
        //     lv_fs_file_t f;
        //     lv_fs_res_t res = lv_fs_open(&f, dsc, LV_FS_MODE_RD);
        //     if (res != LV_FS_RES_OK) { return LV_RESULT_INVALID; }
        //     uint8_t headers[8];

        //     lv_fs_read(&f, headers, 8, NULL);
        //     uint16_t width = headers[2] | (headers[3] << 8);
        //     uint16_t height = headers[4] | (headers[5] << 8);

        //     header->w = width;
        //     header->h = height;
        //     // lv_fs_close(&f);
        //     char input_type = headers[1];

        //     if (input_type == RGB565)
        //     {
        //         header->cf = LV_COLOR_FORMAT_RGB565;
        //     }
        //     else if (input_type == RGB888)
        //     {
        //         header->cf = LV_COLOR_FORMAT_RGB888;
        //     }
        //     else if (input_type == ARGB8565)
        //     {
        //         header->cf = LV_COLOR_FORMAT_ARGB8565;
        //     }
        //     else if (input_type == ARGB8888)
        //     {
        //         header->cf = LV_COLOR_FORMAT_ARGB8888;
        //     }
        //     header->stride = lv_draw_buf_width_to_stride(header->w, header->cf);
        //     LV_LOG_INFO("idu_decoder_info RLE %d %d %d", header->w, header->h, header->cf);
        //     return LV_RESULT_OK;
        // }
    }
    else if (dsc->src_type == LV_IMAGE_SRC_VARIABLE)
    {
        const lv_image_dsc_t *src_dsc = (const lv_image_dsc_t *)dsc->src;
        LV_ASSERT(src_dsc != NULL);
        // if (!is_rle(src_dsc)) { return LV_RESULT_INVALID; }
        const uint8_t *data = src_dsc->data;
        uint16_t width = data[2] | (data[3] << 8);
        uint16_t height = data[4] | (data[5] << 8);

        uint8_t idu_type = data[8] & 0x03;

        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)src_dsc->data;
        char input_type = head->type;

        const uint8_t *rle_header = data + 8;
        idu_file_t *file = (idu_file_t *)rle_header;

        lv_image_dsc_t * image = (lv_image_dsc_t *)src;
        lv_memcpy(header, &image->header, sizeof(lv_image_header_t));
        if (input_type == RGB565)
        {
            header->cf = LV_COLOR_FORMAT_RGB565;
        }
        else if (input_type == RGB888)
        {
            header->cf = LV_COLOR_FORMAT_RGB888;
        }
        else if (input_type == ARGB8565)
        {
            header->cf = LV_COLOR_FORMAT_ARGB8565;
        }
        else if (input_type == ARGB8888)
        {
            header->cf = LV_COLOR_FORMAT_ARGB8888;
        }
        if(header->cf <= LV_COLOR_FORMAT_UNKNOWN) {
            LV_LOG_ERROR("Invalid color format: %d", header->cf);
            return LV_RESULT_INVALID;
        }
        
        LV_LOG_USER("IDU decoder info: %dx%d, cf: %d, stride: %d", 
                    header->w, header->h, header->cf, header->stride);
        
        return LV_RESULT_OK;

    }
    return LV_RESULT_INVALID;
}

static lv_result_t idu_decoder_open(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc)
{
    LV_UNUSED(decoder);
    if (dsc->src_type == LV_IMAGE_SRC_FILE)
    {
        const char *fn = dsc->src;
        LV_LOG_ERROR("Failed to LV_IMAGE_SRC_FILE");
        return LV_RESULT_INVALID;
        if (strcmp(lv_fs_get_ext(fn), "rle") == 0)               /*Check the extension*/
        {
            uint8_t *rle_data;
            lv_fs_file_t f;
            lv_fs_res_t res = lv_fs_open(&f, fn, LV_FS_MODE_RD);
            if (res != LV_FS_RES_OK) { return LV_RESULT_INVALID; }
            uint8_t headers[20];

            // lv_fs_ioctl(&f, (void **)&rle_data, 0);
            memcpy(headers, rle_data, 20);
            uint8_t idu_type = headers[8] & 0x03;// Lowest 2 bits
            uint16_t width = dsc->header.w;
            uint16_t height = dsc->header.h;
            lv_fs_close(&f);

            LV_ASSERT(idu_type == 0); //source RLE

            dsc->decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, width, height, dsc->header.cf, LV_STRIDE_AUTO);
            if(dsc->decoded == NULL) {
                LV_LOG_ERROR("Failed to create draw buffer");
                return;  // Handle error appropriately
            }

            uint8_t *img_data = (uint8_t *)dsc->decoded->data;
            idu_file_t *file = (idu_file_t *)(rle_data + 8);

            lv_result_t ret;
#ifdef LV_USE_GPU_RTK_PPE
            ret = hw_acc_idu_decode(rle_data, img_data, width, height);

            if (ret == LV_RESULT_OK)
            {
                LV_LOG_INFO("Hardware decode succeeded.");
                return LV_RESULT_OK;
            }
#endif
            char input_type = headers[1];
            ret = decompress_rle_data(input_type, file, img_data, width, height);
            if (ret != LV_RESULT_OK) {
                lv_draw_buf_destroy(dsc->decoded);
                LV_LOG_ERROR("Decompression failed for input type: %c", input_type);
                return ret;
            }

            LV_LOG_INFO("Software decode success.");
            return LV_RESULT_OK;     /*The image is fully decoded. Return with its pointer*/
        }

    }
    else if (dsc->src_type == LV_IMAGE_SRC_VARIABLE)
    {
        const lv_image_dsc_t *image = (const lv_image_dsc_t *)dsc->src;
        if(image == NULL || image->data == NULL) {
            LV_LOG_ERROR("Invalid source descriptor or data");
            return LV_RESULT_INVALID;
        }
        LV_ASSERT(image != NULL);
        if(image->header.w == 0 || image->header.h == 0) {
            LV_LOG_ERROR("Invalid image dimensions");
            return LV_RESULT_INVALID;
        }
        const uint8_t *data = image->data;
        uint16_t width = dsc->header.w;
        uint16_t height = dsc->header.h;

        uint8_t raw_bytes_per_pixel = data[8] >> 6;
        uint8_t idu_type = data[8] & 0x03;
        uint8_t bytes_per_pixel = get_bytes_per_pixel(raw_bytes_per_pixel);

        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image->data;
        char input_type = head->type;

        const uint8_t *rle_header = data + 8;
        idu_file_t *file = (idu_file_t *)rle_header;

        LV_ASSERT(idu_type == 0); //source RLE

        uint32_t required_size = height * image->header.stride;
        LV_LOG_USER("Required memory: %u bytes", required_size);
        
        if (required_size == 0 || required_size > (10 * 1024 * 1024)) { 
            LV_LOG_ERROR("Invalid image size: %ux%u, required %u bytes", width, height, required_size);
            return LV_RESULT_INVALID;
        }
        
        dsc->decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, width, height, dsc->header.cf, LV_STRIDE_AUTO);
        if(dsc->decoded == NULL) {
            LV_LOG_ERROR("Failed to create draw buffer");
            return;  // Handle error appropriately
        }

        uint8_t *img_data = (uint8_t *)dsc->decoded->data;

        lv_result_t ret;

#ifdef LV_USE_GPU_RTK_PPE
        ret = hw_acc_idu_decode(data, img_data, width, height);

        if (ret != LV_RESULT_OK) {
            lv_draw_buf_destroy(dsc->decoded);
            LV_LOG_ERROR("HW decompression failed ");
            return ret;
        }
#endif
        ret = decompress_rle_data(input_type, file, img_data, width, height);
        if (ret != LV_RESULT_OK) {
            lv_draw_buf_destroy(dsc->decoded);
            LV_LOG_ERROR("Decompression failed for input type: %c", input_type);
            return ret;
        }

        LV_LOG_USER("Software decode success.");
        return LV_RESULT_OK; 
    }
    return LV_RESULT_INVALID;    /*If not returned earlier then it failed*/

}

void idu_decoder_close(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc)
{
    LV_UNUSED(decoder);
    if(dsc->decoded) {
        if(!lv_image_cache_is_enabled() || (dsc->cache == NULL && dsc->cache_entry == NULL)) {
            lv_draw_buf_destroy(dsc->decoded);
        }
        dsc->decoded = NULL;
    }
    LV_LOG_INFO("Closed IDU image");
}
#if LV_USE_GPU_RTK_PPE
static lv_result_t hw_acc_idu_decode(const uint8_t *image, uint8_t *output, uint16_t width,
                                  uint16_t height)
{
    if (image == NULL || output == NULL)
    {
        LV_ASSERT(image != NULL && output != NULL);
        return LV_RESULT_INVALID;
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
        return LV_RESULT_OK;
    }
    else
    {
        return LV_RESULT_INVALID;
    }
}
#endif
lv_result_t decompress_rle_data(char input_type, idu_file_t *file, uint8_t *img_data, uint16_t width, uint16_t height) {
    lv_result_t ret;

    switch (input_type) {
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
            LV_LOG_ERROR("Unsupported input type: %c", input_type);
            return LV_RESULT_INVALID;
    }

    return ret;
}
lv_result_t decompress_rle_rgb565_data(const idu_file_t *file, uint8_t *img_data,
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

    return LV_RESULT_OK;
}
lv_result_t decompress_rle_argb8565_data(const idu_file_t *file, uint8_t *img_data,
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

    return LV_RESULT_OK;
}


lv_result_t decompress_rle_rgb888_data(const idu_file_t *file, uint8_t *img_data,
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
    return LV_RESULT_OK;
}

lv_result_t decompress_rle_argb8888_data(const idu_file_t *file, uint8_t *img_data,
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
    return LV_RESULT_OK;
}

#endif /*LV_USE_RTK_IDU*/
