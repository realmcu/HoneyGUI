# Specification FAQ

## Graphics

| Platform  | 8762D  |8772F   |87X2D |PC   |8763E|
|---|---|---|---|---|---|
| RGB565  |  Y | Y  | Y  | Y  | Y  |
|  RGB888 | N  | Y  |  Y |  Y | N  |
|  ARGB8888 |  N | Y  |  Y | Y  | N  |
|  SVG | N  | Y   |  N |  Y  | N  |
| TTF | N  | Y   |  N |  Y  | N  |
| DOT font  |  Y | Y  | Y  | Y  |Y  |
|  Vector Graphics | N  | Y   |  N |  Y  |N |
|  Linear gradient | N  | Y   |  N |  Y  |N |
|  Radial gradient | N  | N   |  N |  Y  |N |

## Memory usage

### 8772F demo

```eval_rst
.. raw:: html

    <div style="text-align: center"><iframe src="https://docs.realmcu.com/HoneyGUI/image/FAQ/8772F_demo.mp4" width="640" height="480" allow="autoplay"></iframe></div><br>
```

The memory consumption statistics of this demo are as follows.
| Module | Cost  |
|---|---|
| Widget  |  14.56KB |
| Framebuffer  |  screenWidth *pixelBytes* Lines |
| Thread stack  |  10KB |

### Widget memory usage

| Widget | Memory(Byte) on ARM SoC | Memory(Byte) on Win_32 SIM |
|---|---|---|
|obj | 52| 88|
|img | 112|178|
|win | 72|112|
|page | 124|184|
|tab | 88|136|
|tabview | 100|160|
|button | 88|160|
|text | 100|176|
|scroll_text | 120|200|
|app | 92|152|
|canvas_arc | 156|264|
|canvas_rect | 64|104|
|canvas | 60|104|
|card | 72|112|
|cardview | 124|176|
|colorwheel | 72|112|
|cube | 748|928|
|curtain | 60|96|
|curtainview | 120|168|
|gallery | 112|184|
|grid | 100|144|
|img_live | 84|144|
|img_scope | 124|192|
|stb_img | 76|144|
|kb | 108|192|
|map | 196|272|
|menu_cellular | 76|120|
|multi_level | 60|104|
|pagelist | 96|160|
|pagelistview | 64|112|
|perspective | 736|920|
|progressbar | 80|136|
|qbcode | 84|136|
|scroll_wheel | 388|696|
|seekbar | 128|216|
|simple_img | 68|120|
|svg | 96|144|
|turn_table | 128|192|
|watch_gradient_spot | 60|96|
|wave | 72|112|
|wheel_list | 64|112|
