
##  App切换
### App切换动效示例
- 第一个APP是一个列表, 第二个APP是一个蜂窝网格. 
- 点击第一个APP上的菜单按钮会切换到第二个APP. 
- 在切换时，第一个APP会逐渐变透明直至消失, 第二个APP会逐渐增大直至充满屏幕. 
<br>

<center><img width= "300" src="https://foruda.gitee.com/images/1720769983624603433/05c8f87b_10088396.gif" /></center>
<br>

#### 对应的代码
- 在动画回调函数中,实现了切换过渡动效(缩放和淡入淡出)。
- 当动画结束时, 关闭这个第一个APP。

```eval_rst

.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/menu_in_tab.c
   :language: c
   :start-after: /* app swap animation callback of the first app*/
   :end-before: /* app swap animation configration of the first app*/


```
- 第二个APP的动画在APP启动时就开始播放。
```eval_rst


.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /* app swap animation callback of the next app*/
   :end-before: /* app swap animation configration of the next app*/

```
- 第一个APP的动画在点击事件中开始播放。
- 在点击事件中，同时启动了第二个APP。
```eval_rst


.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/menu_in_tab.c
   :language: c
   :start-after: /* The first app's nimation starts playing on click event*/
   :end-before: /* In the button click callback, execute the next app startup.*/

```





