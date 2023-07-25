
#include <gui_obj.h>
#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_curtain.h>
#include "gui_canvas.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "rtk_gui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_grid.h"
#include <gui_dynamic_img.h>
#include "tp_algo.h"
#include <math.h>
#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>
#include "acc_engine.h"
#include "gui_canvas.h"
#include "nanovg_agge.h"

/*static void svg_to_nanovg(const char *svg)
{
    char * a = strstr(svg, "M");
    if (a)
    {
        char *b = strstr(a,",");
        char c[10] = {0};
        memcpy(c, a+1, b-a);
        float d = atof(c);
        atof(c);
    }
}*/
/*void static sport_chart_draw(gui_canvas_t *c)
{svg_to_nanovg("M27.8,-52.3C38.7,-41.9,51.9,-39.7,58.6,-32.4C65.3,-25,65.5,-12.5,68.4,1.7C71.3,15.9,76.9,31.7,71.5,41.3C66,50.9,49.6,54.1,35.8,61.9C22.1,69.6,11,81.7,-1,83.5C-13.1,85.3,-26.2,76.8,-36,66.7C-45.7,56.7,-52,45.1,-57.8,33.8C-63.7,22.4,-69.1,11.2,-73.1,-2.3C-77.1,-15.8,-79.7,-31.7,-74.8,-44.7C-69.8,-57.7,-57.4,-67.8,-43.7,-76.6C-30,-85.3,-15,-92.6,-3.3,-87C8.5,-81.3,17,-62.7,27.8,-52.3Z");

    {
        GUI_RENDER_DATA
        NVGcontext *vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                                       (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);
        nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);
        nvgSave(vg);
        nvgBeginPath(vg);
        //nvgCircle(vg, center_x,center_y, radius);
        //nvgRect(vg, 20,20,w-40,h-40);
        //nvgRoundedRect(vg, 20,20,100,50, 25);
        nvgLineCap(vg, NVG_ROUND);
         nvgScale(vg, 2,2);
        nvgTranslate(vg, 100,100);

        nvgMoveTo(vg, 28.7,-52);
        nvgBezierTo(vg, 38.2,-44.2,47.6,-38.6,51.5,-30.3);
        nvgBezierTo(vg, 55.3,-22,53.5,-11,57.9,2.6);
        nvgBezierTo(vg, 62.3,16.1,73,32.2,69.9,41.9);
        nvgBezierTo(vg, 66.9,51.6,50.2,54.8,36.3,61);
        nvgBezierTo(vg, 22.4,67.2,11.2,76.3,1.2,74.1);
       nvgBezierTo(vg, -8.7,71.9,-17.4,58.6,-31.1,52.2);
        nvgBezierTo(vg, -44.8,45.9,-63.5,46.7,-72.5,39.2);
        nvgBezierTo(vg, -81.5,31.6,-80.9,15.8,-81.3,-0.2);
        nvgBezierTo(vg, -81.6,-16.3,-83.1,-32.5,-77.4,-45.8);
        nvgBezierTo(vg, -71.6,-59,-58.8,-69.2,-44.7,-74.3);
                nvgBezierTo(vg, -30.6,-79.4,-15.3,-79.3,-2.8,-74.4);
        nvgBezierTo(vg, 9.6,-69.5,19.2,-59.7,28.7,-52);
        NVGpaint bg = nvgLinearGradient(vg,-100,-100,100,100, nvgRGBA(0, 255, 66,
                                   255),
                           nvgRGBA(0, 255, 66,
                                   0));
        nvgFillPaint(vg, bg);
        nvgFill(vg);
        nvgRestore(vg);
        nvgEndFrame(vg);

        nvgDeleteAGGE(vg);
    }

}*/
/**
 * @brief
 *
 * @param parent
 <svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M27.8,-52.3C38.7,-41.9,51.9,-39.7,58.6,-32.4C65.3,-25,65.5,-12.5,68.4,1.7C71.3,15.9,76.9,31.7,71.5,41.3C66,50.9,49.6,54.1,35.8,61.9C22.1,69.6,11,81.7,-1,83.5C-13.1,85.3,-26.2,76.8,-36,66.7C-45.7,56.7,-52,45.1,-57.8,33.8C-63.7,22.4,-69.1,11.2,-73.1,-2.3C-77.1,-15.8,-79.7,-31.7,-74.8,-44.7C-69.8,-57.7,-57.4,-67.8,-43.7,-76.6C-30,-85.3,-15,-92.6,-3.3,-87C8.5,-81.3,17,-62.7,27.8,-52.3Z" transform="translate(100 100)" />
</svg>
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M40.6,-72.3C48.2,-65.9,46.8,-46.1,48.9,-31.8C51.1,-17.5,56.6,-8.8,61.7,2.9C66.7,14.6,71.2,29.2,67.1,40C63,50.9,50.3,58,37.7,59.6C25.1,61.2,12.5,57.2,-0.5,58C-13.5,58.7,-26.9,64.3,-34.7,60C-42.6,55.7,-44.8,41.4,-46.8,29.7C-48.8,18.1,-50.7,9,-56.2,-3.2C-61.6,-15.4,-70.8,-30.7,-67.3,-40C-63.9,-49.3,-48,-52.4,-34.7,-55.5C-21.4,-58.5,-10.7,-61.5,2.9,-66.5C16.5,-71.6,33,-78.7,40.6,-72.3Z" transform="translate(100 100)" />
</svg>
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M41.5,-67.3C54.1,-64.6,64.9,-54.2,66.1,-41.6C67.2,-29.1,58.8,-14.6,59.5,0.5C60.3,15.5,70.3,30.9,68.9,42.9C67.4,54.8,54.4,63.2,41,66C27.6,68.7,13.8,65.9,0,65.9C-13.9,66,-27.7,69.1,-38.6,64.8C-49.4,60.6,-57.2,49,-64.7,37C-72.2,25,-79.4,12.5,-81.3,-1.1C-83.1,-14.7,-79.7,-29.3,-70.2,-37.9C-60.7,-46.4,-45.1,-48.8,-32.4,-51.5C-19.7,-54.2,-9.9,-57.1,2.3,-61C14.4,-65,28.9,-70,41.5,-67.3Z" transform="translate(100 100)" />
</svg>
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M40.8,-67.3C52,-64.2,59.5,-51.4,66.1,-38.6C72.6,-25.8,78.1,-12.9,76.6,-0.9C75,11.1,66.4,22.3,60.3,35.8C54.2,49.3,50.5,65.2,40.9,70.5C31.2,75.8,15.6,70.5,3,65.4C-9.7,60.3,-19.4,55.3,-27.4,49.1C-35.5,42.9,-41.9,35.4,-49.4,27C-56.8,18.6,-65.3,9.3,-65.3,0C-65.3,-9.3,-56.7,-18.5,-51.4,-30.7C-46.1,-42.9,-44.2,-57.9,-36.1,-62.9C-28.1,-67.9,-14.1,-62.7,0.4,-63.4C14.8,-64,29.6,-70.4,40.8,-67.3Z" transform="translate(100 100)" />
</svg>
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M37.9,-62.6C52.1,-57.5,68.5,-53.3,73.6,-43C78.6,-32.7,72.3,-16.4,71,-0.7C69.7,14.9,73.6,29.8,69.3,41.4C65,53,52.6,61.4,39.7,66.9C26.9,72.3,13.4,74.9,1.9,71.6C-9.6,68.3,-19.3,59.2,-32.9,54.2C-46.6,49.2,-64.3,48.2,-68.5,39.9C-72.7,31.5,-63.4,15.8,-58.5,2.8C-53.5,-10.1,-53,-20.2,-50.5,-31.5C-48.1,-42.9,-43.7,-55.5,-34.9,-63.7C-26.2,-71.9,-13.1,-75.8,-0.6,-74.7C11.9,-73.7,23.8,-67.7,37.9,-62.6Z" transform="translate(100 100)" />
</svg>
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M39.5,-70.8C51.9,-61.3,62.9,-52,67.8,-40.2C72.6,-28.5,71.2,-14.2,70.3,-0.5C69.4,13.2,69,26.4,63.8,37.6C58.6,48.7,48.7,57.9,37.3,64.3C25.8,70.7,12.9,74.4,-1.4,76.9C-15.8,79.3,-31.5,80.6,-42.5,73.9C-53.4,67.3,-59.5,52.6,-63.5,38.9C-67.5,25.3,-69.3,12.6,-70.5,-0.7C-71.6,-14,-72.1,-27.9,-66.1,-38.1C-60.1,-48.2,-47.6,-54.6,-35.6,-64.1C-23.5,-73.7,-11.7,-86.4,0.9,-88C13.6,-89.7,27.2,-80.2,39.5,-70.8Z" transform="translate(100 100)" />
</svg>
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M39.9,-68.9C51.8,-62.2,61.6,-51.7,64.8,-39.6C68,-27.5,64.6,-13.8,64.5,-0.1C64.4,13.6,67.7,27.3,64.8,39.9C61.9,52.6,52.8,64.2,41,68C29.2,71.9,14.6,67.9,-0.8,69.4C-16.3,70.8,-32.5,77.7,-42.8,72.9C-53,68.1,-57.2,51.8,-57.9,37.7C-58.7,23.6,-55.9,11.8,-57.5,-0.9C-59.1,-13.6,-65,-27.3,-61.7,-36.9C-58.3,-46.4,-45.8,-52,-33.9,-58.7C-22.1,-65.5,-11.1,-73.5,1.5,-76C14,-78.5,28,-75.6,39.9,-68.9Z" transform="translate(100 100)" />
</svg>
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M33,-63.2C42.1,-51.8,48.6,-41.8,55.4,-31.5C62.3,-21.2,69.5,-10.6,72.4,1.6C75.2,13.9,73.7,27.7,67.8,39.7C61.9,51.7,51.6,61.7,39.6,71.2C27.6,80.7,13.8,89.7,0.6,88.7C-12.6,87.6,-25.2,76.6,-33.3,64.8C-41.4,53,-45,40.6,-50.5,29.6C-56.1,18.7,-63.5,9.4,-69,-3.2C-74.5,-15.7,-77.9,-31.3,-72,-41.6C-66.1,-51.8,-50.8,-56.7,-37.2,-65.5C-23.7,-74.3,-11.8,-87.2,0,-87.2C11.9,-87.3,23.8,-74.5,33,-63.2Z" transform="translate(100 100)" />
</svg>
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M41.1,-64.8C54.5,-63.5,67.4,-54.9,74.5,-42.9C81.6,-30.9,83,-15.5,79.4,-2C75.9,11.4,67.6,22.8,59,32.4C50.5,41.9,41.8,49.7,32,54.7C22.1,59.7,11.1,62.1,-0.3,62.6C-11.6,63,-23.2,61.7,-37.3,59.1C-51.4,56.5,-68,52.7,-75,42.6C-82.1,32.5,-79.6,16.3,-75.9,2.1C-72.3,-12.1,-67.6,-24.2,-59,-31.6C-50.4,-39,-38,-41.6,-27.5,-44.7C-17.1,-47.7,-8.5,-51.1,2.7,-55.7C13.9,-60.3,27.7,-66.2,41.1,-64.8Z" transform="translate(100 100)" />
</svg>
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M32.9,-61.6C42.6,-51.3,50.4,-42.5,55.6,-32.4C60.7,-22.4,63.2,-11.2,67.3,2.4C71.5,16,77.3,32,71.4,40.8C65.5,49.6,48,51.2,34.2,53.5C20.4,55.8,10.2,58.8,0.2,58.5C-9.8,58.1,-19.6,54.6,-31,50.9C-42.4,47.1,-55.3,43.3,-64.2,34.8C-73.1,26.3,-78.1,13.1,-78.9,-0.5C-79.7,-14.1,-76.5,-28.2,-67.3,-36.2C-58.1,-44.2,-42.9,-46.2,-30.7,-55C-18.5,-63.9,-9.3,-79.7,1.2,-81.7C11.6,-83.7,23.2,-71.9,32.9,-61.6Z" transform="translate(100 100)" />
</svg>
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <path fill="#FF0066" d="M30.6,-51.7C44,-45.3,62.2,-45.9,73.5,-38.4C84.8,-30.9,89.2,-15.5,84.5,-2.7C79.8,10,65.9,20,56.5,30.9C47.2,41.7,42.4,53.4,33.7,64.2C25.1,74.9,12.5,84.6,-0.4,85.3C-13.4,86,-26.7,77.7,-38.5,68.8C-50.4,59.9,-60.7,50.4,-63.5,38.9C-66.2,27.3,-61.5,13.7,-58.5,1.7C-55.6,-10.2,-54.4,-20.5,-50.5,-30.1C-46.6,-39.7,-40,-48.7,-31.1,-57.7C-22.2,-66.6,-11.1,-75.4,-1.2,-73.3C8.6,-71.1,17.2,-58,30.6,-51.7Z" transform="translate(100 100)" />
</svg>
 */
static void sport_chart_draw(gui_canvas_t *c)
{
    GUI_RENDER_DATA
    uint32_t cx = 454 / 2;
    uint32_t cy = 454 / 2;

    //dynamic change
    {


        static float vary_anlge =   0;
        vary_anlge += 0.01f;
        float dx1 = 25.0f * (float)cos(1.5f * vary_anlge);
        float dy1 = 25.0f * (float)sin(1.5f * vary_anlge);
        float dx2 = 30.0f * (float)cos(-vary_anlge);
        float dy2 = 30.0f * (float)sin(-vary_anlge);
        float dx3 = 32.0f * (float)cos(2.0f * vary_anlge);
        float dy3 = 25.0f * (float)sin(2.0f * vary_anlge);
        float dx4 = 20.0f * (float)cos(vary_anlge);
        float dy4 = 20.0f * (float)sin(vary_anlge);
        float dx5 = 30.0f * (float)sin(3.0f * vary_anlge);
        float dy5 = 10.0f * (float)sin(3.0f * vary_anlge);

        float cx1 = 60 + dx1 - cx, cy1 = 60 + dy1 - cy;
        float cx2 = 394 + dx2 - cx, cy2 = 60 + dy2 - cy;
        float cx3 = 394 + dx3 - cx, cy3 = 384 + dy3 - cy;
        float cx4 = 70 + dx4 - cx, cy4 = 384 + dy4 - cy;
        float cx5 = 70 + 1.5f * dx5 - cx, cy5 = 227 + dy5 - cy;

        float cx6 = 68 - 0.5f * dx3 - cx, cy6 = 55 + dy3 - cy;
        float cx7 = 344 + dx1 - cx, cy7 = 55 + 0.8f * dy1 - cy;
        float cx8 = 385 + 0.77f * dx5 - cx, cy8 = 377 - dy5 - cy;
        float cx9 = 75 + dx4 - cx, cy9 = 380 + dy4 - cy;

        float cx10 = 80 + 1.5f * dx2 - cx, cy10 = 90 + dy2 - cy;
        float cx11 = 337 + dx3 - cx, cy11 = 130 + 1.5f * dy3 - cy;
        float cx12 = 350 + 0.77f * dx1 - cx, cy12 = 350 - dy1 - cy;
        float cx13 = 80 + dx5 - cx, cy13 = 366 + dy5 - cy;

        float arc_data[] =
        {
            (cx1 + cx5) / 2, (cy1 + cy5) / 2,
            cx1, cy1, (cx1 + cx2) / 2, (cy1 + cy2) / 2,
            cx2, cy2, (cx2 + cx3) / 2, (cy2 + cy3) / 2,
            cx3, cy3, (cx4 + cx3) / 2, (cy3 + cy4) / 2,
            cx4, cy4, (cx4 + cx5) / 2, (cy4 + cy5) / 2,
            cx5, cy5, (cx1 + cx5) / 2, (cy1 + cy5) / 2,
        };

        float arc_data2[] =
        {
            (cx6 + cx9) / 2, (cy6 + cy9) / 2,
            cx6, cy6, (cx6 + cx7) / 2, (cy6 + cy7) / 2,
            cx7, cy7, (cx7 + cx8) / 2, (cy7 + cy8) / 2,
            cx8, cy8, (cx8 + cx9) / 2, (cy8 + cy9) / 2,
            cx9, cy9, (cx9 + cx6) / 2, (cy9 + cy6) / 2,
        };

        float arc_data3[] =
        {
            (cx10 + cx13) / 2, (cy10 + cy13) / 2,
            cx10, cy10, cx10 * 0.4f + cx11 * 0.6f, cy10 * 0.4f + cy11 * 0.6f,
            cx11, cy11, (cx11 + cx12) / 2, (cy11 + cy12) / 2,
            cx12, cy12, (cx12 + cx13) / 2, (cy12 + cy13) / 2,
            cx13, cy13, (cx13 + cx10) / 2, (cy13 + cy10) / 2,
        };

        {
            GUI_RENDER_DATA
            NVGcontext *vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                                           (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);
            nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);
            nvgSave(vg);
            nvgBeginPath(vg);
            nvgTranslate(vg, GET_BASE(c)->dx, GET_BASE(c)->dy);
            //nvgCircle(vg, center_x,center_y, radius);
            //nvgRect(vg, 20,20,w-40,h-40);
            //nvgRoundedRect(vg, 20,20,100,50, 25);
            nvgLineCap(vg, NVG_ROUND);
            //nvgScale(vg, 2,2);
            nvgTranslate(vg, 454 / 2, 454 / 2);

            nvgMoveTo(vg, arc_data[0], arc_data[1]);
            for (size_t i = 0; i < 5; i++)
            {
                nvgQuadTo(vg, arc_data[2 + i * 4], arc_data[2 + i * 4 + 1], arc_data[2 + i * 4 + 2],
                          arc_data[2 + i * 4 + 3]);
            }
            NVGpaint bg = nvgLinearGradient(vg, -100, -100, 200, 200, nvgRGBA(0x4b, 0x5d, 0x3d,
                                                                              230),
                                            nvgRGBA(0x4b, 0x5d, 0x3d,
                                                    0));
            nvgFillPaint(vg, bg);
            nvgFill(vg);
            nvgBeginPath(vg);
            {
                nvgMoveTo(vg, arc_data2[0], arc_data2[1]);
                for (size_t i = 0; i < 4; i++)
                {
                    nvgQuadTo(vg, arc_data2[2 + i * 4], arc_data2[2 + i * 4 + 1], arc_data2[2 + i * 4 + 2],
                              arc_data2[2 + i * 4 + 3]);
                }
                NVGpaint bg = nvgLinearGradient(vg, -100, -100, 200, 200, nvgRGBA(0x41, 0x5d, 0x1E,
                                                                                  200),
                                                nvgRGBA(0, 255, 66,
                                                        0));
                nvgFillPaint(vg, bg);
                nvgFill(vg);
            }
            nvgBeginPath(vg);
            {
                nvgMoveTo(vg, arc_data3[0], arc_data3[1]);
                for (size_t i = 0; i < 4; i++)
                {
                    nvgQuadTo(vg, arc_data3[2 + i * 4], arc_data3[2 + i * 4 + 1], arc_data3[2 + i * 4 + 2],
                              arc_data3[2 + i * 4 + 3]);
                }

                NVGpaint bg = nvgLinearGradient(vg, -100, -100, 200, 200, nvgRGBA(0x41, 0x4f, 0x1E,
                                                                                  100),
                                                nvgRGBA(0x41, 0x4f, 0x1E,
                                                        0));
                nvgFillPaint(vg, bg);
                nvgFill(vg);
            }

            static float d = 0;
            d += 0.0005f;

            nvgRotate(vg, M_PI * (d));

            nvgBeginPath(vg);
            {
                nvgRoundedRect(vg, -2, -454 / 2 + 40, 4, 454 / 2 - 40, 2);
                nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
                nvgFill(vg);
            }
            nvgResetTransform(vg);
            nvgTranslate(vg, GET_BASE(c)->dx, GET_BASE(c)->dy);
            nvgTranslate(vg, 454 / 2, 454 / 2);
            nvgRotate(vg, M_PI * (d / 60.0f));

            nvgBeginPath(vg);
            {
                nvgRoundedRect(vg, 0 - 5, -454 / 2 + 100, 10, 454 / 2 - 100, 5);
                nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
                nvgFill(vg);
            }
            nvgResetTransform(vg);
            nvgTranslate(vg, GET_BASE(c)->dx, GET_BASE(c)->dy);
            nvgTranslate(vg, 454 / 2, 454 / 2);
            nvgRotate(vg, M_PI * (d / 60.0f / 60.0f));

            nvgBeginPath(vg);
            {
                nvgRoundedRect(vg, 0 - 10, -454 / 2 + 170, 20, 454 / 2 - 170, 10);
                nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
                nvgFill(vg);
            }
            nvgResetTransform(vg);
            nvgTranslate(vg, GET_BASE(c)->dx, GET_BASE(c)->dy);
            nvgTranslate(vg, 454 / 2, 454 / 2);
            nvgBeginPath(vg);
            {
                nvgCircle(vg, 0, 0, 16);
                nvgFillColor(vg, nvgRGBA(0x41, 0x5d, 0x1E, 255));
                nvgFill(vg);
            }
            nvgEndFrame(vg);

            nvgDeleteAGGE(vg);
        }
    }
}
static void sport_chart(gui_obj_t *parent)
{
    gui_canvas_t *c = gui_canvas_create(parent, "c", 0, 0, 454, 454, 0);
    c->draw = sport_chart_draw;
}
#ifdef RTK_MODULE_VG_LITE
#include "gui_vg_lite_clock.h"
#endif
void watchface(void *parent)
{
#ifdef RTK_MODULE_VG_LITE
    gui_vg_lite_clock_t *path_clock = gui_vg_lite_clock_create(parent,  "path_clock", NULL, 0, 0,
                                                               454, 454);
#else
    //gui_button_create(parent, 252, 57, 100, 100, ACTIVITY_SVG, NULL, NULL, 2, 5134);
    sport_chart(parent);
#endif
}
