/**
 * @file hml_image.c
 * @brief HML Image Component (hg_image, hg_img)
 *
 * Attribute mapping follows the designer's ImageGenerator.
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "../hml_json.h"
#include "gui_img.h"
#include "gui_api.h"
#include <stdio.h>
#include <string.h>

typedef struct
{
    const char *name;
    BLEND_MODE_TYPE mode;
} hml_blend_map_t;

static const hml_blend_map_t hml_blend_map[] =
{
    { "IMG_BYPASS_MODE",         IMG_BYPASS_MODE },
    { "IMG_FILTER_BLACK",        IMG_FILTER_BLACK },
    { "IMG_SRC_OVER_MODE",       IMG_SRC_OVER_MODE },
    { "IMG_COVER_MODE",          IMG_COVER_MODE },
    { "IMG_RECT",                IMG_RECT },
    { "IMG_2D_SW_RGB565_ONLY",   IMG_2D_SW_RGB565_ONLY },
    { "IMG_2D_SW_SRC_OVER_MODE", IMG_2D_SW_SRC_OVER_MODE },
    { "IMG_2D_SW_FIX_A8_FG",     IMG_2D_SW_FIX_A8_FG },
    { "IMG_2D_SW_FIX_A8_BGFG",   IMG_2D_SW_FIX_A8_BGFG },
    { "IMG_PLUS_DARKER",         IMG_PLUS_DARKER },
    { "IMG_A8_BLUR",             IMG_A8_BLUR },
    { "IMG_SRC_MODE",            IMG_SRC_MODE },
};

static bool hml_parse_blend(const char *name, BLEND_MODE_TYPE *mode)
{
    for (size_t i = 0; name && i < sizeof(hml_blend_map) / sizeof(hml_blend_map[0]); i++)
    {
        if (strcmp(hml_blend_map[i].name, name) == 0)
        {
            *mode = hml_blend_map[i].mode;
            return true;
        }
    }
    return false;
}

static void hml_image_transform(gui_img_t *img, ezxml_t node, int w, int h)
{
    hml_json_t *tf = hml_json_parse(ezxml_attr(node, "transform"));

    float rotation = hml_json_float(tf, "rotation", 0.0f);
    float tx = hml_json_float(tf, "translateX", 0.0f);
    float ty = hml_json_float(tf, "translateY", 0.0f);
    bool has_focus = hml_json_has(tf, "focusX") || hml_json_has(tf, "focusY");
    bool has_rotation = rotation != 0.0f;

    // Display size different from the converted image: scale to fit
    float sx = hml_json_float(tf, "scaleX", 1.0f);
    float sy = hml_json_float(tf, "scaleY", 1.0f);
    bool explicit_scale = sx != 1.0f || sy != 1.0f;
    float auto_sx = 1.0f, auto_sy = 1.0f;
    bool auto_scale = false;
    if (w > 0 && h > 0)
    {
        uint16_t iw = gui_img_get_width(img), ih = gui_img_get_height(img);
        if (iw > 0 && ih > 0 && (iw != w || ih != h))
        {
            auto_sx = (float)w / iw;
            auto_sy = (float)h / ih;
            auto_scale = true;
        }
    }
    bool need_scale = explicit_scale || auto_scale;

    if (has_rotation || need_scale || has_focus)
    {
        float cx = (float)gui_img_get_width(img) / 2.0f;
        float cy = (float)gui_img_get_height(img) / 2.0f;
        if (has_focus)
        {
            float fx = hml_json_float(tf, "focusX", 0.0f);
            float fy = hml_json_float(tf, "focusY", 0.0f);
            gui_img_translate(img, fx + tx, fy + ty);
            gui_img_set_focus(img, fx, fy);
        }
        else if (has_rotation)
        {
            gui_img_translate(img, cx + tx, cy + ty);
            gui_img_set_focus(img, cx, cy);
        }
        else if (tx != 0.0f || ty != 0.0f)
        {
            gui_img_translate(img, tx, ty);
        }
        if (has_rotation)
        {
            gui_img_rotation(img, rotation);
        }
        if (need_scale)
        {
            float fsx = hml_json_has(tf, "scaleX") ? sx : auto_sx;
            float fsy = hml_json_has(tf, "scaleY") ? sy : auto_sy;
            if (fsx != 1.0f || fsy != 1.0f) { gui_img_scale(img, fsx, fsy); }
        }
    }
    else if (tx != 0.0f || ty != 0.0f)
    {
        gui_img_translate(img, tx, ty);
    }

    int opacity = hml_json_int(tf, "opacity", 255);
    if (opacity != 255)
    {
        gui_img_set_opacity(img, (unsigned char)(opacity < 0 ? 0 : opacity));
    }

    hml_json_free(tf);
}

gui_obj_t *hml_create_image(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "img");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 0);
    int h = hml_attr_height(node, 0);
    const char *src = ezxml_attr(node, "src");

    if (!src)
    {
        gui_log("[HML] Image %s: missing 'src'\n", id);
        return NULL;
    }

    char bin_path[160];
    hml_resolve_asset_path(src, bin_path, sizeof(bin_path));

    // gui_img_create_from_fs copies the path
    gui_img_t *img = gui_img_create_from_fs(parent, id, (void *)bin_path, x, y, w, h);
    if (!img) { return NULL; }

    BLEND_MODE_TYPE mode;
    const char *blend = ezxml_attr(node, "blendMode");
    if (blend && strcmp(blend, "IMG_FILTER_BLACK") != 0 && hml_parse_blend(blend, &mode))
    {
        gui_img_set_mode(img, mode);
        if (mode == IMG_2D_SW_FIX_A8_FG || mode == IMG_2D_SW_FIX_A8_BGFG)
        {
            gui_img_a8_recolor(img, hml_parse_argb(ezxml_attr(node, "fgColor"), 0xFFFFFFFF));
            if (mode == IMG_2D_SW_FIX_A8_BGFG)
            {
                gui_img_a8_fix_bg(img, hml_parse_argb(ezxml_attr(node, "bgColor"), 0xFFFFFFFF));
            }
        }
    }

    if (hml_attr_bool(node, "highQuality", false))
    {
        gui_img_set_quality(img, true);
    }
    if (hml_attr_bool(node, "needClip", false))
    {
        img->need_clip = true;
    }

    hml_image_transform(img, node, w, h);

    return (gui_obj_t *)img;
}
