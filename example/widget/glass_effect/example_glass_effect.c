#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_gray.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_video.h"
#include "gui_glass.h"
#include "glass_0.c"
#include "glass_1.c"
#include "glass_1_2.c"

#ifdef _WIN32
extern const unsigned char _binary_bg_bin_start[];
extern const unsigned char _binary_bg_bin_end[];
extern const unsigned char _binary_bg_bin_size[];
#else
#define BG_ADDR     0x7004D100
#endif

/* gui glass widget example start*/
static int app_init(void)
{
#ifdef _WIN32
    unsigned char *resource_root = (unsigned char *)_binary_bg_bin_start;
#else
    unsigned char *resource_root = (unsigned char *)BG_ADDR;
#endif
    gui_video_t *video = gui_video_create_from_mem(gui_obj_get_root(), "background",
                                                   (void *)resource_root,
                                                   0, 0, 410,
                                                   502);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
    gui_video_set_repeat_count(video, GUI_VIDEO_REPEAT_INFINITE);
    gui_glass_create_from_mem(gui_obj_get_root(),  "img_2_test", (void *)glass_0, 195, 40, 0, 0);
    gui_glass_create_from_mem(gui_obj_get_root(),  "img_2_test", (void *)_acglass_1, 50, 40, 0,
                              0);
    gui_glass_create_from_mem(gui_obj_get_root(),  "img_2_test", (void *)glass_0, 30, 261, 0, 0);
    gui_glass_create_from_mem(gui_obj_get_root(),  "img_2_test", (void *)_acglass_1_2, 240, 270,
                              0, 0);

    return 0;
}
GUI_INIT_APP_EXPORT(app_init);
/* gui glass widget example end*/

