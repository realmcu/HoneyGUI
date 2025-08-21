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
static gui_audio_t *gui_audio = NULL;
static gui_control_board_t *gui_control_board = NULL;

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_audio_info_register(gui_audio_t *info)
{
    gui_audio = info;
}

gui_audio_t *gui_get_audio(void)
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
