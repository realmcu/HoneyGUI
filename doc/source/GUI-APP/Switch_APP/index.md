
##  App switching
### App switching animation sample
- The first app has a gray interface, and the second app has a blue interface. 
- Clicking on the first app's interface will switch to the second app. 
- The first app will gradually become transparent until it disappears when switching, and the second app will enlarge to fill the screen. 
- Swipe on the second app interface to switch back to the first app.
- The second app will gradually shrink and become transparent as you drag it until it disappears. The first app will shrink from very large to the screen size and gradually become opaque.




```eval_rst
.. raw:: html

    <iframe src="https://drive.google.com/file/d/1wGSv0POhBqSFntuhLz2p6x_5f_EUw3Ru/preview" width="640" height="480" allow="autoplay"></iframe>
```

#### Source code of the first app
- In the animation frame callback, execute app switching and implement the animation effect of app switching

```c
#include <tp_algo.h>

static bool release_flag;
static void app1_ani_cb(gui_win_t *win)
{
    touch_info_t *tp = tp_get_info();
    static bool press;
    static bool animate1;
    gui_obj_t *obj = (void *)win;
    float sc = 0.05;
    float touch_hold_x_threshold = 300;
    float opacity_max = UINT8_MAX;
    if(tp->pressed)
    {
        press=true;
        win->animate->current_frame=0;
        win->animate->progress_percent=0;
        gui_app_startup(get_app2());
    } 

    if(press&&(!animate1))
    {
        float scale = win->animate->progress_percent*0.95+0.05;
        int opacity = win->animate->progress_percent*(-opacity_max)+opacity_max;
        GET_BASE(win)->opacity_value = opacity;
        if (win->animate->progress_percent==1)
        {
            animate1 = 1;
            press = 0;
            
            gui_app_shutdown(get_app_hongkong());

        }
    }
     //gui_log("press :%d\n",press);

   if (animate1)
   {    
       //if (tp->type ==TOUCH_HOLD_X)
       {
           float scale = _UI_MIN(abs(tp->deltaX), touch_hold_x_threshold)*(-2/touch_hold_x_threshold)+3;
           int opacity = _UI_MIN(abs(tp->deltaX), touch_hold_x_threshold)*(-155.0/touch_hold_x_threshold)+opacity_max;
           opacity = _UI_MIN(abs(tp->deltaX), touch_hold_x_threshold)*(opacity_max/touch_hold_x_threshold);
           GET_BASE(win)->opacity_value = opacity;
            GET_BASE(win)->sx = scale;
           GET_BASE(win)->sy = scale;
       }
       //gui_log("tp->released :%d\n",tp->released);
       if (tp->released)
       {
           animate1 = 0;
           press = 0;
           GET_BASE(win)->opacity_value = opacity_max;
           gui_log("gui_tree_free 2");
           //gui_tree_free(&((gui_app_t *)get_app2())->screen);
       }
   }
}

#include "gui_win.h"
#include "gui_grid.h"

static void app_hongkong_ui_design(gui_app_t *app)
{
    gui_log("app_hongkong_ui_design\n");

#ifndef _WIN32
#include "mem_config.h"
    static bool flag;
    if (!flag)
    {
        memcpy((void *)SPIC2_ADDR, (void *)0x04400000, 0x100000 * 12);
        flag = !flag;
    }

#endif
    gui_win_t *win = gui_win_create(&(app->screen), "win1", 0, 0,gui_get_screen_width(), gui_get_screen_height());
    gui_grid_t *grid = gui_grid_create(win, 0,0,4,2,150,100);
    for (size_t i = 0; i < 12; i++)
    {
        gui_img_t* img =  gui_img_create_from_mem(grid, "img1", MUTE_OFF_BIN, 0,0,0,0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    gui_win_set_animate(win, 1000, -1, app1_ani_cb, win);


}


```
- Source code of the second app
```c
#include <tp_algo.h>
static void app2_ani_cb(gui_win_t *win)
{
   touch_info_t *tp = tp_get_info();
   static bool press;
   static bool animate1;
   gui_obj_t *obj = (void *)win;
   float sc = 0.05;
   float touch_hold_x_threshold = 300;
   float opacity_max = UINT8_MAX;
   GET_BASE(win)->sx = sc;
   GET_BASE(win)->sy = sc ;
    if(tp->pressed)
    {
        press=true;

        gui_app_startup(get_app_hongkong());
    } 
   if(!animate1 && !press)
   {
       float scale = win->animate->progress_percent*0.95+0.05;
       GET_BASE(win)->sx = scale;
       GET_BASE(win)->sy = scale;
       if (win->animate->progress_percent==1)
       {
           animate1 = 1;
           press = 0;
           gui_log("gui_tree_free 1");
           //gui_tree_free(&((gui_app_t *)get_app_hongkong())->screen);
       }
   }
   
   if (animate1)
   {    
       GET_BASE(win)->sx = 1;
       GET_BASE(win)->sy = 1 ;
       if (tp->type ==TOUCH_HOLD_X ||tp->type == TOUCH_RIGHT_SLIDE)
       {
           float scale = _UI_MIN(abs(tp->deltaX), touch_hold_x_threshold)*(-0.95/touch_hold_x_threshold)+1;
           int opacity = _UI_MIN(abs(tp->deltaX), touch_hold_x_threshold)*(-155.0/touch_hold_x_threshold)+opacity_max;
           GET_BASE(win)->sx = scale;
           GET_BASE(win)->sy = scale;
           GET_BASE(win)->opacity_value = 255-opacity;
       }
       gui_log("tp->type :%d\n",tp->type);
       
       if (tp->released)
       {release_flag = tp->released;
           animate1 = 0;
           press = 0;
           GET_BASE(win)->opacity_value = opacity_max;
           GET_BASE(win)->sx = sc;
           GET_BASE(win)->sy = sc;
           GET_BASE(win)->not_show = true;
           gui_app_shutdown(get_app2());
           gui_log("gui_tree_free 238\n");
       }

}
gui_log("GET_BASE(win)->sx :%f\n",GET_BASE(win)->sx);

}
#include "gui_win.h"
#include "gui_grid.h"


static void app2_ui_design(gui_app_t *app)
{
    gui_win_t *win = gui_win_create(&(app->screen), "win2", 0, 0,gui_get_screen_width(), gui_get_screen_height());
    gui_grid_t *grid = gui_grid_create(win, 0,0,4,2,150,100);
    for (size_t i = 0; i < 12; i++)
    {
        gui_img_create_from_mem(grid, "img2", NO_DISTURB_ON_BIN, 0,0,0,0);
    }   
    gui_win_set_animate(win, 1000, -1, app2_ani_cb, win);
}
```

