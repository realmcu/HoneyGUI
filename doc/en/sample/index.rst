=======
Samples
=======

We have provided some example applications to help everyone become familiar with using this environment. The sample program will continue to increase. Use Kconfig to choose the demo via ``menuconfig``:

.. code-block:: shell

   cd win32_sim
   menuconfig ../Kconfig.gui

After saving, the configuration is written to ``win32_sim/.config`` and the build system will compile the selected sample.

.. figure:: https://foruda.gitee.com/images/1718765392647198639/ac4aa231_13408154.png
   :align: center
   :width: 800px

   Configuration Selection


The entry point for any application is:

.. code-block:: c

    GUI_INIT_APP_EXPORT(app_init);



.. toctree::
    :maxdepth: 2

    LiteGFX <LiteGFX/Demo>
    Fruit Ninja <Fruit_Ninja/Fruit_Ninja>
    Music Player <MusicPlayer/Music Player>
    3D Watch Face <WatchfaceKoiClock/KoiClock>
    Weather <Weather/weather>
    Rainbow Analog Clock <Rainbow_analog/rainbow_analog>
    
