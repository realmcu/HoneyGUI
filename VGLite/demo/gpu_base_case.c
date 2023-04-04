
#include "rtthread.h"
#include "vg_lite.h"
#include "vg_lite_platform.h"
#include "vg_lite.h"
#include "tiger_paths.h"
#include <rtdevice.h>

static int zoomOut    = 0;
static int scaleCount = 0;
static vg_lite_matrix_t matrix;

//static vg_lite_buffer_t image;
static vg_lite_buffer_t target;

void animateTiger()
{
    if (zoomOut)
    {
        vg_lite_scale(1.25, 1.25, &matrix);
        if (0 == --scaleCount)
        {
            zoomOut = 0;
        }
    }
    else
    {
        vg_lite_scale(0.8, 0.8, &matrix);
        if (5 == ++scaleCount)
        {
            zoomOut = 1;
        }
    }

    vg_lite_rotate(5, &matrix);
}

static void cleanup(void)
{
    uint8_t i;
    for (i = 0; i < pathCount; i++)
    {
        vg_lite_clear_path(&path[i]);
    }

    vg_lite_close();
}
#if 0
static void redraw()
{
    static struct rt_device_graphic_info info;
    rt_device_t dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);


    rt_memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    target.width  = info.width;
    target.height = info.height;
    target.format = VG_LITE_BGRA8888;
    target.memory = (void *)info.framebuffer;
    target.address = (uint32_t)info.framebuffer;
    target.stride = target.width * 4;
    target.tiled = VG_LITE_LINEAR;

    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint8_t count;

    // Draw the path using the matrix.
    vg_lite_clear(&target, NULL, 0xFF00FF00);
    for (count = 0; count < pathCount; count++)
    {
        //rt_kprintf("line = %d; count = %d\n", __LINE__, count);
        error = vg_lite_draw(&target, &path[count], VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE,
                             color_data[count]);
        if (error)
        {
            rt_kprintf("vg_lite_draw() returned error %d\n", error);
            cleanup();
            return;
        }
    }
    vg_lite_finish();


    rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
    rt_device_close(dev);

    animateTiger();

    return;
}
#else
static char path_data[] =
{
    2, -5, -10, // moveto   -5,-10
    4, 5, -10,  // lineto    5,-10
    4, 10, -5,  // lineto   10, -5
    4, 0, 0,    // lineto    0,  0
    4, 10, 5,   // lineto   10,  5
    4, 5, 10,   // lineto    5, 10
    4, -5, 10,  // lineto   -5, 10
    4, -10, 5,  // lineto  -10,  5
    4, -10, -5, // lineto  -10, -5
    0, // end
};

static vg_lite_path_t test_path =
{
    {
        -10, -10, // left,top
        10, 10
    }, // right,bottom
    VG_LITE_HIGH, // quality
    VG_LITE_S8, // -128 to 127 coordinate range
    {0}, // uploaded
    sizeof(path_data), // path length
    path_data, // path data
    1
};
static void redraw()
{

    static struct rt_device_graphic_info info;
    rt_device_t dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);


    rt_memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    target.width  = info.width;
    target.height = info.height;
    target.format = VG_LITE_RGBA8888;
    target.memory = (void *)info.framebuffer;
    target.address = (uint32_t)info.framebuffer;
    target.stride = target.width * 4;
    target.tiled = VG_LITE_LINEAR;
    //target.tiled = VG_LITE_TILED;

    // Draw the path using the matrix.
    vg_lite_clear(&target, NULL, 0xFFFF0000);

    //vg_lite_rectangle_t rec = {0, 0, 454/2, 454/2};
    //vg_lite_clear(&target, &rec, 0xFF0000FF);


    vg_lite_identity(&matrix);
    vg_lite_translate(target.width / 2, target.height / 2, &matrix);
    vg_lite_scale(5, 5, &matrix);

    //vg_lite_blit(&target, &image, &matrix, VG_LITE_BLEND_SCREEN, 0, VG_LITE_FILTER_POINT);
    vg_lite_draw(&target, &test_path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFF0000FF);

    vg_lite_finish();

    rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
    rt_device_close(dev);

    return;
}
#endif



static void rt_gpu_demo_entry(void *parameter)
{
    static struct rt_device_graphic_info info;
    rt_device_t dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);
    rt_device_close(dev);


    int fb_width = info.width;
    int fb_height = info.height;
    // Setup a scale at center of buffer.
    vg_lite_identity(&matrix);
//    vg_lite_translate(fb_width / 2 - 20 * fb_width / 640.0f, fb_height / 2 - 100 * fb_height / 480.0f,
//                      &matrix);
    vg_lite_translate(fb_width / 2 - 20, fb_height / 2 - 100, &matrix);
    vg_lite_scale(2, 2, &matrix);
    //vg_lite_scale(fb_width / 640.0f, fb_height / 480.0f, &matrix);
    uint32_t startTime = 0;
    uint32_t tick = 0;
    uint32_t n = 0;

    //rt_memset(&image, 0x00, sizeof(vg_lite_buffer_t));
    uint8_t *image_buffer = RT_NULL;
    //vg_lite_load_png(&image, "/png/tiger_blue.png");

    while (1)
    {
        //rt_kprintf("GPU Task! \n");
        redraw();
        //rt_thread_mdelay(1000);
        time_t now;

        now = time(RT_NULL);
        n++;
        if (n >= 60)
        {
            tick = rt_tick_get() - startTime;
            //rt_kprintf("%d frames in %d seconds: %d fps\r\n", n, time / 1000, n * 1000 / time);
            n         = 0;
            startTime = rt_tick_get();
            rt_kprintf("Time: %.*s\n", 25, ctime(&now));

        }
    }
}

void do_gpu(void)
{
    redraw();
}
MSH_CMD_EXPORT(do_gpu, gpu test test);


int rt_gpu_demo_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("gpu_demo",
                           rt_gpu_demo_entry, RT_NULL,
                           4096 * 4, 25, 10);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}
//INIT_APP_EXPORT(rt_gpu_demo_init);

static void ram_gpu_test_entry(void *parameter)
{
    static struct rt_device_graphic_info info;

    rt_device_t dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);
    rt_memset(&target, 0x00, sizeof(vg_lite_buffer_t));

    target.width  = info.width;
    target.height = info.height;
    target.format = VG_LITE_RGBA8888;
    target.memory = (void *)info.framebuffer;
    target.address = (uint32_t)info.framebuffer;
    target.stride = target.width * 4;
    target.tiled = VG_LITE_LINEAR;
    rt_device_close(dev);

    vg_lite_identity(&matrix);

    uint8_t *buffer = rt_malloc((info.height * info.width * 4) + 63);
    buffer = buffer + 64 - (int)buffer % 64;
    vg_lite_buffer_t source;
    source.width  = info.width;
    source.height = info.height;
    source.format = VG_LITE_RGBA8888;
    source.memory = (void *)buffer;
    source.address = (uint32_t)buffer;
    source.stride = source.width * 4;
    source.tiled = VG_LITE_LINEAR;
    vg_lite_clear(&source, NULL, 0xFF00FF00);



    uint32_t startTime = 0;
    uint32_t time = 0;
    uint32_t n = 0;
    uint32_t cnt = 0;
    while (1)
    {
        cnt++;
        vg_lite_clear(&target, NULL, 0x0FFF0000 + cnt);
        //vg_lite_clear(&target, NULL, 0x0FFF00FF + cnt);

        //vg_lite_blit(&target, &target, &matrix, VG_LITE_BLEND_SRC_OVER, 0, VG_LITE_FILL_NON_ZERO);
        vg_lite_blit(&target, &source, &matrix, VG_LITE_BLEND_NONE, 0, VG_LITE_FILL_NON_ZERO);
        vg_lite_finish();


//        rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
//        rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
//        rt_device_close(dev);

        n++;
        if (n >= 60)
        {
            time = rt_tick_get() - startTime;
            rt_kprintf("%d frames in %d seconds: %d fps\r\n", n, time / 1000, n * 1000 / time);
            n = 0;
            startTime = rt_tick_get();


            //rt_kprintf("pixel time = %ldns \n", time * 1000000 /(60*454*454));
            rt_kprintf("pixel time = %ldns \n", time * 16667 / (480 * 480));
        }
    }
}

int ram_gpu_test(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("gpu_demo",
                           ram_gpu_test_entry, RT_NULL,
                           4096 * 4, 25, 10);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}

MSH_CMD_EXPORT(ram_gpu_test, lcd display test);
