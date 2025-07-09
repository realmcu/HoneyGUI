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
#include <math.h>
#include "gui_canvas_rect.h"

static gui_win_t *butterfly0_window = NULL;
static gui_win_t *butterfly1_window = NULL;
static gui_win_t *butterfly2_window = NULL;
static gui_win_t *butterfly0_wing_win = NULL;
static gui_win_t *butterfly1_wing_win = NULL;
static gui_win_t *butterfly2_wing_win = NULL;
static gui_img_t *butterfly0_wing1 = NULL;
static gui_img_t *butterfly0_wing2 = NULL;
static gui_img_t *butterfly0_wing3 = NULL;
static gui_img_t *butterfly0_wing4 = NULL;

static gui_img_t *butterfly1_wing1 = NULL;
static gui_img_t *butterfly1_wing2 = NULL;
static gui_img_t *butterfly1_wing3 = NULL;
static gui_img_t *butterfly1_wing4 = NULL;

static gui_img_t *butterfly2_wing1 = NULL;
static gui_img_t *butterfly2_wing2 = NULL;
static gui_img_t *butterfly2_wing3 = NULL;
static gui_img_t *butterfly2_wing4 = NULL;

static gui_3d_t *butterfly0 = NULL;
static gui_3d_t *butterfly1 = NULL;
static gui_3d_t *butterfly2 = NULL;

static float butterfly_angle = 0.0f;

static float butterfly0_x = 0.0f;
static float butterfly0_y = 0.0f;

static float butterfly1_x = 0;
static float butterfly1_y = 0;

static float butterfly2_x = 0;
static float butterfly2_y = 0;

static float butterfly0_rz = 0.0f;
static float butterfly1_rz = 0.0f;
static float butterfly2_rz = 0.0f;

static void update_butterfly_wing_bg(gui_3d_t *butterfly);

#define CURRENT_VIEW_NAME "butterflys_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static void app_ui_butterflys_design(gui_view_t *view);
static void free_particles_resources(gui_view_t *view);

static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_butterflys_design,
    .on_switch_out = free_particles_resources,

    .keep = false,
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

#define CLOCK_RADIUS_X 60
#define CLOCK_RADIUS_Y 55
#define CLOCK_CENTER_X 0
#define CLOCK_CENTER_Y 10

static float butterfly_time = 0.0f;
static float rotation_factor = 60.0f;
static int x_to_screen_w(float butterfly_x)
{
    float a = 75.0f;
    float b =  -75.0f;
    float c = 0.0f;
    float d = gui_get_screen_width();

    float screen_x = c + ((butterfly_x - a) / (b - a)) * (d - c);
    return (int)screen_x;
}

static int y_to_screen_h(float butterfly_y)
{
    float a = 70.0f;
    float b = -70.0f;
    float c = 0.0f;
    float d = gui_get_screen_height();

    float screen_y = c + ((butterfly_y - a) / (b - a)) * (d - c);
    return (int)screen_y;
}
float angular_velocity = 0.03f; // Adjust as needed for speed
void update_butterfly0()
{
    static float theta0 = 0.0f; // starting angle for butterfly0

    // Updating position for each butterfly using the circular path calculation
    butterfly0_x = CLOCK_CENTER_X + CLOCK_RADIUS_X * cosf(theta0);
    butterfly0_y = CLOCK_CENTER_Y + CLOCK_RADIUS_Y * sinf(theta0);

    butterfly0_rz = theta0 * rotation_factor + sinf(theta0 * 5) * 2.0f;
    // gui_log("butterfly0_rz %f\n",butterfly0_rz);

    theta0 += angular_velocity;

    if (theta0 >= 2 * M_PI_F) { theta0 -= 2 * M_PI_F; }

    butterfly_time += 1.2f;
    butterfly_angle = 30 * sinf(butterfly_time);
    update_butterfly_wing_bg(butterfly0);
}
void update_butterfly1()
{
    static float theta1 = 2 * M_PI_F / 3; // starting angle for butterfly1

    butterfly1_x = CLOCK_CENTER_X + CLOCK_RADIUS_X * cosf(theta1);
    butterfly1_y = CLOCK_CENTER_Y + CLOCK_RADIUS_Y * sinf(theta1);
    butterfly1_rz = theta1 * rotation_factor + sinf(theta1 * 5) * 0.8f;

    theta1 += angular_velocity;

    if (theta1 >= 2 * M_PI_F) { theta1 -= 2 * M_PI_F; }
    update_butterfly_wing_bg(butterfly1);
}
void update_butterfly2()
{
    static float theta2 = M_PI_F + M_PI_F / 3; // starting angle for butterfly2

    butterfly2_x = CLOCK_CENTER_X + CLOCK_RADIUS_X * cosf(theta2);
    butterfly2_y = CLOCK_CENTER_Y + CLOCK_RADIUS_Y * sinf(theta2);
    butterfly2_rz = theta2 * rotation_factor + sinf(theta2 * 5) * 0.8f;

    theta2 += angular_velocity;

    if (theta2 >= 2 * M_PI_F) { theta2 -= 2 * M_PI_F; }
    update_butterfly_wing_bg(butterfly2);
}

static void butterfly0_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, -butterfly0_x, -butterfly0_y, 80, 0, 0, butterfly0_rz,
                             4);
}
static void butterfly1_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, -butterfly1_x, -butterfly1_y, 80, 0, 0, butterfly1_rz,
                             4);
}

static void butterfly2_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, -butterfly2_x, -butterfly2_y, 80, 0, 0, butterfly2_rz,
                             4);
}

static gui_3d_matrix_t butterfly_face_cb(gui_3d_t *this, size_t face_index/*face offset*/)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    if (face_index == 0 || face_index == 2)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 butterfly_angle, 1);
    }
    else if (face_index == 1 || face_index == 3)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 -butterfly_angle, 1);
    }
    else
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0), 0,
                                 1);
    }

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;
}
static void update_wing_position_and_scale(gui_img_t *wing, gui_3d_vertex_t vertexes[],
                                           uint8_t face_index)
{
    // Calculate the center of the four vertices
    float centerX = (vertexes[0].position.x + vertexes[1].position.x + vertexes[2].position.x +
                     vertexes[3].position.x) / 4.0f;
    float centerY = (vertexes[0].position.y + vertexes[1].position.y + vertexes[2].position.y +
                     vertexes[3].position.y) / 4.0f;

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
            wing->base.x = (int16_t)(centerX - 10); // Adjust by 25 as per your scaling offset requirement
            wing->base.y = (int16_t)(centerY - 10); // Adjust by 25 similarly
            scale_factor = fmaxf(width, height) / 45.0f;
            gui_img_scale(wing, scale_factor, scale_factor);
            gui_img_translate(wing, (-wing->base.w / 2) * 0.3f, -wing->base.h / 2 * 0.3f);
        }
        else
        {
            wing->base.x = (int16_t)(centerX + 20); // Adjust by 25 as per your scaling offset requirement
            wing->base.y = (int16_t)(centerY + 20); // Adjust by 25 similarly
            scale_factor = fmaxf(width, height) / 30.0f;
            gui_img_scale(wing, scale_factor, scale_factor);
            gui_img_translate(wing, -wing->base.w / 2 * 1.5f, -wing->base.h / 2 * 1.6f);
        }
    }

    gui_img_set_opacity(wing, 180);
}

static void update_butterfly_wing_bg(gui_3d_t *butterfly)
{
    if (butterfly == butterfly0)
    {
        gui_obj_hidden(GUI_BASE(butterfly0_wing1), false);
        gui_obj_hidden(GUI_BASE(butterfly0_wing2), false);
        gui_obj_hidden(GUI_BASE(butterfly0_wing3), false);
        gui_obj_hidden(GUI_BASE(butterfly0_wing4), false);

        // Define a reasonable base scale factor

        // Update each wing with calculated position and scale
        update_wing_position_and_scale(butterfly0_wing1,
                                       butterfly0->face.rect_face[0].transform_vertex, 0);
        update_wing_position_and_scale(butterfly0_wing2,
                                       butterfly0->face.rect_face[1].transform_vertex, 1);
        update_wing_position_and_scale(butterfly0_wing3,
                                       butterfly0->face.rect_face[2].transform_vertex, 2);
        update_wing_position_and_scale(butterfly0_wing4,
                                       butterfly0->face.rect_face[3].transform_vertex, 3);
    }
    else if (butterfly == butterfly1)
    {
        gui_obj_hidden(GUI_BASE(butterfly1_wing1), false);
        gui_obj_hidden(GUI_BASE(butterfly1_wing2), false);
        gui_obj_hidden(GUI_BASE(butterfly1_wing3), false);
        gui_obj_hidden(GUI_BASE(butterfly1_wing4), false);

        // Define a reasonable base scale factor

        // Update each wing with calculated position and scale
        update_wing_position_and_scale(butterfly1_wing1,
                                       butterfly1->face.rect_face[0].transform_vertex, 0);
        update_wing_position_and_scale(butterfly1_wing2,
                                       butterfly1->face.rect_face[1].transform_vertex, 1);
        update_wing_position_and_scale(butterfly1_wing3,
                                       butterfly1->face.rect_face[2].transform_vertex, 2);
        update_wing_position_and_scale(butterfly1_wing4,
                                       butterfly1->face.rect_face[3].transform_vertex, 3);
    }
    else if (butterfly == butterfly2)
    {
        gui_obj_hidden(GUI_BASE(butterfly2_wing1), false);
        gui_obj_hidden(GUI_BASE(butterfly2_wing2), false);
        gui_obj_hidden(GUI_BASE(butterfly2_wing3), false);
        gui_obj_hidden(GUI_BASE(butterfly2_wing4), false);

        // Define a reasonable base scale factor

        // Update each wing with calculated position and scale
        update_wing_position_and_scale(butterfly2_wing1,
                                       butterfly2->face.rect_face[0].transform_vertex, 0);
        update_wing_position_and_scale(butterfly2_wing2,
                                       butterfly2->face.rect_face[1].transform_vertex, 1);
        update_wing_position_and_scale(butterfly2_wing3,
                                       butterfly2->face.rect_face[2].transform_vertex, 2);
        update_wing_position_and_scale(butterfly2_wing4,
                                       butterfly2->face.rect_face[3].transform_vertex, 3);
    }
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

#define MAX_PARTICLES 20
static Particle *particles = NULL;
static Particle *particles1 = NULL;
static Particle *particles2 = NULL;
static uint32_t last_particle_spawn = 0;
static uint32_t last_particle_spawn1 = 0;
static uint32_t last_particle_spawn2 = 0;

// Calculate butterfly tail position based on current position and rotation
static void get_butterfly_tail_position(gui_3d_t *butterfly, float *tail_x, float *tail_y)
{
    float screen_x = 0.0;
    float screen_y = 0.0;
    if (butterfly == butterfly0)
    {
        screen_x = x_to_screen_w(butterfly0_x);
        screen_y = y_to_screen_h(butterfly0_y);
    }
    else if (butterfly == butterfly1)
    {
        screen_x = x_to_screen_w(butterfly1_x);
        screen_y = y_to_screen_h(butterfly1_y);
    }
    else if (butterfly == butterfly2)
    {
        screen_x = x_to_screen_w(butterfly2_x);
        screen_y = y_to_screen_h(butterfly2_y);
    }

    float angle_rad = (butterfly0_rz - 90) * M_PI_F / 180.0f;
    float offset_x = cosf(angle_rad) * 10.0f;
    float offset_y = sinf(angle_rad) * 10.0f;

    float random_offset_x = (rand() % 10 - 5) * 0.5f; // -2.5 ~ +2.5
    float random_offset_y = (rand() % 10 - 5) * 0.5f;

    *tail_x = screen_x + offset_x + random_offset_x;
    *tail_y = screen_y + offset_y + random_offset_y;

    if (*tail_x < 0) { *tail_x = 0; }
    if (*tail_x > gui_get_screen_width()) { *tail_x = gui_get_screen_width(); }
    if (*tail_y < 0) { *tail_y = 0; }
    if (*tail_y > gui_get_screen_height()) { *tail_y = gui_get_screen_height(); }
}

static void spawn_particle()
{
    uint32_t current_time = gui_ms_get();

    // Only spawn new particles every 50ms (adjust as needed)
    if (current_time - last_particle_spawn < 50)
    {
        return;
    }

    last_particle_spawn = current_time;

    // Find an available particle slot
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].life <= 0.0f)
        {
            // Get butterfly tail position
            float tail_x, tail_y;
            get_butterfly_tail_position(butterfly0, &tail_x, &tail_y);

            // Initialize particle at butterfly tail
            particles[i].x = tail_x;
            particles[i].y = tail_y;
            particles[i].life = 1.0f; // Full life

            {
                float angle_rad = (butterfly0_rz - 90) * M_PI_F / 180.0f;
                particles[i].direction_x = cosf(angle_rad) * -0.5f;
                particles[i].direction_y = sinf(angle_rad) * -0.5f;
                particles[i].direction_x += (rand() % 100 - 100) * 0.001f;
                particles[i].direction_y += (rand() % 100 - 100) * 0.001f;
            }

            particles[i].scale = 0.1f + (rand() % 5) / 10.0f;
            particles[i].spawn_time = current_time;

            // Create image if not already created
            if (particles[i].img == NULL)
            {
                particles[i].img = gui_img_create_from_mem(butterfly0_wing_win, "particle0",
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
static void spawn_particle1()
{
    uint32_t current_time = gui_ms_get();

    // Only spawn new particles every 50ms (adjust as needed)
    if (current_time - last_particle_spawn1 < 50)
    {
        return;
    }

    last_particle_spawn1  = current_time;

    // Find an available particle slot
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles1[i].life <= 0.0f)
        {
            // Get butterfly tail position
            float tail_x, tail_y;
            get_butterfly_tail_position(butterfly1, &tail_x, &tail_y);

            // Initialize particle at butterfly tail
            particles1[i].x = tail_x;
            particles1[i].y = tail_y;
            particles1[i].life = 1.0f; // Full life

            {
                float angle_rad = (butterfly0_rz - 90) * M_PI_F / 180.0f;
                particles1[i].direction_x = cosf(angle_rad) * -0.5f;
                particles1[i].direction_y = sinf(angle_rad) * -0.5f;
                particles1[i].direction_x += (rand() % 100 - 100) * 0.001f;
                particles1[i].direction_y += (rand() % 100 - 100) * 0.001f;
            }

            particles1[i].scale = 0.1f + (rand() % 5) / 10.0f;
            particles1[i].spawn_time = current_time;

            // Create image if not already created
            if (particles1[i].img == NULL)
            {
                particles1[i].img = gui_img_create_from_mem(butterfly1_wing_win, "particle1",
                                                            FIREFLY_14_BIN, 0, 0, 0, 0);
                gui_img_scale(particles1[i].img, particles1[i].scale, particles1[i].scale);
                gui_img_set_mode(particles1[i].img, IMG_SRC_OVER_MODE);
            }

            // Position the image
            gui_img_translate(particles1[i].img, particles1[i].x, particles1[i].y);

            break;
        }
    }
}
static void spawn_particle2()
{
    uint32_t current_time = gui_ms_get();

    // Only spawn new particles every 50ms (adjust as needed)
    if (current_time - last_particle_spawn2 < 50)
    {
        return;
    }

    last_particle_spawn2  = current_time;

    // Find an available particle slot
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles2[i].life <= 0.0f)
        {
            // Get butterfly tail position
            float tail_x, tail_y;
            get_butterfly_tail_position(butterfly2, &tail_x, &tail_y);

            // Initialize particle at butterfly tail
            particles2[i].x = tail_x;
            particles2[i].y = tail_y;
            particles2[i].life = 1.0f; // Full life

            {
                float angle_rad = (butterfly0_rz - 90) * M_PI_F / 180.0f;
                particles2[i].direction_x = cosf(angle_rad) * -0.5f;
                particles2[i].direction_y = sinf(angle_rad) * -0.5f;
                particles2[i].direction_x += (rand() % 100 - 100) * 0.001f;
                particles2[i].direction_y += (rand() % 100 - 100) * 0.001f;
            }

            particles2[i].scale = 0.1f + (rand() % 5) / 10.0f;
            particles2[i].spawn_time = current_time;

            // Create image if not already created
            if (particles2[i].img == NULL)
            {
                particles2[i].img = gui_img_create_from_mem(butterfly2_wing_win, "particle2",
                                                            FIREFLY_14_BIN, 0, 0, 0, 0);
                gui_img_scale(particles2[i].img, particles2[i].scale, particles2[i].scale);
                gui_img_set_mode(particles2[i].img, IMG_SRC_OVER_MODE);
            }

            // Position the image
            gui_img_translate(particles2[i].img, particles2[i].x, particles2[i].y);

            break;
        }
    }
}
static void update_particles()
{
    uint32_t current_time = gui_ms_get();

    float gravity = 0.0f;

    // Spawn new particles when butterfly is moving
    spawn_particle();
    // Update all particles
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].life > 0.0f)
        {
            // Calculate age (0.0-1.0)
            float age = (current_time - particles[i].spawn_time) / 800.0f; // 1 second lifespan
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
static void update_particles1()
{
    uint32_t current_time = gui_ms_get();

    float gravity = 0.0f;

    // Spawn new particles when butterfly is moving
    spawn_particle1();
    // Update all particles
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles1[i].life > 0.0f)
        {
            // Calculate age (0.0-1.0)
            float age = (current_time - particles1[i].spawn_time) / 800.0f; // 1 second lifespan
            particles1[i].life = 1.0f - age;

            if (particles[i].life <= 0.0f)
            {
                // Particle is dead, hide it
                gui_obj_hidden(GUI_BASE(particles1[i].img), true);
                continue;
            }

            particles1[i].direction_y += gravity;

            // Apply air resistance - slow down horizontal movement
            particles1[i].direction_x *= 0.98f;

            particles1[i].x += particles1[i].direction_x;
            particles1[i].y += particles1[i].direction_y;

            gui_obj_hidden(GUI_BASE(particles1[i].img), false);

            // Scale down as particle ages
            float current_scale = particles1[i].scale * (0.5f + 0.5f * particles1[i].life);
            gui_img_scale(particles1[i].img, current_scale, current_scale);

            // Update position
            gui_img_translate(particles1[i].img, particles1[i].x, particles1[i].y);
        }
    }
}
static void update_particles2()
{
    uint32_t current_time = gui_ms_get();

    float gravity = 0.0f;

    // Spawn new particles when butterfly is moving
    spawn_particle2();
    // Update all particles
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles2[i].life > 0.0f)
        {
            // Calculate age (0.0-1.0)
            float age = (current_time - particles2[i].spawn_time) / 800.0f; // 1 second lifespan
            particles2[i].life = 1.0f - age;

            if (particles[i].life <= 0.0f)
            {
                // Particle is dead, hide it
                gui_obj_hidden(GUI_BASE(particles2[i].img), true);
                continue;
            }

            particles2[i].direction_y += gravity;

            // Apply air resistance - slow down horizontal movement
            particles2[i].direction_x *= 0.98f;

            particles2[i].x += particles2[i].direction_x;
            particles2[i].y += particles2[i].direction_y;

            gui_obj_hidden(GUI_BASE(particles2[i].img), false);

            // Scale down as particle ages
            float current_scale = particles2[i].scale * (0.5f + 0.5f * particles2[i].life);
            gui_img_scale(particles2[i].img, current_scale, current_scale);

            // Update position
            gui_img_translate(particles2[i].img, particles2[i].x, particles2[i].y);
        }
    }
}
static void init_butterfly_bg(gui_3d_t *butterfly)
{
    particles = (Particle *)gui_malloc(MAX_PARTICLES * sizeof(Particle));
    if (!particles)
    {
        gui_log("Failed to allocate memory for particles!\n");
        return;
    }
    particles1 = (Particle *)gui_malloc(MAX_PARTICLES * sizeof(Particle));
    if (!particles1)
    {
        gui_log("Failed to allocate memory for particles1!\n");
        return;
    }
    particles2 = (Particle *)gui_malloc(MAX_PARTICLES * sizeof(Particle));
    if (!particles2)
    {
        gui_log("Failed to allocate memory for particles2!\n");
        return;
    }
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].life = 0.0f;
        particles[i].img = NULL;
        particles1[i].life = 0.0f;
        particles1[i].img = NULL;
        particles2[i].life = 0.0f;
        particles2[i].img = NULL;
    }
    if (butterfly == butterfly0)
    {
        butterfly0_wing3 = gui_img_create_from_mem(butterfly0_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        butterfly0_wing4 = gui_img_create_from_mem(butterfly0_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        butterfly0_wing1 = gui_img_create_from_mem(butterfly0_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        butterfly0_wing2 = gui_img_create_from_mem(butterfly0_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        gui_img_set_mode(butterfly0_wing1, IMG_SRC_OVER_MODE);
        gui_img_set_mode(butterfly0_wing2, IMG_SRC_OVER_MODE);
        gui_img_set_mode(butterfly0_wing3, IMG_SRC_OVER_MODE);
        gui_img_set_mode(butterfly0_wing4, IMG_SRC_OVER_MODE);
        gui_obj_hidden(GUI_BASE(butterfly0_wing1), true);
        gui_obj_hidden(GUI_BASE(butterfly0_wing2), true);
        gui_obj_hidden(GUI_BASE(butterfly0_wing3), true);
        gui_obj_hidden(GUI_BASE(butterfly0_wing4), true);
    }
    else if (butterfly == butterfly1)
    {
        butterfly1_wing3 = gui_img_create_from_mem(butterfly1_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        butterfly1_wing4 = gui_img_create_from_mem(butterfly1_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        butterfly1_wing1 = gui_img_create_from_mem(butterfly1_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        butterfly1_wing2 = gui_img_create_from_mem(butterfly1_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        gui_img_set_mode(butterfly1_wing1, IMG_SRC_OVER_MODE);
        gui_img_set_mode(butterfly1_wing2, IMG_SRC_OVER_MODE);
        gui_img_set_mode(butterfly1_wing3, IMG_SRC_OVER_MODE);
        gui_img_set_mode(butterfly1_wing4, IMG_SRC_OVER_MODE);

        gui_obj_hidden(GUI_BASE(butterfly1_wing1), true);
        gui_obj_hidden(GUI_BASE(butterfly1_wing2), true);
        gui_obj_hidden(GUI_BASE(butterfly1_wing3), true);
        gui_obj_hidden(GUI_BASE(butterfly1_wing4), true);
    }
    else if (butterfly == butterfly2)
    {
        butterfly2_wing3 = gui_img_create_from_mem(butterfly2_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        butterfly2_wing4 = gui_img_create_from_mem(butterfly2_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        butterfly2_wing1 = gui_img_create_from_mem(butterfly2_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        butterfly2_wing2 = gui_img_create_from_mem(butterfly2_wing_win, "particle",
                                                   FIREFLY_14_BIN, 0, 0, 0, 0);
        gui_img_set_mode(butterfly2_wing1, IMG_SRC_OVER_MODE);
        gui_img_set_mode(butterfly2_wing2, IMG_SRC_OVER_MODE);
        gui_img_set_mode(butterfly2_wing3, IMG_SRC_OVER_MODE);
        gui_img_set_mode(butterfly2_wing4, IMG_SRC_OVER_MODE);
        gui_obj_hidden(GUI_BASE(butterfly2_wing1), true);
        gui_obj_hidden(GUI_BASE(butterfly2_wing2), true);
        gui_obj_hidden(GUI_BASE(butterfly2_wing3), true);
        gui_obj_hidden(GUI_BASE(butterfly2_wing4), true);
    }
    else
    {
        GUI_ASSERT(NULL != NULL)
        return;
    }

}
static void app_ui_butterflys_design(gui_view_t *view)
{
    srand((uint32_t)gui_ms_get());
    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_dispdev_t *dc = gui_get_dc();

    gui_canvas_rect_create(obj, 0, 0, 0, 410, 502, gui_rgba(30, 30, 30, 255));

    butterfly0_wing_win = gui_win_create(obj, "butterfly0_win", 0, 0, dc->screen_width,
                                         dc->screen_height);
    butterfly1_wing_win = gui_win_create(obj, "butterfly1_win", 0, 0, dc->screen_width,
                                         dc->screen_height);
    butterfly2_wing_win = gui_win_create(obj, "butterfly0_win", 0, 0, dc->screen_width,
                                         dc->screen_height);

    butterfly0_window = gui_win_create(obj, "butterfly0_win", 0, 0, dc->screen_width,
                                       dc->screen_height);
    butterfly1_window = gui_win_create(obj, "butterfly1_win", 0, 0, dc->screen_width,
                                       dc->screen_height);
    butterfly2_window = gui_win_create(obj, "butterfly0_win", 0, 0, dc->screen_width,
                                       dc->screen_height);

    butterfly0 = gui_3d_create(butterfly0_window, "3d-butterfly0", DESC_BUTTERFLY_BIN,
                               GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                               0, 0);
    butterfly1 = gui_3d_create(butterfly1_window, "3d-butterfly1", DESC_BUTTERFLY_BIN,
                               GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                               0, 0);
    butterfly2 = gui_3d_create(butterfly2_window, "3d-butterfly2", DESC_BUTTERFLY_BIN,
                               GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                               0, 0);

    gui_3d_set_global_transform_cb(butterfly0, (gui_3d_global_transform_cb)butterfly0_global_cb);
    gui_3d_set_face_transform_cb(butterfly0, (gui_3d_face_transform_cb)butterfly_face_cb);

    gui_3d_set_global_transform_cb(butterfly1, (gui_3d_global_transform_cb)butterfly1_global_cb);
    gui_3d_set_face_transform_cb(butterfly1, (gui_3d_face_transform_cb)butterfly_face_cb);

    gui_3d_set_global_transform_cb(butterfly2, (gui_3d_global_transform_cb)butterfly2_global_cb);
    gui_3d_set_face_transform_cb(butterfly2, (gui_3d_face_transform_cb)butterfly_face_cb);


    gui_img_t *clock_img = gui_img_create_from_mem(obj, "img_clock", CLOCK_BIN, 26,
                                                   (gui_get_screen_height() - 189) / 3, 0, 0);
    gui_img_set_mode(clock_img, IMG_SRC_OVER_MODE);

    init_butterfly_bg(butterfly0);
    init_butterfly_bg(butterfly1);
    init_butterfly_bg(butterfly2);

    gui_obj_create_timer(&(butterfly0->base), 17, true, update_butterfly0);
    gui_obj_create_timer(&(butterfly1->base), 17, true, update_butterfly1);
    gui_obj_create_timer(&(butterfly2->base), 17, true, update_butterfly2);

    gui_obj_create_timer(&(butterfly0_wing_win->base), 17, true, update_particles);
    gui_obj_create_timer(&(butterfly1_wing_win->base), 17, true, update_particles1);
    gui_obj_create_timer(&(butterfly2_wing_win->base), 17, true, update_particles2);


}

static void free_particles_resources(gui_view_t *view)
{
    if (particles)
    {
        gui_free(particles);
        particles = NULL;
    }
    if (particles1)
    {
        gui_free(particles1);
        particles1 = NULL;
    }
    if (particles2)
    {
        gui_free(particles2);
        particles2 = NULL;
    }
}
