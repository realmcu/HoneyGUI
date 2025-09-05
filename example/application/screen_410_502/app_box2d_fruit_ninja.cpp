/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <math.h>
#include <stdint.h>
#include <vector>
#include "root_image/ui_resource.h"
#include "app_main_watch.h"
#include "gui_win.h"
#include "gui_img.h"
#include "box2d/box2d.h"
#include "gui_text.h"
#include "tp_algo.h"
#include "guidef.h"
#include "gui_api.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define HEIGHT_OFFSET 100
#define CURRENT_VIEW_NAME "fruit_ninja_view"

/*============================================================================*
 *                                  C Interface
 *============================================================================*/
#ifndef __WIN32
/* overload new & delete */
void *lx_platform_malloc(size_t size)
{
    void *ptr = NULL;
    if (size != 0)
    {
        ptr = gui_malloc(size);

        if (ptr != NULL)
        {
            memset(ptr, 0, size);
        }
        else
        {
            gui_log("lx_platform_malloc failed!!!, size:%d", size);
        }
    }

    return (void *)ptr;
}

void lx_platform_free(void *ptr)
{
    gui_free(ptr);
}

void *operator new (size_t size)
{
    void *ptr = lx_platform_malloc(size);
    return ptr;
}

void *operator new[](size_t size)
{
    void *ptr = lx_platform_malloc(size);
    return ptr;
}

void operator delete (void *ptr) noexcept
{
    lx_platform_free(ptr);
}

void operator delete[](void *ptr) noexcept
{
    lx_platform_free(ptr);
}
#endif

extern "C" {
    static gui_view_t *current_view = NULL;
    const static gui_view_descriptor_t *menu_view = NULL;
    static void app_fruit_ninja_design(gui_view_t *view);
    static void close_FN(gui_view_t *view);

    static gui_view_descriptor_t const descriptor =
    {
        /* change Here for current view */
        .name = (const char *)CURRENT_VIEW_NAME,
        .pView = &current_view,
        .on_switch_in = app_fruit_ninja_design,
        .on_switch_out = close_FN,
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
namespace app_fruit_ninja
{
/*============================================================================*
*                           Types
*============================================================================*/
/* Define a point structure */
typedef struct
{
    float x;
    float y;
} Point;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define M2P 20 // A physical unit corresponds to 20 pixels, used to convert physical coordinates to pixel coordinates
#define P2M 1 / M2P // A pixel corresponds to a physical unit
#define RADIUS_ST 36   // st: 68*72  pixels
#define RADIUS_BA 63   // ba: 126*50 pixels
#define RADIUS_PE 31   // pe: 62*59  pixels
#define RADIUS_WM 49   // wm: 98*85  pixels
#define RADIUS_BB 25   // wm: 50*51  pixels

/*============================================================================*
*                              Variables
*============================================================================*/
/* Box2D world */
static b2World *world = nullptr; // Box2D world
static b2Body *body_st; // entities that simulate motion trajectories
static b2Body *body_ba;
static b2Body *body_pe;
static b2Body *body_wm;
static b2Body *body_bomb;

/* Fruit images */
static gui_img_t *img_strawberry;
static gui_img_t *img_banana;
static gui_img_t *img_peach;
static gui_img_t *img_watermelon;
static gui_img_t *img_bomb;
static gui_img_t
*img_cut_array[4]; // img_strawberry_cut, *img_banana_cut, *img_peach_cut, *img_watermelon_cut;


/* Score */
static int16_t fruit_score = 0;
static uint8_t fruit_cut_cnt = 0;
static bool fruit_cut_flag[4] = {0}; // record whether fruits are cut
static gui_text_t *score_board;
static gui_text_t *time_counter;
static char time_counter_content[10] = "Time: 60";
static uint8_t game_time;

/*============================================================================*
*                           Private Functions
*============================================================================*/
static void clear_world()
{
    if (world)
    {
        world->~b2World();
        gui_free(world);
        world = nullptr;
    }
}

static float gui_img_get_transform_degrees(gui_img_t *img)
{
    GUI_ASSERT(GUI_BASE(img)->type == IMAGE_FROM_MEM)
    return img->degrees;
}

/* Rotate to get rectangular's four points */
static Point rotate_point(Point p, Point center, float angle)
{
    /* converting angles to radians */
    float radians = angle * 2.f * 3.14f / 360.0f;
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


    Point center = {(float)(rect_min.x + width / 2.0f), (float)(rect_min.y + height / 2.0f)}; // calculate center point

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

    /* Check if the line segment intersects any of the sides and time_cnt the number of intersections */
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

/* Refresh image */
static bool position_refresh(int x, int y, gui_img_t *img, b2Body *body)
{
    if (y < -70 || y > 550 || x < -70 || x > SCREEN_WIDTH + 70)
    {
        b2Vec2 position((float)(100 + xorshift16() % (354 - 100 + 1)) * P2M, 550.f * P2M);
        // gui_log("x=%f,y=%f\r\n", position.x, position.y);
        b2Vec2 lv; //speed
        body->SetTransform(position, xorshift16() % (360 + 1));
        if (position.x * M2P > 227) // if position left, velocity right
        {
            lv.Set(-(float)(10 + xorshift16() % (15 - 10 + 1)),
                   -(float)(22 + xorshift16() % (22 - 20 + 1))); // velocity x:10~15; y: 18~20
        }
        else
        {
            lv.Set((float)(10 + xorshift16() % (15 - 10 + 1)), -(float)(22 + xorshift16() % (22 - 20 + 1)));
        }
        body->SetLinearVelocity(lv);
        body->SetAngularVelocity(-314);

        gui_img_translate(img, (int)(position.x * M2P), 550);
        gui_img_rotation(img, body->GetAngle());
        return true;
    }
    else
    {
        gui_img_translate(img, x, y);
        gui_img_rotation(img, body->GetAngle());
    }

    return false;
}

static bool cut_judgment(gui_img_t *img, uint8_t index, void *pic_cut)
{
    touch_info_t *tp = tp_get_info();

    int img_w = GUI_BASE(img)->w;
    int img_h = GUI_BASE(img)->h;
    int img_x = img->t_x;
    int img_y = img->t_y;
    float img_rotate_angle = gui_img_get_transform_degrees(img);

    if (!fruit_cut_flag[index] || img == img_bomb)
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
                if (img == img_bomb) {return true;}
                gui_img_set_image_data(img, (const uint8_t *)pic_cut);
                gui_img_refresh_size(img);

                /* display other half fruit */ // smooth display at the first time
                gui_img_translate(img_cut_array[index], img_x + 10, img_y + 10);
                gui_img_rotation(img_cut_array[index], gui_img_get_transform_degrees(img));
                gui_obj_hidden(GUI_BASE(img_cut_array[index]), false);
                fruit_cut_flag[index] = true;

                fruit_cut_cnt++;
            }
        }
    }
    return false;
}

// scoring, cutting pictures. if cut bomb, gameover
static bool score_judgment()
{
    fruit_cut_cnt = 0;
    cut_judgment(img_strawberry, 0, FRUIT_NINJA_STRAWBERRY_HALF_1_BIN);
    cut_judgment(img_banana, 1, FRUIT_NINJA_BANANA_HALF_1_BIN);
    cut_judgment(img_peach, 2, FRUIT_NINJA_PEACH_HALF_1_BIN);
    cut_judgment(img_watermelon, 3, FRUIT_NINJA_WATERMELON_HALF_1_BIN);
    return cut_judgment(img_bomb, 0, NULL);

}

static void app_design_core(void *parent)
{
    // Create a Box2D world with gravity
    b2Vec2 gravity(0.0f, 9.8f);
    world = new (gui_malloc(sizeof(b2World))) b2World(gravity);

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

    // Create whole fruits for displaying on the window
    img_strawberry = gui_img_create_from_mem(parent, "img_strawberry", FRUIT_NINJA_STRAWBERRY_BIN, 0,
                                             0, 0, 0);
    gui_img_translate(img_strawberry, -200, 0);
    gui_img_set_mode(img_strawberry, IMG_SRC_OVER_MODE);
    gui_img_set_focus(img_strawberry, img_strawberry->base.w / 2, img_strawberry->base.h / 2);
    img_banana = gui_img_create_from_mem(parent, "img_banana", FRUIT_NINJA_BANANA_BIN, 0,
                                         0, 0,
                                         0);
    gui_img_translate(img_banana, -200, 0);
    gui_img_set_mode(img_banana, IMG_SRC_OVER_MODE);
    gui_img_set_focus(img_banana, img_banana->base.w / 2, img_banana->base.h / 2);
    img_peach = gui_img_create_from_mem(parent, "img_peach", FRUIT_NINJA_PEACH_BIN, 0,
                                        0, 0,
                                        0);
    gui_img_translate(img_peach, -200, 0);
    gui_img_set_mode(img_peach, IMG_SRC_OVER_MODE);
    gui_img_set_focus(img_peach, img_peach->base.w / 2, img_peach->base.h / 2);
    img_watermelon = gui_img_create_from_mem(parent, "img_watermelon", FRUIT_NINJA_WATERMELON_BIN, 0,
                                             0, 0, 0);
    gui_img_translate(img_watermelon, -200, 0);
    gui_img_set_mode(img_watermelon, IMG_SRC_OVER_MODE);
    gui_img_set_focus(img_watermelon, img_watermelon->base.w / 2, img_watermelon->base.h / 2);

    img_bomb = gui_img_create_from_mem(parent, "img_bomb", FRUIT_NINJA_BOMB_BIN, 0,
                                       0, 0, 0);
    gui_img_translate(img_bomb, -200, 0);
    gui_img_set_mode(img_watermelon, IMG_SRC_OVER_MODE);
    gui_img_set_focus(img_bomb, img_bomb->base.w / 2, img_bomb->base.h / 2);

    // Create half fruits for displaying on the window
    img_cut_array[0] = gui_img_create_from_mem(parent, "img_strawberry_half_cut",
                                               FRUIT_NINJA_STRAWBERRY_HALF_2_BIN, 0,
                                               0, 0, 0);
    gui_img_set_mode(img_cut_array[0], IMG_SRC_OVER_MODE);
    gui_obj_hidden(GUI_BASE(img_cut_array[0]), true);
    gui_img_set_focus(img_cut_array[0], img_cut_array[0]->base.w / 2, img_cut_array[0]->base.h / 2);
    img_cut_array[1] = gui_img_create_from_mem(parent, "img_banana_half_cut",
                                               FRUIT_NINJA_BANANA_HALF_2_BIN,
                                               0, 0, 0, 0);
    gui_img_set_mode(img_cut_array[1], IMG_SRC_OVER_MODE);
    gui_obj_hidden(GUI_BASE(img_cut_array[1]), true);
    gui_img_set_focus(img_cut_array[1], img_cut_array[1]->base.w / 2, img_cut_array[1]->base.h / 2);
    img_cut_array[2] = gui_img_create_from_mem(parent, "img_peach_half_cut",
                                               FRUIT_NINJA_PEACH_HALF_2_BIN,
                                               0, 0, 0, 0);
    gui_img_set_mode(img_cut_array[2], IMG_SRC_OVER_MODE);
    gui_obj_hidden(GUI_BASE(img_cut_array[2]), true);
    gui_img_set_focus(img_cut_array[2], img_cut_array[2]->base.w / 2, img_cut_array[2]->base.h / 2);
    img_cut_array[3] = gui_img_create_from_mem(parent, "img_watermelon_half_cut",
                                               FRUIT_NINJA_WATERMELON_HALF_2_BIN, 0,
                                               0, 0, 0);
    gui_img_set_mode(img_cut_array[3], IMG_SRC_OVER_MODE);
    gui_obj_hidden(GUI_BASE(img_cut_array[3]), true);
    gui_img_set_focus(img_cut_array[3], img_cut_array[3]->base.w / 2, img_cut_array[3]->base.h / 2);

    // Set score_board
    score_board = gui_text_create(parent, "score_board",  30, 30, 150, 50);
    gui_text_set(score_board, (void *)"SCORE: 0", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen("SCORE: 0"),
                 32);
    gui_text_type_set(score_board, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(score_board, LEFT);

    // Set time counter
    time_counter = gui_text_create(parent, 0, 280, 30, 150, 50);
    gui_text_set(time_counter, time_counter_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time_counter_content),
                 32);
    gui_text_type_set(time_counter, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_counter, LEFT);

}

/* Click 'GAME OVER' to restart*/
static void restart_cb(void)
{
    gui_obj_child_free(GUI_BASE(current_view));
    clear_world();
    app_fruit_ninja_design(current_view);
}

static void fruit_ninja_cb(void *p)
{
    gui_obj_t *obj = GUI_BASE(p);
    static uint8_t time_cnt = 0; // time counter
    time_cnt++;
    if (time_cnt >= 50)
    {
        time_cnt = 0;
        game_time--;
    }
    if (world != NULL)
    {
        sprintf(time_counter_content, "Time: %d", game_time);
        gui_text_content_set(time_counter, time_counter_content, strlen((char *)time_counter_content));
        if (game_time == 0)
        {
            gui_img_t *img_gameover = gui_img_create_from_mem(obj, "img_gameover", FRUIT_NINJA_GAMEOVER_BIN, 35,
                                                              203, 0,
                                                              0);
            gui_obj_add_event_cb(img_gameover, (gui_event_cb_t)restart_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
            gui_obj_stop_timer(obj);
            return;
        }

        // Get the position of the ball then set the image location and rotate it on the GUI
        {
            b2Vec2 position = body_st->GetPosition();
            if (position_refresh((int)(position.x * M2P - RADIUS_ST), (int)(position.y * M2P - RADIUS_ST),
                                 img_strawberry, body_st) == 1)
            {
                gui_img_set_image_data(img_strawberry, (const uint8_t *)FRUIT_NINJA_STRAWBERRY_BIN);
                gui_img_refresh_size(img_strawberry);
                gui_img_set_focus(img_strawberry, img_strawberry->base.w / 2, img_strawberry->base.h / 2);

                fruit_cut_flag[0] = false;
                gui_obj_hidden(GUI_BASE(img_cut_array[0]), true);
            }
            // gui_log("img_strawberry->degrees: %f\r\n", img_strawberry->degrees);

            position = body_ba->GetPosition();
            if (position_refresh((int)(position.x * M2P - RADIUS_BA), (int)(position.y * M2P - RADIUS_BA),
                                 img_banana, body_ba) == 1)
            {
                gui_img_set_image_data(img_banana, (const uint8_t *)FRUIT_NINJA_BANANA_BIN);
                gui_img_refresh_size(img_banana);
                gui_img_set_focus(img_banana, img_banana->base.w / 2, img_banana->base.h / 2);

                fruit_cut_flag[1] = false;
                gui_obj_hidden(GUI_BASE(img_cut_array[1]), true);
            }

            position = body_pe->GetPosition();
            if (position_refresh((int)(position.x * M2P - RADIUS_PE), (int)(position.y * M2P - RADIUS_PE),
                                 img_peach, body_pe) == 1)
            {
                gui_img_set_image_data(img_peach, (const uint8_t *)FRUIT_NINJA_PEACH_BIN);
                gui_img_refresh_size(img_peach);
                gui_img_set_focus(img_peach, img_peach->base.w / 2, img_peach->base.h / 2);

                fruit_cut_flag[2] = false;
                gui_obj_hidden(GUI_BASE(img_cut_array[2]), true);
            }

            position = body_wm->GetPosition();
            if (position_refresh((int)(position.x * M2P - RADIUS_WM), (int)(position.y * M2P - RADIUS_WM),
                                 img_watermelon, body_wm))
            {
                gui_img_set_image_data(img_watermelon, (const uint8_t *)FRUIT_NINJA_WATERMELON_BIN);
                gui_img_refresh_size(img_watermelon);
                gui_img_set_focus(img_watermelon, img_watermelon->base.w / 2, img_watermelon->base.h / 2);

                fruit_cut_flag[3] = false;
                gui_obj_hidden(GUI_BASE(img_cut_array[3]), true);
            }

            position = body_bomb->GetPosition();
            if (position_refresh((int)(position.x * M2P - RADIUS_BB), (int)(position.y * M2P - RADIUS_BB),
                                 img_bomb,
                                 body_bomb) == 1) {;}

            // Refresh half-cut fruits pos
            if (fruit_cut_flag[0])
            {
                gui_img_translate(img_cut_array[0], img_strawberry->t_x + 10,
                                  img_strawberry->t_y + 10);
                gui_img_rotation(img_cut_array[0], gui_img_get_transform_degrees(img_strawberry));
            }
            if (fruit_cut_flag[1])
            {
                gui_img_translate(img_cut_array[1], img_banana->t_x + 10, img_banana->t_y + 10);
                gui_img_rotation(img_cut_array[1], gui_img_get_transform_degrees(img_banana));
            }
            if (fruit_cut_flag[2])
            {
                gui_img_translate(img_cut_array[2], img_peach->t_x + 10, img_peach->t_y + 10);
                gui_img_rotation(img_cut_array[2], gui_img_get_transform_degrees(img_peach));
            }
            if (fruit_cut_flag[3])
            {
                gui_img_translate(img_cut_array[3], img_watermelon->t_x + 10,
                                  img_watermelon->t_y + 10);
                gui_img_rotation(img_cut_array[3], gui_img_get_transform_degrees(img_watermelon));
            }
        }
        //cutting judgment
        touch_info_t *tp = tp_get_info();
        if (tp->released)
        {
            bool bomb_flag = score_judgment();
            if (bomb_flag)
            {
                gui_img_t *img_gameover = gui_img_create_from_mem(obj, "img_gameover", FRUIT_NINJA_GAMEOVER_BIN, 35,
                                                                  203, 0,
                                                                  0);
                gui_obj_add_event_cb(img_gameover, (gui_event_cb_t)restart_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
                gui_obj_stop_timer(GUI_BASE(obj));
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

/* GUI design function */
static void fruit_ninja_design(gui_obj_t *obj)
{
    gui_obj_create_timer(obj, 17, true, fruit_ninja_cb);
    gui_obj_start_timer(obj);

    app_design_core(obj);
    fruit_cut_flag[0] = 0;
    fruit_cut_flag[1] = 0;
    fruit_cut_flag[2] = 0;
    fruit_cut_flag[3] = 0;
    fruit_score = 0;
    game_time = 60;
}
}

/*============================================================================*
 *                                  C Interface
 *============================================================================*/
extern "C" {
    static void return_cb()
    {
        gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
    static void app_fruit_ninja_design(gui_view_t *view)
    {
        gui_obj_t *obj = GUI_BASE(view);
        gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_CLICKED);
        app_fruit_ninja::fruit_ninja_design(obj);
    }
    static void close_FN(gui_view_t *view)
    {
        (void)view;
        app_fruit_ninja::clear_world();
    }
}