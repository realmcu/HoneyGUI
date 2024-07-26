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
#include "tp_algo.h"
#include "gui_button.h"
#include "gui_canvas_rect.h"
#include "gui_app.h"
#include "gui_api.h"
#if TARGET_RTL8773E
#include "wchar.h"
#include "ff.h"
#endif

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
#define GPS_LONGITUDE 120.58
#define GPS_LATITUDE 31.3
#define TILE_ZOOM_LEVEL (7)
#define WIN_W (SCREEN_W*3/2)
#define WIN_H (SCREEN_H*3/2)
#define MAP_WIDGET_NAME "_MAP"
#if _WIN32
#define ROOT_PATH "./gui_engine/example/screen_454_454/root_image/SDCARD/"
#elif defined RTL8772F
#define ROOT_PATH "/sdcard/"
#else
#define ROOT_PATH "/"
#endif
#define PATH404 "map/icon/404.jpg"
typedef long off_t;

#if TARGET_RTL8773E
static const TCHAR    *plus_fil_pth             = (const TCHAR *)_T("map/icon/plus.bin");
static const TCHAR    *minus_fil_pth            = (const TCHAR *)_T("map/icon/minus.bin");
static const TCHAR    *gps_fil_pth              = (const TCHAR *)_T("map/icon/GPS.bin");
static const TCHAR    *path404_fil_pth          = (const TCHAR *)_T("map/icon/404.jpg");
#endif
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
static void free_for_rgb(void *p);
// Convert longitude to tileX index at a certain zoom level
static int long2tilex(double lon, int z)
{
    return (int)(floor((lon + EARTH_LONGITUDE_RANGE / 2) / EARTH_LONGITUDE_RANGE * pow(2.0, z)));
}
// Convert latitude to tileY index at a certain zoom level
static int lat2tiley(double lat, int z)
{
    // The formula includes taking tan of latitude then log's  the value . After that it's scaled to 2's pow of zoom level
    return (int)(floor((1.0 - log(tan(lat * M_PI / 180.0) + 1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0
                       * pow(2.0, z)));
}

// function to generate tile URL path given the tile indices and zoom level
static void generateTileURL(int tileX, int tileY, int zoom)
{
    //gui_log("http://mt0.google.com/vt/lyrs=m@221097413,traffic&x=%d&y=%d&z=%d\n", tileX, tileY, zoom);
}
static void generateTilesForWindow(int windowWidth, int windowHeight, double center_lat,
                                   double center_lon,
                                   int zoom, gui_map_t *parent)
{
    // Calculate how many tiles are needed for each direction
    int tileCountX = ceil((double)windowWidth / TILE_SIZE);
    int tileCountY = ceil((double)windowHeight / TILE_SIZE);

    // Calculate the position of the center tile
    int centerX = long2tilex(center_lon, zoom);
    int centerY = lat2tiley(center_lat, zoom);
    //gui_log("centerX:%d,centerY:%d\n", centerX, centerY);
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
            //gui_log("Relative coordinates: (%d, %d)\n", (x - startX)*tile_size, (y - startY)*tile_size);
#if _WIN32 || RTL8772F || TARGET_RTL8773E
            typedef long off_t;

#if TARGET_RTL8773E
            char path_temp[100];
            TCHAR path[100];
            uint16_t path_len = 0;
            memset(path, 0, 100);
            memset(path_temp, 0, 100);
            sprintf(path_temp, "map/%d/%d/%d/tile.jpg", zoom, x, y);
            path_len = (strlen(path_temp) + 1) * 2;
            mbstowcs(path, path_temp, path_len);
#else
            char path[100];
            memset(path, 0, 100);
            sprintf(path, "%s/map/%d/%d/%d/tile.jpg", ROOT_PATH, zoom,
                    x, y);
#endif
            int fd;
            fd = gui_fs_open((const char *)path, 0);
            char *jpg = 0;
            off_t filesize = 0;

            if (fd > 0)
            {
                filesize = gui_fs_lseek(fd, 0, SEEK_END);
                jpg = gui_malloc(filesize);
                //gui_log("open %s Successful!\n", path);
                gui_fs_lseek(fd, 0, SEEK_SET);
                gui_fs_read(fd, jpg, filesize);
                gui_fs_close(fd);
            }
            else
            {
                //gui_log("open %s Fail!\n", path);
                memset(path, 0, 100);
#if TARGET_RTL8773E
                uint16_t path404_path_len = wcslen(path404_fil_pth);
                memcpy((uint8_t *)path, path404_fil_pth, path404_path_len * 2);
#else
                sprintf(path, "%s/%s", ROOT_PATH, PATH404);
#endif
                fd = gui_fs_open((const char *)path, 0);
                if (fd <= 0)
                {
                    return;
                }
                filesize = gui_fs_lseek(fd, 0, SEEK_END);
                jpg = gui_malloc(filesize);
                gui_fs_lseek(fd, 0, SEEK_SET);
                gui_fs_read(fd, jpg, filesize);
                gui_fs_close(fd);
            }
            if (parent->tile[y - startY][x - startX].img->data_buffer)
            {
                gui_free(parent->tile[y - startY][x - startX].img->data_buffer);
            }
            parent->tile[y - startY][x - startX].x = x;
            parent->tile[y - startY][x - startX].y = y;
            gui_img_stb_set_attribute(parent->tile[y - startY][x - startX].img, jpg, filesize, JPEG,
                                      (x - startX)*tile_size, (y - startY)*tile_size);

#else
            gui_canvas_rect_create((gui_obj_t *)parent, "canvas_rect", (x - startX)*tile_size,
                                   (y - startY)*tile_size, TILE_SIZE - 1,
                                   TILE_SIZE - 1,
                                   APP_COLOR_SILVER);
#endif
        }
    }
}
static void destory(gui_obj_t *obj)
{
    gui_win_t *this = (void *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
    gui_map_t *parent = GUI_TYPE(gui_map_t, obj);
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            gui_free(parent->tile[i][j].img->data_buffer);
        }
        gui_free(parent->button_data[i]);
    }

}
extern void gui_win_prepare_globle(gui_obj_t *obj);
static void gui_win_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_win_prepare_globle(obj);
            break;

        case OBJ_DESTORY:
            destory(obj);
            break;

        default:
            break;
        }
    }
}
static void ctor(gui_map_t *this, gui_obj_t *parent)
{
    extern void gui_win_ctor(gui_win_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                             int16_t y, int16_t w, int16_t h);
    gui_win_ctor((void *)this, parent, MAP_WIDGET_NAME, -(WIN_W - SCREEN_W) / 2,
                 -(WIN_H - SCREEN_H) / 2, WIN_W,
                 WIN_H);
    this->start_x = GET_BASE(this)->x;
    this->start_y = GET_BASE(this)->y;
    GET_BASE(this)->obj_cb = gui_win_cb;
}
static void load_new_tile(map_tile_t *tile, int16_t zoom)
{
#if _WIN32 || RTL8772F || TARGET_RTL8773E
    typedef long off_t;
#if TARGET_RTL8773E
    char path_temp[100];
    TCHAR path[100];
    uint16_t path_len = 0;
    memset(path, 0, 100);
    memset(path_temp, 0, 100);
    sprintf(path_temp, "map/%d/%d/%d/tile.jpg",
            zoom, tile->x, tile->y);
    path_len = (strlen(path_temp) + 1) * 2;
    mbstowcs(path, path_temp, path_len);
#else
    static char path[100];
    memset(path, 0, 100);
    sprintf(path, "%s/map/%d/%d/%d/tile.jpg", ROOT_PATH,
            zoom, tile->x, tile->y);
#endif
    int fd;
    fd = gui_fs_open((const char *)path, 0);
    char *jpg = 0;
    off_t filesize = 0;
    if (fd > 0)
    {
        filesize = gui_fs_lseek(fd, 0, SEEK_END);
        jpg = gui_malloc(filesize);

        gui_fs_lseek(fd, 0, SEEK_SET);
        gui_fs_read(fd, jpg, filesize);
        gui_fs_close(fd);
    }
    else
    {
        gui_log("open %s Fail!\n", path);
        memset(path, 0, 100);
#if TARGET_RTL8773E
        uint16_t path404_path_len = wcslen(path404_fil_pth);
        memcpy((uint8_t *)path, path404_fil_pth, path404_path_len * 2);
#else
        sprintf(path, "%s/%s", ROOT_PATH, PATH404);
#endif
        fd = gui_fs_open((const char *)path, 0);
        if (fd <= 0)
        {
            return;
        }
        filesize = gui_fs_lseek(fd, 0, SEEK_END);
        jpg = gui_malloc(filesize);
        gui_fs_lseek(fd, 0, SEEK_SET);
        gui_fs_read(fd, jpg, filesize);
        gui_fs_close(fd);
    }
    gui_img_stb_set_attribute_static(tile->img, jpg, filesize, JPEG, tile->img->base.x,
                                     tile->img->base.y);
#else
#endif
}


void pixel_to_latlon(double px_dx, double px_dy, double lat_prev, double lon_prev, int zoom,
                     double *lat, double *lon)
{
    double tiles_at_this_zoom = pow(2.0, (double)zoom);
    double lon_deg_per_px = 360.0 / (tiles_at_this_zoom * TILE_SIZE);
    double lat_deg_per_px = 2 * atan(exp(M_PI)) / (tiles_at_this_zoom * TILE_SIZE) * 180.0 / M_PI;

    *lon = lon_prev + px_dx * lon_deg_per_px;
    *lat = lat_prev - px_dy * lat_deg_per_px;
}
/**
 * @brief Check if the screen is being pressed and released, and record these states
 * If the screen is being pressed and moved, this code will move the map according to the slide of the finger and handle page turning issues (when the user slides beyond the currently displayed map area). When the map moves to the edge, it will load new tiles
 * When the slide is released, record the final position of the map
 * An important thing to note here is that the logic of moving the map includes how to handle it when the map slides to the edge. That is, when there are no more map tiles to display, the code will load new tiles for display. If no more map tiles can be loaded, the code will attempt to open a default image ("./gui_engine/example/screen_454_454/root_image/SDCARD/map/Tiles not found.jpg").
*/
static void wincb(gui_map_t *this)
{
    if (this->base.animate->progress_percent == 1)
    {
        //gui_log("wincb\n");
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
        double latitude;
        double longitude;
        pixel_to_latlon(-tp->deltaX, -tp->deltaY, this->latitude, this->longitude, this->zoom, &latitude,
                        &longitude);
        this->latitude = latitude;
        this->longitude = longitude;
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
                        free_for_rgb(this->tile[i][j].img->img->data);
                        this->tile[i][j].img->img->data = 0;
                    }

                    // gui_img_stb_set_attribute(
                    //     this->tile[i][j].img,
                    //     this->tile[i][j - 1].img->data_buffer,
                    //     this->tile[i][j - 1].img->data_length,
                    //     JPEG,
                    //     this->tile[i][j].img->base.x,
                    //     this->tile[i][j].img->base.y
                    // );
                    this->tile[i][j].img->data_buffer = this->tile[i][j - 1].img->data_buffer;
                    this->tile[i][j].img->data_length = this->tile[i][j - 1].img->data_length;
                    this->tile[i][j].img->img->data = this->tile[i][j - 1].img->img->data;
                    this->tile[i][j].x--;
                }
                this->tile[i][0].x--;
                load_new_tile(&this->tile[i][0], this->zoom);
            }


            GUI_BASE(this)->x = -256 ;
            this->start_x = -256 - tp->deltaX;
            GUI_BASE(this)->matrix->m[0][2] = GUI_BASE(this)->x;
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
                        free_for_rgb(this->tile[i][j].img->img->data);
                        this->tile[i][j].img->img->data = 0;
                    }

                    // gui_img_stb_set_attribute(
                    //     this->tile[i][j].img,
                    //     this->tile[i][j + 1].img->data_buffer,
                    //     this->tile[i][j + 1].img->data_length,
                    //     JPEG,
                    //     this->tile[i][j].img->base.x,
                    //     this->tile[i][j].img->base.y
                    // );
                    this->tile[i][j].img->data_buffer = this->tile[i][j + 1].img->data_buffer;
                    this->tile[i][j].img->data_length = this->tile[i][j + 1].img->data_length;
                    this->tile[i][j].img->img->data = this->tile[i][j + 1].img->img->data;
                    this->tile[i][j].x++;
                }

                // Load new image data to "tile[i][2]"
                this->tile[i][2].x++;
                load_new_tile(&this->tile[i][2], this->zoom);
            }

            GUI_BASE(this)->x = 0;
            this->start_x = - tp->deltaX;
            GUI_BASE(this)->matrix->m[0][2] = GUI_BASE(this)->x;
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
                        free_for_rgb(this->tile[j][i].img->img->data);
                        this->tile[j][i].img->img->data = 0;
                    }

                    // gui_img_stb_set_attribute(
                    //     this->tile[j][i].img,
                    //     this->tile[j - 1][i].img->data_buffer,
                    //     this->tile[j - 1][i].img->data_length,
                    //     JPEG,
                    //     this->tile[j][i].img->base.x,
                    //     this->tile[j][i].img->base.y
                    // );
                    this->tile[j][i].img->data_buffer = this->tile[j - 1][i].img->data_buffer;
                    this->tile[j][i].img->data_length = this->tile[j - 1][i].img->data_length;
                    this->tile[j][i].img->img->data = this->tile[j - 1][i].img->img->data;
                    this->tile[j][i].y--;
                }
                this->tile[0][i].y--;
                load_new_tile(&this->tile[0][i], this->zoom);
            }


            GUI_BASE(this)->y = -256 ;
            this->start_y = -256 - tp->deltaY;
            GUI_BASE(this)->matrix->m[1][2] = GUI_BASE(this)->y;
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
                        free_for_rgb(this->tile[j][i].img->img->data);
                        this->tile[j][i].img->img->data = 0;
                    }

                    // gui_img_stb_set_attribute(
                    //     this->tile[j][i].img,
                    //     this->tile[j + 1][i].img->data_buffer,
                    //     this->tile[j + 1][i].img->data_length,
                    //     JPEG,
                    //     this->tile[j][i].img->base.x,
                    //     this->tile[j][i].img->base.y
                    // );
                    this->tile[j][i].img->data_buffer = this->tile[j + 1][i].img->data_buffer;
                    this->tile[j][i].img->data_length = this->tile[j + 1][i].img->data_length;
                    this->tile[j][i].img->img->data = this->tile[j + 1][i].img->img->data;
                    this->tile[j][i].y++;
                }
                this->tile[2][i].y++;
                load_new_tile(&this->tile[2][i], this->zoom);
            }


            GUI_BASE(this)->y = -(256 * 3 - SCREEN_H - 256) ;
            this->start_y = -(256 * 3 - SCREEN_H - 256) - tp->deltaY;
            GUI_BASE(this)->matrix->m[1][2] = GUI_BASE(this)->y;
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
static void free_for_rgb(void *p)
{
#ifdef _WIN32
    gui_free(p);
#elif defined RTL8772F || TARGET_RTL8773E
    gui_lower_free(p);
#else
    gui_free(p);
#endif
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/
static void update_zoom(int zoom)
{
    gui_map_t *map = 0;
    gui_obj_tree_get_widget_by_name(&gui_current_app()->screen, MAP_WIDGET_NAME, (void *)&map);
    //float longitude;
    //float latitude;
    //tile_to_latlon(map->tile[1][1].x, map->tile[1][1].y, map->zoom, &latitude, &longitude);
    if (zoom == 1)
    {
        map->zoom++;
    }
    else if (zoom == 0)
    {
        map->zoom--;
    }
    else if (zoom == -1)
    {
        map->latitude = GPS_LATITUDE;
        map->longitude = GPS_LONGITUDE;
        GUI_BASE(map)->x = -TILE_SIZE / 2;
        GUI_BASE(map)->y = -TILE_SIZE / 2;
    }

    if (map->zoom < 0)
    {
        map->zoom = 0;
    }
    else if (map->zoom > 21)
    {
        map->zoom = 21;
    }

    //gui_log("%d,%d,%f,%f\n", map->tile[1][1].x, map->tile[1][1].y, latitude, longitude);
    generateTilesForWindow(WIN_W, WIN_H, map->latitude, map->longitude, map->zoom, map);


}

static void zoom_cb()
{
    update_zoom(1);
}
static void zoom_minus_cb()
{
    update_zoom(0);
}
static void gps_cb()
{
    update_zoom(-1);
}
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
    double center_lat = GPS_LATITUDE;
    double center_lon = GPS_LONGITUDE;
    this->longitude = GPS_LONGITUDE;
    this->latitude = GPS_LATITUDE;
    // Zoom level
    int zoom = TILE_ZOOM_LEVEL;
    this->zoom = zoom;
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
#if TARGET_RTL8773E
            this->tile[i][j].img = gui_img_stb_create_from_mem(win, 0, 0, 0, JPEG, RGB565, 0, 0);
#else
            this->tile[i][j].img = gui_img_stb_create_from_mem(win, 0, 0, 0, JPEG, RGB888, 0, 0);
#endif
        }
    }
    generateTilesForWindow(windowWidth, windowHeight, center_lat, center_lon, zoom, this);
    {
        gui_button_t *zoom = gui_button_create(parent, 50, 200 - 30, 80, 80, 0, 0, 0, BUTTON_BG_ICON, 0);
        {
            static char path[100];
            memset(path, 0, 100);
#if TARGET_RTL8773E
            uint16_t plus_path_len = wcslen(plus_fil_pth);
            memcpy((uint8_t *)path, plus_fil_pth, plus_path_len * 2);
#else
            sprintf(path, "%s/%s", ROOT_PATH, "/map/icon/plus.bin");
#endif
            int fd;
            fd = gui_fs_open(path, 0);

            char *jpg = 0;
            off_t filesize = 0;
            if (fd > 0)
            {
                filesize = gui_fs_lseek(fd, 0, SEEK_END);
                jpg = gui_malloc(filesize);
                this->button_data[0] = jpg;
                gui_fs_lseek(fd, 0, SEEK_SET);
                gui_fs_read(fd, jpg, filesize);
                gui_fs_close(fd);
                gui_img_t *img = gui_img_create_from_mem((void *)zoom, 0, jpg, 30, 30, 0, 0);
                gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            }


        }
        gui_button_click((void *)zoom, (gui_event_cb_t)zoom_cb, 0);
    }
    {
        gui_button_t *zoom = gui_button_create(parent, 50, 300 - 30, 80, 80, 0, 0, 0, BUTTON_BG_ICON, 0);

        {

            static char path[100];
            memset(path, 0, 100);
#if TARGET_RTL8773E
            uint16_t minus_path_len = wcslen(minus_fil_pth);
            memcpy((uint8_t *)path, minus_fil_pth, minus_path_len * 2);
#else
            sprintf(path, "%s/%s", ROOT_PATH, "/map/icon/minus.bin");
#endif
            int fd;
            fd = gui_fs_open(path, 0);

            char *jpg = 0;
            off_t filesize = 0;
            if (fd > 0)
            {
                filesize = gui_fs_lseek(fd, 0, SEEK_END);
                jpg = gui_malloc(filesize);
                this->button_data[1] = jpg;
                gui_fs_lseek(fd, 0, SEEK_SET);
                gui_fs_read(fd, jpg, filesize);
                gui_fs_close(fd);
                gui_img_t *img = gui_img_create_from_mem((void *)zoom, 0, jpg, 30, 30, 0, 0);
                gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            }


        }
        gui_button_click((void *)zoom, (gui_event_cb_t)zoom_minus_cb, 0);
    }
    {
        gui_button_t *zoom = gui_button_create(parent, 300, 300 - 30, 80, 80, 0, 0, 0, BUTTON_BG_ICON, 0);

        {

            static char path[100];
            memset(path, 0, 100);
#if TARGET_RTL8773E
            uint16_t gps_path_len = wcslen(gps_fil_pth);
            memcpy((uint8_t *)path, gps_fil_pth, gps_path_len * 2);
#else
            sprintf(path, "%s/%s", ROOT_PATH, "/map/icon/GPS.bin");
#endif
            int fd;
            fd = gui_fs_open(path, 0);

            char *jpg = 0;
            off_t filesize = 0;
            if (fd > 0)
            {
                filesize = gui_fs_lseek(fd, 0, SEEK_END);
                jpg = gui_malloc(filesize);
                this->button_data[2] = jpg;
                gui_fs_lseek(fd, 0, SEEK_SET);
                gui_fs_read(fd, jpg, filesize);
                gui_fs_close(fd);
                gui_img_t *img = gui_img_create_from_mem((void *)zoom, 0, jpg, 30, 30, 0, 0);
                gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            }


        }
        gui_button_click((void *)zoom, (gui_event_cb_t)gps_cb, 0);
    }
    return this;
}


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */




