# realgui/engine/ - Rendering Engine

**Purpose:** Graphics rendering, fonts, codecs, post-processing

## Subdirectories

    engine/
    +-- font/        # Font rendering (stb_truetype)
    +-- dc/          # Device context operations
    +-- [root]       # Blit, codecs, post-fx

## Key Components

| Component | Purpose |
|-----------|---------|
| Font engine | stb_truetype-based rendering |
| Blit operations | 2D graphics primitives |
| Codecs | Image/video decoding |
| Post-processing | Effects, filters |

## Anti-Patterns

- Use stb_truetype on untrusted font files - SECURITY RISK
- Don't define __ARM_2D_CFG_FORCED_FIXED_POINT_TRANSFORM__ - forbidden
- Don't use virtual screen flag in Arm2D - forbidden