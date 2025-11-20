#ifndef _GUI_STRING_TRANSPARENCY_H_
#define _GUI_STRING_TRANSPARENCY_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "stdbool.h"
//#include "rtl876x.h"
//#include "wristband_gui.h"
typedef enum
{
    UI_STR_ALIGN_LEFT = 0,
    UI_STR_ALIGN_CENTER,
    UI_STR_ALIGN_RIGHT,
} UI_StringAlignTypeDef;

typedef enum
{
    LANGUAGE_ID_ENGLISH = 0x1,
    LANGUAGE_ID_DUTCH,
    LANGUAGE_ID_FRENCH,
    LANGUAGE_ID_GERMAN,
    LANGUAGE_ID_ITALIAN,
    LANGUAGE_ID_JAPANESE,
    LANGUAGE_ID_KOREAN,
    LANGUAGE_ID_PORTUGUESE,
    LANGUAGE_ID_RUSSIAN,
    LANGUAGE_ID_CHINESE,
    LANGUAGE_ID_SPANISH,
    LANGUAGE_ID_THAI,
    LANGUAGE_ID_VIETNAMESE
} Language_type_e;

#define RGB565(R, G, B)         (((R&0xF8))|((G&0xE0)>>5)|((G&0x1C)<<11)|((B&0xF8)<<5))
#define RGB565_R(C)             (C&0xF8)
#define RGB565_G(C)             (((C&0x07)<<5)|(C&0xE000)>>11)
#define RGB565_B(C)             ((C&0x1F00)>>5)

// #define LCD_WIDTH                           200
// #define LCD_HEIGHT                          320
#define FONT_RENDERING_STRIDE                  320

typedef struct _UI_StringRecordTypeDef
{
    uint16_t font_color;  //used
    uint8_t font_size;    //used
    uint8_t string_len;   //used

    uint8_t align:       2;  //used
    uint8_t bold:        1;  //used
    uint8_t dirty:       1;
    uint8_t scroll:      1;
    uint8_t need_scroll: 1;
    uint8_t reserved:    2;
    uint8_t last_pos;
    int16_t string_width; //used
} UI_StringRecordTypeDef;

typedef struct _UI_WidgetTypeDef
{
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    int16_t active_ys;
    int16_t active_ye;
    uint8_t widget_type;
    uint8_t touch_capability: 1; // true & false
    uint8_t compress_flag: 1;    // true & false
    uint8_t reserved0: 6;
    uint8_t reserved1;
    uint8_t reserved2;
    union
    {
        uint32_t addr;  //string addr
        void *addr_ptr;
    };
    union
    {
        UI_StringRecordTypeDef string_mem;      // ICON_STRING_BLEN

        uint32_t graph_color;                   // ICON_QRCODE
        uint32_t container_size;                // ICON_CONTAINER

        struct
        {
            uint32_t customer_param0;
            uint32_t customer_param1;
        };
    };
    void (*touch_func)(uint32_t touch_type);
} UI_WidgetTypeDef;

void set_font_language_type(Language_type_e type);
Language_type_e get_font_language_type(void);


uint8_t rtl_gui_show_string_transparency(UI_WidgetTypeDef *icon_font, int16_t delta_x,
                                         int16_t delta_y, int Zs, int Ze, uint8_t *writebuf);
uint8_t rtl_gui_get_string_idx_by_width(uint16_t *unicode, uint8_t data_length, uint8_t font_size,
                                        bool bold, uint16_t max_width, bool need_line_break);
uint16_t rtl_gui_get_app_name_icon_ys_by_width(uint16_t *unicode, uint8_t data_length,
                                               uint8_t font_size, bool bold, uint16_t max_width);


#ifdef __cplusplus
}
#endif

#endif /* _GUI_STRING_H_ */
