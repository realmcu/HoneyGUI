/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include <limits.h>
#include <math.h>
#include "guidef.h"
#include "def_list.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "gui_obj_tree.h"
#include "gui_obj_event.h"
#include "gui_matrix.h"
#include "tp_algo.h"
#include "gui_fb.h"
#include "gui_api_os.h"
#include "gui_card_list.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/

/* Maximum slot_count (display_count <= 21 -> slot_count <= 23). */
#define CARD_LIST_MAX_SLOTS  24

#define CARD_LIST_EFFECTIVE_LOOP(cl) ((cl)->loop && (cl)->card_count > 0)
/* Sentinel stored in virtual_id for a free/unassigned slot.
 * Must differ from any valid logical virtual_id (which can be any integer in
 * loop mode), so we use INT_MIN as an out-of-band value. */
#define CARD_LIST_SLOT_FREE_ID INT_MIN

/*============================================================================*
 *                           Forward Declarations
 *============================================================================*/
static void gui_card_list_input_prepare(gui_obj_t *obj);
static void gui_card_list_prepare(gui_obj_t *obj);
static void gui_card_list_destroy(gui_obj_t *obj);
static void gui_card_list_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);

static void gui_card_slot_prepare(gui_obj_t *obj);
static void gui_card_slot_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);

static void gui_card_list_pressing_cb(void *obj, gui_event_t *e);
static void gui_card_list_released_cb(void *obj, gui_event_t *e);

static void card_list_inertia_motion(gui_card_list_t *cl);
static void card_list_update_pool(gui_card_list_t *cl);
static void card_list_update_visibility(gui_card_list_t *cl);
static void card_list_sort_zorder(gui_card_list_t *cl);

/*============================================================================*
 *                           Inline Helpers
 *============================================================================*/
static inline int   clamp_i(int v, int lo, int hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}
static inline float clamp_f(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

/* Wrap offset into canonical range (-period, 0] for loop mode. */
static inline void wrap_offset(int *offset, int period)
{
    if (period <= 0) { return; }
    while (*offset > 0)       { *offset -= period; }
    while (*offset <= -period) { *offset += period; }
}

/*============================================================================*
 *                     Slot Pool - Internal Helpers
 *============================================================================*/

/**
 * @brief Initialise one pool slot at array index @p idx.
 *
 * Slots are embedded in the @p cl->slots array (not individually heap-allocated),
 * so we use gui_obj_ctor without gui_obj_create.  All slots start hidden.
 */
static void card_slot_init(gui_card_list_t *cl, int idx)
{
    gui_card_slot_t *s = &cl->slots[idx];
    memset(s, 0, sizeof(gui_card_slot_t));

    gui_obj_ctor(&s->base, &cl->base, "card_slot",
                 0, 0, (int16_t)cl->card_w, (int16_t)cl->card_h);

    gui_list_init(&s->base.child_list);
    s->base.obj_cb            = gui_card_slot_cb;
    s->base.has_prepare_cb    = true;
    s->base.type              = CARD_LIST_SLOT;
    s->base.create_done       = true;
    s->virtual_id             = CARD_LIST_SLOT_FREE_ID;
    s->card_id                = -1;
    s->base.not_show          = true;

    /* Append to card_list's child_list (z-order is rebuilt every frame). */
    gui_list_insert_before(&cl->base.child_list, &s->base.brother_list);
}

/*============================================================================*
 *                         Touch Event Callbacks
 *============================================================================*/

static void gui_card_list_pressing_cb(void *obj, gui_event_t *e)
{
    (void)e;
    gui_card_list_t *cl = (gui_card_list_t *)obj;
    touch_info_t    *tp = tp_get_info();

    cl->scroll_to_active = false;

    int delta = (cl->dir == 0) ? (int)tp->deltaX : (int)tp->deltaY;

    cl->offset = cl->hold + delta;

    /* Hard rubber-band clamping in non-loop mode. */
    if (!CARD_LIST_EFFECTIVE_LOOP(cl) && cl->card_count > 0)
    {
        int min_off = -(int)(cl->card_count - 1) * cl->slot_size;
        int rubber  = cl->slot_size / 2;
        if (cl->offset > rubber)            { cl->offset = rubber; }
        else if (cl->offset < min_off - rubber) { cl->offset = min_off - rubber; }
    }

    /* Shift speed record and store latest delta. */
    for (int i = 4; i > 0; i--) { cl->record[i] = cl->record[i - 1]; }
    cl->record[0] = (int16_t)delta;
}

static void gui_card_list_released_cb(void *obj, gui_event_t *e)
{
    (void)e;
    gui_card_list_t *cl = (gui_card_list_t *)obj;

    bool effective_loop = CARD_LIST_EFFECTIVE_LOOP(cl);

    if (effective_loop)
    {
        int period = (int)cl->card_count * cl->slot_size;
        wrap_offset(&cl->offset, period);
    }
    else if (cl->card_count > 0)
    {
        int min_off = -(int)(cl->card_count - 1) * cl->slot_size;
        if (cl->offset > 0)       { cl->offset = 0; }
        if (cl->offset < min_off) { cl->offset = min_off; }
    }

    cl->hold = cl->offset;

    /* Speed = delta between oldest and newest record entry. */
    cl->speed = (int16_t)(cl->record[0] - cl->record[4]);
    int max_speed = cl->slot_size;
    if (cl->speed >  max_speed) { cl->speed = (int16_t) max_speed; }
    if (cl->speed < -max_speed) { cl->speed = (int16_t) - max_speed; }

    memset(cl->record, 0, sizeof(cl->record));
}

/*============================================================================*
 *                       Inertia & Auto-Align
 *============================================================================*/

static void card_list_inertia_motion(gui_card_list_t *cl)
{
    if (cl->card_count == 0) { return; }

    bool effective_loop = CARD_LIST_EFFECTIVE_LOOP(cl);
    int  period  = (int)cl->card_count * cl->slot_size;
    int  min_off = -(int)(cl->card_count - 1) * cl->slot_size;

    if (cl->scroll_to_active)
    {
        /* Programmatic ease-to-target. */
        int dist = cl->scroll_target - cl->offset;
        if (dist == 0)
        {
            cl->scroll_to_active = false;
        }
        else
        {
            int step = (int)((float)dist * 0.2f);
            if (step == 0) { step = (dist > 0) ? 1 : -1; }
            cl->offset += step;
            if (abs(cl->offset - cl->scroll_target) <= 1)
            {
                cl->offset           = cl->scroll_target;
                cl->scroll_to_active = false;
                if (effective_loop) { wrap_offset(&cl->offset, period); }
            }
        }
    }
    else if (cl->inertia && cl->speed != 0)
    {
        /* Inertia (fling) phase. */
        cl->offset += cl->speed;
        cl->speed   = (int16_t)((float)cl->speed * (1.0f - cl->factor));
        if (abs((int)cl->speed) < 1) { cl->speed = 0; }

        if (effective_loop)
        {
            wrap_offset(&cl->offset, period);
        }
        else
        {
            if (cl->offset > 0)       { cl->offset = 0;       cl->speed = 0; }
            if (cl->offset < min_off) { cl->offset = min_off; cl->speed = 0; }
        }
    }
    else
    {
        /* Auto-align: snap to nearest card slot. */
        cl->speed = 0;
        float f_snap = roundf((float)cl->offset / (float)cl->slot_size) * (float)cl->slot_size;
        int   snap   = (int)f_snap;

        if (!effective_loop)
        {
            snap = clamp_i(snap, min_off, 0);
        }

        int dist = snap - cl->offset;
        if (dist != 0)
        {
            int step = (int)((float)dist * 0.2f);
            if (step == 0) { step = (dist > 0) ? 1 : -1; }
            cl->offset += step;
        }
        else if (effective_loop)
        {
            wrap_offset(&cl->offset, period);
        }
    }

    /* Keep hold in sync so the next finger-down starts from the right base. */
    cl->hold = cl->offset;
}

/*============================================================================*
 *                         Pool Management
 *============================================================================*/

static void card_list_update_pool(gui_card_list_t *cl)
{
    if (cl->card_count == 0 || cl->card_design == NULL) { return; }

    bool effective_loop = CARD_LIST_EFFECTIVE_LOOP(cl);
    int  half           = cl->display_count / 2;
    int  center_virtual = (int)roundf((float)(-cl->offset) / (float)cl->slot_size);

    /* Build the list of (virtual_id, card_id) pairs that should be visible. */
    int needed_vids[CARD_LIST_MAX_SLOTS];
    int needed_cids[CARD_LIST_MAX_SLOTS];
    int needed_n = 0;

    for (int s = -(half + 1); s <= (half + 1); s++)
    {
        int vid = center_virtual + s;
        int cid;

        if (effective_loop)
        {
            cid = ((vid % (int)cl->card_count) + (int)cl->card_count) % (int)cl->card_count;
        }
        else
        {
            if (vid < 0 || vid >= (int)cl->card_count) { continue; }
            cid = vid;
        }

        needed_vids[needed_n] = vid;
        needed_cids[needed_n] = cid;
        needed_n++;
    }

    /* Pass 1: release slots whose virtual_id is no longer needed. */
    bool needed_assigned[CARD_LIST_MAX_SLOTS] = {false};

    for (int i = 0; i < cl->slot_count; i++)
    {
        gui_card_slot_t *s = &cl->slots[i];
        if (s->virtual_id == CARD_LIST_SLOT_FREE_ID) { continue; }

        bool found = false;
        for (int j = 0; j < needed_n; j++)
        {
            if (s->virtual_id == needed_vids[j])
            {
                found             = true;
                needed_assigned[j] = true;
                break;
            }
        }

        if (!found)
        {
            gui_obj_child_free(&s->base);   /* Frees children, reinits child_list. */
            s->virtual_id     = CARD_LIST_SLOT_FREE_ID;
            s->card_id        = -1;
            s->base.not_show  = true;
        }
    }

    /* Pass 2: assign free slots to unassigned needed entries. */
    for (int j = 0; j < needed_n; j++)
    {
        if (needed_assigned[j]) { continue; }

        for (int i = 0; i < cl->slot_count; i++)
        {
            if (cl->slots[i].virtual_id == CARD_LIST_SLOT_FREE_ID)
            {
                gui_card_slot_t *s = &cl->slots[i];
                s->virtual_id    = needed_vids[j];
                s->card_id       = needed_cids[j];
                s->base.not_show = false;
                cl->card_design(&s->base, s->card_id, cl->design_param);
                break;
            }
        }
    }
}

/*
 * @brief Clamp slot visibility to [-(display_count/2+1), +(display_count/2+1)].
 *
 * Buffer slots (abs_pos >= half+1) are hidden so only display_count cards are
 * visible at rest.  They are re-shown as they scroll into the visible zone.
 * Must be called in INPUT_PREPARE (after card_list_update_pool) so the
 * not_show flag is honoured by the subsequent obj_draw_prepare traversal.
 */
static void card_list_update_visibility(gui_card_list_t *cl)
{
    float max_abs = (float)(cl->display_count / 2) + 1.0f;

    for (int i = 0; i < cl->slot_count; i++)
    {
        gui_card_slot_t *s = &cl->slots[i];

        if (s->virtual_id == CARD_LIST_SLOT_FREE_ID)
        {
            s->base.not_show = true;
            continue;
        }

        float sp    = (float)s->virtual_id + (float)cl->offset / (float)cl->slot_size;
        s->base.not_show = (fabsf(sp) >= max_abs);
    }
}

/*============================================================================*
 *                          Z-Order Sorting
 *============================================================================*/

/**
 * @brief Rebuild card_list->child_list in ascending distance order.
 *
 * Farthest slots are inserted first (= rendered first = bottom layer).
 * Centre slot (|slot_pos| smallest) is inserted last (= top layer).
 * Inactive slots (virtual_id == CARD_LIST_SLOT_FREE_ID) always go to the bottom.
 */
static void card_list_sort_zorder(gui_card_list_t *cl)
{
    int n = cl->slot_count;
    if (n == 0) { return; }

    /* Compute sort keys: |slot_pos| * 1000, descending = bottom first. */
    int sort_val[CARD_LIST_MAX_SLOTS];
    int order[CARD_LIST_MAX_SLOTS];

    for (int i = 0; i < n; i++)
    {
        order[i] = i;
        if (cl->slots[i].virtual_id == CARD_LIST_SLOT_FREE_ID)
        {
            sort_val[i] = 999000;
        }
        else
        {
            float sp    = (float)cl->slots[i].virtual_id
                          + (float)cl->offset / (float)cl->slot_size;
            sort_val[i] = (int)(fabsf(sp) * 1000.0f);
        }
    }

    /* Insertion sort descending (farthest first -> bottom of child_list). */
    for (int i = 1; i < n; i++)
    {
        int key_idx = order[i];
        int key_val = sort_val[key_idx];
        int j = i - 1;
        while (j >= 0 && sort_val[order[j]] < key_val)
        {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = key_idx;
    }

    /*
     * Detach all slots from child_list, then re-insert in sort order.
     * gui_list_insert_before(head, n) appends to the tail of the list,
     * so the last inserted slot becomes the topmost layer.
     */
    for (int i = 0; i < n; i++)
    {
        gui_list_remove(&cl->slots[i].base.brother_list);
    }
    for (int i = 0; i < n; i++)
    {
        gui_list_insert_before(&cl->base.child_list,
                               &cl->slots[order[i]].base.brother_list);
    }
}

/*============================================================================*
 *                      card_list - Lifecycle Callbacks
 *============================================================================*/

static void gui_card_list_input_prepare(gui_obj_t *obj)
{
    gui_card_list_t *cl = (gui_card_list_t *)obj;

    if (cl->enable_scroll)
    {
        touch_info_t *tp = tp_get_info();
        if (!tp->pressing)
        {
            card_list_inertia_motion(cl);
        }
    }

    card_list_update_pool(cl);
    card_list_update_visibility(cl);
}

static void gui_card_list_prepare(gui_obj_t *obj)
{
    gui_card_list_t *cl = (gui_card_list_t *)obj;

    if (cl->enable_scroll)
    {
        if (cl->dir == 0)
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, "touch");
        }
        else
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_VERTICAL, "touch");
        }
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    }

    card_list_sort_zorder(cl);

    /* Track scrolling state for gui_card_list_is_scrolling(). */
    {
        touch_info_t *tp   = tp_get_info();
        bool pressing      = tp->pressing;
        bool aligned       = cl->slot_size > 0 &&
                             (cl->offset % cl->slot_size == 0);
        bool idle          = !pressing && !cl->scroll_to_active &&
                             cl->speed == 0 && aligned;
        cl->is_scrolling = idle ? 0u : 1u;
    }

    uint8_t last      = cl->checksum;
    cl->checksum      = 0;
    cl->checksum      = gui_obj_checksum(0, (uint8_t *)cl, (uint8_t)sizeof(gui_card_list_t));
    if (last != cl->checksum)
    {
        gui_fb_change();
    }
}

static void gui_card_list_destroy(gui_obj_t *obj)
{
    gui_card_list_t *cl = (gui_card_list_t *)obj;
    if (!cl->slots) { return; }

    for (int i = 0; i < cl->slot_count; i++)
    {
        gui_card_slot_t *s = &cl->slots[i];

        /* Free any children created by card_design. */
        gui_obj_child_free(&s->base);

        /* Free the matrix allocated by gui_obj_ctor. */
        if (s->base.matrix)
        {
            gui_free(s->base.matrix);
            s->base.matrix = NULL;
        }

        /* Free any event descriptors. */
        if (s->base.event_dsc)
        {
            gui_free(s->base.event_dsc);
            s->base.event_dsc = NULL;
        }
    }

    gui_free(cl->slots);
    cl->slots = NULL;
}

static void gui_card_list_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (!obj) { return; }
    switch (cb_type)
    {
    case OBJ_INPUT_PREPARE: gui_card_list_input_prepare(obj); break;
    case OBJ_PREPARE:       gui_card_list_prepare(obj);       break;
    case OBJ_DESTROY:       gui_card_list_destroy(obj);       break;
    default:                                                   break;
    }
}

/*============================================================================*
 *                      card_slot - Lifecycle Callbacks
 *============================================================================*/

/**
 * @brief Apply the 3-D perspective transform for this slot in OBJ_PREPARE.
 *
 * By the time this is called, obj->matrix already holds:
 *   parent_chain_transforms * T(slot->base.x, slot->base.y)
 * which (since x=y=0) equals the card_list's accumulated transform.
 *
 * We post-multiply with a projective matrix that maps the card's logical
 * rectangle (0,0,card_w,card_h) to the four 3-D perspective-projected
 * corners computed for this slot's position in the carousel.
 */
static void gui_card_slot_prepare(gui_obj_t *obj)
{
    gui_card_slot_t *slot = (gui_card_slot_t *)obj;
    if (slot->virtual_id == CARD_LIST_SLOT_FREE_ID) { return; }

    gui_card_list_t *cl     = (gui_card_list_t *)obj->parent;
    float            cl_w   = (float)cl->base.w;
    float            cl_h   = (float)cl->base.h;
    float            card_w = (float)cl->card_w;
    float            card_h = (float)cl->card_h;

    /* Fractional position along the carousel axis (0 = centre). */
    float slot_pos = (float)slot->virtual_id
                     + (float)cl->offset / (float)cl->slot_size;
    float abs_pos  = fabsf(slot_pos);

    /* Visual attributes derived from distance from centre. */
    float scale    = fmaxf(cl->min_scale, 1.0f - abs_pos * cl->scale_step);
    float opacity_f = fmaxf(cl->min_opacity, 1.0f - abs_pos * cl->opacity_step);

    /* Smoothly fade buffer slots out as they leave the visible zone. */
    float vis_edge = (float)(cl->display_count / 2);
    if (abs_pos > vis_edge)
    {
        float t = clamp_f(abs_pos - vis_edge, 0.0f, 1.0f);
        opacity_f *= (1.0f - t);
    }

    /* Rotation angle (clamped to avoid degenerate perspective). */
    float theta_deg = clamp_f(slot_pos * cl->angle_per_slot, -85.0f, 85.0f);
    float theta     = theta_deg * (float)M_PI / 180.0f;
    float cos_t     = cosf(theta);
    float sin_t     = sinf(theta);

    /*
     * z_s controls the facing direction of off-centre cards.
     *
     * The "inward" tilt sign is physically opposite for rotateY (horizontal)
     * vs rotateX (vertical) because the two rotation axes are orthogonal.
     *
     *   HORIZONTAL (rotateY):
     *     z_s = +1 -> face_inward: outer/right edge of right card is closer,
     *                inner/left edge goes back -> front face toward centre.
     *     z_s = -1 -> face_outward (opposite tilt).
     *
     *   VERTICAL (rotateX):
     *     z_s = -1 -> face_inward: inner/top edge of bottom card goes back,
     *                outer/bottom edge comes forward -> front face toward centre.
     *     z_s = +1 -> face_outward (original vertical behaviour).
     *
     * Because sin_t carries the algebraic sign of slot_pos the same vertex
     * formula works symmetrically for left/right (or top/bottom) cards.
     */
    float z_s = (cl->dir == 0) ? (cl->face_inward ?  1.0f : -1.0f)   /* H */
                : (cl->face_inward ? -1.0f :  1.0f);  /* V */

    float hw = card_w * 0.5f * scale;
    float hh = card_h * 0.5f * scale;
    float z_off = -abs_pos * cl->z_per_slot;

    /* 3-D corner vertices in card_list local coordinate space. */
    gui_vertex_t v0, v1, v2, v3;

    if (cl->dir == 0) /* HORIZONTAL - rotateY */
    {
        float cx = cl_w * 0.5f + slot_pos * cl->screen_gap;
        float cy = cl_h * 0.5f + abs_pos  * cl->y_arc_per_slot;

        /*
         * rotateY around the card centre.
         * z_s = +1 (face_inward for H): outer/right edge of a right card
         *   comes forward, inner/left edge goes back.
         * z_s = -1 (face_outward for H): left edge forward - opposite tilt.
         *
         * For a left card (slot_pos < 0) sin_t < 0, so the formula flips
         * automatically - no extra branch needed.
         */
        v0 = (gui_vertex_t) {cx - hw *cos_t, cy - hh, z_off - z_s *hw *sin_t};   /* TL */
        v1 = (gui_vertex_t) {cx + hw *cos_t, cy - hh, z_off + z_s *hw *sin_t};   /* TR */
        v2 = (gui_vertex_t) {cx + hw *cos_t, cy + hh, z_off + z_s *hw *sin_t};   /* BR */
        v3 = (gui_vertex_t) {cx - hw *cos_t, cy + hh, z_off - z_s *hw *sin_t};   /* BL */
    }
    else /* VERTICAL - rotateX */
    {
        float cx = cl_w * 0.5f + abs_pos  * cl->y_arc_per_slot;
        float cy = cl_h * 0.5f + slot_pos * cl->screen_gap;

        /*
         * rotateX around the card centre.
         * z_s = -1 (face_inward for V): inner/top edge of a bottom card
         *   goes back, outer/bottom edge comes forward.
         * z_s = +1 (face_outward for V): top edge forward - original V tilt.
         *
         * For a top card (slot_pos < 0) sin_t < 0 -> formula flips correctly.
         */
        v0 = (gui_vertex_t) {cx - hw, cy - hh *cos_t, z_off + z_s *hh *sin_t};   /* TL */
        v1 = (gui_vertex_t) {cx + hw, cy - hh *cos_t, z_off + z_s *hh *sin_t};   /* TR */
        v2 = (gui_vertex_t) {cx + hw, cy + hh *cos_t, z_off - z_s *hh *sin_t};   /* BR */
        v3 = (gui_vertex_t) {cx - hw, cy + hh *cos_t, z_off - z_s *hh *sin_t};   /* BL */
    }

    /* Perspective eye at card_list centre, distance fov_depth along Z. */
    gui_vertex_t p = {cl_w * 0.5f, cl_h * 0.5f, cl->fov_depth};

    gui_matrix_t temp;
    /* matrix_transfrom_blit modifies v0..v3 in-place (projects to 2-D). */
    matrix_transfrom_blit(card_w, card_h, &p, &v0, &v1, &v2, &v3, &temp);

    /* Chain with the accumulated parent transform. */
    matrix_multiply(obj->matrix, &temp);

    obj->opacity_value = (uint8_t)(opacity_f * 255.0f);
}

static void gui_card_slot_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (!obj) { return; }
    switch (cb_type)
    {
    case OBJ_PREPARE: gui_card_slot_prepare(obj); break;
    default:                                       break;
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_card_list_t *gui_card_list_create(void       *parent,
                                      const char *name,
                                      int16_t     x,
                                      int16_t     y,
                                      int16_t     w,
                                      int16_t     h,
                                      uint16_t    card_w,
                                      uint16_t    card_h,
                                      uint8_t     display_count,
                                      uint8_t     dir,
                                      void (*card_design)(gui_obj_t *obj, int card_id, void *param),
                                      void       *param)
{
    GUI_ASSERT(parent != NULL);

    /* Enforce odd display_count >= 3. */
    if (display_count < 3)            { display_count = 3; }
    if ((display_count & 1u) == 0u)   { display_count--; }

    gui_card_list_t *cl = gui_malloc(sizeof(gui_card_list_t));
    GUI_ASSERT(cl != NULL);
    memset(cl, 0, sizeof(gui_card_list_t));

    if (w == 0) { w = (int16_t)((int)gui_get_screen_width()  - x); }
    if (h == 0) { h = (int16_t)((int)gui_get_screen_height() - y); }

    gui_obj_ctor(&cl->base, (gui_obj_t *)parent, name, x, y, w, h);

    cl->dir           = (dir != 0) ? 1u : 0u;
    cl->card_w        = card_w;
    cl->card_h        = card_h;
    cl->display_count = display_count;
    cl->slot_count    = display_count + 2;
    cl->slot_size     = (cl->dir == 0) ? (int)card_w : (int)card_h;
    {
        int list_size = (cl->dir == 0) ? (int)cl->base.w : (int)cl->base.h;
        int card_size = (cl->dir == 0) ? (int)card_w      : (int)card_h;
        int gap_space = list_size - card_size;
        int n_gaps    = (cl->display_count > 1) ? (cl->display_count - 1) : 1;
        cl->screen_gap = (gap_space > 0)
                         ? (float)gap_space / (float)n_gaps
                         : 1.0f;
    }

    cl->min_scale     = 0.6f;
    cl->scale_step    = 0.15f;
    cl->min_opacity   = 0.2f;
    cl->opacity_step  = 0.25f;
    cl->angle_per_slot = 22.0f;
    cl->y_arc_per_slot = 8.0f;
    cl->z_per_slot    = 50.0f;
    cl->fov_depth     = 1200.0f;
    cl->factor        = 0.05f;
    cl->loop          = true;
    cl->inertia       = true;
    cl->enable_scroll = true;
    cl->face_inward   = 1;
    cl->card_design   = card_design;
    cl->design_param  = param;

    GET_BASE(cl)->obj_cb          = gui_card_list_cb;
    GET_BASE(cl)->has_input_prepare_cb = true;
    GET_BASE(cl)->has_prepare_cb  = true;
    GET_BASE(cl)->has_destroy_cb  = true;
    GET_BASE(cl)->type            = CARD_LIST;
    GET_BASE(cl)->create_done     = true;

    gui_list_init(&GET_BASE(cl)->child_list);
    gui_list_insert_before(&((gui_obj_t *)parent)->child_list,
                           &cl->base.brother_list);

    /* Register scroll and release event callbacks. */
    if (cl->dir == 0)
    {
        gui_obj_add_event_cb(cl, gui_card_list_pressing_cb,
                             GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, NULL);
    }
    else
    {
        gui_obj_add_event_cb(cl, gui_card_list_pressing_cb,
                             GUI_EVENT_TOUCH_SCROLL_VERTICAL, NULL);
    }
    gui_obj_add_event_cb(cl, gui_card_list_released_cb,
                         GUI_EVENT_TOUCH_RELEASED, NULL);

    /* Allocate and initialise the slot pool. */
    cl->slots = gui_malloc(cl->slot_count * sizeof(gui_card_slot_t));
    GUI_ASSERT(cl->slots != NULL);
    memset(cl->slots, 0, cl->slot_count * sizeof(gui_card_slot_t));

    for (int i = 0; i < cl->slot_count; i++)
    {
        card_slot_init(cl, i);
    }

    return cl;
}

void gui_card_list_set_card_count(gui_card_list_t *cl, uint16_t count)
{
    GUI_ASSERT(cl != NULL);

    cl->card_count = count;

    /* Release all active slot contents. */
    for (int i = 0; i < cl->slot_count; i++)
    {
        gui_card_slot_t *s = &cl->slots[i];
        if (s->virtual_id != CARD_LIST_SLOT_FREE_ID)
        {
            gui_obj_child_free(&s->base);
            s->virtual_id    = CARD_LIST_SLOT_FREE_ID;
            s->card_id       = -1;
            s->base.not_show = true;
        }
    }

    /* Reset scroll to card[0]. */
    cl->offset           = 0;
    cl->hold             = 0;
    cl->speed            = 0;
    cl->scroll_to_active = false;
    memset(cl->record, 0, sizeof(cl->record));

    gui_fb_change();
}

void gui_card_list_enable_loop(gui_card_list_t *cl, bool enable)
{
    GUI_ASSERT(cl != NULL);
    cl->loop = enable ? 1u : 0u;
}

void gui_card_list_enable_scroll(gui_card_list_t *cl, bool enable)
{
    GUI_ASSERT(cl != NULL);
    cl->enable_scroll = enable ? 1u : 0u;
}

void gui_card_list_set_face_inward(gui_card_list_t *cl, bool inward)
{
    GUI_ASSERT(cl != NULL);
    cl->face_inward = inward ? 1u : 0u;
}

void gui_card_list_set_factor(gui_card_list_t *cl, float factor)
{
    GUI_ASSERT(cl != NULL);
    cl->factor = factor;
}

void gui_card_list_set_effect_params(gui_card_list_t *cl,
                                     float screen_gap,
                                     float min_scale,
                                     float scale_step,
                                     float min_opacity,
                                     float opacity_step,
                                     float angle_per_slot,
                                     float y_arc_per_slot,
                                     float z_per_slot,
                                     float fov_depth)
{
    GUI_ASSERT(cl != NULL);
    if (screen_gap    != 0.0f) { cl->screen_gap    = screen_gap;    }
    if (min_scale     != 0.0f) { cl->min_scale     = min_scale;     }
    if (scale_step    != 0.0f) { cl->scale_step    = scale_step;    }
    if (min_opacity   != 0.0f) { cl->min_opacity   = min_opacity;   }
    if (opacity_step  != 0.0f) { cl->opacity_step  = opacity_step;  }
    if (angle_per_slot != 0.0f) { cl->angle_per_slot = angle_per_slot;}
    if (y_arc_per_slot != 0.0f) { cl->y_arc_per_slot = y_arc_per_slot;}
    if (z_per_slot    != 0.0f) { cl->z_per_slot    = z_per_slot;    }
    if (fov_depth     != 0.0f) { cl->fov_depth     = fov_depth;     }
}

int gui_card_list_get_current_index(gui_card_list_t *cl)
{
    GUI_ASSERT(cl != NULL);
    if (cl->card_count == 0) { return -1; }

    int idx = (int)roundf((float)(-cl->offset) / (float)cl->slot_size);
    if (CARD_LIST_EFFECTIVE_LOOP(cl))
    {
        idx = ((idx % (int)cl->card_count) + (int)cl->card_count) % (int)cl->card_count;
    }
    else
    {
        idx = clamp_i(idx, 0, (int)cl->card_count - 1);
    }
    return idx;
}

gui_obj_t *gui_card_list_get_current_card(gui_card_list_t *cl)
{
    GUI_ASSERT(cl != NULL);
    if (cl->card_count == 0 || cl->slots == NULL) { return NULL; }

    int idx = gui_card_list_get_current_index(cl);

    for (int i = 0; i < cl->slot_count; i++)
    {
        gui_card_slot_t *s = &cl->slots[i];
        if (s->virtual_id != CARD_LIST_SLOT_FREE_ID && s->card_id == idx)
        {
            /* Return the first child - the card widget created by card_design. */
            if (s->base.child_list.next != &s->base.child_list)
            {
                return gui_list_entry(s->base.child_list.next, gui_obj_t, brother_list);
            }
            return NULL;
        }
    }
    return NULL;
}

gui_obj_t *gui_card_list_get_card_by_index(gui_card_list_t *cl, int idx)
{
    GUI_ASSERT(cl != NULL);
    if (cl->card_count == 0 || cl->slots == NULL) { return NULL; }
    if (idx < 0 || idx >= (int)cl->card_count) { return NULL; }

    for (int i = 0; i < cl->slot_count; i++)
    {
        gui_card_slot_t *s = &cl->slots[i];
        if (s->virtual_id != CARD_LIST_SLOT_FREE_ID && s->card_id == idx)
        {
            if (s->base.child_list.next != &s->base.child_list)
            {
                return gui_list_entry(s->base.child_list.next, gui_obj_t, brother_list);
            }
            return NULL;
        }
    }
    return NULL;
}

void gui_card_list_scroll_to_card(gui_card_list_t *cl, int idx)
{
    GUI_ASSERT(cl != NULL);
    if (cl->card_count == 0) { return; }

    idx = clamp_i(idx, 0, (int)cl->card_count - 1);

    int target = -(int)idx * cl->slot_size;

    if (CARD_LIST_EFFECTIVE_LOOP(cl))
    {
        /* Choose the shortest modular path from current offset. */
        int period = (int)cl->card_count * cl->slot_size;
        int cur = cl->offset;
        wrap_offset(&cur, period);

        int best_target = target;
        int best_dist   = abs(cur - target);
        for (int k = -1; k <= 1; k++)
        {
            int t = target + k * period;
            int d = abs(cur - t);
            if (d < best_dist)
            {
                best_dist   = d;
                best_target = t;
            }
        }
        target = best_target;
    }

    cl->scroll_target    = target;
    cl->scroll_to_active = true;
    cl->speed            = 0;
    gui_fb_change();
}

void gui_card_list_jump_to_card(gui_card_list_t *cl, int idx)
{
    GUI_ASSERT(cl != NULL);
    if (cl->card_count == 0) { return; }

    idx = clamp_i(idx, 0, (int)cl->card_count - 1);

    cl->offset           = -(int)idx * cl->slot_size;
    cl->hold             = cl->offset;
    cl->speed            = 0;
    cl->scroll_to_active = false;
    gui_fb_change();
}

bool gui_card_list_is_scrolling(gui_card_list_t *cl)
{
    GUI_ASSERT(cl != NULL);
    return cl->is_scrolling != 0;
}
