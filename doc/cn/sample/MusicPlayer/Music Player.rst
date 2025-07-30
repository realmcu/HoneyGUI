音乐播放器
============

- UI 设计: `Figma - 音乐移动应用 UI <https://www.figma.com/community/file/1180142936953078905/music-mobile-app-ui>`_
- I直观的三层设计：在三个不同的界面之间轻松导航。中央界面展示当前曲目的专辑封面以及基本的播放控件。

- 滑动导航：只需简单滑动，即可切换到顶部界面访问您的歌曲列表。
- 歌词显示：向下滑动可以显示全屏歌词界面，同步显示音乐歌词。

- 流畅动画：享受界面间美妙流畅的过渡，以独特的缩放动画让专辑封面生动起来，让您在各种查看选项间切换自如。


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1753858248785874706/6b965e7d_10737458.gif" width= "400" /></div>
   <br>



实现
--------------

代码
~~~~
函数 ``static void app_music_ui_design(gui_view_t *view)`` 位于文件 ``example/application/screen_410_502/app_music.cpp``。

控件树设计
~~~~~~~~~~~~~~~~~~~
.. figure:: https://foruda.gitee.com/images/1753858294236683166/2d6d6fc0_10737458.jpeg
   :width: 600px
   :align: center
   :name: 控件树设计
   
   控件树设计

