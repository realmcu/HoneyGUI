## 3.1 GUI-APP
  +  The GUI frame diagram is shown in the figure below：
![输入图片说明](https://foruda.gitee.com/images/1669962011556176197/5cf9c62c_10088396.png "app框架.png")
  +  Each APP consists of a thread, a control tree, and event trigger callbacks (optional).
  +  The GUI_SERVER traverses the widgets according to the refresh instruction, renders the frame buffer, executes the trigger callback, schedules the app and other operations.
  +  APP can be installed, opened, closed, uninstalled and switched.



## 3.2 Demo: Develop a "progress arc APP"
###
- sdk\src\app\wearable\gui_engine\example\app_arc.c
- The function of the progress arc app is displaying the arc progress bar dynamically.
- Define the app structure.You need to input the app name and the ui_design function.

```c
/**
 * @brief app stuff
 *
 */
static gui_app_t progress =
{
    .screen =
    {
        .name = "progress",
    },
    .ui_design = progress_ui_design,
};
gui_app_t *get_app_progress(void)
{
    return &progress;
}
```

- Define the ui_design function, which adds a window and the grogress arc. 

```c
static void progress_ui_design(gui_app_t *app)
{
    gui_win_t *win = gui_win_create(&app->screen, "back_win", 0, 0, gui_get_screen_width(),
                                    gui_get_screen_height());
    gui_cb_set(win, win_return_cb, RIGHT_SLIDE_CB);
    create_pie3(&app->screen, 0, 0);
}
static void create_pie3(gui_obj_t *parent, int x, int y)
{
    gui_arc_hl_t *hl = gui_arc_highlight_create(
                           parent,
                           "arc",
                           x + 80,
                           y + 80,
                           60,
                           20,
                           0x1E90FFff,
                           0x1E90FF66,
                           50,
                           160,
                           true);
    GET_BASE(hl)->set_animate(hl, 2000, -1, pie3_cb, hl);
}
```

- Define the callback for the right slide event of the window, which is switching to the launcher app.

```c
static void win_return_cb()
{
    extern gui_app_t *get_launcher_app(void);
    gui_switch_app(get_app_progress(), get_launcher_app());
}
```

- Define the animation callback function of the progress arc, which sets the highlight arc's degree based on the animation progress.

```c
static void pie3_cb(void *p)
{
    gui_arc_hl_t *hl = p;
    float per = hl->base.animate->progress_percent;
    int from = 0;
    int to = 360;
    gui_log("per:%f\n", per);
    if (per < 0.5f)
    {
        hl->highlight->set(hl->highlight, hl->highlight->base_canvas.base.color, 0, (to - from)*per * 2,
                           true);
    }
    else
    {
        hl->highlight->set(hl->highlight, hl->highlight->base_canvas.base.color, 0,
                           (to - from) * (1.0f - per) * 2, true);
    }
}
```
