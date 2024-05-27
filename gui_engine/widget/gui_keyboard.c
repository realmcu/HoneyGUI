/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_keyboard.c
  * @brief keyboard widget
  * @details keyboard widget
  * @author roy@realsil.com.cn
  * @date 2024/5/20
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
#include "font_mem.h"
#include <gui_img.h>

#include <gui_app.h>
#include "gui_button.h"
#include "gui_keyboard.h"
// #include "gui_img.h"
// #include "gui_button.h"
// #include "gui_switch.h"
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
#define KB_BUFF_INPUT_MAXLEN 128  //!<
#define KB_BUFF_DISPLAY_MAXLEN 128 //!<
/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */

static gui_kb_t *global_kb;
//static char buff_input[KB_BUFF_INPUT_MAXLEN];  // input debug
static char buff_display[KB_BUFF_DISPLAY_MAXLEN];
static char buff_txt_display[KB_BUFF_DISPLAY_MAXLEN + 4];

static uint16_t len_display = 0;
static char *cursor; // point to buff_display


void gui_keyboard_set_mode(gui_kb_t *this, uint8_t mode);

gui_kb_t *gui_get_kb(void)
{
    return global_kb;
}

static char *gui_kb_get_cursor(void)
{
    return cursor;
}
static bool gui_kb_cursor_is_head(void)
{
    return (cursor == buff_display);
}
static bool gui_kb_cursor_is_tail(void)
{
    return ((cursor - buff_display) == len_display);
}
static void gui_kb_cursor_ls(void)
{
    if (cursor <= buff_display)
    {
        cursor = buff_display;
    }
    else
    {
        cursor --;
    }
}
static void gui_kb_cursor_rs(void)
{
    if ((cursor - buff_display) >= len_display)
    {
        cursor = buff_display + len_display;
    }
    else
    {
        cursor ++;
    }
}



static bool buff_display_isempty(void)
{
    if ((len_display == 0))
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
    uint16_t len = len_display;
    if (len < (KB_BUFF_DISPLAY_MAXLEN - 1))
    {
        buff_display[len] = ch;
        buff_display[len + 1] = '\0';
        len_display++;
        gui_kb_cursor_rs();
        return 0;
    }
    return 1;
}

static void buff_display_pop(void)
{
    uint16_t len = len_display;

    if (buff_display_isempty())
    {
        return;
    }

    len_display--;
    buff_display[len - 1] = '\0';
    gui_kb_cursor_ls();
}

//static uint8_t buff_display_top(void)
//{
//    uint16_t len = len_display;

//    return buff_display[len - 1];
//}

//static void buff_display_clean(void)
//{
//    buff_display[0] = '\0';
//    len_display = 0;
//}

static uint16_t gui_kb_cursor_len(void)
{
    return (len_display - (cursor - buff_display));
}
static void gui_kb_cursor_set_tail(void)
{
    cursor = buff_display + len_display;
}
static void gui_kb_cursor_insert(char ch)
{
    if (gui_kb_cursor_is_tail())
    {
        buff_display_push(ch);
    }
    else
    {
        if (len_display + 1 < KB_BUFF_DISPLAY_MAXLEN - 1)
        {
            uint16_t len = gui_kb_cursor_len();
            memmove(cursor + 1, cursor, len + 1);
            *cursor = ch;
            len_display++;
            cursor++;
        }
    }
}
static void gui_kb_cursor_del()
{
    if (gui_kb_cursor_is_tail())
    {
        buff_display_pop();
    }
    else
    {
        if (!gui_kb_cursor_is_head())
        {
            uint16_t len = gui_kb_cursor_len();
            memmove(cursor - 1, cursor, len + 1);
            len_display--;
            cursor--;
        }
    }
}
static void gui_kb_prepare(gui_obj_t *obj)
{}
static void gui_kb_draw_cb(gui_obj_t *obj)
{}
static void gui_kb_end(gui_obj_t *obj)
{}
static void gui_kb_destory(gui_obj_t *obj)
{}

static void gui_kb_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_kb_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_kb_draw_cb(obj);
            break;

        case OBJ_END:
            gui_kb_end(obj);
            break;

        case OBJ_DESTORY:
            gui_kb_destory(obj);
            break;

        default:
            break;
        }
    }
}

static bool key_is_sw(uint32_t key_value)
{
    uint32_t idx = KB_VALUE_IDX(key_value);
    uint32_t area = KB_VALUE_AREA(key_value);

    if (area == KB_AREA_FUNC)
    {
        if ((idx == KB_SW_IDX_CAP) || (idx == KB_SW_IDX_SYMBOL) || (idx == KB_SW_IDX_NUMBER) ||
            (idx == KB_SW_IDX_CNEN) || (idx == KB_SW_IDX_LOCK))
        {
            return true;
        }
    }
    return false;
}

static uint8_t kb2letter_index(uint8_t key_idx)
{
    char table[26] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm'};
    return (table[key_idx] - 'a');
}
static uint8_t kb2number_index(uint8_t key_idx)
{
    char table[10] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    return (table[key_idx] - '0');
}

static void *get_key_img(gui_kb_t *this, uint32_t key_value, uint8_t type)
{
    void *data = NULL;
    uint32_t idx = KB_VALUE_IDX(key_value);
    uint32_t area = KB_VALUE_AREA(key_value);
    uint16_t offset = 0;

    if (area == KB_AREA_LETTER)
    {
        uint8_t letter_idx = kb2letter_index(idx);

        //   KB_SRC_LOWWER     = 0,
        //   KB_SRC_LOWWER_CLK = 1,
        //   KB_SRC_UPPER      = 2,
        //   KB_SRC_UPPER_CLK  = 3,
        offset = (type > KB_SRC_LOWWER_CLK) ? 2 * 26 : 0;
        idx = 2 * letter_idx + (type & 0x01);
    }
    else if (area == KB_AREA_NUM)
    {
        uint8_t num_idx = kb2number_index(idx);

        offset = 4 * 26;
        idx = 2 * num_idx + (type & 0x01);
    }
    else if (area == KB_AREA_SYMBOL)
    {
        offset = 4 * 26 + 2 * 10;
        idx = 2 * idx + (type & 0x01);
    }
    else if (area == KB_AREA_FUNC)
    {
        offset = this->file_mark[0];
        idx = 2 * idx + (type & 0x01);
    }

    data = this->img_array[idx + offset];
    return data;
}

static void *get_other_img(gui_kb_t *this, uint8_t img_idx)
{
    void *data = NULL;
    uint32_t idx = img_idx;
    uint16_t offset = this->file_mark[1];

    data = this->img_array[idx + offset];
    return data;
}

static void get_letter_location(uint8_t key_idx, uint8_t *cor_x, uint8_t *cor_y)
{
    //  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
    char table_x[26] = {0, 4, 2, 2, 2, 3, 4, 5, 7, 6, 7, 8, 6, 5, 8, 9, 0, 3, 1, 4, 6, 3, 1, 1, 5, 0};
    char table_y[26] = {1, 2, 2, 1, 0, 1, 1, 1, 0, 1, 1, 1, 2, 2, 0, 0, 0, 0, 1, 0, 0, 2, 0, 2, 0, 2};

    *cor_x = table_x[key_idx];
    *cor_y = table_y[key_idx];
}
static void get_func_location(uint8_t key_idx, uint8_t *cor_x, uint8_t *cor_y)
{
    //CAP,DL,SY,123, L,SP, R,CE,DN, ,,ET,LK
    char table_x[KB_BASIC_FUNC_NUM] = {0, 6, 0, 1, 2, 3, 4, 5, 6, 0, 5, 0};
    char table_y[KB_BASIC_FUNC_NUM] = {2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 3, 2};

    *cor_x = table_x[key_idx];
    *cor_y = table_y[key_idx];
}
static void get_number_location(uint8_t key_idx, uint8_t *cor_x, uint8_t *cor_y)
{
    //  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    char table_x[10] = {3, 0, 1, 2, 0, 1, 2, 0, 1, 2};
    char table_y[10] = {2, 0, 0, 0, 1, 1, 1, 2, 2, 2};

    *cor_x = table_x[key_idx];
    *cor_y = table_y[key_idx];
}
static void get_symbol_location(uint8_t key_idx, uint8_t *cor_x, uint8_t *cor_y)
{
    //  ,, :, ?, ~,
    char table_x[15] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 3, 4, 5, 3, 4, 5};
    char table_y[15] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 0, 0, 0, 1, 1, 1};

    *cor_x = table_x[key_idx];
    *cor_y = table_y[key_idx];
}

static bool gui_kb_is_func(uint32_t key_val)
{
    uint8_t area = KB_VALUE_AREA(key_val);
    uint32_t idx = KB_VALUE_IDX(key_val);

    if (area == KB_AREA_FUNC)
    {
        if ((idx != KB_BTN_IDX_SP) && (idx != KB_BTN_IDX_COMIC) && (idx != KB_BTN_IDX_ENTER))
        {
            return true;
        }
    }
    return false;
}

// static gui_obj_t *gui_kb_get_subobj(gui_obj_t *win, uint16_t obj_idx)
// {
//     gui_obj_t *obj = NULL;
//     uint16_t idx = 0;
//     gui_list_t *node = NULL;

//     gui_list_for_each(node, &win->child_list)
//     {
//         gui_obj_t *obj_item = gui_list_entry(node, gui_obj_t, brother_list);
//         // gui_log("%s type: 0x%x\n", obj_item->name, obj_item->type);
//         if(idx == obj_idx)
//         {
//             obj = obj_item;
//             break;
//         }
//         idx++;
//     }
//     return obj;
// }

static void gui_basic_func_proc(uint32_t key_val)
{
    gui_kb_t *this = gui_get_kb();
    uint32_t kb_idx = KB_VALUE_IDX(key_val);
    uint32_t idx = kb_idx;
    uint8_t mode = this->mode;

    // KB_SW_IDX_CAP = 0,

    //   KB_BTN_IDX_DEL,
    //   KB_SW_IDX_SYMBOL,
    //   KB_SW_IDX_NUMBER,
    //   KB_BTN_IDX_LS,    // Left shift
    //   KB_BTN_IDX_SP,    x
    //   KB_BTN_IDX_RS,    // Right shift
    //   KB_SW_IDX_CNEN,
    //   KB_BTN_IDX_DONE,

    //   KB_BTN_IDX_COMIC,    x
    //   KB_BTN_IDX_ENTER,    x

    //   KB_SW_IDX_LOCK,
    switch (idx)
    {
    case KB_SW_IDX_CAP:
        {
            this->caps_Lk = !this->caps_Lk;
            if (this->caps_Lk)
            {
                gui_keyboard_set_mode(this, KB_MODE_BASIC_ENG_UPPER);
            }
            else
            {
                gui_keyboard_set_mode(this, KB_MODE_BASIC_ENG_LOWWER);
            }
        }
        break;
    case KB_BTN_IDX_DEL:
        {
            // buff_display_pop();
            gui_kb_cursor_del();
        }
        break;
    case KB_SW_IDX_SYMBOL:
        {
            if ((mode != KB_MODE_BASIC_SYMBOL_EN) && (mode != KB_MODE_BASIC_SYMBOL_CN))
            {
                // symbol
                this->symbol_Lk = false;
                gui_keyboard_set_mode(this, KB_MODE_BASIC_SYMBOL_EN);
            }
            else
            {
                // abc
                gui_keyboard_set_mode(this, KB_MODE_BASIC_ENG_LOWWER);
            }
        }
        break;
    case KB_SW_IDX_NUMBER:
        {
            if ((mode != KB_MODE_BASIC_NUM))
            {
                gui_keyboard_set_mode(this, KB_MODE_BASIC_NUM);
            }
            else
            {
                if (this->last_mode == KB_MODE_BASIC_NUM)
                {
                    this->last_mode = KB_MODE_BASIC_ENG_LOWWER;
                }
                gui_keyboard_set_mode(this, this->last_mode);
            }
        }
        break;
    case KB_BTN_IDX_LS:
        {
            gui_kb_cursor_ls();
        }
        break;
    case KB_BTN_IDX_RS:
        {
            gui_kb_cursor_rs();
        }
        break;
    case KB_SW_IDX_CNEN:
        {
            if ((mode == KB_MODE_BASIC_SYMBOL_EN))
            {
                // symbol
                gui_keyboard_set_mode(this, KB_MODE_BASIC_SYMBOL_CN);
            }
            else if ((mode == KB_MODE_BASIC_SYMBOL_CN))
            {
                // symbol
                gui_keyboard_set_mode(this, KB_MODE_BASIC_SYMBOL_EN);
            }
            else if ((mode == KB_MODE_BASIC_PY))
            {
                // pinyin to abc
                gui_keyboard_set_mode(this, KB_MODE_BASIC_ENG_LOWWER);
            }
            else if ((mode == KB_MODE_BASIC_ENG_LOWWER) || (mode == KB_MODE_BASIC_ENG_UPPER))
            {
                // abc to pinyin
                gui_keyboard_set_mode(this, KB_MODE_BASIC_PY);
            }
        }
        break;
    case KB_BTN_IDX_DONE:
        {
            gui_text_t *txt = this->txt_input;
            uint32_t size = len_display;
            uint8_t *str = gui_malloc(size + 1);
            if (str)
            {
                memcpy(str, buff_display, size);
                str[size] = '\0';
                gui_text_content_set(txt, (void *)str, size);
            }
            gui_obj_show(this, false);

        }
        break;
    case KB_SW_IDX_LOCK:
        {
            this->symbol_Lk = !this->symbol_Lk;
        }
        break;
    }


}

static char *gui_kb_basic_symbol[] = {",.-!~:%#+&/@*\"|\\_[]{}<>()?",
                                      ",.?!",
                                      ",:?~@.(!)-=+%#*"
                                     };

static char gui_get_basic_char(uint32_t key_val)
{
    gui_kb_t *this = gui_get_kb();
    uint8_t  kb_area = KB_VALUE_AREA(key_val);
    uint32_t kb_idx = KB_VALUE_IDX(key_val);
    uint32_t idx = kb2letter_index(kb_idx);
    char ch = '\0';
    uint8_t mode = this->mode;

    if (kb_area == KB_AREA_FUNC)
    {
        if (kb_idx == KB_BTN_IDX_SP)
        {
            ch = ' ';
        }
        else if (kb_idx == KB_BTN_IDX_COMIC)
        {
            // TODO: CN
            ch = ',';
        }
        else if (kb_idx == KB_BTN_IDX_ENTER)
        {
            ch = '\n';
        }
    }
    else
    {
        switch (mode)
        {
        case KB_MODE_BASIC_ENG_LOWWER:
        case KB_MODE_BASIC_PY:
            {
                ch = 'a' + idx;
            }
            break;
        case KB_MODE_BASIC_ENG_UPPER:
            {
                ch = 'A' + idx;
            }
            break;
        case KB_MODE_BASIC_SYMBOL_EN:
            {
                ch = gui_kb_basic_symbol[0][kb_idx];
                if (!this->symbol_Lk)
                {
                    gui_keyboard_set_mode(this, this->last_mode);
                }
            }
            break;
        case KB_MODE_BASIC_SYMBOL_CN:
            {
                ch = gui_kb_basic_symbol[1][kb_idx];
                if (!this->symbol_Lk)
                {
                    gui_keyboard_set_mode(this, this->last_mode);
                }
            }
            break;
        case KB_MODE_BASIC_NUM:
            {
                if (kb_area == KB_AREA_NUM)
                {
                    idx = kb2number_index(kb_idx);
                    ch = '0' + idx;
                }
                else
                {
                    ch = gui_kb_basic_symbol[2][kb_idx];
                }
            }
            break;
        }
    }

    return ch;
}

static void gui_kb_input_func_proc(uint32_t key_val)
{
    gui_kb_t *this = gui_get_kb();

    if (this->layout == KB_LAYOUT_BASIC)
    {
        gui_basic_func_proc(key_val);
    }

}
static void gui_kb_input_char_proc(uint32_t key_val)
{
    gui_kb_t *this = gui_get_kb();
    char ch = '\0';

    if (this->layout == KB_LAYOUT_BASIC)
    {
        ch = gui_get_basic_char(key_val);
    }

    // buff_display_push(ch);
    gui_kb_cursor_insert(ch);
}


static void gui_kb_cursor_animate(void *p)
{
    gui_kb_t *this = gui_get_kb();
    char *cur = gui_kb_get_cursor();
    uint16_t len_bf = cur - buff_display;
    char *pcontent = (char *)(this->txt_display->content);
    static uint8_t i = 0;

    i++;
    // printf(">%c\n", *(char*)cur);

    memset(pcontent, 0, KB_BUFF_DISPLAY_MAXLEN);
    if (len_bf)
    {
        memcpy(pcontent, buff_display, len_bf);
    }
    pcontent[len_bf] = (i & 0x01) ? '_' : ' ';
    memcpy(pcontent + len_bf + 1, cur, len_display - len_bf);
    this->txt_display->len = len_display + 1;
}

static void gui_kb_update_display(void)
{
    // gui_kb_t *this = gui_get_kb();

    // memset(this->txt_display->content, 0, KB_BUFF_DISPLAY_MAXLEN);
    // memcpy(this->txt_display->content, buff_display, len_display);
    // this->txt_display->len = len_display;
    gui_kb_cursor_animate(NULL);
}
static void gui_kb_input_proc(void *obj, gui_event_t event_code, void *param)
{
    gui_obj_t *this = (void *)obj;
    T_OBJ_TYPE type = this->type;
    size_t key_val = (size_t)param;

    gui_log("0x%x type: 0x%x, key: 0x%x %d %d\n", obj, type, key_val, KB_VALUE_AREA(key_val),
            KB_VALUE_IDX(key_val));
    // gui_log(">%s\n", buff_input);

    if (gui_kb_is_func(key_val))
    {
        gui_kb_input_func_proc(key_val);
    }
    else
    {
        gui_kb_input_char_proc(key_val);
    }

    // update display
    // printf(">%s< %d, cur: %d-%c-\n", buff_display, len_display, cursor-buff_display, *cursor);
    gui_kb_update_display();
}




static void gui_kb_set_func_mode(gui_kb_t *this, uint8_t mode)
{
    gui_obj_t *win = (gui_obj_t *)this->win_func;
    gui_obj_t *obj_func[KB_BASIC_FUNC_NUM];
    // KB_MODE_BASIC_ENG_LOWWER,
    // KB_MODE_BASIC_ENG_UPPER,
    // KB_MODE_BASIC_PY,
    // KB_MODE_BASIC_NUM,
    // KB_MODE_BASIC_SYMBOL_EN,
    // KB_MODE_BASIC_SYMBOL_CN,
    gui_list_t *node = NULL;
    uint16_t idx = KB_SW_IDX_CAP;
    gui_list_for_each(node, &win->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        // gui_log("%s type: 0x%x\n", obj->name, obj->type);
        obj_func[idx] = obj;
        idx++;
    }

    switch (mode)
    {
    case KB_MODE_BASIC_ENG_LOWWER:
        {
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], false);
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], false);
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], false);  // symbol
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], false);  // number
            gui_obj_show(obj_func[KB_SW_IDX_CAP], true);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], true);
        }
        break;
    case KB_MODE_BASIC_ENG_UPPER:
        {
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], false);
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], false);
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], false);  // symbol
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], false);  // number
            gui_obj_show(obj_func[KB_SW_IDX_CAP], true);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], true);
        }
        break;
    case KB_MODE_BASIC_PY:
        {
            gui_obj_show(obj_func[KB_SW_IDX_CAP], false);
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], false);
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], false);  // symbol
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], false);  // number
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], true);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], true);
        }
        break;
    case KB_MODE_BASIC_NUM:
        {
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], false);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], false);
            gui_obj_show(obj_func[KB_SW_IDX_CAP], false);
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], false);  // symbol
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], true);  // back
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], true);
        }
        break;
    case KB_MODE_BASIC_SYMBOL_EN:
        {
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], false);
            gui_obj_show(obj_func[KB_SW_IDX_CAP], false);
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], true);  // abc
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], false);  // number
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], true);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], true);
        }
        break;
    case KB_MODE_BASIC_SYMBOL_CN:
        {
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], false);
            gui_obj_show(obj_func[KB_SW_IDX_CAP], false);
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], true);  // abc
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], false);  // number
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], true);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], true);
        }
        break;
    }
    // 5 sw
    gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_CAP], this->caps_Lk);
    gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_LOCK], this->symbol_Lk);
    gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_CNEN], this->is_cn);

    gui_obj_show(obj_func[KB_BTN_IDX_DEL], true);
    gui_obj_show(obj_func[KB_SW_IDX_SYMBOL], true);
    gui_obj_show(obj_func[KB_SW_IDX_NUMBER], true);
    gui_obj_show(obj_func[KB_BTN_IDX_LS], true);
    gui_obj_show(obj_func[KB_BTN_IDX_SP], true);
    gui_obj_show(obj_func[KB_BTN_IDX_RS], true);
    gui_obj_show(obj_func[KB_BTN_IDX_DONE], true);
}


static void gui_kb_set_letter_mode(gui_kb_t *this, uint8_t mode)
{
    gui_obj_t *win = (gui_obj_t *)this->win_letter;
    void *img_default = NULL, *img_clk = NULL;
    gui_obj_t *obj_list[26];
    uint32_t key_val = 0;
    uint16_t i = 0;

    gui_list_t *node = NULL;
    uint16_t idx = 0;
    gui_list_for_each(node, &win->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        // gui_log("%s type: 0x%x\n", obj->name, obj->type);
        obj_list[idx] = obj;
        idx++;
    }

    switch (mode)
    {
    case KB_MODE_BASIC_ENG_LOWWER:
    case KB_MODE_BASIC_PY:
        {
            for (i = 0; i < 26; i++)
            {
                key_val = KB_VALUE_PACK(KB_AREA_LETTER, i);
                img_default = get_key_img(this, key_val, KB_SRC_LOWWER);
                img_clk = get_key_img(this, key_val, KB_SRC_LOWWER_CLK);
                gui_button_set_img((gui_button_t *)obj_list[i], img_default, img_clk);
            }
        }
        break;
    case KB_MODE_BASIC_ENG_UPPER:
        {
            for (i = 0; i < 26; i++)
            {
                key_val = KB_VALUE_PACK(KB_AREA_LETTER, i);
                img_default = get_key_img(this, key_val, KB_SRC_UPPER);
                img_clk = get_key_img(this, key_val, KB_SRC_UPPER_CLK);
                gui_button_set_img((gui_button_t *)obj_list[i], img_default, img_clk);
            }
        }
        break;
    case KB_MODE_BASIC_SYMBOL_EN:
        {
            for (i = 0; i < 26; i++)
            {
                key_val = KB_VALUE_PACK(KB_AREA_SYMBOL, 15 + i);
                img_default = get_key_img(this, key_val, KB_SRC_DEFAULT);
                img_clk = get_key_img(this, key_val, KB_SRC_CLK);
                gui_button_set_img((gui_button_t *)obj_list[i], img_default, img_clk);
            }
        }
        break;
    case KB_MODE_BASIC_SYMBOL_CN:
        {
            for (i = 0; i < 26; i++)
            {
                key_val = KB_VALUE_PACK(KB_AREA_SYMBOL, (15 + 26) + i);
                img_default = get_key_img(this, key_val, KB_SRC_DEFAULT);
                img_clk = get_key_img(this, key_val, KB_SRC_CLK);
                gui_button_set_img((gui_button_t *)obj_list[i], img_default, img_clk);
            }
        }
        break;
    }
}


static void gui_kb_mode_switch(gui_kb_t *this, uint8_t mode)
{
    if (((mode == KB_MODE_BASIC_SYMBOL_CN) && (this->mode == KB_MODE_BASIC_SYMBOL_EN)) \
        || ((mode == KB_MODE_BASIC_SYMBOL_EN) && (this->mode == KB_MODE_BASIC_SYMBOL_CN))
        || ((mode == KB_MODE_BASIC_NUM) && (this->mode == KB_MODE_BASIC_SYMBOL_EN))
        || ((mode == KB_MODE_BASIC_NUM) && (this->mode == KB_MODE_BASIC_SYMBOL_CN)))
    {

    }
    else
    {
        this->last_mode = this->mode;
    }
    this->mode = mode;
    this->caps_Lk = false;
    this->is_cn = false;
    gui_log("last: %d\n", this->last_mode);

    if ((mode == KB_MODE_BASIC_SYMBOL_CN) || (mode == KB_MODE_BASIC_PY))
    {
        this->is_cn = true;
    }
    else if ((mode == KB_MODE_BASIC_ENG_UPPER))
    {
        this->caps_Lk = true;
    }

    switch (mode)
    {
    case KB_MODE_BASIC_ENG_LOWWER:
    case KB_MODE_BASIC_ENG_UPPER:
    case KB_MODE_BASIC_PY:
    case KB_MODE_BASIC_SYMBOL_EN:
    case KB_MODE_BASIC_SYMBOL_CN:
        {
            gui_kb_set_letter_mode(this, mode);
            gui_obj_show(this->win_num, false);
            gui_obj_show(this->win_letter, true);
        }
        break;
    case KB_MODE_BASIC_NUM:
        {
            gui_obj_show(this->win_letter, false);
            gui_obj_show(this->win_num, true);
        }
        break;

    }

    gui_kb_set_func_mode(this, mode);
    gui_obj_show(this->win_func, true);

}
static void gui_kb_layout_basic_letter(gui_kb_t  *this)
{
    int16_t x = this->base.x, y = this->base.y, w = this->base.w, h = this->base.h;
    uint16_t i = 0;
    gui_button_t *btn = NULL;


    // location calculate
    // 3 row
    int16_t y_start = 120;
    float y_step = 59;
    float x_letter_step = 61;
    float x_letter_start[3] = {19, 50, 111};

    this->win_letter = gui_win_create(this, "WIN_letter",
                                      x, y, w, h);

    // create 26 letter like layout button
    for (i = 0; i < 26; i++)
    {
        int16_t obj_x = 0, obj_y = 0;
        int16_t obj_w = 0, obj_h = 0;
        void *img_default = NULL, *img_clk = NULL;
        // 1: 10, 2: 9, 3: 7
        uint8_t cor_x = 0, cor_y = 0;
        uint8_t letter_idx = kb2letter_index(i);
        size_t key_val = KB_VALUE_PACK(KB_AREA_LETTER, i);

        get_letter_location(letter_idx, &cor_x, &cor_y);
        obj_x = x_letter_start[cor_y] + x_letter_step * cor_x;
        obj_y = y_start + cor_y * y_step;
        img_default = get_key_img(this, key_val, KB_SRC_LOWWER);
        img_clk = get_key_img(this, key_val, KB_SRC_LOWWER_CLK);
        obj_w = ((gui_rgb_data_head_t *)img_default)->w;
        obj_h = ((gui_rgb_data_head_t *)img_default)->h;


        btn = gui_button_create(this->win_letter, obj_x, obj_y, obj_w, obj_h, img_default, img_clk, "",
                                BUTTON_BG_ICON, 0);
        gui_button_click(btn, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);

    }
}

static void gui_kb_layout_basic_func(gui_kb_t  *this)
{
    int16_t x = this->base.x, y = this->base.y, w = this->base.w, h = this->base.h;
    uint16_t i = 0;
    gui_button_t *btn = NULL;
    gui_switch_t *sw = NULL;

    // location calculate
    // 2 row
    int16_t y_start = 120;
    float y_step = 59;
    float x_pos[] = {19, 111, 203, 264, 384, 445, 537};

    this->win_func = gui_win_create(this, "WIN_func",
                                    x, y, w, h);

    // create 11 func like layout obj: 7 button, 4 switch KB_BASIC_FUNC_NUM
    for (i = 0; i < KB_BASIC_FUNC_NUM; i++)
    {
        int16_t obj_x = 0, obj_y = 0;
        int16_t obj_w = 0, obj_h = 0;
        size_t key_val = KB_VALUE_PACK(KB_AREA_FUNC, i);
        void *img_default = NULL, *img_clk = NULL;
        // 1: 10, 2: 9, 3: 7
        uint8_t cor_x = 0, cor_y = 0;

        get_func_location(i, &cor_x, &cor_y);
        obj_x = x_pos[cor_x] ;
        obj_y = y_start + cor_y * y_step;

        img_default = get_key_img(this, key_val, KB_SRC_DEFAULT);
        img_clk = get_key_img(this, key_val, KB_SRC_CLK);
        obj_w = ((gui_rgb_data_head_t *)img_default)->w;
        obj_h = ((gui_rgb_data_head_t *)img_default)->h;

        if (key_is_sw(key_val) == false)
        {
            btn = gui_button_create(this->win_func, obj_x, obj_y, obj_w, obj_h, img_default, img_clk, "",
                                    BUTTON_BG_ICON, 0);
            gui_button_click(btn, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);
        }
        else
        {
            sw = gui_switch_create(this->win_func, obj_x, obj_y, obj_w, obj_h, img_default, img_clk);
            GUI_API(gui_switch_t).on_turn_on(sw, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);
            GUI_API(gui_switch_t).on_turn_off(sw, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);
        }
    }
}


static void gui_kb_layout_basic_number(gui_kb_t  *this)
{
    int16_t x = this->base.x, y = this->base.y, w = this->base.w, h = this->base.h;
    uint16_t i = 0;
    gui_button_t *btn = NULL;


    // location calculate
    // 3 row
    int16_t y_start = 120;



    this->win_num = gui_win_create(this, "WIN_num",
                                   x, y, w, h);
    // TODO: grid img

    {
        float y_step = 59;
        float x_pos[] = {203, 284, 365, 445};
        // create 10 number like layout button
        for (i = 0; i < 10; i++)
        {
            int16_t obj_x = 0, obj_y = 0;
            int16_t obj_w = 0, obj_h = 0;
            void *img_default = NULL, *img_clk = NULL;
            // 1: 3, 2: 3, 3: 4
            uint8_t cor_x = 0, cor_y = 0;
            uint8_t number_idx = kb2number_index(i);
            size_t key_val = KB_VALUE_PACK(KB_AREA_NUM, i);

            get_number_location(number_idx, &cor_x, &cor_y);
            obj_x = x_pos[cor_x];
            obj_y = y_start + cor_y * y_step;
            img_default = get_key_img(this, key_val, KB_SRC_DEFAULT);
            img_clk = get_key_img(this, key_val, KB_SRC_CLK);
            obj_w = ((gui_rgb_data_head_t *)img_default)->w;
            obj_h = ((gui_rgb_data_head_t *)img_default)->h;

            btn = gui_button_create(this->win_num, obj_x, obj_y, obj_w, obj_h, img_default, img_clk, " ",
                                    BUTTON_BG_ICON, 0);
            gui_button_click(btn, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);

        }
    }

    {
        float y_step = 56;
        float x_pos[] = {19, 77, 135, 445, 503, 561};
        // create 15 number symbol layout button
        for (i = 0; i < 15; i++)
        {
            int16_t obj_x = 0, obj_y = 0;
            int16_t obj_w = 0, obj_h = 0;
            void *img_default = NULL, *img_clk = NULL;
            // 1: 3, 2: 3, 3: 4
            uint8_t cor_x = 0, cor_y = 0;
            // uint8_t symbol_idx = kb2number_index(i);
            size_t key_val = KB_VALUE_PACK(KB_AREA_SYMBOL, i);

            get_symbol_location(i, &cor_x, &cor_y);
            obj_x = x_pos[cor_x];
            obj_y = y_start + cor_y * y_step;
            img_default = get_key_img(this, key_val, KB_SRC_DEFAULT);
            img_clk = get_key_img(this, key_val, KB_SRC_CLK);
            obj_w = ((gui_rgb_data_head_t *)img_default)->w;
            obj_h = ((gui_rgb_data_head_t *)img_default)->h;

            btn = gui_button_create(this->win_num, obj_x, obj_y, obj_w, obj_h, img_default, img_clk, " ",
                                    BUTTON_BG_ICON, 0);
            gui_button_click(btn, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);

        }
    }

}

static void gui_kb_layout_basic_ctor(gui_kb_t  *this, gui_kb_config_t  *config)
{
    // letters
    gui_kb_layout_basic_letter(this);
    gui_kb_layout_basic_func(this);
    gui_kb_layout_basic_number(this);

    // default
    gui_kb_mode_switch(this, KB_MODE_BASIC_SYMBOL_CN);
}

static void gui_kb_ctor(gui_kb_t                        *this,
                        gui_obj_t                 *parent,
                        const char                *name,
                        gui_kb_config_t            *config,
                        int16_t                    x,
                        int16_t                    y,
                        int16_t                    w,
                        int16_t                    h)
{
    gui_obj_t *root = (gui_obj_t *)this;
    void *img_data = NULL;
//    char *path;
//    gui_img_t *img = NULL;
//    int16_t indx = 0;

//    int16_t img_w = 0, img_h = 0;

    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->type = KEYBOARD;
    root->obj_cb = gui_kb_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    // keyboard
    this->layout = config->layout;
    this->mode = config->mode;
    this->img_array = config->img_array;
    this->num_pic = config->num_pic;
    this->file_mark[0] = config->file_mark[0];
    this->file_mark[1] = config->file_mark[1];

    img_data = get_other_img(this, KB_IMG_IDX_BG);
    this->img_bg = gui_img_create_from_mem(this, "KB_BG",
                                           img_data,
                                           0, 0, w, h);
    gui_img_set_mode(this->img_bg, IMG_BYPASS_MODE);

    //
    if (this->layout == KB_LAYOUT_BASIC)
    {
        gui_kb_layout_basic_ctor(this, config);
    }



    img_data = get_other_img(this, KB_IMG_IDX_INPUTBOX);
    this->img_box = gui_img_create_from_mem(this, "KB_BOX",
                                            img_data,
                                            19, 17, w, h);
    gui_img_set_mode(this->img_box, IMG_BYPASS_MODE);

    char *text = buff_txt_display;

    this->txt_display = gui_text_create(this, "icon_text", 24, 26,
                                        w, h - 60);
    gui_text_set(this->txt_display, text, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text), 32);
    gui_text_mode_set(this->txt_display, LEFT);
    {
        int font_size = 30;
        gui_text_set(this->txt_display, text, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                     strlen(text), font_size);
        void *addr1 = gui_get_file_address("app/system/resource/font/calibri_size30_bits4_font.bin");
        gui_font_mem_init(addr1);
        gui_text_type_set(this->txt_display, addr1);
    }
    gui_obj_show(this, false);
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_keyboard_set_mode(gui_kb_t *this, uint8_t mode)
{
    // this->mode = mode;
    if (mode != this->mode)
    {
        gui_kb_mode_switch(this, mode);
    }
}

void gui_keyboard_launch(gui_obj_t *obj)
{
    gui_kb_t *this = global_kb;
    if (this)
    {
        this->txt_input = (gui_text_t *)obj;

        memset(buff_display, 0, KB_BUFF_DISPLAY_MAXLEN);
        len_display = 0;
        memcpy(buff_display, this->txt_input->content, this->txt_input->len);
        len_display = this->txt_input->len;
        gui_kb_cursor_set_tail();
        gui_kb_update_display();

        gui_text_set_animate(this->txt_display,
                             500,
                             -1,
                             gui_kb_cursor_animate,
                             gui_kb_get_cursor());


        gui_keyboard_set_mode(this, KB_MODE_BASIC_ENG_LOWWER);
        gui_obj_show(this, true);

    }
    else
    {
        gui_log("Keyboard not found !\n");
    }
}


gui_kb_t *gui_keyboard_create(void                    *parent,
                              const char                *name,
                              gui_kb_config_t           *cofig,
                              int16_t                    x,
                              int16_t                    y,
                              int16_t                    w,
                              int16_t                    h)
{
    gui_kb_t *this;

    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_GLRY";
    }

    this = gui_malloc(sizeof(gui_kb_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_kb_t));

    gui_kb_ctor(this, (gui_obj_t *)parent, name, cofig, x, y, w, h);

    GET_BASE(this)->create_done = true;
    global_kb = this;
    return this;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
