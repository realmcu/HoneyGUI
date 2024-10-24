#include "gui_canvas.h"
#include "gui_canvas_rect.h"
#include "gui_obj.h"
#include "gui_text.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_win.h"
#include "gui_calendar.h"
#include "time.h"
static void get_current_date(int *day, int *month, int *year)
{
    time_t now = time(0);
    struct tm *ltm = localtime(&now);
    *day = ltm->tm_mday;
    *month = 1 + ltm->tm_mon;
    *year = 1900 + ltm->tm_year;
}

static void get_previous_month(int month, int year, int *prevMonth, int *prevYear)
{
    if (month == 1)
    {
        *prevMonth = 12;
        *prevYear = year - 1;
    }
    else
    {
        *prevMonth = month - 1;
        *prevYear = year;
    }
}

static void get_next_month(int month, int year, int *nextMonth, int *nextYear)
{
    if (month == 12)
    {
        *nextMonth = 1;
        *nextYear = year + 1;
    }
    else
    {
        *nextMonth = month + 1;
        *nextYear = year;
    }
}


static gui_text_t *display_text_center(gui_obj_t *parent, const char *text_string, int x, int y,
                                       int w,
                                       gui_color_t color, gui_calendar_t *widget)
{
    const char *text = gui_strdup(text_string);

    gui_text_t *t = gui_text_create(parent, 0, x, y, w, widget->font_size);
    gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, color, strlen(text), widget->font_size);
    void *addr1 = widget->font_source;
    gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MULTI_CENTER);
    return t;
}
static void draw_calendar(gui_obj_t *parent, int windowWidth, int windowHeight, int month, int year,
                          int cellWidth, int cellHeight, gui_calendar_t *widget)
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    timeinfo->tm_sec = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_hour = 0;
    timeinfo->tm_mon = month - 1;
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
        float x = widget->margin + col * cellWidth;
        float y = widget->margin + widget->headerHeight + row * cellHeight + cellHeight;
        gui_color_t text_color = widget->color;
        if (dayNum == widget->currentDay && month == widget->currentMonth && year == widget->currentYear)
        {
            gui_canvas_rect_create(parent, 0, x, y - cellHeight / 2, cellWidth - 1, cellHeight - 1,
                                   widget->item_color_highlight);
            text_color = widget->font_color_highlight;
        }
        else
        {
            gui_canvas_rect_create(parent, 0, x, y - cellHeight / 2, cellWidth - 1, cellHeight - 1,
                                   widget->item_color);

        }

        snprintf(widget->buffer, sizeof(widget->buffer), "%d", dayNum);
        display_text_center(parent, widget->buffer, x, y - widget->font_size / 2, cellWidth, text_color,
                            widget);
    }
}




static int get_left(gui_calendar_t *widget)
{
    for (size_t i = 0; i < 3; i++)
    {
        if (widget->tabs[i]->id.x == -1)
        {
            return i;
        }

    }
    return 0;
}
static int get_right(gui_calendar_t *widget)
{
    for (size_t i = 0; i < 3; i++)
    {
        if (widget->tabs[i]->id.x == 1)
        {
            return i;
        }

    }
    return 0;
}
static void tabview_change(void *obj, gui_event_t e, void *param)
{
    gui_tabview_t *tabview = (gui_tabview_t *)obj;
    gui_calendar_t *widget = param;
    int cur_idx = tabview->cur_id.x;

    int nextMonth, nextYear;
    int prevMonth, prevYear;
    if (cur_idx == 1)
    {

        get_next_month(widget->months_show, widget->years_show, &nextMonth, &nextYear);
        widget->months_show = nextMonth;
        widget->years_show = nextYear;
        get_next_month(widget->months_show, widget->years_show, &nextMonth, &nextYear);
        gui_obj_child_free(GUI_BASE(widget->tabs[get_left(widget)]));
        gui_list_init(&GUI_BASE(widget->tabs[get_left(widget)])->child_list);
        draw_calendar(GUI_BASE(widget->tabs[get_left(widget)]), 454, 454, nextMonth, nextYear,
                      widget->cellWidth, widget->cellHeight, widget);
        tabview->cur_id.x = 0;
        widget->tabs[0]->id.x -= 1;
        widget->tabs[1]->id.x -= 1;
        widget->tabs[2]->id.x -= 1;
        snprintf(widget->buffer, sizeof(widget->buffer), "%04d-%02d", widget->years_show,
                 widget->months_show);
        gui_text_content_set(widget->title, gui_strdup(widget->buffer), strlen(widget->buffer));


    }
    else if (cur_idx == -1)
    {

        get_previous_month(widget->months_show, widget->years_show, &prevMonth, &prevYear);
        widget->months_show = prevMonth;
        widget->years_show = prevYear;
        get_previous_month(widget->months_show, widget-> years_show, &prevMonth, &prevYear);
        gui_obj_child_free(GUI_BASE(widget->tabs[get_right(widget)]));
        gui_list_init(&GUI_BASE(widget->tabs[get_right(widget)])->child_list);
        draw_calendar(GUI_BASE(widget->tabs[get_right(widget)]), 454, 454, prevMonth, prevYear,
                      widget->cellWidth, widget->cellHeight, widget);

        tabview->cur_id.x = 0;
        widget->tabs[0]->id.x += 1;
        widget->tabs[1]->id.x += 1;
        widget->tabs[2]->id.x += 1;
        snprintf(widget->buffer, sizeof(widget->buffer), "%04d-%02d", widget->years_show,
                 widget->months_show);
        gui_text_content_set(widget->title, gui_strdup(widget->buffer), strlen(widget->buffer));
    }
    for (size_t i = 0; i < 3; i++)
    {

        if (widget->tabs[i]->id.x > 1)
        {
            widget->tabs[i]->id.x = -1;
        }
        if (widget->tabs[i]->id.x < -1)
        {
            widget->tabs[i]->id.x = 1;
        }
    }


}

static void setup_ui(gui_obj_t *parent, int windowWidth, int windowHeight, int month, int year,
                     gui_calendar_t *widget)
{
    widget->cellWidth = (windowWidth - 2 * widget->margin) / 7;
    widget->cellHeight = (windowHeight - widget->headerHeight - 2 * widget->margin) / 6;
    gui_canvas_rect_t *rect = gui_canvas_rect_create(parent, 0, widget->margin, widget->margin,
                                                     windowWidth - 2 * widget->margin, widget->headerHeight, widget->title_background_color);
    snprintf(widget->buffer, sizeof(widget->buffer), "%04d-%02d", year, month);
    widget->title = display_text_center(parent, widget->buffer, widget->margin,
                                        widget->margin + widget->headerHeight / 2, windowWidth - 2 * widget->margin, widget->title_color,
                                        widget);

    const char *weekdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    for (int i = 0; i < 7; i++)
    {
        display_text_center(parent, weekdays[i], widget->margin + i * widget->cellWidth,
                            widget->margin + widget->headerHeight, widget->cellWidth,
                            widget->title_color, widget);
    }

    int prevMonth, prevYear, nextMonth, nextYear;
    get_previous_month(month, year, &prevMonth, &prevYear);
    get_next_month(month, year, &nextMonth, &nextYear);
    widget->months_show = month;
    widget->years_show = year;
    gui_tabview_t *tabview = gui_tabview_create(parent, 0, 0, 0, 0, 0);
    gui_tabview_tab_change(tabview, (void *)tabview_change, widget);

    widget->tabs[0] = gui_tab_create(tabview, "tab0", 0, 0, 0, 0, -1, 0);
    widget->tabs[1] = gui_tab_create(tabview, "tab1", 0, 0, 0, 0, 0, 0);
    widget->tabs[2] = gui_tab_create(tabview, "tab2", 0, 0, 0, 0, 1, 0);

    draw_calendar(GUI_BASE(widget-> tabs[0]), windowWidth, windowHeight, prevMonth, prevYear,
                  widget->cellWidth,
                  widget->cellHeight, widget);
    draw_calendar(GUI_BASE(widget->tabs[1]), windowWidth, windowHeight, month, year, widget->cellWidth,
                  widget->cellHeight, widget);
    draw_calendar(GUI_BASE(widget->tabs[2]), windowWidth, windowHeight, nextMonth, nextYear,
                  widget->cellWidth,
                  widget->cellHeight, widget);
}

static void ctor(gui_calendar_t *widget, gui_obj_t *parent, int x, int y, int w, int h,
                 void *font_source_p,
                 int font_size_p, gui_color_t color_p,    gui_color_t font_color_highlight,
                 gui_color_t item_color,
                 gui_color_t item_color_highlight,
                 gui_color_t title_color,
                 gui_color_t title_background_color)
{
    gui_obj_ctor(&widget->base, parent, "calendar", x, y, w, h);
    GET_BASE(widget)->type = MACRO_CALENDAR;
    widget->margin = 20;
    widget-> headerHeight = 50;
    widget->font_source = font_source_p;
    widget->font_size = font_size_p;
    widget->color = color_p;
    widget->font_color_highlight = font_color_highlight;
    widget->item_color = item_color;
    widget->item_color_highlight = item_color_highlight;
    widget->title_color = title_color;
    widget->title_background_color = title_background_color;
}

void gui_calender_create(gui_obj_t *parent, int x, int y, int w, int h, void *font_source_p,
                         int font_size_p, gui_color_t color_p,    gui_color_t font_color_highlight,
                         gui_color_t item_color,
                         gui_color_t item_color_highlight,
                         gui_color_t title_color,
                         gui_color_t title_background_color)
{
#define _GUI_NEW_GUI_CALENDAR_PARAM this, parent, x, y, w, h, font_source_p, font_size_p, color_p, font_color_highlight, item_color, item_color_highlight, title_color, title_background_color
    GUI_CREATE_HELPER(gui_calendar_t, ctor, _GUI_NEW_GUI_CALENDAR_PARAM)
    get_current_date(&this->currentDay, &this->currentMonth, &this->currentYear);
    setup_ui(GUI_BASE(this), w, h, this->currentMonth, this->currentYear, this);
}

