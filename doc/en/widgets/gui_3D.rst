.. _3D Model:

=========
3D Model
=========

Lite3D is a lightweight, cross-platform 3D graphics rendering library independently developed by Realtek. It supports software rendering and hardware acceleration extensions with the following features:

   - Lightweight: Small code size, easy to integrate.
   - Cross-platform: Compatible with GUI frameworks such as HoneyGUI and LVGL, and supports environments like FreeRTOS and Zephyr.
   - Flexible Rendering: Provides a software rendering pipeline with support for extending GPU acceleration interfaces.

Lite3D supports loading 3D models comprised of :file:`.obj` and :file:`.mtl` files, capable of handling both the geometry and material information of the models. It also supports adding rich animation effects to enhance visual performance. The engine workflow is as follows:

.. figure:: https://foruda.gitee.com/images/1755252209828076518/c91dad94_13408154.png
   :width: 800px
   :align: center
   
   Lite3D Workflow


Components of a 3D Model
------------------------

A complete 3D model consists of three core components:

1. **.obj File**

   - Stores geometric data, including:

     - Vertex coordinates
     - Normal vectors
     - Texture coordinates (UV mapping)
     - Face definitions
   - References material information from the :file:`.mtl` file.

2. **.mtl File (Material Library)**

   - Defines surface properties, including:

     - Ambient/Diffuse/Specular colors
     - Refractive index
     - Transparency
     - Illumination model
     - Texture map references

3. **Texture Images**

   - Usually in PNG format, used for:

     - Diffuse maps
     - Normal maps
     - Specular maps
     - Transparency maps

.. figure:: https://foruda.gitee.com/images/1735113754178839767/916a3f95_13408154.png
   :width: 800px
   :align: center
   
   Example of 3D Model Components


3D Model Preprocessing
--------------------------

Before rendering a 3D model, it needs to be converted into a binary format. The processing flow is as follows:

1. **Locate Conversion Tools**
   
   - Find the following tools in the HoneyGUI installation directory:

     - ``your_HoneyGUI_dir/tool/3D-tool/png2c.py``
     - ``your_HoneyGUI_dir/tool/3D-tool/extract_desc.exe``

2. **Prepare the Model Directory**
   
   - Copy the above tools into the model directory.

   - Ensure the model directory contains:

     - :file:`.obj` file
     - :file:`.mtl` file
     - All referenced texture images

3. **Generate Descriptor File**
   
   - Use the extractor to process the model: ``extract_desc.exe xxx.obj``. This executable will automatically call :file:`png2c.py` to convert all PNG textures into binary arrays.
   
   .. figure:: https://foruda.gitee.com/images/1735540370568112173/cf1c0126_13408154.png
      :width: 800px
      :align: center

      Script Processing
   
   - The generated :file:`desc.txt` and :file:`desc.bin` files contain the following:

     - Parsed obj data
     - Parsed mtl data
     - Embedded texture data
   
   .. figure:: https://foruda.gitee.com/images/1735114445910760790/2a41eeab_13408154.png
      :width: 800px
      :align: center

      Generating Binary Arrays



3D Model Generation
-------------------

Creating a Model
~~~~~~~~~~~~~~~~
Invoke the function ``l3_create_model(void *desc_addr, L3_DRAW_TYPE draw_type, int16_t x, int16_t y, int16_t view_w, int16_t view_h)`` from the Lite3D library to create a 3D model. The imported ``desc_addr`` file contains the parsed data extracted by the script, whereas ``draw_type`` specifies the rendering method for the model. It supports the following three modes:

+ ``L3_DRAW_FRONT_ONLY``: Renders only the front of the model, suitable for scenarios like a butterfly model where the back needs to be hidden.
+ ``L3_DRAW_FRONT_AND_BACK``: Renders both the front and back of the model, suitable for scenarios where both sides need to be visible, like a prism model.
+ ``L3_DRAW_FRONT_AND_SORT``: Renders the front of the model with sorting, suitable for scenarios with foreground and background occlusion, like a face model.

Transformation Control
~~~~~~~~~~~~~~~~~~~~~~

Global Transformation
^^^^^^^^^^^^^^^^^^^^^
Use the function ``l3_set_global_transform(l3_model_t *_this, l3_global_transform_cb cb)`` to perform global transformations on the 3D model. The callback function of type ``l3_global_transform_cb`` can apply the same shape transformation to all faces of the object. Examples include ``world`` coordinate transformations and ``camera`` view projections.

**Typical use cases:**

+ Overall rotation/translation of the model
+ Scene perspective switching

Camera Transformation
************************
The initialization function is ``l3_camera_UVN_initialize(l3_camera_t *camera, l3_4d_point_t cameraPosition, l3_4d_point_t cameraTarget, float near, float far, float fov, float viewPortWidth, float viewPortHeight)``.

+ ``camera``: A pointer to the camera structure, used to initialize camera properties.

+ ``cameraPosition``: The position of the camera in world coordinates.

+ ``cameraTarget``: The target point the camera is directed at, i.e., the focal point of the camera's line of sight.

+ ``near``: The near clipping plane distance, defining the distance from the camera to the near plane of the camera's view frustum. Objects closer than this distance will be clipped.

+ ``far``: The far clipping plane distance, defining the distance from the camera to the far plane of the view frustum. Objects farther than this distance will be clipped.

+ ``fov``: The field of view, usually expressed as a vertical angle (in degrees), defining the openness of the camera, i.e., the opening angle of the camera's view frustum.

+ ``viewPortWidth``: The width of the viewport, defining the horizontal size of the rendering target or window.

+ ``viewPortHeight``: The height of the viewport, defining the vertical size of the rendering target or window.


**Purpose:**

1. Camera transformation defines the observer's position and direction in the scene, transforming the world coordinate system to the camera coordinate system.
2. By manipulating the camera, different perspectives can be achieved, such as translating the camera position or changing the viewing direction.


World Transformation
************************
The initialization function is ``l3_world_initialize(l3_4x4_matrix_t *world, float x, float y, float z, float rotX, float rotY, float rotZ, float scale)``.

+ ``world``: A pointer to the world transformation matrix, which transforms the 3D object from model coordinates to world coordinates.

+ ``x``: The distance of translation along the X-axis, used to determine the object's position in the X direction within the world coordinate system.

+ ``y``: The distance of translation along the Y-axis, used to determine the object's position in the Y direction within the world coordinate system.

+ ``z``: The distance of translation along the Z-axis, used to determine the object's position in the Z direction within the world coordinate system.

+ ``rotX``: The angle of rotation around the X-axis (in degrees).

+ ``rotY``: The angle of rotation around the Y-axis (in degrees).

+ ``rotZ``: The angle of rotation around the Z-axis (in degrees).

+ ``scale``: A uniform scaling factor used to proportionally scale the object in all directions.


**Purpose:**

1. The world transformation defines the model's position and orientation in the world coordinate system, responsible for converting the model coordinate system to the world coordinate system.
2. Through the world transformation matrix, the overall translation, rotation, and scaling of the model can be achieved.


Face Transformation
^^^^^^^^^^^^^^^^^^^^
Use the function ``l3_set_face_transform(l3_model_t *_this, l3_face_transform_cb cb)`` for local transformations on a 3D model. The callback function of type ``l3_face_transform_cb`` can apply different shape transformations to each face of the object, with ``face_index`` specifying the face to transform.

**Features:**

+ Supports independent control by face index.
+ Use ``l3_calculator_4x4_matrix(l3_4x4_matrix_t *matrix, float tx, float ty, float tz, l3_4d_point_t point, l3_4d_vector_t vector, float degrees, float scale)`` to generate different matrices for each face, enabling customized local animations.



3D Model Rendering
--------------------

Create Widget
~~~~~~~~~~~~~~~~
The Lite3D library has been integrated into HoneyGUI and encapsulated into the ``gui_lite3d`` widget. You can create a 3D model widget using the function :cpp:any:`gui_lite3d_create`.

Set Click Events
~~~~~~~~~~~~~~~~~
The function :cpp:any:`gui_lite3d_on_click` can be used to set click events for the 3D model widget. When the user clicks the model, a callback function is triggered.

Set Animations
~~~~~~~~~~~~~~~
The function :cpp:any:`gui_obj_create_timer` can be used to set animation properties for the 3D model widget, where ``callback`` is the animation update callback function.


Example
--------

.. _3D Butterfly:

3D Butterfly
~~~~~~~~~~~~~

This model consists of 8 rectangular faces, each with a corresponding texture map, and uses the ``L3_DRAW_FRONT_ONLY`` rendering method. By calling the function ``l3_set_face_transform(l3_model_t *_this, l3_face_transform_cb cb)``, you can set local transformations for different faces to achieve animation effects.

.. literalinclude:: ../../../example/application/screen_410_502/app_3d_butterfly.c
   :language: c
   :start-after: /* 3d butterfly demo start*/
   :end-before: /* 3d butterfly demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/Lite3D/3d_butterfly.gif" width= "400" /></div>
   <br>


.. _3D Face:

3D Face
~~~~~~~~

This model consists of 1454 triangular faces and is filled with a default material base color. The rendering method used is ``L3_DRAW_FRONT_AND_SORT``.

.. literalinclude:: ../../../example/application/screen_410_502/app_3d_face.c
   :language: c
   :start-after: /* 3d face demo start*/
   :end-before: /* 3d face demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/Lite3D/3d_face.gif" width= "400" /></div>
   <br>


.. _3D Dog:

3D Dog
~~~~~~~~

To run this example, enable the macro definition ``CONFIG_REALTEK_BUILD_REAL_DOG_3D`` in :file:`menu_config.h`. The model consists of 774 triangular surfaces and supports filling with a custom material base color.

.. literalinclude:: ../../../example/widget/3d/app_ui_realgui_3d_dog.c
   :language: c
   :start-after: /* 3d dog demo start*/
   :end-before: /* 3d dog demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/dog.gif" width= "400" /></div>
   <br>


.. _3D App List:

3D App List
~~~~~~~~~~~~

This interface consists of 6 3D application icons. By calling the function ``l3_set_face_image(l3_model_t *_this, uint8_t face_index, void *image_addr)``, you can replace the texture map of a specified face of the 3D model.

.. literalinclude:: ../../../example/application/screen_410_502/app_3d_applist.c
   :language: c
   :start-after: /* 3d applist demo start*/
   :end-before: /* 3d applist demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/Lite3D/3d_applist.gif" width= "400" /></div>
   <br>


FPS Benchmark
--------------

The table below shows the frame rate performance of various examples on different chip platforms. The compilation environment uses ``ARMCLANG V6.22`` with the ``-O3 LTO`` compilation option.

.. csv-table:: FPS Benchmark Results
   :header: Chip Model,CPU Frequency,Resolution,:ref:`3D Butterfly`,:ref:`3D Face`,:ref:`3D Dog`,:ref:`3D App List`
   :align: center
   :name: FPS Benchmark Results

   RTL8773E,100MHz,410 x 502,33 FPS,13 FPS,22 FPS,24 FPS
   RTL8773G,200MHz,410 x 502,58 FPS,24 FPS,46 FPS,56 FPS


API
---

.. doxygenfile:: gui_lite3d.h
