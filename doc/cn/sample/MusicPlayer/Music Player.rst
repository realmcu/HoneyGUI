音乐播放器
============

- UI 设计: `Figma - 音乐移动应用 UI <https://www.figma.com/community/file/1180142936953078905/music-mobile-app-ui>`_
- I直观的三层设计：在三个不同的界面之间轻松导航。中央界面展示当前曲目的专辑封面以及基本的播放控件。

- 滑动导航：只需简单滑动，即可切换到顶部界面访问您的歌曲列表。
- 歌词显示：向下滑动可以显示全屏歌词界面，同步显示音乐歌词。

- 流畅动画：享受界面间美妙流畅的过渡，以独特的缩放动画让专辑封面生动起来，让您在各种查看选项间切换自如。


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1732871582054056041/61fa89a6_10088396.gif" width= "400" /></div>
   <br>



实现
--------------

代码
~~~~
函数 ``void app_music_ui_design(gui_obj_t *obj)`` 位于文件 ``realgui/example/screen_454_454/gui_menu/app_music.cpp``。

控件树设计
~~~~~~~~~~~~~~~~~~~
.. figure:: https://foruda.gitee.com/images/1737429147962828506/953bab9f_13408154.png
   :width: 600px
   :align: center
   :name: 控件树设计
   
   控件树设计

