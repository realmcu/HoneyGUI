#include "gui_win.h"
#include "gui_app.h"
#include "gui_img.h"
#include "../root_image/ui_resource.h"
#include "box2d/box2d.h"
#include <iostream>
#include <cstdio>
#include <cmath>
#include <box2d/box2d.h>
#include <SDL.h>
#include <vector>
#include <cstdlib>  // For using srand and rand
#include <ctime>   // For using time
#include <nanovg.h>
#include "gui_canvas.h"

namespace app_box2d_ring
{

const float TIMESTEP = 1.0f / 60.0f; // Timestep
const int VELOCITY_ITERATIONS = 8; // Velocity iterations
const int POSITION_ITERATIONS = 3; // Position iterations

const int RING_SEGMENTS = 100; // Number of ring segments
const int BALL_COUNT = 30; // Number of balls
const float BALL_RADIUS = 10.0f; // Ball radius
const float PIXELS_PER_METER = 30.0f; // Pixels per meter
const float INITIAL_SPEED = 100.0f; // Initial speed
const float RING_GAP = 70.0f; // RING_GAP
const float BALL_DENSITY = 0.1f; // BALL_DENSITY
const float BALL_RESTITUTION = 0.3f; // BALL_RESTITUTION
gui_obj_t *parent;
gui_canvas *this_widget;
NVGcontext *vg = nullptr; // NanoVG context
b2World *world = nullptr; // Box2D world
float OUTER_RING_RADIUS; // Outer ring radius
float INNER_RING_RADIUS; // Inner ring radius
int SCREEN_WIDTH; // Screen width
int SCREEN_HEIGHT; // Screen height
struct Ball
{
    b2Body *body;
    NVGcolor color;
};

std::vector<Ball> balls; // Vector to store balls and their colors

bool init();
void close();
void createRing(b2World *world, float radius, float restitution);
void createBalls(b2World *world);
void applyCentripetalForce(b2Body *ball);
void render(gui_canvas *this_widget);

// App callback function
void app_box2d_cb(gui_win_t *win)
{
    for (const Ball &ball : balls)
    {
        applyCentripetalForce(ball.body); // Apply centripetal force
    }

    world->Step(TIMESTEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS); // Update the physics world
}

bool init()
{
    //GUI_WIDGET_TRY_EXCEPT(parent)
    SCREEN_WIDTH = gui_get_screen_width(); // Screen width
    SCREEN_HEIGHT = gui_get_screen_height(); // Screen height
    OUTER_RING_RADIUS = SCREEN_WIDTH / 2.0f; // Outer ring radius
    INNER_RING_RADIUS = SCREEN_HEIGHT / 2.0f - RING_GAP; // Inner ring radius
    gui_win_t *win = gui_win_create(parent, "APP_BOX2D ring", 0, 0, 0, 0);

    // Set the animation function of the window
    gui_win_set_animate(win, 1000, -1, (void *)app_box2d_cb, win);

    this_widget = gui_canvas_create(parent, "canvas", 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gui_canvas_set_canvas_cb(this_widget, render);

    return true;
}

void close()
{
    delete world;
}

void createRing(b2World *world, float radius, float restitution)
{
    b2BodyDef ringBodyDef;
    ringBodyDef.position.Set(SCREEN_WIDTH / 2.0f / PIXELS_PER_METER,
                             SCREEN_HEIGHT / 2.0f / PIXELS_PER_METER);
    b2Body *ringBody = world->CreateBody(&ringBodyDef);

    float angleStep = 2 * M_PI / RING_SEGMENTS; // Calculate angle step for each segment
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

        ringBody->CreateFixture(&fixtureDef); // Create fixture
    }
}

void createBalls(b2World *world)
{
    for (int i = 0; i < BALL_COUNT; ++i)
    {
        float angle = 2 * i * M_PI / BALL_COUNT;
        float ballX = (SCREEN_WIDTH / 2.0f + (OUTER_RING_RADIUS - BALL_RADIUS - 10) * cos(
                           angle)); // Position the ball between two rings
        float ballY = (SCREEN_HEIGHT / 2.0f + (OUTER_RING_RADIUS - BALL_RADIUS - 10) * sin(angle));

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

        ballBody->CreateFixture(&fixtureDef);

        // Apply initial tangential velocity to the ball
        float vx = INITIAL_SPEED * -sin(angle);  // Tangential velocity x component
        float vy = INITIAL_SPEED * cos(angle);   // Tangential velocity y component
        ballBody->SetLinearVelocity(b2Vec2(vx, vy));

        // Assign random color to the ball
        NVGcolor color = nvgRGB(rand() % 256, rand() % 256, rand() % 256);
        balls.push_back({ballBody, color}); // Add ball and color to vector
    }
}

// Apply centripetal force to ensure balls move along the ring
void applyCentripetalForce(b2Body *ball)
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

// Render function
void render(gui_canvas *this_widget)
{
    // Clear background color
    vg = this_widget->vg;

    // Draw balls
    for (const Ball &ball : balls)
    {
        float ballX = ball.body->GetPosition().x * PIXELS_PER_METER;
        float ballY = ball.body->GetPosition().y * PIXELS_PER_METER;

        // Draw ball
        NVGpaint ballPaint = nvgRadialGradient(vg, ballX, ballY, 1, BALL_RADIUS,
                                               ball.color, nvgRGBA(0, 0, 0, 0));
        nvgBeginPath(vg);
        nvgCircle(vg, ballX, ballY, BALL_RADIUS);
        nvgFillPaint(vg, ballPaint); // Set gradient color
        nvgFill(vg);
    }
    this_widget->render = 1;
}

// Main function
int ui_design(gui_obj_t *obj)
{
    parent = obj;
    if (!init())
    {
        std::cout << "Initialization failed!" << std::endl;
        return 1;
    }

    b2Vec2 gravity(0.0f, 0.0f); // Remove gravity to make it purely rotational
    world = new b2World(gravity);

    createRing(world, OUTER_RING_RADIUS, BALL_RESTITUTION); // Create outer ring with restitution of 0.3
    createRing(world, INNER_RING_RADIUS, BALL_RESTITUTION); // Create inner ring with restitution of 0.3
    createBalls(world); // Create balls

    return 0;
}
}

// C interface
extern "C" {
    void app_box2d_ring_ui_design(gui_obj_t *obj)
    {
        app_box2d_ring::ui_design(obj);
    }
}
