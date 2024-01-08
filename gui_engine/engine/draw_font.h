/*
 * File      : font.h
 * This file is part of RT-Thread GUI Engine
 * COPYRIGHT (C) 2006 - 2017, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-10-16     Bernard      first version
 */
#ifndef __DRAW_FONT_H__
#define __DRAW_FONT_H__

#include <guidef.h>
#include <gui_api.h>
#include <gui_text.h>
#include "font_mem.h"
#include "font_stb.h"
#ifdef __cplusplus
extern "C" {
#endif

uint16_t utf8_to_unicode(uint8_t *utf8, uint16_t len, uint16_t *unicode_array,
                         uint16_t unicode_buf_len);

#ifdef __cplusplus
}
#endif

#endif

