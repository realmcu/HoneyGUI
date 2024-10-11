#include "gui_canvas.h"
#include "gui_canvas_rect.h"
#include <ctime>
#include <string>
#include "gui_obj.h"
#include "gui_text.h"
#include "../root_image/ui_resource.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include <iostream>
float cellWidth ;
float cellHeight;
int currentDay, currentMonth, currentYear;
char buffer[32];
float margin = 20;
float headerHeight = 50;
gui_text_t *title;

void getCurrentDate(int &day, int &month, int &year)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    day = ltm->tm_mday;
    month = 1 + ltm->tm_mon; // NanoVG月份是从1开始
    year = 1900 + ltm->tm_year;
}

void getPreviousMonth(int month, int year, int &prevMonth, int &prevYear)
{
    if (month == 1)
    {
        prevMonth = 12;
        prevYear = year - 1;
    }
    else
    {
        prevMonth = month - 1;
        prevYear = year;
    }
}

void getNextMonth(int month, int year, int &nextMonth, int &nextYear)
{
    if (month == 12)
    {
        nextMonth = 1;
        nextYear = year + 1;
    }
    else
    {
        nextMonth = month + 1;
        nextYear = year;
    }
}

gui_text_t *displayText(gui_obj_t *parent, const char *text_string, int x, int y,
                        gui_color_t color)
{
    const char *text = gui_strdup(text_string);
    int font_size = 16;
    gui_text_t *t = gui_text_create(parent, 0, x, y, gui_get_screen_width(), font_size);
    gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, color, strlen(text), font_size);
    void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
    gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MULTI_LEFT);
    return t;
}

void drawCalendar(gui_obj_t *parent, int windowWidth, int windowHeight, int month, int year,
                  int cellWidth, int cellHeight)
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    timeinfo->tm_sec = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_hour = 0;
    timeinfo->tm_mon = month - 1;  // tm_mon 是从0-11的
    timeinfo->tm_mday = 1;
    timeinfo->tm_year = year - 1900;
    mktime(timeinfo);
    int dayOffset = timeinfo->tm_wday;

    int daysInMonth = 31;
    switch (month)
    {
    case 4: case 6: case 9: case 11:
        daysInMonth = 30;
        break;
    case 2:
        daysInMonth = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0) ? 29 : 28;
        break;
    }

    for (int dayNum = 1; dayNum <= daysInMonth; dayNum++)
    {
        int row = (dayNum + dayOffset - 1) / 7;
        int col = (dayNum + dayOffset - 1) % 7;
        float x = margin + col * cellWidth;
        float y = margin + headerHeight + row * cellHeight + cellHeight;

        // 判断是否为今天的日期
        if (dayNum == currentDay && month == currentMonth && year == currentYear)
        {
            gui_canvas_rect_create(parent, 0, x, y - cellHeight / 2, cellWidth - 1, cellHeight - 1,
                                   gui_rgba(255, 0, 0, 255)); // 使用红色高亮
        }
        else
        {
            gui_canvas_rect_create(parent, 0, x, y - cellHeight / 2, cellWidth - 1, cellHeight - 1,
                                   gui_rgba(220, 220, 220, 255));
        }

        snprintf(buffer, sizeof(buffer), "%d", dayNum);
        displayText(parent, buffer, x + cellWidth / 2, y, gui_rgba(0, 0, 0, 255));
    }
}

gui_tab_t *tabs[3]; // 定义一个包含三个标签页的数组
int months_show; // 存储标签页对应月份
int years_show;  // 存储标签页对应年份

void updateTabContent(int tabIndex, gui_obj_t *tab, int windowWidth, int windowHeight, int month,
                      int year, int cellWidth, int cellHeight)
{
    drawCalendar(tab, windowWidth, windowHeight, month, year, cellWidth, cellHeight);
}
int get_left()
{
    for (size_t i = 0; i < 3; i++)
    {
        if (tabs[i]->id.x == -1)
        {
            return i;
        }

    }
    return 0;
}
int get_right()
{
    for (size_t i = 0; i < 3; i++)
    {
        if (tabs[i]->id.x == 1)
        {
            return i;
        }

    }
    return 0;
}
static void tabview_change(void *obj, gui_event_t e, void *param)
{
    gui_tabview_t *tabview = (gui_tabview_t *)obj;
    int cur_idx = tabview->cur_id.x;

    int nextMonth, nextYear;
    int prevMonth, prevYear;
    std::cout << cur_idx << tabs[0]->id.x << tabs[1]->id.x << tabs[2]->id.x << std::endl;
    if (cur_idx == 1)
    {
        // 当前是显示在中间的标签是 tab1，位于中间
        // 设置 tab2 左侧, tab0 右侧，然后更新其内容
        getNextMonth(months_show, years_show, nextMonth, nextYear);
        months_show = nextMonth;
        years_show = nextYear;
        getNextMonth(months_show, years_show, nextMonth, nextYear);
        gui_obj_child_free(GUI_BASE(tabs[get_left()]));
        gui_list_init(&GUI_BASE(tabs[get_left()])->child_list);
        drawCalendar(GUI_BASE(tabs[get_left()]), 454, 454, nextMonth, nextYear, cellWidth, cellHeight);
        tabview->cur_id.x = 0;
        tabs[0]->id.x -= 1;
        tabs[1]->id.x -= 1;
        tabs[2]->id.x -= 1;
        snprintf(buffer, sizeof(buffer), "%04d-%02d", years_show, months_show);
        gui_text_content_set(title, gui_strdup(buffer), strlen(buffer));


    }
    else if (cur_idx == -1)
    {
        // 当前是显示在中间的标签是 tab2，位于中间
        // 设置 tab0 左侧, tab1 右侧，然后更新其内容
        getPreviousMonth(months_show, years_show, prevMonth, prevYear);
        months_show = prevMonth;
        years_show = prevYear;
        getPreviousMonth(months_show, years_show, prevMonth, prevYear);
        gui_obj_child_free(GUI_BASE(tabs[get_right()]));
        gui_list_init(&GUI_BASE(tabs[get_right()])->child_list);
        drawCalendar(GUI_BASE(tabs[get_right()]), 454, 454, prevMonth, prevYear, cellWidth, cellHeight);

        tabview->cur_id.x = 0;
        tabs[0]->id.x += 1;
        tabs[1]->id.x += 1;
        tabs[2]->id.x += 1;
        snprintf(buffer, sizeof(buffer), "%04d-%02d", years_show, months_show);
        gui_text_content_set(title, gui_strdup(buffer), strlen(buffer));
    }
    for (size_t i = 0; i < 3; i++)
    {

        if (tabs[i]->id.x > 1)
        {
            tabs[i]->id.x = -1;
        }
        if (tabs[i]->id.x < -1)
        {
            tabs[i]->id.x = 1;
        }
    }


}

void setup_ui(gui_obj_t *parent, int windowWidth, int windowHeight, int month, int year)
{
    cellWidth = (windowWidth - 2 * margin) / 7;
    cellHeight = (windowHeight - headerHeight - 2 * margin) / 6;
    gui_canvas_rect_t *rect = gui_canvas_rect_create(parent, 0, margin, margin,
                                                     windowWidth - 2 * margin, headerHeight, gui_rgba(28, 30, 34, 255));
    snprintf(buffer, sizeof(buffer), "%04d-%02d", year, month);
    title = displayText(parent, buffer, windowWidth / 2, margin + headerHeight / 2, gui_rgba(255, 255,
                        255, 255));

    const char *weekdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    for (int i = 0; i < 7; i++)
    {
        displayText(parent, weekdays[i], margin + i * cellWidth + cellWidth / 2, margin + headerHeight,
                    gui_rgba(220, 220, 220, 255));
    }

    int prevMonth, prevYear, nextMonth, nextYear;
    getPreviousMonth(month, year, prevMonth, prevYear);
    getNextMonth(month, year, nextMonth, nextYear);
    months_show = month;
    years_show = year;
    gui_tabview_t *tabview = gui_tabview_create(parent, 0, 0, 0, 0, 0);
    gui_tabview_tab_change(tabview, (void *)tabview_change, 0);

    tabs[0] = gui_tab_create(tabview, "tab0", 0, 0, 0, 0, -1, 0);
    tabs[1] = gui_tab_create(tabview, "tab1", 0, 0, 0, 0, 0, 0);
    tabs[2] = gui_tab_create(tabview, "tab2", 0, 0, 0, 0, 1, 0);

    drawCalendar(GUI_BASE(tabs[0]), windowWidth, windowHeight, prevMonth, prevYear, cellWidth,
                 cellHeight);
    drawCalendar(GUI_BASE(tabs[1]), windowWidth, windowHeight, month, year, cellWidth, cellHeight);
    drawCalendar(GUI_BASE(tabs[2]), windowWidth, windowHeight, nextMonth, nextYear, cellWidth,
                 cellHeight);
}

extern "C" {
    void app_calender_ui_design(gui_obj_t *parent)
    {
        getCurrentDate(currentDay, currentMonth, currentYear); // 获取当前日期的完整信息
        setup_ui(parent, 454, 454, currentMonth, currentYear);
    }
}
