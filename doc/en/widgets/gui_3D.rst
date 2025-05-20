=========
3D Model
=========

This widget supports loading 3D models composed of :file:`.obj` and :file:`.mtl` files, handling the geometric and material information of the models. It also supports adding rich animation effects to enhance visual performance.

This widget supports rendering 3D models with two basic geometric face types: rectangular faces and triangular faces. Rectangular faces are suitable for simple geometries like cubes and planes, while triangular faces are used for more complex models like human faces or animals. Users can choose the appropriate face type based on their needs.


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


Processing 3D Models
---------------------

Before using 3D models in HoneyGUI, they need to be converted into binary format. The processing steps are as follows:

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
   
   - The generated :file:`desc.txt` file contains the following:

     - Parsed OBJ data
     - Parsed MTL data
     - Embedded texture data
   
   .. figure:: https://foruda.gitee.com/images/1735114445910760790/2a41eeab_13408154.png
      :width: 800px
      :align: center

      Generating Binary Arrays

.. note::
   Models composed of triangular faces currently do not support texture mapping.


Integration in HoneyGUI
-----------------------

1. **Add Descriptor File to Project**

   - Place the generated :file:`desc.txt` file into the project directory.

2. **Create 3D Widget**

   - Use the :cpp:any:`gui_3d_create` function to create the widget:

   .. code-block:: c

      gui_3d_t *test_3d = gui_3d_create(gui_obj_get_root(), "3d-widget", (void *)_acdesc, 0, 0, 480, 480);


Usage
-----

Creating the Widget
~~~~~~~~~~~~~~~~~~~~
Use :cpp:any:`gui_3d_create` to create the 3D model. The imported ``desc_addr`` file is the parsed data extracted by the script.

Transformation Control
~~~~~~~~~~~~~~~~~~~~~~~

Global Transformation
^^^^^^^^^^^^^^^^^^^^^
Use :cpp:any:`gui_3d_set_global_transform_cb` to apply a global transformation to the 3D model, where ``gui_3d_global_transform_cb`` type callback functions can set the same shape transformation for all faces of the object. For example, ``world`` represents the world coordinate transformation, and ``camera`` represents the camera view projection. Rectangular faces also support setting ``light`` information. For detailed configuration, refer to the :ref:`Coordinate Transformations and Lighting System` section.

Typical use cases:

+ Overall rotation/translation of the model
+ Scene perspective switching

Face Transformation
^^^^^^^^^^^^^^^^^^^^
Use :cpp:any:`gui_3d_set_face_transform_cb` to apply a local transformation to the 3D model, where ``gui_3d_face_transform_cb`` type callback functions can set different shape transformations for each face of the object. The ``face_index`` specifies the face to be transformed.

Features:

+ Supports local animations
+ Supports independent control by face indexs

.. _Coordinate Transformations and Lighting System:

Coordinate Transformations and Lighting System
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

World Transformation
^^^^^^^^^^^^^^^^^^^^^

The initialization function is ``gui_3d_world_inititalize(gui_3d_matrix_t *world, float x, float y, float z, float rotX, float rotY, float rotZ, float scale)``.

+ ``world``: A pointer to the world transformation matrix, which transforms the 3D object from model coordinates to world coordinates.

+ ``x``: The distance of translation along the X-axis, used to determine the object's position in the X direction within the world coordinate system.

+ ``y``: The distance of translation along the Y-axis, used to determine the object's position in the Y direction within the world coordinate system.

+ ``z``: The distance of translation along the Z-axis, used to determine the object's position in the Z direction within the world coordinate system.

+ ``rotX``: The angle of rotation around the X-axis (in degrees).

+ ``rotY``: The angle of rotation around the Y-axis (in degrees).

+ ``rotZ``: The angle of rotation around the Z-axis (in degrees).

+ ``scale``: A uniform scaling factor used to proportionally scale the object in all directions.


Purpose:

1. The world transformation matrix typically handles transforming the model coordinate system to the world coordinate system. For example, if an object is located at the origin of the model coordinate system, it can be moved to any position in the scene and scaled/rotated through world transformation.
2. Performing independent world transformations for each face can achieve localized animations or static displays.
3. Different faces can share the same world matrix, or you can use ``gui_3d_calculator_matrix(gui_3d_matrix_t *matrix, float x, float y, float z, gui_point_4d_t point, gui_vector_4d_t vector, float degrees, float scale)`` to generate different matrices for each face to achieve personalized local transformations.


Camera Transformation
^^^^^^^^^^^^^^^^^^^^^^

The initialization function is ``gui_3d_camera_UVN_initialize(gui_3d_camera_t *camera, gui_point_4d_t cameraPosition, gui_point_4d_t cameraTarget, float near, float far, float fov, float viewPortWidth, float viewPortHeight)``.

+ ``camera``: A pointer to the camera structure, used to initialize camera properties.

+ ``cameraPosition``: The position of the camera in world coordinates.

+ ``cameraTarget``: The target point the camera is directed at, i.e., the focal point of the camera's line of sight.

+ ``near``: The near clipping plane distance, defining the distance from the camera to the near plane of the camera's view frustum. Objects closer than this distance will be clipped.

+ ``far``: The far clipping plane distance, defining the distance from the camera to the far plane of the view frustum. Objects farther than this distance will be clipped.

+ ``fov``: The field of view, usually expressed as a vertical angle (in degrees), defining the openness of the camera, i.e., the opening angle of the camera's view frustum.

+ ``viewPortWidth``: The width of the viewport, defining the horizontal size of the rendering target or window.

+ ``viewPortHeight``: The height of the viewport, defining the vertical size of the rendering target or window.


Purpose:

1. Camera transformation defines the observer's position and direction in the scene, transforming the world coordinate system to the camera coordinate system.
2. By manipulating the camera, different perspectives can be achieved, such as translating the camera position or changing the viewing direction.


Lighting Information
^^^^^^^^^^^^^^^^^^^^^

The initialization function is ``gui_3d_light_inititalize(gui_3d_light_t *light, gui_point_4d_t lightPosition, gui_point_4d_t lightTarget, float included_angle, float blend_ratio, gui_3d_RGBAcolor_t color)``.

+ ``light``: A pointer to the light source structure, used to initialize the properties of the light source.

+ ``lightPosition``: The position of the light source in world coordinates.

+ ``lightTarget``: The target position of the light source, defining the direction of illumination.

+ ``included_angle``: The cone angle of the light (in degrees), represented as angle :math:`\alpha` in the diagram. It determines the illumination range of the spotlight, which corresponds to the outer circle of the spotlight in the diagram.

+ ``blend_ratio``: The ratio of the light blending region, defining the softness of the spotlight's edge. It ranges from 0 to 1 and determines angle :math:`\beta` in the diagram. The value is calculated using the following formula:

   .. math::
   
      β = α (1 - ratio)

   The blending region extends from the inner circle to the outer circle of the spotlight. Within the inner circle, the light intensity is constant, while it gradually diminishes from the inner to the outer circle.

+ ``color``: The color of the light source in RGBA8888 format.

.. figure:: https://foruda.gitee.com/images/1735889400996762341/a4f7e0c8_13408154.png
   :width: 400px
   :align: center

   Example of Spotlight Effect


Purpose:

1. The light source type is a spotlight, and its properties include initial position, light direction, cone angle, blend ratio, and light color.
2. Adjusting lighting locally for each face or object can create different visual styles.


Set Animation
~~~~~~~~~~~~~~
The :cpp:any:`gui_obj_create_timer` function can be used to set animation properties for a 3D object. The ``callback`` parameter is a callback function for animation updates.


Example
--------

.. _3D Butterfly:

3D Butterfly
~~~~~~~~~~~~~

To run this example, enable the macro definition ``CONFIG_REALTEK_BUILD_REAL_BUTTERFLY_3D`` in :file:`menu_config.h`. This model consists of 8 rectangular surfaces, each with its corresponding texture mapping. By calling the function :cpp:any:`gui_3d_set_face_transform_cb()`, you can set local transformations for different surfaces to achieve animation effects.

.. literalinclude:: ../../../example/widget/3d/app_ui_realgui_3d_butterfly.c
   :language: c
   :start-after: /* 3d butterfly demo start*/
   :end-before: /* 3d butterfly demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/butterfly.gif" width= "400" /></div>
   <br>


.. _3D Face:

3D Face
~~~~~~~~

To run this example, enable the macro definition ``CONFIG_REALTEK_BUILD_REAL_BUTTERFLY_3D`` in :file:`menu_config.h`. This model consists of 1454 triangular surfaces, filled with the default material base color.

.. literalinclude:: ../../../example/widget/3d/app_ui_realgui_3d_face.c
   :language: c
   :start-after: /* 3d face demo start*/
   :end-before: /* 3d face demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/face.gif" width= "400" /></div>
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


.. _3D Disc:

3D Disc
~~~~~~~~

To run this example, enable the macro definition ``CONFIG_REALTEK_BUILD_REAL_DISC_3D`` in :file:`menu_config.h`. The model is composed of 133 rectangular surfaces, with the disc part using texture mapping and the rectangular part using color filling.

.. literalinclude:: ../../../example/widget/3d/app_ui_realgui_3d_disc.c
   :language: c
   :start-after: /* 3d disc demo start*/
   :end-before: /* 3d disc demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/disc.gif" width= "400" /></div>
   <br>


FPS Benchmark
--------------

The table below shows the frame rate performance of various examples on different chip platforms. The compilation environment uses ``ARMCLANG V6.22`` with the ``-O3 LTO`` compilation option.

.. csv-table:: FPS Benchmark Results
   :header: Chip Model,CPU Frequency,Resolution,:ref:`3D Butterfly`,:ref:`3D Face`,:ref:`3D Dog`,:ref:`3D Disc`
   :align: center
   :name: FPS Benchmark Results

   RTL8773E,100MHz,410 x 502,40 FPS,13 FPS,20 FPS,10 FPS


API
---

.. doxygenfile:: gui_3d.h