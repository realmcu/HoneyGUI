/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_curtain.c
  * @brief create a curtain effect widget,which should be nested in a curtainview.
  * @details Slide to extend and retract curtains
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/24
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include <stdio.h>

#include "gui_map.h"
#include "gui_text.h"
#include "gui_img_stb.h"
#include "gui_rect.h"
#include "tp_algo.h"
/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */



/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{int
  */

#define SCREEN_W ((int)gui_get_screen_width())
#define SCREEN_H ((int)gui_get_screen_height())
#define TILE_SIZE (256)
#define EARTH_LONGITUDE_RANGE 360.0
#define BEIJING_CITY_LONGITUDE 104.0668
#define BEIJING_CITY_LATITUDE 30.5728
#define TILE_ZOOM_LEVEL (8)
#define WIN_W (SCREEN_W*3/2)
#define WIN_H (SCREEN_H*3/2)
/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */

static int tile_size = TILE_SIZE;

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */
#// Convert longitude to tileX index at a certain zoom level
int long2tilex(double lon, int z)
{
    return (int)(floor((lon + EARTH_LONGITUDE_RANGE / 2) / EARTH_LONGITUDE_RANGE * pow(2.0, z)));
}
// Convert latitude to tileY index at a certain zoom level
int lat2tiley(double lat, int z)
{
    // The formula includes taking tan of latitude then log's  the value . After that it's scaled to 2's pow of zoom level
    return (int)(floor((1.0 - log(tan(lat * M_PI / 180.0) + 1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0
                       * pow(2.0, z)));
}

double calculateScale(double latitude, int zoomLevel)
{
    double scale = 156543.03 * cos(latitude * (M_PI / 180)) / pow(2, zoomLevel);
    return scale;
}
// function to generate tile URL path given the tile indices and zoom level
void generateTileURL(int tileX, int tileY, int zoom)
{
    gui_log("http://mt0.google.com/vt/lyrs=m@221097413,traffic&x=%d&y=%d&z=%d\n", tileX, tileY, zoom);
}
void generateTilesForWindow(int windowWidth, int windowHeight, double center_lat, double center_lon,
                            int zoom, gui_map_t *parent)
{
    // Calculate how many tiles are needed for each direction
    int tileCountX = ceil((double)windowWidth / TILE_SIZE);
    int tileCountY = ceil((double)windowHeight / TILE_SIZE);

    // Calculate the position of the center tile
    int centerX = long2tilex(center_lon, zoom);
    int centerY = lat2tiley(center_lat, zoom);

    // Calculate the range of x,y of the needed tiles
    int startX = centerX - tileCountX / 2;
    int startY = centerY - tileCountY / 2;
    int endX = startX + tileCountX;
    int endY = startY + tileCountY;

    // Generate the URLs and relative coordinates for all tiles in the range
    for (int x = startX; x < endX; x++)
    {
        for (int y = startY; y < endY; y++)
        {
            generateTileURL(x, y, zoom);
            gui_log("Relative coordinates: (%d, %d)\n", (x - startX)*tile_size, (y - startY)*tile_size);
#if _WIN32
            char path[100];
            memset(path, 0, 100);
            sprintf(path, "./gui_engine/example/screen_454_454/root_image/SDCARD/map/%d/%d/%d/tile.jpg", zoom,
                    x, y);
            int fd;
            fd = gui_fs_open(path, 0);

            char *jpg = 0;
            off_t filesize = 0;
            if (fd > 0)
            {
                filesize = gui_fs_lseek(fd, 0, SEEK_END);
                jpg = gui_malloc(filesize);
                gui_log("open %s Successful!\n", path);
                gui_fs_lseek(fd, 0, SEEK_SET);
                gui_fs_read(fd, jpg, filesize);
            }
            else
            {
                gui_log("open %s Fail!\n", path);
                //return 0;
            }
            if (parent->tile[y - startY][x - startX].img->data_buffer)
            {
                gui_free(parent->tile[y - startY][x - startX].img->data_buffer);
            }
            parent->tile[y - startY][x - startX].x = x;
            parent->tile[y - startY][x - startX].y = y;
            parent->tile[y - startY][x - startX].zoom = zoom;
            gui_stbimg_set_attribute(parent->tile[y - startY][x - startX].img, jpg, filesize, JPEG,
                                     (x - startX)*tile_size, (y - startY)*tile_size);

#else
            gui_rect((gui_obj_t *)parent, (x - startX)*tile_size, (y - startY)*tile_size, TILE_SIZE - 1,
                     TILE_SIZE - 1,
                     APP_COLOR_SILVER);
#endif
        }
    }
}
static void ctor(gui_map_t *this, gui_obj_t *parent)
{
    extern void gui_win_ctor(gui_win_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                             int16_t y, int16_t w, int16_t h);
    gui_win_ctor((void *)this, parent, 0, -(WIN_W - SCREEN_W) / 2, -(WIN_H - SCREEN_H) / 2, WIN_W,
                 WIN_H);
    this->start_x = GET_BASE(this)->x;
    this->start_y = GET_BASE(this)->y;
}


static void wincb(gui_map_t *this)
{
    if (this->base.animate->progress_percent == 1)
    {
        gui_log("wincb\n");
    }
    touch_info_t *tp = tp_get_info();
    if (tp->pressed)
    {
        this->press = 1;
        this->release = 0;
    }
    if (tp->released)
    {
        this->press = 0;
        this->release = 1;
    }
    static bool deltaX_right_flag, deltaX_left_flag;
    if (this->press)
    {
        GUI_BASE(this)->x = this->start_x + tp->deltaX;
        GUI_BASE(this)->y = this->start_y + tp->deltaY;
        static int deltaX_old;

        if (deltaX_old < tp->deltaX)
        {
            deltaX_right_flag = 1;
            deltaX_left_flag = 0;
        }
        else if (deltaX_old > tp->deltaX)
        {
            deltaX_left_flag = 1;
            deltaX_right_flag = 0;
        }

        //gui_log("deltaX_old:%d, d:%d\n", deltaX_old, tp->deltaX);
        if (GUI_BASE(this)->x >= 0 && (tp->deltaX > 0 || deltaX_right_flag) && !deltaX_left_flag)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 2; j > 0; j--)
                {
                    if (j == 2)
                    {
                        gui_free(this->tile[i][j].img->data_buffer);
                    }

                    gui_stbimg_set_attribute(
                        this->tile[i][j].img,
                        this->tile[i][j - 1].img->data_buffer,
                        this->tile[i][j - 1].img->data_length,
                        JPEG,
                        this->tile[i][j].img->base.x,
                        this->tile[i][j].img->base.y
                    );

                    this->tile[i][j].x--;
                }
                {
#if _WIN32
                    char path[100];
                    memset(path, 0, 100);
                    sprintf(path, "./gui_engine/example/screen_454_454/root_image/SDCARD/map/%d/%d/%d/tile.jpg",
                            this->tile[i][0].zoom,
                            this->tile[i][0].x - 1, this->tile[i][0].y);
                    int fd;
                    fd = gui_fs_open(path, 0);

                    char *jpg = 0;
                    off_t filesize = 0;
                    if (fd > 0)
                    {
                        filesize = gui_fs_lseek(fd, 0, SEEK_END);
                        jpg = gui_malloc(filesize);
                        //gui_log("open %s Successful!\n", path);
                        gui_fs_lseek(fd, 0, SEEK_SET);
                        gui_fs_read(fd, jpg, filesize);
                    }
                    else
                    {
                        //gui_log("open %s Fail!\n", path);
                    }
                    gui_stbimg_set_attribute(this->tile[i][0].img, jpg, filesize, JPEG, this->tile[i][0].img->base.x,
                                             this->tile[i][0].img->base.y);
                    this->tile[i][0].x--;
#else
#endif
                }
            }


            GUI_BASE(this)->x = -256 ;
            this->start_x = -256 - tp->deltaX;
        }
        if (GUI_BASE(this)->x <= -(GUI_BASE(this)->w - SCREEN_W)  && (tp->deltaX < 0 || deltaX_left_flag) &&
            !deltaX_right_flag)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    if (j == 0)
                    {
                        gui_free(this->tile[i][j].img->data_buffer);
                    }

                    gui_stbimg_set_attribute(
                        this->tile[i][j].img,
                        this->tile[i][j + 1].img->data_buffer,
                        this->tile[i][j + 1].img->data_length,
                        JPEG,
                        this->tile[i][j].img->base.x,
                        this->tile[i][j].img->base.y
                    );

                    this->tile[i][j].x++;
                }

                // Load new image data to "tile[i][2]"
#if _WIN32
                char path[100];
                memset(path, 0, 100);
                sprintf(path, "./gui_engine/example/screen_454_454/root_image/SDCARD/map/%d/%d/%d/tile.jpg",
                        this->tile[i][2].zoom, this->tile[i][2].x + 1, this->tile[i][2].y);
                int fd;
                fd = gui_fs_open(path, 0);

                char *jpg = 0;
                off_t filesize = 0;
                if (fd > 0)
                {
                    filesize = gui_fs_lseek(fd, 0, SEEK_END);
                    jpg = gui_malloc(filesize);
                    //gui_log("open %s Successful!\n", path);
                    gui_fs_lseek(fd, 0, SEEK_SET);
                    gui_fs_read(fd, jpg, filesize);
                }
                else
                {
                    //gui_log("open %s Fail!\n", path);
                }
                gui_stbimg_set_attribute(this->tile[i][2].img, jpg, filesize, JPEG, this->tile[i][2].img->base.x,
                                         this->tile[i][2].img->base.y);
                this->tile[i][2].x++;
#else
#endif
            }

            GUI_BASE(this)->x = 0;
            this->start_x = - tp->deltaX;
        }

        if (GUI_BASE(this)->y >= 0 && (tp->deltaY > 0))
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 2; j > 0; j--)
                {
                    if (j == 2)
                    {
                        gui_free(this->tile[j][i].img->data_buffer);
                    }

                    gui_stbimg_set_attribute(
                        this->tile[j][i].img,
                        this->tile[j - 1][i].img->data_buffer,
                        this->tile[j - 1][i].img->data_length,
                        JPEG,
                        this->tile[j][i].img->base.x,
                        this->tile[j][i].img->base.y
                    );

                    this->tile[j][i].y--;
                }
                {
#if _WIN32
                    char path[100];
                    memset(path, 0, 100);
                    sprintf(path, "./gui_engine/example/screen_454_454/root_image/SDCARD/map/%d/%d/%d/tile.jpg",
                            this->tile[0][i].zoom,
                            this->tile[0][i].x, this->tile[0][i].y - 1);
                    int fd;
                    fd = gui_fs_open(path, 0);

                    char *jpg = 0;
                    off_t filesize = 0;
                    if (fd > 0)
                    {
                        filesize = gui_fs_lseek(fd, 0, SEEK_END);
                        jpg = gui_malloc(filesize);
                        //gui_log("open %s Successful!\n", path);
                        gui_fs_lseek(fd, 0, SEEK_SET);
                        gui_fs_read(fd, jpg, filesize);
                    }
                    else
                    {
                        //gui_log("open %s Fail!\n", path);
                    }
                    gui_stbimg_set_attribute(this->tile[0][i].img, jpg, filesize, JPEG, this->tile[0][i].img->base.x,
                                             this->tile[0][i].img->base.y);
                    this->tile[0][i].y--;
#else
#endif
                }
            }


            GUI_BASE(this)->y = -256 ;
            this->start_y = -256 - tp->deltaY;
        }

        if (GUI_BASE(this)->y <= -(256 * 3 - SCREEN_H) && (tp->deltaY < 0))
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    if (j == 0)
                    {
                        gui_free(this->tile[j][i].img->data_buffer);
                    }

                    gui_stbimg_set_attribute(
                        this->tile[j][i].img,
                        this->tile[j + 1][i].img->data_buffer,
                        this->tile[j + 1][i].img->data_length,
                        JPEG,
                        this->tile[j][i].img->base.x,
                        this->tile[j][i].img->base.y
                    );

                    this->tile[j][i].y++;
                }
                {
#if _WIN32
                    char path[100];
                    memset(path, 0, 100);
                    sprintf(path, "./gui_engine/example/screen_454_454/root_image/SDCARD/map/%d/%d/%d/tile.jpg",
                            this->tile[2][i].zoom,
                            this->tile[2][i].x, this->tile[2][i].y + 1);
                    int fd;
                    fd = gui_fs_open(path, 0);

                    char *jpg = 0;
                    off_t filesize = 0;
                    if (fd > 0)
                    {
                        filesize = gui_fs_lseek(fd, 0, SEEK_END);
                        jpg = gui_malloc(filesize);
                        //gui_log("open %s Successful!\n", path);
                        gui_fs_lseek(fd, 0, SEEK_SET);
                        gui_fs_read(fd, jpg, filesize);
                    }
                    else
                    {
                        //gui_log("open %s Fail!\n", path);
                    }
                    gui_stbimg_set_attribute(this->tile[2][i].img, jpg, filesize, JPEG,
                                             this->tile[2][i].img->base.x, this->tile[2][i].img->base.y);
                    this->tile[2][i].y++;
#else
#endif
                }
            }


            GUI_BASE(this)->y = -(256 * 3 - SCREEN_H - 256) ;
            this->start_y = -(256 * 3 - SCREEN_H - 256) - tp->deltaY;
        }


        deltaX_old = tp->deltaX;
    }
    if (this->release)
    {
        this->start_x = GUI_BASE(this)->x;
        this->start_y = GUI_BASE(this)->y;
        deltaX_right_flag = 0;
        deltaX_left_flag = 0;
    }

    //gui_log("x:%d,%d\n",GUI_BASE(this)->x, tp->deltaX);




}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_map_t *gui_map_create(void *parent)
{
#define _GUI_NEW_GUI_MAP_PARAM this, parent
    GUI_CREATE_HELPER(gui_map_t, ctor, _GUI_NEW_GUI_MAP_PARAM)
    gui_win_t *win = &(this->base);
    gui_win_set_animate(win, 1000, -1, wincb, this);
    // Window size
    int windowWidth = WIN_W;
    int windowHeight = WIN_H;

    // The longitude and latitude to be displayed
    double center_lat = BEIJING_CITY_LATITUDE;
    double center_lon = BEIJING_CITY_LONGITUDE;

    // Zoom level
    int zoom = TILE_ZOOM_LEVEL;
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            this->tile[i][j].img = gui_stbimg_create_from_mem(win, 0, 0, 0, JPEG, 0, 0);
        }
    }
    generateTilesForWindow(windowWidth, windowHeight, center_lat, center_lon, zoom, this);


    return 0;
}


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */




