/*
 * File      : gui_stb_img.c
 * This file is part of GUI Engine
 */
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <gui_stb_img.h>

void gui_stbimg_set_attribute(gui_stb_img_t *this, void *addr, uint32_t size,
                              uint8_t type, int16_t x, int16_t y)
{

}


gui_stb_img_t *gui_stbimg_create_from_mem(void *parent,  const char *name,
                                          void *addr, uint32_t size, uint8_t type, int16_t x,
                                          int16_t y)
{
    GUI_ASSERT(addr != NULL);
    return NULL;
}


