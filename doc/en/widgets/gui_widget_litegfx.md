# LiteGFX

In collaboration with LiteGFX, various cool widgets and smart watch faces have been launched based on the HoneyGUI framework.

## Usage

The widget layer for LiteGFX has been encapsulated, so there's no need to create and configure widgets separately. Simply call the effect creation API.
If running LiteGFX on an emulator, enable the macro `CONFIG_REALTEK_BUILD_LITE_GFX` in `menu_config.h`.

## Example

### Notifications

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723430546161167009/198f4430_9325830.gif"/></div>
<br>

```c

static void design_app_watch_ui(gui_app_t *app)
{
    extern void design_tab_app_notifications(void *parent);
    design_tab_app_notifications(&(app->screen));
}
```

### Energybox

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723444521333503824/5a699e73_9325830.gif"/></div>
<br>

```c

static void design_app_watch_ui(gui_app_t *app)
{
    extern void design_tab_app_energybox(void *parent);
    design_tab_app_energybox(&(app->screen));
}
```

### Prism

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723446841825807274/e17dcd08_9325830.gif"/></div>
<br>

```c

static void design_app_watch_ui(gui_app_t *app)
{
    extern void design_tab_app_prsim_refl(void *parent);
    design_tab_app_prsim_refl(&(app->screen));
}
```

### Digit Clock

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723448952426583702/594b5c97_9325830.gif"/></div>
<br>

```c

static void design_app_watch_ui(gui_app_t *app)
{
    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

    extern void design_tab_watchface_digit_clock(void *parent);
    design_tab_watchface_digit_clock(tab0);
}
```

### Butterfly

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723449031267718568/527bf443_9325830.gif"/></div>
<br>

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

### Flowerfall

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723449064680099900/269758fb_9325830.gif"/></div>
<br>

```c

static void design_app_watch_ui(gui_app_t *app)
{
    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

    extern void design_tab_watchface_flowerfall(void *parent);
    design_tab_watchface_flowerfall(tab0);
}
```

### Windmill

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723449195666990344/7ec4093f_9325830.gif"/></div>
<br>

```c

static void design_app_watch_ui(gui_app_t *app)
{
    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

    extern void design_tab_watchface_windmill(void *parent);
    design_tab_watchface_windmill(tab0);
}
```

## API

Following the examples above, it is recommended that users directly use the effects API provided by the upper layer rather than the widget layer API.

</span>

```eval_rst

.. doxygenfile:: gui_widget_litegfx.h

```
