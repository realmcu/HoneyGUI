#ifndef __PLAYER_MGR_H__
#define __PLAYER_MGR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gui_app.h>
#include "gui_tabview.h"
#include "root_image/ui_resource.h"
#ifndef _WIN32
#include "fs_if.h"
#endif

typedef enum
{
    PLAYER_MAIN,
    PLAYER_LIST,
    PLAYER_MAX
} player_ids;

//player mgr apis
int app_player_switch_tabs(player_ids id);
void free_player_mgr(void);

//player main apis
void design_tab_player_main(void *parent);

//player list apis
void design_tab_song_lists(void *parent);
void play_next_music(void);
void play_prev_music(void);
uint16_t get_song_count(void);
#ifndef _WIN32
T_HEAD_INFO *get_cur_play_header_info(void);
#endif

#ifdef __cplusplus
}
#endif
#endif


