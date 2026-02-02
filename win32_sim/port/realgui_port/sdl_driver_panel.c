/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "sdl_driver_panel.h"
#include "gui_api.h"

typedef struct
{
    int x, y, w, h;
    uint32_t bg_color;
} ControlPanel;

typedef struct
{
    int x, y, w, h;
    int radius;
    uint32_t color_normal;
    uint32_t color_pressed;
    bool is_pressed;
    void (*on_press)(void);
    void (*on_release)(void);
} PhysicalButton;

static ControlPanel control_panel = {0};
static PhysicalButton buttons[8];
static int button_count = 0;

void sdl_panel_set_control_panel(int x, int y, int w, int h)
{
    control_panel.x = x;
    control_panel.y = y;
    control_panel.w = w;
    control_panel.h = h;
    control_panel.bg_color = 0xFF202020;
}

/**
 * @brief Add a button to the control panel
 *
 * Coordinates are relative to the control panel's origin (0,0).
 * Button colors: normal=gray(0xFF606060), pressed=blue(0xFF4080FF)
 *
 * @param x X coordinate relative to panel (button's left-top corner)
 * @param y Y coordinate relative to panel (button's left-top corner)
 * @param w Button width in pixels
 * @param h Button height in pixels
 * @param radius Shape: -1=circle, 0=rectangle, >0=rounded corner radius in pixels
 * @param on_press Callback function called when button is pressed (can be NULL)
 * @param on_release Callback function called when button is released (can be NULL)
 *
 * @note Maximum 8 buttons supported
 * @note For circle buttons, use w=h and radius=-1
 */
void sdl_panel_add_button(int x, int y, int w, int h, int radius,
                          void (*on_press)(void), void (*on_release)(void))
{
    if (button_count >= 8) { return; }

    PhysicalButton *btn = &buttons[button_count++];
    btn->x = control_panel.x + x;
    btn->y = control_panel.y + y;
    btn->w = w;
    btn->h = h;
    btn->radius = radius;
    btn->color_normal = 0xFF606060;
    btn->color_pressed = 0xFF4080FF;
    btn->is_pressed = false;
    btn->on_press = on_press;
    btn->on_release = on_release;
}

static void draw_buttons(SDL_Surface *surface)
{
    for (int i = 0; i < button_count; i++)
    {
        PhysicalButton *btn = &buttons[i];
        uint32_t color = btn->is_pressed ? btn->color_pressed : btn->color_normal;
        uint32_t *pixels = (uint32_t *)surface->pixels;
        int pitch = surface->pitch / 4;

        int cx = btn->x + btn->w / 2;
        int cy = btn->y + btn->h / 2;
        int r = (btn->w < btn->h ? btn->w : btn->h) / 2;

        for (int y = btn->y; y < btn->y + btn->h && y < surface->h; y++)
        {
            for (int x = btn->x; x < btn->x + btn->w && x < surface->w; x++)
            {
                bool draw = true;
                if (btn->radius < 0)
                {
                    int dx = x - cx, dy = y - cy;
                    draw = (dx * dx + dy * dy <= r * r);
                }
                else if (btn->radius > 0)
                {
                    int rad = btn->radius;
                    int dx, dy;
                    if (x < btn->x + rad && y < btn->y + rad)
                    {
                        dx = x - (btn->x + rad);
                        dy = y - (btn->y + rad);
                        draw = (dx * dx + dy * dy <= rad * rad);
                    }
                    else if (x >= btn->x + btn->w - rad && y < btn->y + rad)
                    {
                        dx = x - (btn->x + btn->w - rad - 1);
                        dy = y - (btn->y + rad);
                        draw = (dx * dx + dy * dy <= rad * rad);
                    }
                    else if (x < btn->x + rad && y >= btn->y + btn->h - rad)
                    {
                        dx = x - (btn->x + rad);
                        dy = y - (btn->y + btn->h - rad - 1);
                        draw = (dx * dx + dy * dy <= rad * rad);
                    }
                    else if (x >= btn->x + btn->w - rad && y >= btn->y + btn->h - rad)
                    {
                        dx = x - (btn->x + btn->w - rad - 1);
                        dy = y - (btn->y + btn->h - rad - 1);
                        draw = (dx * dx + dy * dy <= rad * rad);
                    }
                }
                if (draw)
                {
                    pixels[y * pitch + x] = color;
                }
            }
        }
    }
}

void sdl_panel_draw(SDL_Surface *surface)
{
    if (control_panel.w == 0) { return; }

    SDL_Rect panel_rect = {control_panel.x, control_panel.y, control_panel.w, control_panel.h};
    SDL_FillRect(surface, &panel_rect, control_panel.bg_color);

    draw_buttons(surface);
}

bool sdl_panel_check_click(int x, int y, bool is_down)
{
    for (int i = 0; i < button_count; i++)
    {
        PhysicalButton *btn = &buttons[i];
        if (x >= btn->x && x < btn->x + btn->w &&
            y >= btn->y && y < btn->y + btn->h)
        {
            btn->is_pressed = is_down;

            /* Trigger callbacks */
            if (is_down && btn->on_press)
            {
                btn->on_press();
            }
            else if (!is_down && btn->on_release)
            {
                btn->on_release();
            }

            return true;
        }
    }
    return false;
}
