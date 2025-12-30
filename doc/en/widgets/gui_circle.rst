.. _Circle (Geometry Circle):

Geometry Circle
========================

Overview
--------

The Circle widget is a lightweight :term:`GUI` drawing component designed specifically for rendering circle shapes in user interfaces. This component provides a simple and easy-to-use :term:`API` for customizing the circle's center position, radius, color, etc., allowing the creation of various circular :term:`UI` elements.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1763463135933874666/f2c5f5f8_13406851.png" width= "400" /></div>
   <br>

Core Features
-------------

.. list-table::
   :header-rows: 1

   * - Description
     - API 
   * - Create Widget
     - :cpp:any:`gui_circle_create`  
   * - Set Style
     - :cpp:any:`gui_circle_set_style`
   * - Set Position
     - :cpp:any:`gui_circle_set_position` 
   * - Set Radius
     - :cpp:any:`gui_circle_set_radius`
   * - Set Color
     - :cpp:any:`gui_circle_set_color`
   * - Set Opacity
     - :cpp:any:`gui_circle_set_opacity`
   * - Register Click Event Callback
     - :cpp:any:`gui_circle_on_click`
   * - Rotation Transform
     - :cpp:any:`gui_circle_rotate`
   * - Scale Transform
     - :cpp:any:`gui_circle_scale`
   * - Translation Transform
     - :cpp:any:`gui_circle_translate`
   * - Set Radial Gradient
     - :cpp:any:`gui_circle_set_radial_gradient`
   * - Set Angular Gradient
     - :cpp:any:`gui_circle_set_angular_gradient`
   * - Add Gradient Stop
     - :cpp:any:`gui_circle_add_gradient_stop`
   * - Clear Gradient
     - :cpp:any:`gui_circle_clear_gradient`

Circle Characteristics
----------------------

The circle widget has the following geometric characteristics:

- **Center Positioning**: Precisely locate through center point coordinates
- **Radius Control**: Support any size radius value
- **Perfect Circle**: Ensure perfect geometric circular rendering
- **Boundary Handling**: Automatically handle clipping with boundaries

Gradient Fill
-------------

The circle widget supports both radial and angular gradient color filling, enabling rich visual effects.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1767064721463984141/294aaa5b_13406851.png" width= "400" /></div>
   <br>

**Gradient Types**

Supports 2 gradient types:

- **CIRCLE_GRADIENT_RADIAL**: Radial gradient (from center to edge)
- **CIRCLE_GRADIENT_ANGULAR**: Angular gradient (along the circumference)

**How Gradient Works**

- Radial gradient: Colors are linearly interpolated from center to edge
- Angular gradient: Colors are linearly interpolated along the circumference
- Supports up to 8 color stops
- Color stop positions are represented as normalized values from 0.0 to 1.0
- Pixel colors are computed through linear interpolation between adjacent color stops
- Uses ordered dithering algorithm to eliminate banding artifacts on RGB565 displays

**Usage Steps**

1. Create a circle widget
2. Call :cpp:any:`gui_circle_set_radial_gradient` for radial gradient, or :cpp:any:`gui_circle_set_angular_gradient` for angular gradient
3. Call :cpp:any:`gui_circle_add_gradient_stop` to add color stops (minimum 2 required)
4. Optional: Call :cpp:any:`gui_circle_clear_gradient` to clear gradient settings

**Key Points**

- **Radial Gradient**: Ideal for creating glow, sun, and other center-to-edge effects
- **Angular Gradient**: Ideal for creating rainbow rings, color wheels, and other circumferential effects
- **Seamless Loop**: For angular gradients, start and end colors should be identical for seamless color cycling
- **Color Interpolation**: Supports linear interpolation in RGBA color space, including the alpha channel
- **Anti-Banding**: Optimized for RGB565 displays using dithering algorithm to eliminate color banding
- **Performance Optimization**: Minimal performance impact on non-gradient scenarios

Feature Highlights
------------------

- **High Performance**: Utilizes optimized circle drawing algorithms to ensure smooth rendering performance
- **Anti-Aliasing**: Supports anti-aliasing for smooth circular edge effects
- **Flexible Configuration**: Supports custom radius, position, and color
- **Transparency Support**: Supports ARGB color format for semi-transparent circle effects
- **Lightweight**: Minimal memory usage, suitable for embedded systems and resource-limited environments
- **Dynamic Updates**: Allows for runtime dynamic modification of position and style
- **Matrix Transforms**: Supports rotation, scale, and translation matrix transforms to create ellipses and complex transform effects

Use Cases
---------

The circle widget is suitable for the following scenarios:

- **Status Indicators**: Create circular status indicator lights
- **User Avatar**: Implement circular user avatar display
- **Icon Design**: Draw various circular icon elements
- **Progress Indication**: Create circular progress bars or loading animations
- **Button Design**: Implement circular interactive buttons
- **Decorative Elements**: Serve as decorative circular elements in the UI
- **Data Visualization**: Used in pie charts, radar charts, etc., for data visualization components
- **Ellipse Drawing**: Create ellipse shapes using scale transforms
- **Transform Animations**: Leverage rotation, scale, and translation for complex animation effects

Configuration Instructions
--------------------------

To use the circle widget, enable the corresponding macro definition in the configuration file:

Enable the Kconfig option via ``menuconfig``:

.. code-block:: shell

   cd win32_sim
   menuconfig ../Kconfig.gui

Select ``Geometry CIRCLE Demo`` ( ``CONFIG_REALTEK_BUILD_REAL_LITE_CIRCLE`` ), then save to ``win32_sim/.config``.

.. code-block:: c

   #define CONFIG_REALTEK_BUILD_REAL_LITE_CIRCLE 1

Complete Example
----------------

.. literalinclude:: ../../../example/widget/circle/example_gui_circle.c
   :language: c
   :start-after: /* gui circle example start */
   :end-before: /* gui circle example end */

API
---

.. doxygenfile:: gui_circle.h
