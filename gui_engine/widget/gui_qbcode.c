/*===================================================================================
*                   gui QBCode Widget
*===================================================================================*/
#include "gui_qrcode_gen.h"
#include "gui_barcode_gen.h"
#include "gui_qbcode.h"
#include "gui_img.h"

/*===================================================================================
*                   Local Functions
*===================================================================================*/
void gui_qbcode_ctor(gui_qbcode_t *this, gui_obj_t *parent, const char *name, int16_t x,
                     int16_t y, int16_t w, int16_t h);
void qbcode_destroy(gui_obj_t *obj);


static void qbcode_prepare(gui_obj_t *obj)
{
    gui_point_t point = {0, 0, 1};

    matrix_multiply_point(obj->matrix, &point);

    gui_qbcode_t *this = (void *)obj;
    this->offset_x = point.p[0];
    this->offset_y = point.p[1];

    this->scale_x = obj->matrix->m[0][0];
    this->scale_y = obj->matrix->m[1][1];

}

static void gui_qbcode_draw(gui_qbcode_t *qbcode, gui_rect_t *rect)
{
    gui_dispdev_t *dc = gui_get_dc();

    if ((rect->x2 < dc->section.x1) || (rect->y2 < dc->section.y1)
        || (rect->x1 >= dc->section.x1 + dc->fb_width) || (rect->y1 >= dc->section.y1 + dc->fb_height))
    {
        return;
    }

    if (qbcode->data == NULL)
    {
        return;
    }

    uint8_t *qbcode_data = qbcode->data;
    int qbcode_x = rect->x1 ;//base->x;
    int qbcode_y = rect->y1 ;//base->y;
    int qbcode_w = (rect->x2 - rect->x1) ;//base->w;
    int qbcode_h = (rect->y2 - rect->y1);
    int x_start = _UI_MAX(qbcode_x, 0);
    int x_end = _UI_MIN(qbcode_x + qbcode_w, dc->fb_width);

    int y_start = _UI_MAX(dc->section.y1, qbcode_y);
    int y_end = _UI_MIN(dc->section.y2, qbcode_y + qbcode_h) + 1;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    uint16_t qbcode_size = qbcode_data[0];//qrcodegen_getSize((uint8_t *)qbcode_data);
    qbcode_data += 1;
    uint16_t qbcode_border_size = qbcode->border_size;
    uint16_t line_width = (x_end - x_start) << 1;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int16_t findey = (i - qbcode_y) * (qbcode_size + (qbcode_border_size + qbcode_border_size)) /
                         qbcode_h - qbcode_border_size;
        int write_off = (i - dc->section.y1) * dc->fb_width ;
        if ((findey < 0) || (findey >= qbcode_size))
        {
            memset(&writebuf[write_off + x_start], 0xff, line_width);
            continue;
        }

        //int16_t jx = (j - qbcode_x);
        for (uint32_t j = x_start; j < x_end; j++)
        {
            int16_t findex = (j - qbcode_x) * (qbcode_size + (qbcode_border_size + qbcode_border_size)) /
                             qbcode_w - qbcode_border_size;
            if ((0 > findex) || (findex >= qbcode_size))
            {
                writebuf[write_off + j] = 0xffff;
            }
            else
            {
                int index = findey * qbcode_size + findex;
                if (!((qbcode_data[(index >> 3)] >> (index & 7)) & 1))
                {
                    writebuf[write_off + j] = 0xffff;
                }
                else
                {
                    writebuf[write_off + j] = 0x00;
                }
            }
        }
    }
}
static void gui_barcode_draw(gui_qbcode_t *barcode, gui_rect_t *rect)
{
    gui_dispdev_t *dc = gui_get_dc();

    if ((rect->x2 < dc->section.x1) || (rect->y2 < dc->section.y1)
        || (rect->x1 >= dc->section.x1 + dc->fb_width) || (rect->y1 >= dc->section.y1 + dc->fb_height))
    {
        return;
    }

    if (barcode->data == NULL)
    {
        return;
    }

    int qbcode_x = rect->x1 ;//base->x;
    int qbcode_y = rect->y1 ;//base->y;
    int qbcode_w = (rect->x2 - rect->x1) ;//base->w;
    int qbcode_h = (rect->y2 - rect->y1);
    int x_start = _UI_MAX(qbcode_x, 0);
    int x_end = _UI_MIN(qbcode_x + qbcode_w, dc->fb_width);

    int y_start = _UI_MAX(dc->section.y1, qbcode_y);
    int y_end = _UI_MIN(dc->section.y2, qbcode_y + qbcode_h) + 1;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint8_t *barcode_data = ((barcode_symbol_t *)(barcode->data))->encoded_data[0];
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    uint16_t barcode_size = ((barcode_symbol_t *)(barcode->data))->width;
    uint16_t qbcode_border_size = barcode->border_size;
    uint16_t line_width = (x_end - x_start) << 1;

    uint32_t offset_record = 0;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int16_t findy = (i - qbcode_y);
        int write_off = (i - dc->section.y1) * dc->fb_width;
        if ((findy < qbcode_border_size) || (findy >= (qbcode_h - qbcode_border_size)))
        {
            memset(writebuf + write_off + x_start, 0xff, line_width);
            continue;
        }
#if 1
        if ((offset_record) == 0)
        {
            for (uint32_t j = x_start; j < x_end; j++)
            {
                int16_t findex = (j - qbcode_x) * (barcode_size + qbcode_border_size + qbcode_border_size) /
                                 qbcode_w - qbcode_border_size;
                if ((findex < 0) || (findex >= barcode_size))
                {
                    writebuf[write_off + j] = 0xffff;
                }
                else
                    //writebuf[write_off + j] = ((barcode_data[findex / 8]) & (0x01 << (findex) % 8))? 0x0000:0xffff;
                {
                    writebuf[write_off + j] = ((barcode_data[findex >> 3] >> (findex & 7)) & (0x01)) ? 0x0000 : 0xffff;
                }
            }

            offset_record = write_off + x_start;
        }
        else
        {
            memcpy(writebuf + write_off + x_start, writebuf + offset_record, line_width);
        }
#else
        for (uint32_t j = x_start; j < x_end; j++)
        {
            int16_t findex = (j - qbcode_x) * (barcode_size + qbcode_border_size + qbcode_border_size) /
                             qbcode_w - qbcode_border_size;
            if ((findex < 0) || (findex >= barcode_size))
            {
                writebuf[write_off + j] = 0xffff;
            }
            else
                //writebuf[write_off + j] = ((barcode_data[findex / 8]) & (0x01 << (findex) % 8))? 0x0000:0xffff;
            {
                writebuf[write_off + j] = ((barcode_data[findex >> 3] >> (findex & 7)) & (0x01)) ? 0x0000 : 0xffff;
            }
        }
#endif
    }
}

static void qbcode_draw(gui_obj_t *obj)
{
    if (obj == NULL)
    {
        return;
    }

    gui_qbcode_t *qbcode = (gui_qbcode_t *)obj;

    //struct gui_dispdev *dc = gui_get_dc();
    gui_rect_t draw_rect = {0};
    draw_rect.x1 = /*obj->x +*/ qbcode->offset_x;//obj->ax + obj->dx + obj->tx;
    draw_rect.y1 = /*obj->y + */qbcode->offset_y;//obj->ay + obj->dy + obj->ty;
    draw_rect.x2 = draw_rect.x1 + obj->w * qbcode->scale_x;
    draw_rect.y2 = draw_rect.y1 + (obj->h) * qbcode->scale_y ;

    if (qbcode->type == QRCODE_DISPLAY_SECTION)
    {
        gui_qbcode_draw(qbcode, &draw_rect);
    }
    else if (qbcode->type == BARCODE_DISPLAY_SECTION)
    {
        gui_barcode_draw(qbcode, &draw_rect);
    }
}

static void qbcode_end(gui_obj_t *obj)
{

}

void qbcode_destroy(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_qbcode_t *qbcode = (gui_qbcode_t *) obj;
    if (qbcode && qbcode->image_data)
    {
        gui_free(qbcode->image_data);
    }

    if (qbcode && (qbcode->encodeType == BARCODE_ENCODE_TEXT) && (qbcode->data))
    {
        gui_free(qbcode->data);
    }

    if (qbcode)
    {
        gui_free(qbcode);
    }
}


void gui_qbcode_ctor(gui_qbcode_t *this, gui_obj_t *parent, const char *name, int16_t x,
                     int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = QRCODE;
    root->obj_prepare = qbcode_prepare;
    root->obj_draw = qbcode_draw;
    root->obj_end = qbcode_end;
    root->obj_destory = qbcode_destroy;

    //for qrcodeself
    if (this->type == QRCODE_DISPLAY_IMAGE)
    {

    }
    else if (this->type == QRCODE_DISPLAY_SECTION)
    {
        this->data = NULL;
    }
    else if (this->type == BARCODE_DISPLAY_IMAGE)
    {
        this->data = NULL;
    }
    else if (this->type == BARCODE_DISPLAY_SECTION)
    {
        this->data = NULL;
    }
}

/**
*
*/
void gui_qbcode_config(gui_qbcode_t *qbcode, uint8_t *data, uint32_t data_len, uint8_t border_size)
{
    GUI_ASSERT(qbcode);
    //generate qbcode data
    if (qbcode->encodeType == QRCODE_ENCODE_TEXT)
    {
        qbcode->data = qrcode_get_data(data, data_len, 0);
    }
    else if (qbcode->encodeType == QRCODE_ENCODE_BINARY)
    {
        qbcode->data = qrcode_get_data(data, data_len, 1);
    }
    else if (qbcode->encodeType == BARCODE_ENCODE_TEXT)
    {
        if (qbcode->data) { gui_free(qbcode->data); }
        barcode_symbol_t *barcode_encode(unsigned char source[], int length);
        qbcode->data = barcode_encode(data, data_len);
    }

    qbcode->border_size = border_size;
    //generate image and scale
    if ((qbcode->data) && (qbcode->type == QRCODE_DISPLAY_IMAGE))
    {
        uint8_t *img_data = gui_realloc(qbcode->image_data, qbcode->base.w * qbcode->base.h * 2 + 8);
        GUI_ASSERT(img_data);
        memset(img_data, 0, qbcode->base.w * qbcode->base.h * 2 + 8);

        uint16_t qbcode_size = qrcodegen_getSize(qbcode->data);
        uint16_t image_wsize = qbcode->base.w;
        uint16_t image_hsize = qbcode->base.h;
        uint16_t *write_buf = (uint16_t *)(img_data + 8);

        for (uint32_t i = 0; i < image_hsize; i ++)
        {
            int16_t findey = (i) * (qbcode_size + border_size + border_size) / image_hsize - border_size;
            int write_off = (i) * image_wsize ;
            for (uint32_t j = 0; j < image_wsize; j++)
            {
                int16_t findex = (j) * (qbcode_size + border_size + border_size) / image_wsize - border_size;
                write_buf[write_off + j] = qrcodegen_getModule((uint8_t *)qbcode->data, findex,
                                                               findey) ? 0 : 0xffff;
            }
        }
        gui_rgb_data_head_t *p_data = (gui_rgb_data_head_t *)img_data;
        p_data->scan = 0;
        p_data->align = 0;
        p_data->resize = 0;
        p_data->compress = 0;
        p_data->rsvd = 0;
        p_data->type = RGB565;
        p_data->w = image_wsize;
        p_data->h = image_hsize;
        p_data->version = 0;
        p_data->rsvd2 = 0;

        gui_img_t *qbcode_img = gui_img_create_from_mem(qbcode, "qbcode_img", img_data, 0, 0,
                                                        qbcode->base.w, qbcode->base.h);

        //qbcode->qbcode_img = btn;
        qbcode->image_data = img_data;
    }

    else if ((qbcode->data) && (qbcode->type == BARCODE_DISPLAY_IMAGE))
    {
        uint8_t *img_data = gui_realloc(qbcode->image_data, qbcode->base.w * qbcode->base.h * 2 + 8);
        GUI_ASSERT(img_data);
        memset(img_data, 0, qbcode->base.w * qbcode->base.h * 2 + 8);

        uint16_t image_wsize = qbcode->base.w;
        uint16_t image_hsize = qbcode->base.h;
        uint16_t *write_buf = (uint16_t *)(img_data + 8);
        barcode_symbol_t *p_barcode = (barcode_symbol_t *)qbcode->data;

        for (int32_t i = 0; i < image_hsize; i ++)
        {
            int write_off = (i) * image_wsize ;
            if ((i < border_size) || (i >= (image_hsize - border_size)))
            {
                //write_buf[write_off] = 0xffff;
                memset(write_buf + write_off, 0xff, image_wsize * 2);
                continue;
            }

            for (uint32_t j = 0; j < image_wsize; j++)
            {

                int16_t findex = (j) * (p_barcode->width + border_size + border_size) / image_wsize - border_size;
                write_buf[write_off + j] = ((p_barcode->encoded_data[0][findex / 8]) & (0x01 <<
                                                                                        (findex) % 8)) ? 0 : 0xffff;
            }
        }
        gui_rgb_data_head_t *p_data = (gui_rgb_data_head_t *)img_data;
        p_data->scan = 0;
        p_data->align = 0;
        p_data->resize = 0;
        p_data->compress = 0;
        p_data->rsvd = 0;
        p_data->type = RGB565;
        p_data->w = image_wsize;
        p_data->h = image_hsize;
        p_data->version = 0;
        p_data->rsvd2 = 0;

        gui_img_t *qbcode_img = gui_img_create_from_mem(qbcode, "qbcode_img", img_data, 0, 0,
                                                        qbcode->base.w, qbcode->base.h);

        //qbcode->qbcode_img = btn;
        qbcode->image_data = img_data;
    }

}

gui_qbcode_t *gui_qbcode_create(void *parent, const char *name, int16_t x, int16_t y,
                                int16_t w, int16_t h, t_QBcode_disType type, t_QBcode_encodeType encodeType)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "qbcode_unknown";
    }
    gui_qbcode_t *qbCode = gui_malloc(sizeof(gui_qbcode_t));
    GUI_ASSERT(qbCode != NULL);
    memset(qbCode, 0, sizeof(gui_qbcode_t));

    qbCode->type = type;
    qbCode->encodeType = encodeType;
    gui_qbcode_ctor(qbCode, parent, name, x, y, w, h);

    gui_list_init(&(GET_BASE(qbCode)->child_list));
    if ((GET_BASE(qbCode)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(qbCode)->parent)->child_list),
                               &(GET_BASE(qbCode)->brother_list));
    }
    GET_BASE(qbCode)->create_done = true;

    return qbCode;
}
