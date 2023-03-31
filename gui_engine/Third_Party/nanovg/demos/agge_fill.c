#if 0
#include <string.h>
#include <stdlib.h>
#include "agge/nanovg_agge.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

//#include "fill.inc"
void drawColorwheel(NVGcontext *vg, float x, float y, float w, float h, float t)
{
    int i;
    float r0, r1, ax, ay, bx, by, cx, cy, aeps, r;
    float hue = sinf(t * 0.12f);
    NVGpaint paint;

    nvgSave(vg);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, 400, 400);
    nvgFillColor(vg, nvgRGBA(0x0, 0xff, 0x00, 0xff));
    nvgFill(vg);
    /*
        cx = x + w*0.5f;
        cy = y + h*0.5f;
        r1 = (w < h ? w : h) * 0.5f - 5.0f;
        r0 = r1 - 20.0f;
        aeps = 0.5f / r1;   // half a pixel arc length in radians (2pi cancels out).

        for (i = 0; i < 6; i++) {
            float a0 = (float)i / 6.0f * NVG_PI * 2.0f - aeps;
            float a1 = (float)(i+1.0f) / 6.0f * NVG_PI * 2.0f + aeps;
            nvgBeginPath(vg);
            nvgArc(vg, cx,cy, r0, a0, a1, NVG_CW);
            nvgArc(vg, cx,cy, r1, a1, a0, NVG_CCW);
            nvgClosePath(vg);
            ax = cx + cosf(a0) * (r0+r1)*0.5f;
            ay = cy + sinf(a0) * (r0+r1)*0.5f;
            bx = cx + cosf(a1) * (r0+r1)*0.5f;
            by = cy + sinf(a1) * (r0+r1)*0.5f;
            paint = nvgLinearGradient(vg, ax,ay, bx,by, nvgHSLA(a0/(NVG_PI*2),1.0f,0.55f,255), nvgHSLA(a1/(NVG_PI*2),1.0f,0.55f,255));
            nvgFillPaint(vg, paint);
            nvgFill(vg);
        }
        nvgBeginPath(vg);
        nvgCircle(vg, 100,100, 100);
        //nvgFillColor(vg, nvgRGBA(255,0,255,255));
        paint = nvgLinearGradient(vg, 0,100, 200,100, nvgRGBA(255,0,0,255), nvgRGBA(0,0,255,255));
      nvgFillPaint(vg, paint);
        nvgFill(vg);

      nvgBeginPath(vg);
        nvgCircle(vg, 300,100, 100);
      paint = nvgRadialGradient(vg, 300,100, 30, 100, nvgRGBA(255,0,0,255), nvgRGBA(0,0,255,255));
      nvgFillPaint(vg, paint);
        nvgFill(vg);

      nvgBeginPath(vg);
        nvgCircle(vg, 100,300, 100);
      paint = nvgBoxGradient(vg, 0, 200, 200, 200,70, 30, nvgRGBA(255,0,0,255), nvgRGBA(0,0,255,255));
      nvgFillPaint(vg, paint);
        nvgFill(vg);
      paint = nvgBoxGradient(vg, 200,200, 180,180, 15*2, 10, nvgRGBA(255,0,0,255), nvgRGBA(0,0,255,255));
        nvgBeginPath(vg);
        nvgRect(vg, 200,200, 180,180);
        nvgRoundedRect(vg, x,y, w,h, 15);
        nvgPathWinding(vg, NVG_HOLE);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
        nvgBeginPath(vg);
        nvgCircle(vg, cx,cy, r0-0.5f);
        nvgCircle(vg, cx,cy, r1+1.5f);
        nvgStrokeColor(vg, nvgRGBA(255,255,0,100));
        nvgStrokeWidth(vg, 10.0f);
        nvgStroke(vg);

        // Selector
        nvgSave(vg);
        nvgTranslate(vg, cx,cy);
        nvgRotate(vg, hue*NVG_PI*2);

        // Marker on
        nvgStrokeWidth(vg, 2.0f);
        nvgBeginPath(vg);
        nvgRect(vg, r0-1,-3,r1-r0+2,6);
        nvgStrokeColor(vg, nvgRGBA(255,255,255,192));
        nvgStroke(vg);

        paint = nvgBoxGradient(vg, r0-3,-5,r1-r0+6,10, 2,4, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
        nvgBeginPath(vg);
        nvgRect(vg, r0-2-10,-4-10,r1-r0+4+20,8+20);
        nvgRect(vg, r0-2,-4,r1-r0+4,8);
        nvgPathWinding(vg, NVG_HOLE);
        nvgFillPaint(vg, paint);
        nvgFill(vg);

        // Center triangle
        r = r0 - 6;
        ax = cosf(120.0f/180.0f*NVG_PI) * r;
        ay = sinf(120.0f/180.0f*NVG_PI) * r;
        bx = cosf(-120.0f/180.0f*NVG_PI) * r;
        by = sinf(-120.0f/180.0f*NVG_PI) * r;
        nvgBeginPath(vg);
        nvgMoveTo(vg, r,0);
        nvgLineTo(vg, ax,ay);
        nvgLineTo(vg, bx,by);
        nvgClosePath(vg);
        paint = nvgLinearGradient(vg, r,0, ax,ay, nvgHSLA(hue,1.0f,0.5f,255), nvgRGBA(255,255,255,255));
        nvgFillPaint(vg, paint);
        nvgFill(vg);
        paint = nvgLinearGradient(vg, (r+ax)*0.5f,(0+ay)*0.5f, bx,by, nvgRGBA(0,0,0,0), nvgRGBA(0,0,0,255));
        nvgFillPaint(vg, paint);
        nvgFill(vg);
        nvgStrokeColor(vg, nvgRGBA(0,0,0,64));
        nvgStroke(vg);

        // Select circle on triangle
        ax = cosf(120.0f/180.0f*NVG_PI) * r*0.3f;
        ay = sinf(120.0f/180.0f*NVG_PI) * r*0.4f;
        nvgStrokeWidth(vg, 2.0f);
        nvgBeginPath(vg);
        nvgCircle(vg, ax,ay,5);
        nvgStrokeColor(vg, nvgRGBA(255,255,255,192));
        nvgStroke(vg);

        paint = nvgRadialGradient(vg, ax,ay, 7,9, nvgRGBA(0,0,0,64), nvgRGBA(0,0,0,0));
        nvgBeginPath(vg);
        nvgRect(vg, ax-20,ay-20,40,40);
        nvgCircle(vg, ax,ay,7);
        nvgPathWinding(vg, NVG_HOLE);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    */
    nvgRestore(vg);

    nvgRestore(vg);
}
static void do_fill(NVGcontext *vg, int32_t w, int32_t h)
{
    nvgBeginFrame(vg, w, h, 1);

    drawColorwheel(vg, w - 300, h - 300, 250.0f, 250.0f, 10);
    nvgEndFrame(vg);
}
#include "guidef.h"
#include "gui_port.h"
#include "drv_sdl.h"
#include "rtthread.h"
static void run_test(int32_t w, int32_t h, int32_t BPP, const char *filename)
{
    int32_t size = w * h * BPP;
    uint8_t *data = (uint8_t *)malloc(size);
    NVGcontext *vg = nvgCreateAGGE(w, h, w * BPP, BPP == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA,
                                   data);
    uint32_t *d1 = data;
    memset(data, 0xff, size);
    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
        {
            *(d1 + i * w + j) = 0x33ffffff;
        }
    }
    nvgBeginFrame(vg, w, h, 1);
    nvgSave(vg);
    nvgBeginPath(vg);
    //nvgCircle(vg, center_x,center_y, radius);
    nvgRect(vg, 20, 20, w - 40, h - 40);
    nvgFillColor(vg, nvgRGBA(0xff, 0xff, 0x00, 0x66));
    nvgFill(vg);
    nvgRestore(vg);
    nvgEndFrame(vg);

    nvgDeleteAGGE(vg);
    /*
     static struct rt_device_graphic_info info;
    rt_device_t dev = rt_device_find("sdl");
       rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
       rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);
      for (uint32_t i = 0; i < 400; i++)
       {
           int write_off = (i - 0) * 720 ;

           int read_off = 0;

           uint16_t *pixel;

           for (uint32_t j = 0; j < 400; j++)
           {
               pixel = ((uint16_t *)data + j +i*400);

               {
               //printf("pixel[0]<<11:%x,pixel[1]<<5:%x,pixel[2]:%x   ,", pixel[0] ,pixel[1] , pixel[2]);
               //printf("pixel[0]<<11+pixel[1]<<5+pixel[2]:%x\n", pixel[0] << 11 + pixel[1] << 5 + pixel[2]);
                   ((uint16_t *)(info.framebuffer))[(write_off + j) ] = pixel;//R
               }
           }
       }
           struct rt_device_rect_info rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = 720;
    rect.height = 480;
       rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, &rect);*/
    if (filename != NULL)
    {
        stbi_write_png(filename, w, h, BPP, data, 0);
    }


    free(data);
}

int main1()
{
    run_test(400, 400, 4, "result32.png");
    //run_test(400, 400, 2, "result16.png");

    return 0;
}
#endif