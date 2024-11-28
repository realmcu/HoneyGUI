#include "gui_win.h"
#include "gui_app.h"
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

static void *allocate(size_t size)
{
    return gui_lower_malloc(size);
}

static void deallocate(void *ptr) noexcept
{
    gui_lower_free(ptr);
}
namespace app_box2d_ring
{

const float TIMESTEP = 1.0f / 60.0f; // Timestep
const int VELOCITY_ITERATIONS = 8; // Velocity iterations
const int POSITION_ITERATIONS = 3; // Position iterations
const int RING_SEGMENTS = 100; // Number of ring segments
const int BALL_COUNT = 40; // Number of balls
const float BALL_RADIUS = 10.0f; // Ball radius
const float PIXELS_PER_METER = 30.0f; // Pixels per meter
const float INITIAL_SPEED = 20.0f; // Initial speed
const float RING_GAP = 70.0f; // RING_GAP
const float BALL_DENSITY = 0.1f; // BALL_DENSITY
const float BALL_RESTITUTION = 0.3f; // BALL_RESTITUTION
const NVGcolor BACKGROUND_COLOR = nvgRGB(255, 255, 255);
const float EFFECT_RADIUS = 20.0f; // Effect radius of the finger
constexpr float MINIMUM_LINEAR_VELOCITY = 5.0f; // Minimum linear velocity to avoid stopping
constexpr float MAX_ANGULAR_VELOCITY = 15.0f; // Maximum angular velocity for balls
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
    gui_img_t *img;
};

std::vector<Ball> balls; // Vector to store balls and their colors
std::vector<b2Body *> temporaryBodies; // Vector to store temporary bodies
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 255);
bool init();
void close();
void createRing(b2World *world, float radius, float restitution);
void createBalls(b2World *world);
void applyCentripetalForce(b2Body *ball);
void render();
void maintainMinimumVelocity(b2Body *ball);
void limitMaxAngularVelocity(b2Body *ball);

static uint16_t seed = 12345;

static uint16_t xorshift16()
{
    seed ^= seed << 6;
    seed ^= seed >> 9;
    seed ^= seed << 2;
    return seed;
}

// App callback function
void app_box2d_cb(void *p, void *this_widget, gui_animate_t *animate)
{
    for (const Ball &ball : balls)
    {
        applyCentripetalForce(ball.body); // Apply centripetal force
        limitMaxAngularVelocity(ball.body); // Limit the maximum angular velocity
        maintainMinimumVelocity(ball.body); // Ensure minimum motion
        float ballX = ball.body->GetPosition().x * PIXELS_PER_METER;
        float ballY = ball.body->GetPosition().y * PIXELS_PER_METER;
        GUI_BASE(ball.img)->x = ballX - BALL_RADIUS;
        GUI_BASE(ball.img)->y = ballY - BALL_RADIUS;
    }

    world->Step(TIMESTEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS); // Update the physics world
}
void win_press_callback(void *obj, gui_event_t e, void *param)
{
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
void win_release_callback()
{
    // Destroy temporary bodies after a short period
    for (b2Body *body : temporaryBodies)
    {
        world->DestroyBody(body);
    }
    temporaryBodies.clear();
}
// Maintain minimum linear velocity to avoid stopping
// Maintain minimum linear velocity to avoid stopping
void maintainMinimumVelocity(b2Body *ball)
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
void limitMaxAngularVelocity(b2Body *ball)
{
    float currentSpeed = ball->GetLinearVelocity().Length();
    if (currentSpeed > MAX_ANGULAR_VELOCITY)
    {
        b2Vec2 scaledVelocity = ball->GetLinearVelocity();
        scaledVelocity *= (MAX_ANGULAR_VELOCITY / currentSpeed);
        ball->SetLinearVelocity(scaledVelocity);
    }
}
bool init()
{
    GUI_WIDGET_TRY_EXCEPT(parent)

    gui_win_t *win = gui_win_create(parent, "APP_BOX2D ring", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!win)
    {
        return false; // Handle window creation failure
    }

    // Set the animation function of the window
    gui_win_set_animate(win, 1000, -1, app_box2d_cb, win);
    gui_win_press(win, win_press_callback, win);
    gui_win_release(win, (gui_event_cb_t)win_release_callback, win);
    render();
    return true;
}

void close()
{
    for (Ball body : balls)
    {
        world->DestroyBody(body.body);
    }
    balls.clear();
    win_release_callback();
    deallocate(world);
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
        fixtureDef.friction = 0.0f; // Set friction to zero to minimize stopping
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
// Define custom comparison operator for NVGcolor
bool operator!=(const NVGcolor &c1, const NVGcolor &c2)
{
    return !(c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a);
}
static int count;
// Render function
void canvas_callback(NVGcontext *vg)

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
void render()
{
    count = 0;
    if (BACKGROUND_COLOR != nvgRGB(0, 0, 0))
    {
        gui_canvas_rect_create(parent, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgba(255, 255, 255, 255));
        // gui_log("r = %f, g = %f, b = %f, a = %f\n", BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
        // gui_canvas_rect_create(parent, 0, 0,0, SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgba(BACKGROUND_COLOR.r*255, BACKGROUND_COLOR.g*255, BACKGROUND_COLOR.b*255, BACKGROUND_COLOR.a*255));
    }


    // Draw balls
    for (Ball &ball : balls)
    {

        float ballX = ball.body->GetPosition().x * PIXELS_PER_METER;
        float ballY = ball.body->GetPosition().y * PIXELS_PER_METER;


        const uint8_t *img_data =  gui_canvas_output(GUI_CANVAS_OUTPUT_RGBA, 0, BALL_RADIUS * 2,
                                                     BALL_RADIUS * 2, canvas_callback);
        gui_img_t *img = gui_img_create_from_mem(parent, 0, (void *)img_data, ballX - BALL_RADIUS,
                                                 ballY - BALL_RADIUS, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        ball.img = img;
    }
}

// Main function
int ui_design(gui_obj_t *obj)
{
    parent = obj;
    // render();
    // return 1;

    b2Vec2 gravity(0.0f, 0.0f); // Remove gravity to make it purely rotational

    // if (world != nullptr)
    // {
    //     for (Ball body : balls)
    //     {
    //         world->DestroyBody(body.body);
    //     }
    //     balls.clear();
    //     win_release_callback();
    //     deallocate(world);
    // }
    void *mem = allocate(sizeof(b2World));
    world = new (mem) b2World(gravity);
    SCREEN_WIDTH = gui_get_screen_width(); // Screen width
    SCREEN_HEIGHT = gui_get_screen_height(); // Screen height
    OUTER_RING_RADIUS = SCREEN_WIDTH / 2.0f; // Outer ring radius
    INNER_RING_RADIUS = OUTER_RING_RADIUS - RING_GAP; // Inner ring radius
    createRing(world, OUTER_RING_RADIUS, BALL_RESTITUTION); // Create outer ring with restitution of 0.3
    createRing(world, INNER_RING_RADIUS, BALL_RESTITUTION); // Create inner ring with restitution of 0.3
    createBalls(world); // Create balls
    if (!init())
    {
        std::cout << "Initialization failed!" << std::endl;
        return 1;
    }
    return 0;
}
}

// C interface
extern "C" {
    void app_box2d_ring_ui_design(gui_obj_t *obj)
    {
        app_box2d_ring::ui_design(obj);
    }
    void close_box2d_ring(void)
    {
        app_box2d_ring::close();
    }
}
