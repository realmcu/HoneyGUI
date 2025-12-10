# GUI Lottie Widget

Lottie animation widget for displaying JSON-based animations.

## Features

- Load Lottie JSON from memory or file
- Play/Pause/Stop controls
- Loop mode support
- Frame control
- Opacity control

## Usage

### Create from memory

```c
#include "gui_lottie.h"

const char lottie_json[] = "{\"fr\":30,\"ip\":0,\"op\":60,...}";
gui_lottie_t *lottie = gui_lottie_create_from_mem(parent, "my_lottie",
                                                   (void *)lottie_json,
                                                   sizeof(lottie_json) - 1,
                                                   10, 10, 100, 100);
gui_lottie_play(lottie);
```

### Create from file

```c
#include "gui_lottie.h"

gui_lottie_t *lottie = gui_lottie_create_from_file(parent, "my_lottie",
                                                    "/path/to/animation.json",
                                                    10, 10, 100, 100);
gui_lottie_set_loop(lottie, true);
gui_lottie_play(lottie);
```

## API

| Function | Description |
|----------|-------------|
| `gui_lottie_create_from_mem` | Create Lottie widget from memory |
| `gui_lottie_create_from_file` | Create Lottie widget from file |
| `gui_lottie_play` | Start animation |
| `gui_lottie_pause` | Pause animation |
| `gui_lottie_stop` | Stop and reset animation |
| `gui_lottie_set_loop` | Enable/disable loop |
| `gui_lottie_set_frame` | Set current frame |
| `gui_lottie_get_frame` | Get current frame |
| `gui_lottie_get_total_frames` | Get total frame count |
| `gui_lottie_set_opacity` | Set opacity (0-255) |

## Note

This is a basic Lottie implementation. For full Lottie feature support, 
consider integrating a complete Lottie library like rlottie.
