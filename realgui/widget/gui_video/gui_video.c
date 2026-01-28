/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_video.h"
#include "acc_api.h"
#include <math.h>
#include "gui_h264bsd.h"
#include "gui_vfs.h"
#include "acc_sw_jpeg.h"
#include "gui_api.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/

#define MGPEG_SEG_SOI  (0xD8FF)
#define MGPEG_SEG_EOI  (0xD9FF)

#define SOF0  0xC0  // Start Of Frame
#define SOF2  0xC2  // Start Of Frame (Progressive JPEG)
#define SOS   0xDA  // Start Of Scan
#define EOI   0xD9  // End Of Image

#define YUV_SAMPLE_444 0x11
#define YUV_SAMPLE_422 0x21
#define YUV_SAMPLE_420 0x22
#define YUV_SAMPLE_400 0x01
// #define YUV_SAMPLE_224 0x12
/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static bool is_mjpeg(uint8_t *pdata)
{
    return ((*((uint16_t *)(pdata)) == MGPEG_SEG_SOI));
}
static bool is_h264(uint8_t *pdata)
{
#define VIDEO_H264_SYMBOL "H264"
    char symbol[16];
    memset((void *)symbol, 0, sizeof(symbol));
    memcpy((void *)symbol, pdata, strlen(VIDEO_H264_SYMBOL));
    return (strcmp(symbol, VIDEO_H264_SYMBOL) == 0);
}
static bool is_avi(uint8_t *pdata)
{
#define VIDEO_AVI_SYMBOL_0 "RIFF"
#define VIDEO_AVI_SYMBOL_1 "AVI "
    char symbol[5];

    /* RIFF Header */
    memset((void *)symbol, 0, sizeof(symbol));
    memcpy((void *)symbol, pdata, strlen(VIDEO_AVI_SYMBOL_0));

    if (memcmp(symbol, VIDEO_AVI_SYMBOL_0, 4))
    {
        return false;
    }
    /* Format */
    memset((void *)symbol, 0, sizeof(symbol));
    memcpy((void *)symbol, pdata + 4 + 4, strlen(VIDEO_AVI_SYMBOL_1));

    return (memcmp(symbol, VIDEO_AVI_SYMBOL_1, 4) == 0);
}

static int get_jpeg_size(const unsigned char *jpeg_data, size_t jpeg_size, uint16_t *width,
                         uint16_t *height)
{
    size_t i = 0;

    if (jpeg_size < 2 || jpeg_data[0] != 0xFF || jpeg_data[1] != 0xD8)
    {
        return -1; // Not a valid JPEG
    }

    i = 2; // Skip SOI marker

    while (i < jpeg_size)
    {
        if (jpeg_data[i] != 0xFF)
        {
            return -1; // Not a proper JPEG marker
        }

        unsigned char marker = jpeg_data[i + 1];
        i += 2;

        if (marker == SOF0)
        {
            i += 3; // Skip length and precision
            *height = (jpeg_data[i] << 8) + jpeg_data[i + 1];
            *width = (jpeg_data[i + 2] << 8) + jpeg_data[i + 3];
            uint8_t comp = jpeg_data[i + 4];
            uint8_t subsample = jpeg_data[i + 6];
            gui_log("w %d h %d comp %d subsample 0x%x\n", *width, *height, comp, subsample);

            if (gui_get_acc()->jpeg_load)
            {
                if (comp == 4 || ((subsample != YUV_SAMPLE_420) && (subsample != YUV_SAMPLE_422) &&
                                  (subsample != YUV_SAMPLE_444) && (subsample != YUV_SAMPLE_400)))
                {
                    gui_log("unknown sample! %d 0x%x", comp, subsample);
                    return -1;
                }
                else
                {
                    if (subsample == YUV_SAMPLE_420 || subsample == YUV_SAMPLE_422)
                    {
                        *width = ((*width + 15) >> 4) << 4;
                    }
                    else
                    {
                        *width = ((*width + 7) >> 3) << 3;
                    }
                    if (subsample == YUV_SAMPLE_420)
                    {
                        *height = ((*height + 15) >> 4) << 4;
                    }
                    else
                    {
                        *height = ((*height + 7) >> 3) << 3;
                    }
                }

                gui_log("align w %d h %d ", *width, *height);
            }
            return 0; // Success
        }
        else if (marker == SOS || marker == EOI)
        {
            break; // Unexpected end
        }
        else
        {
            // Skip the current segment
            size_t length = (jpeg_data[i] << 8) + jpeg_data[i + 1];
            i += length;
        }
    }

    return -1; // Failed to find dimensions
}

static uint8_t **mjpeg2jpeg_slicer(uint8_t *mjpeg, uint32_t *slice_cnt)
{
    uint8_t *pdata = mjpeg;
    uint8_t *pstrt = mjpeg;
    uint8_t *pend = mjpeg;
    uint8_t **array = NULL;
    *slice_cnt = 0;
    uint8_t scan_state = 0;  // 0: init, 1: soi, 2: eoi

    if ((*((uint16_t *)pdata) != 0xd8ff))
    {
        return NULL;
    }

    while (1)
    {
        if (*((uint16_t *)pdata) == MGPEG_SEG_SOI)
        {
            scan_state = 1;
            pstrt = pdata;
        }
        else if ((scan_state == 1) && (*((uint16_t *)pdata) == MGPEG_SEG_EOI))
        {
            scan_state = 2;
            (*slice_cnt)++;
            // TODO: fix realloc
            if (!array)
            {
                // gui_log("%s %d\n", __FUNCTION__, __LINE__);
                array = (uint8_t **)gui_malloc(2 * sizeof(uint8_t *));
            }
            else
            {
                // gui_log("%s %d\n", __FUNCTION__, __LINE__);
                array = (uint8_t **)gui_realloc(array, (*slice_cnt + 1) * sizeof(uint8_t *));
            }
            if (!array)
            {
                gui_log("array malloc error!\n");
                return NULL;
            }
            pend = pdata + 2; // eoi
            array[*slice_cnt - 1] = pstrt;
            array[*slice_cnt] = pend;
            scan_state = 0;
            gui_log("jpg: %d 0x%x\n", *slice_cnt - 1, array[*slice_cnt - 1]);
        }
        else if ((scan_state == 0) && (pend + 8 < pdata)) // aligned to 8
        {
            gui_log("mjpeg EOF\n");
            break;
        }
        pdata++;
    }
    return array;
}


static void gui_video_reset(gui_video_t *this)
{
    this->frame_cur = -1;
    this->frame_last = -1;
    this->frame_buff = 0;
}

static void gui_video_play_cb(void *p)
{
    gui_video_t *this = (gui_video_t *)p;

    if (this->state == GUI_VIDEO_STATE_PLAYING)
    {
        this->frame_cur += this->frame_step;
        if ((uint32_t)this->frame_cur >= this->num_frame)
        {
            if (this->repeat_cnt > 0)
            {
                this->repeat_cnt --;
                this->frame_cur %= this->num_frame;
            }
            else if (this->repeat_cnt == 0)
            {
                this->frame_cur = this->num_frame - 1; // stop at the last frame
                this->state = GUI_VIDEO_STATE_STOP;
            }
            else
            {
                this->frame_cur %= this->num_frame;
            }
        }

    }



    // gui_log("Cur frame %d \n", this->frame_cur);
    if (this->frame_cur != this->frame_last)
    {
        gui_fb_change();
    }

}


static void gui_video_draw(gui_obj_t *obj)
{
    gui_video_t *this = (gui_video_t *)obj;
    uint8_t *img_data_raw = NULL;
    uint8_t *img_data = NULL;
    uint32_t img_sz = 0;
    uint8_t *frame_buff = NULL;

    // gui_log("gui_video_draw  %d %d", this->frame_cur, this->frame_last);
    this->frame_cur = (this->frame_cur < 0) ? 0 : this->frame_cur;
    if (this->frame_cur == this->frame_last && this->frame_buff)
    {
        // gui_log("draw from cache %d 0x%x\n", this->frame_cur, this->frame_buff);
        gui_img_set_image_data(this->img, this->frame_buff);
        gui_img_refresh_draw_data(this->img);
        return;
    }
    else
    {
        if (this->frame_buff_raw)
        {
            if (this->img_type == VIDEO_TYPE_MJPEG || this->img_type == VIDEO_TYPE_AVI)
            {
                gui_acc_jpeg_free(this->frame_buff_raw);
            }
            else if (this->img_type == VIDEO_TYPE_H264)
            {
                gui_free(this->frame_buff_raw);
            }
        }
        this->frame_buff = NULL;
        this->frame_buff_raw = NULL;
    }


    if (this->img_type == VIDEO_TYPE_MJPEG)
    {
        if (this->storage_type == IMG_SRC_FILESYS)
        {
            uint32_t *array = (uint32_t *)this->array;
            img_sz = array[this->frame_cur + 1] - array[this->frame_cur];
            // gui_log(" %d  %d, %d, sz %d \n", this->frame_cur, array[this->frame_cur + 1], array[this->frame_cur], img_sz);
            img_data_raw = (uint8_t *)gui_malloc(img_sz + 8);
            if (!img_data_raw)
            {
                gui_log("gui_video_draw malloc error! \n");
                return ;
            }
            img_data = (uint8_t *)((((uintptr_t)img_data_raw + 7) >> 3) << 3);

            const char *fn = this->data;
            gui_vfs_file_t *fp;

            fp = gui_vfs_open(fn, GUI_VFS_READ);
            if (fp != NULL)
            {
                gui_vfs_seek(fp, array[this->frame_cur], GUI_VFS_SEEK_SET);
                gui_vfs_read(fp, img_data, img_sz);
                gui_vfs_close(fp);
            }
            else
            {
                gui_log("gui_video_draw open error! \n");
            }


            if (*(uint16_t *)img_data != 0xd8ff || *(uint16_t *)(img_data + img_sz - 2) != 0xd9ff)
            {
                gui_log("gui_video_draw load error! \n");
            }

        }
        else if (this->storage_type == IMG_SRC_MEMADDR)
        {
            img_data = this->array[this->frame_cur];
            img_sz = this->array[this->frame_cur + 1] - this->array[this->frame_cur];
        }
        else if (this->storage_type == IMG_SRC_FTL)
        {
            uint32_t *array = (uint32_t *)this->array;
            img_sz = array[this->frame_cur + 1] - array[this->frame_cur];
            // gui_log(" %d  %d, %d, sz %d \n", this->frame_cur, array[this->frame_cur + 1], array[this->frame_cur], img_sz);
            img_data_raw = (uint8_t *)gui_malloc(img_sz + 8);
            if (!img_data_raw)
            {
                gui_log("gui_video_draw malloc error! \n");
                return ;
            }
            img_data = (uint8_t *)((((uintptr_t)img_data_raw + 7) >> 3) << 3);

            uint32_t fp = (uintptr_t)this->data;

            fp += array[this->frame_cur];
            gui_ftl_read(fp, img_data, img_sz);

            if (*(uint16_t *)img_data != 0xd8ff || *(uint16_t *)(img_data + img_sz - 2) != 0xd9ff)
            {
                gui_log("gui_video_draw load error! \n");
            }
        }
        // gui_log("%d off 0x%x, %d\n", this->frame_cur, img_data - (uint8_t *)this->data, img_sz);

        int w = 0;
        int h = 0;
        int channel = 0;
        frame_buff = gui_acc_jpeg_load(img_data, img_sz, &w, &h, &channel);
        if (!frame_buff)
        {
            gui_log("decode video file failed");
            return ;
        }

        this->frame_buff = frame_buff;
        this->frame_buff_raw = frame_buff;

        gui_rgb_data_head_t *pheader = (gui_rgb_data_head_t *)this->frame_buff;
        pheader->type = this->header.type;
        pheader->jpeg = false;

        // gui_log("this frame_buff  0x%x\n", this->frame_buff);

        if (this->storage_type == IMG_SRC_FILESYS || this->storage_type == IMG_SRC_FTL)
        {
            if (img_data_raw)
            {
                gui_free(img_data_raw);
            }
        }

    }
    else if (this->img_type == VIDEO_TYPE_AVI)
    {
        if (this->storage_type == IMG_SRC_FILESYS)
        {
            const char *fn = this->data;
            gui_vfs_file_t *fp;

            fp = gui_vfs_open(fn, GUI_VFS_READ);
            if (!fp)
            {
                gui_log("gui_video_draw open error! \n");
                return;
            }


            int32_t frame_cnt = -1;
            for (uint32_t i = 0; i < this->chunk_num; i++)
            {
                AviMoviChunk_t *chunk = (AviMoviChunk_t *)this->array;
                uint32_t chunk_sz = chunk[i].len;

                if (chunk[i].type == CHUNK_VIDEO)
                {
                    frame_cnt++;
                    if (frame_cnt == this->frame_cur)
                    {
                        // video frame
                        if (chunk_sz)
                        {
                            this->frame_chunk_cur = i;
                        }
                        img_sz = chunk[this->frame_chunk_cur].len;
                        // gui_log(" %d  %d, sz %d \n", this->frame_chunk_cur, chunk[this->frame_chunk_cur].data, img_sz);

                        img_data_raw = (uint8_t *)gui_malloc(img_sz + 8);
                        if (!img_data_raw)
                        {
                            gui_log("gui_video_draw malloc error! \n");
                            gui_vfs_close(fp);
                            return ;
                        }
                        img_data = (uint8_t *)((((uintptr_t)img_data_raw + 7) >> 3) << 3);


                        gui_vfs_seek(fp, (uintptr_t)chunk[this->frame_chunk_cur].offset, GUI_VFS_SEEK_SET);
                        gui_vfs_read(fp, img_data, img_sz);

                        if ((*(uint16_t *)img_data != 0xd8ff || *(uint16_t *)(img_data + img_sz - 2) != 0xd9ff))
                        {
                            gui_log("gui_video_draw load error! \n");
                            gui_vfs_close(fp);
                            return;
                        }
                    }
                    else if (frame_cnt > this->frame_cur)
                    {
                        break;
                    }
                }
                else if (chunk[i].type == CHUNK_AUDIO)
                {
                    if (this->frame_cur == 0 || (frame_cnt == this->frame_cur))
                    {
                        // to audio
                        // gui_log("audio chunk %d \n", i);
                        uint8_t *audio_data = NULL;
                        uint32_t audio_sz = 0;
                        audio_sz = chunk[i].len;
                        // gui_log(" %d  %d, sz %d \n", i, chunk[i].offset, audio_sz);

                        audio_data = (uint8_t *)gui_malloc(audio_sz);
                        if (!audio_data)
                        {
                            gui_log("gui_video_draw malloc error! \n");
                            gui_vfs_close(fp);
                            return ;
                        }
                        gui_vfs_seek(fp, (uintptr_t)chunk[i].offset, GUI_VFS_SEEK_SET);
                        gui_vfs_read(fp, audio_data, audio_sz);

                        gui_audio_decode_chunk(audio_data, audio_sz);
                        gui_free(audio_data);
                    }
                }
                else
                {
                    gui_log("avi: unknown chunk type\n");
                    gui_vfs_close(fp);
                    return;
                }
            }

            gui_vfs_close(fp);

        }
        else if (this->storage_type == IMG_SRC_MEMADDR)
        {
            int32_t frame_cnt = -1;
            for (uint32_t i = 0; i < this->chunk_num; i++)
            {
                AviMoviChunk_t *chunk = (AviMoviChunk_t *)this->array;
                uint32_t chunk_sz = chunk[i].len;

                if (chunk[i].type == CHUNK_VIDEO)
                {
                    frame_cnt++;
                    if (frame_cnt == this->frame_cur)
                    {
                        // video frame
                        if (chunk_sz)
                        {
                            this->frame_chunk_cur = i;
                        }
                        img_sz = chunk[this->frame_chunk_cur].len;
                        // gui_log(" %d  %d, sz %d \n", this->frame_chunk_cur, chunk[this->frame_chunk_cur].offset, img_sz);

                        img_data = (uint8_t *)this->data + chunk[this->frame_chunk_cur].offset;
                    }
                    else if (frame_cnt > this->frame_cur)
                    {
                        break;
                    }
                }
                else if (chunk[i].type == CHUNK_AUDIO)
                {
                    if (this->frame_cur == 0 || (frame_cnt == this->frame_cur))
                    {
                        // to audio
                        // gui_log("audio chunk %d \n", i);
                        uint8_t *audio_data = NULL;
                        uint32_t audio_sz = 0;
                        audio_sz = chunk[i].len;
                        // gui_log(" %d  %d, sz %d \n", i, chunk[i].offset, audio_sz);

                        audio_data = (uint8_t *)this->data + chunk[i].offset;

                        gui_audio_decode_chunk(audio_data, audio_sz);
                    }
                }
                else
                {
                    gui_log("avi: unknown chunk type\n");
                    return;
                }
            }

        }
        else if (this->storage_type == IMG_SRC_FTL)
        {
            uint32_t fp = (uintptr_t)this->data;

            int32_t frame_cnt = -1;
            for (uint32_t i = 0; i < this->chunk_num; i++)
            {
                AviMoviChunk_t *chunk = (AviMoviChunk_t *)this->array;
                uint32_t chunk_sz = chunk[i].len;

                if (chunk[i].type == CHUNK_VIDEO)
                {
                    frame_cnt++;
                    if (frame_cnt == this->frame_cur)
                    {
                        // video frame
                        if (chunk_sz)
                        {
                            this->frame_chunk_cur = i;
                        }
                        img_sz = chunk[this->frame_chunk_cur].len;
                        // gui_log(" %d  %d, sz %d \n", this->frame_chunk_cur, chunk[this->frame_chunk_cur].offset, img_sz);

                        img_data_raw = (uint8_t *)gui_malloc(img_sz + 8);
                        if (!img_data_raw)
                        {
                            gui_log("gui_video_draw malloc error! \n");
                            return ;
                        }
                        img_data = (uint8_t *)((((uintptr_t)img_data_raw + 7) >> 3) << 3);

                        gui_ftl_read(fp + chunk[this->frame_chunk_cur].offset, img_data, img_sz);

                        if ((*(uint16_t *)img_data != 0xd8ff || *(uint16_t *)(img_data + img_sz - 2) != 0xd9ff))
                        {
                            gui_log("gui_video_draw load error! \n");
                            return;
                        }
                    }
                    else if (frame_cnt > this->frame_cur)
                    {
                        break;
                    }
                }
                else if (chunk[i].type == CHUNK_AUDIO)
                {
                    if (this->frame_cur == 0 || (frame_cnt == this->frame_cur))
                    {
                        // to audio
                        // to audio
                        // gui_log("audio chunk %d \n", i);
                        uint8_t *audio_data = NULL;
                        uint32_t audio_sz = 0;
                        audio_sz = chunk[i].len;
                        // gui_log(" %d  %d, sz %d \n", i, chunk[i].offset, audio_sz);

                        audio_data = (uint8_t *)gui_malloc(audio_sz);
                        if (!audio_data)
                        {
                            gui_log("gui_video_draw malloc error! \n");
                            return ;
                        }
                        gui_ftl_read(fp + chunk[i].offset, audio_data, audio_sz);

                        gui_audio_decode_chunk(audio_data, audio_sz);
                        gui_free(audio_data);
                    }
                }
                else
                {
                    gui_log("avi: unknown chunk type\n");
                    return;
                }
            }

        }
        // gui_log("%d off 0x%x, %d\n", this->frame_cur, img_data - (uint8_t *)this->data, img_sz);

        int w = 0;
        int h = 0;
        int channel = 0;
        frame_buff = gui_acc_jpeg_load(img_data, img_sz, &w, &h, &channel);
        if (!frame_buff)
        {
            gui_log("decode video file failed");
            return ;
        }

        this->frame_buff = frame_buff;
        this->frame_buff_raw = frame_buff;

        gui_rgb_data_head_t *pheader = (gui_rgb_data_head_t *)this->frame_buff;
        pheader->type = this->header.type;
        pheader->jpeg = false;

        // gui_log("this frame_buff  0x%x\n", this->frame_buff);

        if (this->storage_type == IMG_SRC_FILESYS || this->storage_type == IMG_SRC_FTL)
        {
            if (img_data_raw)
            {
                gui_free(img_data_raw);
            }
        }
    }
    else if (this->img_type == VIDEO_TYPE_H264)
    {
        if (this->storage_type == IMG_SRC_FILESYS)
        {
            const char *fn = this->data;
            gui_vfs_file_t *fp;

            fp = gui_vfs_open(fn, GUI_VFS_READ);
            if (!fp)
            {
                gui_log("gui_video_draw open error! \n");
                return;
            }
            gui_h264_header_t temp;
            gui_h264_header_t *header = &temp;

            gui_vfs_read(fp, header, sizeof(gui_h264_header_t));

            if (!this->decoder)
            {
                this->decoder = gui_h264bsd_create_decoder((uint8_t *)this->array, header->size);
            }
            if (this->frame_cur == 0)
            {
                // reload
                gui_vfs_read(fp, (void *)this->array, header->size);
                gui_h264bsd_rewind(this->decoder);
            }

            this->frame_buff_raw = gui_malloc(sizeof(gui_rgb_data_head_t) + header->w * header->h * 2);

            if (!this->frame_buff_raw)
            {
                gui_log("gui_video_draw malloc h264 fb error! \n");
            }
            this->frame_buff = this->frame_buff_raw;
            // gui_log("dec frame %d\n", this->frame_cur);
            int ret = gui_h264bsd_get_frame(this->decoder, this->frame_buff + sizeof(gui_rgb_data_head_t),
                                            header->w * header->h * 2);
            if (ret)
            {
                gui_log("h264 dec ret: %d\n", ret);
            }

            gui_rgb_data_head_t *pheader = (gui_rgb_data_head_t *)this->frame_buff;
            memcpy(pheader, &(this->header), sizeof(gui_rgb_data_head_t));

            gui_vfs_close(fp);
        }
        else if (this->storage_type == IMG_SRC_MEMADDR)
        {
            gui_h264_header_t *header = (gui_h264_header_t *)this->data;

            if (!this->decoder)
            {
                this->decoder = gui_h264bsd_create_decoder((uint8_t *)this->array, header->size);
            }
            if (this->frame_cur == 0)
            {
                // reload
                memcpy((void *)this->array, (void *)((uint8_t *)this->data + sizeof(gui_h264_header_t)),
                       header->size);
                gui_h264bsd_rewind(this->decoder);
            }

            this->frame_buff_raw = gui_malloc(sizeof(gui_rgb_data_head_t) + header->w * header->h * 2);

            if (!this->frame_buff_raw)
            {
                gui_log("gui_video_draw malloc h264 fb error! \n");
            }
            this->frame_buff = this->frame_buff_raw;
            // gui_log("dec frame %d\n", this->frame_cur);
            int ret = gui_h264bsd_get_frame(this->decoder, this->frame_buff + sizeof(gui_rgb_data_head_t),
                                            header->w * header->h * 2);
            if (ret)
            {
                gui_log("h264 dec ret: %d\n", ret);
            }

            gui_rgb_data_head_t *pheader = (gui_rgb_data_head_t *)this->frame_buff;
            memcpy(pheader, &(this->header), sizeof(gui_rgb_data_head_t));
        }
        else if (this->storage_type == IMG_SRC_FTL)
        {
            uint32_t fp = (uintptr_t)this->data;

            gui_h264_header_t temp;
            gui_h264_header_t *header = &temp;

            gui_ftl_read(fp, (uint8_t *)header, sizeof(gui_h264_header_t));

            if (!this->decoder)
            {
                this->decoder = gui_h264bsd_create_decoder((uint8_t *)this->array, header->size);
            }
            if (this->frame_cur == 0)
            {
                // reload
                gui_ftl_read(fp + sizeof(gui_h264_header_t), (uint8_t *)this->array, header->size);
                gui_h264bsd_rewind(this->decoder);
            }

            this->frame_buff_raw = gui_malloc(sizeof(gui_rgb_data_head_t) + header->w * header->h * 2);

            if (!this->frame_buff_raw)
            {
                gui_log("gui_video_draw malloc h264 fb error! \n");
            }
            this->frame_buff = this->frame_buff_raw;
            // gui_log("dec frame %d\n", this->frame_cur);
            int ret = gui_h264bsd_get_frame(this->decoder, this->frame_buff + sizeof(gui_rgb_data_head_t),
                                            header->w * header->h * 2);
            if (ret)
            {
                gui_log("h264 dec ret: %d\n", ret);
            }

            gui_rgb_data_head_t *pheader = (gui_rgb_data_head_t *)this->frame_buff;
            memcpy(pheader, &(this->header), sizeof(gui_rgb_data_head_t));
        }
    }

    gui_img_set_image_data(this->img, this->frame_buff);
    gui_img_refresh_draw_data(this->img);
    this->frame_last = this->frame_cur;
}

static void gui_video_draw_end(gui_obj_t *obj)
{
    (void)obj;
    // gui_log("gui_video_draw_end \n");
    // not free fb here
}

static void gui_video_destory(gui_obj_t *obj)
{
    gui_video_t *this = (gui_video_t *)obj;

    if (this->storage_type == IMG_SRC_FILESYS)
    {

    }
    else
    {

    }

    gui_free(this->array);
    if (this->frame_buff_raw)
    {
        if (this->img_type == VIDEO_TYPE_MJPEG)
        {
            gui_acc_jpeg_free(this->frame_buff_raw);
        }
        else if (this->img_type == VIDEO_TYPE_H264)
        {

        }

        this->frame_buff_raw = NULL;
        this->frame_buff = NULL;
    }

    if (this->decoder)
    {
        if (this->img_type == VIDEO_TYPE_H264)
        {
            gui_h264bsd_destroy_decoder(this->decoder);
        }
    }
}

static void gui_video_prepare(gui_obj_t *obj)
{
    gui_video_t *this = (gui_video_t *)obj;

    // gui_log("gui_video_prepare  %d %d", this->frame_cur, this->frame_last);
    gui_img_set_image_data(this->img, (const uint8_t *) & (this->header));
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED);
}


static int video_src_init_avi(gui_video_t  *this)
{
    if (this->storage_type == IMG_SRC_FILESYS)
    {
        gui_obj_t *obj = (gui_obj_t *)this;
        uint32_t file_size = 0;

        const char *fn = this->data;
        gui_vfs_file_t *fp;

        fp = gui_vfs_open(fn, GUI_VFS_READ);
        if (fp > 0)
        {
            file_size = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_END); // end
        }
        if (file_size <= 0)
        {
            return -1;
        }

        // uint32_t pos = 0;
        uint8_t sigver[5];

        uint32_t list_size = 0;
        uint32_t chunk_size = 0;

        uint8_t list_type[5];
        uint8_t id[5];

        uint32_t hdrl_size = 0;
        int hdrl_data_beacon = 0;
        uint32_t stream_list_size = 0;
        int stream_list_beacon = 0;
        uint32_t movi_size = 0;
        int movi_data_beacon = 0;
        uint32_t idx1_size = 0;

        memset((void *)sigver, 0, sizeof(sigver));
        memset((void *)list_type, 0, sizeof(list_type));
        memset((void *)id, 0, sizeof(id));


        gui_vfs_seek(fp, 12, GUI_VFS_SEEK_SET); // start + 12B
        /* LIST - hdrl */
        gui_vfs_read(fp, sigver, 4);
        if (memcmp(sigver, "LIST", 4) != 0)
        {
            gui_log("invalid format");
            gui_vfs_close(fp);
            return -1;
        }
        gui_vfs_read(fp, &hdrl_size, 4);
        hdrl_data_beacon = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_CUR); // current

        gui_vfs_read(fp, list_type, 4);
        if (memcmp(list_type, "hdrl", 4) != 0)
        {
            gui_log("invalid list type %s", list_type);
            gui_vfs_close(fp);
            return -1;
        }
        /* AVI main header */
        gui_vfs_read(fp, id, 4);
        if (memcmp(id, "avih", 4) != 0)
        {
            gui_log("invalid header %s", id);
            gui_vfs_close(fp);
            return -1;
        }
        gui_vfs_read(fp, &chunk_size, 4);
        MainAVIHeader_t main_hdr;
        gui_vfs_read(fp, &main_hdr, sizeof(MainAVIHeader_t));
        gui_vfs_seek(fp, chunk_size - sizeof(MainAVIHeader_t), GUI_VFS_SEEK_CUR);


        /*************   Video  ********************/
        /* LIST - strl */
        gui_vfs_read(fp, sigver, 4);
        if (memcmp(sigver, "LIST", 4) != 0)
        {
            gui_log("invalid format");
            gui_vfs_close(fp);
            return -1;
        }
        gui_vfs_read(fp, &stream_list_size, 4);
        stream_list_beacon = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_CUR); // current

        gui_vfs_read(fp, list_type, 4);
        if (memcmp(list_type, "strl", 4) != 0)
        {
            gui_log("invalid list type %s", list_type);
            gui_vfs_close(fp);
            return -1;
        }

        /* Stream header */
        gui_vfs_read(fp, id, 4);
        if (memcmp(id, "strh", 4) != 0)
        {
            gui_log("invalid header %s", id);
            gui_vfs_close(fp);
            return -1;
        }
        gui_vfs_read(fp, &chunk_size, 4);

        AVIStreamHeader_t video_stream_hdr;
        gui_vfs_read(fp, &video_stream_hdr, sizeof(AVIStreamHeader_t));

        /* Stream format */
        memcpy(id, &video_stream_hdr.stream_format, 4);
        if (memcmp(id, "strf", 4) != 0)
        {
            gui_log("invalid header %s", id);
            gui_vfs_close(fp);
            return -1;
        }
        chunk_size = video_stream_hdr.length_format;

        BitMapInfoHeader_t stream_format;
        gui_vfs_read(fp, &stream_format, sizeof(BitMapInfoHeader_t));
        if (memcmp((void *)&stream_format.compression, "MJPG", 4) != 0)
        {
            gui_log("invalid compression %s", (char *)&stream_format.compression);
            gui_vfs_close(fp);
            return -1;
        }


        do
        {
            /* Try audio stream list */
            gui_vfs_seek(fp, stream_list_beacon, GUI_VFS_SEEK_SET);
            gui_vfs_seek(fp, stream_list_size, GUI_VFS_SEEK_CUR);
            /*************   Audio  ********************/
            /* LIST - strl */
            gui_vfs_read(fp, sigver, 4);
            if (memcmp(sigver, "LIST", 4) != 0)
            {
                gui_log("Audio: invalid format %s", sigver);
                break;
            }
            gui_vfs_read(fp, &list_size, 4);
            gui_vfs_read(fp, list_type, 4);
            if (memcmp(list_type, "strl", 4) != 0)
            {
                gui_log("invalid list type %s", list_type);
                break;
            }
            /* Stream header */
            gui_vfs_read(fp, id, 4);
            if (memcmp(id, "strh", 4) != 0)
            {
                gui_log("invalid header %s", id);
                break;
            }
            gui_vfs_read(fp, &chunk_size, 4);

            AVIStreamHeader_t audio_stream_hdr;
            gui_vfs_read(fp, &audio_stream_hdr, sizeof(AVIStreamHeader_t));
            if (memcmp((void *)audio_stream_hdr.type, "auds", 4) != 0)
            {
                gui_log("Not audio list %s", (char *)audio_stream_hdr.type);
                break;
            }

            /* Stream format */
            memcpy(id, &audio_stream_hdr.stream_format, 4);
            if (memcmp(id, "strf", 4) != 0)
            {
                gui_log("invalid header %s", id);
                break;
            }
            chunk_size = audio_stream_hdr.length_format;

            WaveFormateX_t audio_stream_format;
            gui_vfs_read(fp, &audio_stream_format, sizeof(WaveFormateX_t));

            // ad_audio_open(&audio_stream_hdr, &audio_stream_format);
        }
        while (0);


        /* Find movi list */
        uint32_t pos = gui_vfs_seek(fp, hdrl_data_beacon, GUI_VFS_SEEK_SET);
        pos = gui_vfs_seek(fp, hdrl_size, GUI_VFS_SEEK_CUR);
        while (pos < file_size)
        {
            uint32_t size = 0;
            gui_vfs_read(fp, sigver, 4);
            gui_vfs_read(fp, &size, 4);
            if (memcmp(sigver, "JUNK", 4) == 0)
            {
                pos = gui_vfs_seek(fp, size, GUI_VFS_SEEK_CUR);
            }
            else if (memcmp(sigver, "LIST", 4) == 0)
            {
                gui_vfs_read(fp, list_type, 4);
                if (memcmp(list_type, "movi", 4) != 0)
                {
                    pos = gui_vfs_seek(fp, size - 4, GUI_VFS_SEEK_CUR);
                }
                else
                {
                    pos = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_CUR);
                    movi_size = size;
                    movi_data_beacon = pos - 4;
                    break;
                }
            }
            pos = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_CUR);
        }
        if (pos >= file_size)
        {
            gui_log("movi not found");
            gui_vfs_close(fp);
            return -1;
        }

        pos = gui_vfs_seek(fp, movi_size - 4, GUI_VFS_SEEK_CUR);
        /* idx1 */
        gui_vfs_read(fp, sigver, 4);
        if (memcmp(sigver, "idx1", 4) != 0)
        {
            gui_log("invalid format");
            gui_vfs_close(fp);
            return -1;
        }
        gui_vfs_read(fp, &idx1_size, 4);

        obj->w  = main_hdr.width;
        obj->h = main_hdr.height;
        this->frame_time = main_hdr.usec_per_frame / 1000;
        this->num_frame = main_hdr.total_frame;
        gui_log("video w %d, h %d, frame time %d, frame %d", main_hdr.width, main_hdr.height,
                this->frame_time, this->num_frame);
        /*  slicing  */
        uint32_t chunk_num = idx1_size / sizeof(IndexItem_t);
        this->array = (uint8_t **)gui_malloc(chunk_num * sizeof(AviMoviChunk_t));
        this->chunk_num = chunk_num;

        uint32_t frame_cnt = 0;
        for (uint32_t i = 0; i < chunk_num; i++)
        {
            IndexItem_t IndexItem;
            gui_vfs_read(fp, &IndexItem, sizeof(IndexItem_t));
            AviMoviChunk_t chunk;

            chunk.offset = (movi_data_beacon + IndexItem.offset + 8);
            chunk.len = IndexItem.size;
            chunk.type = (IndexItem.chunk_ID == 0x63643030) ? CHUNK_VIDEO : (IndexItem.chunk_ID == 0x62773130) ?
                         CHUNK_AUDIO : CHUNK_UNKNOWN;

            memcpy((AviMoviChunk_t *)(this->array) + i, &chunk, sizeof(AviMoviChunk_t));
            if (IndexItem.chunk_ID == 0x63643030) // "00dc"
            {
                gui_log("V frame %d, offset 0x%x, size %d\n", frame_cnt, IndexItem.offset, IndexItem.size);
                frame_cnt++;
            }
            else if (IndexItem.chunk_ID == 0x62773130) // "01wb"  30 31 77 62
            {
                gui_log("A         , offset 0x%x, size %d\n",  IndexItem.offset, IndexItem.size);
            }
        }
        gui_vfs_close(fp);
    }
    else if (this->storage_type == IMG_SRC_MEMADDR)
    {
        uint8_t *img_data = (uint8_t *)this->data;
        gui_obj_t *obj = (gui_obj_t *)this;

        uint8_t sigver[5];

        uint32_t list_size = 0;
        uint32_t chunk_size = 0;

        uint8_t list_type[5];
        uint8_t id[5];

        uint32_t hdrl_size = 0;
        int hdrl_data_beacon = 0;
        uint32_t stream_list_size = 0;
        int stream_list_beacon = 0;
        uint32_t movi_size = 0;
        int movi_data_beacon = 0;
        uint32_t idx1_size = 0;


        uint32_t file_size;
        uint32_t pos = 0;

        memset((void *)sigver, 0, sizeof(sigver));
        memset((void *)list_type, 0, sizeof(list_type));
        memset((void *)id, 0, sizeof(id));

        pos = 4;
        file_size = *(uint32_t *)(img_data + pos) + 8; // + RIFF + 4B size
        pos += 4;
        /* Format AVI */
        pos += 4;


        /* LIST - hdrl */
        memcpy(sigver, (img_data + pos), 4);
        if (memcmp(sigver, "LIST", 4) != 0)
        {
            gui_log("invalid format");
            return -1;
        }
        pos += 4;
        memcpy(&hdrl_size, (img_data + pos), 4);
        pos += 4;
        hdrl_data_beacon = pos; // current

        memcpy(list_type, (img_data + pos), 4);
        if (memcmp(list_type, "hdrl", 4) != 0)
        {
            gui_log("invalid list type %s", list_type);
            return -1;
        }
        pos += 4;
        /* AVI main header */
        memcpy(id, (img_data + pos), 4);
        if (memcmp(id, "avih", 4) != 0)
        {
            gui_log("invalid header %s", id);
            return -1;
        }
        pos += 4;
        memcpy(&chunk_size, (img_data + pos), 4);
        pos += 4;
        MainAVIHeader_t main_hdr;
        memcpy(&main_hdr, (img_data + pos), sizeof(MainAVIHeader_t));
        pos += chunk_size;



        /*************   Video  ********************/
        /* LIST - strl */
        memcpy(sigver, (img_data + pos), 4);
        if (memcmp(sigver, "LIST", 4) != 0)
        {
            gui_log("invalid format");
            return -1;
        }
        pos += 4;
        memcpy(&stream_list_size, (img_data + pos), 4);
        pos += 4;
        stream_list_beacon = pos; // current

        memcpy(list_type, (img_data + pos), 4);
        if (memcmp(list_type, "strl", 4) != 0)
        {
            gui_log("invalid list type %s", list_type);
            return -1;
        }
        pos += 4;

        /* Stream header */
        memcpy(id, (img_data + pos), 4);
        if (memcmp(id, "strh", 4) != 0)
        {
            gui_log("invalid header %s", id);
            return -1;
        }
        pos += 4;
        memcpy(&chunk_size, (img_data + pos), 4);
        pos += 4;

        AVIStreamHeader_t video_stream_hdr;
        memcpy(&video_stream_hdr, (img_data + pos), sizeof(AVIStreamHeader_t));
        pos += sizeof(AVIStreamHeader_t);

        /* Stream format */
        memcpy(id, &video_stream_hdr.stream_format, 4);
        if (memcmp(id, "strf", 4) != 0)
        {
            gui_log("invalid header %s", id);
            return -1;
        }
        chunk_size = video_stream_hdr.length_format;

        BitMapInfoHeader_t stream_format;
        memcpy(&stream_format, (img_data + pos), sizeof(BitMapInfoHeader_t));
        if (memcmp((void *)&stream_format.compression, "MJPG", 4) != 0)
        {
            gui_log("invalid compression %s", (char *)&stream_format.compression);
            return -1;
        }
        pos += sizeof(BitMapInfoHeader_t);


        do
        {
            /* Try audio stream list */
            pos = stream_list_beacon + stream_list_size;
            /*************   Audio  ********************/
            /* LIST - strl */
            memcpy(sigver, (img_data + pos), 4);
            if (memcmp(sigver, "LIST", 4) != 0)
            {
                gui_log("Audio: invalid format %s", sigver);
                break;
            }
            pos += 4;
            memcpy(&list_size, (img_data + pos), 4);
            pos += 4;
            memcpy(list_type, (img_data + pos), 4);
            if (memcmp(list_type, "strl", 4) != 0)
            {
                gui_log("invalid list type %s", list_type);
                break;
            }
            pos += 4;
            /* Stream header */
            memcpy(id, (img_data + pos), 4);
            if (memcmp(id, "strh", 4) != 0)
            {
                gui_log("invalid header %s", id);
                break;
            }
            pos += 4;
            memcpy(&chunk_size, (img_data + pos), 4);
            pos += 4;

            AVIStreamHeader_t audio_stream_hdr;
            memcpy(&audio_stream_hdr, (img_data + pos), sizeof(AVIStreamHeader_t));
            if (memcmp((void *)audio_stream_hdr.type, "auds", 4) != 0)
            {
                gui_log("Not audio list %s", (char *)audio_stream_hdr.type);
                break;
            }
            pos += sizeof(AVIStreamHeader_t);

            /* Stream format */
            memcpy(id, &audio_stream_hdr.stream_format, 4);
            if (memcmp(id, "strf", 4) != 0)
            {
                gui_log("invalid header %s", id);
                break;
            }
            chunk_size = audio_stream_hdr.length_format;

            WaveFormateX_t audio_stream_format;
            memcpy(&audio_stream_format, (img_data + pos), sizeof(WaveFormateX_t));

            /* maybe some audio decoder need these info, process here */
            // to audio decoder
        }
        while (0);


        /* Find movi list */
        pos = hdrl_data_beacon + hdrl_size;
        while (pos < file_size)
        {
            uint32_t size = 0;
            memcpy(sigver, (img_data + pos), 4);
            pos += 4;
            memcpy(&size, (img_data + pos), 4);
            pos += 4;
            if (memcmp(sigver, "JUNK", 4) == 0)
            {
                pos += size;
            }
            else if (memcmp(sigver, "LIST", 4) == 0)
            {
                memcpy(list_type, (img_data + pos), 4);
                pos += 4;
                if (memcmp(list_type, "movi", 4) != 0)
                {
                    pos += size - 4;
                }
                else
                {
                    movi_size = size;
                    movi_data_beacon = pos - 4;
                    break;
                }
            }
        }
        if (pos >= file_size)
        {
            gui_log("movi not found");
            return -1;
        }

        pos += movi_size - 4;
        /* idx1 */
        memcpy(sigver, (img_data + pos), 4);
        pos += 4;
        if (memcmp(sigver, "idx1", 4) != 0)
        {
            gui_log("invalid format");
            return -1;
        }
        memcpy(&idx1_size, (img_data + pos), 4);
        pos += 4;


        /* construct index */
        obj->w  = main_hdr.width;
        obj->h = main_hdr.height;
        this->frame_time = main_hdr.usec_per_frame / 1000;
        this->num_frame = main_hdr.total_frame;
        gui_log("video w %d, h %d, frame time %d, frame %d", main_hdr.width, main_hdr.height,
                this->frame_time, this->num_frame);
        /*  slicing  */
        uint32_t chunk_num = idx1_size / sizeof(IndexItem_t);
        this->array = (uint8_t **)gui_malloc(chunk_num * sizeof(AviMoviChunk_t));
        this->chunk_num = chunk_num;

        uint32_t frame_cnt = 0;
        for (uint32_t i = 0; i < chunk_num; i++)
        {
            IndexItem_t IndexItem;
            memcpy(&IndexItem, (img_data + pos), sizeof(IndexItem_t));
            pos += sizeof(IndexItem_t);
            AviMoviChunk_t chunk;

            chunk.offset = (movi_data_beacon + IndexItem.offset + 8);
            chunk.len = IndexItem.size;
            chunk.type = (IndexItem.chunk_ID == 0x63643030) ? CHUNK_VIDEO : (IndexItem.chunk_ID == 0x62773130) ?
                         CHUNK_AUDIO : CHUNK_UNKNOWN;

            memcpy((AviMoviChunk_t *)(this->array) + i, &chunk, sizeof(AviMoviChunk_t));
            if (IndexItem.chunk_ID == 0x63643030) // "00dc"
            {
                gui_log("V frame %d, offset 0x%x, size %d\n", frame_cnt, IndexItem.offset, IndexItem.size);
                frame_cnt++;
            }
            else if (IndexItem.chunk_ID == 0x62773130) // "01wb"  30 31 77 62
            {
                gui_log("A         , offset 0x%x, size %d\n",  IndexItem.offset, IndexItem.size);
            }
        }
    }
    else if (this->storage_type == IMG_SRC_FTL)
    {
        uint32_t fp = (uintptr_t)this->data;
        gui_obj_t *obj = (gui_obj_t *)this;
        uint32_t file_size = 0;

        uint32_t pos = 0;
        uint8_t sigver[5];

        uint32_t list_size = 0;
        uint32_t chunk_size = 0;

        uint8_t list_type[5];
        uint8_t id[5];

        uint32_t hdrl_size = 0;
        int hdrl_data_beacon = 0;
        uint32_t stream_list_size = 0;
        int stream_list_beacon = 0;
        uint32_t movi_size = 0;
        int movi_data_beacon = 0;
        uint32_t idx1_size = 0;

#define AVI_FTL_READ(afp, det, sz)  {gui_ftl_read(((uintptr_t)afp+pos), (uint8_t*)det, sz); pos+=sz;}
        memset((void *)sigver, 0, sizeof(sigver));
        memset((void *)list_type, 0, sizeof(list_type));
        memset((void *)id, 0, sizeof(id));


        pos = 4;
        AVI_FTL_READ(fp, &file_size, 4);
        file_size += 8; // + RIFF + 4B size
        /* Format AVI */
        pos += 4;

        /* LIST - hdrl */
        AVI_FTL_READ(fp, sigver, 4);
        if (memcmp(sigver, "LIST", 4) != 0)
        {
            gui_log("invalid format");
            return -1;
        }
        AVI_FTL_READ(fp, &hdrl_size, 4);
        hdrl_data_beacon = pos; // current

        AVI_FTL_READ(fp, list_type, 4);
        if (memcmp(list_type, "hdrl", 4) != 0)
        {
            gui_log("invalid list type %s", list_type);
            return -1;
        }
        /* AVI main header */
        AVI_FTL_READ(fp, id, 4);
        if (memcmp(id, "avih", 4) != 0)
        {
            gui_log("invalid header %s", id);
            return -1;
        }
        AVI_FTL_READ(fp, &chunk_size, 4);
        MainAVIHeader_t main_hdr;
        AVI_FTL_READ(fp, &main_hdr, sizeof(MainAVIHeader_t));
        pos += chunk_size - sizeof(MainAVIHeader_t);

        /*************   Video  ********************/
        /* LIST - strl */
        AVI_FTL_READ(fp, sigver, 4);
        if (memcmp(sigver, "LIST", 4) != 0)
        {
            gui_log("invalid format");
            return -1;
        }
        AVI_FTL_READ(fp, &stream_list_size, 4);
        stream_list_beacon = pos; // current

        AVI_FTL_READ(fp, list_type, 4);
        if (memcmp(list_type, "strl", 4) != 0)
        {
            gui_log("invalid list type %s", list_type);
            return -1;
        }

        /* Stream header */
        AVI_FTL_READ(fp, id, 4);
        if (memcmp(id, "strh", 4) != 0)
        {
            gui_log("invalid header %s", id);
            return -1;
        }
        AVI_FTL_READ(fp, &chunk_size, 4);

        AVIStreamHeader_t video_stream_hdr;
        AVI_FTL_READ(fp, &video_stream_hdr, sizeof(AVIStreamHeader_t));

        /* Stream format */
        memcpy(id, &video_stream_hdr.stream_format, 4);
        if (memcmp(id, "strf", 4) != 0)
        {
            gui_log("invalid header %s", id);
            return -1;
        }
        chunk_size = video_stream_hdr.length_format;

        BitMapInfoHeader_t stream_format;
        AVI_FTL_READ(fp, &stream_format, sizeof(BitMapInfoHeader_t));
        if (memcmp((void *)&stream_format.compression, "MJPG", 4) != 0)
        {
            gui_log("invalid compression %s", (char *)&stream_format.compression);
            return -1;
        }


        do
        {
            /* Try audio stream list */
            pos = stream_list_beacon + stream_list_size;
            /*************   Audio  ********************/
            /* LIST - strl */
            AVI_FTL_READ(fp, sigver, 4);
            if (memcmp(sigver, "LIST", 4) != 0)
            {
                gui_log("Audio: invalid format %s", sigver);
                break;
            }
            AVI_FTL_READ(fp, &list_size, 4);
            AVI_FTL_READ(fp, list_type, 4);
            if (memcmp(list_type, "strl", 4) != 0)
            {
                gui_log("invalid list type %s", list_type);
                break;
            }
            /* Stream header */
            AVI_FTL_READ(fp, id, 4);
            if (memcmp(id, "strh", 4) != 0)
            {
                gui_log("invalid header %s", id);
                break;
            }
            AVI_FTL_READ(fp, &chunk_size, 4);

            AVIStreamHeader_t audio_stream_hdr;
            AVI_FTL_READ(fp, &audio_stream_hdr, sizeof(AVIStreamHeader_t));
            if (memcmp((void *)audio_stream_hdr.type, "auds", 4) != 0)
            {
                gui_log("Not audio list %s", (char *)audio_stream_hdr.type);
                break;
            }

            /* Stream format */
            memcpy(id, &audio_stream_hdr.stream_format, 4);
            if (memcmp(id, "strf", 4) != 0)
            {
                gui_log("invalid header %s", id);
                break;
            }
            chunk_size = audio_stream_hdr.length_format;

            WaveFormateX_t audio_stream_format;
            AVI_FTL_READ(fp, &audio_stream_format, sizeof(WaveFormateX_t));

            // ad_audio_open(&audio_stream_hdr, &audio_stream_format);
        }
        while (0);


        /* Find movi list */
        pos = hdrl_data_beacon + hdrl_size;
        while (pos < file_size)
        {
            uint32_t size = 0;
            AVI_FTL_READ(fp, sigver, 4);
            AVI_FTL_READ(fp, &size, 4);
            if (memcmp(sigver, "JUNK", 4) == 0)
            {
                pos += size;
            }
            else if (memcmp(sigver, "LIST", 4) == 0)
            {
                AVI_FTL_READ(fp, list_type, 4);
                if (memcmp(list_type, "movi", 4) != 0)
                {
                    pos += size - 4;
                }
                else
                {
                    movi_size = size;
                    movi_data_beacon = pos - 4;
                    break;
                }
            }
        }
        if (pos >= file_size)
        {
            gui_log("movi not found");
            return -1;
        }

        pos += movi_size - 4;
        /* idx1 */
        AVI_FTL_READ(fp, sigver, 4);
        if (memcmp(sigver, "idx1", 4) != 0)
        {
            gui_log("invalid format");
            return -1;
        }
        AVI_FTL_READ(fp, &idx1_size, 4);


        obj->w  = main_hdr.width;
        obj->h = main_hdr.height;
        this->frame_time = main_hdr.usec_per_frame / 1000;
        this->num_frame = main_hdr.total_frame;
        gui_log("video w %d, h %d, frame time %d, frame %d", main_hdr.width, main_hdr.height,
                this->frame_time, this->num_frame);
        /*  slicing  */
        uint32_t chunk_num = idx1_size / sizeof(IndexItem_t);
        this->array = (uint8_t **)gui_malloc(chunk_num * sizeof(AviMoviChunk_t));
        this->chunk_num = chunk_num;

        uint32_t frame_cnt = 0;
        for (uint32_t i = 0; i < chunk_num; i++)
        {
            IndexItem_t IndexItem;
            AVI_FTL_READ(fp, &IndexItem, sizeof(IndexItem_t));
            AviMoviChunk_t chunk;

            chunk.offset = (movi_data_beacon + IndexItem.offset + 8);
            chunk.len = IndexItem.size;
            chunk.type = (IndexItem.chunk_ID == 0x63643030) ? CHUNK_VIDEO : (IndexItem.chunk_ID == 0x62773130) ?
                         CHUNK_AUDIO : CHUNK_UNKNOWN;

            memcpy((AviMoviChunk_t *)(this->array) + i, &chunk, sizeof(AviMoviChunk_t));
            if (IndexItem.chunk_ID == 0x63643030) // "00dc"
            {
                gui_log("V frame %d, offset 0x%x, size %d\n", frame_cnt, IndexItem.offset, IndexItem.size);
                frame_cnt++;
            }
            else if (IndexItem.chunk_ID == 0x62773130) // "01wb"  30 31 77 62
            {
                gui_log("A         , offset 0x%x, size %d\n",  IndexItem.offset, IndexItem.size);
            }
        }
    }
    return 0;
}

// load into mem
static int video_src_init_h264(gui_video_t  *this)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_h264_header_t *header = NULL;

    if (this->storage_type == IMG_SRC_FILESYS)
    {
        gui_h264_header_t temp;
        header = &temp;
        const char *fn = this->data;
        gui_vfs_file_t *fp;

        fp = gui_vfs_open(fn, GUI_VFS_READ);
        if (fp <= 0)
        {
            return -1;
        }
        gui_vfs_read(fp, header, sizeof(gui_h264_header_t));

        obj->w = header->w;
        obj->h = header->h;
        this->num_frame = header->frame_num;
        this->frame_time = header->frame_time;
        this->array = (uint8_t **)gui_malloc(header->size);

        if (!this->array)
        {
            gui_log("load h264 failed, malloc size %d\n", header->size);
            return -1;
        }
        // load video to mem
        gui_vfs_read(fp, (void *)this->array, header->size);
        gui_log("h264 w %d h %d size %d n %d frame_time %d\n", obj->w, obj->h, header->size,
                this->num_frame,
                this->frame_time);
        gui_vfs_close(fp);
    }
    else if (this->storage_type == IMG_SRC_MEMADDR)
    {
        header = this->data;
        obj->w = header->w;
        obj->h = header->h;
        this->num_frame = header->frame_num;
        this->frame_time = header->frame_time;
        this->array = (uint8_t **)gui_malloc(header->size);

        if (!this->array)
        {
            gui_log("load h264 failed, malloc size %d\n", header->size);
            return -1;
        }
        // load video to mem
        memcpy((void *)this->array, (void *)((uint8_t *)this->data + sizeof(gui_h264_header_t)),
               header->size);
        gui_log("h264 w %d h %d size %d n %d frame_time %d\n", obj->w, obj->h, header->size,
                this->num_frame,
                this->frame_time);
    }
    else if (this->storage_type == IMG_SRC_FTL)
    {
        gui_h264_header_t temp;
        header = &temp;
        uint32_t fp = (uintptr_t)this->data;

        gui_ftl_read(fp, (uint8_t *)header, sizeof(gui_h264_header_t));

        obj->w = header->w;
        obj->h = header->h;
        this->num_frame = header->frame_num;
        this->frame_time = header->frame_time;
        this->array = (uint8_t **)gui_malloc(header->size);

        if (!this->array)
        {
            gui_log("load h264 failed, malloc size %d\n", header->size);
            return -1;
        }
        // load video to mem
        gui_ftl_read(fp + sizeof(gui_h264_header_t), (uint8_t *)this->array, header->size);
        gui_log("h264 w %d h %d size %d n %d frame_time %d\n", obj->w, obj->h, header->size,
                this->num_frame,
                this->frame_time);
    }
    return 0;
}

static int video_src_init_mjpg(gui_video_t  *this)
{
#define VIDEO_INIT_LOAD_BUFF_SZ  (2*1024)
    uint8_t **slice_array = NULL;
    uint32_t slice_cnt = 0;
    uint32_t sz_file = 0;


    if (this->storage_type == IMG_SRC_FILESYS)
    {
        uint32_t *array = NULL;
        const char *fn = this->data;
        gui_vfs_file_t *fp;
        uint8_t *buff = NULL;

        fp = gui_vfs_open(fn, GUI_VFS_READ);
        if (fp != NULL)
        {
            sz_file = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_END); // end
        }
        if (sz_file <= 0)
        {
            return -1;
        }
        buff = (uint8_t *)gui_malloc(VIDEO_INIT_LOAD_BUFF_SZ);
        if (!buff)
        {
            gui_log("mjpeg2jpeg_slicer malloc error! \n");
            return -1;
        }

        uint32_t cur_start = 0;
        uint32_t cur_rd = 0;
        gui_obj_t *obj = (gui_obj_t *)this;
        uint8_t scan_state = MJPEG_SCAN_INIT;
        uint32_t offset = 0;
        while (scan_state != MJPEG_SCAN_EOF)
        {
            cur_start += cur_rd ;
            cur_rd = VIDEO_INIT_LOAD_BUFF_SZ;
            gui_vfs_seek(fp, cur_start, GUI_VFS_SEEK_SET);
            gui_vfs_read(fp, buff, cur_rd);
            if (cur_start < cur_rd)
            {
                // first time to get size
                uint16_t w = 0, h = 0;
                get_jpeg_size(buff, cur_rd, &w, &h);
                obj->w = w;
                obj->h = h;
            }

            uint8_t *pdata = buff;
            while (pdata < buff + cur_rd - 1)
            {
                if (*((uint16_t *)pdata) == MGPEG_SEG_SOI)
                {
                    scan_state = MJPEG_SCAN_START;
                    offset = pdata - buff + cur_start;
                }
                else if ((scan_state == MJPEG_SCAN_START) && (*((uint16_t *)pdata) == MGPEG_SEG_EOI))
                {
                    scan_state = MJPEG_SCAN_END;
                    slice_cnt++;
                    if (!array)
                    {
                        gui_log("%s %d\n", __FUNCTION__, __LINE__);
                        array = (uint32_t *)gui_malloc(2 * sizeof(uint32_t));
                    }
                    else
                    {
                        uint32_t *temp = NULL;
                        // gui_log("%s %d cnt %d array 0x%x\n", __FUNCTION__, __LINE__, slice_cnt, array);
                        temp = (uint32_t *)gui_malloc((slice_cnt + 1) * sizeof(uint32_t));
                        memcpy(temp, array, slice_cnt  * sizeof(uint32_t));
                        gui_free(array);
                        array = temp;
                    }

                    if (!array)
                    {
                        gui_log("array malloc error!\n");
                        gui_free(buff);
                        gui_vfs_close(fp);
                        return -1;
                    }
                    array[slice_cnt - 1] = offset;
                    array[slice_cnt] = pdata + 2 - buff + cur_start;
                    gui_log("%d: %d -> %d ,sz %d\n", slice_cnt - 1, array[slice_cnt - 1], array[slice_cnt],
                            array[slice_cnt] - array[slice_cnt - 1]);
                    pdata++; // +1 +1 -> EOI
                    // scan_state = MJPEG_SCAN_INIT;
                }
                else if ((scan_state == MJPEG_SCAN_END) && (*((uint16_t *)pdata) != MGPEG_SEG_SOI))
                {
                    // case: 0xffd9 | 0xffd8 will get into the first "if"
                    gui_log("mjpeg EOF\n");
                    scan_state = MJPEG_SCAN_EOF;
                }
                else if (scan_state == MJPEG_SCAN_EOF)
                {
                    // case: 0xffd9 , 0xXXXX  ... |  should break
                    break;
                }
                pdata++;
            }
            memset(buff, 0, cur_rd);
        }
        gui_free(buff);
        gui_vfs_close(fp);
        this->array = (uint8_t **)array;
        this->num_frame = slice_cnt;
    }
    else if (this->storage_type == IMG_SRC_MEMADDR)
    {
        uint8_t *img_data = (uint8_t *)this->data;
        gui_obj_t *obj = (gui_obj_t *)this;

        uint16_t w = 0, h = 0;
        get_jpeg_size(img_data, 2 * 1024, &w, &h);
        obj->w = w;
        obj->h = h;
        slice_array = mjpeg2jpeg_slicer(img_data, &slice_cnt);
        if (!slice_array)
        {
            gui_log("mjpeg2jpeg_slicer error! \n");
        }

        this->array = slice_array;
        this->num_frame = slice_cnt;
    }
    else if (this->storage_type == IMG_SRC_FTL)
    {
        uint32_t *array = NULL;
        uint32_t fn = (uintptr_t)this->data;
        uint32_t fp = 0;
        uint8_t *buff = NULL;

        buff = (uint8_t *)gui_malloc(VIDEO_INIT_LOAD_BUFF_SZ);
        if (!buff)
        {
            gui_log("mjpeg2jpeg_slicer malloc error! \n");
            return -1;
        }

        uint32_t cur_start = 0;
        uint32_t cur_rd = 0;
        gui_obj_t *obj = (gui_obj_t *)this;
        uint8_t scan_state = MJPEG_SCAN_INIT;
        uint32_t offset = 0;
        while (scan_state != MJPEG_SCAN_EOF)
        {
            // no need overlap reading, cus already aligned and padded inside
            // cur_start += (cur_rd ? (cur_rd - 0) : 0);
            cur_start += cur_rd ;
            cur_rd = VIDEO_INIT_LOAD_BUFF_SZ;
            fp = fn + cur_start;
            gui_ftl_read(fp, buff, cur_rd);
            if (cur_start < cur_rd)
            {
                // first time to get size
                uint16_t w = 0, h = 0;
                get_jpeg_size(buff, cur_rd, &w, &h);
                obj->w = w;
                obj->h = h;
            }

            uint8_t *pdata = buff;
            while (pdata < buff + cur_rd - 1)
            {
                if (*((uint16_t *)pdata) == MGPEG_SEG_SOI)
                {
                    scan_state = MJPEG_SCAN_START;
                    offset = pdata - buff + cur_start;
                }
                else if ((scan_state == MJPEG_SCAN_START) && (*((uint16_t *)pdata) == MGPEG_SEG_EOI))
                {
                    scan_state = MJPEG_SCAN_END;
                    slice_cnt++;
                    if (!array)
                    {
                        gui_log("%s %d\n", __FUNCTION__, __LINE__);
                        array = (uint32_t *)gui_malloc(2 * sizeof(uint32_t));
                    }
                    else
                    {
                        uint32_t *temp = NULL;
                        // gui_log("%s %d cnt %d array 0x%x\n", __FUNCTION__, __LINE__, slice_cnt, array);
                        temp = (uint32_t *)gui_malloc((slice_cnt + 1) * sizeof(uint32_t));
                        memcpy(temp, array, slice_cnt  * sizeof(uint32_t));
                        gui_free(array);
                        array = temp;
                    }

                    if (!array)
                    {
                        gui_log("array malloc error!\n");
                        gui_free(buff);
                        return -1;
                    }
                    array[slice_cnt - 1] = offset;
                    array[slice_cnt] = pdata + 2 - buff + cur_start;
                    gui_log("%d: %d -> %d ,sz %d\n", slice_cnt - 1, array[slice_cnt - 1], array[slice_cnt],
                            array[slice_cnt] - array[slice_cnt - 1]);
                    pdata++; // +1 +1 -> EOI
                    // scan_state = MJPEG_SCAN_INIT;
                }
                else if ((scan_state == MJPEG_SCAN_END) && (*((uint16_t *)pdata) != MGPEG_SEG_SOI))
                {
                    // case: 0xffd9 | 0xffd8 will get into the first "if"
                    gui_log("mjpeg EOF\n");
                    scan_state = MJPEG_SCAN_EOF;
                }
                else if (scan_state == MJPEG_SCAN_EOF)
                {
                    // case: 0xffd9 , 0xXXXX  ... |  should break
                    break;
                }
                pdata++;
            }
            memset(buff, 0, cur_rd);
        }
        gui_free(buff);
        this->array = (uint8_t **)array;
        this->num_frame = slice_cnt;
    }
    return 0;
}

int gui_video_get_filesize_from_addr(void *addr, uint8_t storage_type)
{
    if (addr == NULL || storage_type == IMG_SRC_FILESYS)
    {
        return -1;
    }

    int res = -1;
    uint8_t header[32];

    memset(header, 0, sizeof(header));
    if (storage_type == IMG_SRC_MEMADDR)
    {
        memcpy(header, addr, sizeof(header));
    }
    else if (storage_type == IMG_SRC_FTL)
    {
        gui_ftl_read((uintptr_t)addr, (uint8_t *)header, sizeof(header));
    }


    if (is_mjpeg(header))
    {
        if (storage_type == IMG_SRC_MEMADDR)
        {
            uint16_t *op = (uint16_t *)addr;
            while (1)
            {
                if (*op == 0xD9FF)
                {
                    op++;
                    if (*op != 0xD8FF)
                    {
                        return ((uintptr_t)op - (uintptr_t)addr);
                    }
                }
                op++;
            }
        }
        else if (storage_type == IMG_SRC_FTL)
        {
            uint8_t *rbuff = gui_malloc(2 * 1024);
            uint32_t offset = 0;
            uint8_t flg_eoi = false;
            if (!rbuff)
            {
                gui_log("video read buff malloc failed\n");
                return -1;
            }
            memset(rbuff, 0, 2 * 1024);
            while (1)
            {
                gui_ftl_read((uintptr_t)((uint8_t *)addr + offset), rbuff, 2 * 1024);
                uint16_t *op = (uint16_t *)rbuff;
                while ((uint8_t *)op - (uint8_t *)rbuff < 2 * 1024)
                {
                    if (*op == 0xD9FF)
                    {
                        // static uint16_t frame = 0;
                        // gui_log("Frame %d", frame++);
                        flg_eoi = true;
                    }
                    else if (flg_eoi)
                    {
                        flg_eoi = false;
                        if (*op != 0xD8FF)
                        {
                            uint32_t sz = (uint8_t *)op - (uint8_t *)rbuff + offset;
                            // gui_log("EOF: %d\n", sz);
                            gui_free(rbuff);
                            return (sz);
                        }
                    }
                    op++;
                }
                offset += 2 * 1024;
            }
        }
    }
    else if (is_h264(header))
    {
        gui_h264_header_t *h264_header = (gui_h264_header_t *)header;
        return h264_header->size;
    }
    else if (is_avi(header))
    {
        gui_riff_header_t *avi_header = (gui_riff_header_t *)header;
        return avi_header->size + 8;
    }
    else
    {
        res = -1;
        gui_log("video src type err!\n");
    }

    return res;
}

static int gui_video_src_init(gui_video_t  *this)
{
    if (this == NULL)
    {
        return -1;
    }

    int res = -1;
    uint8_t header[16];

    memset(header, 0, sizeof(header));
    if (this->storage_type == IMG_SRC_FILESYS)
    {
        char *file = this->data;
        gui_vfs_file_t *fp = gui_vfs_open(file, GUI_VFS_READ);
        int rdlen = 0;
        GUI_UNUSED(rdlen);
        if (fp == NULL)
        {
            gui_log("Error: open file failed\n");
            return -1;
        }
        gui_vfs_seek(fp, 0, GUI_VFS_SEEK_SET);

        rdlen = gui_vfs_read(fp, header, sizeof(header));
        gui_vfs_close(fp);
    }
    else if (this->storage_type == IMG_SRC_MEMADDR)
    {
        memcpy(header, this->data, sizeof(header));
    }
    else if (this->storage_type == IMG_SRC_FTL)
    {
        gui_ftl_read((uintptr_t)this->data, (uint8_t *)header, sizeof(header));
    }


    if (is_mjpeg(header))
    {
        this->img_type = VIDEO_TYPE_MJPEG;
        res = video_src_init_mjpg(this);
    }
    else if (is_h264(header))
    {
        this->img_type = VIDEO_TYPE_H264;
        res = video_src_init_h264(this);
    }
    else if (is_avi(header))
    {
        this->img_type = VIDEO_TYPE_AVI;
        res = video_src_init_avi(this);
    }
    else
    {
        res = -1;
        gui_log("video src type err!\n");
    }

    return res;
}
static void gui_video_input_prepare(gui_obj_t *obj)
{
    (void)obj;
    // gui_log("gui_video_input_prepare \n");
}

static void gui_video_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_video_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_video_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_video_draw(obj);
            break;
        case OBJ_END:
            gui_video_draw_end(obj);
            break;

        case OBJ_DESTROY:
            gui_video_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_img_video_ctor(gui_video_t  *this,
                               gui_obj_t      *parent,
                               const char     *name,
                               void           *addr,
                               int16_t         x,
                               int16_t         y,
                               int16_t         w,
                               int16_t         h)
{
    if (this == NULL)
    {
        return;
    }

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    // uint32_t live_time = 0;
    // gui_rgb_data_head_t *img_header = NULL;

    gui_obj_ctor(root, parent, name, x, y, w, h);
    root->type = IMAGE_FROM_MEM;
    root->obj_cb = gui_video_cb;
    root->has_input_prepare_cb = true;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    //for self
    this->frame_time = 42; // 24fps, 42 ms
    this->frame_step = 1;
    this->frame_cur = -1;
    this->frame_last = -1;
    this->data = addr;


    // get img data from file, and img type
    if (gui_video_src_init(this) < 0)
    {
        gui_log("video src err\n");
    }
    // prepare a temp header for image(child widget)
    if (this->img_type == VIDEO_TYPE_MJPEG || this->img_type == VIDEO_TYPE_AVI)
    {
        memset(&(this->header), 0, sizeof(gui_rgb_data_head_t));
        this->header.w = root->w;
        this->header.h = root->h;
        if (gui_get_acc()->jpeg_load)
        {
            this->header.type = 0x00; // RGB565
        }
        else
        {
            this->header.type = 0x03; // RGB888
        }
    }
    else if (this->img_type == VIDEO_TYPE_H264)
    {
        memset(&(this->header), 0, sizeof(gui_rgb_data_head_t));
        this->header.w = root->w;
        this->header.h = root->h;
        this->header.type = 0x00; // RGB565
    }
    else
    {
        gui_log("video unknown RGB type\n");
    }


    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    //
    this->img = gui_img_create_from_mem(this, "VIDEO",
                                        (void *)&this->header,
                                        0, 0, w, h);
    gui_img_set_mode(this->img, IMG_BYPASS_MODE);
    gui_img_set_image_data(this->img, (const uint8_t *) & (this->header));
    gui_img_refresh_size(this->img);
    gui_obj_create_timer((gui_obj_t *)this, this->frame_time, true, gui_video_play_cb);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_video_set_frame_rate(gui_video_t *this, float fps)
{
    if (!this) { return; }

    if (fps > 0)
    {
        this->frame_time = round(1000.f / fps);
        gui_obj_create_timer((gui_obj_t *)this, this->frame_time, true, gui_video_play_cb);
    }
}

uint32_t gui_video_get_frame_time(gui_video_t *this)
{
    if (!this) { return 0; }

    return this->frame_time;
}

void gui_video_set_scale(gui_video_t *this, float scale_x, float scale_y)
{
    if (!this) { return; }

    gui_img_scale(this->img, scale_x, scale_y);
}

void gui_video_set_state(gui_video_t *this, GUI_VIDEO_STATE state)
{
    if (!this) { return; }
    // handle stop -> play reset
    if (this->state == GUI_VIDEO_STATE_STOP && state == GUI_VIDEO_STATE_PLAYING)
    {
        gui_video_reset(this);
    }
    this->state = state;

    if (this->state == GUI_VIDEO_STATE_INIT)
    {
        gui_video_reset(this);
    }
}

void gui_video_set_repeat_count(gui_video_t *this, int32_t cnt)
{
    if (!this) { return; }

    this->repeat_cnt = cnt;
}

GUI_VIDEO_STATE gui_video_get_state(gui_video_t *this)
{
    if (!this) { return GUI_VIDEO_STATE_ERR; }

    return (GUI_VIDEO_STATE)this->state;
}

void gui_video_set_frame_step(gui_video_t *this, uint32_t step)
{
    if (!this) { return; }

    this->frame_step = step;
}

uint32_t gui_video_get_frame_step(gui_video_t *this)
{
    if (!this) { return 0; }

    return this->frame_step;
}

void gui_video_refresh_size(gui_video_t *this)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    uint8_t *img_data = NULL;
    uint16_t w = 0, h = 0;

    switch (this->storage_type)
    {
    case IMG_SRC_MEMADDR:
        {
            img_data = this->data;
        }
        break;
    // case IMG_SRC_FILESYS:
    // {

    // }
    // break;
    // case IMG_SRC_FTL:
    // {

    // }
    // break;
    default:
        {
            gui_log("live img src not available!");
            return;
        }
    }
    if (img_data != NULL)
    {
        if (this->img_type == VIDEO_TYPE_MJPEG)
        {
            get_jpeg_size(img_data, 1024, &w, &h);
        }
        obj->w = w;
        obj->h = h;
    }
}

void gui_video_refresh_type(gui_video_t *this)
{
    uint8_t *img_data = NULL;

    this->img_type = VIDEO_TYPE_NULL;
    switch (this->storage_type)
    {
    case IMG_SRC_MEMADDR:
        {
            img_data = this->data;
        }
        break;
    // case IMG_SRC_FILESYS:
    // {

    // }
    // break;
    // case IMG_SRC_FTL:
    // {

    // }
    // break;
    default:
        {
            gui_log("live img src not available!");
            return;
        }
    }

    if (img_data != NULL)
    {
        do
        {
            if (is_mjpeg(img_data))
            {
                this->img_type = VIDEO_TYPE_MJPEG;
                gui_log("live: MJPEG");
                break;
            }

            gui_log("live img type not supported!");
        }
        while (0);
    }
}

gui_video_t *gui_video_create_from_ftl(void           *parent,
                                       const char     *name,
                                       void           *addr,
                                       int16_t         x,
                                       int16_t         y,
                                       int16_t         w,
                                       int16_t         h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "DEFAULT_LIVE_IMAGE";
    }

    gui_video_t *img = gui_malloc(sizeof(gui_video_t));
    GUI_ASSERT(img != NULL);
    if (img == NULL)
    {
        return NULL;
    }

    memset(img, 0x00, sizeof(gui_video_t));
    img->storage_type = IMG_SRC_FTL;

    gui_img_video_ctor(img, (gui_obj_t *)parent, name, addr, x, y, w, h);

    GET_BASE(img)->create_done = true;

    return img;
}

gui_video_t *gui_video_create_from_fs(void           *parent,
                                      const char     *name,
                                      void           *addr,
                                      int16_t         x,
                                      int16_t         y,
                                      int16_t         w,
                                      int16_t         h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "DEFAULT_LIVE_IMAGE";
    }

    gui_video_t *img = gui_malloc(sizeof(gui_video_t));
    GUI_ASSERT(img != NULL);
    if (img == NULL)
    {
        return NULL;
    }

    memset(img, 0x00, sizeof(gui_video_t));
    img->storage_type = IMG_SRC_FILESYS;

    gui_img_video_ctor(img, (gui_obj_t *)parent, name, addr, x, y, w, h);

    GET_BASE(img)->create_done = true;

    return img;
}

gui_video_t *gui_video_create_from_mem(void           *parent,
                                       const char     *name,
                                       void           *addr,
                                       int16_t         x,
                                       int16_t         y,
                                       int16_t         w,
                                       int16_t         h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "DEFAULT_LIVE_IMAGE";
    }

    gui_video_t *img = gui_malloc(sizeof(gui_video_t));
    GUI_ASSERT(img != NULL);
    if (img == NULL)
    {
        return NULL;
    }

    memset(img, 0x00, sizeof(gui_video_t));
    img->storage_type = IMG_SRC_MEMADDR;

    gui_img_video_ctor(img, (gui_obj_t *)parent, name, addr, x, y, w, h);

    GET_BASE(img)->create_done = true;

    return img;
}

