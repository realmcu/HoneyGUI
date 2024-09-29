# 趣戴 (LiteGFX)

与趣戴（LiteGFX）合作，基于 HoneyGUI 推出了各种酷炫的控件和智能手表表盘。

## 使用方法

HoneyGUI 已经做好了趣戴特效控件层的封装，因此不需要单独创建和配置控件。只需调用特效创建的API即可。
如果要在模拟器上运行趣戴特效，请在 `menu_config.h` 中通过注释宏`CONFIG_REALTEK_BUILD_GUI_454_454_DEMO`关闭默认的示例，启用宏 `CONFIG_REALTEK_BUILD_LITE_GFX`将趣戴文件加入编译，启用宏`CONFIG_REALTEK_BUILD_REAL_LITEGFX_DEMO`打开趣戴示例，然后进行编译。

## 示例

### 通知

```c

static void design_app_watch_ui(gui_app_t *app)
{
    extern void design_tab_app_notifications(void *parent);
    design_tab_app_notifications(&(app->screen));
}
```
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723430546161167009/198f4430_9325830.gif"/></div>
<br>

### 能量盒

```c

static void design_app_watch_ui(gui_app_t *app)
{
    extern void design_tab_app_energybox(void *parent);
    design_tab_app_energybox(&(app->screen));
}
```
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723444521333503824/5a699e73_9325830.gif"/></div>
<br>


### 棱镜

```c

static void design_app_watch_ui(gui_app_t *app)
{
    extern void design_tab_app_prsim_refl(void *parent);
    design_tab_app_prsim_refl(&(app->screen));
}
```
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723446841825807274/e17dcd08_9325830.gif"/></div>
<br>



### 数字时钟

```c

static void design_app_watch_ui(gui_app_t *app)
{
    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

    extern void design_tab_watchface_digit_clock(void *parent);
    design_tab_watchface_digit_clock(tab0);
}
```
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723448952426583702/594b5c97_9325830.gif"/></div>
<br>



### 蝴蝶

```c

static void design_app_watch_ui(gui_app_t *app)
{
    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

    gui_font_mem_init(SIMKAI_SIZE32_BITS2_FONT_BIN);
    extern void design_tab_watchface_butterfly(void *parent);
    design_tab_watchface_butterfly(tab0);

}
```
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723449031267718568/527bf443_9325830.gif"/></div>
<br>



### 落樱

```c

static void design_app_watch_ui(gui_app_t *app)
{
    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

    extern void design_tab_watchface_flowerfall(void *parent);
    design_tab_watchface_flowerfall(tab0);
}
```
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723449064680099900/269758fb_9325830.gif"/></div>
<br>

### 风车

```c

static void design_app_watch_ui(gui_app_t *app)
{
    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

    extern void design_tab_watchface_windmill(void *parent);
    design_tab_watchface_windmill(tab0);
}
```
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723449195666990344/7ec4093f_9325830.gif"/></div>
<br>



## API

参考上述示例，建议用户直接使用上层提供的特效 API，而非控件层 API。

</span>

