/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/* Simulator stand-in for A2DP/AVRCP and HFP state. */

#include <stddef.h>
#include <string.h>

#include "gui_components_init.h"
#include "gui_sysval_adapter.h"
#include "gui_sysval_keys.h"

static const char *media_state = GUI_SYSVAL_MEDIA_STATE_PAUSED;
static int32_t media_volume = 50;
static const char *media_title = "Simulator";
static const char *media_artist = "HoneyGUI";
static const char *phone_state = GUI_SYSVAL_PHONE_STATE_IDLE;
static char phone_number[GUI_SYSVAL_STR_MAX] = "10086";
static const char *phone_route = GUI_SYSVAL_PHONE_ROUTE_DEVICE;

static void respond_int(gui_sysval_request_t *request, int32_t value)
{
    gui_sysval_value_t out = gui_sysval_value_int(value);
    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &out);
}

static void respond_str(gui_sysval_request_t *request, const char *value)
{
    gui_sysval_value_t out = gui_sysval_value_str(value);
    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &out);
}

static void publish_int(const char *key, int32_t value)
{
    gui_sysval_value_t out = gui_sysval_value_int(value);
    (void)gui_sysval_publish(key, &out);
}

static void publish_str(const char *key, const char *value)
{
    gui_sysval_value_t out = gui_sysval_value_str(value);
    (void)gui_sysval_publish(key, &out);
}

static void get_media_state(gui_sysval_request_t *request)
{
    respond_str(request, media_state);
}

static void get_media_volume(gui_sysval_request_t *request)
{
    respond_int(request, media_volume);
}

static void set_media_volume(const gui_sysval_value_t *value,
                             gui_sysval_request_t *request)
{
    if (value->data.i < 0 || value->data.i > 100)
    {
        gui_sysval_respond(request, GUI_SYSVAL_STATUS_INVALID_VALUE, NULL);
        return;
    }
    media_volume = value->data.i;
    publish_int(GUI_SYSVAL_KEY_MEDIA_MUSIC_VOLUME, media_volume);
    respond_int(request, media_volume);
}

static void get_media_title(gui_sysval_request_t *request)
{
    respond_str(request, media_title);
}

static void get_media_artist(gui_sysval_request_t *request)
{
    respond_str(request, media_artist);
}

static void get_phone_state(gui_sysval_request_t *request)
{
    respond_str(request, phone_state);
}

static void get_phone_number(gui_sysval_request_t *request)
{
    respond_str(request, phone_number);
}

static void get_phone_route(gui_sysval_request_t *request)
{
    respond_str(request, phone_route);
}

static void set_media_request(const gui_sysval_value_t *value,
                              gui_sysval_request_t *request)
{
    const char *command = value->data.s;
    if (strcmp(command, "play") == 0 || strcmp(command, "next") == 0 ||
        strcmp(command, "previous") == 0)
    {
        media_state = GUI_SYSVAL_MEDIA_STATE_PLAYING;
    }
    else if (strcmp(command, "pause") == 0)
    {
        media_state = GUI_SYSVAL_MEDIA_STATE_PAUSED;
    }
    else if (strcmp(command, "toggle") == 0)
    {
        media_state = strcmp(media_state, GUI_SYSVAL_MEDIA_STATE_PLAYING) == 0 ?
                      GUI_SYSVAL_MEDIA_STATE_PAUSED :
                      GUI_SYSVAL_MEDIA_STATE_PLAYING;
    }
    else
    {
        gui_sysval_respond(request, GUI_SYSVAL_STATUS_INVALID_VALUE, NULL);
        return;
    }

    publish_str(GUI_SYSVAL_KEY_MEDIA_MUSIC_STATE, media_state);
    respond_str(request, command);
}

static void set_phone_request(const gui_sysval_value_t *value,
                              gui_sysval_request_t *request)
{
    const char *command = value->data.s;
    if (strcmp(command, "answer") == 0)
    {
        phone_state = GUI_SYSVAL_PHONE_STATE_ACTIVE;
    }
    else if (strcmp(command, "hangup") == 0)
    {
        phone_state = GUI_SYSVAL_PHONE_STATE_IDLE;
        phone_number[0] = '\0';
    }
    else if (strncmp(command, "dial:", 5) == 0 && command[5] != '\0')
    {
        strncpy(phone_number, &command[5], sizeof(phone_number) - 1u);
        phone_number[sizeof(phone_number) - 1u] = '\0';
        phone_state = GUI_SYSVAL_PHONE_STATE_OUTGOING;
    }
    else
    {
        gui_sysval_respond(request, GUI_SYSVAL_STATUS_INVALID_VALUE, NULL);
        return;
    }

    publish_str(GUI_SYSVAL_KEY_PHONE_STATE, phone_state);
    publish_str(GUI_SYSVAL_KEY_PHONE_NUMBER, phone_number);
    respond_str(request, command);
}

static void set_phone_route(const gui_sysval_value_t *value,
                            gui_sysval_request_t *request)
{
    if (strcmp(value->data.s, GUI_SYSVAL_PHONE_ROUTE_DEVICE) != 0 &&
        strcmp(value->data.s, GUI_SYSVAL_PHONE_ROUTE_PHONE) != 0)
    {
        gui_sysval_respond(request, GUI_SYSVAL_STATUS_INVALID_VALUE, NULL);
        return;
    }
    phone_route = strcmp(value->data.s, GUI_SYSVAL_PHONE_ROUTE_DEVICE) == 0 ?
                  GUI_SYSVAL_PHONE_ROUTE_DEVICE :
                  GUI_SYSVAL_PHONE_ROUTE_PHONE;
    publish_str(GUI_SYSVAL_KEY_PHONE_AUDIO_ROUTE, phone_route);
    respond_str(request, phone_route);
}

static gui_sysval_adapter_t media_state_adapter =
{
    .key = GUI_SYSVAL_KEY_MEDIA_MUSIC_STATE,
    .type = GUI_SYSVAL_TYPE_STR,
    .get_request = get_media_state,
};

static gui_sysval_adapter_t media_volume_adapter =
{
    .key = GUI_SYSVAL_KEY_MEDIA_MUSIC_VOLUME,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = get_media_volume,
    .set_request = set_media_volume,
};

static gui_sysval_adapter_t media_title_adapter =
{
    .key = GUI_SYSVAL_KEY_MEDIA_MUSIC_TITLE,
    .type = GUI_SYSVAL_TYPE_STR,
    .get_request = get_media_title,
};

static gui_sysval_adapter_t media_artist_adapter =
{
    .key = GUI_SYSVAL_KEY_MEDIA_MUSIC_ARTIST,
    .type = GUI_SYSVAL_TYPE_STR,
    .get_request = get_media_artist,
};

static gui_sysval_adapter_t phone_state_adapter =
{
    .key = GUI_SYSVAL_KEY_PHONE_STATE,
    .type = GUI_SYSVAL_TYPE_STR,
    .get_request = get_phone_state,
};

static gui_sysval_adapter_t phone_number_adapter =
{
    .key = GUI_SYSVAL_KEY_PHONE_NUMBER,
    .type = GUI_SYSVAL_TYPE_STR,
    .get_request = get_phone_number,
};

static gui_sysval_adapter_t phone_route_adapter =
{
    .key = GUI_SYSVAL_KEY_PHONE_AUDIO_ROUTE,
    .type = GUI_SYSVAL_TYPE_STR,
    .get_request = get_phone_route,
    .set_request = set_phone_route,
};

static gui_sysval_adapter_t media_request_adapter =
{
    .key = GUI_SYSVAL_KEY_MEDIA_REQUEST,
    .type = GUI_SYSVAL_TYPE_STR,
    .set_request = set_media_request,
};

static gui_sysval_adapter_t phone_request_adapter =
{
    .key = GUI_SYSVAL_KEY_PHONE_REQUEST,
    .type = GUI_SYSVAL_TYPE_STR,
    .set_request = set_phone_request,
};

static int port_sysval_media_phone_init(void)
{
    gui_sysval_adapter_register(&media_state_adapter);
    gui_sysval_adapter_register(&media_volume_adapter);
    gui_sysval_adapter_register(&media_title_adapter);
    gui_sysval_adapter_register(&media_artist_adapter);
    gui_sysval_adapter_register(&phone_state_adapter);
    gui_sysval_adapter_register(&phone_number_adapter);
    gui_sysval_adapter_register(&phone_route_adapter);
    gui_sysval_adapter_register(&media_request_adapter);
    gui_sysval_adapter_register(&phone_request_adapter);
    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_media_phone_init);
