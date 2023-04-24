#include <stdio.h>
#include <string.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include "Elm.h"
#include "rtthread.h"
static struct rt_device_graphic_info info;
static ElmBuffer buffer;
static void cleanup(void)
{
    ElmDestroyBuffer(buffer);
}
static ELM_BUFFER_FORMAT _buffer_format_to_Elm(vg_lite_buffer_format_t format)
{
    switch (format)
    {
    case VG_LITE_RGB565:
        return ELM_BUFFER_FORMAT_RGB565;
        break;
    case VG_LITE_BGR565:
        return ELM_BUFFER_FORMAT_BGR565;
        break;
    default:
        return ELM_BUFFER_FORMAT_RGBA8888;
        break;
    }
}
#define PPEM_SIZE 50
static void build_path(float *dst, float *src, size_t size)
{
    float *dst_path = dst;
    float *src_path = src;
    uint32_t count = size / 4;
    uint32_t iter = 0;
    uint8_t *operation;
    uint8_t op;
    for (iter = 0; iter < count;)
    {
        op = (uint8_t) * src_path;
        *dst_path = *src_path++;
        operation = (uint8_t *)dst_path++;
        *operation = op;
        iter ++;
        switch (op)
        {
        case VLC_OP_END:
            break;
        case VLC_OP_QUAD:
            *dst_path ++ = *src_path++;
            *dst_path ++ = *src_path++;
            *dst_path ++ = *src_path++;
            *dst_path ++ = *src_path++;
            iter += 4;
            break;
        case VLC_OP_MOVE:
        case VLC_OP_LINE:
            *dst_path ++ = *src_path++;
            *dst_path ++ = *src_path++;
            iter += 2;
            break;
        default:
            break;
        }
    }
    return;
}
#define DEFAULT_SIZE   60;
static void vladimir_font(void)
{
#if 1
    int   fb_width = 320, fb_height = 320;
    float fb_scale = 1.0f;

#include "vladimir_font.h"
    vg_lite_matrix_t matrix;
    float *path_data;
    float offsetX = 0;
    float startX = 5.0f;
    float startY = 100;

    char string[] = "BCDEF!";
    char character = 'H';
    int iter = 0;
    float lineHeight = 1.0f * PPEM_SIZE;
    font_path_header fontData;
    rt_device_t dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);
    rt_device_close(dev);
    //int err = vg_lite_init(256, 256);
    //rt_kprintf("err= %d\n",err);
    fb_scale = (float)fb_width / DEFAULT_SIZE;
    vg_lite_buffer_t target;
    target.width  = 390;
    target.height = 390;
    target.format = VG_LITE_RGBA8888;
    target.memory = (void *)info.framebuffer;
    target.address = (uint32_t)info.framebuffer;
    target.stride = target.width * 4;
    target.tiled = VG_LITE_LINEAR;
    vg_lite_clear(&target, NULL, 0xFF1D9BCD);
    vg_lite_path_t path;
    vg_lite_identity(&matrix);
    /* Translate the matrix to the center of the buffer.*/
    vg_lite_translate(startX, startY - 50, &matrix);
    /* Scale up matrix with fb_scale times.*/
    vg_lite_scale(fb_scale, fb_scale, &matrix);
    for (iter = 0; string[iter] != '\0'; iter ++)
    {
        rt_kprintf("iter= %d\n", iter);
        fontData = fontHeader[string[iter]];
        if (string[iter] == '\n')
        {
            vg_lite_translate(-offsetX, lineHeight, &matrix);
            offsetX = 0;
            continue;
        }

        memset(&path, 0, sizeof(path));
        path.format = (vg_lite_format_t)fontData.format;
        path_data = rt_malloc(fontData.pathSize);

        memcpy(path.bounding_box, fontData.boundingBox, 16);
        build_path(path_data, fontData.path_data, fontData.pathSize);

        path.path = path_data;
        path.path_changed = 1;
        path.path_length = fontData.pathSize;
        path.quality = VG_LITE_HIGH;

        /* Draw the path using the matrix.*/
        vg_lite_draw(&target, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFFccff33);

        rt_free(path_data);
        vg_lite_finish();
        vg_lite_clear_path(&path);

        vg_lite_translate(fontData.dx, fontData.dy, &matrix);
        offsetX += fontData.dx;
    }
    vg_lite_identity(&matrix);
    /* Translate the matrix to the center of the buffer.*/
    vg_lite_translate(startX, startY, &matrix);
    /* Scale up matrix with fb_scale times.*/
    vg_lite_scale(fb_scale, fb_scale, &matrix);

    for (iter = 0; string[iter] != '\0'; iter ++)
    {
        rt_kprintf("iter= %d\n", iter);
        fontData = fontHeader[string[iter]];
        if (string[iter] == '\n')
        {
            vg_lite_translate(-offsetX, lineHeight, &matrix);
            offsetX = 0;
            continue;
        }

        memset(&path, 0, sizeof(path));
        path.format = (vg_lite_format_t)fontData.format;
        path_data = rt_malloc(fontData.pathSize);

        memcpy(path.bounding_box, fontData.boundingBox, 16);
        build_path(path_data, fontData.path_data, fontData.pathSize);

        path.path = path_data;
        path.path_changed = 1;
        path.path_length = fontData.pathSize;
        path.quality = VG_LITE_HIGH;

        /* Draw the path using the matrix.*/
        vg_lite_draw(&target, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFFcc66ff);

        rt_free(path_data);
        vg_lite_finish();
        vg_lite_clear_path(&path);

        vg_lite_translate(fontData.dx, fontData.dy, &matrix);
        offsetX += fontData.dx;
    }

    vg_lite_identity(&matrix);
    /* Translate the matrix to the center of the buffer.*/
    vg_lite_translate(startX, startY + 50, &matrix);
    /* Scale up matrix with fb_scale times.*/
    vg_lite_scale(fb_scale, fb_scale, &matrix);
    for (iter = 0; string[iter] != '\0'; iter ++)
    {
        rt_kprintf("iter= %d\n", iter);
        fontData = fontHeader[string[iter]];
        if (string[iter] == '\n')
        {
            vg_lite_translate(-offsetX, lineHeight, &matrix);
            offsetX = 0;
            continue;
        }

        memset(&path, 0, sizeof(path));
        path.format = (vg_lite_format_t)fontData.format;
        path_data = rt_malloc(fontData.pathSize);

        memcpy(path.bounding_box, fontData.boundingBox, 16);
        build_path(path_data, fontData.path_data, fontData.pathSize);

        path.path = path_data;
        path.path_changed = 1;
        path.path_length = fontData.pathSize;
        path.quality = VG_LITE_HIGH;

        /* Draw the path using the matrix.*/
        vg_lite_draw(&target, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFF99ff00);

        rt_free(path_data);
        vg_lite_finish();
        vg_lite_clear_path(&path);

        vg_lite_translate(fontData.dx, fontData.dy, &matrix);
        offsetX += fontData.dx;
    }
    dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
    rt_device_close(dev);
    rt_kprintf("vg_lite_save_png1\n");
    vg_lite_save_png("a.png", &target);
    rt_kprintf("vg_lite_save_png2\n");
#endif
}
static void loadpng(void)
{

    vg_lite_buffer_t image;
    vg_lite_load_png(&image, "1.png");
    vg_lite_buffer_t target;
    rt_device_t dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);
    rt_device_close(dev);
    target.width  = 390;
    target.height = 390;
    target.format = VG_LITE_RGBA8888;
    target.memory = (void *)info.framebuffer;
    target.address = (uint32_t)info.framebuffer;
    target.stride = target.width * 4;
    target.tiled = VG_LITE_LINEAR;
    vg_lite_clear(&target, NULL, 0x00ff0000);

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_translate(100, 100, &matrix);
    //vg_lite_rotate(33.0f, &matrix);
    //vg_lite_scale(0.4f, 0.4f, &matrix);
    //vg_lite_translate(320 / -2.0f, 320 / -4.0f, &matrix);
    //vg_lite_identity(&matPath);
    //vg_lite_translate(320 / 2.0f, 320 / 4.0f, &matPath);
    //vg_lite_scale(10, 10, &matPath);
    //vg_lite_draw_pattern(&target, &path, VG_LITE_FILL_EVEN_ODD, &matPath,
    //                     &image, &matrix, VG_LITE_BLEND_NONE, VG_LITE_PATTERN_COLOR, 0xffaabbcc, VG_LITE_FILTER_POINT);
    vg_lite_blit(&target, &image, &matrix, VG_LITE_BLEND_NONE, 0, VG_LITE_FILTER_POINT);
    //vg_lite_identity(&matrix);
    //vg_lite_translate(320 / 2.0f, 320 / 1.2f, &matrix);
    //vg_lite_rotate(33.0f, &matrix);
    //vg_lite_scale(0.4f, 0.4f, &matrix);
    //vg_lite_translate(320 / -1.5f, 320 / -2.0f, &matrix);
    //vg_lite_identity(&matPath);
    //vg_lite_translate(320 / 2.0f, 320 / 1.3f, &matPath);
    //vg_lite_scale(10, 10, &matPath);
    //vg_lite_draw_pattern(&target, &path, VG_LITE_FILL_EVEN_ODD, &matPath,
    //              &image, &matrix, VG_LITE_BLEND_NONE, VG_LITE_PATTERN_PAD, 0xffaabbcc, VG_LITE_FILTER_POINT);
    vg_lite_finish();
    dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
    rt_device_close(dev);
}
static void pattenfill(void)
{

    //vg_lite_buffer_t image;
    //vg_lite_load_raw(&image, "/elm/landscape.raw");
    vg_lite_buffer_t target;
    rt_device_t dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);
    rt_device_close(dev);
    target.width  = 390;
    target.height = 390;
    target.format = VG_LITE_RGBA8888;
    target.memory = (void *)info.framebuffer;
    target.address = (uint32_t)info.framebuffer;
    target.stride = target.width * 4;
    target.tiled = VG_LITE_LINEAR;
    vg_lite_clear(&target, NULL, 0x00ff0000);

    //vg_lite_matrix_t matrix, matPath;
    //vg_lite_identity(&matrix);
    //vg_lite_translate(320 / 2.0f, 320 / 4.0f, &matrix);
    //vg_lite_rotate(33.0f, &matrix);
    //vg_lite_scale(0.4f, 0.4f, &matrix);
    //vg_lite_translate(320 / -2.0f, 320 / -4.0f, &matrix);
    //vg_lite_identity(&matPath);
    //vg_lite_translate(320 / 2.0f, 320 / 4.0f, &matPath);
    //vg_lite_scale(10, 10, &matPath);
    //vg_lite_draw_pattern(&target, &path, VG_LITE_FILL_EVEN_ODD, &matPath,
    //                     &image, &matrix, VG_LITE_BLEND_NONE, VG_LITE_PATTERN_COLOR, 0xffaabbcc, VG_LITE_FILTER_POINT);
    //vg_lite_blit(&target, &image, &matrix, VG_LITE_BLEND_NONE, 0, VG_LITE_FILTER_POINT);
    //vg_lite_identity(&matrix);
    //vg_lite_translate(320 / 2.0f, 320 / 1.2f, &matrix);
    //vg_lite_rotate(33.0f, &matrix);
    //vg_lite_scale(0.4f, 0.4f, &matrix);
    //vg_lite_translate(320 / -1.5f, 320 / -2.0f, &matrix);
    //vg_lite_identity(&matPath);
    //vg_lite_translate(320 / 2.0f, 320 / 1.3f, &matPath);
    //vg_lite_scale(10, 10, &matPath);
    //vg_lite_draw_pattern(&target, &path, VG_LITE_FILL_EVEN_ODD, &matPath,
    //              &image, &matrix, VG_LITE_BLEND_NONE, VG_LITE_PATTERN_PAD, 0xffaabbcc, VG_LITE_FILTER_POINT);
    vg_lite_finish();
    dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
    rt_device_close(dev);
}
static void show_path(void)
{
    rt_kprintf("show_path1\n");
    vg_lite_buffer_t buffer;
    vg_lite_buffer_t *fb;

    /*            *-----*
               /       \
              /         \
             *           *
             |          /
             |         X
             |          \
             *           *
              \         /
               \       /
                *-----*
    */
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

    static vg_lite_path_t path =
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
    int err = 0;
    rt_device_t dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);
    buffer.width  = info.width;
    buffer.height = info.height;
    buffer.format = VG_LITE_RGBA8888;
    buffer.memory = (void *)info.framebuffer;
    buffer.address = (uint32_t)info.framebuffer;
    buffer.stride = buffer.width * 4;
    buffer.tiled = VG_LITE_LINEAR;
    //err = vg_lite_allocate(&buffer);rt_kprintf("err=%d\n",err);
    fb = &buffer;
    err = vg_lite_clear(fb, NULL, 0xFF00ffff); rt_kprintf("err=%d", err);
    // *** DRAW ***
    // Setup a 10x10 scale at center of buffer.
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    // Translate the matrix to the center of the buffer.
    vg_lite_translate(200, 160, &matrix);
    // Scale up matrix with 10 times.
    vg_lite_scale(10, 10, &matrix);
    // Scale matrix according to buffer size.
    vg_lite_scale(1, 1, &matrix);
    err = vg_lite_draw(fb, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFF00ff00);



    rt_kprintf("err=%d\n", err);
    err = vg_lite_finish(); rt_kprintf("err=%d\n", err);
    //void *p1 = rt_malloc(2048);
    //void *p2 = rt_realloc(p1, 2048+16);

    dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
    rt_device_close(dev);
    rt_kprintf("vg_lite_save_png1\n");
    vg_lite_save_png("vector.png", fb);
    rt_kprintf("vg_lite_save_png2\n");
}
static void show_ego(int argc, const char *argv[])
{

#if 1
    rt_device_t dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);
    rt_device_close(dev);
    ElmHandle evo_handle = ELM_NULL_HANDLE;
    vg_lite_buffer_t target;
    rt_memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    target.width  = info.width;
    target.height = info.height;
    target.format = VG_LITE_RGBA8888;
    target.memory = (void *)info.framebuffer;
    target.address = (uint32_t)info.framebuffer;
    target.stride = target.width * 4;
    target.tiled = VG_LITE_LINEAR;
    buffer = ElmWrapBuffer(info.width, info.height, info.width * 4,
                           (void *)info.framebuffer, (uint32_t)info.framebuffer,
                           _buffer_format_to_Elm(VG_LITE_RGBA8888));
    vg_lite_clear(&target, NULL, 0xffffffff);
    evo_handle = ElmCreateObjectFromFile(ELM_OBJECT_TYPE_EGO, argv[1]);
    //ElmTransfer(evo_handle,-150,-150);
    {
        //ElmRotate(evo_handle, 30);
        //ElmTransfer(evo_handle,-150,-300);
    }
    //ElmRotate(evo_handle, 30);
    //ElmTransfer(evo_handle, 200, 200);
    int status = ElmDraw(buffer, evo_handle);
    rt_kprintf("ElmDraw\n");

    ElmFinish();
    rt_kprintf("vg_lite_save_png1\n");
    //vg_lite_save_png("a.png", &target);
    rt_kprintf("vg_lite_save_png2\n");
    //dev = rt_device_find("lcd");
    //rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    //rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
    //rt_device_close(dev);
#endif
    dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
    rt_device_close(dev);
}
static void show_ebo(void)
{
    rt_device_t dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);
    rt_device_close(dev);
    ElmInitialize(480, 480);
    ElmHandle ebo_handle = ELM_NULL_HANDLE;
    vg_lite_buffer_t target;
    rt_memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    target.width  = info.width;
    target.height = info.height;
    target.format = VG_LITE_RGBA8888;
    target.memory = (void *)info.framebuffer;
    target.address = (uint32_t)info.framebuffer;
    target.stride = target.width * 4;
    target.tiled = VG_LITE_LINEAR;
    buffer = ElmWrapBuffer(info.width, info.height, info.width * 4,
                           (void *)info.framebuffer, (uint32_t)info.framebuffer,
                           _buffer_format_to_Elm(VG_LITE_RGBA8888));
    vg_lite_clear(&target, NULL, 0xffffffff);
    ebo_handle = ElmCreateObjectFromFile(ELM_OBJECT_TYPE_EBO, "/elm/git2.ebo");
    int status = ElmDraw(buffer, ebo_handle);
    rt_kprintf("ElmDraw\n");
    if (!status)
    {
        rt_kprintf("ElmDraw() failed:");
        cleanup();
        return;
    }
    ElmFinish();
    dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
    rt_device_close(dev);

}
static void rt_gpu_demo_entry(void *parameter)
{
    //pattenfill();
    //vladimir_font();
    show_path();
    //show_ego(parameter);
    //show_ebo();
    //loadpng();
    while (1)
    {
        rt_thread_mdelay(5000);
    }
}


int rt_gpu_ele_demo_init()
{
    rt_thread_t tid;
    tid = rt_thread_create("ele",
                           rt_gpu_demo_entry, NULL,
                           4096 * 4, 25, 10);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
    return 0;
}
//INIT_APP_EXPORT(rt_gpu_ele_demo_init);
MSH_CMD_EXPORT(show_ego, lcd display test);