/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include "watch_adapt.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_audio_ctrl_t *gui_audio = NULL;
static gui_control_board_t *gui_control_board = NULL;
static gui_call_t *gui_call = NULL;

static bool call_incoming_flag = false; // Flag to indicate if the call is incoming

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_audio_ctrl_register(gui_audio_ctrl_t *info)
{
    gui_audio = info;
}

gui_audio_ctrl_t *gui_get_audio_ctrl(void)
{
    return gui_audio;
}

void gui_control_board_info_register(gui_control_board_t *info)
{
    gui_control_board = info;
}

gui_control_board_t *gui_get_control_board(void)
{
    return gui_control_board;
}

void gui_call_info_register(gui_call_t *info)
{
    gui_call = info;
}

gui_call_t *gui_get_call(void)
{
    return gui_call;
}

bool *gui_call_incoming_flag_get(void)
{
    return &call_incoming_flag;
}
