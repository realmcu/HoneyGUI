
/*
 * File      : gui_kb.c
 */
#include <string.h>
#include <gui_server.h>
#include <guidef.h>
#include <gui_api.h>
#include <stdlib.h>
#include <gui_kb.h>


static kb_info_t kb = {0};

void kb_algo_process(gui_kb_port_data_t *kb_raw)
{
    if (kb_raw->flag == true)
    {
        kb.type = KB_KEYDOWN;
    }
    else
    {
        kb.type = 0;
    }

}
kb_info_t *kb_get_info(void)
{
    return &kb;
}

