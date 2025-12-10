# GUI SVG Widget

SVG widget for displaying scalable vector graphics.

## Features

- Load SVG from memory or file
- Scale support
- Opacity control
- Based on nanosvg library

## Usage

### Create from memory

```c
#include "gui_svg.h"

const char svg_data[] = "<svg>...</svg>";
gui_svg_t *svg = gui_svg_create_from_mem(parent, "my_svg", 
                                          (void *)svg_data, 
                                          sizeof(svg_data) - 1,
                                          10, 10);
gui_svg_set_scale(svg, 2.0f);
gui_svg_set_opacity(svg, 200);
```

### Create from file

```c
#include "gui_svg.h"

gui_svg_t *svg = gui_svg_create_from_file(parent, "my_svg",
                                           "/path/to/image.svg",
                                           10, 10);
```

## API

| Function | Description |
|----------|-------------|
| `gui_svg_create_from_mem` | Create SVG widget from memory |
| `gui_svg_create_from_file` | Create SVG widget from file |
| `gui_svg_set_scale` | Set scale factor |
| `gui_svg_set_opacity` | Set opacity (0-255) |
| `gui_svg_get_width` | Get scaled width |
| `gui_svg_get_height` | Get scaled height |
