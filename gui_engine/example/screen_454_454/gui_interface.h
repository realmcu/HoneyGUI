#ifndef __GUI_INTERFACE_H__
#define __GUI_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_win.h"
#include "gui_tab.h"
#include "gui_event.h"
//#include "gui_obj.h"

//for gui app main

//for menu buds
extern gui_win_t *win_menu_buds;
extern gui_win_t *win_buds_device;
extern gui_win_t *win_search_buds;
//for menu phone
extern gui_win_t *win_menu_phone;
extern gui_win_t *win_menu_phone_pairing;
//fir menu setting
extern gui_win_t *win_menu_setting;
//for player
extern gui_tab_t *tab_player;
extern gui_win_t *win_song_lists;
//for call
extern gui_win_t *win_calling;
extern gui_win_t *win_incoming_call;
extern gui_win_t *win_dial;
//for record
extern gui_win_t *win_record_play;
//for contacts

typedef enum
{
    GUI_UPDATE_TYPE_NORMAL,
    GUI_UPDATE_INCOMING_CALL,
    GUI_FREE_CALLING,
    GUI_UPDATE_PLAYER
}
T_GUI_UPDATE_TYPE;

typedef struct
{
    T_GUI_UPDATE_TYPE type;
    union
    {
        uint32_t  param;
        void     *buf;
    } u;
} T_GUI_UPDATE_EVENT;

typedef struct
{
    uint8_t bd_addr[6];
    uint8_t nam_len;
    uint16_t device_name[20];
    uint32_t cod;
} T_SEARCH_RESULT;

void gui_update_by_event2(void *obj, T_GUI_UPDATE_EVENT *gui_update_event, bool force_update);
void gui_update_by_event(gui_event_t event, void *data, bool force_update);
void gui_incoming_call_create(void);
void set_dial_num(char *dial_num);
char *get_dial_num(void);
void gui_win_calling_destruct(void);
uint16_t get_song_count(void);
void play_next_music(void);
void play_prev_music(void);

#ifdef __cplusplus
}
#endif
#endif
