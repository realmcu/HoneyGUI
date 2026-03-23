# realgui/core/ - Core Event System

**Purpose:** Event system, object model, base definitions

## Key Files

| File | Purpose |
|------|---------|
| def_event.h | Event types: GUI_EVENT_TOUCH_*, GUI_EVENT_KB_*, GUI_EVENT_SYSTEM_* |
| gui_obj.h | Base gui_obj_t structure |
| gui_obj_tree.h | Object tree management |
| gui_obj_event.h | Event callback: void (*gui_event_cb_t)(void *obj, gui_event_t *e) |

## Event Types

- Touch: TOUCH_CLICKED, TOUCH_PRESSED, TOUCH_LONG, TOUCH_SLIDE
- Keyboard: KB_SHORT_PRESSED, KB_LONG_PRESSED
- System: SYSTEM_RESET, SYSTEM_SLEEP, SYSTEM_WAKE

## View Descriptors

    static gui_view_descriptor_t const descriptor = {
        .name = "view_name",
        .pView = &current_view,
        .on_switch_in = callback,
        .on_switch_out = callback,
        .keep = false,
    };
    gui_view_descriptor_register(&descriptor);

## Conventions

- Event struct recently renamed: gui_event_t -> gui_event_code_t for type
- View must be registered before use
- All widgets inherit from gui_obj_t