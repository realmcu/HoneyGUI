#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"
#include "app_hongkong.h"
#include "gui_view.h"
#include "gui_list.h"
#include "gui_3d.h"

#define CURRENT_VIEW_NAME "butterfly_particle_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static void butterfly_particle_app(gui_view_t *view);
static void free_particles_resources(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = butterfly_particle_app,
    .on_switch_out = free_particles_resources,
};

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void return_to_menu()
{
    gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }

//buterrfly_shadow app demo
static gui_win_t *butterfly_win = NULL;
gui_img_t *butterfly_wing1 = NULL;
gui_img_t *butterfly_wing2 = NULL;
gui_img_t *butterfly_wing3 = NULL;
gui_img_t *butterfly_wing4 = NULL;
gui_3d_t *butterfly_particle_3d = NULL;

static float wing_angle = 0.0f;
static float wing_time = 0.0f;
static float butterfly_x = 0.0f;
static float butterfly_y = 0.0f;
static float butterfly_z = 0.0f;
static float butterfly_rz = 0.0f;


typedef enum
{
    FLY_STOP,
    FLY_DOING,
    FLY_RETURNING
} FlyState;


static const float RETURN_DELAY = 500.0f;    //ms
static const float RETURN_DURATION = 1000.0f;

static FlyState fly_state = FLY_STOP;
static uint32_t touch_end_time = 0;
static uint32_t return_start_time = 0;
static const float MIN_RIGHT_FLY_TIME = 5000.0f; //Shortest right flight time ms

static uint32_t fly_right_start_time = 0;

int x_to_screen_w(float butterfly_x)
{
    float a = 30.0f;   //left fish's max x (most left)
    float b = -40.0f;  //right fish's min x (most right)
    float c = 0.0f;    //set the screen's left edge to 0
    float d = gui_get_screen_width();  // set the screen's right edge to 410

    float screen_x = c + ((butterfly_x - a) / (b - a)) * (d - c);

    if (screen_x < 0) { screen_x = 0; }
    if (screen_x > 410) { screen_x = 410; }

    return (int)screen_x;
}
int y_to_screen_h(float butterfly_y)
{
    float a = 40.0f;
    float b = -50.0f;
    float c = 0.0f;
    float d = gui_get_screen_height();  // set the screen's right edge to 410

    float screen_y = c + ((butterfly_y - a) / (b - a)) * (d - c);

    if (screen_y < 0) { screen_y = 0; }
    if (screen_y > 502) { screen_y = 502; }

    return (int)screen_y;
}
void update_wing_position_and_scale(gui_img_t *wing, gui_3d_vertex_t vertexes[],
                                    uint8_t face_index)
{
    // Calculate the center of the four vertices
    float centerX = (vertexes[0].position.x + vertexes[1].position.x + vertexes[2].position.x +
                     vertexes[3].position.x) / 4.0f;
    float centerY = (vertexes[0].position.y + vertexes[1].position.y + vertexes[2].position.y +
                     vertexes[3].position.y) / 4.0f;

    wing->base.x = (int16_t)(centerX - 25); // Adjust by 25 as per your scaling offset requirement
    wing->base.y = (int16_t)(centerY - 25); // Adjust by 25 similarly

    // Calculate width and height of the face
    float width = sqrtf(powf(vertexes[1].position.x - vertexes[0].position.x, 2) +
                        powf(vertexes[1].position.y - vertexes[0].position.y, 2));
    float height = sqrtf(powf(vertexes[3].position.y - vertexes[0].position.y, 2) +
                         powf(vertexes[3].position.x - vertexes[0].position.x, 2));

    // Determine scale factor based on largest dimension
    float scale_factor = 0.0; // Adjust 50.0 based on expected size

    // Set the scale
    {
        if (face_index == 2 || face_index == 3)
        {
            scale_factor = fmaxf(width, height) / 40.0f;
            gui_img_scale(wing, scale_factor, scale_factor);
            gui_img_translate(wing, (-wing->base.w / 2) * 0.4, -wing->base.h / 2 * 0.4);
        }
        else
        {
            scale_factor = fmaxf(width, height) / 20.0f;
            gui_img_scale(wing, scale_factor, scale_factor);
            gui_img_translate(wing, -wing->base.w / 2 * 1.8, -wing->base.h / 2 * 1.8);
        }
    }



    gui_img_set_opacity(wing, 180);
}

void update_butterfly_wing_bg()
{
    gui_obj_hidden(GUI_BASE(butterfly_wing1), false);
    gui_obj_hidden(GUI_BASE(butterfly_wing2), false);
    gui_obj_hidden(GUI_BASE(butterfly_wing3), false);
    gui_obj_hidden(GUI_BASE(butterfly_wing4), false);

    // Define a reasonable base scale factor

    // Update each wing with calculated position and scale
    update_wing_position_and_scale(butterfly_wing1,
                                   butterfly_particle_3d->face.rect_face[0].transform_vertex, 0);
    update_wing_position_and_scale(butterfly_wing2,
                                   butterfly_particle_3d->face.rect_face[1].transform_vertex, 1);
    update_wing_position_and_scale(butterfly_wing3,
                                   butterfly_particle_3d->face.rect_face[2].transform_vertex, 2);
    update_wing_position_and_scale(butterfly_wing4,
                                   butterfly_particle_3d->face.rect_face[3].transform_vertex, 3);
}

static void update_butterfly()
{
    touch_info_t *tp = tp_get_info();
    uint32_t current_time = gui_ms_get();
    float progress = 0.0f;

    if (tp->pressed)
    {
        fly_state = FLY_DOING;
        touch_end_time = 0;
        fly_right_start_time = current_time;
    }
    else if (fly_state == FLY_DOING)
    {
        if (touch_end_time == 0)
        {
            touch_end_time = current_time;
        }
        if (current_time - touch_end_time > RETURN_DELAY &&
            current_time - fly_right_start_time >= MIN_RIGHT_FLY_TIME)
        {
            fly_state = FLY_RETURNING;
            return_start_time = current_time;
        }
    }

    switch (fly_state)
    {
    case FLY_STOP:
        butterfly_rz = 0.0f;
        wing_time += 0.1f;
        wing_angle = 50.0f * sinf(wing_time);
        butterfly_z = -5.0f * sinf(wing_time);
        break;

    case FLY_DOING:
        butterfly_rz = 90.0f + sinf(current_time * 0.005f) * 15.0f;
        wing_time += 0.3f;
        wing_angle = 70.0f * sinf(wing_time);
        butterfly_z = 2.0f * sinf(wing_time * 1.5f);
        break;

    case FLY_RETURNING:
        progress = (current_time - return_start_time) / RETURN_DURATION;

        if (progress >= 1.0f)
        {
            progress = 1.0f;
            fly_state = FLY_STOP;
        }

        float ease_progress = 1.0f - cosf(progress * M_PI_F / 2.0f);
        butterfly_rz = 90.0f * (1.0f - ease_progress);

        wing_time += 0.1f + 0.15f * (1.0f - ease_progress);
        wing_angle = (40.0f + 15.0f * (1.0f - ease_progress)) * sinf(wing_time);

        butterfly_z = (-5.0f + 13.0f * (1.0f - ease_progress)) * sinf(wing_time);
        break;
    }
    update_butterfly_wing_bg();
}

static void butterfly_particle_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 40), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, butterfly_x, butterfly_y, 40.0f - butterfly_z, 0, 0,
                             butterfly_rz,
                             4.5);

}

static gui_3d_matrix_t butterfly_particle_face_cb(gui_3d_t *this, size_t face_index/*face offset*/)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    if (face_index == 0 || face_index == 2)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 wing_angle, 1);
    }
    else if (face_index == 1 || face_index == 3)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 -wing_angle, 1);
    }
    else
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0), 0,
                                 1);
    }

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;

}

typedef struct
{
    float x;          // Current x position of the particle
    float y;          // Current y position of the particle
    float direction_x ;
    float direction_y ;
    float life;       // Life time of the particle (0.0-1.0)
    float scale;      // Scale of the particle
    gui_img_t *img;   // Image object
    uint32_t spawn_time; // Time when particle was spawned
} Particle;

#define MAX_PARTICLES 100
static Particle *particles = NULL;
static gui_win_t *particle_win = NULL;
static uint32_t last_particle_spawn = 0;

#define VIRTUAL_PATH_LENGTH 10
static gui_point2f_t virtual_path[VIRTUAL_PATH_LENGTH] =
{
    {200.0f, 250.0f}, {190.0f, 270.0f}, {185.0f, 250.0f}, {180.0f, 280.0f}, {200.0f, 250.0f},
    {190.0f, 280.0f}, {200.0f, 250.0f}, {200.0f, 270.0f}, {200.0f, 250.0f}, {200.0f, 270.0f}
};

static int current_virtual_index = 0;
static uint32_t last_virtual_update_time = 0;
static float virtual_butterfly_x = 0.0f;
static float virtual_butterfly_y = 0.0f;

void update_virtual_position(uint32_t current_time)
{
    // Update every 100 ms or adjust as needed
    if (current_time - last_virtual_update_time > 100)
    {
        last_virtual_update_time = current_time;
        current_virtual_index = (current_virtual_index + 1) % VIRTUAL_PATH_LENGTH;

        // Update virtual positions without affecting actual butterfly's position
        virtual_butterfly_x = virtual_path[current_virtual_index].x;
        virtual_butterfly_y = virtual_path[current_virtual_index].y;
    }
}
// Calculate butterfly tail position based on current position and rotation
static void get_butterfly_tail_position(float *tail_x, float *tail_y)
{
    // Convert angle to radians
    float angle_rad = (butterfly_rz - 90) * M_PI_F / 180.0f;

    // Tail is behind the butterfly (offset by -20 pixels in the direction it's facing)
    switch (fly_state)
    {
    case FLY_STOP:
        *tail_x = x_to_screen_w(butterfly_x) + cosf(angle_rad) * (rand() % 20 - 5);
        *tail_y = y_to_screen_h(butterfly_y) + sinf(angle_rad) * (rand() % 20 - 10);
        break;
    case FLY_RETURNING:
    case FLY_DOING:
        *tail_x = (virtual_butterfly_x) + cosf(angle_rad) * -20.0f;
        *tail_y = (virtual_butterfly_y) + sinf(angle_rad) * -20.0f;
        break;

    }

}

static void spawn_particle()
{
    uint32_t current_time = gui_ms_get();

    // Only spawn new particles every 50ms (adjust as needed)
    if (fly_state == FLY_DOING || fly_state == FLY_RETURNING)
    {
        if (current_time - last_particle_spawn < 50)
        {
            return;
        }
    }
    else
    {
        if (current_time - last_particle_spawn < 200)
        {
            return;
        }
    }

    last_particle_spawn = current_time;

    // Find an available particle slot
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].life <= 0.0f)
        {
            // Get butterfly tail position
            float tail_x, tail_y;
            get_butterfly_tail_position(&tail_x, &tail_y);

            // Initialize particle at butterfly tail
            particles[i].x = tail_x;
            particles[i].y = tail_y;
            particles[i].life = 1.0f; // Full life

            if (fly_state == FLY_DOING || fly_state == FLY_RETURNING)
            {
                //In flight, particles have initial upward and leftward velocities
                particles[i].direction_x = -(rand() % 30 + 15) * 0.1f;
                particles[i].direction_y = -(rand() % 20 + 5) * 0.1f;
            }
            else
            {
                particles[i].direction_x = (rand() % 200 - 100) * 0.01f;
                particles[i].direction_y = (rand() % 200 - 100) * 0.01f;
            }

            particles[i].scale = 0.1f + (rand() % 5) / 10.0f;
            particles[i].spawn_time = current_time;

            // Create image if not already created
            if (particles[i].img == NULL)
            {
                particles[i].img = gui_img_create_from_mem(particle_win, "particle",
                                                           FIREFLY_14_BIN, 0, 0, 0, 0);
                gui_img_scale(particles[i].img, particles[i].scale, particles[i].scale);
                gui_img_set_mode(particles[i].img, IMG_SRC_OVER_MODE);
            }

            // Position the image
            gui_img_translate(particles[i].img, particles[i].x, particles[i].y);

            break;
        }
    }
}
static void update_particles()
{
    uint32_t current_time = gui_ms_get();
    update_virtual_position(current_time);
    float gravity = 0.1f;

    // Spawn new particles when butterfly is moving
    if (fly_state == FLY_DOING || fly_state == FLY_RETURNING)
    {
        // stop_up_particles();
        spawn_particle();
        gravity = 0.0f;
    }
    if (fly_state == FLY_STOP)
    {
        // stop_up_particles();
        spawn_particle();
        gravity = 0.0f;
    }
    // Update all particles
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].life > 0.0f)
        {
            // Calculate age (0.0-1.0)
            float age = (current_time - particles[i].spawn_time) / 2000.0f; // 2 second lifespan
            particles[i].life = 1.0f - age;

            if (particles[i].life <= 0.0f)
            {
                // Particle is dead, hide it
                gui_obj_hidden(GUI_BASE(particles[i].img), true);
                continue;
            }

            particles[i].direction_y += gravity;

            // Apply air resistance - slow down horizontal movement
            particles[i].direction_x *= 0.98f;

            particles[i].x += particles[i].direction_x;
            particles[i].y += particles[i].direction_y;

            gui_obj_hidden(GUI_BASE(particles[i].img), false);

            // Scale down as particle ages
            float current_scale = particles[i].scale * (0.5f + 0.5f * particles[i].life);
            gui_img_scale(particles[i].img, current_scale, current_scale);

            // Update position
            gui_img_translate(particles[i].img, particles[i].x, particles[i].y);
        }
    }
}
extern void *text_num_array[];
extern const char *day[];
static char date_text_content[10];
static void time_update_cb(void *p)
{
    int millisecond = 0;
#ifdef __WIN32
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    uint16_t seconds = timeinfo->tm_sec;
    uint16_t minute = timeinfo->tm_min;
    uint16_t hour = timeinfo->tm_hour;
#else
    // extern struct tm watch_clock_get(void);
    // struct tm watch_time = watch_clock_get();
    // uint16_t seconds = watch_time.tm_sec;
    // uint16_t minute = watch_time.tm_min;
    // uint16_t hour = watch_time.tm_hour;

    extern struct tm *timeinfo;
#endif
    GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "date_text", current_view);
    sprintf(date_text_content, "%s %d", day[timeinfo->tm_wday], timeinfo->tm_mday);
    gui_text_content_set((gui_text_t *)text, date_text_content, strlen(date_text_content));

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_decimal, "watch_hour_decimal", current_view);
    gui_img_set_image_data((gui_img_t *)img_hour_decimal, text_num_array[timeinfo->tm_hour / 10]);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_single, "watch_hour_single", current_view);
    gui_img_set_image_data((gui_img_t *)img_hour_single, text_num_array[timeinfo->tm_hour % 10]);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_decimal, "watch_minute_decimal", current_view);
    gui_img_set_image_data((gui_img_t *)img_minute_decimal,
                           text_num_array[timeinfo->tm_min / 10]);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_single, "watch_minute_single", current_view);
    gui_img_set_image_data((gui_img_t *)img_minute_single, text_num_array[timeinfo->tm_min % 10]);
}
static void data_create(void *p)
{
    gui_obj_t *obj = GUI_BASE(p);
    gui_win_t *data_win = gui_win_create(obj, "particle_win", 0, 0, 410, 502);
    sprintf(date_text_content, "FRI 16");
    gui_text_t *text = gui_text_create(data_win, "date_text", -40, 33, 0, 0);
    gui_text_set(text, (void *)date_text_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(date_text_content),
                 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, RIGHT);
    gui_text_rendermode_set(text, 2);
    {
        int text_w = 35;
        gui_img_t *img = gui_img_create_from_mem(data_win, "watch_hour_decimal", text_num_array[0],
                                                 211, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(data_win, "watch_hour_single", text_num_array[0],
                                      211 + text_w, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(data_win, "colon", text_num_array[10],
                                      211 + text_w * 2 + 5, 88 + 5, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(data_win, "watch_minute_decimal", text_num_array[0],
                                      211 + text_w * 2 + 17, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(data_win, "watch_minute_single", text_num_array[0],
                                      211 + text_w * 3 + 17, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    gui_obj_create_timer(&(data_win->base), 3000, true, time_update_cb);
}
static void butterfly_particle_app(gui_view_t *view)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
    // date & time text
    data_create(obj);

    butterfly_win = gui_win_create(obj, "butterfly_win", 0, 0, dc->screen_width, dc->screen_height);


    butterfly_wing3 = gui_img_create_from_mem(butterfly_win, "particle",
                                              FIREFLY_14_BIN, x_to_screen_w(0), y_to_screen_h(0), 0, 0);
    butterfly_wing4 = gui_img_create_from_mem(butterfly_win, "particle",
                                              FIREFLY_14_BIN, x_to_screen_w(0), y_to_screen_h(0), 0, 0);
    butterfly_wing1 = gui_img_create_from_mem(butterfly_win, "particle",
                                              FIREFLY_14_BIN, x_to_screen_w(0), y_to_screen_h(0), 0, 0);
    butterfly_wing2 = gui_img_create_from_mem(butterfly_win, "particle",
                                              FIREFLY_14_BIN, x_to_screen_w(0), y_to_screen_h(0), 0, 0);
    gui_img_set_mode(butterfly_wing1, IMG_SRC_OVER_MODE);
    gui_img_set_mode(butterfly_wing2, IMG_SRC_OVER_MODE);
    gui_img_set_mode(butterfly_wing3, IMG_SRC_OVER_MODE);
    gui_img_set_mode(butterfly_wing4, IMG_SRC_OVER_MODE);
    butterfly_particle_3d = gui_3d_create(butterfly_win, "3d-widget", DESC_BUTTERFLY_BIN,
                                          GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                                          410, 502);
    particle_win = gui_win_create(obj, "particle_win", 0, 0, dc->screen_width, dc->screen_height);


    gui_3d_set_global_transform_cb(butterfly_particle_3d,
                                   (gui_3d_global_transform_cb)butterfly_particle_global_cb);
    gui_3d_set_face_transform_cb(butterfly_particle_3d,
                                 (gui_3d_face_transform_cb)butterfly_particle_face_cb);

    gui_obj_create_timer(&(butterfly_particle_3d->base), 17, true, update_butterfly);

    particles = (Particle *)gui_malloc(MAX_PARTICLES * sizeof(Particle));
    if (!particles)
    {
        gui_log("Failed to allocate memory for particles!\n");
        return;
    }
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].life = 0.0f;
        particles[i].img = NULL;
    }

    gui_obj_hidden(GUI_BASE(butterfly_wing1), true);
    gui_obj_hidden(GUI_BASE(butterfly_wing2), true);
    gui_obj_hidden(GUI_BASE(butterfly_wing3), true);
    gui_obj_hidden(GUI_BASE(butterfly_wing4), true);

    gui_obj_create_timer(&(particle_win->base), 17, true, update_particles);
}
static void free_particles_resources(gui_view_t *view)
{
    if (particles)
    {
        gui_free(particles);
        particles = NULL;
    }
}
