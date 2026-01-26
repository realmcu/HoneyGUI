/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <iostream>
#include <cstdio>
#include <cmath>
#include <box2d/box2d.h>
#include <vector>
#include <random>  // For secure random numbers
#include "gui_canvas.h"
#include "tp_algo.h"
#include "gui_win.h"
#include "gui_img.h"
#include "box2d/box2d.h"
#include "gui_rect.h"
#include "gui_view.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "box2d_ring_view"

/*============================================================================*
 *                                  C Interface
 *============================================================================*/
extern "C" {
    static gui_view_t *current_view = NULL;
    const static gui_view_descriptor_t *menu_view = NULL;
    static void app_box2d_ring_ui_design(gui_view_t *view);
    static void close_box2d_ring(gui_view_t *view);

    static gui_view_descriptor_t const descriptor =
    {
        /* change Here for current view */
        .name = (const char *)CURRENT_VIEW_NAME,
        .pView = &current_view,
        .on_switch_in = app_box2d_ring_ui_design,
        .on_switch_out = close_box2d_ring,
        .keep = 0,
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
namespace app_box2d_ring
{
/*============================================================================*
*                           Types
*============================================================================*/
struct Ball
{
    b2Body *body;
    NVGcolor color;
    gui_img_t *img;
};

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH  (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void apply_centripetal_force(b2Body *ball);
static void render();
static void maintain_minimum_velocity(b2Body *ball);
static void limit_max_angular_velocity(b2Body *ball);

/*============================================================================*
*                              Variables
*============================================================================*/
static const float TIMESTEP = 1.0f / 60.0f; // Timestep
static const int VELOCITY_ITERATIONS = 8; // Velocity iterations
static const int POSITION_ITERATIONS = 3; // Position iterations
static const int RING_SEGMENTS = 100; // Number of ring segments
static const int BALL_COUNT = 40; // Number of balls
static const float BALL_RADIUS = 10.0f; // Ball radius
static const float PIXELS_PER_METER = 30.0f; // Pixels per meter
static const float INITIAL_SPEED = 20.0f; // Initial speed
static const float RING_GAP = 70.0f; // RING_GAP
static const float BALL_DENSITY = 0.1f; // BALL_DENSITY
static const float BALL_RESTITUTION = 0.3f; // BALL_RESTITUTION
static const NVGcolor BACKGROUND_COLOR = nvgRGB(255, 255, 255);
static const float EFFECT_RADIUS = 20.0f; // Effect radius of the finger
static constexpr float MINIMUM_LINEAR_VELOCITY = 5.0f; // Minimum linear velocity to avoid stopping
static constexpr float MAX_ANGULAR_VELOCITY = 15.0f; // Maximum angular velocity for balls

b2World *world = nullptr; // Box2D world
float outer_ring_radius = 0; // Outer ring radius
float inner_ring_radius = 0; // Inner ring radius
static uint16_t count = 0;

std::vector<Ball> balls; // Vector to store balls and their colors
std::vector<b2Body *> temporaryBodies; // Vector to store temporary bodies
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 255);

/*============================================================================*
*                           Private Functions
*============================================================================*/
static void app_box2d_cb(void *obj)
{
    (void)obj;
    for (const Ball &ball : balls)
    {
        apply_centripetal_force(ball.body); // Apply centripetal force
        limit_max_angular_velocity(ball.body); // Limit the maximum angular velocity
        maintain_minimum_velocity(ball.body); // Ensure minimum motion
        float ballX = ball.body->GetPosition().x * PIXELS_PER_METER;
        float ballY = ball.body->GetPosition().y * PIXELS_PER_METER;
        GUI_BASE(ball.img)->x = ballX - BALL_RADIUS;
        GUI_BASE(ball.img)->y = ballY - BALL_RADIUS;
    }

    if (world)
    {
        world->Step(TIMESTEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS); // Update the physics world
    }
}

static void win_press_callback(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    touch_info_t *touch = tp_get_info();
    int mouseX = touch->x;
    int mouseY = touch->y;

    // Convert mouse coordinates to Box2D world coordinates
    b2Vec2 clickPoint(mouseX / PIXELS_PER_METER, mouseY / PIXELS_PER_METER);

    // Create a temporary large invisible ball to create the effect
    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody; // Set as dynamic body
    ballBodyDef.position.Set(clickPoint.x, clickPoint.y);
    b2Body *ballBody = world->CreateBody(&ballBodyDef);

    b2CircleShape circleShape;
    circleShape.m_radius = EFFECT_RADIUS / PIXELS_PER_METER;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = BALL_DENSITY * 10; // Set density
    fixtureDef.restitution = BALL_RESTITUTION * 10; // Set restitution
    fixtureDef.friction = 0.0f; // Set friction to zero to minimize stopping
    ballBody->CreateFixture(&fixtureDef);

    // Apply initial tangential velocity to the ball
    float vx = INITIAL_SPEED ;  // Tangential velocity x component
    float vy = INITIAL_SPEED ;   // Tangential velocity y component
    ballBody->SetLinearVelocity(b2Vec2(vx, vy));
    temporaryBodies.push_back(ballBody); // Store the temporary body
}

static void win_release_callback()
{
    // Destroy temporary bodies after a short period
    for (b2Body *body : temporaryBodies)
    {
        world->DestroyBody(body);
    }
    std::vector<b2Body *>().swap(temporaryBodies);
}

// Maintain minimum linear velocity to avoid stopping
static void maintain_minimum_velocity(b2Body *ball)
{
    if (ball->GetLinearVelocity().Length() < MINIMUM_LINEAR_VELOCITY)
    {
        // Apply a small random impulse to keep the ball moving
        float angle = dis(gen) % 360;
        float impulseMagnitude = ball->GetMass() * MINIMUM_LINEAR_VELOCITY;
        b2Vec2 impulse(impulseMagnitude * cos(angle), impulseMagnitude * sin(angle));
        ball->ApplyLinearImpulseToCenter(impulse, true);
    }
}

// Limit the maximum angular velocity to prevent unrealistic speeds
static void limit_max_angular_velocity(b2Body *ball)
{
    float currentSpeed = ball->GetLinearVelocity().Length();
    if (currentSpeed > MAX_ANGULAR_VELOCITY)
    {
        b2Vec2 scaledVelocity = ball->GetLinearVelocity();
        scaledVelocity *= (MAX_ANGULAR_VELOCITY / currentSpeed);
        ball->SetLinearVelocity(scaledVelocity);
    }
}

bool init(gui_obj_t *parent)
{
    GUI_WIDGET_TRY_EXCEPT(parent)

    gui_win_t *win = gui_win_create(parent, "APP_BOX2D ring", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Set the animation function of the window
    gui_obj_create_timer(GUI_BASE(win), 20, true, app_box2d_cb);
    gui_obj_start_timer(GUI_BASE(win));

    gui_win_press(win, win_press_callback, win);
    gui_win_release(win, (gui_event_cb_t)win_release_callback, win);
    render();
    return true;
}

static void clear_mem()
{
    if (world)
    {
        for (Ball &ball : balls)
        {
            gui_free((void *)gui_img_get_image_data(ball.img));
        }
        std::vector<Ball>().swap(balls);

        win_release_callback();
        world->~b2World();
        gui_free(world);
        world = nullptr;
        gui_log("close world done\n");
    }
}

static void create_ring(b2World *world, float radius, float restitution)
{
    b2BodyDef ringBodyDef;
    ringBodyDef.position.Set(SCREEN_WIDTH / 2.0f / PIXELS_PER_METER,
                             SCREEN_HEIGHT / 2.0f / PIXELS_PER_METER);
    b2Body *ringBody = world->CreateBody(&ringBodyDef);

    float angleStep = 2 * M_PI_F / RING_SEGMENTS; // Calculate angle step for each segment
    for (int i = 0; i < RING_SEGMENTS; ++i)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;
        b2Vec2 vertex1(radius * cos(angle1) / PIXELS_PER_METER, radius * sin(angle1) / PIXELS_PER_METER);
        b2Vec2 vertex2(radius * cos(angle2) / PIXELS_PER_METER, radius * sin(angle2) / PIXELS_PER_METER);

        b2EdgeShape edgeShape;
        edgeShape.m_vertex1 = vertex1; // Vertex 1
        edgeShape.m_vertex2 = vertex2; // Vertex 2

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &edgeShape;
        fixtureDef.restitution = restitution; // Set restitution
        fixtureDef.friction = 0.0f; // Set friction to zero to minimize stopping
        ringBody->CreateFixture(&fixtureDef); // Create fixture
    }
}

static void create_balls(b2World *world)
{
    for (int i = 0; i < BALL_COUNT; ++i)
    {
        float angle = 2 * i * M_PI_F / BALL_COUNT;
        float ballX = (SCREEN_WIDTH / 2.0f + (outer_ring_radius - BALL_RADIUS - 10) * cos(
                           angle)); // Position the ball between two rings
        float ballY = (SCREEN_HEIGHT / 2.0f + (outer_ring_radius - BALL_RADIUS - 10) * sin(angle));
        // gui_log("ball_%d %f,%f\n", i, ballX, ballY);
        b2BodyDef ballBodyDef;
        ballBodyDef.type = b2_dynamicBody; // Set as dynamic body
        ballBodyDef.position.Set(ballX / PIXELS_PER_METER, ballY / PIXELS_PER_METER);
        b2Body *ballBody = world->CreateBody(&ballBodyDef);

        b2CircleShape circleShape;
        circleShape.m_radius = BALL_RADIUS / PIXELS_PER_METER;

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circleShape;
        fixtureDef.density = BALL_DENSITY; // Set density
        fixtureDef.restitution = BALL_RESTITUTION; // Set restitution
        fixtureDef.friction = 0.0f; // Set friction to zero to minimize stopping
        ballBody->CreateFixture(&fixtureDef);

        // Apply initial tangential velocity to the ball
        float vx = INITIAL_SPEED * -sin(angle);  // Tangential velocity x component
        float vy = INITIAL_SPEED * cos(angle);   // Tangential velocity y component
        ballBody->SetLinearVelocity(b2Vec2(vx, vy));

        // Assign random color to the ball
        NVGcolor color = nvgRGB(xorshift16() % 256, xorshift16() % 256, xorshift16() % 256);
        balls.push_back({ballBody, color, nullptr}); // Add ball and color to vector
    }
}

// Apply centripetal force to ensure balls move along the ring
static void apply_centripetal_force(b2Body *ball)
{
    b2Vec2 center(SCREEN_WIDTH / 2.0f / PIXELS_PER_METER,
                  SCREEN_HEIGHT / 2.0f / PIXELS_PER_METER); // Center of the ring
    b2Vec2 toCenter = center - ball->GetPosition(); // Vector from ball to center
    float distance = toCenter.Length(); // Distance
    toCenter.Normalize(); // Normalize
    float speed = ball->GetLinearVelocity().Length(); // Ball's speed
    float forceMagnitude = ball->GetMass() * speed * speed / distance; // Centripetal force magnitude
    b2Vec2 force = forceMagnitude * toCenter; // Centripetal force vector
    ball->ApplyForceToCenter(force, true); // Apply force to ball center
}

static void canvas_callback(NVGcontext *vg)
{
    // Draw ball
    const Ball &ball = balls.at(count++);
    {
        // Draw ball
        NVGpaint ballPaint = nvgRadialGradient(vg, BALL_RADIUS, BALL_RADIUS, 1, BALL_RADIUS,
                                               ball.color, nvgRGBA(ball.color.r * UINT8_MAX, ball.color.g * UINT8_MAX, ball.color.b * UINT8_MAX,
                                                                   0));
        // gui_log("r = %f, g = %f, b = %f, a = %f\n", ball.color.r, ball.color.g, ball.color.b, ball.color.a);
        nvgBeginPath(vg);
        nvgCircle(vg, BALL_RADIUS, BALL_RADIUS, BALL_RADIUS);
        nvgFillPaint(vg, ballPaint); // Set gradient color
        nvgFill(vg);
    }
}

// Render function
static void render()
{
    count = 0;
    gui_rect_create((gui_obj_t *)current_view, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0,
                    gui_rgba(255, 255, 255, 255));

    // Draw balls
    for (Ball &ball : balls)
    {
        float ballX = ball.body->GetPosition().x * PIXELS_PER_METER;
        float ballY = ball.body->GetPosition().y * PIXELS_PER_METER;


        size_t buffer_size = BALL_RADIUS * BALL_RADIUS * 4 * 4 + sizeof(
                                 gui_rgb_data_head_t);
        uint8_t *img_data = (uint8_t *)gui_lower_malloc(buffer_size);
        memset(img_data, 0, buffer_size);

        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, BALL_RADIUS * 2,
                                          BALL_RADIUS * 2, canvas_callback, img_data);
        gui_img_t *img = gui_img_create_from_mem((gui_obj_t *)current_view, 0, (void *)img_data,
                                                 ballX - BALL_RADIUS,
                                                 ballY - BALL_RADIUS, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        ball.img = img;
    }
}

// Main function
static int ui_design(gui_obj_t *obj)
{
    b2Vec2 gravity(0.0f, 0.0f); // Remove gravity to make it purely rotational

    world = new (gui_malloc(sizeof(b2World))) b2World(gravity);
    outer_ring_radius = SCREEN_WIDTH / 2.0f; // Outer ring radius
    inner_ring_radius = outer_ring_radius - RING_GAP; // Inner ring radius
    create_ring(world, outer_ring_radius,
                BALL_RESTITUTION); // Create outer ring with restitution of 0.3
    create_ring(world, inner_ring_radius,
                BALL_RESTITUTION); // Create inner ring with restitution of 0.3
    create_balls(world); // Create balls
    if (!init(obj))
    {
        std::cout << "Initialization failed!" << std::endl;
        return 1;
    }
    return 0;
}
}

/*============================================================================*
 *                                  C Interface
 *============================================================================*/
extern "C" {
    static void app_box2d_ring_ui_design(gui_view_t *view)
    {
        gui_obj_t *obj = GUI_BASE(view);
        // gui_win_t *win = gui_win_create(view, "win_ring", 0, 0, 0, 0);
        gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_CLICKED);
        app_box2d_ring::ui_design(obj);
    }
    static void close_box2d_ring(gui_view_t *view)
    {
        (void)view;
        app_box2d_ring::clear_mem();
    }
}
