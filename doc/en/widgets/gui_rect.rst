.. _Rounded_Rectangle:

Geometry Rounded Rectangle
=============================

Overview
--------

The Rounded Rectangle widget is a lightweight :term:`GUI` drawing component specifically designed for rendering rectangles with rounded corners in user interfaces. It offers a simple and user-friendly :term:`API` that allows customization of the rounded rectangle's size, corner radius, and color, enabling the creation of modern :term:`UI` elements.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1763457468837106189/45d195d0_13406851.png" width= "400" /></div>
   <br>

Core Features
-------------

.. list-table::
   :header-rows: 1

   * - Description
     - API 
   * - Create Widget
     - :cpp:any:`gui_rect_create`  
   * - Set Style
     - :cpp:any:`gui_rect_set_style`
   * - Set Position
     - :cpp:any:`gui_rect_set_position` 
   * - Set Size
     - :cpp:any:`gui_rect_set_size`
   * - Set Color
     - :cpp:any:`gui_rect_set_color`
   * - Set Stroke
     - :cpp:any:`gui_rect_set_stroke`
   * - Clear Stroke
     - :cpp:any:`gui_rect_clear_stroke`
   * - Set Glow
     - :cpp:any:`gui_rect_set_glow`
   * - Clear Glow
     - :cpp:any:`gui_rect_clear_glow`
   * - Set Opacity
     - :cpp:any:`gui_rect_set_opacity`
   * - Register Click Event Callback
     - :cpp:any:`gui_rect_on_click`
   * - Rotation Transform
     - :cpp:any:`gui_rect_rotate`
   * - Scale Transform
     - :cpp:any:`gui_rect_scale`
   * - Translation Transform
     - :cpp:any:`gui_rect_translate`
   * - Set Linear Gradient
     - :cpp:any:`gui_rect_set_linear_gradient`
   * - Add Gradient Stop
     - :cpp:any:`gui_rect_add_gradient_stop`
   * - Clear Gradient
     - :cpp:any:`gui_rect_clear_gradient`

Corner Radius Description
-------------------------

The rounded rectangle supports the same corner radius for all four corners:

- **radius = 0**: Draw a rectangle with sharp corners
- **radius > 0**: Draw a rounded rectangle, with the radius determining the arc of the corners
- **Inset Stroke**: Draws the stroke inside the rectangle without changing widget size

Outer Glow
----------

An optional outer glow can be drawn around the widget. It is a separate layer
painted under the body: the body's own fill, stroke and gradient are untouched,
and changing the glow does not rebuild them.

.. code-block:: c

   gui_color_t glow = gui_rgba(255, 200, 80, 160);
   gui_rect_set_glow(rect, glow, 12);   /* 12 px blur radius */
   gui_rect_clear_glow(rect);

**Parameters**

- The colour's **alpha is the strength**: alpha 0 switches the glow off.
- ``blur_radius`` is in pixels, clamped to 1..32. 0 switches the glow off.
- Changing colour or strength re-paints without rebuilding the mask; changing
  size, corner radius or blur radius rebuilds it.

**How It Works**

The glow mask is an A8 signed-distance field sampled from the same rounded
rectangle distance the body's anti-aliasing uses, so the two edges line up
exactly. The stored value is ``F(d) * (1 - c) / (1 - c * a)``, which composes
with the body's own SRC_OVER instead of multiplying the edge coverage in twice.
Without that compensation the seam between body and glow shows as a dark ring
about one pixel wide.

The mask is stored **split**, not as one bounding box: one corner block drawn
four times through mirroring, plus one horizontal and one vertical band. Only
the ring around the outline is non-zero, so a body's interior costs nothing and
the memory depends on the corner radius and blur radius rather than on the
widget's area. Blocks are shared through the geometry cache, so two rects with
the same corner and blur radius share one set of pixels regardless of colour.

**Limitations**

- The glow may extend past a parent container: it is clipped to the display
  section only, never to the parent's bounds. Hit testing still uses the body.
- When the body leaves the screen, the part of the glow still on screen is not
  erased until something else redraws that area.

Gradient Fill
-------------

The rounded rectangle widget supports linear gradient color filling, enabling smooth color transitions along different directions.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1767064264621594527/6fa7801d_13406851.png" width= "400" /></div>
   <br>

**Gradient Directions**

Supports 4 linear gradient directions:

- **RECT_GRADIENT_HORIZONTAL**: Horizontal gradient (left to right)
- **RECT_GRADIENT_VERTICAL**: Vertical gradient (top to bottom)
- **RECT_GRADIENT_DIAGONAL_TL_BR**: Diagonal gradient (top-left to bottom-right)
- **RECT_GRADIENT_DIAGONAL_TR_BL**: Diagonal gradient (top-right to bottom-left)

**How Gradient Works**

- Gradient colors are linearly interpolated along the specified direction
- Supports up to 8 color stops
- Color stop positions are represented as normalized values from 0.0 to 1.0
- Pixel colors are computed through linear interpolation between adjacent color stops
- Uses ordered dithering algorithm to eliminate banding artifacts on RGB565 displays

**Usage Steps**

1. Create a rounded rectangle widget
2. Call :cpp:any:`gui_rect_set_linear_gradient` to set the gradient direction
3. Call :cpp:any:`gui_rect_add_gradient_stop` to add color stops (minimum 2 required)
4. Optional: Call :cpp:any:`gui_rect_clear_gradient` to clear gradient settings

**Key Points**

- **Color Interpolation**: Supports linear interpolation in RGBA color space, including the alpha channel
- **Anti-Banding**: Optimized for RGB565 displays using dithering algorithm to eliminate color banding
- **Performance Optimization**: Minimal performance impact on non-gradient scenarios

Feature Highlights
------------------

- **High Performance**: Utilizes optimized rendering algorithms for smooth performance
- **Anti-Aliasing**: Supports anti-aliasing for smooth rounded corner effects
- **Flexible Configuration**: Allows customization of size, corner radius, and color
- **Transparency Support**: Uses ARGB color format for semi-transparent effects
- **Lightweight**: Minimal memory footprint, suitable for embedded systems and resource-constrained environments
- **Dynamic Updates**: Supports runtime dynamic modification of position and style
- **Matrix Transforms**: Supports rotation, scale, and translation matrix transforms for complex geometric effects

Use Cases
---------

Rounded Rectangle widgets are applicable in scenarios such as:

- **Modern Buttons**: Create interactive buttons with rounded corner effects
- **Card Layouts**: Implement card-style UI design elements
- **Dialogs and Panels**: Render rounded corner dialogs and information panels
- **Icon Backgrounds**: Serve as background for application icons or functional icons
- **Progress Bar Containers**: Create rounded containers for progress bars
- **List Items**: Implement rounded list items or menu items
- **Animated Elements**: Support animation effects with dynamic position changes
- **Transform Animations**: Leverage rotation, scale, and translation for complex animation effects

Configuration Instructions
--------------------------

To use the rounded rectangle widget, enable the corresponding macro in the configuration file:

Enable the Kconfig option via ``menuconfig``:

.. code-block:: shell

   cd simulator/win32
   menuconfig ../../Kconfig.gui

Select ``Geometry RECT Demo`` ( ``CONFIG_REALTEK_BUILD_REAL_LITE_RECT`` ), then save to ``simulator/win32/.config``.

.. code-block:: c

   #define CONFIG_REALTEK_BUILD_REAL_LITE_RECT 1

Complete Example
----------------

.. literalinclude:: ../../../example/widget/rect/example_gui_rect.c
   :language: c
   :start-after: /* gui rect example start */
   :end-before: /* gui rect example end */

API 
-------------


.. doxygenfile:: gui_rect.h

