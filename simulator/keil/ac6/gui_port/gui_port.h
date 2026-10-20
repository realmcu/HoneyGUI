/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * File      : gui_port.h
 */
#ifndef __GUI_PORT_H__
#define __GUI_PORT_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void gui_port_dc_init(void);
void gui_port_indev_init(void);
void gui_port_os_init(void);
void gui_port_fs_init(void);
#ifdef __cplusplus
}
#endif

#endif
