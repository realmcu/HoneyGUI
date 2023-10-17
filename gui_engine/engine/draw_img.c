#include <draw_img.h>
#include <string.h>

extern void rtgui_image_bmp_init(void);
extern void rtgui_image_rgb_init(void);
extern void rtgui_image_rgb_compress_init(void);
extern void rtgui_image_compress_mem_init(void);
extern void rtgui_image_evo_init(void);
extern void rtgui_image_path_init(void);
extern void rtgui_image_bitmap_init(void);
static gui_list_t _rtgui_system_image_list = {NULL};

/* initialize rtgui image system */
void rtgui_system_image_init(void)
{
    gui_list_init(&_rtgui_system_image_list);
}

static struct rtgui_image_engine *rtgui_image_get_engine_from_mem(draw_img_t *img)
{
    gui_img_file_head_t head;
    memcpy(&head, img->data, sizeof(head));
    char *string = NULL;
    switch (head.type)
    {
    case 6:
        string = "evo_mem";
        break;
    case 0:
    case 1:
    case 2:

    case 4:
        if (head.compress)
        {
            string = "compress_mem";
        }
        else
        {
            string = "rgb_mem";
        }
        break;
    case 5:
        string = "bitmap";
        break;
    case 7:
    case 9:
    case 10:
    case 11:
        string = "stb";
        break;
    case 8:
        string = "dynamic";
        break;
    default:
        string = "not_support";
        break;
    }
    gui_list_t *node = NULL;
    struct rtgui_image_engine *engine;
    gui_list_for_each(node, &_rtgui_system_image_list)
    {
        GUI_ASSERT(node != NULL);
        engine = gui_list_entry(node, struct rtgui_image_engine, list);

        if (strlen(engine->name) == strlen(string))
        {
            if (strncasecmp(engine->name, string, strlen(engine->name)) == 0)
            {
                return engine;
            }
        }
    }

    bool match_no_image_engine = false;
    GUI_ASSERT(match_no_image_engine);
    return NULL;
}

#if 0
static struct rtgui_image_engine *rtgui_image_get_engine_by_filename(const char *fn)
{
    gui_list_t *node = NULL;
    struct rtgui_image_engine *engine;
    const char *ext;

    ext = fn + strlen(fn);
    while (ext != fn)
    {
        if (*ext == '.')
        {
            ext ++;
            if (*(ext - 3) == '.')//check '9.png'
            {
                ext -= 2;
            }
            break;
        }
        ext --;
    }
    if (ext == fn) { return NULL; } /* no ext */
    if ((strncasecmp("jpg", ext, strlen("jpg")) == 0) ||
        (strncasecmp("jpeg", ext, strlen("jpeg")) == 0) ||
        (strncasecmp("JPG", ext, strlen("JPG")) == 0) ||
        (strncasecmp("JPEG", ext, strlen("JPEG")) == 0))
    {
        ext = "jpeg";
    }
    gui_list_for_each(node, &_rtgui_system_image_list)
    {
        engine = gui_list_entry(node, struct rtgui_image_engine, list);
        if (strncasecmp(engine->name, ext, strlen(engine->name)) == 0)
        {
            return engine;
        }
    }

    return NULL;
}
#endif


void rtgui_image_load_scale(draw_img_t *img)
{
    struct gui_rgb_data_head head;
    memcpy(&head, img->data, sizeof(head));
    img->img_w = head.w;
    img->img_h = head.h;
}

bool rtgui_image_create(draw_img_t *img, bool load)
{
    struct rtgui_image_engine *engine;

    engine = rtgui_image_get_engine_from_mem(img);
    engine->image_load(img, load);
    img->engine = engine;


    return true;
}

bool rtgui_image_new_area(draw_img_t *img)
{

    memcpy(img->inverse, img->matrix, sizeof(struct rtgui_matrix));
    matrix_inverse(img->inverse);

    struct rtgui_pox pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    pox.p[0] = 0.0f;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    pox_mul(img->matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];


    pox.p[0] = (float)img->img_w - 1;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    pox_mul(img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }


    pox.p[0] = 0.0f;
    pox.p[1] = (float)img->img_h - 1;
    pox.p[2] = 1.0f;
    pox_mul(img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = (float)img->img_w - 1;
    pox.p[1] = (float)img->img_h - 1;
    pox.p[2] = 1.0f;
    pox_mul(img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    img->img_x = (int16_t)x_min;
    img->img_y = (int16_t)y_min;
    img->target_w = (int16_t)x_max - (int16_t)x_min;
    img->target_h = (int16_t)y_max - (int16_t)y_min;
    return true;
}




void rtgui_image_destroy(draw_img_t *image)
{
    GUI_ASSERT(image != NULL);

    image->engine->image_unload(image);

}

/* register an image engine */
void rtgui_image_register_engine(struct rtgui_image_engine *engine)
{
    GUI_ASSERT(engine != NULL);

    gui_list_append(&_rtgui_system_image_list, &(engine->list));
}

void rtgui_image_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect)
{
    if (image != NULL && image->engine != NULL)
    {
        /* use image engine to blit */
        image->engine->image_blit(image, dc, rect);
    }
}


