#ifndef __WATCH_ADAPT_H__
#define __WATCH_ADAPT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct gui_audio
{
    /* gui music */
    void (*music_load)(void *p);
    void (*music_play)(void *p);
    void (*music_stop)(void);
    void (*music_backward)(void);
    void (*music_forward)(void);
    bool (*music_completion_status)(void);
    float (*music_length)(void);
    float (*music_current_time)(void);

    /* gui record */
    void (*record_start)(void);
    void (*record_stop)(void);
    void (*record_play)(void);
    void (*record_pause)(void);
    bool (*record_completion_status)(void);
} gui_audio_t;

typedef struct gui_control_board
{
    void (*send_bt_status)(bool)        ;
    void (*send_local_play_status)(bool);
    void (*send_phone_status)(bool)     ;
    void (*send_earphone_status)(bool)  ;
} gui_control_board_t;

typedef struct gui_call
{
    void (*call_start)(void);
    void (*call_end)(void);
    void (*call_dail)(char *);
    bool (*call_get_dail_status)(void); //True: the call is taken, switch to calling window
    void (*call_set_mic)(bool); // True: on, false: off
    void (*call_set_speaker)(bool); // True: on, false: off
} gui_call_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/
/* Audio */
void gui_audio_info_register(gui_audio_t *info);
gui_audio_t *gui_get_audio(void);


/* Controlboard */
void switch_bt(bool state); // true: on, false: off
void switch_local_play(bool state);
void switch_phone(bool state);
void switch_earphone(bool state);
void gui_control_board_info_register(gui_control_board_t *info);
gui_control_board_t *gui_get_control_board(void);

/* Call */
void gui_call_info_register(gui_call_t *info);
gui_call_t *gui_get_call(void);
void gui_update_call_number(char *number);
bool *gui_call_incoming_flag_get(
    void); // Get the call incoming flag, then set it to true when a call is incoming

#ifdef __cplusplus
}
#endif
#endif


