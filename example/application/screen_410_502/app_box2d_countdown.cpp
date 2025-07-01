#include "gui_win.h"
#include "gui_img.h"
#include "box2d/box2d.h"
#include <vector>
#include <random>
#include <chrono>
#include "gui_canvas.h"
#include "tp_algo.h"
#include "gui_canvas_rect.h"
#include "gui_view.h"
#include "app_hongkong.h"

#define CURRENT_VIEW_NAME "box2d_countdown_view"
extern "C" {
    static gui_view_t *current_view = NULL;
    const static gui_view_descriptor_t *menu_view = NULL;
    void app_box2d_time_ui_design(gui_view_t *view);
    static void clear_mem(gui_view_t *view);

    static gui_view_descriptor_t const descriptor =
    {
        /* change Here for current view */
        .name = (const char *)CURRENT_VIEW_NAME,
        .pView = &current_view,
        .on_switch_in = app_box2d_time_ui_design,
        .on_switch_out = clear_mem,
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
}


namespace app_box2d_countdown
{
const float TIMESTEP = 1.0f / 60.0f;
const int VELOCITY_ITERATIONS = 6;
const int POSITION_ITERATIONS = 2;
const float PIXELS_PER_METER = 30.0f;
const float PARTICLE_RADIUS = 6.0f;
const float PARTICLE_DENSITY = 0.3f;
const float PARTICLE_RESTITUTION = 0.3f;
const NVGcolor BACKGROUND_COLOR = nvgRGB(30, 30, 30);

const int COUNTDOWN_START = 30; // Countdown from 30
const float COUNTDOWN_INTERVAL = 1.0f; // Updated every second

// Explosion effect parameters
const float EXPLOSION_FORCE = 100.0f;
const float EXPLOSION_RADIUS = 2.0f;

gui_obj_t *parent;
b2World *world = nullptr;
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

struct Particle
{
    b2Body *body;
    gui_img_t *img;
    void *img_data;
    b2Vec2 originalPos;
    float opacity;
    float scaleFactor;
    bool isActive;
} Particle_t;

#define MAX_PARTICLES_IMG 140
static Particle *particles_img = NULL;
static int active_particle_count = 0;

int currentNumber = COUNTDOWN_START;
float countdownTimer = COUNTDOWN_INTERVAL;
bool isExploding = false;
float explosionTimer = 0.0f;

static uint16_t xorshift16_state = 1;

uint16_t xorshift16()
{
    uint16_t x = xorshift16_state;
    x ^= x << 7;
    x ^= x >> 9;
    x ^= x << 8;
    xorshift16_state = x;
    return x;
}

void particle_canvas_callback(NVGcontext *vg)
{
    NVGcolor color = nvgRGB(xorshift16() % 256, xorshift16() % 256, xorshift16() % 256);
    nvgBeginPath(vg);
    nvgCircle(vg, PARTICLE_RADIUS, PARTICLE_RADIUS, PARTICLE_RADIUS);
    nvgFillColor(vg, color);
    nvgFill(vg);
}

void initParticlesPool()
{
    particles_img = (Particle *)gui_malloc(MAX_PARTICLES_IMG * sizeof(Particle));
    if (!particles_img)
    {
        gui_log("Failed to allocate memory for particles_img!\n");
        return;
    }
    size_t buffer_size = PARTICLE_RADIUS * PARTICLE_RADIUS * 4 * GUI_CANVAS_OUTPUT_RGBA * 4 + sizeof(
                             gui_rgb_data_head_t);

    for (int i = 0; i < MAX_PARTICLES_IMG; i++)
    {
        particles_img[i].img_data = (uint8_t *)gui_lower_malloc(buffer_size);
        memset(particles_img[i].img_data, 0, buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, PARTICLE_RADIUS * 2,
                                          PARTICLE_RADIUS * 2, particle_canvas_callback, (uint8_t *)particles_img[i].img_data);
        particles_img[i].isActive = false;
    }
}

void resetParticles()
{
    for (int i = 0; i < MAX_PARTICLES_IMG; i++)
    {
        if (particles_img[i].isActive)
        {
            if (particles_img[i].body)
            {
                world->DestroyBody(particles_img[i].body);
                particles_img[i].body = nullptr;
            }
            if (particles_img[i].img)
            {
                gui_obj_tree_free(particles_img[i].img);
                particles_img[i].img = nullptr;
            }
            particles_img[i].isActive = false;
        }
    }
    active_particle_count = 0;
}

Particle *getAvailableParticle()
{
    for (int i = 0; i < MAX_PARTICLES_IMG; i++)
    {
        if (!particles_img[i].isActive)
        {
            particles_img[i].isActive = true;
            active_particle_count++;
            return &particles_img[i];
        }
    }
    return nullptr;
}

std::vector<b2Vec2> getNumberShape(int number)
{
    std::vector<b2Vec2> shape;
    float scale = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.3f;
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;

    const float segLen = scale * 0.8f;
    const float hOffset = segLen * 0.5f;
    const float vOffset = segLen * 0.8f;
    const float additionalSpacing = segLen * 1.3f; // Adjusted spacing

    const bool digitSegments[10][7] =
    {
        {1, 1, 1, 1, 1, 1, 0}, // 0
        {0, 1, 1, 0, 0, 0, 0}, // 1
        {1, 1, 0, 1, 1, 0, 1}, // 2
        {1, 1, 1, 1, 0, 0, 1}, // 3
        {0, 1, 1, 0, 0, 1, 1}, // 4
        {1, 0, 1, 1, 0, 1, 1}, // 5
        {1, 0, 1, 1, 1, 1, 1}, // 6
        {1, 1, 1, 0, 0, 0, 0}, // 7
        {1, 1, 1, 1, 1, 1, 1}, // 8
        {1, 1, 1, 1, 0, 1, 1} // 9
    };

    auto addSegment = [&](const b2Vec2 & start, const b2Vec2 & end)
    {
        b2Vec2 dir = end - start;
        float length = dir.Normalize();
        int steps = (int)(length / (PARTICLE_RADIUS * 1.5f));
        for (int i = 0; i <= steps; i++)
        {
            float t = (float)i / steps;
            shape.push_back(start + t * length * dir);
        }
    };

    if (number < 0 || number > 99) { return shape; }

    int tens = number / 10;
    int units = number % 10;

    const float firstDigitCenterX = centerX - additionalSpacing / 2;
    const bool *firstSegments = digitSegments[tens];
    // first data
    if (firstSegments[0]) { addSegment(b2Vec2(firstDigitCenterX - hOffset, centerY - vOffset), b2Vec2(firstDigitCenterX + hOffset, centerY - vOffset)); }
    if (firstSegments[1]) { addSegment(b2Vec2(firstDigitCenterX + hOffset, centerY - vOffset), b2Vec2(firstDigitCenterX + hOffset, centerY)); }
    if (firstSegments[2]) { addSegment(b2Vec2(firstDigitCenterX + hOffset, centerY), b2Vec2(firstDigitCenterX + hOffset, centerY + vOffset)); }
    if (firstSegments[3]) { addSegment(b2Vec2(firstDigitCenterX - hOffset, centerY + vOffset), b2Vec2(firstDigitCenterX + hOffset, centerY + vOffset)); }
    if (firstSegments[4]) { addSegment(b2Vec2(firstDigitCenterX - hOffset, centerY), b2Vec2(firstDigitCenterX - hOffset, centerY + vOffset)); }
    if (firstSegments[5]) { addSegment(b2Vec2(firstDigitCenterX - hOffset, centerY - vOffset), b2Vec2(firstDigitCenterX - hOffset, centerY)); }
    if (firstSegments[6]) { addSegment(b2Vec2(firstDigitCenterX - hOffset, centerY), b2Vec2(firstDigitCenterX + hOffset, centerY)); }

    const float secondDigitCenterX = centerX + additionalSpacing / 2;
    const bool *secondSegments = digitSegments[units];
    // second data
    if (secondSegments[0]) { addSegment(b2Vec2(secondDigitCenterX - hOffset, centerY - vOffset), b2Vec2(secondDigitCenterX + hOffset, centerY - vOffset)); }
    if (secondSegments[1]) { addSegment(b2Vec2(secondDigitCenterX + hOffset, centerY - vOffset), b2Vec2(secondDigitCenterX + hOffset, centerY)); }
    if (secondSegments[2]) { addSegment(b2Vec2(secondDigitCenterX + hOffset, centerY), b2Vec2(secondDigitCenterX + hOffset, centerY + vOffset)); }
    if (secondSegments[3]) { addSegment(b2Vec2(secondDigitCenterX - hOffset, centerY + vOffset), b2Vec2(secondDigitCenterX + hOffset, centerY + vOffset)); }
    if (secondSegments[4]) { addSegment(b2Vec2(secondDigitCenterX - hOffset, centerY), b2Vec2(secondDigitCenterX - hOffset, centerY + vOffset)); }
    if (secondSegments[5]) { addSegment(b2Vec2(secondDigitCenterX - hOffset, centerY - vOffset), b2Vec2(secondDigitCenterX - hOffset, centerY)); }
    if (secondSegments[6]) { addSegment(b2Vec2(secondDigitCenterX - hOffset, centerY), b2Vec2(secondDigitCenterX + hOffset, centerY)); }

    return shape;
}

void createNumber(int number)
{
    resetParticles();

    // Create particles for new numbers
    auto numberShape = getNumberShape(number);
    // gui_log("Creating number %d with %d particles\n", number, numberShape.size());

    for (const auto &pos : numberShape)
    {
        Particle *p = getAvailableParticle();
        if (!p)
        {
            gui_log("No more particles available!\n");
            break;
        }

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(pos.x / PIXELS_PER_METER, pos.y / PIXELS_PER_METER);
        p->body = world->CreateBody(&bodyDef);

        b2CircleShape circle;
        circle.m_radius = PARTICLE_RADIUS / PIXELS_PER_METER;

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circle;
        fixtureDef.density = PARTICLE_DENSITY;
        fixtureDef.restitution = PARTICLE_RESTITUTION;
        p->body->CreateFixture(&fixtureDef);

        p->img = gui_img_create_from_mem(parent, 0, p->img_data,
                                         (int)(pos.x - PARTICLE_RADIUS),
                                         (int)(pos.y - PARTICLE_RADIUS),
                                         0, 0);
        p->originalPos = pos;
        p->opacity = 255.0f;
        p->scaleFactor = 1.0f;
    }
}

void explode()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> forceDist(-1.0f, 1.0f);

    for (int i = 0; i < MAX_PARTICLES_IMG; i++)
    {
        if (particles_img[i].isActive)
        {
            b2Vec2 randomDir(forceDist(gen), forceDist(gen));
            randomDir.Normalize();
            particles_img[i].body->ApplyLinearImpulse(EXPLOSION_FORCE * 0.5f * randomDir,
                                                      particles_img[i].body->GetWorldCenter(), true);
            particles_img[i].body->SetLinearDamping(1.0f);
            particles_img[i].body->SetAngularDamping(1.0f);
            particles_img[i].scaleFactor = 1.0f;
        }
    }

    isExploding = true;
    explosionTimer = 0.5f; // explosion hold time
}

void resetToNumber(int number)
{
    createNumber(number);
    currentNumber = number;
    countdownTimer = 0.1;
}

void app_box2d_cb(void *obj)
{
    // update world
    world->Step(TIMESTEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> scaleDist(0.5f, 1.0f);

    for (int i = 0; i < MAX_PARTICLES_IMG; i++)
    {
        if (particles_img[i].isActive)
        {
            float x = particles_img[i].body->GetPosition().x * PIXELS_PER_METER - PARTICLE_RADIUS;
            float y = particles_img[i].body->GetPosition().y * PIXELS_PER_METER - PARTICLE_RADIUS;

            if (isExploding && particles_img[i].opacity > 0)
            {
                particles_img[i].opacity -= 5;
                particles_img[i].opacity = std::max(0.0f, particles_img[i].opacity);
                gui_img_set_opacity(particles_img[i].img, static_cast<int>(particles_img[i].opacity));

                float scaleFactor = scaleDist(gen);
                gui_img_scale(particles_img[i].img, scaleFactor, scaleFactor);

                GUI_BASE(particles_img[i].img)->x = x;
                GUI_BASE(particles_img[i].img)->y = y;
            }
            else if (!isExploding && currentNumber > 0)
            {
                GUI_BASE(particles_img[i].img)->x = x;
                GUI_BASE(particles_img[i].img)->y = y;
            }
        }
    }

    // Handle countdown logic
    if (!isExploding)
    {
        countdownTimer -= TIMESTEP;
        if (countdownTimer <= 0)
        {
            if (currentNumber > 0)
            {
                explode();
            }
        }
    }
    else
    {
        explosionTimer -= TIMESTEP;
        if (explosionTimer <= 0)
        {
            isExploding = false;
            for (int i = 0; i < MAX_PARTICLES_IMG; i++)
            {
                if (particles_img[i].isActive)
                {
                    gui_obj_hidden(GUI_BASE(particles_img[i].img), true);
                }
            }

            if (currentNumber > 0)
            {
                resetToNumber(currentNumber - 1);
            }
        }
    }
}

bool init()
{
    GUI_WIDGET_TRY_EXCEPT(parent)

    gui_win_t *win = gui_win_create(parent, "Countdown", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!win) { return false; }

    // background
    gui_canvas_rect_create(parent, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                           gui_rgba(BACKGROUND_COLOR.r * 255, BACKGROUND_COLOR.g * 255,
                                    BACKGROUND_COLOR.b * 255, 255));

    gui_obj_create_timer(GUI_BASE(win), 16, true, app_box2d_cb);
    gui_obj_start_timer(GUI_BASE(win));

    return true;
}

void close()
{
    if (world)
    {
        {
            for (int i = 0; i < MAX_PARTICLES_IMG; i++)
            {
                if (particles_img[i].isActive)
                {
                    if (particles_img[i].body)
                    {
                        world->DestroyBody(particles_img[i].body);
                        particles_img[i].body = nullptr;
                    }
                    particles_img[i].isActive = false;
                }
                if (particles_img[i].img_data)
                {
                    gui_lower_free(particles_img[i].img_data);
                    particles_img[i].img_data = nullptr;
                }
            }
            active_particle_count = 0;
        }
        if (particles_img)
        {
            gui_free(particles_img);
            particles_img = NULL;
        }

        world->~b2World();
        gui_free(world);
        world = nullptr;
    }
}

int ui_design(gui_obj_t *obj)
{
    parent = obj;
    SCREEN_WIDTH = gui_get_screen_width();
    SCREEN_HEIGHT = gui_get_screen_height();
    isExploding = false;


    // Initialize particles pool
    initParticlesPool();

    b2Vec2 gravity(0.0f, 0.0f);
    world = new (gui_malloc(sizeof(b2World))) b2World(gravity);
    resetToNumber(COUNTDOWN_START);

    if (!init())
    {
        return 1;
    }
    return 0;
}
}

extern "C" {
    static void return_cb()
    {
        gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
    // static void return_timer_cb(void *obj)
    // {
    //     touch_info_t *tp = tp_get_info();
    //     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_cb)
    // }

    static void clear_mem(gui_view_t *view)
    {
        app_box2d_countdown::close();
    }

    void app_box2d_time_ui_design(gui_view_t *view)
    {
        gui_obj_t *obj = GUI_BASE(view);
        gui_win_t *win = gui_win_create(view, "win_time", 0, 0, 0, 0);
        gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_CLICKED);
        // gui_obj_create_timer(GUI_BASE(win), 10, true, return_timer_cb);

        app_box2d_countdown::ui_design(obj);
    }
}
