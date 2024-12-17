#include "gui_win.h"
#include "gui_app.h"
#include "gui_img.h"
#include "root_image_hongkong/ui_resource.h"
#include "box2d/box2d.h"
#include "gui_text.h"
#include "../input/tp_algo.h"
#include "guidef.h"
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <stdint.h>
#include "gui_api.h"
#include <vector>

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define HEIGHT_OFFSET 100
namespace app_fruit_ninja
{
static const float M2P =
    20; // A physical unit corresponds to 20 pixels, used to convert physical coordinates to pixel coordinates
static const float P2M = 1 / M2P; // A pixel corresponds to a physical unit
static const int RADIUS_ST = 72 / 2;   // st: 68*72  pixels
static const int RADIUS_BA = 126 / 2;  // ba: 126*50 pixels
static const int RADIUS_PE = 62 / 2;   // pe: 62*59  pixels
static const int RADIUS_WM = 98 / 2;   // wm: 98*85  pixels
static const int RADIUS_BB = 50 / 2;   // wm: 50*51  pixels

static int16_t fruit_score = 0;
static uint8_t fruit_cut_cnt = 0;
static bool init_flag = true;
static bool fruit_cut_flag[4] = {0}; // record whether fruits are cut

static b2World *world = nullptr; // Box2D world
static b2Body *body_st, *body_ba, *body_pe, *body_wm,
       *body_bomb; // entities that simulate motion trajectories
std::vector<b2Body *> temporaryBodies;

static gui_img_t *img_strawberry, *img_banana,
       *img_peach, *img_watermelon,
       *img_gameover, *img_bg, *img_bomb;
static gui_text_t *score_board, *time_counter;
static void *time_counter_content = NULL;
static uint8_t game_time;
static gui_img_t
*img_cut_arry[4]; // img_strawberry_cut, *img_banana_cut, *img_peach_cut, *img_watermelon_cut;
/* Define a point structure */
typedef struct
{
    float x, y;
} Point;

static bool get_init_flag(void)
{
    return init_flag;
}

void clear_world()
{
    if (world)
    {
        for (b2Body *body : temporaryBodies)
        {
            world->DestroyBody(body);
        }
        temporaryBodies.clear();
        gui_free(world);
        world = nullptr;
        gui_log("close world done\n");
    }

}
/* rotate to get rectangular's four points */
static Point rotate_point(Point p, Point center, float angle)
{
    /* converting angles to radians */
    float radians = angle * 2 * 3.14 / 360.0;
    float cos_theta = cos(radians);
    float sin_theta = sin(radians);

    /* translate the point so that the center point is the origin */
    float translated_x = p.x - center.x;
    float translated_y = p.y - center.y;

    /* rotating */
    float rotated_x = translated_x * cos_theta - translated_y * sin_theta;
    float rotated_y = translated_x * sin_theta + translated_y * cos_theta;


    Point rotated_point = {rotated_x + center.x, rotated_y + center.y};
    return rotated_point;
}

/* Compute the cross product */
static float cross_product(Point p1, Point p2, Point p3)
{
    return (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
}

/* Determine whether the point q is on the line pr */
static int on_segment(Point p, Point q, Point r)
{
    if (q.x <= fmax(p.x, r.x) && q.x >= fmin(p.x, r.x) &&
        q.y <= fmax(p.y, r.y) && q.y >= fmin(p.y, r.y))
    {
        return 1;
    }
    return 0;
}

/* Determine whether two line segments intersect */
static int segments_intersect(Point p1, Point p2, Point p3, Point p4)
{
    float d1 = cross_product(p1, p3, p4);
    float d2 = cross_product(p2, p3, p4);
    float d3 = cross_product(p3, p1, p2);
    float d4 = cross_product(p4, p1, p2);

    if (((d1 * d2 < 0) && (d3 * d4 < 0)) ||
        (d1 == 0 && on_segment(p3, p1, p4)) ||
        (d2 == 0 && on_segment(p3, p2, p4)) ||
        (d3 == 0 && on_segment(p1, p3, p2)) ||
        (d4 == 0 && on_segment(p1, p4, p2)))
    {
        return 1;
    }
    return 0;
}

/* Calculate the point of intersection of line segments */
static int intersection_point(Point p1, Point p2, Point p3, Point p4, Point *intersect)
{
    float A1 = p2.y - p1.y;
    float B1 = p1.x - p2.x;
    float C1 = A1 * p1.x + B1 * p1.y;

    float A2 = p4.y - p3.y;
    float B2 = p3.x - p4.x;
    float C2 = A2 * p3.x + B2 * p3.y;

    float det = A1 * B2 - A2 * B1;
    if (det == 0)
    {
        return 0; // Parallel or co-linear
    }
    else
    {
        intersect->x = (B2 * C1 - B1 * C2) / det;
        intersect->y = (A1 * C2 - A2 * C1) / det;

        if (on_segment(p1, *intersect, p2) && on_segment(p3, *intersect, p4))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

/* Determine if a line segment has two points of intersection with a rectangle */
static bool line_has_two_intersections_with_rectangle(Point rect_min, float width, float height,
                                                      Point line_start, Point line_end, float angle)
{
    /* The four vertices of the rectangle */
    Point rect_max = {rect_min.x + width, rect_min.y + height};
    Point rect_p2 = {rect_min.x, rect_min.y + height}; // left-down
    Point rect_p3 = {rect_min.x + width, rect_min.y}; // right-top


    Point center = {(float)(rect_min.x + width / 2.0), (float)(rect_min.y + height / 2.0)}; // calculate center point

    /* calculate rectangular's four rotated points */

    Point rotated_rect_min = rotate_point(rect_min, center, angle); // left-up
    Point rotated_rect_max = rotate_point(rect_max, center, angle); // right-down
    Point rotated_rect_p2 = rotate_point(rect_p2, center, angle); // left-down
    Point rotated_rect_p3 = rotate_point(rect_p3, center, angle); // right-top

    /* four sides of a rectangle */
    Point rect_edges[4][2] =
    {
        {rotated_rect_min, rotated_rect_p3},  // top
        {rotated_rect_p3, rotated_rect_max},  // right
        {rotated_rect_max, rotated_rect_p2},  // bottom
        {rotated_rect_p2, rotated_rect_min}   // left
    };

    int intersection_count = 0;
    Point intersect;

    /* Check if the line segment intersects any of the sides and count the number of intersections */
    for (int i = 0; i < 4; i++)
    {
        if (segments_intersect(line_start, line_end, rect_edges[i][0], rect_edges[i][1]))
        {
            if (intersection_point(line_start, line_end, rect_edges[i][0], rect_edges[i][1], &intersect))
            {
                intersection_count++;
                if (intersection_count == 2)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

static uint16_t seed = 12345;

static uint16_t xorshift16()
{
    seed ^= seed << 6;
    seed ^= seed >> 9;
    seed ^= seed << 2;
    return seed;
}

// Image refresh
static bool position_refresh(int x, int y, gui_img_t *img, b2Body *body)
{
    if (y < -70 || y > 550 || x < -70 || x > SCREEN_WIDTH + 70)
    {
        b2Vec2 position((100 + xorshift16() % (354 - 100 + 1)) * P2M, 550 * P2M);
        // gui_log("x=%f,y=%f\r\n", position.x, position.y);
        b2Vec2 lv; //speed
        body->SetTransform(position, xorshift16() % (360 + 1));
        if (position.x * M2P > 227) // if position left, velocity right
        {
            lv.Set(-(10 + xorshift16() % (15 - 10 + 1)),
                   -(22 + xorshift16() % (22 - 20 + 1))); // velocity x:10~15; y: 18~20
        }
        else
        {
            lv.Set((10 + xorshift16() % (15 - 10 + 1)), -(22 + xorshift16() % (22 - 20 + 1)));
        }
        body->SetLinearVelocity(lv);
        body->SetAngularVelocity(-314);

        gui_img_set_location(img, (int)(position.x * M2P), 550);
        gui_img_rotation(img, body->GetAngle(), gui_img_get_width(img) / 2,
                         gui_img_get_height(img) / 2);
        return true;
    }
    else
    {
        gui_img_set_location(img, x, y);
        gui_img_rotation(img, body->GetAngle(), gui_img_get_width(img) / 2,
                         gui_img_get_height(img) / 2);
    }

    return false;
}

// scoring, cutting pictures. if cut bomb, gameover
static bool cutting_judgment(gui_win_t *win, gui_img_t *ST, gui_img_t *BA, gui_img_t *PE,
                             gui_img_t *WM,
                             gui_img_t *BB, touch_info_t *tp, gui_img_t *img_cut_arry[], bool *fruit_cut_flag)
{
    fruit_cut_cnt = 0;
    /* strawberry */
    {
        int img_w = GUI_BASE(ST)->w;
        int img_h = GUI_BASE(ST)->h;
        int img_x = GUI_BASE(ST)->x;
        int img_y = GUI_BASE(ST)->y;
        float img_rotate_angle = gui_img_get_transform_degrees(ST);

        if (!fruit_cut_flag[0])
        {
            if (tp->x >= img_x &&
                tp->x <= img_x + img_w &&
                tp->y >= img_y &&
                tp->y <= img_y + img_h) //tp is inside or on the edges of the picture
            {
                ;
            }
            else
            {
                Point img_coordinate = {(float)img_x, (float)img_y};
                Point tp_start = {(float)tp->x, (float)tp->y};
                Point tp_end = {(float)(tp->x + tp->deltaX), (float)(tp->y + tp->deltaY)};
                if (line_has_two_intersections_with_rectangle(img_coordinate, img_w, img_h, tp_start, tp_end,
                                                              img_rotate_angle))
                {
                    // if (!strcmp(ST->base.name, "img_strawberry"))
                    // {
                    gui_img_set_attribute(ST, "img_strawberry_cut", FRUIT_NINJA_STRAWBERRY_HALF_1_BIN, img_x, img_y);

                    /* display other half fruit */ // smooth display at the first time
                    gui_img_set_location(img_cut_arry[0], img_x + 10, img_y + 10);
                    gui_img_rotation(img_cut_arry[0], gui_img_get_transform_degrees(ST),
                                     gui_img_get_width(img_cut_arry[0]) / 2,
                                     gui_img_get_height(img_cut_arry[0]) / 2);
                    fruit_cut_flag[0] = true;

                    fruit_cut_cnt++;
                    // }
                }
            }
        }

    }
    /* banana */
    {
        int img_w = GUI_BASE(BA)->w;
        int img_h = GUI_BASE(BA)->h;
        int img_x = GUI_BASE(BA)->x;
        int img_y = GUI_BASE(BA)->y;
        float img_rotate_angle = gui_img_get_transform_degrees(BA);

        if (!fruit_cut_flag[1])
        {
            if (tp->x >= img_x &&
                tp->x <= img_x + img_w &&
                tp->y >= img_y &&
                tp->y <= img_y + img_h) //tp is inside or on the edges of the picture
            {
                ;
            }
            else
            {
                Point img_coordinate = {(float)img_x, (float)img_y};
                Point tp_start = {(float)tp->x, (float)tp->y};
                Point tp_end = {(float)(tp->x + tp->deltaX), (float)(tp->y + tp->deltaY)};
                if (line_has_two_intersections_with_rectangle(img_coordinate, img_w, img_h, tp_start, tp_end,
                                                              img_rotate_angle))
                {
                    if (!strcmp(BA->base.name, "img_banana"))
                    {
                        gui_img_set_attribute(BA, "img_banana_cut", FRUIT_NINJA_BANANA_HALF_1_BIN, img_x, img_y);
                        /* display other half fruit */
                        gui_img_set_location(img_cut_arry[1], img_x + 10, img_y + 10);
                        gui_img_rotation(img_cut_arry[1], gui_img_get_transform_degrees(BA),
                                         gui_img_get_width(img_cut_arry[1]) / 2,
                                         gui_img_get_height(img_cut_arry[1]) / 2);
                        fruit_cut_flag[1] = true;

                        fruit_cut_cnt++;
                    }
                }
            }
        }
    }
    /* peach */
    {
        int img_w = GUI_BASE(PE)->w;
        int img_h = GUI_BASE(PE)->h;
        int img_x = GUI_BASE(PE)->x;
        int img_y = GUI_BASE(PE)->y;
        float img_rotate_angle = gui_img_get_transform_degrees(PE);

        if (!fruit_cut_flag[2])
        {
            if (tp->x >= img_x &&
                tp->x <= img_x + img_w &&
                tp->y >= img_y &&
                tp->y <= img_y + img_h) //tp is inside or on the edges of the picture
            {
                ;
            }
            else
            {
                Point img_coordinate = {(float)img_x, (float)img_y};
                Point tp_start = {(float)tp->x, (float)tp->y};
                Point tp_end = {(float)(tp->x + tp->deltaX), (float)(tp->y + tp->deltaY)};
                if (line_has_two_intersections_with_rectangle(img_coordinate, img_w, img_h, tp_start, tp_end,
                                                              img_rotate_angle))
                {
                    if (!strcmp(PE->base.name, "img_peach"))
                    {
                        gui_img_set_attribute(PE, "img_peach_cut", FRUIT_NINJA_PEACH_HALF_1_BIN, img_x, img_y);
                        /* display other half fruit */
                        gui_img_set_location(img_cut_arry[2], img_x + 10, img_y + 10);
                        gui_img_rotation(img_cut_arry[2], gui_img_get_transform_degrees(PE),
                                         gui_img_get_width(img_cut_arry[2]) / 2,
                                         gui_img_get_height(img_cut_arry[2]) / 2);
                        fruit_cut_flag[2] = true;

                        fruit_cut_cnt++;
                    }
                }
            }
        }
    }
    /* watermelon */
    {
        int img_w = GUI_BASE(WM)->w;
        int img_h = GUI_BASE(WM)->h;
        int img_x = GUI_BASE(WM)->x;
        int img_y = GUI_BASE(WM)->y;
        float img_rotate_angle = gui_img_get_transform_degrees(WM);
        if (!fruit_cut_flag[3])
        {
            if (tp->x >= img_x &&
                tp->x <= img_x + img_w &&
                tp->y >= img_y &&
                tp->y <= img_y + img_h) //tp is inside or on the edges of the picture
            {
                ;
            }
            else
            {
                Point img_coordinate = {(float)img_x, (float)img_y};
                Point tp_start = {(float)tp->x, (float)tp->y};
                Point tp_end = {(float)(tp->x + tp->deltaX), (float)(tp->y + tp->deltaY)};
                if (line_has_two_intersections_with_rectangle(img_coordinate, img_w, img_h, tp_start, tp_end,
                                                              img_rotate_angle))
                {
                    if (!strcmp(WM->base.name, "img_watermelon"))
                    {
                        gui_img_set_attribute(WM, "img_watermelon_cut", FRUIT_NINJA_WATERMELON_HALF_1_BIN, img_x, img_y);
                        /* display other half fruit */
                        gui_img_set_location(img_cut_arry[3], img_x + 10, img_y + 10);
                        gui_img_rotation(img_cut_arry[3], gui_img_get_transform_degrees(WM),
                                         gui_img_get_width(img_cut_arry[3]) / 2,
                                         gui_img_get_height(img_cut_arry[3]) / 2);
                        fruit_cut_flag[3] = true;

                        fruit_cut_cnt++;
                    }
                }
            }
        }
    }
    /* bomb */
    {
        int img_w = GUI_BASE(BB)->w;
        int img_h = GUI_BASE(BB)->h;
        int img_x = GUI_BASE(BB)->x;
        int img_y = GUI_BASE(BB)->y;
        float img_rotate_angle = gui_img_get_transform_degrees(BB);
        if (tp->x >= img_x &&
            tp->x <= img_x + img_w &&
            tp->y >= img_y &&
            tp->y <= img_y + img_h) //tp is inside or on the edges of the picture
        {
            ;
        }
        else
        {
            Point img_coordinate = {(float)img_x, (float)img_y};
            Point tp_start = {(float)tp->x, (float)tp->y};
            Point tp_end = {(float)(tp->x + tp->deltaX), (float)(tp->y + tp->deltaY)};
            if (line_has_two_intersections_with_rectangle(img_coordinate, img_w, img_h, tp_start, tp_end,
                                                          img_rotate_angle))
            {
                return true;
            }
        }
    }
    return false;
}

// App callback function
static GUI_ANIMATION_CALLBACK(fruit_ninja_cb)
{
    gui_win_t *win = static_cast<gui_win_t *>(p);
    if (win->animate->end_frame)
    {
        game_time--;
    }

    if (get_init_flag())
    {
        // Create a Box2D world with gravity
        void *mem = gui_malloc(sizeof(b2World));
        b2Vec2 gravity(0.0f, 9.8f);
        world = new (mem) b2World(gravity);

        // Add dynamic bodys
        b2BodyDef ballBodyDef;
        ballBodyDef.type = b2_dynamicBody;
        ballBodyDef.position.Set(4, (SCREEN_HEIGHT + HEIGHT_OFFSET) * P2M);
        ballBodyDef.angularVelocity = -314;    //-PI rad/s
        ballBodyDef.linearVelocity.Set(5, -20); // move up
        body_st = world->CreateBody(&ballBodyDef);

        ballBodyDef.position.Set(8, (SCREEN_HEIGHT + HEIGHT_OFFSET) * P2M);
        body_ba = world->CreateBody(&ballBodyDef);

        ballBodyDef.position.Set(12, (SCREEN_HEIGHT + HEIGHT_OFFSET) * P2M);
        body_pe = world->CreateBody(&ballBodyDef);

        ballBodyDef.position.Set(16, (SCREEN_HEIGHT + HEIGHT_OFFSET) * P2M);
        body_wm = world->CreateBody(&ballBodyDef);

        ballBodyDef.position.Set(20, (SCREEN_HEIGHT + HEIGHT_OFFSET) * P2M);
        body_bomb = world->CreateBody(&ballBodyDef);
        //creat body shape and attach the shape to the Body
        b2CircleShape circleShape;
        circleShape.m_radius = 0.01; //RADIUS_ST * P2M; small number reducing the impact of collisions
        b2FixtureDef FixtureDef;
        FixtureDef.shape = &circleShape;
        FixtureDef.density = 1;
        FixtureDef.restitution =
            0.0f; // Set elasticity, so that the ball bounces back when it hits the boundary
        FixtureDef.friction = 0.5f;
        body_st->CreateFixture(&FixtureDef);

        body_ba->CreateFixture(&FixtureDef);

        body_pe->CreateFixture(&FixtureDef);

        body_wm->CreateFixture(&FixtureDef);

        body_bomb->CreateFixture(&FixtureDef);

        temporaryBodies.push_back(body_st);
        temporaryBodies.push_back(body_ba);
        temporaryBodies.push_back(body_pe);
        temporaryBodies.push_back(body_wm);
        temporaryBodies.push_back(body_bomb);

        // Create bg and whole fruits for displaying on the window
        img_bg = gui_img_create_from_mem(win, "img_bg", FRUIT_NINJA_BG_BIN, 0, 0, 0,
                                         0);
        img_strawberry = gui_img_create_from_mem(win, "img_strawberry", FRUIT_NINJA_STRAWBERRY_BIN, 0,
                                                 SCREEN_HEIGHT + HEIGHT_OFFSET, 0, 0);
        gui_img_set_mode(img_strawberry, IMG_SRC_OVER_MODE);
        img_banana = gui_img_create_from_mem(win, "img_banana", FRUIT_NINJA_BANANA_BIN, 0,
                                             SCREEN_HEIGHT + HEIGHT_OFFSET, 0,
                                             0);
        gui_img_set_mode(img_banana, IMG_SRC_OVER_MODE);
        img_peach = gui_img_create_from_mem(win, "img_peach", FRUIT_NINJA_PEACH_BIN, 0,
                                            SCREEN_HEIGHT + HEIGHT_OFFSET, 0,
                                            0);
        gui_img_set_mode(img_peach, IMG_SRC_OVER_MODE);
        img_watermelon = gui_img_create_from_mem(win, "img_watermelon", FRUIT_NINJA_WATERMELON_BIN, 0,
                                                 SCREEN_HEIGHT + HEIGHT_OFFSET, 0, 0);
        gui_img_set_mode(img_watermelon, IMG_SRC_OVER_MODE);

        img_bomb = gui_img_create_from_mem(win, "img_bomb", FRUIT_NINJA_BOMB_BIN, 0,
                                           SCREEN_HEIGHT + HEIGHT_OFFSET, 0, 0);
        gui_img_set_mode(img_watermelon, IMG_SRC_OVER_MODE);

        // Create half fruits for displaying on the window
        img_cut_arry[0] = gui_img_create_from_mem(win, "img_strawberry_half_cut",
                                                  FRUIT_NINJA_STRAWBERRY_HALF_2_BIN, 0,
                                                  500, 0, 0);
        gui_img_set_mode(img_cut_arry[0], IMG_SRC_OVER_MODE);
        img_cut_arry[1] = gui_img_create_from_mem(win, "img_banana_half_cut", FRUIT_NINJA_BANANA_HALF_2_BIN,
                                                  0,
                                                  600, 0, 0);
        gui_img_set_mode(img_cut_arry[1], IMG_SRC_OVER_MODE);
        img_cut_arry[2] = gui_img_create_from_mem(win, "img_peach_half_cut", FRUIT_NINJA_PEACH_HALF_2_BIN,
                                                  0,
                                                  600, 0, 0);
        gui_img_set_mode(img_cut_arry[2], IMG_SRC_OVER_MODE);
        img_cut_arry[3] = gui_img_create_from_mem(win, "img_watermelon_half_cut",
                                                  FRUIT_NINJA_WATERMELON_HALF_2_BIN, 0,
                                                  600, 0, 0);
        gui_img_set_mode(img_cut_arry[3], IMG_SRC_OVER_MODE);

        // Set score_board
        score_board = gui_text_create(win, "score_board",  30, 30, 150, 50);
        gui_text_set(score_board, (void *)"SCORE: 0", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen("SCORE: 0"),
                     32);
        void *addr1 = SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN;
        gui_text_type_set(score_board, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(score_board, LEFT);

        // Set time counter
        time_counter_content = gui_malloc(10);
        sprintf((char *)time_counter_content, "TIME: 60");
        time_counter = gui_text_create(win, "0", 280, 30, 150, 50);
        gui_text_set(time_counter, time_counter_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen((char *)time_counter_content),
                     32);
        gui_text_type_set(time_counter, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(time_counter, LEFT);

        init_flag = false;
    }
    if (world)
    {
        sprintf((char *)time_counter_content, "TIME: %d", game_time);
        gui_text_content_set(time_counter, time_counter_content, strlen((char *)time_counter_content));
        if (game_time == 0)
        {
            img_gameover = gui_img_create_from_mem(win, "img_gameover", FRUIT_NINJA_GAMEOVER_BIN, 45, 203, 0,
                                                   0);
            gui_img_set_mode(img_gameover, IMG_SRC_OVER_MODE); // pic needs to be changed
            // gui_win_stop_animation(win);
            return;
        }

        // Get the position of the ball then set the image location and rotate it on the GUI
        {
            b2Vec2 position = body_st->GetPosition();
            if (position_refresh((int)(position.x * M2P - RADIUS_ST), (int)(position.y * M2P - RADIUS_ST),
                                 img_strawberry, body_st))
            {
                gui_img_set_attribute(img_strawberry, "img_strawberry", FRUIT_NINJA_STRAWBERRY_BIN,
                                      img_strawberry->base.x, img_strawberry->base.y);
                fruit_cut_flag[0] = false;
                gui_img_set_location(img_cut_arry[0], 0, 550);
            }
            // gui_log("img_strawberry->degrees: %f\r\n", img_strawberry->degrees);

            position = body_ba->GetPosition();
            if (position_refresh((int)(position.x * M2P - RADIUS_BA), (int)(position.y * M2P - RADIUS_BA),
                                 img_banana, body_ba))
            {
                gui_img_set_attribute(img_banana, "img_banana", FRUIT_NINJA_BANANA_BIN, img_banana->base.x,
                                      img_banana->base.y);
                fruit_cut_flag[1] = false;
                gui_img_set_location(img_cut_arry[1], 0, 550);
            }

            position = body_pe->GetPosition();
            if (position_refresh((int)(position.x * M2P - RADIUS_PE), (int)(position.y * M2P - RADIUS_PE),
                                 img_peach, body_pe))
            {
                gui_img_set_attribute(img_peach, "img_peach", FRUIT_NINJA_PEACH_BIN, img_peach->base.x,
                                      img_peach->base.y);
                fruit_cut_flag[2] = false;
                gui_img_set_location(img_cut_arry[2], 0, 550);
            }

            position = body_wm->GetPosition();
            if (position_refresh((int)(position.x * M2P - RADIUS_WM), (int)(position.y * M2P - RADIUS_WM),
                                 img_watermelon, body_wm))
            {
                gui_img_set_attribute(img_watermelon, "img_watermelon", FRUIT_NINJA_WATERMELON_BIN,
                                      img_watermelon->base.x, img_watermelon->base.y);
                fruit_cut_flag[3] = false;
                gui_img_set_location(img_cut_arry[3], 0, 550);
            }

            position = body_bomb->GetPosition();
            position_refresh((int)(position.x * M2P - RADIUS_BB), (int)(position.y * M2P - RADIUS_BB), img_bomb,
                             body_bomb);

            // Refresh half-cut fruits pos
            if (fruit_cut_flag[0])
            {
                gui_img_set_location(img_cut_arry[0],  GUI_BASE(img_strawberry)->x + 10,
                                     GUI_BASE(img_strawberry)->y + 10);
                gui_img_rotation(img_cut_arry[0], gui_img_get_transform_degrees(img_strawberry),
                                 gui_img_get_width(img_cut_arry[0]) / 2,
                                 gui_img_get_height(img_cut_arry[0]) / 2);
            }
            if (fruit_cut_flag[1])
            {
                gui_img_set_location(img_cut_arry[1],  GUI_BASE(img_banana)->x + 10, GUI_BASE(img_banana)->y + 10);
                gui_img_rotation(img_cut_arry[1], gui_img_get_transform_degrees(img_banana),
                                 gui_img_get_width(img_cut_arry[1]) / 2,
                                 gui_img_get_height(img_cut_arry[1]) / 2);
            }
            if (fruit_cut_flag[2])
            {
                gui_img_set_location(img_cut_arry[2],  GUI_BASE(img_peach)->x + 10, GUI_BASE(img_peach)->y + 10);
                gui_img_rotation(img_cut_arry[2], gui_img_get_transform_degrees(img_peach),
                                 gui_img_get_width(img_cut_arry[2]) / 2,
                                 gui_img_get_height(img_cut_arry[2]) / 2);
            }
            if (fruit_cut_flag[3])
            {
                gui_img_set_location(img_cut_arry[3],  GUI_BASE(img_watermelon)->x + 10,
                                     GUI_BASE(img_watermelon)->y + 10);
                gui_img_rotation(img_cut_arry[3], gui_img_get_transform_degrees(img_watermelon),
                                 gui_img_get_width(img_cut_arry[3]) / 2,
                                 gui_img_get_height(img_cut_arry[3]) / 2);
            }
        }
        //cutting judgment
        GUI_TOUCHPAD_IMPORT_AS_TP // get touchpoint
        if (tp->released)
        {
            bool bomb_flag = cutting_judgment(win, img_strawberry, img_banana, img_peach, img_watermelon,
                                              img_bomb, tp, img_cut_arry, fruit_cut_flag);
            if (bomb_flag)
            {
                img_gameover = gui_img_create_from_mem(win, "img_gameover", FRUIT_NINJA_GAMEOVER_BIN, 45, 203, 0,
                                                       0);
                gui_img_set_mode(img_gameover, IMG_SRC_OVER_MODE); // pic needs to be changed
                gui_win_stop_animation(win);
            }
            else
            {
                fruit_score += (fruit_cut_cnt * fruit_cut_cnt * 10);
                static char text_content[16];
                sprintf(text_content, "SCORE: %d", fruit_score);
                gui_text_content_set(score_board, text_content, strlen(text_content));
            }
        }

        // Update the physics world
        world->Step(1 / 60.f, 8, 3);
    }
}

// GUI design function
static void fruit_ninja_design(gui_obj_t *obj)
{
    gui_win_t *win = gui_win_create(obj,
                                    "FRUIT_NINJA_BOX2D", 0, 0, 0, 0);

    // Set the animation function of the window
    gui_win_set_animate(win, 1000, 60, fruit_ninja_cb, win);
    init_flag = true;
    fruit_cut_flag[0] = 0;
    fruit_cut_flag[1] = 0;
    fruit_cut_flag[2] = 0;
    fruit_cut_flag[3] = 0;
    fruit_score = 0;
    game_time = 60;
}
}

extern "C" {
    void app_fruit_ninja_design(gui_obj_t *obj)
    {
        app_fruit_ninja::fruit_ninja_design(obj);
    }
    void close_FN_APP()
    {
        app_fruit_ninja::clear_world();
    }
}