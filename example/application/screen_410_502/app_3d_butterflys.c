/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "app_main_watch.h"
#include "gui_view.h"
#include "gui_lite3d.h"
#include "gui_canvas_rect.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "butterflys_view"
#define DEG_TO_RAD(angle) ((angle) * M_PI_F / 180.0f)
#define MAX_PARTICLES 20
#define BUTTERFLY_COUNT 3
#define RADIUS 45

/*============================================================================*
*                             Types
*============================================================================*/
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

typedef struct
{
    float x;
    float y;
    float angle;
} Butterfly_Pos;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_ui_butterflys_design(gui_view_t *view);
static void free_particles_resources(gui_view_t *view);
static void update_butterfly_wing_bg(gui_lite3d_t *lite3d_butterfly, gui_win_t *win);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_butterflys_design,
    .on_switch_out = free_particles_resources,

    .keep = false,
};

/* Particle Window */
static gui_win_t *butterfly_win[BUTTERFLY_COUNT] = {NULL};

/* Butterfly */
static Butterfly_Pos butterfly_pos[BUTTERFLY_COUNT];
static gui_lite3d_t *lite3d_butterflies[BUTTERFLY_COUNT] = {NULL};

static float butterfly_angle = 0.0f;
static float butterfly_time = 0.0f;
const static float angular_velocity = 1.3f; // Adjust as needed for speed

/* Particles */
static Particle *particles[BUTTERFLY_COUNT] = {NULL};
static uint32_t last_particle_spawn = 0;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
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

static int x_to_screen_w(float butterfly_x)
{
    float a = -65.0f;
    float b = 65.0f;
    float c = 0.0f;
    float d = gui_get_screen_width();

    float screen_x = c + ((butterfly_x - a) / (b - a)) * (d - c);
    return (int)screen_x;
}

static int y_to_screen_h(float butterfly_y)
{
    float a = -68.0f;
    float b = 68.0f;
    float c = 0.0f;
    float d = gui_get_screen_height();

    float screen_y = c + ((butterfly_y - a) / (b - a)) * (d - c);
    return (int)screen_y;
}

static void butterfly_pos_init()
{
    const float initial_angles[BUTTERFLY_COUNT] = {0, 120, 240};

    for (int i = 0; i < BUTTERFLY_COUNT; i++)
    {
        float rad = DEG_TO_RAD(initial_angles[i]);
        butterfly_pos[i] = (Butterfly_Pos)
        {
            .x = RADIUS * cosf(rad),
             .y = RADIUS * sinf(rad),
              .angle = initial_angles[i]
        };
    }

}

static void update_butterfly()
{
    butterfly_time += 1.2f;
    butterfly_angle = 30 * sinf(butterfly_time);

    for (int i = 0; i < BUTTERFLY_COUNT; i++)
    {
        butterfly_pos[i].angle += angular_velocity;
        if (butterfly_pos[i].angle > 360.0f)
        {
            butterfly_pos[i].angle -= 360.0f;
        }
        float rad = DEG_TO_RAD(butterfly_pos[i].angle);

        butterfly_pos[i].x = RADIUS * cosf(rad);
        butterfly_pos[i].y = RADIUS * sinf(rad);

        update_butterfly_wing_bg(lite3d_butterflies[i], butterfly_win[i]);
    }


}

static void butterfly0_global_cb(l3_model_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 70), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, butterfly_pos[0].x, butterfly_pos[0].y, 70, 0, 0,
                        180 + butterfly_pos[0].angle,
                        3);
}

static void butterfly1_global_cb(l3_model_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 70), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, butterfly_pos[1].x, butterfly_pos[1].y, 70, 0, 0,
                        180 + butterfly_pos[1].angle,
                        3);
}

static void butterfly2_global_cb(l3_model_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 70), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, butterfly_pos[2].x, butterfly_pos[2].y, 70, 0, 0,
                        180 + butterfly_pos[2].angle,
                        3);
}

static l3_4x4_matrix_t butterfly_face_cb(l3_model_t *this, size_t face_index/*face offset*/)
{
    l3_4x4_matrix_t face_matrix;
    l3_4x4_matrix_t transform_matrix;

    if (face_index == 0 || face_index == 2)
    {
        l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0), l3_4d_vector(0, 1, 0),
                                 butterfly_angle, 1);
    }
    else if (face_index == 1 || face_index == 3)
    {
        l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0), l3_4d_vector(0, 1, 0),
                                 -butterfly_angle, 1);
    }
    else
    {
        l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0), l3_4d_vector(0, 1, 0), 0,
                                 1);
    }

    l3_4x4_matrix_mul(&this->world, &face_matrix, &transform_matrix);

    return transform_matrix;
}

static void update_wing_position_and_scale(gui_img_t *wing, l3_vertex_t vertexes[],
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
            gui_img_translate(wing, (-wing->base.w / 2.0f) * 0.3f, -wing->base.h / 2.0f * 0.3f);
        }
        else
        {
            wing->base.x = (int16_t)(centerX + 20); // Adjust by 25 as per your scaling offset requirement
            wing->base.y = (int16_t)(centerY + 20); // Adjust by 25 similarly
            scale_factor = fmaxf(width, height) / 30.0f;
            gui_img_scale(wing, scale_factor, scale_factor);
            gui_img_translate(wing, -wing->base.w / 2.0f * 1.5f, -wing->base.h / 2.0f * 1.6f);
        }
    }

    gui_img_set_opacity(wing, 180);
}

static void update_butterfly_wing_bg(gui_lite3d_t *lite3d_butterfly, gui_win_t *win)
{
    GUI_WIDGET_POINTER_BY_NAME_ROOT(wing1, "wing1", GUI_BASE(win))
    GUI_WIDGET_POINTER_BY_NAME_ROOT(wing2, "wing2", GUI_BASE(win))
    GUI_WIDGET_POINTER_BY_NAME_ROOT(wing3, "wing3", GUI_BASE(win))
    GUI_WIDGET_POINTER_BY_NAME_ROOT(wing4, "wing4", GUI_BASE(win))
    gui_obj_hidden(wing1, false);
    gui_obj_hidden(wing2, false);
    gui_obj_hidden(wing3, false);
    gui_obj_hidden(wing4, false);

    // Define a reasonable base scale factor

    // Update each wing with calculated position and scale
    update_wing_position_and_scale((gui_img_t *)wing1,
                                   lite3d_butterfly->model->face.rect_face[0].transform_vertex, 0);
    update_wing_position_and_scale((gui_img_t *)wing2,
                                   lite3d_butterfly->model->face.rect_face[1].transform_vertex, 1);
    update_wing_position_and_scale((gui_img_t *)wing3,
                                   lite3d_butterfly->model->face.rect_face[2].transform_vertex, 2);
    update_wing_position_and_scale((gui_img_t *)wing4,
                                   lite3d_butterfly->model->face.rect_face[3].transform_vertex, 3);
}

// Calculate butterfly tail position based on current position and rotation
static void get_butterfly_tail_position(gui_lite3d_t *lite3d_butterfly,
                                        Butterfly_Pos *butterfly_pos, float *tail_x, float *tail_y)
{
    (void)lite3d_butterfly;
    float screen_x = x_to_screen_w(butterfly_pos->x);
    float screen_y = y_to_screen_h(butterfly_pos->y);

    float angle_rad = (butterfly_pos->angle) * M_PI_F / 180.0f;
    float offset_x = sinf(angle_rad) * 20.0f;
    float offset_y = -cosf(angle_rad) * 20.0f;

    float random_offset_x = (xorshift16() % 10 - 5) * 0.5f; // -2.5 ~ +2.5
    float random_offset_y = (xorshift16() % 10 - 5) * 0.5f;

    *tail_x = screen_x + offset_x + random_offset_x;
    *tail_y = screen_y + offset_y + random_offset_y;

    if (*tail_x < 0) { *tail_x = 0; }
    if (*tail_x > gui_get_screen_width()) { *tail_x = gui_get_screen_width(); }
    if (*tail_y < 0) { *tail_y = 0; }
    if (*tail_y > gui_get_screen_height()) { *tail_y = gui_get_screen_height(); }
}

static void spawn_particle(gui_obj_t *parent, Particle *particles, gui_lite3d_t *lite3d_butterfly,
                           Butterfly_Pos *butterfly_pos, uint32_t current_time)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        // Find an available particle slot
        if (particles[i].life <= 0.0f)
        {
            // Get butterfly tail position
            float tail_x;
            float tail_y;
            get_butterfly_tail_position(lite3d_butterfly, butterfly_pos, &tail_x, &tail_y);

            // Initialize particle at butterfly tail
            particles[i].x = tail_x;
            particles[i].y = tail_y;
            particles[i].life = 1.0f; // Full life

            {
                float angle_rad = butterfly_pos->angle * M_PI_F / 180.0f;
                particles[i].direction_x = cosf(angle_rad) * -0.5f;
                particles[i].direction_y = sinf(angle_rad) * -0.5f;
                particles[i].direction_x += (xorshift16() % 100 - 100) * 0.001f;
                particles[i].direction_y += (xorshift16() % 100 - 100) * 0.001f;
            }

            particles[i].scale = 0.1f + (xorshift16() % 5) / 10.0f;
            particles[i].spawn_time = current_time;

            // Create image if not already created
            if (particles[i].img == NULL)
            {
                particles[i].img = gui_img_create_from_mem(parent, 0,
                                                           FIREFLY_19_BIN, 0, 0, 0, 0);
                gui_img_scale(particles[i].img, particles[i].scale, particles[i].scale);
                gui_img_set_mode(particles[i].img, IMG_SRC_OVER_MODE);
            }

            // Position the image
            gui_img_translate(particles[i].img, particles[i].x, particles[i].y);

            break;
        }
    }
    // Update all particles
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].img == NULL) { break; }
        float gravity = 0.0f;
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

static void update_particles(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    uint32_t current_time = gui_ms_get();
    // Only spawn new particles every 50ms (adjust as needed)
    if (current_time - last_particle_spawn < 100)
    {
        return;
    }
    last_particle_spawn = current_time;
    // Spawn new particles when butterfly is moving
    for (int i = 0; i < BUTTERFLY_COUNT; i++)
    {
        spawn_particle(obj, particles[i], lite3d_butterflies[i], &butterfly_pos[i], current_time);
    }
}

static void init_butterfly_bg(gui_win_t *parent, Particle **particles)
{
    *particles = (Particle *)gui_malloc(MAX_PARTICLES * sizeof(Particle));
    if (!(*particles))
    {
        gui_log("Failed to allocate memory for particles0!\n");
        return;
    }
    else
    {
        memset(*particles, 0, MAX_PARTICLES * sizeof(Particle));
    }
    gui_img_t *butterfly_wing1 = gui_img_create_from_mem(parent, "wing1",
                                                         FIREFLY_19_BIN, 0, 0, 0, 0);
    gui_img_t *butterfly_wing2 = gui_img_create_from_mem(parent, "wing2",
                                                         FIREFLY_19_BIN, 0, 0, 0, 0);
    gui_img_t *butterfly_wing3 = gui_img_create_from_mem(parent, "wing3",
                                                         FIREFLY_19_BIN, 0, 0, 0, 0);
    gui_img_t *butterfly_wing4 = gui_img_create_from_mem(parent, "wing4",
                                                         FIREFLY_19_BIN, 0, 0, 0, 0);
    gui_img_set_mode(butterfly_wing1, IMG_SRC_OVER_MODE);
    gui_img_set_mode(butterfly_wing2, IMG_SRC_OVER_MODE);
    gui_img_set_mode(butterfly_wing3, IMG_SRC_OVER_MODE);
    gui_img_set_mode(butterfly_wing4, IMG_SRC_OVER_MODE);
    gui_obj_hidden(GUI_BASE(butterfly_wing1), true);
    gui_obj_hidden(GUI_BASE(butterfly_wing2), true);
    gui_obj_hidden(GUI_BASE(butterfly_wing3), true);
    gui_obj_hidden(GUI_BASE(butterfly_wing4), true);
}

static void free_particles_resources(gui_view_t *view)
{
    (void)view;
    for (int i = 0; i < BUTTERFLY_COUNT; i++)
    {
        if (particles[i])
        {
            gui_free(particles[i]);
            particles[i] = NULL;
        }
    }
}

static void app_ui_butterflys_design(gui_view_t *view)
{
    srand((uint32_t)gui_ms_get());
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_dispdev_t *dc = gui_get_dc();

    gui_canvas_rect_create(obj, 0, 0, 0, 410, 502, gui_rgba(30, 30, 30, 255));

    // Particles
    butterfly_win[0] = gui_win_create(obj, 0, 0, 0, dc->screen_width, dc->screen_height);
    butterfly_win[1] = gui_win_create(obj, 0, 0, 0, dc->screen_width, dc->screen_height);
    butterfly_win[2] = gui_win_create(obj, 0, 0, 0, dc->screen_width, dc->screen_height);

    init_butterfly_bg(butterfly_win[0], &particles[0]);
    init_butterfly_bg(butterfly_win[1], &particles[1]);
    init_butterfly_bg(butterfly_win[2], &particles[2]);

    // Butterflies
    l3_model_t *butterfly0 = l3_create_model(DESC_BUTTERFLY_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410, 502);
    l3_model_t *butterfly1 = l3_create_model(DESC_BUTTERFLY_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410, 502);
    l3_model_t *butterfly2 = l3_create_model(DESC_BUTTERFLY_BIN, L3_DRAW_FRONT_ONLY, 0, 0, 410, 502);

    butterfly_pos_init();
    l3_set_global_transform(butterfly0, (l3_global_transform_cb)butterfly0_global_cb);
    l3_set_face_transform(butterfly0, (l3_face_transform_cb)butterfly_face_cb);

    l3_set_global_transform(butterfly1, (l3_global_transform_cb)butterfly1_global_cb);
    l3_set_face_transform(butterfly1, (l3_face_transform_cb)butterfly_face_cb);

    l3_set_global_transform(butterfly2, (l3_global_transform_cb)butterfly2_global_cb);
    l3_set_face_transform(butterfly2, (l3_face_transform_cb)butterfly_face_cb);

    lite3d_butterflies[0] = gui_lite3d_create(obj, "lite3d_butterfly0", butterfly0, 0, 0, 410, 502);
    lite3d_butterflies[1] = gui_lite3d_create(obj, "lite3d_butterfly1", butterfly1, 0, 0, 410, 502);
    lite3d_butterflies[2] = gui_lite3d_create(obj, "lite3d_butterfly2", butterfly2, 0, 0, 410, 502);

    gui_obj_create_timer(GUI_BASE(lite3d_butterflies[0]), 17, true, update_butterfly);


    gui_img_t *clock_img = gui_img_create_from_mem(obj, "img_clock", CLOCK_BIN, 26,
                                                   (gui_get_screen_height() - 189) / 3, 0, 0);
    gui_img_set_mode(clock_img, IMG_SRC_OVER_MODE);


    gui_win_t *particles_win = gui_win_create(obj, 0, 0, 0, dc->screen_width,
                                              dc->screen_height);
    gui_obj_create_timer(&(particles_win->base), 17, true, update_particles);
}

