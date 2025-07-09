/* gui setWater example start*/
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
#include "gui_view.h"
#include "gui_canvas.h"
#include "gui_3d.h"
#include "gui_matrix.h"

#include <tp_algo.h>
#include <math.h>
#include "gui_components_init.h"
#include "ui_resource.h"
#include "./SeaWaterDClock/fish0/desc_fish0.txt"
#include "./SeaWaterDClock/fish1/desc_fish1.txt"
#include "./SeaWaterDClock/fish2/desc_fish2.txt"
#include "./SeaWaterDClock/fish3/desc_fish3.txt"

// gui_view_t *view = NULL;
gui_3d_t *fish0 = NULL;
gui_3d_t *fish1 = NULL;
gui_3d_t *fish2 = NULL;
gui_3d_t *fish3 = NULL;
gui_img_t *can0 = NULL;
gui_img_t *can1 = NULL;
gui_img_t *can2 = NULL;
gui_img_t *can3 = NULL;
gui_img_t *time0 = NULL;
gui_img_t *time1 = NULL;
gui_img_t *time2 = NULL;
gui_img_t *time3 = NULL;
gui_img_t *waterface_bg = NULL;
// gui_img_t *waterface = NULL;
gui_win_t *fish_window = NULL;
gui_win_t *can0_window = NULL;
gui_win_t *can1_window = NULL;
gui_win_t *can2_window = NULL;
gui_win_t *can3_window = NULL;
gui_win_t *wave_window = NULL;
gui_win_t *bubble_window = NULL;

float can_rotation = 0.0f;
float time_rotation = 0.0f;
static float fish_angle = 0.0f;
static float fish_time = 0.0f;
static bool wave_active = false;
static uint32_t wave_start_time = 0;
static const uint32_t WAVE_DURATION_MS = 5000;

int fish0_x = 70;
int fish0_y = -50;
int fish1_x = -70;
int fish1_y = -40;
int fish2_x = 50;
int fish2_y = -60;
int fish3_x = 45;
int fish3_y = -45;
static void *display_time_resource_def[] =
{
    NUM0_BIN,
    NUM1_BIN,
    NUM2_BIN,
    NUM3_BIN,
    NUM4_BIN,
    NUM5_BIN,
    NUM6_BIN,
    NUM7_BIN,
    NUM8_BIN,
    NUM9_BIN,
};

#define CURRENT_VIEW_NAME "sea_view"

static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *yellow_view_descriptor = NULL;
static void app_ui_wave_nums_design(gui_view_t *view);

static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_wave_nums_design,
    .on_switch_out = NULL,

    .keep = false,
};

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

typedef struct BubbleNode
{
    gui_img_t *img;
    float scale;
    int opacity;
    float speed;
    int start_x;
    int start_y;
    uint32_t create_time;
    struct BubbleNode *next;
} BubbleNode;


#define MAX_BUBBLES 20
static BubbleNode bubble_pool[MAX_BUBBLES];
static BubbleNode *free_bubble_list = NULL;
static BubbleNode *active_bubble_list = NULL;
static void init_bubble_pool()
{
    for (int i = 0; i < MAX_BUBBLES - 1; i++)
    {
        bubble_pool[i].next = &bubble_pool[i + 1];
    }
    bubble_pool[MAX_BUBBLES - 1].next = NULL;
    free_bubble_list = &bubble_pool[0];
}
static BubbleNode *get_bubble()
{
    if (free_bubble_list == NULL) { return NULL; }

    BubbleNode *new_bubble = free_bubble_list;
    free_bubble_list = free_bubble_list->next;

    new_bubble->next = active_bubble_list;
    active_bubble_list = new_bubble;

    return new_bubble;
}
static void release_bubble(BubbleNode *bubble)
{
    if (active_bubble_list == bubble)
    {
        active_bubble_list = bubble->next;
    }
    else
    {
        BubbleNode *prev = active_bubble_list;
        while (prev && prev->next != bubble)
        {
            prev = prev->next;
        }
        if (prev) { prev->next = bubble->next; }
    }

    bubble->next = free_bubble_list;
    free_bubble_list = bubble;

    bubble->img->base.not_show = true;
}

static void create_bubble(void *parent, int x, int y)
{
    BubbleNode *new_bubble = get_bubble();
    if (!new_bubble) { return; }

    if (new_bubble->img == NULL)
    {
        new_bubble->img = gui_img_create_from_mem(bubble_window, "droplet", DROPLET_BIN, x, y, 0, 0);
        // gui_log("create bubble at %d, %d\n", x, y);
    }
    else
    {
        new_bubble->img->base.x = x;
        new_bubble->img->base.y = y;
        new_bubble->img->base.not_show = false;
    }

    new_bubble->scale = 0.3f + (rand() % 7) / 20.0f;
    new_bubble->opacity = 150 + rand() % 105;
    new_bubble->speed = 0.8f + (rand() % 14) / 20.0f;
    new_bubble->start_x = x;
    new_bubble->start_y = y;
    new_bubble->create_time = gui_ms_get();

    gui_img_scale(new_bubble->img, new_bubble->scale, new_bubble->scale);
    gui_img_set_opacity(new_bubble->img, new_bubble->opacity);
}

static void update_all_bubbles()
{
    BubbleNode *current = active_bubble_list;
    uint32_t current_time = gui_ms_get();

    while (current)
    {
        uint32_t elapsed = current_time - current->create_time;

        int y_offset = (int)(current->speed * elapsed / 20.0f);
        current->img->base.y = current->start_y - y_offset;
        current->img->base.x = current->start_x + 5 * sin(elapsed / 200.0f);
        current->scale += 0.002f;
        current->opacity = 255 - (elapsed / 10);

        gui_img_scale(current->img, current->scale, current->scale);
        gui_img_set_opacity(current->img, current->opacity > 0 ? current->opacity : 0);

        if (elapsed > 2500 || current->opacity <= 0)
        {
            BubbleNode *to_remove = current;
            current = current->next;
            release_bubble(to_remove);
            continue;
        }
        current = current->next;
    }
}
int fish_x_to_screen(float fish_x)
{
    float a = 80.0f;   //left fish's max x (most left)
    float b = -90.0f;  //right fish's min x (most right)
    float c = 0.0f;    //set the screen's left edge to 0
    float d = 410.0f;  // set the screen's right edge to 410

    float screen_x = c + ((fish_x - a) / (b - a)) * (d - c);

    if (screen_x < 0) { screen_x = 0; }
    if (screen_x > 410) { screen_x = 410; }

    return (int)screen_x;
}
static uint32_t fish_last_bubble_time[4] = {0};
static void fish_animate_cb(void *parent)
{
    touch_info_t *tp = tp_get_info();

    static uint32_t last_bubble_time = 0;

    float fish_pos_x[4];

    if (tp->released)
    {
        for (int i = 0; i < 4; i++)
        {
            float pos = rand() % 161;;
            fish_pos_x[i] = pos;
        }
        if (fish0_x < -90)
        {
            // uint16_t i = rand() % 161; //-90~70
            fish0_x = 70 - fish_pos_x[0]; // -90~70
            // -50~-30
        }
        if (fish1_x > 80)
        {
            // uint16_t i = rand() % 161; //-90~70
            fish1_x = fish_pos_x[1] - 70; // -90~70
        }
        if (fish2_x < -90)
        {
            fish2_x = 70 - fish_pos_x[2]; // -90~70
        }
        if (fish3_x < -90)
        {
            fish3_x = 70 - fish_pos_x[3]; // -90~70
        }

    }
    else
    {
        uint32_t current_time = gui_ms_get();
        if ((current_time - fish_last_bubble_time[0] > 200) && (fish0_x < 70))
        {
            create_bubble(parent, fish_x_to_screen(fish0_x), -(fish0_y - 60));
            fish_last_bubble_time[0] = current_time;
        }

        if (current_time - fish_last_bubble_time[1] > 400)
        {
            create_bubble(parent, fish_x_to_screen(fish1_x), -(fish1_y - 60));
            fish_last_bubble_time[1] = current_time;
        }

        if (current_time - fish_last_bubble_time[2] > 800)
        {
            create_bubble(parent, fish_x_to_screen(fish2_x), -(fish2_y - 60));
            fish_last_bubble_time[2] = current_time;
        }

        if (current_time - fish_last_bubble_time[3] > 1200)
        {
            create_bubble(parent, fish_x_to_screen(fish3_x), -(fish3_y - 60));
            fish_last_bubble_time[3] = current_time;
        }
        if (fish0)
        {
            fish0_x -= 1;
            fish_time += 0.1f;
            fish_angle = 10.0f * sinf(fish_time);
        }
        if (fish1)
        {
            fish1_x += 1;
            fish_time += 0.1f;
            fish_angle = 10.0f * sinf(fish_time);
        }
        if (fish2)
        {
            fish2_x -= 1.2f;
            fish_time += 0.1f;
            fish_angle = 10.0f * sinf(fish_time);
        }
        if (fish3)
        {
            fish3_x -= 1;
            fish_time += 0.1f;
            fish_angle = 10.0f * sinf(fish_time);
        }
    }
    update_all_bubbles();

}

static void fish0_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 80), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish0_x, fish0_y, 0, 0, 0, 0,
                             5);

}
static void fish1_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 80), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish1_x, fish1_y, 0, 0, 0, 0,
                             5);

}
static void fish2_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 80), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish2_x, fish2_y, 0, 0, 0, 0,
                             5);

}
static void fish3_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 80), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish3_x, fish3_y, 0, 0, 0, 0,
                             5);
}
static gui_3d_matrix_t fish_face_cb(gui_3d_t *this, size_t face_index/*face offset*/)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    if (face_index == 0)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 fish_angle, 1);
    }

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;

}
static void canvas_cb_black(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;


    nvgRect(vg, 0, 236, 410, 244);

    NVGpaint gradient = nvgLinearGradient(vg, 0, 236, 410, 244, nvgRGBA(0, 160, 192, 0), nvgRGBA(0, 160,
                                          192, 255));
    nvgFillPaint(vg, gradient);
    nvgFill(vg);

}
gui_canvas_t *canvas_wave = NULL;
extern void gui_wave_draw_graph(gui_canvas_t *canvas);
static void wave_animate_cb(void *parent)
{
    touch_info_t *tp = tp_get_info();
    uint32_t current_time = gui_ms_get();

    if (tp->pressed)
    {
        wave_active = true;
        wave_start_time = current_time;
        gui_canvas_set_canvas_cb(canvas_wave, gui_wave_draw_graph);
    }

    if (wave_active && (current_time - wave_start_time > WAVE_DURATION_MS))
    {
        wave_active = false;
        gui_canvas_set_canvas_cb(canvas_wave, NULL);
    }
}
static void app_ui_wave_nums_design(gui_view_t *view)
{
    init_bubble_pool();

    gui_img_t *background = gui_img_create_from_mem(view, "background", BACKGROUND_BIN, 0, 0, 0, 0);
    gui_img_t *ForeGround = gui_img_create_from_mem(view, "ForeGround", FOREGROUND_BIN, 0, 270, 0, 0);

    fish_window = gui_win_create(view, "fish_window", 0, 0, 410, 502);

    fish0 = gui_3d_create(fish_window, "3d-fish0", (void *)_acdesc_fish0, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                          410, 502);
    fish1 = gui_3d_create(fish_window, "3d-fish1", (void *)_acdesc_fish1, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                          410, 502);
    fish2 = gui_3d_create(fish_window, "3d-fish2", (void *)_acdesc_fish2, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                          410, 502);
    fish3 = gui_3d_create(fish_window, "3d-fish3", (void *)_acdesc_fish3, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                          410, 502);

    gui_3d_set_global_transform_cb(fish0, (gui_3d_global_transform_cb)fish0_global_cb);
    gui_3d_set_face_transform_cb(fish0, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish1, (gui_3d_global_transform_cb)fish1_global_cb);
    gui_3d_set_face_transform_cb(fish1, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish2, (gui_3d_global_transform_cb)fish2_global_cb);
    gui_3d_set_face_transform_cb(fish2, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish3, (gui_3d_global_transform_cb)fish3_global_cb);
    gui_3d_set_face_transform_cb(fish3, (gui_3d_face_transform_cb)fish_face_cb);

    can0_window = gui_win_create(view, "can_window", 70, 190, 64, 116);
    can0 = gui_img_create_from_mem(can0_window, "can0", CAN_BIN, 0, 0, 0, 0);
    gui_img_t *CanSkin_purple = gui_img_create_from_mem(can0_window, "can2", CANSKIN_PURPLE_BIN, 0, 0,
                                                        0, 0);
    time0 = gui_img_create_from_mem(can0_window, "time0", display_time_resource_def[2], 16, 32, 0, 0);


    can1_window = gui_win_create(view, "can_window", 155, 190, 64, 116);
    can1 = gui_img_create_from_mem(can1_window, "can1", CAN_BIN, 0, 0, 0, 0);
    gui_img_t *CanSkin_yellow = gui_img_create_from_mem(can1_window, "can2", CANSKIN_YELLOW_BIN, 0, 0,
                                                        0, 0);
    time1 = gui_img_create_from_mem(can1_window, "can1", display_time_resource_def[0], 16, 32, 0, 0);

    can2_window = gui_win_create(view, "can_window", 240, 190, 64, 116);
    can2 = gui_img_create_from_mem(can2_window, "can2", CAN_BIN, 0, 0, 0, 0);
    gui_img_t *CanSkin_green = gui_img_create_from_mem(can2_window, "can2", CANSKIN_GREEN_BIN, 0, 0, 0,
                                                       0);
    time2 = gui_img_create_from_mem(can2_window, "can2", display_time_resource_def[2], 16, 32, 0, 0);

    can3_window = gui_win_create(view, "can_window", 330, 190, 64, 116);
    can3 = gui_img_create_from_mem(can3_window, "can3", CAN_BIN, 0, 0, 0, 0);
    gui_img_t *CanSkin_red = gui_img_create_from_mem(can3_window, "can2", CANSKIN_RED_BIN, 0, 0, 0, 0);
    time3 = gui_img_create_from_mem(can3_window, "can3", display_time_resource_def[5], 16, 32, 0, 0);

    wave_window = gui_win_create(view, "wave_window", 0, 0, 410, 502);
    waterface_bg = gui_img_create_from_mem(wave_window, "waterface", WATERFACTOR_BG_BIN, 0, 240, 0,
                                           244);
    gui_img_set_mode(waterface_bg, IMG_SRC_OVER_MODE);
    gui_img_set_opacity(waterface_bg, 100);
    extern void gui_wave_draw_graph(gui_canvas_t *canvas);
    canvas_wave = gui_canvas_create(wave_window, "canvas", 0, 0, 0, 410, 502);
    // gui_canvas_set_canvas_cb(canvas, gui_wave_draw_graph);
    gui_canvas_set_canvas_cb(canvas_wave, NULL);
    extern void can_animate_cb(void *parent);

    //bubble
    bubble_window = gui_win_create(view, "bubble_window", 0, 240, 410, 240);

    gui_obj_create_timer(&(wave_window->base), 17, true, wave_animate_cb);
    gui_obj_start_timer(&(wave_window->base));
    gui_obj_create_timer(&(can0_window->base), 17, true, can_animate_cb);
    gui_obj_start_timer(&(can0_window->base));
    gui_obj_create_timer(&(can1_window->base), 17, true, can_animate_cb);
    gui_obj_start_timer(&(can1_window->base));
    gui_obj_create_timer(&(can2_window->base), 17, true, can_animate_cb);
    gui_obj_start_timer(&(can2_window->base));
    gui_obj_create_timer(&(can3_window->base), 17, true, can_animate_cb);
    gui_obj_start_timer(&(can3_window->base));
    gui_obj_create_timer(&(fish_window->base), 17, true, fish_animate_cb);
    gui_obj_start_timer(&(fish_window->base));

}
#if defined _WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static int app_init(void)
{

#if defined _WIN32
    extern int open(const char *file, int flags, ...);
    extern int read(int fd, void *buf, size_t len);
    extern int close(int fd);
    defaultPath = "realgui\\example\\screen_410_502\\root_image_hongkong\\root\\";
    int fd;
    fd = open("./example/widget/3d/SeaWaterDClock/root(0x253E400).bin", 0);

    if (fd > 0)
    {
        printf("open root(0x253E400).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x253E400).bin Fail!\n");
        printf("open root(0x253E400).bin Fail!\n");
        printf("open root(0x253E400).bin Fail!\n");
        return 0;
    }
#endif

    gui_view_create(gui_obj_get_root(), &descriptor, 0, 0, 0, 0);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* gui setWater example end*/
