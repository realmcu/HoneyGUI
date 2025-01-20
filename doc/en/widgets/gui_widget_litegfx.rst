.. _Widget_LiteGFX_EN:

========
LiteGFX
========

In collaboration with LiteGFX, various cool widgets and smart watch faces have been launched based on the HoneyGUI framework.

Usage
-----

HoneyGUI has already encapsulated the LiteGFX effect widget layer, so there's no need to create and configure
widgets separately. Simply call the effect creation API.

To run the LiteGFX effects on the simulator, comment out the macro ``CONFIG_REALTEK_BUILD_GUI_454_454_DEMO``
in ``menu_config.h`` to disable the default example. Enable the macro ``CONFIG_REALTEK_BUILD_LITE_GFX`` to
include the LiteGFX files in the compilation. Finally, enable the macro ``CONFIG_REALTEK_BUILD_REAL_LITEGFX_DEMO``
to activate the LiteGFX example, and then compile.

Example
-------

Notifications
~~~~~~~~~~~~~

.. code-block:: c

    static void design_app_watch_ui(gui_app_t *app)
    {
        extern void design_tab_app_notifications(void *parent);
        design_tab_app_notifications(&(app->screen));
    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1723430546161167009/198f4430_9325830.gif" width= "456" /></div>
   <br>

.. centered:: Notifications

Energybox
~~~~~~~~~

.. code-block:: c

    static void design_app_watch_ui(gui_app_t *app)
    {
        extern void design_tab_app_energybox(void *parent);
        design_tab_app_energybox(&(app->screen));
    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1723444521333503824/5a699e73_9325830.gif" width= "456" /></div>
   <br>

.. centered:: Energybox

Prism
~~~~~

.. code-block:: c

    static void design_app_watch_ui(gui_app_t *app)
    {
        extern void design_tab_app_prsim_refl(void *parent);
        design_tab_app_prsim_refl(&(app->screen));
    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1723446841825807274/e17dcd08_9325830.gif" width= "456" /></div>
   <br>

.. centered:: Prism

Digit Clock
~~~~~~~~~~~

.. code-block:: c

    static void design_app_watch_ui(gui_app_t *app)
    {
        gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
        gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

        extern void design_tab_watchface_digit_clock(void *parent);
        design_tab_watchface_digit_clock(tab0);
    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1723448952426583702/594b5c97_9325830.gif" width= "456" /></div>
   <br>

.. centered:: Digit Clock

Butterfly
~~~~~~~~~

.. code-block:: c

    static void design_app_watch_ui(gui_app_t *app)
    {
        gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
        gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

        gui_font_mem_init(SIMKAI_SIZE32_BITS2_FONT_BIN);
        extern void design_tab_watchface_butterfly(void *parent);
        design_tab_watchface_butterfly(tab0);
    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1723449031267718568/527bf443_9325830.gif" width= "456" /></div>
   <br>

.. centered:: Butterfly

Flowerfall
~~~~~~~~~~

.. code-block:: c

    static void design_app_watch_ui(gui_app_t *app)
    {
        gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
        gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

        extern void design_tab_watchface_flowerfall(void *parent);
        design_tab_watchface_flowerfall(tab0);
    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1723449064680099900/269758fb_9325830.gif" width= "456" /></div>
   <br>

.. centered:: Flowerfall

Windmill
~~~~~~~~

.. code-block:: c

    static void design_app_watch_ui(gui_app_t *app)
    {
        gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
        gui_tab_t *tab0 = gui_tab_create(tv, "tab0", 0, 0, 0, 0, 0, 0);

        extern void design_tab_watchface_windmill(void *parent);
        design_tab_watchface_windmill(tab0);
    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1723449195666990344/7ec4093f_9325830.gif" width= "456" /></div>
   <br>

.. centered:: Windmill

API
---

Following the examples above, it is recommended that users directly use the effects API provided by the upper layer rather than the widget layer API.
