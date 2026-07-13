/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_canvas.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"

#include "box2d/box2d.h"

#include <iostream>

/* Box representation ------------------------------------------------------ */
#define BOX_W   50
#define BOX_H   50
#define BOX_R   7   /* corner radius */

static gui_img_t *box_img;

/* Draw a rounded rect into the canvas buffer. Coordinates are relative to
 * the image, so keep the shape at (0, 0). */
static void draw_box_cb(NVGcontext *vg)
{
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0, 0, BOX_W, BOX_H, BOX_R);
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
    nvgFill(vg);
}

// UI design function
static void app_ui_design(void)
{
    /* RGB565 -> 2 bytes/pixel, plus the header the img widget expects. */
    uint32_t buf_size = BOX_W * BOX_H * 2 + sizeof(gui_rgb_data_head_t);
    uint8_t *image_buffer = (uint8_t *)gui_lower_malloc(buf_size);
    memset(image_buffer, 0, buf_size);

    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0,
                                      BOX_W, BOX_H,
                                      draw_box_cb, image_buffer);

    box_img = gui_img_create_from_mem(gui_obj_get_root(), "box_img",
                                      image_buffer, 100, 0, 0, 0);
}

static b2World *world;
static void box2d_loop(void *p)
{
    GUI_UNUSED(p);
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
    dynamicBox.SetAsBox(BOX_W / 2.0f, BOX_H / 2.0f);

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

        /* Box2D +Y is up, screen Y is down -- flip it. */
        gui_img_set_pos(box_img, 100, (int16_t)(454 - position.y));
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
