Music Player
============

- UI design: `Figma - Music Mobile App UI <https://www.figma.com/community/file/1180142936953078905/music-mobile-app-ui>`_
- Intuitive Three-Layer Design: Navigate effortlessly between three distinct interfaces. The central interface features a sleek display of the current track’s album cover along with essential playback widgets.

- Swipe Navigation: With a simple swipe, transition to the top interface to access your song list.
- Lyric Display: Swipe down to reveal the lyrics interface, which is a full-screen display of lyrics, synchronized with the music.

- Smooth Animation: Enjoy a beautiful and fluid transition between interfaces with unique zoom animation, bringing the album cover to life as you switch between viewing options.


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1753858248785874706/6b965e7d_10737458.gif" width= "400" /></div>
   <br>



Implementation
--------------

Code
~~~~
Function ``static void app_music_ui_design(gui_view_t *view)`` is located in file ``example/application/screen_410_502/app_music.cpp``.

Widgets Tree Design
~~~~~~~~~~~~~~~~~~~
.. figure:: https://foruda.gitee.com/images/1753858294236683166/2d6d6fc0_10737458.jpeg
   :width: 1000px
   :align: center
   :name: Widgets Tree Design
   
   Widgets Tree Design

