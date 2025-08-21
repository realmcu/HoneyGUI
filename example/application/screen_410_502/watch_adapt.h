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

#ifdef __cplusplus
}
#endif
#endif


