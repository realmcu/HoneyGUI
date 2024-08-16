#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"
#include "gui_scroll_text.h"


gui_text_t *text_volume;
gui_text_t *text_music_title_prefix;
gui_scroll_text_t *text_music_title;
gui_scroll_text_t *text_music_title2;
gui_text_t *text_music_artist;
gui_text_t *text_music_album;

static char music_title[MUSIC_TITLE_MAX_LENGTH];
static char music_artist[MUSIC_ARTIST_MAX_LENGTH];
static char music_album[MUSIC_ALBUM_MAX_LENGTH];
char *title_prefix = "Title: ";
char *artist_prefix = "Artist: ";
char *album_prefix = "Album: ";
static char volume_str[3] = {'8', '5'};

void app_gui_show_volume(uint8_t current_phone_volume)
{
    sprintf(volume_str, "%d", current_phone_volume);
    gui_text_set(text_volume, volume_str, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(volume_str), 28);
}

void app_gui_show_title(uint8_t *title, uint16_t length)
{
    gui_log("app_tb_set_title: %s", title);
    memset(music_title, 0, sizeof(music_title));

    snprintf(music_title, sizeof(music_title), "%s", title);

    gui_scroll_text_content_set(text_music_title, music_title,
                                length < MUSIC_TITLE_MAX_LENGTH ? length : MUSIC_TITLE_MAX_LENGTH);
    gui_scroll_text_scroll_set(text_music_title, SCROLL_X, 0, 0, MUSIC_SCROLL_TEXT_DURATION,
                               0);
}

void app_gui_show_artist(uint8_t *artist, uint16_t length)
{
    gui_log("app_tb_set_artist: %s", artist);
    memset(music_artist, 0, sizeof(music_artist));

    snprintf(music_artist, sizeof(music_artist), "%s%s", artist_prefix, artist);

    gui_text_content_set(text_music_artist, music_artist,
                         (length + strlen(artist_prefix)) < MUSIC_ARTIST_MAX_LENGTH ? (length + strlen(
                                     artist_prefix)) : MUSIC_ARTIST_MAX_LENGTH);
}

void app_gui_show_album(uint8_t *album, uint16_t length)
{
    gui_log("app_tb_set_album: %s", album);
    memset(music_album, 0, sizeof(music_album));

    snprintf(music_album, sizeof(music_album), "%s%s", album_prefix, album);

    gui_text_content_set(text_music_album, music_album,
                         (length + strlen(album_prefix)) < MUSIC_ALBUM_MAX_LENGTH ? (length + strlen(
                                     album_prefix)) : MUSIC_ALBUM_MAX_LENGTH);
}

void page_tb_volume(void *parent)
{
    text_music_title_prefix = gui_text_create(parent, "title txt prefix", 20, 60, 100, 28);
    gui_text_set(text_music_title_prefix, title_prefix, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                                  UINT8_MAX),
                 strlen(title_prefix), 28);

    text_music_title = gui_scroll_text_create(parent, "title txt", 120, 60, 200, 28);
    gui_scroll_text_set(text_music_title, music_title, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                                 UINT8_MAX),
                        strlen(music_title), 28);
    gui_scroll_text_encoding_set(text_music_title, UTF_8);

    snprintf(music_artist, sizeof(music_artist), "%s", artist_prefix);
    text_music_artist = gui_text_create(parent, "artist txt", 20, 105, 200, 360);
    gui_text_set(text_music_artist, music_artist, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                            UINT8_MAX),
                 strlen(music_artist), 28);

    snprintf(music_album, sizeof(music_album), "%s", album_prefix);
    text_music_album = gui_text_create(parent, "album txt", 20, 150, 200, 28);
    gui_text_set(text_music_album, music_album, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                          UINT8_MAX),
                 strlen(music_album), 28);

    gui_button_t *button_volume_down = gui_button_create(parent, 60, 200, 80, 80, VOLUME_DOWN_BIN,
                                                         VOLUME_DOWN_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_volume_down, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_VOLUME_DOWN);

    text_volume = gui_text_create(parent, "volume", 175, 230, 28, 100);
    gui_text_set(text_volume, volume_str, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(volume_str), 28);

    gui_button_t *button_volume_up = gui_button_create(parent, 240, 200, 80, 80, VOLUME_UP_BIN,
                                                       VOLUME_UP_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_volume_up, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_VOLUME_UP);
    // extern void app_gui_enter_call(void *obj, gui_event_t e, void *param);
    // gui_obj_add_event_cb(button_volume_up, app_gui_enter_call, GUI_EVENT_TOUCH_LONG, NULL);
}
