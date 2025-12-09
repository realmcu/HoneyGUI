.. _Glass:

=====
Glass
=====

The glass widget is used to simulate the refraction effect of glass on a screen. Glass widgets can move on screen and requires an entire frame buffer.

To reduce time consumption in HoneyGUI, a simplified glass model which doesn't follow the principles of light refraction is applied. The simplified glass model only includes the the difference between the current coordinates and the refracted coordinates, as illustrated in the diagram below:

.. figure:: https://foruda.gitee.com/images/1760423436149037567/32e6dd13_8391010.jpeg
   :width: 400px
   :align: center
   
   Simplified Model 

Glass Data Generation
----------------------

Based on the simplified model above, the rule can be derived that the offset of the pixel points after refraction is only related to the pixels within the range of the glass. Therefore, only a collection of the offset of each point on the glass is needed. In the design of the :ref:`example <glass_example_en>` , the offset is determined by the thickness (refractive) of the glass and the position of the observing point (x_o, y_o) through the following relationship:

.. math::

   offset(x, y) = distortion(x, y) * d(x, y)

Where distortion represents the thickness (refractive index) of each point on the glass, and d(x, y) represents the relationship between the current point on the glass and the observed point (x_o, y_o).

Glass Effect Image Generator User Manual
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

GlassTool: :download:`GlassTool <../GlassTool.zip>`

Software Overview
^^^^^^^^^^^^^^^^^

This software is a Python-based glass effect image generator that supports creating glass effect images in various shapes, currently including circle, rounded rectangle, and ring shapes.

Features
^^^^^^^^

Shape Selection
***************

- **Circle**: Creates circular glass effects
- **Rounded Rectangle**: Creates rounded rectangle glass effects  
- **Ring**: Creates ring-shaped glass effects
- **Custom Path**: Creates stroke curves through path commands
- **Close Shape**: Creates automatically closed custom path shapes
- **Load SVG**: Imports SVG files and applies glass effects

Parameter Adjustment
********************

- **Radius/Size**: Controls the size of the shape
- **Distortion**: Adjusts the distortion strength of the glass effect (0-1)
- **Region**: Controls the effective area range (0-1)

Custom Save Path
****************

- **Path Input**: Specify data save path
- **Confirm Output**: Generate final data after clicking
- **Reset**: Restore settings to default values

Image Operations
****************

- **Real-time Preview**: Updates image in real-time when parameters are adjusted
- **Image Save**: Supports saving generated glass effect data for use in HoneyGUI graphics rendering
- **Motion Effect Preview**: Preview dynamic effects by dragging glass graphics with data

Usage Instructions
^^^^^^^^^^^^^^^^^^

Basic Operations
****************

.. figure:: https://foruda.gitee.com/images/1764729198150245214/585f6f68_8391010.png
   :width: 400px
   :align: center
   
   Tool Window 

1. Select the desired shape type
2. Adjust the corresponding parameter sliders
3. Click :guilabel:`Browse` to specify data save path
4. Click OK to preview display effects and support drag functionality

Vector Graphics
********


**Custom Path Usage Instructions:**

1. Select :guilabel:`Custom Path` shape type
2. Enter drawing commands in the path command input box, or use :guilabel:`Insert Commands` menu to insert predefined commands
3. Set path line width in :guilabel:`Stroke` input box (default 30 pixels)
4. Use :guilabel:`Add` button to add commands to path list
5. Can delete selected commands through :guilabel:`Delete`, clear all commands through :guilabel:`Clear`
6. Click :guilabel:`OK` to generate custom path glass effect
7. Support mouse dragging to adjust position


**Close Shape Usage Instructions:**

1. Select :guilabel:`Close Shape` shape type
2. Input and edit path commands following the same way as custom path
3. System will automatically add end command to ensure proper path closure
4. Click :guilabel:`OK` to generate closed shape glass effect
5. Support mouse dragging and real-time effect updates


**Load SVG Usage Instructions:**

1. Select :guilabel:`Load SVG` type
2. Click :guilabel:`Browse` button to select SVG file
3. Software automatically parses path data in SVG file
4. Adjust :guilabel:`Effect Strength` slider to set glass distortion strength
5. Adjust :guilabel:`Effect Area` slider to set effect area range
6. Click :guilabel:`Confirm` to apply settings and generate SVG glass effect
7. Support drag functionality, can freely adjust SVG graphic position

Usage of Vector
*****************

The software supports three vector graphics creation methods, providing flexible custom path drawing functionality:


**Custom Path**

Custom path allows users to create arbitrary shapes through command sequences:

- **Path Command Input**: Enter drawing commands in the path command input box
- **Command Format Support**:
    - ``move(x, y)`` - Move pen to specified position
    - ``lineto(x, y)`` - Draw straight line to specified position
    - ``bezierto(x1,y1,x2,y2,x,y)`` - Draw cubic Bezier curve
    - ``quadto(x1,y1,x,y)`` - Draw quadratic Bezier curve
    - ``cubicto(x1,y1,x2,y2,x,y)`` - Draw cubic Bezier curve (same as bezierto)
    - ``end`` - Close path (draw line back to start point)
- **Common Commands Insert**: Quickly insert predefined commands through :guilabel:`Insert Commands` menu
- **Line Width Setting**: Set path line width in :guilabel:`Stroke` input box (default 30 pixels)
- **Command Management**: Support adding, deleting, clearing path commands, and displaying all command content
- **Default Example**: Automatically load triangle example path containing move, lineto command sequences


**Close Shape**

Close shape provides path drawing functionality, automatically ensuring path closure:

- **Path Command System**: Uses same command format and syntax as custom path
- **Auto-close**: System automatically detects and adds end command to ensure proper path closure
- **Path Validation**: Automatically validate path validity before generating image, ensuring geometric correctness
- **Command Editing**: Provides complete path command editing functionality, including add, delete, clear operations
- **Help System**: Built-in command format help to guide users in correct path command usage


**Load SVG**

Load SVG functionality supports importing standard SVG files and applying glass effects:

- **File Selection**: Select local SVG files through :guilabel:`Browse` button
- **SVG Parsing**: Automatically parse path elements in SVG files, extract path data
- **Path Conversion**: Convert SVG path commands to internal vector path format
    - Support M/m (move), L/l (line), C/c (cubic Bezier), Q/q (quadratic Bezier), Z/z (close) commands
- **Multi-path Processing**: Automatically process SVG files containing multiple paths, use first valid path
- **Effect Parameters**:
    - **Effect Strength**: Adjust glass effect distortion strength (0-100%)
    - **Effect Area**: Control effect area range (0-100%)
- **Path Display**: Display converted path command sequence in interface after loading
- **Error Handling**: Provide complete file reading and parsing error prompts

Drag Functionality
***********************

- All shapes (including basic graphics and vector graphics) support mouse dragging
- Shapes follow mouse movement in real-time during dragging
- Automatic boundary checking prevents moving out of visible area
- Vector graphics dragging maintains path shape and proportions unchanged
- Real-time glass effect rendering updates during dragging process

Notes
^^^^^

- Drag functionality requires glass data generation first
- Please confirm save path before saving data. If no path is specified, data will be saved in the tool's root directory by default

Usage
-----

.. list-table::
   :header-rows: 1

   * - Description
     - :term:`API`
   * - Create Widget
     - :cpp:any:`gui_glass_create_from_mem`  
   * - Set Attribute 
     - :cpp:any:`gui_glass_set_attribute`
   * - Obtain Height 
     - :cpp:any:`gui_glass_get_height` 
   * - Obtain Width 
     - :cpp:any:`gui_glass_get_width`
   * - Refresh Attribute
     - :cpp:any:`gui_glass_refresh_size`
   * - Position Translation
     - :cpp:any:`gui_glass_translate`

.. _glass_example_en:

Example
-------

.. literalinclude:: ../../../example/widget/glass_effect/example_glass_effect.c
    :language: c
    :start-after: /* gui glass widget example start*/
    :end-before: /* gui glass widget example end*/

    

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1760426386384113377/a1a16aae_8391010.gif" width= "400" /></div>
   <br>


API
---

.. doxygenfile:: gui_glass.h
