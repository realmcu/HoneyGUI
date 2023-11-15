# Grid
The Grid Widget allows you to display widget set in a grid view.
## Demo
- classic style
  

![classic style](https://foruda.gitee.com/images/1693896763454036220/6c0a498b_10088396.png "grid calssic.PNG")


```cpp
void page_tb_grid(void *parent)
{
    void *array[] = {CLOCK_BIN, EMAIL_BIN, SPORT_BIN, SLEEP_BIN, MAP_BIN};
    gui_grid_t *grid = gui_grid_create(parent, 0, 0, 2, 3, 210, 100);
    gui_grid_style(grid, GRID_CLASSIC);
    for (size_t i = 0; i < 5; i++)
    {
        gui_magic_img_create_from_mem(grid, "1", array[i], 0, 0, 0, 0);
    }
}
```

- scale style
  

[scale style video](https://drive.google.com/file/d/1JuL5OWKgfBuEiSbvaiRGtf1P5muiYE6P/view?usp=sharing)

```cpp
void page_tb_grid(void *parent)
{
    void *array[] = {CLOCK_BIN, EMAIL_BIN, SPORT_BIN, SLEEP_BIN, MAP_BIN};
    gui_grid_t *grid = gui_grid_create(parent, 0, 0, 2, 3, 210, 100);
    gui_grid_style(grid, GRID_SCALE);
    for (size_t i = 0; i < 5; i++)
    {
        gui_magic_img_create_from_mem(grid, "1", array[i], 0, 0, 0, 0);
    }
}
```





## API 


```eval_rst

.. doxygenfunction:: gui_grid_create

.. doxygenfunction:: gui_grid_style
.. doxygenenum:: gui_grid_style
```


-  `gui_grid_create()` 
        
        create a grid widget

|param  | description  |type|
|--|--|--|
|`parent`|the father widget the grid nested in|`void *`|
|`x`|the X-axis coordinate|`int`|
|`y`|the Y-axis coordinate|`int`|
|`row`|Rows number|`int`|
|`col`|Columns number|`int`|
|`gap_col`|gap between two Columns|`int`|
|`gap_row`|gap between two Rows|`int`|
|`return`|the widget object pointer|`gui_grid_t`|

-  `gui_grid_style()` 
        
        config grid style

|param  | description  |type|
|--|--|--|
|`grid`|grid widget pointer|`gui_grid_t *`|
|`style`|3 styles: GRID_CLASSIC/ GRID_SCALE/ GRID_3D |`enum gui_grid_style`|
```c
enum gui_grid_style
{
    GRID_CLASSIC,
    GRID_SCALE,
};
```


