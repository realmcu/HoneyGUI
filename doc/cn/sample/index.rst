=======
示例
=======

为了帮助用户熟悉使用该环境，我们提供了一些示例应用程序。这些示例程序会不断增加。请通过 Kconfig 选择示例应用：

.. code-block:: shell

   cd win32_sim
   menuconfig ../Kconfig.gui

保存后，配置写入 ``win32_sim/.config``，构建系统会根据选择编译对应示例。


.. figure:: https://foruda.gitee.com/images/1718765392647198639/ac4aa231_13408154.png
   :align: center
   :width: 800px

   配置选择


每个应用程序的入口点如下：

.. code-block:: c

    GUI_INIT_APP_EXPORT(app_init);



.. toctree::
    :maxdepth: 2

    LiteGFX <LiteGFX/Demo>
    水果忍者 <Fruit_Ninja/Fruit_Ninja>
    音乐播放器 <MusicPlayer/Music Player>
    3D 表盘 <WatchfaceKoiClock/KoiClock>
    天气 <Weather/weather>
    彩虹模拟时钟 <Rainbow_analog/rainbow_analog>
