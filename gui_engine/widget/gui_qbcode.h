#ifndef _GUI_QBCODE_H_
#define _GUI_QBCODE_H_

#include <string.h>
#include "guidef.h"
#include "gui_obj.h"
#include "gui_img.h"

/** @brief  t_QBcode_encodeType structure */
typedef enum
{
    QRCODE_ENCODE_TEXT,
    QRCODE_ENCODE_BINARY,
    BARCODE_ENCODE_TEXT,
} t_QBcode_encodeType;

/** @brief  t_QBcode_disType structure */
typedef enum
{
    QRCODE_DISPLAY_SECTION,
    QRCODE_DISPLAY_IMAGE,
    BARCODE_DISPLAY_SECTION,
    BARCODE_DISPLAY_IMAGE,
} t_QBcode_disType;

/** @brief  qrcode widget structure */
typedef struct gui_qrcode
{
    gui_obj_t base;
    int16_t offset_x;
    int16_t offset_y;
    uint16_t border_size;
    float scale_x;
    float scale_y;
    void *data;
    void *image_data;
    gui_img_t *qbcode_img;
    t_QBcode_disType type;
    t_QBcode_encodeType encodeType;
} gui_qbcode_t;


/**
 * @brief Create a qrcode widget.
 *
 * @param parent the father widget
 * @param filename this qrcode widget's name.
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w qrcode image display width including border
 * @param h qrcode image display height including border
 * @param type QRCODE_DISPLAY_SECTION (gui in real-time) or QRCODE_DISPLAY_IMAGE(save in psRAM by default) or BARCODE_DISPLAY_SECTION or BARCODE_DISPLAY_IMAGE.
 * @param encodeType QRCODE_ENCODE_TEXT or QRCODE_ENCODE_BINARY or BARCODE_ENCODE_TEXT supported.
 * @return gui_qbcode_t* success, NULL failed.
 * @note  this function just create a qrcode object, qbcode data and param should be config by gui_qbcode_config() api.
 */
gui_qbcode_t *gui_qbcode_create(void *parent, const char *name, int16_t x, int16_t y,
                                int16_t w, int16_t h, t_QBcode_disType type, t_QBcode_encodeType encodeType);

/**
 * @brief config qbcode data and border param for a qbcode object.
 *
 * @param qbcode a qbcode object pointer.
 * @param data  input data encoded for qbcode
 * @param data_len input data length for encode data
 * @param border_size qrode border size, can be 1, 2, 3... by qbcode size, default white color border;
 * @return null
 */
void gui_qbcode_config(gui_qbcode_t *qbcode, uint8_t *data, uint32_t data_len, uint8_t border_size);

/**
 * for example:
 *
 *  qbcode width 320, height 320, t_QBcode_disType qbcode_DISPLAY_SECTION, t_QBcode_encodeType qbcode_ENCODE_TEXT, border size 2;
 *  gui_qbcode_t * qbcode = gui_qbcode_create(parent, "qbcode", 454/2 - 320/2, 454/2 - 320/2, 320, 320, qbcode_DISPLAY_SECTION, qbcode_ENCODE_TEXT);
 *  char* str = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    gui_qbcode_config(qbcode, str, strlen(str), 2);
 *
 */
#endif
