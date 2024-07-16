
##  App switching
### App switching animation sample
- The first app has a list, and the second app has a grid. 
- Clicking on the first app's button will switch to the second app. 
- The first app will gradually become transparent until it disappears when switching, and the second app will enlarge to fill the screen. 
<br>

<center><img width= "300" src="https://foruda.gitee.com/images/1720769983624603433/05c8f87b_10088396.gif" /></center>
<br>

#### Source code
- In the animation frame callback, implement the animation effect of app switching(scaling and fading).
- When the animation is finished, shut down the app.

```eval_rst

.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/menu_in_tab.c
   :language: c
   :start-after: /* app swap animation callback of the first app*/
   :end-before: /* app swap animation configration of the first app*/


```
- The next app's animation starts playing on app starting up.
```eval_rst


.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /* app swap animation callback of the next app*/
   :end-before: /* app swap animation configration of the next app*/

```
- The first app's animation starts playing on click event.
- In the button click callback, execute the next app startup.
```eval_rst


.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/menu_in_tab.c
   :language: c
   :start-after: /* The first app's nimation starts playing on click event*/
   :end-before: /* In the button click callback, execute the next app startup.*/

```





