# curtain
## Demo

```eval_rst
.. raw:: html

    <iframe src="https://drive.google.com/file/d/1ldpbh_wWDRebAIMNhPelFBF98Der49An/preview" width="640" height="480" allow="autoplay"></iframe>
```
```c
{
    gui_curtainview_t *ct = gui_curtainview_create(&(app->screen), "ct", 0, 0, 368, 448);
    gui_curtain_t *ct_middle = gui_curtain_create(ct, NULL, 0, 0, 368, 448, CURTAIN_MIDDLE, 1);
    gui_curtain_t *ct_up     = gui_curtain_create(ct, NULL, 0, 0, 368, 448, CURTAIN_UP,     1);
    gui_curtain_t *ct_left   = gui_curtain_create(ct, NULL, 0, 0, 368, 448, CURTAIN_LEFT,   0.65f);
    gui_curtain_t *ct_down   = gui_curtain_create(ct, NULL, 0, 0, 368, 448, CURTAIN_DOWN,   0.5f);
    gui_curtain_t *ct_right  = gui_curtain_create(ct, NULL, 0, 0, 368, 448, CURTAIN_RIGHT,   1);
    
    gui_magic_img_create_from_mem(ct_middle, NULL, CLOCKN_BIN, 0, 0, 0, 0);
    gui_magic_img_create_from_mem(ct_up, NULL, WEATHER_BIN, 0, 0, 0, 0);
    gui_magic_img_create_from_mem(ct_left, NULL, WEATHER_BIN, 0, 0, 0, 0);
    gui_magic_img_create_from_mem(ct_down, NULL, MUSIC_BIN, 0, 0, 0, 0);
    gui_magic_img_create_from_mem(ct_right, NULL, QUICKCARD_BIN, 0, 0, 0, 0);
}
```
## api
```eval_rst
.. doxygenfunction:: gui_curtain_create
.. doxygentypedef:: gui_curtain_enum_t
.. doxygenenum:: CURTAIN_ORIENTATION
```


