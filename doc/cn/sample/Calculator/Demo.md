#  计算器应用程序

本示例通过演示如何开发一个简单的 “计算器 APP”，来介绍设计开发 UI APP的基本方法和流程。示例中的 “计算器 APP” 其功能和传统的计算器功能一致，应用使用 "button" 控件来获取用户输入，用 "text" 控件来显示输入内容和计算结果。请观看以下演示视频来了解完整功能。

<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/Calculator/calculator.gif" width = "300" /></div>
<br>

## 源文件
为了帮助学习和熟悉基本的开发流程，开发者可以在路径 `gui_engine\example\screen_448_368` 下获取该示例的源文件 `app_calculator.c`。


## 操作步骤
__步骤 1:  声明 APP 的结构体__


APP 结构体保存了 UI 的所有信息，开发者应该使用 APP 名称和 UI 设计函数完成初始化。

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
__步骤 2:  定义 APP ui_design 函数__

APP ui_design 函数包含了复杂 UI 的所有控件创建及其配置。在本示例 APP 中，我们加入一个 "window" 控件，并将所有的 "button" 控件和 "text" 控件都其作为子控件完成整个 APP UI 设计。

```c
static void app_calculator_ui_design(gui_app_t *app)
{
    gui_win_t *win = gui_win_create(&app->screen, "back_win", 0, 0, gui_get_screen_width(),
                                    gui_get_screen_height());

    gui_calculator_create(win, "calculator", 0, 0, 454, 454);
}
```

