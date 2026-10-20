/**
 * @file hml_streaming.c
 * @brief HML Streaming Component (hg_streaming)
 *
 * The designer stores the transport as a C expression, e.g.
 * transporter="gui_stream_transport_get()". At run time the function name is
 * looked up in the HML function registry and called to get the transport.
 * Built only with the gui_stream widget (HML_HAS_STREAM, set by the build).
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "../hml_runtime.h"
#include "gui_api.h"
#include <string.h>

#ifdef HML_HAS_STREAM
#include "gui_stream.h"

typedef stp_transport_t *(*hml_transport_getter_t)(void);

gui_obj_t *hml_create_streaming(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "stream");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 100);

    // "name()" or "name" -> name
    const char *expr = hml_attr_str(node, "transporter", "");
    char name[64];
    size_t n = 0;
    while (expr[n] && expr[n] != '(' && n + 1 < sizeof(name))
    {
        name[n] = expr[n];
        n++;
    }
    name[n] = '\0';

    hml_transport_getter_t get = (hml_transport_getter_t)hml_find_function(name);
    stp_transport_t *tp = get ? get() : NULL;
    if (!tp)
    {
        gui_log("[HML] Streaming %s: transporter '%s' not available\n", id, expr);
        return NULL;
    }

    const char *codec_s = hml_attr_str(node, "codec", "jpeg");
    gui_stream_codec_t codec = GUI_STREAM_CODEC_JPEG;
    if (!strcmp(codec_s, "msv1")) { codec = GUI_STREAM_CODEC_MSV1; }
    else if (!strcmp(codec_s, "raw")) { codec = GUI_STREAM_CODEC_RAW; }
    else if (!strcmp(codec_s, "h264")) { codec = GUI_STREAM_CODEC_H264; }

    gui_stream_t *stream = gui_stream_create(parent, id, codec, tp, x, y, w, h);
    if (!stream) { return NULL; }

    int interval = hml_attr_int(node, "updateInterval", 40);
    if (interval != 40) { gui_stream_set_update_interval(stream, (uint32_t)interval); }
    if (!strcmp(hml_attr_str(node, "dropMode", "none"), "unconditional"))
    {
        gui_stream_set_drop_mode(stream, GUI_STREAM_DROP_UNCONDITIONAL);
    }
    gui_stream_set_state(stream, GUI_VIDEO_STATE_PLAYING);

    return (gui_obj_t *)stream;
}

#else

gui_obj_t *hml_create_streaming(gui_obj_t *parent, ezxml_t node)
{
    GUI_UNUSED(parent);
    gui_log("[HML] <hg_streaming id=%s> needs the gui_stream widget\n", hml_attr_str(node, "id", "?"));
    return NULL;
}

#endif
