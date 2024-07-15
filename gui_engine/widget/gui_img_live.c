/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img_live.c
  * @brief live image widget
  * @details live image widget
  * @author roy@realsil.com.cn
  * @date 2024/5/15
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_img_live.h"

#ifdef IMG_LIVE_TJPGDEC
#include "../3rd/tjpgd/tjpgd.h"
#endif
/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */



/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */


#define MGPEG_SEG_SOI  (0xD8FF)
#define MGPEG_SEG_EOI  (0xD9FF)

static bool is_mjpeg(void *pdata)
{
    return ((*((uint16_t *)(pdata)) == MGPEG_SEG_SOI));
}


static uint8_t **mjpeg2jpeg_slicer(uint8_t *mjpeg, uint32_t size, uint32_t *slice_cnt)
{
    // int ret = 0;
    // int input_skip = 0, output_size = 0;
    uint8_t *pdata = mjpeg;
    uint8_t *ptmp = mjpeg;
    // uint8_t *output = NULL;
    uint8_t **array = NULL;
    *slice_cnt = 0;
    uint8_t scan_state = 0;  // 0: init, 1: soi, 2: eoi

    if (size < 12)
    {
        // av_log(ctx, AV_LOG_ERROR, "input is truncated\n");
        return NULL;
    }
    if (*((uint16_t *)(pdata)) != 0xd8ff)
    {
        // av_log(ctx, AV_LOG_ERROR, "input is not MJPEG\n");
        return NULL;
    }

    // if (in_data[2] == 0xff && in_data[3] == APP0) {
    //     input_skip = (in_data[4] << 8) + in_data[5] + 4;
    // } else {
    //     input_skip = 2;
    // }

    // if (size < input_skip) {
    //     // av_log(ctx, AV_LOG_ERROR, "input is truncated\n");
    //     // ret = AVERROR_INVALIDDATA;
    //     goto fail;
    // }

    // output_size = in->size - input_skip +
    //               sizeof(jpeg_header) + dht_segment_size;
    // ret = av_new_packet(out, output_size);
    // if (ret < 0)
    //     goto fail;

    while ((pdata - mjpeg) < size)
    {
        if (*((uint16_t *)pdata) == MGPEG_SEG_SOI)
        {
            scan_state = 1;
            ptmp = pdata;
        }
        else if ((scan_state == 1) && (*((uint16_t *)pdata) == MGPEG_SEG_EOI))
        {
            scan_state = 2;
            (*slice_cnt)++;
            if (!array)
            {
                array = (uint8_t **)gui_malloc(sizeof(uint8_t *));
            }
            else
            {
                gui_realloc(array, *slice_cnt * sizeof(uint8_t *));
            }
            if (!array)
            {
                gui_log("array malloc error!");
                return NULL;
            }
            array[*slice_cnt - 1] = ptmp;
            scan_state = 0;
        }
        pdata++;
    }
    return array;
}

#ifdef IMG_LIVE_TJPGDEC
static size_t in_func(     /* Returns number of bytes read (zero on error) */
    JDEC *jd,       /* Decompression object */
    uint8_t *buff,  /* Pointer to the read buffer (null to remove data) */
    size_t nbyte    /* Number of bytes to read/remove */
)
{
    LIVE_IODEV *dev = (LIVE_IODEV *)
                      jd->device;  /* Session identifier (5th argument of jd_prepare function) */
    static uint32_t offset = 0;
    uint8_t *pstart = dev->mem_addr;

    // reset offset
    offset = (0 == dev->state) ? 0 : offset;
    dev->state = 1;

    if (buff)
    {
        /* Read data from imput stream */
        memcpy(buff, pstart + offset, nbyte);
    }
    else
    {
        /* Remove data from input stream */
    }

    offset += nbyte;
    return nbyte;
}

static int out_func(       /* Returns 1 to continue, 0 to abort */
    JDEC *jd,       /* Decompression object */
    void *bitmap,   /* Bitmap data to be output */
    JRECT *rect,     /* Rectangular region of output image */
    gui_obj_t *obj
)
{
    LIVE_IODEV *dev = (LIVE_IODEV *)
                      jd->device;  /* Session identifier (5th argument of jd_prepare function) */
    uint8_t *src, *dst;
    uint16_t y, bws;
    unsigned int bwd;
    uint8_t N_BPP = (3 - JD_FORMAT);

    /* Progress indicator */
    if (rect->left == 0)
    {
        printf("\r%lu%%", (rect->top << jd->scale) * 100UL / jd->height);
    }

    /* Copy the output image rectangle to the frame buffer */
    src = (uint8_t *)bitmap;                          /* Output bitmap */
    dst = dev->fbuf + N_BPP * (rect->top * dev->wfbuf +
                               rect->left);  /* Left-top of rectangle in the frame buffer */
    bws = N_BPP * (rect->right - rect->left + 1);     /* Width of the rectangle [byte] */
    bwd = N_BPP * dev->wfbuf;                         /* Width of the frame buffer [byte] */
    for (y = rect->top; y <= rect->bottom; y++)
    {
        memcpy(dst, src, bws);   /* Copy a line */
        src += bws; dst += bwd;  /* Next line */
    }

    return 1;    /* Continue to decompress */
}
static void *gui_img_live_decode_jpeg(void *jepg)
{
    JRESULT res = 0;      /* Result code of TJpgDec API */
    JDEC jdec;        /* Decompression object */
    void *work = NULL;       /* Pointer to the work area */
    size_t sz_work = 3500; /* Size of work area */
    LIVE_IODEV devid;      /* Session identifier */
    void *data = NULL;
    uint8_t N_BPP = (3 - JD_FORMAT);

    /* Initialize input stream */
    GUI_ASSERT(jepg != NULL);
    devid.mem_addr = jepg;
    devid.state = 0;

    /* Prepare to decompress */
    work = (void *)gui_malloc(sz_work);
    res = jd_prepare(&jdec, in_func, work, sz_work, &devid);
    if (res == JDR_OK)
    {
        /* It is ready to dcompress and image info is available here */
        printf("Image size is %u x %u.\n%u bytes of work ares is used.\n", jdec.width, jdec.height,
               sz_work - jdec.sz_pool);

        /* Initialize output device */
        gui_rgb_data_head_t header = {0};
        memset(&header, 0, sizeof(header));
        header.type = (JD_FORMAT == 0) ? 0x03 : (JD_FORMAT == 1) ? 0x00 : 0x00;
        header.w = jdec.width;
        header.h = jdec.height;

        data = (uint8_t *)gui_malloc(N_BPP * jdec.width * jdec.height + sizeof(
                                         gui_rgb_data_head_t)); /* Create frame buffer for output image */
        memcpy(data, &header, sizeof(gui_rgb_data_head_t));

        devid.fbuf = (data + sizeof(gui_rgb_data_head_t)) ;
        devid.wfbuf = jdec.width;


        // res = jd_decomp(&jdec, out_func, 0);   /* Start to decompress with 1/1 scaling */
        // jd_decomp(&jdec, out_func, 0, obj);
        res = jd_decomp(&jdec, out_func, 0, NULL);

        if (res == JDR_OK)
        {
            /* Decompression succeeded. You have the decompressed image in the frame buffer here. */
            printf("\rDecompression succeeded.\n");

        }
        else
        {
            printf("jd_decomp() failed (rc=%d)\n", res);
        }

    }
    else
    {
        printf("jd_prepare() failed (rc=%d)\n", res);
    }

    gui_free(work);             /* Discard work area */
    return data;
}
#endif


void gui_img_live_play_cb(void *p)
{
    gui_img_live_t *this = (gui_img_live_t *)p;

    this->frame_cur = (this->frame_cur + 1) % this->num_frame;
    gui_log("Cur frame %d\n", this->frame_cur);
    gui_fb_change();
}



static void gui_img_live_draw(gui_obj_t *obj)
{
    gui_img_live_t *this = (gui_img_live_t *)obj;
    uint8_t *data = NULL;

    // set by img prepare, reset to 1 here
    gui_img_scale(this->img, 1, 1);
    // decode jpeg
    if (this->img_type == IMG_LIVE_MJPEG)
    {
#ifdef __WIN32
        if (this->frame_buff)
        {
            // free(this->frame_buff);
            extern void stbi_image_free(void *retval_from_stbi_load);
            stbi_image_free(this->frame_buff);
            this->frame_buff = NULL;
        }
#ifdef IMG_LIVE_TJPGDEC
        this->frame_buff = gui_img_live_decode_jpeg(this->array[this->frame_cur]);
#else
        // stb decode
        {
            int x, y, n, ok;
            typedef unsigned char stbi_uc;
            extern stbi_uc *stbi_load_from_memory(stbi_uc const * buffer, int len, int *x, int *y,
                                                  int *channels_in_file, int desired_channels);
            this->frame_buff = (void *)stbi_load_from_memory(this->array[this->frame_cur], 40248, &x, &y, &n,
                                                             0);
            if (this->frame_buff)
            {
                gui_log("x %d y %d n %d\n", x, y, n);
                gui_rgb_data_head_t header = {0};
                memset(&header, 0, sizeof(header));
                header.type = 0x03; // RGB888
                header.w = x;
                header.h = y;
                memcpy(this->frame_buff, &header, sizeof(gui_rgb_data_head_t));
            }
        }
#endif

#endif
        data = this->frame_buff;
    }


    gui_img_set_attribute(this->img, "IMG_LIVE", data, this->base.x, this->base.y);
    this->img->draw_img->data = data;
}

static void gui_img_live_destory(gui_obj_t *obj)
{
    gui_img_live_t *this = (gui_img_live_t *)obj;

    if (this->src_mode == IMG_SRC_FILESYS)
    {
        // TODO: need free mjpeg
        gui_free(*(this->array));
    }
    else
    {

    }

    gui_free(this->array);
    if (this->frame_buff)
    {
        gui_free(this->frame_buff);
        this->frame_buff = NULL;
    }
}


static void gui_img_live_prepare(gui_obj_t *obj)
{
    gui_img_live_t *this = (gui_img_live_t *)obj;
    uint32_t frame_time_ms = 1000.f / this->fps;

    // TODO: tp
    // click play once
}


static void gui_img_live_file_init(gui_img_live_t  *this)
{
    void *mjpeg_buff = NULL;
    uint8_t **slice_array = NULL;
    uint32_t slice_cnt = 0;
    size_t sz_mjpeg = 200000;  // IMG_SRC_MEMADDR: should be set

    if (this->src_mode == IMG_SRC_FILESYS)
    {
        // char *file = "D:/Browser/ffmpeg-2024-05-08-git-e8e84dc325-full_build/bin/output.mjpeg";
        char *file = this->data;
        FILE *fp = fopen(file, "rb");
        int rdlen = 0;
        GUI_UNUSED(rdlen);
        if (!fp)
        {
            gui_log("Error: open mjpeg failed\n");
            return;
        }
        fseek(fp, 0, SEEK_END);
        sz_mjpeg = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        if (sz_mjpeg == 0)
        {
            gui_log("Error: sz_mjpeg cannot be negative.\n");
            fclose(fp);
            return;
        }
        if (!mjpeg_buff)
        {
            gui_log("Error: Memory allocation failed\n");
            fclose(fp);
            return;
        }
        mjpeg_buff = gui_malloc(sz_mjpeg);
        GUI_ASSERT(mjpeg_buff != NULL);
        memset(mjpeg_buff, 0, sz_mjpeg);
        rdlen = fread(mjpeg_buff, 1, sz_mjpeg, fp);
        fclose(fp);
    }
    else if (this->src_mode == IMG_SRC_MEMADDR)
    {
        mjpeg_buff = this->data;
    }
    if (!mjpeg_buff)
    {
        gui_log("Error: mjpeg_buff is NULL\n");
        return;
    }
    if (is_mjpeg(mjpeg_buff))
    {
        this->img_type = IMG_LIVE_MJPEG;
        slice_array = mjpeg2jpeg_slicer(mjpeg_buff, sz_mjpeg, &slice_cnt);
        if (!slice_array)
        {
            gui_log("mjpeg2jpeg_slicer error! \n");
        }
    }

    this->array = slice_array;
    this->num_frame = slice_cnt;
}


static void gui_img_live_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_img_live_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_img_live_draw(obj);
            break;

        case OBJ_DESTORY:
            gui_img_live_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_img_live_ctor(gui_img_live_t  *this,
                              gui_obj_t      *parent,
                              const char     *name,
                              void           *addr,
                              int16_t         x,
                              int16_t         y,
                              int16_t         w,
                              int16_t         h)
{
    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    uint32_t frame_time_ms = 0;
    // uint32_t live_time = 0;

    gui_obj_ctor(root, parent, name, x, y, w, h);
    root->type = IMAGE_FROM_MEM;
    root->obj_cb = gui_img_live_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_destroy_cb = true;

    //for self
    this->fps = IMG_LIVE_FPS;
    // this->fps = 24;
    this->data = addr;
    frame_time_ms = 1000.f / this->fps;
    // live_time = this->num_frame * frame_time_ms;

    // get img data from file, and img type
    gui_img_live_file_init(this);


    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    //
    this->img = gui_img_create_from_mem(this, "IMG_LIVE",
                                        this->data,
                                        x, y, w, h);
    gui_img_set_mode(this->img, IMG_BYPASS_MODE);
    gui_img_set_animate(this->img, frame_time_ms, -1, gui_img_live_play_cb, this);
    // gui_img_set_animate(this->img, live_time, -1, gui_img_live_play_cb, this);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_img_live_set_fps(gui_img_live_t *this, float fps)
{
    uint32_t frame_time_ms = 0;

    this->fps = fps;
    frame_time_ms = 1000.f / this->fps;
    gui_img_set_animate(this->img, frame_time_ms, -1, gui_img_live_play_cb, this);
}




gui_img_live_t *gui_img_live_create_from_mem(void           *parent,
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

    gui_img_live_t *img = gui_malloc(sizeof(gui_img_live_t));
    GUI_ASSERT(img != NULL);

    memset(img, 0x00, sizeof(gui_img_live_t));
    img->src_mode = IMG_SRC_MEMADDR;

    gui_img_live_ctor(img, (gui_obj_t *)parent, name, addr, x, y, w, h);

    GET_BASE(img)->create_done = true;

    return img;
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
