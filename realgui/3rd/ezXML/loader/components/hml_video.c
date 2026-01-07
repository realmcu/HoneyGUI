/**
 * @file hml_video.c
 * @brief HML Video Component (hg_video)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_video.h"
#include "gui_vfs.h"
#include "gui_api.h"
#include <stdio.h>

gui_obj_t *hml_create_video(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "video");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 0);
    int h = hml_attr_height(node, 0);
    const char *src = ezxml_attr(node, "src");

    if (!src)
    {
        gui_log("hml_video: missing 'src' attribute\n");
        return NULL;
    }

    // Convert: assets/birds.mp4 → /hml/birds.mjpeg
    const char *filename = strrchr(src, '/');
    if (!filename) { filename = src; }
    else { filename++; }

    const char *dot = strrchr(filename, '.');
    size_t name_len = dot ? (size_t)(dot - filename) : strlen(filename);

    char video_path[128];
    snprintf(video_path, sizeof(video_path), "/hml/%.*s.mjpeg", (int)name_len, filename);

    // Load from romfs
    const void *video_data = gui_vfs_get_file_address(video_path);
    if (!video_data)
    {
        /* Fallback: read file into memory */
        gui_vfs_file_t *f = gui_vfs_open(video_path, GUI_VFS_READ);
        if (f)
        {
            gui_vfs_seek(f, 0, GUI_VFS_SEEK_END);
            int size = gui_vfs_tell(f);
            gui_vfs_seek(f, 0, GUI_VFS_SEEK_SET);
            void *buf = gui_malloc(size);
            if (buf) { gui_vfs_read(f, buf, size); video_data = buf; }
            gui_vfs_close(f);
        }
    }

    if (!video_data)
    {
        gui_log("hml_video: failed to load '%s'\n", video_path);
        return NULL;
    }

    gui_video_t *video = gui_video_create_from_mem(parent, id, (void *)video_data, x, y, w, h);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
    gui_video_set_repeat_count(video, GUI_VIDEO_REPEAT_INFINITE);
    return (gui_obj_t *)video;
}
