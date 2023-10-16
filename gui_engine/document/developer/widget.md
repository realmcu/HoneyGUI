# Widget

## Overview

Code style for UI widget developer 

## Render methods
- prepare 
  
```c
static void prepare(gui_xxx_t *this)
{
    ...
    gui_obj_t *root = GET_BASE(this);
    ...
}
```

- draw 
  
```c
static void draw(gui_xxx_t *this)
{
    ...
    gui_obj_t *root = GET_BASE(this);
    ...
}
```

- end 
  
```c
static void end(gui_xxx_t *this)
{
    ...
    gui_obj_t *root = GET_BASE(this);
    ...
}
```

- destory
  
```c
static void destory(gui_xxx_t *this)
{
    ...
    gui_obj_t *root = GET_BASE(this);
    ...
}
```





