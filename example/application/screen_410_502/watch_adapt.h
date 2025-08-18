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
} gui_audio_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/
void gui_audio_info_register(gui_audio_t *info);

gui_audio_t *gui_get_audio(void);



#ifdef __cplusplus
}
#endif
#endif


