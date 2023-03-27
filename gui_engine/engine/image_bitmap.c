#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>
#include <gui_graphic.h>
#include "gui_bitmap.h"
static bool rtgui_image_bitmap_check(draw_img_t *image);
static bool rtgui_image_bitmap_load(draw_img_t *image,
                                    bool load);
static void rtgui_image_bitmap_unload(draw_img_t *image);
static void rtgui_image_bitmap_blit(draw_img_t *image, struct gui_dispdev *dc,
                                    struct rtgui_rect *rect);

struct rtgui_image_engine rtgui_image_bitmap_engine =
{
    "bitmap",
    { NULL },
    rtgui_image_bitmap_check,
    rtgui_image_bitmap_load,
    rtgui_image_bitmap_unload,
    rtgui_image_bitmap_blit,
};

static bool rtgui_image_bitmap_check(draw_img_t *image)
{
    return true;
}

static bool rtgui_image_bitmap_load(draw_img_t *image,
                                    bool load)
{
    gui_img_file_head_t head;
    memcpy(&head, image->data, sizeof(head));

    /* set image information */
    (image)->img_w = head.size.img_size.img_w;
    (image)->img_h = head.size.img_size.img_h;
    if (head.type != 5)
    {
        return false;
    }
    image->engine = &rtgui_image_bitmap_engine;
    //rt_kprintf("w = %d, h = %d, bit_depth =%d\n", image->w, image->h, image->bit_depth);
    return true;
}

static void rtgui_image_bitmap_unload(draw_img_t *image)
{

}


bool get_bit_from_bitmap(uint8_t *bitmap, int row_start, int col_start, int w, int h)
{

    int length = row_start * w + col_start;
    return ((bitmap[length / 8]) >> (length % 8)) & 0x1;
}
/*     void GaussianSmooth2D(const Mat &src, Mat &dst, double sigma)
    {
        sigma = sigma > 0 ? sigma : 0;

        double scale = -0.5/(sigma*sigma);
        const double PI = 3.141592653;
        double cons = -scale/PI;

        double sum = 0;
        cons * exp(scale * (x*x + y*y));


        for(int i = ksize*ksize-1; i >=0; i--)
        {
            *(kernel+i) /= sum;
        }
        for(int j = 0; j < src.cols-ksize; j++)
        {
            for(int i = 0; i < src.rows-ksize; i++)
            {
                double acc = 0;

                for(int m = 0; m < ksize; m++)
                {
                    for(int n = 0; n < ksize; n++)
                    {
                        acc += *(srcData + src.step * (i+n) + src.channels() * (j+m)) * kernel[m*ksize+n];
                    }
                }


                *(dstData + dst.step * (i + (ksize - 1)/2) + (j + (ksize -1)/2)) = (int)acc;
            }
        }
        delete []kernel;
    }  */
void QuickSort(uint8_t *arr, int low, int high);
void QuickSort(uint8_t *arr, int low, int high)
{
    if (low < high)
    {
        int i = low;
        int j = high;
        int k = arr[low];
        while (i < j)
        {
            while (i < j && arr[j] >= k)
            {
                j--;
            }

            if (i < j)
            {
                arr[i++] = arr[j];
            }

            while (i < j && arr[i] < k)
            {
                i++;
            }

            if (i < j)
            {
                arr[j--] = arr[i];
            }
        }

        arr[i] = k;

        QuickSort(arr, low, i - 1);
        QuickSort(arr, i + 1, high);
    }
}

void Show(uint8_t *arr, int len)
{
    for (int i = 0; i < len; i++)
    {
//        printf("%d ", arr[i]);
    }
//    printf("\n");
}

void quick_sort(unsigned char *arr, size_t len)
{
    QuickSort(arr, 0, len - 1);
}
/*     void GaussianSmooth(double sigma)
    {
        sigma = sigma > 0 ? sigma : -sigma;
        int ksize = ceil(sigma * 3) * 2 + 1;

        double *kernel = new double[ksize];

        double scale = -0.5/(sigma*sigma);
        const double PI = 3.141592653;
        double cons = 1/sqrt(-scale / PI);

        double sum = 0;
        int kcenter = ksize/2;
        int i = 0, j = 0;
        for(i = 0; i < ksize; i++)
        {
            int x = i - kcenter;
            *(kernel+i) = cons * exp(x * x * scale);
            sum += *(kernel+i);

    //      cout << " " << *(kernel+i);
        }
    //  cout << endl;
        for(i = 0; i < ksize; i++)
        {
            *(kernel+i) /= sum;
    //      cout << " " << *(kernel+i);
        }
    //  cout << endl;


        for(int y = 0; y < src.rows; y++)
        {
            for(int x = 0; x < src.cols; x++)
            {
                double mul = 0;
                sum = 0;
                double bmul = 0, gmul = 0, rmul = 0;
                for(i = -kcenter; i <= kcenter; i++)
                {
                    if((x+i) >= 0 && (x+i) < src.cols)
                    {
                        if(src.channels() == 1)
                        {
                            mul += *(srcData+y*src.step+(x+i))*(*(kernel+kcenter+i));
                        }
                        else
                        {
                            bmul += *(srcData+y*src.step+(x+i)*src.channels() + 0)*(*(kernel+kcenter+i));
                            gmul += *(srcData+y*src.step+(x+i)*src.channels() + 1)*(*(kernel+kcenter+i));
                            rmul += *(srcData+y*src.step+(x+i)*src.channels() + 2)*(*(kernel+kcenter+i));
                        }
                        sum += (*(kernel+kcenter+i));
                    }
                }
                if(src.channels() == 1)
                {
                    *(tempData+y*temp.step+x) = mul/sum;
                }
                else
                {
                    *(tempData+y*temp.step+x*temp.channels()+0) = bmul/sum;
                    *(tempData+y*temp.step+x*temp.channels()+1) = gmul/sum;
                    *(tempData+y*temp.step+x*temp.channels()+2) = rmul/sum;
                }
            }
        }


        for(int x = 0; x < temp.cols; x++)
        {
            for(int y = 0; y < temp.rows; y++)
            {
                double mul = 0;
                sum = 0;
                double bmul = 0, gmul = 0, rmul = 0;
                for(i = -kcenter; i <= kcenter; i++)
                {
                    if((y+i) >= 0 && (y+i) < temp.rows)
                    {
                        if(temp.channels() == 1)
                        {
                            mul += *(tempData+(y+i)*temp.step+x)*(*(kernel+kcenter+i));
                        }
                        else
                        {
                            bmul += *(tempData+(y+i)*temp.step+x*temp.channels() + 0)*(*(kernel+kcenter+i));
                            gmul += *(tempData+(y+i)*temp.step+x*temp.channels() + 1)*(*(kernel+kcenter+i));
                            rmul += *(tempData+(y+i)*temp.step+x*temp.channels() + 2)*(*(kernel+kcenter+i));
                        }
                        sum += (*(kernel+kcenter+i));
                    }
                }
                if(temp.channels() == 1)
                {
                    *(dstData+y*dst.step+x) = mul/sum;
                }
                else
                {
                    *(dstData+y*dst.step+x*dst.channels()+0) = bmul/sum;
                    *(dstData+y*dst.step+x*dst.channels()+1) = gmul/sum;
                    *(dstData+y*dst.step+x*dst.channels()+2) = rmul/sum;
                }

            }
        }

        delete[] kernel;
    } */
static uint16_t rgb8882565(gui_color_t color_blend)
{
    uint16_t red = color_blend.channel.red * 0x1f / 0xff << 11;
    uint16_t green = color_blend.channel.green * 0x3f / 0xff << 5;
    uint16_t blue = color_blend.channel.blue * 0x1f / 0xff;
    return red + green + blue;

}
static void normal_blit_bitmap_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                        struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = (image)->img_w;
    int image_h = (image)->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    uint8_t *image_off = sizeof(gui_img_file_head_t) + (uint8_t *)(image->data);
    gui_color_t color_blend = {.rgba = ((gui_bitmap_t *)image)->color };
    uint16_t color = rgb8882565(color_blend);

    uint8_t alpha_value = color_blend.channel.alpha;
    GUI_UNUSED(alpha_value);

#if 1
    //gui_log("image : %s\n",image->base.name);
    //gui_log("y_start : %d,y_end : %d, x_start : %d, x_end : %d\n",y_start,y_end, x_start, x_end);
    uint16_t *blurbuf = 0;
    GUI_UNUSED(blurbuf);

    {
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int row_start = i - image_y;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                if (get_bit_from_bitmap(image_off, row_start, j - image_x, image_w, image_h))
                {
                    writebuf[write_off + j] = color;
                }
            }
        }
    }

#endif

}




static void rtgui_image_bitmap_blit(draw_img_t *image, struct gui_dispdev *dc,
                                    struct rtgui_rect *rect)
{
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;//
    if (image->blend_mode == IMG_FILTER_BLACK)
    {
        if ((dc_bytes_per_pixel == 2))
        {
            normal_blit_bitmap_2_rgb565(image, dc, rect);
        }
    }

    else if (image->blend_mode == IMG_BYPASS_MODE)
    {
        if ((dc_bytes_per_pixel == 2))
        {
            normal_blit_bitmap_2_rgb565(image, dc, rect);
        }
    }

}


void rtgui_image_bitmap_init(void)
{
    /* register rgb565 on image system */
    rtgui_image_register_engine(&rtgui_image_bitmap_engine);
}

