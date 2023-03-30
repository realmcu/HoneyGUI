#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>
#include "acc_engine.h"

#ifdef MODULE_VG_LITE
#elif defined (MODULE_RTK_PPE)
#include "rtl876x_rcc.h"
#else
#endif

extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect);
extern void sw_draw_circle(canvas_circle_t *c, struct gui_dispdev *dc);
extern void sw_draw_rectangle(canvas_rectangle_t *r, struct gui_dispdev *dc);
extern void sw_draw_arc(canvas_arc_t *a, struct gui_dispdev *dc);
extern void sw_draw_line(canvas_line_t *l, struct gui_dispdev *dc);
extern void sw_draw_polyline(canvas_polyline_t *p, struct gui_dispdev *dc);
extern void sw_draw_svg(void *svg, uint32_t data_length, struct gui_dispdev *dc, int x, int y,
                        float scale);
extern void sw_draw_path(draw_path_t *path, struct gui_dispdev *dc);

extern void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect);
extern void hw_draw_arc(canvas_arc_t *a, struct gui_dispdev *dc);
extern void hw_draw_line(canvas_line_t *l, struct gui_dispdev *dc);
extern void hw_draw_rectangle(canvas_rectangle_t *r, struct gui_dispdev *dc);
extern void hw_draw_circle(canvas_circle_t *circle, struct gui_dispdev *dc);
extern void hw_acc_draw_svg(void *svg, uint32_t data_length, struct gui_dispdev *dc, int x, int y,
                            float scale);

static struct acc_engine acc = {0};

void gui_acc_init(void)
{
#ifdef MODULE_VG_LITE
    acc.blit = hw_acc_blit;
    acc.draw_circle = hw_draw_circle;
    acc.draw_rectangle = hw_draw_rectangle;
    //acc.draw_arc = sw_draw_arc;
    acc.draw_arc = hw_draw_arc;
    acc.draw_line = hw_draw_line;
    acc.draw_path = sw_draw_path;
    acc.draw_polyline = sw_draw_polyline;
    acc.draw_svg = hw_acc_draw_svg;
#elif defined (MODULE_RTK_PPE)
    RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
    acc.blit = hw_acc_blit;
    acc.draw_circle = sw_draw_circle;
    acc.draw_rectangle = sw_draw_rectangle;
    acc.draw_arc = sw_draw_arc;
    acc.draw_line = sw_draw_line;
    acc.draw_path = sw_draw_path;
    acc.draw_polyline = sw_draw_polyline;
    acc.draw_svg = sw_draw_svg;
#else
    acc.blit = sw_acc_blit;
    acc.draw_circle = sw_draw_circle;
    acc.draw_rectangle = sw_draw_rectangle;
    acc.draw_arc = sw_draw_arc;
    acc.draw_line = sw_draw_line;
    acc.draw_path = sw_draw_path;
    acc.draw_polyline = sw_draw_polyline;
    acc.draw_svg = sw_draw_svg;
#endif
}



struct acc_engine *gui_get_acc(void)
{
    return &acc;
}

