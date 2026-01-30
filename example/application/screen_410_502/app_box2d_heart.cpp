/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

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
#include "root_image/ui_resource.h"
#include "gui_view.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "heart_particle_view"

/*============================================================================*
 *                                  C Interface
 *============================================================================*/
extern "C" {
    static gui_view_t *current_view = NULL;
    const static gui_view_descriptor_t *menu_view = NULL;
    static void app_heart_particle_ui_design(gui_view_t *view);
    static void app_close(gui_view_t *view);

    static gui_view_descriptor_t const descriptor =
    {
        /* change Here for current view */
        .name = (const char *)CURRENT_VIEW_NAME,
        .pView = &current_view,
        .on_switch_in = app_heart_particle_ui_design,
        .on_switch_out = app_close,
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
namespace app_heart_particle
{
/*============================================================================*
*                           Types
*============================================================================*/
struct Heart
{
    b2Body *body;
    gui_img_t *img;
    float breathe_scale;  // Scaling ratio during bouncing
    float breathe_phase;
    float distance_to_center;

    float targetX;
    float targetY;
    float targetYPos;    // Original physical Y position (meters)
};

/*============================================================================*
*                              Variables
*============================================================================*/
/* GUI */
gui_obj_t *parent = nullptr;

/* Box2D World */
const float TIMESTEP = 1.0f / 60.0f; // Timestep
const int VELOCITY_ITERATIONS = 8; // Velocity iterations
const int POSITION_ITERATIONS = 3; // Position iterations
const float PIXELS_PER_METER = 30.0f;
b2World *world = nullptr; // Box2D world

/* Heart Particle */
std::vector<Heart> Heart_Particles;
const uint8_t DRAW_CENTER = 200;
const float HEART_RADIUS = 4.0f;

/* Animation Variables */
float expand_progress = 0.0f;
b2Vec2 waveOrigin;
bool isWaveActive = false;
float wave_radius = 0.0f;   // Ripple radius
const float SIGMA = 10.0f;  // Ripple smoothness
const float MAX_REBOUND_FORCE = 0.5f; // Maximum rebound force
const float DAMPING_FACTOR = 0.9f;    // Damping factor, controls the speed of rebound

/*============================================================================*
*                           Private Functions
*============================================================================*/
// Determine whether the given point is inside the heart shape
bool point_in_heart(float x, float y)
{
    float scaledX = (x - DRAW_CENTER) / 125.0f;
    float scaledY = (240 - y) / 125.0f;

    // (x^2 + y^2 - 1)^3 - x^2 y^3 <= 0
    float x2 = scaledX * scaledX;
    float y2 = scaledY * scaledY;
    float term = x2 + y2 - 1;
    return (term * term * term - x2 * scaledY * y2) <= 0;
}

void create_heart_shape(gui_obj_t *obj, std::vector<Heart> &hearts)
{
    for (float x = HEART_RADIUS; x < 2 * DRAW_CENTER; x += 4 * HEART_RADIUS)
    {
        for (float y = HEART_RADIUS; y < 2 * DRAW_CENTER; y += 4 * HEART_RADIUS)
        {
            if (point_in_heart(x, y))
            {
                Heart heart;
                heart.body = nullptr; // Placeholder for a Box2D body

                heart.targetX = x;
                heart.targetY = y;
                float dx = x - DRAW_CENTER;
                float dy = y - DRAW_CENTER;
                heart.distance_to_center = sqrt(dx * dx + dy * dy);
                heart.breathe_scale = 1.0f;
                heart.breathe_phase = 0.0f;
                heart.targetYPos = y / PIXELS_PER_METER;

                heart.img = gui_img_create_from_mem(obj, "heart", HEART_BIN, 0, 0, 0, 0);
                GUI_BASE(heart.img)->x = DRAW_CENTER;
                GUI_BASE(heart.img)->y = DRAW_CENTER;


                b2BodyDef bodyDef;
                bodyDef.type = b2_dynamicBody;
                bodyDef.position.Set(x / PIXELS_PER_METER, y / PIXELS_PER_METER);
                bodyDef.linearDamping = 5.0f;
                bodyDef.angularDamping = 5.0f;
                heart.body = world->CreateBody(&bodyDef);

                b2CircleShape circleShape;
                circleShape.m_radius = HEART_RADIUS / PIXELS_PER_METER;

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &circleShape;
                fixtureDef.density = 0.8f;
                fixtureDef.friction = 0.3f;
                fixtureDef.restitution = 0.0f;
                heart.body->CreateFixture(&fixtureDef);

                hearts.push_back(heart);
            }
        }
    }
}

void bounce_animation_cb(void *obj)
{
    (void)obj;
    for (auto &heart : Heart_Particles)
    {
        // Adjust the phase of the animation based on distance
        float phase_offset = heart.distance_to_center / 160.0f * 2 * M_PI_F;
        heart.breathe_phase += 0.05f;
        heart.breathe_scale = 1.0f + 0.25f * sin(heart.breathe_phase - phase_offset);

        // Drift
        float amplitude = 0.1f;
        b2Vec2 force(0.0f, amplitude * sin(heart.breathe_phase - phase_offset));
        heart.body->ApplyForceToCenter(force, true);

        if (isWaveActive)
        {
            // Calculate the distance to the wave source
            float dx = GUI_BASE(heart.img)->x - waveOrigin.x;
            float dy = GUI_BASE(heart.img)->y - waveOrigin.y;
            float distance = sqrt(dx * dx + dy * dy);

            float wave_effect = 2.0f * expf(-powf(distance - wave_radius, 2) / (2.0f * SIGMA));

            b2Vec2 wave_force(0.0f, -wave_effect); // Downward ripple force
            heart.body->ApplyForceToCenter(wave_force, true);

            // Rebound
            if (heart.body->GetPosition().y < heart.targetYPos)
            {
                float rebound_force = MAX_REBOUND_FORCE * (heart.targetYPos - heart.body->GetPosition().y);
                rebound_force *= DAMPING_FACTOR;
                b2Vec2 up_force(0.0f, rebound_force);
                heart.body->ApplyForceToCenter(up_force, true);
            }
        }
    }


    if (isWaveActive)
    {
        wave_radius += 5.0f; // Gradually increasing to simulate ripple diffusion

        if (wave_radius > 2 * DRAW_CENTER)
        {
            isWaveActive = false;
            wave_radius = 0.0f;
        }
    }


    if (world)
    {
        world->Step(TIMESTEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS); // Update the physics world

        for (auto &heart : Heart_Particles)
        {
            if (heart.body)
            {
                b2Vec2 pos = heart.body->GetPosition();
                GUI_BASE(heart.img)->x = pos.x * PIXELS_PER_METER - HEART_RADIUS;
                GUI_BASE(heart.img)->y = pos.y * PIXELS_PER_METER - HEART_RADIUS;

                gui_img_scale(heart.img, heart.breathe_scale, heart.breathe_scale);
            }
        }
    }
}

// easing function
float ease_in_out_quad(float t)
{
    return t < 0.5f ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2.0f;
}

void expand_animation_cb(void *param)
{
    float easedProgress = ease_in_out_quad(expand_progress);
    expand_progress += 0.01f;

    for (auto &heart : Heart_Particles)
    {
        float currentX = DRAW_CENTER + (heart.targetX - DRAW_CENTER) * easedProgress;
        float currentY = DRAW_CENTER + (heart.targetY - DRAW_CENTER) * easedProgress;

        heart.body->SetTransform(b2Vec2(currentX / PIXELS_PER_METER,
                                        currentY / PIXELS_PER_METER), heart.body->GetAngle());

        GUI_BASE(heart.img)->x = currentX - HEART_RADIUS;
        GUI_BASE(heart.img)->y = currentY - HEART_RADIUS;
    }

    if (expand_progress >= 1.0f)
    {
        gui_obj_t *win = (gui_obj_t *)param;
        gui_obj_delete_timer(GUI_BASE(win));
        gui_obj_create_timer(GUI_BASE(win), 10, true, bounce_animation_cb);
    }
}

void win_press_callback(void *obj, gui_event_t e, void *param)
{
    (void)e;
    (void)param;
    (void)obj;
    touch_info_t *tp = tp_get_info();

    waveOrigin.Set(tp->x, tp->y);
    isWaveActive = true;
}

bool init()
{
    GUI_WIDGET_TRY_EXCEPT(parent)

    gui_win_t *win = gui_win_create(parent, "Heart Particle", 5, 51, 400, 400);

    if (!win)
    {
        return false; // Handle window creation failure
    }

    create_heart_shape(GUI_BASE(win), Heart_Particles);

    expand_progress = 0.0f;
    gui_obj_create_timer(GUI_BASE(win), 1, true, expand_animation_cb);

    gui_win_press(win, win_press_callback, win);

    return true;
}

void close()
{
    if (world)
    {
        std::vector<Heart>().swap(Heart_Particles);

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

    b2Vec2 gravity(0.0f, 0.0f); // Remove gravity to make it purely rotationa
    world = new (gui_malloc(sizeof(b2World))) b2World(gravity);

    if (!init())
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
    static void app_close(gui_view_t *view)
    {
        (void)view;
        app_heart_particle::close();
    }

    static void app_heart_particle_ui_design(gui_view_t *view)
    {
        gui_obj_t *obj = GUI_BASE(view);
        // gui_win_t *win = gui_win_create(view, "win_heart", 0, 0, 0, 0);
        gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_PRESSED);
        app_heart_particle::ui_design(obj);
    }
}
