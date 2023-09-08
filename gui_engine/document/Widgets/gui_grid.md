---
description: The Grid Widget allows you to display widget set in a grid view.
---

# Grid

### Overview <a href="#overview" id="overview"></a>

The Grid Widget allows you to display widget set in a grid view.

### API <a href="#api" id="api"></a>

{% swagger method="get" path="" baseUrl="" summary="gui_grid_create()" %}
{% swagger-description %}
create a grid widget
{% endswagger-description %}

{% swagger-parameter name="parent" type="void *" in="body" required="true" %}
the father widget the grid nested in
{% endswagger-parameter %}

{% swagger-parameter in="body" name="x" type="int" required="true" %}
 the X-axis coordinate
{% endswagger-parameter %}

{% swagger-parameter in="body" name="y" type="int" required="true" %}
 the Y-axis coordinate
{% endswagger-parameter %}

{% swagger-parameter in="body" name="row" type="int" required="true" %}
Rows number
{% endswagger-parameter %}

{% swagger-parameter in="body" name="col" type="int" required="true" %}
 Columns number
{% endswagger-parameter %}

{% swagger-parameter in="body" name="gap_col" type="int" required="true" %}
 gap between two Columns
{% endswagger-parameter %}

{% swagger-parameter in="body" name="gap_row" type="int" required="true" %}
gap between two Rows
{% endswagger-parameter %}

{% swagger-parameter in="body" name="return" type="gui_grid_t *" required="true" %}
 the widget object pointer
{% endswagger-parameter %}
{% endswagger %}



{% swagger method="get" path="" baseUrl="" summary="gui_grid_style()" %}
{% swagger-description %}
config grid style
{% endswagger-description %}

{% swagger-parameter in="body" name="grid" type="gui_grid_t *" required="true" %}
grid widget pointer
{% endswagger-parameter %}

{% swagger-parameter in="body" name="style" type="enum gui_grid_style" required="true" %}
3 styles: GRID_CLASSIC/ GRID_SCALE/ GRID_3D
{% endswagger-parameter %}
{% endswagger %}

```c
enum gui_grid_style
{
    GRID_CLASSIC,
    GRID_SCALE,
    GRID_3D
};
```



### Example <a href="#example" id="example"></a>



* classic style

<figure><img src="https://foruda.gitee.com/images/1693896763454036220/6c0a498b_10088396.png" alt=""><figcaption></figcaption></figure>

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

* scale style

{% embed url="https://drive.google.com/file/d/1JuL5OWKgfBuEiSbvaiRGtf1P5muiYE6P/view?usp=sharing" %}

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

* 3D style

{% embed url="https://drive.google.com/file/d/1bZWN2LKSXdb-HsNS3gSeXscYDeRV26B_/view?usp=sharing" %}

```cpp
void page_tb_grid(void *parent)
{
    void *array[] = {CLOCK_BIN, EMAIL_BIN, SPORT_BIN, SLEEP_BIN, MAP_BIN};
    gui_grid_t *grid = gui_grid_create(parent, 100, 200, 1, 5, 30, 30);
    gui_grid_style(grid, GRID_3D);
    for (size_t i = 0; i < 5; i++)
    {
        gui_magic_img_create_from_mem(grid, "1", array[i], 0, 0, 0, 0);
    }
}
```
