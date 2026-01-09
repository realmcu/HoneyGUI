.. _geometry_arc:

Geometry Arc
============

Overview
--------

The Geometry Arc widget is a lightweight :term:`GUI` drawing component designed for rendering arcs and circular shapes in user interfaces. It provides a simple and user-friendly :term:`API` to customize arc properties such as start angle, end angle, line width, and color, addressing various arc drawing requirements.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1766129069269773340/db7572e4_13406851.png" width= "400" /></div>
   <br>

Core Features
-------------

.. list-table::
   :header-rows: 1

   * - Description
     - API 
   * - Create Widget
     - :cpp:any:`gui_arc_create`  
   * - Set Position
     - :cpp:any:`gui_arc_set_position` 
   * - Set Radius
     - :cpp:any:`gui_arc_set_radius`
   * - Set Color
     - :cpp:any:`gui_arc_set_color`
   * - Set Opacity
     - :cpp:any:`gui_arc_set_opacity`
   * - Set Start Angle
     - :cpp:any:`gui_arc_set_start_angle`
   * - Set End Angle
     - :cpp:any:`gui_arc_set_end_angle`
   * - Set Line Width
     - :cpp:any:`gui_arc_set_line_width`
   * - Register Click Event Callback
     - :cpp:any:`gui_arc_on_click`
   * - Rotation Transform
     - :cpp:any:`gui_arc_rotate`
   * - Scale Transform
     - :cpp:any:`gui_arc_scale`
   * - Translation Transform
     - :cpp:any:`gui_arc_translate`
   * - Set Angular Gradient
     - :cpp:any:`gui_arc_set_angular_gradient`
   * - Add Gradient Stop
     - :cpp:any:`gui_arc_add_gradient_stop`
   * - Clear Gradient
     - :cpp:any:`gui_arc_clear_gradient`

Arc Group Features
------------------

.. list-table::
   :header-rows: 1

   * - Description
     - API 
   * - Create Arc Group
     - :cpp:any:`gui_arc_group_create`  
   * - Add Arc to Group
     - :cpp:any:`gui_arc_group_add_arc` 
   * - Set Gradient for Arc
     - :cpp:any:`gui_arc_group_set_gradient`
   * - Add Gradient Stop
     - :cpp:any:`gui_arc_group_add_gradient_stop`

Angle Description
-----------------

The arc uses a standard mathematical coordinate system for angles:

- **0°**: Points to 3 o'clock (due east)
- **90°**: Points to 6 o'clock (due south)
- **180°**: Points to 9 o'clock (due west)
- **270°**: Points to 12 o'clock (due north)

Gradient Fill
-------------

The arc widget supports angular gradient color filling, enabling smooth color transitions along the arc's angular direction.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1767061805476727891/be89d853_13406851.png" width= "400" /></div>
   <br>

**How Gradient Works**

- Gradient colors are linearly interpolated along the arc's angular direction
- Supports up to 8 color stops
- Color stop positions are represented as normalized values from 0.0 to 1.0, where 0.0 corresponds to the start angle and 1.0 corresponds to the end angle
- Pixel colors are computed through linear interpolation between adjacent color stops
- Uses angular gradient approach, providing smooth color transitions along the arc's angular direction

**Usage Steps**

1. Create an arc widget
2. Call :cpp:any:`gui_arc_set_angular_gradient` to set the gradient range
3. Call :cpp:any:`gui_arc_add_gradient_stop` to add color stops (minimum 2 required)
4. Optional: Call :cpp:any:`gui_arc_clear_gradient` to clear gradient settings

**Key Points**

- **Seamless Loop**: For a full circle (0° to 360°), the start and end colors should be identical to achieve seamless color cycling
- **Color Interpolation**: Supports linear interpolation in RGBA color space, including the alpha channel
- **Performance Optimization**: Uses a color lookup table (LUT) for pre-computation, ensuring efficient rendering performance

Features Highlights
-------------------

- **High Performance**: Utilizes optimized rendering algorithms for smooth performance
- **Anti-Aliasing**: Supports anti-aliasing for smooth visual effects
- **Flexible Configuration**: Allows customization of angle range, line width, and color
- **Lightweight**: Minimal memory footprint, suitable for embedded systems and resource-constrained environments
- **Ring Support**: Automatically draws a full circle when start and end angles differ by 360°
- **Matrix Transforms**: Supports rotation, scale, and translation matrix transforms for complex arc effects
- **Batch Rendering**: Arc Group widget enables batch rendering of multiple static arcs for improved performance

Arc Group Widget
----------------

The Arc Group widget is designed for batch rendering multiple static arcs in a single buffer, significantly improving rendering performance by reducing hardware transfer operations.

**Performance Benefits**

- **Reduced Transfer Overhead**: Combines multiple arcs into one buffer, reducing DMA/GPU transfer calls
- **Optimized for Static Content**: Ideal for background arcs that don't change frequently
- **Memory Efficient**: Shares a single buffer for multiple arcs

**Use Cases**

- Activity rings (fitness tracking displays)
- Multi-layer progress indicators
- Dashboard backgrounds with multiple circular elements
- Static decorative arc patterns

**Limitations**

- Maximum 8 arcs per group (configurable via MAX_ARCS_IN_GROUP)
- All arcs in a group share the same buffer and are rendered together
- Best suited for static or infrequently changing content

Use Cases
---------

The arc widget is useful in scenarios such as:

- **Progress Indicators**: Displaying loading progress, battery levels, etc.
- **Dashboard Gauges**: Constructing speedometers, thermometers, etc.
- **Data Visualization**: Showing proportional data, statistical charts
- **UI Decoration**: As decorative elements in the interface
- **State Indicators**: Representing system statuses, connection statuses, etc.
- **Transform Animations**: Leverage rotation, scale, and translation for complex arc animation effects

Configuration Instructions
--------------------------

To use the arc widget, enable the corresponding macro in the configuration file:

Enable the Kconfig option via ``menuconfig``:

.. code-block:: shell

   cd win32_sim
   menuconfig ../Kconfig.gui

Select ``Geometry ARC Demo`` ( ``CONFIG_REALTEK_BUILD_REAL_LITE_ARC`` ), then save to ``win32_sim/.config``.

.. code-block:: c

   #define CONFIG_REALTEK_BUILD_REAL_LITE_ARC 1

Complete Example
----------------

.. literalinclude:: ../../../example/widget/arc/example_gui_arc.c
   :language: c
   :start-after: /* gui arc example start */
   :end-before: /* gui arc example end */

API
-------------

.. doxygenfile:: gui_arc.h

.. doxygenfile:: gui_arc_group.h

