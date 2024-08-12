/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file app_calculator.c
  * @brief calculator
  * @details calculator
  * @author howie_wang@realsil.com.cn
  * @date 2023/11/13
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <gui_matrix.h>
#include "gui_button.h"
#include <gui_img.h>
#include "root_image_hongkong/ui_resource.h"

#include <math.h>
#include <gui_app.h>

/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */
typedef struct gui_calculator
{
    gui_obj_t base;                 //!< base structure
} gui_calculator_t;

typedef enum
{
    CAL_BUFF_INPUT_INDEX = 0,
    CAL_BUFF_OP_INDEX    = 1,
    CAL_BUFF_NUM_INDEX   = 2,
    CAL_BUFF_DISPLAY_INDEX   = 3,

    CAL_TEXT_FONT_SIZE_DISPLAY = 24,
    CAL_TEXT_FONT_SIZE_BTN     = 24,

};

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
#define CAL_BUFF_INPUT_MAXLEN 128  //!<
#define CAL_BUFF_OP_MAXLEN 64  //!<
#define CAL_BUFF_NUM_MAXLEN 64  //!<
#define CAL_BUFF_DISPLAY_MAXLEN 128 //!<


/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */

static void app_calculator_ui_design(gui_app_t *app);

static gui_app_t calculator =
{
    .screen =
    {
        .name = "calculator",
    },
    .ui_design = app_calculator_ui_design,
};

static char buff_input[CAL_BUFF_INPUT_MAXLEN];  // input debug
static char buff_op[CAL_BUFF_OP_MAXLEN];
static double buff_num[CAL_BUFF_NUM_MAXLEN];
static char buff_display[CAL_BUFF_DISPLAY_MAXLEN];

static uint16_t buff_data_len[4];

// display text
gui_text_t *text_display_up;
gui_text_t *text_display_dn;

// flg
static bool flg_key_point = false;
static bool flg_caldone = false;
static bool flg_error = false;


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */
static uint8_t buff_input_push(char ch)
{
    uint16_t len = buff_data_len[CAL_BUFF_INPUT_INDEX];

    if (len >= CAL_BUFF_INPUT_MAXLEN - 1)
    {
        // buff[] = "...\0", make the whole buff printable
        buff_input[len] = '\0';
        // loop buff
        buff_data_len[CAL_BUFF_INPUT_INDEX] = 0;
        len = 0;
    }
    buff_input[len] = ch;
    buff_data_len[CAL_BUFF_INPUT_INDEX]++;
    return 0;
}

static uint8_t buff_op_push(char ch)
{
    uint16_t len = buff_data_len[CAL_BUFF_OP_INDEX];

    if (len < CAL_BUFF_OP_MAXLEN)
    {
        buff_op[len] = ch;
        buff_data_len[CAL_BUFF_OP_INDEX]++;
        return 0;
    }
    return 1;
}

static char buff_op_pop(void)
{
    uint16_t len = buff_data_len[CAL_BUFF_OP_INDEX];
    char op = buff_op[len - 1];

    if (len > 1)
    {
        buff_data_len[CAL_BUFF_OP_INDEX]--;
        buff_op[len - 1] = '\0';
    }
    return op;
}

static void buff_op_clean(void)
{
    buff_op[0] = '#';
    buff_data_len[CAL_BUFF_OP_INDEX] = 1;
}

static char buff_op_top(void)
{
    uint16_t len = buff_data_len[CAL_BUFF_OP_INDEX];

    return buff_op[len - 1];
}

static uint8_t buff_num_push(double num)
{
    uint16_t len = buff_data_len[CAL_BUFF_NUM_INDEX];

    if (len < CAL_BUFF_NUM_MAXLEN)
    {
        buff_num[len] = num;
        buff_data_len[CAL_BUFF_NUM_INDEX]++;
        return 0;
    }
    return 1;
}

static double buff_num_pop(void)
{
    uint16_t len = buff_data_len[CAL_BUFF_NUM_INDEX];
    double num = 0;

    if (len != 0)
    {
        num = buff_num[len - 1];
        buff_data_len[CAL_BUFF_NUM_INDEX]--;
        buff_num[len - 1] = 0;
        return num;
    }
    return -1;
}

static void buff_num_clean(void)
{
    buff_num[0] = 0;
    buff_data_len[CAL_BUFF_NUM_INDEX] = 0;
}

static bool buff_display_isempty(void)
{
    if ((buff_data_len[CAL_BUFF_DISPLAY_INDEX] == 1) && (buff_display[0] == '0'))
    {
        return  true;
    }
    else
    {
        return false;
    }
}

static uint8_t buff_display_push(char ch)
{
    // new num input, replace default '0'
    if (buff_display_isempty() && ((ch >= '0') && (ch <= '9')))
    {
        buff_data_len[CAL_BUFF_DISPLAY_INDEX]--;
    }

    uint16_t len = buff_data_len[CAL_BUFF_DISPLAY_INDEX];
    if (len < (CAL_BUFF_INPUT_MAXLEN - 1))
    {
        buff_display[len] = ch;
        buff_display[len + 1] = '\0';
        buff_data_len[CAL_BUFF_DISPLAY_INDEX]++;
        return 0;
    }
    return 1;
}

static void buff_display_pop(void)
{
    uint16_t len = buff_data_len[CAL_BUFF_DISPLAY_INDEX];

    buff_data_len[CAL_BUFF_DISPLAY_INDEX]--;
    buff_display[len - 1] = '\0';
    if (len == 1)
    {
        buff_data_len[CAL_BUFF_DISPLAY_INDEX] = 1;;
        buff_display[0] = '0';
    }
}

static uint8_t buff_display_top(void)
{
    uint16_t len = buff_data_len[CAL_BUFF_DISPLAY_INDEX];

    return buff_display[len - 1];
}

static void buff_display_clean(void)
{
    buff_display[0] = '0';
    buff_display[1] = '\0';
    buff_data_len[CAL_BUFF_DISPLAY_INDEX] = 1;
}

static void cal_display_update(void)
{
    uint8_t line_len = 26;
    char *pstr = buff_display;
    uint8_t len = buff_data_len[CAL_BUFF_DISPLAY_INDEX];

    if (len <= line_len)
    {
        // single line
        gui_text_set(text_display_up, NULL, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                     0,
                     CAL_TEXT_FONT_SIZE_DISPLAY);
        gui_text_set(text_display_dn, pstr, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                     len,
                     CAL_TEXT_FONT_SIZE_DISPLAY);
    }
    else
    {
        // double line
        gui_text_set(text_display_up, pstr, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                     line_len,
                     CAL_TEXT_FONT_SIZE_DISPLAY);
        gui_text_set(text_display_dn, pstr + line_len, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                                 UINT8_MAX), len - line_len,
                     CAL_TEXT_FONT_SIZE_DISPLAY);
    }
}

static char cal_index2char(uint8_t btnIndex)
{
    char table[21] = "C<%/789x456-123+0 .=<";
    return table[btnIndex];
}
static bool cal_isac(char ch)
{
    return (ch == 'C') ? true : false;
}
static bool cal_isdel(char ch)
{
    return (ch == '<') ? true : false;
}
static bool cal_ispercent(char ch)
{
    return (ch == '%') ? true : false;
}
static bool cal_iseq(char ch)
{
    return (ch == '=') ? true : false;
}
static bool cal_isnum(char ch)
{
    return ((ch >= '0') && (ch <= '9')) ? true : false;
}
static bool cal_ispoint(char ch)
{
    return (ch == '.') ? true : false;
}
static bool cal_isop(char ch)
{
    if ((ch == '+') || (ch == '-') || (ch == 'x') || (ch == '/') || (ch == '=')) { return true; }
    return false;
}
static bool cal_isprintable(char ch)
{
    if (cal_isac(ch) || cal_isdel(ch) || cal_iseq(ch) || cal_ispercent(ch)) { return false; }
    else { return true; }
}

static uint8_t cal_op_priority(char ch)
{
    switch (ch)
    {
    case '#':
    case '=':
        return 0;

    case '-':
    case '+':
        return 1;

    case 'x':
    case '/':
        return 2;

    default:
        break;
    }
    return 0;
}

static uint8_t cal_op(void)
{
    double b = buff_num_pop();
    double a = buff_num_pop();
    char op = buff_op_pop();

    switch (op)
    {
    case '+':
        buff_num_push(a + b);
        return 0;
    case '-':
        buff_num_push(a - b);
        return 0;
    case 'x':
        buff_num_push(a * b);
        return 0;
    case '/':
        if (b == 0)
        {
            flg_error = true;
            return 1;
        }
        buff_num_push(a / b);
        return 0;
    default:
        break;
    }
    return 0;
}

static void cal_display_result(void)
{
    char buff_tmp[64] = "error";
    uint8_t cnt = 0;

    if (!flg_error)
    {
        double res = buff_num_pop();
        sprintf(buff_tmp, "%.6g", res);
    }
    cnt = strlen(buff_tmp);
    memcpy(buff_display, buff_tmp, cnt + 1);
    buff_data_len[CAL_BUFF_DISPLAY_INDEX] = cnt;
}

static void cal_start(void)
{
    uint8_t len = buff_data_len[CAL_BUFF_DISPLAY_INDEX];
    char cal_buff[CAL_BUFF_DISPLAY_MAXLEN];
    char *pinput_r = cal_buff;
    double num = 0;
    double power_decimal = 0.1;
    bool flg_point = false;

    memset(cal_buff, '\0', CAL_BUFF_DISPLAY_MAXLEN);
    memcpy(cal_buff, buff_display, len);
    gui_log("\ncal_buff: %s\n", cal_buff);
    while (*pinput_r)
    {
        if (cal_isnum(*pinput_r))
        {
            if (!flg_point)
            {
                num = num * 10 + (*pinput_r - '0');
            }
            else
            {
                num += (*pinput_r - '0') * power_decimal;
                power_decimal *= 0.1;
            }
        }
        else if (cal_isop(*pinput_r))
        {
            // save last num
            buff_num_push(num);
            num = 0;
            power_decimal = 0.1;
            flg_point = false;

            while (cal_op_priority(*pinput_r) <= cal_op_priority(buff_op_top()))
            {
                if (buff_op_top() == '#') { break; }
                if (cal_op()) { break; }
            }

            if (*pinput_r != '=')
            {
                buff_op_push(*pinput_r);
            }
        }
        else if (cal_ispoint(*pinput_r))
        {
            flg_point = true;
        }
        else if (cal_ispercent(*pinput_r))
        {
            num *= 0.01;
        }
        pinput_r++;
    }

    cal_display_result();
    flg_caldone = true;
}


static void cal_ac_proc(void)
{
    flg_key_point = false;
    flg_caldone = false;
    flg_error = false;

    buff_display_clean();
    buff_op_clean();
    buff_num_clean();
}

static void cal_del_proc(void)
{
    if (cal_ispoint(buff_display_top()))
    {
        flg_key_point = false;
    }
    buff_display_pop();
}

static void cal_num_proc(char ch)
{
    if (!buff_display_isempty() && (cal_ispercent(buff_display_top())))
    {}
    else
    {
        buff_display_push(ch);
    }
}

static void cal_op_proc(char ch)
{
    flg_key_point = false;

    if (cal_iseq(ch))
    {
        buff_display_push(ch);
        cal_start();
        return;
    }
    if (!buff_display_isempty() && cal_isop(buff_display_top()))
    {
        buff_display_pop();
        buff_display_push(ch);
    }
    else
    {
        buff_display_push(ch);
    }
}

static void cal_point_proc(char ch)
{
    if (!buff_display_isempty() && (cal_ispercent(buff_display_top()) || flg_key_point))
    {}
    else
    {
        flg_key_point = true;
        buff_display_push(ch);
    }
}

static void cal_percent_proc(char ch)
{
    if (!buff_display_isempty() && cal_isop(buff_display_top()))
    {}
    else
    {
        // buff_display_push(ch);
        char temp[64];
        char *pdisplay = buff_display + buff_data_len[CAL_BUFF_DISPLAY_INDEX] - 1;
        uint8_t len = 0;
        double percent_data = 0;

        while (((pdisplay + 1) != buff_display) && ((cal_isnum(*pdisplay)) || (cal_ispoint(*pdisplay))))
        {
            pdisplay--;
            len++;
        }
        memset(temp, 0, 64);
        memcpy(temp, pdisplay + 1, len);
        percent_data = atof(temp) * 0.01;
        sprintf(pdisplay + 1, "%.6g", percent_data);
        buff_data_len[CAL_BUFF_DISPLAY_INDEX] -= len;
        buff_data_len[CAL_BUFF_DISPLAY_INDEX] += strlen(pdisplay + 1);
    }
}


static void input_proc(uint8_t keyIndex)
{
    char cal_key = cal_index2char(keyIndex);
    // // log input
    // buff_input_push(cal_key);
    // gui_log(buff_input);

    do
    {
        // recover from error
        if (flg_error)
        {
            cal_ac_proc();
            break;
        }
        if (cal_isnum(cal_key))
        {
            if (flg_caldone)
            {
                cal_ac_proc();
            }
            cal_num_proc(cal_key);
            break;
        }
        if (cal_isop(cal_key))
        {
            flg_caldone = false;
            cal_op_proc(cal_key);
            break;
        }

        if (cal_isac(cal_key))
        {
            cal_ac_proc();
            break;
        }
        if (cal_isdel(cal_key))
        {
            if (flg_caldone)
            {
                cal_ac_proc();
            }
            cal_del_proc();
            break;
        }

        if (cal_ispoint(cal_key))
        {
            if (flg_caldone)
            {
                break;
            }
            cal_point_proc(cal_key);
            break;
        }
        if (cal_ispercent(cal_key))
        {
            flg_caldone = false;
            cal_percent_proc(cal_key);
            break;
        }

    }
    while (0);

    // gui_log("\n\r> %s\n\r", buff_display);
    cal_display_update();
}

static void cb_button_0() {     gui_log("0");     input_proc(0);}
static void cb_button_1() {     gui_log("1");     input_proc(1);}
static void cb_button_2() {     gui_log("2");     input_proc(2);}
static void cb_button_3() {     gui_log("3");     input_proc(3);}
static void cb_button_4() {     gui_log("4");     input_proc(4);}
static void cb_button_5() {     gui_log("5");     input_proc(5);}
static void cb_button_6() {     gui_log("6");     input_proc(6);}
static void cb_button_7() {     gui_log("7");     input_proc(7);}
static void cb_button_8() {     gui_log("8");     input_proc(8);}
static void cb_button_9() {     gui_log("9");     input_proc(9);}
static void cb_button_10() {    gui_log("10");    input_proc(10);}
static void cb_button_11() {    gui_log("11");    input_proc(11);}
static void cb_button_12() {    gui_log("12");    input_proc(12);}
static void cb_button_13() {    gui_log("13");    input_proc(13);}
static void cb_button_14() {    gui_log("14");    input_proc(14);}
static void cb_button_15() {    gui_log("15");    input_proc(15);}
static void cb_button_16() {    gui_log("16");    input_proc(16);}
static void cb_button_17() {    gui_log("17");    input_proc(17);}
static void cb_button_18() {    gui_log("18");    input_proc(18);}
static void cb_button_19() {    gui_log("19");    input_proc(19);}
static void cb_button_20() {    gui_log("20");    input_proc(20);}

static gui_event_cb_t cb_btn[21] = {cb_button_0, cb_button_1, cb_button_2, cb_button_3, cb_button_4, \
                                    cb_button_5, cb_button_6, cb_button_7, cb_button_8, cb_button_9, \
                                    cb_button_10, cb_button_11, cb_button_12, cb_button_13, cb_button_14, \
                                    cb_button_15, cb_button_16, cb_button_17, cb_button_18, cb_button_19, cb_button_20
                                   };


static void calculator_draw_button(void *parent)
{
    char *text = NULL;
    int16_t del_x = 88;  int16_t del_y = 65;
    int16_t offset_x = 8; int16_t offset_y = 90;
    int8_t i = 0;
    int8_t j = 0;

    gui_button_t *button[21];
    // grid
    int16_t pos_x[4] = {offset_x, offset_x + del_x, offset_x + del_x * 2, offset_x + del_x * 3};
    int16_t pos_y[4] = {offset_y, offset_y + del_y, offset_y + del_y * 2, offset_y + del_y * 3};
    uint8_t *btn_char = "C<%/789x456-123+00.= ";
    void *background_pic = BUTTON_CAL_GRAY_BIN;
    void *highlight_pic  = BUTTON_CAL_BLK_BIN;

    for (i = 0; i < 20; i++)
    {
        int16_t btn_delx = del_x;
        if (i == 17) { continue; }

        if ((i + 1) % 4 == 0)
        {
            background_pic = BUTTON_CAL_ORG_BIN;
            highlight_pic  = BUTTON_CAL_YELLOW_BIN;
        }
        else
        {
            if (j == 0)
            {
                background_pic = BUTTON_CAL_GRAY_BIN;
                highlight_pic  = BUTTON_CAL_BLK_BIN;
            }
            else
            {
                background_pic = BUTTON_CAL_BLK_BIN;
                highlight_pic  = BUTTON_CAL_GRAY_BIN;
            }
            if (i == 16)
            {
                background_pic = BUTTON_CAL_BLK_LONG_BIN;
                highlight_pic  = BUTTON_CAL_GRAY_LONG_BIN;
                btn_delx = 2 * del_x;
            }
        }
        button[i] = gui_button_create(parent, offset_x + del_x * (i % 4), offset_y + del_y * j, btn_delx,
                                      del_y, background_pic, highlight_pic, "text", 0, 0);
        j += ((i + 1) % 4 == 0);
    }
    // delete key
    button[20] = gui_button_create(parent, offset_x + del_x * 3 + 42 - 22, offset_y / 2 - 10, del_x,
                                   del_y, BUTTON_CAL_DEL_BIN, BUTTON_CAL_DEL_BIN, "text", 0, 0);

    // load Font data
    void *addr_font = SIMKAI_SIZE24_BITS4_FONT_BIN;
    gui_font_mem_init(addr_font);

    // display text line create
    text = "0";
    int font_size = 24;
    text_display_up = gui_text_create(parent, "txt", 0, offset_y / 2 - 33, offset_x + del_x * 3 + 20,
                                      font_size);
    text_display_dn = gui_text_create(parent, "txt", 0, offset_y / 2 - 9, offset_x + del_x * 3 + 20,
                                      font_size);
    gui_text_set(text_display_dn, text, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text), font_size);
    gui_text_type_set(text_display_up, addr_font, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_display_up, RIGHT);
    gui_text_type_set(text_display_dn, addr_font, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_display_dn, RIGHT);

    for (i = 0; i < 21; i++)
    {
        if (i == 17) { continue; }

        // btn text config
        GUI_TYPE(gui_button_t, button[i])->text->font_height = 24;
        GUI_TYPE(gui_button_t, button[i])->text->path = 0;
        gui_text_type_set(GUI_TYPE(gui_button_t, button[i])->text, addr_font, FONT_SRC_MEMADDR);
        gui_text_mode_set(GUI_TYPE(gui_button_t, button[i])->text, LEFT);
        GET_BASE(GUI_TYPE(gui_button_t, button[i])->text)->x = 36;
        GET_BASE(GUI_TYPE(gui_button_t, button[i])->text)->y = 18;
        GUI_TYPE(gui_button_t, button[i])->text->content = btn_char + i;
        GUI_TYPE(gui_button_t, button[i])->text->len = 1;

        // setup btn callback
        gui_button_click(button[i], (gui_event_cb_t)cb_btn[i], "0"); // revised
    }

    // reset as ac
    cal_ac_proc();
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void *gui_calculator_create(void *parent,  const char *name,
                            int16_t x, int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    calculator_draw_button(parent);
    return NULL;
}

gui_app_t *get_app_calculator(void)
{
    return &calculator;
}

static void app_calculator_ui_design(gui_app_t *app)
{
    gui_win_t *win = gui_win_create(&app->screen, "back_win", 0, 0, gui_get_screen_width(),
                                    gui_get_screen_height());
    gui_calculator_create(&app->screen, "calculator", 0, 0, 454, 454);
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
