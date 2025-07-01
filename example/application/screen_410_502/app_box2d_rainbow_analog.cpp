/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_win.h"
#include "gui_img.h"
#include "box2d/box2d.h"
#include <iostream>
#include <cstdio>
#include <cmath>
#include <box2d/box2d.h>
#include <vector>
#include "gui_canvas.h"
#include "tp_algo.h"
#include <random>  // For secure random numbers
#include "gui_canvas_rect.h"
#include "gui_view.h"
#include "app_hongkong.h"
#include "root_image_hongkong/ui_resource.h"
#include <time.h>

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "rainbow_analog_view"

/*============================================================================*
 *                                  C Interface
 *============================================================================*/
extern "C" {
    static gui_view_t *current_view = NULL;
    const static gui_view_descriptor_t *menu_view = NULL;
    static void app_rainbow_analog_ui_design(gui_view_t *view);
    static void app_close(gui_view_t *view);

    static gui_view_descriptor_t const descriptor =
    {
        /* change Here for current view */
        .name = (const char *)CURRENT_VIEW_NAME,
        .pView = &current_view,
        .on_switch_in = app_rainbow_analog_ui_design,
        .on_switch_out = app_close,
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

/*============================================================================*
 *                             C++ Namespace
 *============================================================================*/
namespace app_rainbow_analog
{
/*============================================================================*
*                           Types
*============================================================================*/
struct Capsule
{
    b2Body *body;
    NVGcolor color;
    gui_img_t *img;
    float selfRandomRot; // Random self rotation angle

    float initialRadius;
    float currentPathAngle;

    bool need_recovery;
    float recoveryTimer;
};

struct HandState
{
    NVGcolor previousColor;
};

/*============================================================================*
*                              Variables
*============================================================================*/
/* GUI */
gui_obj_t *parent = nullptr;
const int SCREEN_WIDTH = 410;
const int SCREEN_HEIGHT = 502;

/* Box2D World */
const float TIMESTEP = 1.0f / 60.0f; // Timestep
const int VELOCITY_ITERATIONS = 8; // Velocity iterations
const int POSITION_ITERATIONS = 3; // Position iterations
const float PIXELS_PER_METER = 30.0f; // Pixels per meter
b2World *world = nullptr; // Box2D world

/* Capsules Management*/
const uint8_t CAPSULE_COLOR_COUNT = 8;
const uint8_t CAPSULES_PER_COLOR = 5;
const uint8_t CAPSULE_IMG_HALF_WIDTH = 20;
const uint8_t CAPSULE_IMG_HALF_HEIGHT = 7;
void *Capsule_Img[CAPSULE_COLOR_COUNT] =
{
    RAINBOWACLOCK_CAPSULE00_BIN,
    RAINBOWACLOCK_CAPSULE01_BIN,
    RAINBOWACLOCK_CAPSULE02_BIN,
    RAINBOWACLOCK_CAPSULE03_BIN,
    RAINBOWACLOCK_CAPSULE04_BIN,
    RAINBOWACLOCK_CAPSULE05_BIN,
    RAINBOWACLOCK_CAPSULE06_BIN,
    RAINBOWACLOCK_CAPSULE07_BIN,
};

const NVGcolor Capsule_Color[CAPSULE_COLOR_COUNT] =
{
    nvgRGB(1, 234, 23),
    nvgRGB(0, 122, 131),
    nvgRGB(4, 28, 224),
    nvgRGB(69, 4, 175),
    nvgRGB(192, 0, 62),
    nvgRGB(232, 23, 0),
    nvgRGB(190, 66, 1),
    nvgRGB(121, 137, 0),
};
std::vector<Capsule> capsules;
float OUTER_RING_RADIUS = 200;
const b2Vec2 CIRCLE_CENTER(SCREEN_WIDTH / 2.0f / PIXELS_PER_METER,
                           SCREEN_HEIGHT / 2.0f / PIXELS_PER_METER);

/* Clock Management*/
HandState secondHandState = { nvgRGB(0, 0, 0) };
HandState minuteHandState = { nvgRGB(0, 0, 0) };
HandState hourHandState = { nvgRGB(0, 0, 0) };
const uint8_t hourLength = 80;
const uint8_t minuteLength = 100;
const uint8_t secondLength = 130;
const float hourWidth = 10.0f;
const float minuteWidth = 7.0f;
const float secondWidth = 4.0f;
size_t buffer_size;
gui_img_t *clock_img = NULL;
uint8_t *clock_img_data = NULL;

/* Capsules Animation Variables */
const float EXPLOSION_RADIUS = 3.0f;  // Influence radius (meters)
const float EXPLOSION_POWER = 10.0f;  // Bouncing force
const float RECOVERY_TIME = 2.0f;     // Recovery time required (seconds)
const float GLOBAL_ANGULAR_VELOCITY = 0.3f;   // Angular velocity rad/s

/*============================================================================*
*                           Private Functions
*============================================================================*/
// Apply centripetal force to ensure capsules move along the ring
void apply_centripetal_force(Capsule *capsule)
{
    b2Vec2 toCenter = CIRCLE_CENTER - capsule->body->GetPosition(); // Vector from capsule to center
    float distance = toCenter.Length();
    if (distance < 0.1f)
    {
        distance = 0.1f; // avoid division by zero
    }

    // v = ω * r
    float requiredSpeed = GLOBAL_ANGULAR_VELOCITY * distance;

    // Rotate counterclockwise
    b2Vec2 tangentDir(-toCenter.y, toCenter.x);
    tangentDir.Normalize();
    capsule->body->SetLinearVelocity(requiredSpeed * tangentDir);
    capsule->body->SetAngularVelocity(-GLOBAL_ANGULAR_VELOCITY);

    toCenter.Normalize();
    float forceMagnitude = capsule->body->GetMass() * requiredSpeed * requiredSpeed /
                           distance; // Centripetal force magnitude
    capsule->body->ApplyForceToCenter(forceMagnitude * toCenter, true); // Apply force to capsule center

    // Adjust rotation to face the center
    float selfRot = atan2(toCenter.y, toCenter.x) * 180 / M_PI_F + 90;
    gui_img_rotation(capsule->img, selfRot + capsule->selfRandomRot);
}

void capsule_animation_cb(void *obj)
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        // Convert mouse coordinates to Box2D world coordinates
        b2Vec2 clickPoint(tp->x / PIXELS_PER_METER, tp->y / PIXELS_PER_METER);

        for (auto &capsule : capsules)
        {
            b2Vec2 capsulePos = capsule.body->GetPosition();
            float distance = (capsulePos - clickPoint).Length();

            if (distance < EXPLOSION_RADIUS && !capsule.need_recovery)
            {
                // Calculate direction pointing outward (from the click point to the capsule)
                b2Vec2 radialDir = capsulePos - clickPoint;
                radialDir.Normalize();

                // Compute attenuation force (force decreases with distance)
                float forceMagnitude = EXPLOSION_POWER * (1.0f - distance / EXPLOSION_RADIUS);

                // Apply impulse
                capsule.body->ApplyLinearImpulse(forceMagnitude * radialDir,
                                                 capsule.body->GetWorldCenter(),
                                                 true);
                capsule.need_recovery = true;
                capsule.recoveryTimer = 0.0f;

            }
        }
    }

    for (auto &capsule : capsules)
    {
        if (capsule.need_recovery)
        {
            capsule.recoveryTimer += TIMESTEP;
            float progress = std::min(capsule.recoveryTimer / RECOVERY_TIME, 1.0f);

            // Calculate the angle the capsule should have rotated during explosion
            float missedAngle = GLOBAL_ANGULAR_VELOCITY * capsule.recoveryTimer;

            b2Vec2 targetPos(
                CIRCLE_CENTER.x + capsule.initialRadius * cosf(capsule.currentPathAngle - missedAngle),
                CIRCLE_CENTER.y + capsule.initialRadius * sinf(capsule.currentPathAngle - missedAngle)
            );

            // Linear interpolation
            b2Vec2 currentPos = capsule.body->GetPosition();
            b2Vec2 newPos;
            newPos.x = currentPos.x + (targetPos.x - currentPos.x) * progress;
            newPos.y = currentPos.y + (targetPos.y - currentPos.y) * progress;

            // Recovering centripetal angle
            b2Vec2 toCenter = CIRCLE_CENTER - targetPos;
            float currentAngle = capsule.body->GetAngle() * 180 / M_PI_F;
            float targetRotAngle = atan2(toCenter.y, toCenter.x) * 180 / M_PI_F + 90 + capsule.selfRandomRot;
            // Normalize angle difference (ensuring it's in the range [-180, 180])
            float angleDiff = targetRotAngle - currentAngle;
            angleDiff = fmod(angleDiff + 180, 360) - 180;
            float newAngle = currentAngle + angleDiff * progress;

            capsule.body->SetTransform(newPos, newAngle * M_PI_F / 180.0f);
            gui_img_rotation(capsule.img, newAngle);

            if (progress >= 0.95f)
            {
                capsule.need_recovery = false;
                capsule.body->SetLinearVelocity(b2Vec2_zero);
                capsule.currentPathAngle -= missedAngle;
                capsule.body->SetTransform(targetPos, targetRotAngle * M_PI_F / 180.0f);
                gui_img_rotation(capsule.img, targetRotAngle);
            }

        }
        else
        {
            capsule.currentPathAngle -= GLOBAL_ANGULAR_VELOCITY * TIMESTEP;
            apply_centripetal_force(&capsule);
        }

        b2Vec2 pos = capsule.body->GetPosition();
        GUI_BASE(capsule.img)->x = pos.x * PIXELS_PER_METER;
        GUI_BASE(capsule.img)->y = pos.y * PIXELS_PER_METER;
    }

    if (world)
    {
        world->Step(TIMESTEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS); // Update the physics world
    }
}


void create_capsules(b2World *world)
{
    for (int i = 0; i < CAPSULE_COLOR_COUNT; i++)
    {
        // Calculate the reference angle (radians) for the current color group
        float baseAngle = 2 * M_PI_F * i / CAPSULE_COLOR_COUNT;

        for (int j = 0; j < CAPSULES_PER_COLOR; j++)
        {
            Capsule capsule;

            capsule.img = gui_img_create_from_mem(parent, "heart", Capsule_Img[i], 0, 0, 0, 0);
            capsule.color = Capsule_Color[i];

            // Random angle and radius offset
            float angleOffset = rand() % 800 / 1000.0f;
            float radiusRandom = OUTER_RING_RADIUS * (0.8f + rand() % 201 / 1000.0f);
            float rotAngle = baseAngle + angleOffset;

            // Calculate initial position
            float capsuleX = SCREEN_WIDTH / 2 + radiusRandom * cos(rotAngle);
            float capsuleY = SCREEN_HEIGHT / 2 + radiusRandom * sin(rotAngle);

            capsule.initialRadius = radiusRandom / PIXELS_PER_METER;
            capsule.currentPathAngle = rotAngle;

            GUI_BASE(capsule.img)->x = capsuleX;
            GUI_BASE(capsule.img)->y = capsuleY;

            b2BodyDef BodyDef;
            BodyDef.type = b2_dynamicBody; // Set as dynamic body
            BodyDef.position.Set(capsuleX / PIXELS_PER_METER, capsuleY / PIXELS_PER_METER);

            // Calculate the rotation angle of oneself (pointing to the center of the circle+random offset)
            b2Vec2 toCenter(SCREEN_WIDTH / 2 - capsuleX, SCREEN_HEIGHT / 2 - capsuleY);
            float selfRot = atan2(toCenter.y, toCenter.x) * 180 / M_PI_F + 90;
            capsule.selfRandomRot = rand() % 60 - 30; // ± 30 degree random offset
            gui_img_set_focus(capsule.img, CAPSULE_IMG_HALF_WIDTH, CAPSULE_IMG_HALF_HEIGHT);
            gui_img_rotation(capsule.img, selfRot + capsule.selfRandomRot);
            BodyDef.angle = (selfRot + capsule.selfRandomRot) * M_PI_F / 180.0f;
            capsule.body = world->CreateBody(&BodyDef);

            b2PolygonShape rectangleShape;
            rectangleShape.SetAsBox(
                CAPSULE_IMG_HALF_WIDTH / PIXELS_PER_METER,
                CAPSULE_IMG_HALF_HEIGHT / PIXELS_PER_METER
            );

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &rectangleShape;
            fixtureDef.density = 1.0f; // Set density
            fixtureDef.restitution = 0.1f; // Set restitution
            fixtureDef.friction = 0.0f; // Set friction to zero to minimize stopping
            capsule.body->CreateFixture(&fixtureDef);

            capsule.need_recovery = false;
            capsule.recoveryTimer = 0.0f;
            capsules.push_back(capsule);

        }
    }
}


bool angles_equal(float angle1, float angle2)
{
    angle1 = fmod(angle1, 2 * M_PI_F);
    angle2 = fmod(angle2, 2 * M_PI_F);

    return std::fabs(cos(angle1) - cos(angle2)) < 0.1f &&
           std::fabs(sin(angle1) - sin(angle2)) < 0.1f;
}

void draw_clock_hand(NVGcontext *vg, float length, float width, int angleInDegrees,
                     HandState &handState)
{
    float angleInRadians = (angleInDegrees - 90) * M_PI_F /
                           180.0f; // Adjust to start from '12' position
    float xEnd = SCREEN_WIDTH / 2 + cos(angleInRadians) * length;
    float yEnd = SCREEN_HEIGHT / 2 + sin(angleInRadians) * length;

    NVGcolor draw_color = handState.previousColor;
    for (auto &capsule : capsules)
    {
        if (angles_equal(capsule.currentPathAngle, angleInRadians))
        {
            draw_color = capsule.color;
            break;
        }
    }
    handState.previousColor = draw_color;

    nvgBeginPath(vg);
    nvgMoveTo(vg, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    nvgLineTo(vg, xEnd, yEnd);
    nvgStrokeColor(vg, draw_color);
    nvgStrokeWidth(vg, width);
    nvgStroke(vg);
}

#ifndef __WIN32
extern "C" {
    extern struct tm *timeinfo;
}
#endif

void nvg_create_clock_cb(NVGcontext *vg)
{
#ifdef __WIN32
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
#endif

    uint16_t second = timeinfo->tm_sec;
    uint16_t minute = timeinfo->tm_min;
    uint16_t hour = timeinfo->tm_hour;

    // Calculate angles for each hand
    int hourAngle = (hour % 12 + minute / 60.0f) * 30.0f;  // 360/12 = 30 degrees per hour
    int minuteAngle = (minute + second / 60.0f) * 6.0f;    // 360/60 = 6 degrees per minute
    int secondAngle = second * 6.0f;                    // 360/60 = 6 degrees per second

    draw_clock_hand(vg, hourLength, hourWidth, hourAngle, hourHandState);
    draw_clock_hand(vg, minuteLength, minuteWidth, minuteAngle, minuteHandState);
    draw_clock_hand(vg, secondLength, secondWidth, secondAngle, secondHandState);

    // Central fixed point
    nvgBeginPath(vg);
    nvgCircle(vg, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 4.0f);
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
    nvgFill(vg);
}

void update_clock_cb(void *param)
{
    gui_win_t *win_clock = (gui_win_t *)param;

    memset(clock_img_data, 0, buffer_size);

    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, SCREEN_WIDTH,
                                      SCREEN_HEIGHT, nvg_create_clock_cb, clock_img_data);

    if (clock_img)
    {
        gui_obj_tree_free(clock_img);
        clock_img = NULL;
    }

    clock_img = gui_img_create_from_mem(win_clock, 0, (void *)clock_img_data, 0, 0, 0, 0);
}

void close()
{
    if (clock_img_data)
    {
        gui_lower_free(clock_img_data);
        clock_img_data = NULL;
    }

    if (world)
    {
        std::vector<Capsule>().swap(capsules);
        world->~b2World();
        gui_free(world);
        world = nullptr;
        gui_log("close world done\n");
    }
}

// Main function
int ui_design(gui_obj_t *obj)
{
    parent = obj;

    b2Vec2 gravity(0.0f, 0.0f); // Remove gravity to make it purely rotational
    world = new (gui_malloc(sizeof(b2World))) b2World(gravity);

    // Create capsules
    gui_win_t *win = gui_win_create(parent, "Rainbow Analog Clock", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    create_capsules(world);
    gui_obj_create_timer(GUI_BASE(win), 10, true, capsule_animation_cb);
    gui_obj_start_timer(GUI_BASE(win));

    // Create clock
    gui_win_t *win_clock = gui_win_create(parent, "clock_win", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    buffer_size = SCREEN_WIDTH * SCREEN_HEIGHT * 4 + sizeof(gui_rgb_data_head_t);
    clock_img_data = (uint8_t *)gui_lower_malloc(buffer_size);
    memset(clock_img_data, 0, buffer_size);
    clock_img = NULL;
    gui_obj_create_timer(GUI_BASE(win_clock), 200, true, update_clock_cb);

    return 0;
}
}

/*============================================================================*
 *                                  C Interface
 *============================================================================*/
extern "C" {
    static void app_close(gui_view_t *view)
    {
        app_rainbow_analog::close();
    }

    static void app_rainbow_analog_ui_design(gui_view_t *view)
    {
        gui_obj_t *obj = GUI_BASE(view);
        gui_win_t *win = gui_win_create(view, "win_ring", 0, 0, 0, 0);
        gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_CLICKED);
        app_rainbow_analog::ui_design(obj);
    }
}
