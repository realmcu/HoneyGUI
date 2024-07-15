##  示例效果: 集成趣戴效果
### 介绍

趣戴科技是一家依托自研LiteGfx框架，充分发挥各类芯片性能，为客户提供跨平台、一站式GUI解决方案及丰富绚丽特效产品的软件服务企业。通过自研2.5D特效框架，模拟实现3D技术并融合了粒子系统物理引擎技术，所有2.5D特效都内嵌于LiteGfx Designer，客户可轻松使用并进行个性化开发，为客户打造与众不同的视觉形象。趣戴科技将不断丰富公司在2.5D技术上的产品储备，助力客户在激烈的市场竞争中脱颖而出。其坚信优秀的视觉设计是提升企业品牌价值和市场竞争力的关键。了解更多信息，请访问：www.qdaytek.com

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1720697969622095349/cabe7d88_1860080.gif " width = "300" /></div>
<br>


<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1720698151241639563/a3744130_1860080.gif " width = "300" /></div>
<br>




### Source File
To help learn and be familiar with the development, you can find all source files you may need in path `sdk\src\app\gui\gui_engine\example\`. The source file for this demostration is `app_calculator.c`, you can find it in the path mentioned for more details.


### Two Steps 
__Step 1:  Declare the app structure__

The app structure saves all the information of ui. Developers should initialize the app structure with the app name and ui design function.

```c
#include <gui_app.h>
static void app_calculator_ui_design(gui_app_t *app);

static gui_app_t calculator =
{
    .screen =
    {
        .name = "calculator",
    },
    .ui_design = app_calculator_ui_design,
};

/*
 * Public API to get app structure
 */
gui_app_t *get_app_calculator(void)
{
    return &calculator;
}
```
__Step 2:  Declare the app ui design function__

The app ui design function adds all the widgets required to form a complex ui. In this example, we add a window widgets and draw the calculator ui. 

```c
static void app_calculator_ui_design(gui_app_t *app)
{
    gui_win_t *win = gui_win_create(&app->screen, "back_win", 0, 0, gui_get_screen_width(),
                                    gui_get_screen_height());

    gui_calculator_create(&app->screen, "calculator", 0, 0, 454, 454);
}
```

