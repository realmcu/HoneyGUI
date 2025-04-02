/**
 * @file lv_draw_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_draw_private.h"
#if LV_USE_DRAW_PPE_RTL8773E
#include "lv_draw_ppe_rtl8773e.h"
#include "lv_draw_sw_private.h"
#include "../../../core/lv_refr.h"
#include "../../../display/lv_display_private.h"
#include "../../../stdlib/lv_string.h"
#include "../../../core/lv_global.h"
#include "rtl_ppe.h"

/*********************
 *      DEFINES
 *********************/
#define DRAW_UNIT_ID_PPE     2

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_USE_OS
static void render_thread_cb(void *ptr);
#endif

static void execute_drawing(lv_draw_ppe_unit_t *u);

static int32_t ppe_dispatch(lv_draw_unit_t *draw_unit, lv_layer_t *layer);
static int32_t ppe_evaluate(lv_draw_unit_t *draw_unit, lv_draw_task_t *task);
static int32_t lv_draw_ppe_delete(lv_draw_unit_t *draw_unit);

/**********************
 *  STATIC VARIABLES
 **********************/
#define _draw_info LV_GLOBAL_DEFAULT()->draw_info

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_ppe_init(void)
{
    lv_draw_ppe_unit_t *draw_ppe_unit = lv_draw_create_unit(sizeof(lv_draw_ppe_unit_t));
    draw_ppe_unit->base_unit.dispatch_cb = ppe_dispatch;
    draw_ppe_unit->base_unit.evaluate_cb = ppe_evaluate;
    draw_ppe_unit->idx = 0;
    draw_ppe_unit->base_unit.delete_cb = LV_USE_OS ? lv_draw_ppe_delete : NULL;
    draw_ppe_unit->base_unit.name = "PPE";
    lv_acc_dma_channel_init();
    RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
#if LV_USE_OS
    lv_thread_init(&draw_ppe_unit->thread, LV_THREAD_PRIO_HIGH, render_thread_cb,
                   LV_DRAW_THREAD_STACK_SIZE, draw_ppe_unit);
#endif
}

void lv_draw_ppe_deinit(void)
{
}

static int32_t lv_draw_ppe_delete(lv_draw_unit_t *draw_unit)
{
#if LV_USE_OS
    lv_draw_ppe_unit_t *draw_ppe_unit = (lv_draw_ppe_unit_t *) draw_unit;

    LV_LOG_INFO("cancel software rendering thread");
    draw_ppe_unit->exit_status = true;

    if (draw_ppe_unit->inited)
    {
        lv_thread_sync_signal(&draw_ppe_unit->sync);
    }

    return lv_thread_delete(&draw_ppe_unit->thread);
#else
    LV_UNUSED(draw_unit);
    return 0;
#endif
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
static inline void execute_drawing_unit(lv_draw_ppe_unit_t *u)
{
    execute_drawing(u);

    u->task_act->state = LV_DRAW_TASK_STATE_READY;
    u->task_act = NULL;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();
}

static int32_t ppe_evaluate(lv_draw_unit_t *draw_unit, lv_draw_task_t *task)
{
    LV_UNUSED(draw_unit);

    switch (task->type)
    {
    case LV_DRAW_TASK_TYPE_FILL:
        {
            const lv_draw_fill_dsc_t *draw_dsc = (lv_draw_fill_dsc_t *) task->draw_dsc;
            if ((draw_dsc->grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE))
            {
                return 0;
            }

            if (task->preference_score > 80)
            {
                task->preference_score = 80;
                task->preferred_draw_unit_id = DRAW_UNIT_ID_PPE;
            }
        }
        return 1;
    case LV_DRAW_TASK_TYPE_LAYER:
        {
            const lv_draw_image_dsc_t *draw_dsc = (lv_draw_image_dsc_t *) task->draw_dsc;
            lv_layer_t *layer = (lv_layer_t *)draw_dsc->src;

            if (lv_ppe_get_format(layer->color_format, layer->draw_buf->data) == PPE_FORMAT_NOT_SUPPORT)
            {
                return 0;
            }

            if (draw_dsc->clip_radius > 0)
            {
                return 0;
            }

            if (task->preference_score > 80)
            {
                task->preference_score = 80;
                task->preferred_draw_unit_id = DRAW_UNIT_ID_PPE;
            }
            return 1;
        }
    case LV_DRAW_TASK_TYPE_IMAGE:
        {
            lv_draw_image_dsc_t *draw_dsc = task->draw_dsc;

            const lv_image_dsc_t *img_dsc = draw_dsc->src;
            if (draw_dsc->clip_radius > 0 || \
                (draw_dsc->recolor_opa > LV_OPA_MIN && \
                 (img_dsc->header.cf == LV_COLOR_FORMAT_ARGB8888 || img_dsc->header.cf == LV_COLOR_FORMAT_ARGB8565 \
                  || img_dsc->header.cf == LV_COLOR_FORMAT_XRGB8888)))
            {
                return 0;
            }

            bool masked = draw_dsc->bitmap_mask_src != NULL;
            if (masked)
            {
                return 0;
            }

            lv_color_format_t cf = draw_dsc->header.cf;
            PPE_PIXEL_FORMAT format = lv_ppe_get_format(img_dsc->header.cf, img_dsc->data);
            uint32_t recolor = lv_ppe_get_color(draw_dsc->recolor, 0);
            if (format == PPE_FORMAT_NOT_SUPPORT || (format == PPE_A8 && recolor != 0))
            {
                return 0;
            }

            if (cf >= LV_COLOR_FORMAT_PROPRIETARY_START)
            {
                return 0;
            }
            if (task->preference_score >= 80)
            {
                task->preference_score = 80;
                task->preferred_draw_unit_id = DRAW_UNIT_ID_PPE;
            }
            return 1;
        }
        break;
    default:
        break;
    }



    return 0;
}

static int32_t ppe_dispatch(lv_draw_unit_t *draw_unit, lv_layer_t *layer)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_ppe_unit_t *draw_ppe_unit = (lv_draw_ppe_unit_t *) draw_unit;

    /*Return immediately if it's busy with draw task*/
    if (draw_ppe_unit->task_act)
    {
        LV_PROFILER_DRAW_END;
        return 0;
    }

    lv_draw_task_t *t = NULL;
    t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_PPE);
    if (t == NULL)
    {
        LV_PROFILER_DRAW_END;
        return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    }

    void *buf = lv_draw_layer_alloc_buf(layer);
    if (buf == NULL)
    {
        LV_PROFILER_DRAW_END;
        return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_ppe_unit->base_unit.target_layer = layer;
    draw_ppe_unit->base_unit.clip_area = &t->clip_area;
    draw_ppe_unit->task_act = t;

#if LV_USE_OS
    /*Let the render thread work*/
    if (draw_ppe_unit->inited) { lv_thread_sync_signal(&draw_ppe_unit->sync); }
#else
    execute_drawing_unit(draw_ppe_unit);
#endif
    LV_PROFILER_DRAW_END;
    return 1;
}

static void execute_drawing(lv_draw_ppe_unit_t *u)
{
    LV_PROFILER_DRAW_BEGIN;
    /*Render the draw task*/
    lv_draw_task_t *t = u->task_act;
    switch (t->type)
    {
    case LV_DRAW_TASK_TYPE_FILL:
        lv_draw_ppe_fill((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
        break;
    case LV_DRAW_TASK_TYPE_IMAGE:
#if LV_DRAW_TRANSFORM_USE_MATRIX
        lv_draw_ppe_image_use_matrix((lv_draw_unit_t *)u, t->draw_dsc, &t->area, &t->matrix);
#else
        lv_draw_ppe_image((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
#endif
        break;
    case LV_DRAW_TASK_TYPE_LAYER:
#if LV_DRAW_TRANSFORM_USE_MATRIX
        lv_draw_ppe_layer_use_matrix((lv_draw_unit_t *)u, t->draw_dsc, &t->area, &t->matrix);
#else
        lv_draw_ppe_layer((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
#endif
        break;
    default:
        break;
    }
    LV_PROFILER_DRAW_END;
}

#endif /*LV_USE_DRAW_SW*/
