/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file test_multi_language.c
  * @brief Text multi language example
  * @details Test multi language feature of text widget
  * @author luke_sun@realsil.com.cn
  * @date 2025/08/11
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "gui_text.h"
#include "gui_list.h"
#include "test_font.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

typedef struct
{
    char *localName;
    char *chineseName;
    char *englishName;
    char *isoCode;
    void *fontfile;
} LanguageInfo;

/*============================================================================*
 *                           Constants
 *============================================================================*/

LanguageInfo languages[] =
{
    { "English",        "英语",      "English",                "en", NULL },
    { "中文",           "简体中文",  "Chinese",                "zh", NULL },
    { "中文（繁體）",    "繁体中文",  "Chinese (Traditional)",  "zh", NULL },
    { "Русский",        "俄语",      "Russian",                "ru", NULL },
    { "Deutsch",        "德语",      "German",                 "de", NULL },
    { "日本語",         "日语",      "Japanese",               "ja", NULL },
    { "Français",       "法语",      "French",                 "fr", NULL },
    { "Español",        "西班牙语",  "Spanish",                "es", NULL },
    { "Italiano",       "意大利语",  "Italian",                "it", NULL },
    { "Português",      "葡萄牙语",  "Portuguese",             "pt", NULL },
    { "한국어",          "韩语",     "Korean",                 "ko", NULL },
    { "Polski",         "波兰语",    "Polish",                 "pl", NULL },
    { "Bahasa Melayu",  "马来语",    "Malay",                  "ms", NULL },
    { "العربية",        "阿拉伯语",  "Arabic",                 "ar", NULL },
    { "ภาษาไทย",        "泰语",      "Thai",                   "th", NULL },
    { "Čeština",        "捷克语",    "Czech",                  "cs", NULL },
    { "Ελληνικά",       "希腊语",    "Greek",                  "el", NULL },
    { "Tiếng Việt",     "越南语",    "Vietnamese",             "vi", NULL },
    { "Türkçe",         "土耳其语",  "Turkish",                "tr", NULL },
    { "Română",         "罗马尼亚语", "Romanian",               "ro", NULL },
    { "Magyar",         "匈牙利语",   "Hungarian",              "hu", NULL },
    { "Dansk",          "丹麦语",     "Danish",                 "da", NULL },
    { "Suomi",          "芬兰语",    "Finnish",                 "fi", NULL },
    { "Українська",     "乌克兰语",   "Ukrainian",              "uk", NULL },
    { "עברית",          "希伯来语",  "Hebrew",                 "he", NULL },
    { "فارسی",          "波斯语",    "Persian",                "fa", NULL },
    { "Gaeilge",        "爱尔兰语",   "Irish",                  "ga", NULL },
    { "Slovenčina",     "斯洛伐克语", "Slovak",                 "sk", NULL },
    { "Norsk",          "挪威语",     "Norwegian",              "no", NULL },
    { "Hrvatski",       "克罗地亚语", "Croatian",               "hr", NULL },
    { "Svenska",        "瑞典语",     "Swedish",                "sv", NULL },
    { "हिन्दी",           "印地语",     "Hindi",                  "hi", NULL }
};

/*============================================================================*
 *                            Macros
 *============================================================================*/

#define LANGUAGE_COUNT (sizeof(languages)/sizeof(languages[0]))
#define FONT_SIZE 32

/*============================================================================*
 *                            Variables
 *============================================================================*/
gui_text_t *return_text;
gui_text_t *single_text;
gui_list_t *list;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void load_font_file_list(void)
{
    for (int i = 0; i < (int)LANGUAGE_COUNT; i++)
    {
        if (strcmp(languages[i].isoCode, "zh") == 0)
        {
            languages[i].fontfile = fontharmonysc;
        }
        else if (strcmp(languages[i].isoCode, "ja") == 0)
        {
            languages[i].fontfile = fontnotojp;
        }
        else if (strcmp(languages[i].isoCode, "ko") == 0)
        {
            languages[i].fontfile = fontnotokr;
        }
        else if (strcmp(languages[i].isoCode, "ar") == 0 || strcmp(languages[i].isoCode, "fa") == 0)
        {
            languages[i].fontfile = fontnotoarabic;
        }
        else if (strcmp(languages[i].isoCode, "th") == 0)
        {
            languages[i].fontfile = fontnotothai;
        }
        else if (strcmp(languages[i].isoCode, "he") == 0)
        {
            languages[i].fontfile = fontnotohebrew;
        }
        else
        {
            languages[i].fontfile = fontnoto;
        }
    }
}

static void arabic_render_test(void)
{
    char *test_text =
        "لكل شخص الحق في أن يلجأ إلى المحاكم الوطنية لإنصافه عن أعمال فيها اعتداء على الحقوق الأساسية التي يمنحها له القانون. لا يجوز القبض على أي إنسان أو حجزه أو نفيه تعسفاً. لكل إنسان الحق، على قدم المساواة التامة مع الآخرين، في أن تنظر قضيته أمام محكمة مستقلة نزيهة نظراً عادلاً علنياً للفصل في حقوقه والتزاماته وأية تهمة جنائية توجه له.";

    single_text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(single_text, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text),
                 FONT_SIZE);
    gui_text_type_set(single_text, fontnotoarabic, FONT_SRC_MEMADDR);
    gui_text_mode_set(single_text, RTL_MULTI_RIGHT);
}
static void english_render_test(void)
{
    char *test_text =
        "Everyone has the right to freedom of thought, conscience and religion; this right includes freedom to change his religion or belief, and freedom, either alone or in community with others and in public or private, to manifest his religion or belief in teaching, practice, worship and observance. ";

    single_text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(single_text, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text),
                 FONT_SIZE);
    gui_text_type_set(single_text, fontnoto, FONT_SRC_MEMADDR);
    gui_text_mode_set(single_text, MULTI_LEFT);
    gui_text_wordwrap_set(single_text, true);
}
static void chinese_render_test(void)
{
    char *test_text =
        "滕王阁序\n滕王高阁临江渚，佩玉鸣鸾罢歌舞。\n画栋朝飞南浦云，珠帘暮卷西山雨。\n闲云潭影日悠悠，物换星移几度秋。\n阁中帝子今何在？槛外长江空自流。\n滕王閣序\n滕王高閣臨江渚，佩玉鳴鸞罷歌舞。\n畫棟朝飛南浦雲，珠簾暮卷西山雨。\n閑雲潭影日悠悠，物換星移幾度秋。\n閣中帝子今何在？檻外長江空自流。";

    single_text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(single_text, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text),
                 FONT_SIZE);
    gui_text_type_set(single_text, fontharmonysc, FONT_SRC_MEMADDR);
    gui_text_mode_set(single_text, MULTI_CENTER);
}
static void thai_render_test(void)
{
    char *test_text =
        "โดยที่การยอมรับศักดิ์ศรีแต่กำเนิด และสิทธิที่เท่าเทียมกันและที่ไม่อาจเพิกถอนได้ของสมาชิกทั้งมวลแห่งครอบครัวมนุษยชาติ เป็นพื้นฐานแห่งอิสรภาพ ความยุติธรรม และสันติภาพในโลก การไม่นำพาและการหมิ่นในคุณค่าของสิทธิมนุษยชน ยังผลให้มีการกระทำอันป่าเถื่อน ซึ่งเป็นการขัดอย่างร้ายแรงต่อมโนธรรมของมนุษยชาติ";

    single_text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(single_text, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text),
                 FONT_SIZE);
    gui_text_type_set(single_text, fontnotothai, FONT_SRC_MEMADDR);
    gui_text_mode_set(single_text, MULTI_LEFT);
}
static void hebrew_render_test(void)
{
    char *test_text =
        "לא יהא אדם נתון להתערבות שרירותית בחייו הפרטיים, במשפחתו, במעונו, בחליפת מכתבים שלו ולא לפגיעה בכבודו או בשמו הפוב כל אדם זכאי להגנת החוק בפני התערבות או פגיעה באלה כל אדם זכאי לחירות המחשבה, המצפון והדת;";

    single_text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(single_text, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text),
                 FONT_SIZE);
    gui_text_type_set(single_text, fontnotohebrew, FONT_SRC_MEMADDR);
    gui_text_mode_set(single_text, RTL_MULTI_RIGHT);
}

static void return_language_list(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_hidden((gui_obj_t *)return_text, true);
    gui_obj_hidden((gui_obj_t *)list, false);
    gui_obj_tree_free((gui_obj_t *)single_text);
}

static void single_language_test(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    uint16_t index = ((gui_list_note_t *)obj)->index;
    char *isoCode = languages[index].isoCode;
    gui_obj_hidden((gui_obj_t *)list, true);
    gui_obj_hidden((gui_obj_t *)return_text, false);

    if (strcmp(isoCode, "ar") == 0)
    {
        arabic_render_test();
    }
    else if (strcmp(isoCode, "en") == 0)
    {
        english_render_test();
    }
    else if (strcmp(isoCode, "zh") == 0)
    {
        chinese_render_test();
    }
    else if (strcmp(isoCode, "th") == 0)
    {
        thai_render_test();
    }
    else if (strcmp(isoCode, "he") == 0)
    {
        hebrew_render_test();
    }
}

static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    uint16_t index = ((gui_list_note_t *)obj)->index;

    gui_text_t *localname = gui_text_create(obj, "text", 0, 0, 140, 0);
    gui_text_set(localname, languages[index].localName, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(languages[index].localName), FONT_SIZE);
    gui_text_type_set(localname, languages[index].fontfile, FONT_SRC_MEMADDR);

    gui_text_t *chinesename = gui_text_create(obj, "text", 145, 0, 115, 0);
    gui_text_set(chinesename, languages[index].chineseName, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(languages[index].chineseName), FONT_SIZE);
    gui_text_type_set(chinesename, fontharmonysc, FONT_SRC_MEMADDR);

    gui_text_t *englishname = gui_text_create(obj, "text", 265, 0, 170, 0);
    gui_text_set(englishname, languages[index].englishName, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(languages[index].englishName), FONT_SIZE);
    gui_text_type_set(englishname, fontnoto, FONT_SRC_MEMADDR);

    gui_text_t *isocode = gui_text_create(obj, "text", 440, 0, 40, 0);
    gui_text_set(isocode, languages[index].isoCode, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(languages[index].isoCode), FONT_SIZE);
    gui_text_type_set(isocode, fontnoto, FONT_SRC_MEMADDR);

    char *isoCode = languages[index].isoCode;
    if (strcmp(isoCode, "ar") == 0
        || strcmp(isoCode, "en") == 0
        || strcmp(isoCode, "zh") == 0
        || strcmp(isoCode, "th") == 0
        || strcmp(isoCode, "he") == 0)
    {
        gui_obj_add_event_cb(obj, single_language_test, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_text_color_set(localname, APP_COLOR_CRIMSON);
    }
}

static void create_return_text(void)
{
    return_text = gui_text_create(gui_obj_get_root(), "text", 0, 430, 0, 48);
    gui_text_set(return_text, "RETURN", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, 6, 48);
    gui_text_type_set(return_text, font32vb4, FONT_SRC_MEMADDR);
    gui_text_mode_set(return_text, RIGHT);
    gui_obj_add_event_cb(return_text, return_language_list, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_hidden((gui_obj_t *)return_text, true);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void text_multi_language_test(void)
{
    load_font_file_list();
    create_return_text();

    list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, FONT_SIZE, 5, VERTICAL, note_design,
                           NULL, true);
    gui_list_set_note_num(list, LANGUAGE_COUNT);
}
