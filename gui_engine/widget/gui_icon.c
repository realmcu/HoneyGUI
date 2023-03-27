#include "gui_icon.h"
#include "gui_iconlist.h"
void gui_icon_ctor(gui_icon_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                   int16_t y, int16_t w, int16_t h, int16_t id)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    this->base.type = ICON;
    this->id = id;
    if (parent != NULL)
    {
        if (parent->type == ICONLIST)
        {
            gui_iconlist_t *parent_ext = (gui_iconlist_t *)parent;
            parent_ext->icon_cnt++;
        }
    }
}
extern gui_icon_default_t *gui_icon_create_default(void *parent, const char *filename, int16_t x,
                                                   int16_t y, int16_t w, int16_t h, int16_t id);

gui_icon_t *gui_icon_create(void *parent, const char *filename, int16_t x, int16_t y,
                            int16_t w, int16_t h, int16_t id)
{
    gui_obj_t *par = (gui_obj_t *)parent;
    if (par->type == ICONLIST)
    {
        ICON_LIST_THEME_T theme = gui_iocnlist_get_theme((char *)par->parent->name);
        switch (theme)
        {
        case ICON_LIST_THEME_0:
            return (gui_icon_t *)gui_icon_create_default(par, filename, x, y, w, h, id);
        case ICON_LIST_THEME_1:
        //return (gui_icon_t *)gui_chessboard_icon_create(par, filename, x, y, w, h, id);
        case ICON_LIST_THEME_2:
        //return (gui_icon_t *)gui_roulette_icon_create(par, filename, x, y, w, h, id);
        default:
            return (gui_icon_t *)gui_icon_create_default(par, filename, x, y, w, h, id);
        }
    }
    else
    {
        return (gui_icon_t *)gui_icon_create_default(par, filename, x, y, w, h, id);
    }
}

