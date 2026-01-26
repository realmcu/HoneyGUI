/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_img.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_img.h"
#include "gui_img_scope.h"
#include "gui_obj.h"
#include "gui_canvas.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "../../assets/tiger_blue.txt"
#include "../../assets/tiger_grey.txt"
#include "../../assets/tiger_laven.txt"
#include "../../assets/tiger_lime.txt"
#include "../../assets/tiger_turk.txt"
#include "../../assets/tiger_white.txt"
#include "../../assets/tiger_yellow.txt"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_cube.h"
#include "gui_win.h"
#include "gui_canvas_arc.h"
#include "gui_canvas_round_rect.h"
#include "../../assets/tiger_blue_compressed.txt"
#include "acc_sw_rle.h"

#include "box2d/box2d.h"

#include <iostream>

static gui_canvas_round_rect_t *round_rect;

// UI design function
static void app_ui_design(void)
{
    round_rect = gui_canvas_round_rect_create(gui_obj_get_root(), "canva_rect", 100, 0, 50, 50, 7,
                                              APP_COLOR_WHITE);
    return;
}

static b2World *world;
static void box2d_loop(void *p)
{
    // Create a Box2D world with gravity
    {
        b2Vec2 gravity(0.0f, -10.0f);
        world = new b2World(gravity);
    }
    // Create ground
    {
        b2BodyDef bodyDef;
        bodyDef.position.Set(0.0f, -10.0f);
        b2Body *groundBody = world->CreateBody(&bodyDef);

        b2PolygonShape groundBox;
        groundBox.SetAsBox(50.0f, 10.0f);
        groundBody->CreateFixture(&groundBox, 0.0f);
    }
    // Create a dynamic body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.0f, 454.0f);
    bodyDef.angle = b2_pi / 4.0f;  // Set initial angle to 45 degrees (radians)
    b2Body *body = world->CreateBody(&bodyDef);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(25.0f, 25.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density =
        1.0f;          // Density of the object: affects the mass and inertia of the object. Higher density makes the object heavier.
    fixtureDef.friction =
        0.3f;         // Friction coefficient: affects the resistance when the object contacts other objects. Higher friction makes it harder for the object to slide.
    fixtureDef.restitution =
        0.8f;      // Restitution coefficient (bounciness): determines the bounce effect after the object collides. 0 means no bounce, 1 means perfectly elastic collision.
    body->CreateFixture(&fixtureDef);

    for (int i = 0; i < 6000; ++i)
    {
        float timeStep = 1.0f / 60.0f;
        int32 velocityIterations = 6;
        int32 positionIterations = 2;
        world->Step(timeStep, velocityIterations, positionIterations);
        b2Vec2 position = body->GetPosition();
        float angle = body->GetAngle();
        std::printf("Step %d: position=(%.2f, %.2f), angle=%.2f\n", i, position.x, position.y, angle);
        gui_thread_mdelay(1000 / 60);

        gui_canvas_round_rect_set_location(round_rect, 100, 454 - position.y);
    }

    while (1)
    {
        gui_log("line = %d \n", __LINE__);
        gui_thread_mdelay(1000);
    }
}

static int app_init(void)
{
    app_ui_design();
    gui_thread_create("box2d", box2d_loop, NULL, 1024 * 10, 15);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
