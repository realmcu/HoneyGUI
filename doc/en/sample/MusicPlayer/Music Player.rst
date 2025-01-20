Music Player
============

- UI design: `Figma - Music Mobile App UI <https://www.figma.com/community/file/1180142936953078905/music-mobile-app-ui>`_
- Intuitive Three-Layer Design: Navigate effortlessly between three distinct interfaces. The central interface features a sleek display of the current track’s album cover along with essential playback widgets.

- Swipe Navigation: With a simple swipe, transition to the top interface to access your song list.
- Lyric Display: Swipe down to reveal the lyrics interface, which is a full-screen display of lyrics, synchronized with the music.

- Smooth Animation: Enjoy a beautiful and fluid transition between interfaces with unique zoom animation, bringing the album cover to life as you switch between viewing options.


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1732871582054056041/61fa89a6_10088396.gif" width= "400" /></div>
   <br>



Implementation
--------------

Code
~~~~
Function ``void app_music_ui_design(gui_obj_t *obj)`` is located in file ``realgui/example/screen_454_454/gui_menu/app_music.cpp``.

Widgets Tree Design
~~~~~~~~~~~~~~~~~~~
.. figure:: https://foruda.gitee.com/images/1733109387176750266/2d2ebb9b_10088396.png
   :width: 400px
   :align: center
   :name: Widgets Tree Design
   
   Widgets Tree Design

