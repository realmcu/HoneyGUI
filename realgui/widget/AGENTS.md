# realgui/widget/ - UI Widget Library

**Purpose:** 21+ GUI components inheriting from gui_obj_t

## Widgets

| Widget | Location | Purpose |
|--------|----------|---------|
| gui_obj | gui_obj/ | Base object (all inherit from this) |
| gui_view | gui_view/ | Screen containers, view descriptors |
| gui_win | gui_win/ | Window management |
| gui_text | gui_text/ | Text rendering |
| gui_img | gui_img/ | Image display (multiple formats) |
| gui_list | gui_list/ | Scrollable lists |
| gui_button | (in gui_view/) | Button widgets |
| gui_canvas | gui_canvas/ | Custom drawing canvas |
| gui_svg | gui_svg/ | SVG vector graphics |
| gui_lottie | gui_lottie/ | Lottie animations |
| gui_lite3d | gui_lite3d/ | 3D rendering (Lite3D) |
| gui_particle | gui_particle/ | Particle effects |
| gui_glass | gui_glass/ | Glass/blur effects |
| gui_video | gui_video/ | Video playback |
| gui_geometry | gui_geometry/ | Shapes (arc, circle, rect) |
| gui_gif | gui_gif/ | GIF animation |
| gui_soccer | gui_soccer/ | 2.5D soccer demo |
| gui_qbcode | gui_qbcode/ | Barcode/QR code |
| gui_menu_cellular | gui_menu_cellular/ | Cellular menu UI |
| gui_gray | gui_gray/ | Grayscale handling |

## File Naming

- Implementation: gui_*.c (e.g., gui_button.c)
- Headers: gui_*/gui_*.h (e.g., gui_button/gui_button.h)

## Anti-Patterns

- Free cached geometry in gui_circle.c:923 - NEVER
- Free cached geometry in gui_rect.c:1047 - NEVER
- Use non-VFS file access in widgets - forbidden