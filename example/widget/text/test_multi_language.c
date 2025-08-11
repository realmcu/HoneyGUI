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
    const char *localName;
    const char *chineseName;
    const char *englishName;
    const char *isoCode;
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
    { "Suomi",          "芬兰语",    "Finnish",                "fi", NULL },
    { "Українська",     "乌克兰语",   "Ukrainian",              "uk", NULL },
    { "עברית",          "希伯来语",  "Hebrew",                 "he", NULL },
    { "فارسی",          "波斯语",    "Persian",                "fa", NULL },
    { "Gaeilge",        "爱尔兰语",   "Irish",                  "ga", NULL },
    { "Slovenčina",     "斯洛伐克语", "Slovak",                "sk", NULL },
    { "Norsk",          "挪威语",     "Norwegian",              "no", NULL },
    { "Hrvatski",       "克罗地亚语", "Croatian",              "hr", NULL },
    { "Svenska",        "瑞典语",     "Swedish",                "sv", NULL },
    { "हिन्दी",           "印地语",     "Hindi",                  "hi", NULL }
};

#define FONT_SIZE 32
/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void create_font_list(void)
{
    for (int i = 0; i < sizeof(languages) / sizeof(languages[0]); i++)
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

static void note_design(gui_obj_t *obj, void *p)
{
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
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void text_multi_language_test(void)
{
    create_font_list();

    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, FONT_SIZE, 5, VERTICAL,
                                       note_design, NULL, true);
    gui_list_set_note_num(list, sizeof(languages) / sizeof(languages[0]));
}
