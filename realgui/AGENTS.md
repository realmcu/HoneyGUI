# realgui/ - Core GUI Engine

**Purpose:** Core GUI framework library (not an application)

## Structure

    realgui/
    +-- core/        # Event system, definitions (def_*.h)
    +-- widget/      # 21 UI components
    +-- engine/      # Rendering, fonts, codecs, post-processing
    +-- dc/          # Display controller layer
    +-- input/       # Touch/keyboard handling
    +-- server/      # GUI server components
    +-- 3rd/         # Third-party: Arm2D, box2d, nanovg, stb, h264bsd, Lite3D

## Key Components

| Component | Files | Purpose |
|-----------|-------|---------|
| core/ | 20+ def_*.h | Object model, event types, base structures |
| widget/ | 50+ gui_*.c | All UI widgets |
| engine/ | 40+ | Blit, fonts, codecs, post-fx |
| 3rd/ | 15+ libraries | Arm2D, Lite3D, h264bsd, ezXML |

## Build Integration

- Included via CMakeLists.txt or SConscript in parent
- Linked into: win32_sim, keil_sim, lib/*, zephyr
- Kconfig options in Kconfig.gui for feature selection

## Anti-Patterns

- Edit auto-generated cconfig.h - NEVER
- Modify 3rd-party without understanding - WARNING
- Use printf() instead of gui_log() - forbidden