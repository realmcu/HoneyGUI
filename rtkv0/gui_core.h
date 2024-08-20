#ifndef _GUI_CORE_H_
#define _GUI_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "stdbool.h"
#include "def_matrix.h"

typedef enum
{
    FRAMEBUFFER_MODE_NONE_PSRAM = 0,
    FRAMEBUFFER_MODE_PSRAM,
} GUI_FRAMEBUFFER_MODE;

typedef struct _UI_StringRecordTypeDef
{
    uint16_t single_font_height;
    uint16_t current_number;
    uint16_t total_count;
    bool     new_font;
    uint32_t font_color;
    uint32_t font_string_addr;
} UI_StringRecordTypeDef;

typedef struct _UI_ClockPointerTypeDef
{
    int16_t center_x;
    int16_t center_y;
    uint16_t source_pic_width;
    uint16_t source_pic_height;
    uint16_t source_pic_center_height;
    float   rotation_cosa;
    float   rotation_sina;
} UI_ClockPointerTypeDef;


typedef struct
{
    uint32_t timer_id;
    char     *menu_name;
} RtkGuiTimerInfo_t;

typedef struct _UI_WidgetTypeDef
{
    int16_t x;
    int16_t y;
    int16_t active_ys;
    int16_t active_ye;
    int16_t width;
    int16_t hight;
    uint32_t addr;
    uint16_t widget_id_type;
    union
    {
        UI_ClockPointerTypeDef clock_pointer;
        UI_StringRecordTypeDef string_mem;
        uint32_t mask_addr;
        uint32_t graph_color;
        uint32_t num_string_addr;
    } u;
    int8_t arc_pos;
    bool touch_capability;
    void (*widget_touch_func)(uint32_t button_touch_type);
} UI_WidgetTypeDef;

typedef struct _UI_BMPTypeDef
{
    int16_t x;
    int16_t y;
    int16_t active_ys;
    int16_t active_ye;
    int16_t width;
    int16_t hight;
    uint32_t addr;
} UI_BMPTypeDef;

typedef struct _UI_3DTypeDef
{
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t hight;
    uint32_t addr;
    gui_matrix_t matrix;
    gui_matrix_t inverse;
} UI_3DTypeDef;

typedef struct _UI_PSRAMBufferTypeDef
{
    const char *name;
    struct _UI_MenuTypeDef *Menu;
    uint8_t *pLCDBuffer;
    bool BufferReadFlag;
} UI_PSRAMBufferTypeDef;

typedef enum
{
    LEGACY_TOUCH_HOLD_X,
    LEGACY_TOUCH_HOLD_Y,
    LEGACY_TOUCH_SHORT,
    LEGACY_TOUCH_LONG,
    LEGACY_TOUCH_ORIGIN_FROM_X,
    LEGACY_TOUCH_ORIGIN_FROM_Y,
    LEGACY_TOUCH_LEFT_SLIDE,
    LEGACY_TOUCH_RIGHT_SLIDE,
    LEGACY_TOUCH_UP_SLIDE,
    LEGACY_TOUCH_DOWN_SLIDE,
    LEGACY_TOUCH_SHORT_BUTTON,
    LEGACY_TOUCH_LONG_BUTTON,
    LEGACY_TOUCH_UP_SLIDE_TWO_PAGE,
    LEGACY_TOUCH_DOWN_SLIDE_TWO_PAGE,//13
    LEGACY_TOUCH_INVALIDE,
} GESTURE_T;

typedef enum
{
    SHORT_TOUCH_BUTTON,
    LONG_TOUCH_BUTTON,
} TOUCH_BUTTON_T;

typedef enum
{
    TOUCH_SPEED_SLOW,
    TOUCH_SPEED_FAST,
} TOUCH_SPEED_T;


typedef struct
{
    GESTURE_T old_type;
    GESTURE_T type;
    TOUCH_SPEED_T speed;
    int16_t deltaX;
    int16_t deltaY;
    int16_t x;
    int16_t y;
} RtkTouchInfo_t;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t counter;
    uint16_t pressFlag;
    int32_t rtc_counter;
} RtkTouchRawData_t;

typedef struct _UI_MenuTypeDef
{
    const char *name;
    uint32_t *pMenuTime;
    struct _UI_MenuTypeDef *MenuParent;
    struct _UI_MenuTypeDef *MenuSub;
    void (*button_func)(struct _UI_MenuTypeDef *cur_menu, uint8_t type);
    void (*touch_func)(struct _UI_MenuTypeDef *cur_menu, RtkTouchInfo_t *info);
    void (*cur_display_info)(struct _UI_MenuTypeDef *cur_menu, struct _UI_MenuTypeDef *return_menu,
                             void *argv);

    void (* dynamic_cb)(struct _UI_MenuTypeDef *cur_menu, struct _UI_MenuTypeDef *return_menu,
                        void *argv);
    void (* display_cb)(struct _UI_MenuTypeDef *cur_menu, struct _UI_MenuTypeDef *return_menu,
                        void *argv);
    void (* constructor_cb)(struct _UI_MenuTypeDef *cur_menu, void *argv);
    void (* destructor_cb)(struct _UI_MenuTypeDef *cur_menu, void *argv);

    UI_WidgetTypeDef *pWidgetList;
    UI_BMPTypeDef *pBMPList;
    UI_3DTypeDef *p3DList;
    uint8_t current_max_widget;//this means widget
    uint8_t current_max_bmp;//this means bmp
    uint8_t current_max_3d;//this means 3d
    int16_t detal_x;
    int16_t detal_y;

    uint16_t psram_has_up_cache : 1;
    uint16_t psram_has_down_cache : 1;
    uint16_t psram_has_left_cache : 1;
    uint16_t psram_has_right_cache : 1;
    uint16_t psram_has_center_cache : 1;

    uint16_t dynamicFlag : 1;
    gui_matrix_t matrix;
    gui_matrix_t inverse;

} __attribute__((aligned(128))) UI_MenuTypeDef;


typedef enum
{
    ICON_BACKGROUND,
    ICON_WIDGET,
    ICON_STRING,
    ICON_CLOCK_POINTER,
    ICON_NUM,
    ICON_MASK,
    ICON_RECT,
    ICON_QRCODE,
    ICON_LINE,
    ICON_ROLL,
} ICON_TYPE;

typedef enum
{
    ICON_LEFT,
    ICON_RIGHT,
    ICON_UP,
    ICON_DOWN,
    ICON_MIDDLE
} CONSTRUCT_TYPE;



typedef struct
{
    bool        te_enable;
    bool        bak;
} GUI_ConfigDef;

extern bool lcd_screen_updating_flag;

extern GUI_FRAMEBUFFER_MODE FrameBufferMode;

void gui_set_te_function(bool flag);
void gui_core_init(void);
void rtl_gui_lcd_update_internal(void);
void rtl_gui_lcd_update(bool force_update);
void rtl_gui_menu_update(UI_MenuTypeDef *ui_cur, void *p_ui_cur_argv, \
                         UI_MenuTypeDef *ui_sub, void *p_ui_sub_argv, \
                         UI_MenuTypeDef *ui_par, void *p_ui_par_argv, \
                         int16_t detal_x, int16_t delta_y);
void rtl_gui_prepare_frame_buffer(UI_MenuTypeDef *pCurMenu, \
                                  UI_MenuTypeDef *pLoadMenu, \
                                  void *p_ui_cur_argv, \
                                  UI_PSRAMBufferTypeDef *pPSRAM);
void rtl_gui_update_detal(int16_t x, int16_t y);

void gui_v0_cube(float cbsize, \
                 float c_x, \
                 float c_y, \
                 float xrot, \
                 float yrot, \
                 float zrot, \
                 uint16_t img_w, uint16_t img_h, \
                 gui_matrix_t *m0, \
                 gui_matrix_t *m1, \
                 gui_matrix_t *m2, \
                 gui_matrix_t *m3, \
                 gui_matrix_t *m4, \
                 gui_matrix_t *m5);
void gui_v0_perspective(float release_x, \
                        uint16_t img_w, uint16_t img_h, \
                        gui_matrix_t *m0, \
                        gui_matrix_t *m1, \
                        gui_matrix_t *m2, \
                        gui_matrix_t *m3, \
                        gui_matrix_t *m4, \
                        gui_matrix_t *m5);
extern uint8_t *disp_write_buff1;
extern uint8_t *disp_write_buff2;
extern uint8_t *disp_read_buff;
extern uint8_t *dma_link_list_buff;

#ifdef __cplusplus
}
#endif

#endif /* _GUI_CORE_H_ */
