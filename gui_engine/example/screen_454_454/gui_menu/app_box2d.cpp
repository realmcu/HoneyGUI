#include "gui_win.h"
#include "gui_app.h"
#include "gui_img.h"
#include "../root_image/ui_resource.h"
#include "box2d/box2d.h"
#include <iostream>
#include <stdio.h>
#include <cmath>
namespace app_box2d
{
const int SCREEN_WIDTH = 454;
const int SCREEN_HEIGHT = 454;
const float M2P =
    20; // A physical unit corresponds to 20 pixels, used to convert physical coordinates to pixel coordinates
const float P2M = 1 / M2P; // A pixel corresponds to a physical unit
b2World world(b2Vec2(0.0f, -9.8f)); // Create a Box2D world with gravity
b2Body *ballBody;
gui_img_t *img;

// App callback function
void app_box2d_cb(gui_win_t *win)
{
    // Update the physics world
    world.Step(1 / 60.f, 8, 3);

    // Get the position of the ball
    b2Vec2 position = ballBody->GetPosition();

    // Set the image location and rotate it on the GUI
    gui_img_set_location(img, (int)(position.x * M2P - 20), (int)(position.y * M2P - 20));
    gui_img_rotation(img, ballBody->GetAngle(), gui_img_get_width(img) / 2,
                     gui_img_get_height(img) / 2);

    // Log the vertical linear velocity
    std::cout << (ballBody->GetLinearVelocity().y) << std::endl;

    // When the linear velocity is low, give the ball a nudge
    if (abs(ballBody->GetLinearVelocity().x) < 0.05f || abs(ballBody->GetLinearVelocity().y) <= 0.03f)
    {
        b2Vec2 lv(50, 50);
        ballBody->SetLinearVelocity(lv);
        ballBody->SetAngularVelocity(3.14 * 1.5);
    }
}

// GUI design function
void ui_design(gui_obj_t *obj)
{
    gui_win_t *win = gui_win_create(obj,
                                    "APP_BOX2D", 0, 0, 0, 0);

    // Set the animation function of the window
    gui_win_set_animate(win, 1000, -1, (void *)app_box2d_cb, win);

    // Create an image for displaying on the window
    img = gui_img_create_from_mem(win, "img", ICON_BUDS_ON_BIN, 0, 0, 0, 0);

    // Define the ground
    b2EdgeShape groundEdge;
    groundEdge.SetTwoSided(b2Vec2(-100.0f, 0.0f), b2Vec2(100.0f, 0.0f));

    // Define the boundaries
    b2BodyDef wallsBodyDef;
    wallsBodyDef.position.Set(0, 0);
    b2Body *wallsBody = world.CreateBody(&wallsBodyDef);

    // Define the walls
    b2EdgeShape topWall, bottomWall, leftWall, rightWall;
    topWall.SetTwoSided(b2Vec2(0, 0), b2Vec2(SCREEN_WIDTH * P2M, 0));
    bottomWall.SetTwoSided(b2Vec2(0, SCREEN_HEIGHT * P2M), b2Vec2(SCREEN_WIDTH * P2M,
                                                                  SCREEN_HEIGHT * P2M));
    leftWall.SetTwoSided(b2Vec2(0, 0), b2Vec2(0, SCREEN_HEIGHT * P2M));
    rightWall.SetTwoSided(b2Vec2(SCREEN_WIDTH * P2M, 0), b2Vec2(SCREEN_WIDTH * P2M,
                                                                SCREEN_HEIGHT * P2M));

    // Associate the walls with the boundary body
    wallsBody->CreateFixture(&topWall, 0.0f);
    wallsBody->CreateFixture(&bottomWall, 0.0f);
    wallsBody->CreateFixture(&leftWall, 0.0f);
    wallsBody->CreateFixture(&rightWall, 0.0f);

    // Add a dynamic body, for example a ball
    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.position.Set(SCREEN_WIDTH / 1.5 * P2M, SCREEN_HEIGHT / 1.5 * P2M);
    ballBodyDef.angularVelocity = -314;    //-PI rad/s
    ballBodyDef.linearVelocity.Set(100, 50);
    ballBody = world.CreateBody(&ballBodyDef);

    b2CircleShape circleShape;
    circleShape.m_radius = 20 *
                           P2M; // The radius of the circle should also be converted to physical units

    b2FixtureDef ballFixtureDef;
    ballFixtureDef.shape = &circleShape;
    ballFixtureDef.density = 5;
    ballFixtureDef.restitution =
        0.9; // Set elasticity, so that the ball bounces back when it hits the boundary
    ballFixtureDef.friction = 0.8f;

    //Attach the shape to the ballBody
    ballBody->CreateFixture(&ballFixtureDef);
}
}



extern "C" {
    void app_box2d_ui_design(gui_obj_t *obj)
    {
        app_box2d::ui_design(obj);
    }
}








