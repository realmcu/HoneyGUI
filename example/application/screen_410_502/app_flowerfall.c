/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"
#include "app_main_watch.h"
#include "gui_view.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "flower_view"
#define NUM_PETALS 30
#define NUM_GHOST 2

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct
{
    float driftX;    // Horizontal drift
    float driftY;    // Vertical drift
    float scale;     // Scale factor, size is 64*scale
    gui_img_t *img;  // Petal's image object
    gui_img_t *ghosts[NUM_GHOST]; // ghost images

    float target_dx; // gather target X position
    float move_speed;
} Petal;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void flower_app(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = flower_app,
};

/* Branch Management*/
static gui_img_t *branch1;
static gui_img_t *branch2;

/* Petals Management*/
static Petal petals[NUM_PETALS];

/* Branch Animation Variables */
static float branch_rot_angle = 0.0f;
static int branch_direction = 1;

/* Petals Animation Variables */
static float petal_rot_angle = 0.0f;
static float petal_oscillation = 0.0f; // petals oscillation effect

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
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


static void update_branch_rotation(void)
{
    if (branch_direction == 1)
    {
        branch_rot_angle += 0.1f;
        if (branch_rot_angle >= 10.0f)
        {
            branch_rot_angle = 10.0f;
            branch_direction = 0;
        }
    }
    else
    {
        branch_rot_angle -= 0.1f;
        if (branch_rot_angle <= 0.0f)
        {
            branch_rot_angle = 0.0f;
            branch_direction = 1;
        }
    }
    gui_img_rotation(branch1, -branch_rot_angle);
    gui_img_rotation(branch2, branch_rot_angle);
}

static void update_flower_animation(void *param)
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();
    (void)param;

    // branch animation
    update_branch_rotation();

    // petals animation
    petal_rot_angle++;
    if (tp->pressed || tp->pressing)
    {
        if (tp->pressed) // ghost effect on touch
        {
            for (int i = 0; i < NUM_PETALS; i++)
            {
                float ghost0_offset, ghost1_offset;
                if (tp->x < petals[i].driftX)
                {
                    ghost0_offset = petals[i].scale * 16.0f;
                    ghost1_offset = petals[i].scale * 32.0f;
                }
                else
                {
                    ghost0_offset = -petals[i].scale * 64.0f;
                    ghost1_offset = -petals[i].scale * 80.0f;
                }

                gui_img_translate(petals[i].ghosts[0], petals[i].driftX + ghost0_offset,
                                  petals[i].driftY - petals[i].scale * 32.0f);
                gui_img_translate(petals[i].ghosts[1], petals[i].driftX + ghost1_offset,
                                  petals[i].driftY - petals[i].scale * 32.0f);

                petals[i].ghosts[0]->base.not_show = false;
                petals[i].ghosts[1]->base.not_show = false;
            }
        }
        else // petals drift towards touch point
        {
            int new_x = tp->x + tp->deltaX;
            if (new_x != tp->history_x)
            {
                for (int i = 0; i < NUM_PETALS; i++)
                {
                    petals[i].target_dx = tp->deltaX;

                    float dist = fabsf(new_x - petals[i].driftX);
                    petals[i].move_speed =  fmaxf(1.0f, dist / 20.0f); // Adjust the speed based on the distance

                    if (fabsf(petals[i].driftX - petals[i].target_dx) > 5.0f)
                    {
                        int direction = (petals[i].target_dx > 0) ? 1 : -1;
                        petals[i].driftX += direction * petals[i].move_speed;
                    }

                    petals[i].ghosts[0]->base.not_show = true;
                    petals[i].ghosts[1]->base.not_show = true;

                }
                tp->history_x = new_x;
            }
            else // petals hover
            {
                petal_oscillation += 0.1f;
                for (int i = 0; i < NUM_PETALS; i++)
                {
                    petals[i].driftX += (4.0f + i * 0.2f) * sinf(petal_oscillation + i * 0.3f);
                }
            }
        }
    }
    else // when no touch, petals drift down
    {
        for (int i = 0; i < NUM_PETALS; i++)
        {
            petals[i].driftY += 3.0f * petals[i].scale; // big petals drift faster

            petals[i].ghosts[0]->base.not_show = true;
            petals[i].ghosts[1]->base.not_show = true;
        }
    }

    for (int i = 0; i < NUM_PETALS; i++)
    {
        gui_img_set_focus(petals[i].img, 32, 32);
        gui_img_translate(petals[i].img, petals[i].driftX, petals[i].driftY);
        gui_img_rotation(petals[i].img, petal_rot_angle);

        if (petals[i].driftY > dc->screen_height + 100)
        {
            petals[i].driftX = xorshift16() % dc->screen_width;
            petals[i].driftY = -xorshift16() % dc->screen_height;
            petals[i].scale = (xorshift16() % 41) / 100.0f + 0.1f;
            gui_img_scale(petals[i].img, petals[i].scale, petals[i].scale);
        }
        if (petals[i].driftX < -dc->screen_width)
        {
            petals[i].driftX += 2 * dc->screen_width;
        }
        else if (petals[i].driftX > dc->screen_width)
        {
            petals[i].driftX -= 2 * dc->screen_width;
        }
    }

}

static void single_petal_init(Petal *single_petal, gui_obj_t *obj, gui_dispdev_t *dc, float scale)
{
    single_petal->driftX = xorshift16() % dc->screen_width;
    single_petal->driftY = -xorshift16() % dc->screen_height;
    single_petal->scale = scale;

    int random_image_index = xorshift16() % 6;
    void *images[6] = {(void *)LEAF01_BIN, (void *)LEAF02_BIN, (void *)LEAF03_BIN,
                       (void *)LEAF04_BIN, (void *)LEAF05_BIN, (void *)LEAF06_BIN
                      };

    single_petal->img = gui_img_create_from_mem(obj, "petal", images[random_image_index], 0, 0, 0, 0);
    gui_img_set_focus(single_petal->img, 32, 32);
    gui_img_translate(single_petal->img, single_petal->driftX, single_petal->driftY);

    gui_img_scale(single_petal->img, single_petal->scale, single_petal->scale);


    single_petal->ghosts[0] = gui_img_create_from_mem(obj, "ghost0", DOT01_BIN, 0, 0, 0, 0);
    single_petal->ghosts[1] = gui_img_create_from_mem(obj, "ghost1", DOT02_BIN, 0, 0, 0, 0);

    gui_img_translate(single_petal->ghosts[0], single_petal->driftX, single_petal->driftY);
    gui_img_translate(single_petal->ghosts[1], single_petal->driftX, single_petal->driftY);

    gui_img_scale(single_petal->ghosts[0], single_petal->scale, single_petal->scale);
    gui_img_scale(single_petal->ghosts[1], single_petal->scale, single_petal->scale);
    single_petal->ghosts[0]->base.not_show = true;
    single_petal->ghosts[1]->base.not_show = true;
    single_petal->ghosts[0]->opacity_value = 100;
    single_petal->ghosts[1]->opacity_value = 150;
}

static void flower_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_dispdev_t *dc = gui_get_dc();
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    // gui_img_t *background = gui_img_create_from_mem(obj, "flower_background", BACKGROUND_BIN, 0, 0, 0,
    //                                                 0);

    for (int i = 0; i < NUM_PETALS / 2; i++)
    {
        float scale = (xorshift16() % 21) / 100.0f + 0.1f;
        single_petal_init(&petals[i], obj, dc, scale);
    }

    branch1 = gui_img_create_from_mem(obj, "branch1", BRANCH01_BIN, 0, 0, 0, 0);
    gui_img_set_focus(branch1, 336, 0);
    gui_img_translate(branch1, 410, 50);

    for (int i = NUM_PETALS / 2; i < NUM_PETALS; i++)
    {
        float scale = (xorshift16() % 21) / 100.0f + 0.3f;
        single_petal_init(&petals[i], obj, dc, scale);
    }

    branch2 = gui_img_create_from_mem(obj, "branch2", BRANCH02_BIN, 0, 300, 0, 0);

    gui_obj_create_timer(GUI_BASE(view), 10, true, update_flower_animation);

}