.. _3D Model:

=========
3D Model
=========

Lite3D is a lightweight, cross-platform 3D graphics rendering library independently developed by Realtek. It supports software rendering and hardware acceleration extensions with the following features:

   - Lightweight: Small code size, easy to integrate.
   - Cross-platform: Compatible with :term:`GUI` frameworks such as HoneyGUI and LVGL, and supports environments like FreeRTOS and Zephyr.
   - Flexible Rendering: Provides a software rendering pipeline with support for extending :term:`GPU` acceleration interfaces.

Lite3D supports loading multiple 3D model formats to meet the needs of different application scenarios:

   - Static Models: Supports loading static 3D models from :file:`.obj` and :file:`.mtl` files, allowing for precise handling of model geometry and material information.
   - Skeletal Animation Models: Supports loading the :file:`.gltf` format, enabling the rendering of 3D models with skeletal animations to deliver more dynamic and richer visual effects in applications.

The engine's workflow is as follows:

.. figure:: https://foruda.gitee.com/images/1755252209828076518/c91dad94_13408154.png
   :width: 800px
   :align: center
   
   Lite3D Workflow


3D Model Asset Processing
-------------------------

OBJ Static Model
~~~~~~~~~~~~~~~~~

OBJ is a widely used static model file format, suitable for scenarios that do not require complex animations, such as product showcases or :term:`UI` decorative elements.

A complete OBJ model typically consists of the following three core components:

1. .obj File

   - This is the main file of the model, storing its geometric data. Its content includes:

     - Vertex coordinates
     - Normal vectors
     - Texture coordinates (UV mapping)
     - Face definitions
   - It must reference the material information in the :file:`.mtl` file.

2. .mtl File (Material Library)

   Defines the optical properties of the model's surfaces. Its content includes:

     - Ambient/Diffuse/Specular colors
     - Refractive index
     - Transparency
     - Illumination model
     - Texture map references

3. Texture Images

   Usually in :term:`PNG` format, these images are used to enhance the surface detail and realism of the model. Common map types include:

     - Diffuse maps
     - Normal maps
     - Specular maps
     - Transparency maps

.. figure:: https://foruda.gitee.com/images/1735113754178839767/916a3f95_13408154.png
   :width: 800px
   :align: center
   
   Example of OBJ Model Components

GLTF Skeletal Animation Model
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GLTF (GL Transmission Format) is an open standard designed for the efficient transmission and loading of 3D scenes and models. It is often hailed as the 'JPEG of 3D' and is particularly suitable for rendering dynamic models that contain skeletal animations.

A complete GLTF model typically consists of the following three core components:

   - :file:`.gltf` file: A JSON-formatted file that describes the scene's hierarchy and constituent elements in a structured manner.
   - :file:`.bin` file: Contains the model's binary data, such as vertex coordinates, normal vectors, and texture coordinates.
   - All referenced texture images: Such as textures in PNG format.

Its core data components include:

   - Scene Graph: A hierarchical structure composed of Nodes, defining the position, rotation, and scale of the model's various parts.
   - Mesh: The model's geometric information, including vertex attributes (coordinates, normals, UVs) and indices.
   - Material: Physically Based Rendering (PBR) properties, such as base color, metallic, roughness, etc.
   - Skin: Defines the key parts of the skeletal animation, containing the hierarchy of bone Joints and their Inverse Bind Matrices.
   - Animation: Stores keyframe data for the transformation (translation, rotation, scale) of nodes (bones) over time.
   - Texture: Image data referenced by materials.


Model Preprocessing
~~~~~~~~~~~~~~~~~~~~~

Before a 3D model can be rendered, it needs to be converted into a binary format. The following is the processing workflow:

1. **Locate Conversion Tools**
   
   Find the following tools in the HoneyGUI installation directory:

     - ``your_HoneyGUI_dir\tool\3D-tool\extract_desc.exe``
     - ``your_HoneyGUI_dir\tool\image-convert-tool\image_converter.py``

2. **Prepare the Model Files**
   
   Copy your model files to the ``tool\3D-tool`` directory.

      - For OBJ models, ensure the directory contains:

         - :file:`.obj` file
         - :file:`.mtl` file
         - All referenced texture images

      - For GLTF models, ensure the directory contains:

         - :file:`.gltf` file
         - :file:`.bin` file
         - All referenced texture images

3. **Generate Descriptor File**
   
   Open a command line terminal in the ``tool\3D-tool`` directory and run:
   
   - Processing OBJ Model

      Run the command: :kbd:`extract_desc_v3.exe your_model.obj` or use the Python version :kbd:`python extract_desc_v3.py your_model.obj` .
      
      The tool will ask if you want to convert textures:
      
      - Press Enter or type ``Y`` - Automatically convert all PNG textures to bin format
      - Type ``N`` - Skip conversion (already converted or no textures needed)

      .. figure:: https://foruda.gitee.com/images/1765767298921057824/1cafcec4_13408154.png
         :width: 800px
         :align: center

         Script Processing
   
      After execution, :file:`desc_xxx.txt` and :file:`desc_xxx.bin` files will be generated. These files contain the parsed obj, mtl data, and the embedded texture data in binary format.

      .. figure:: https://foruda.gitee.com/images/1765768566599235890/a974e84c_13408154.png
         :width: 800px
         :align: center

         Generating Binary Files

   - Processing GLTF Model

      Run the command: :kbd:`extract_desc_v3.exe xxx.gltf` or :kbd:`python extract_desc_v3.py your_model.obj`. This command will also automatically process all referenced texture images.

      After execution, :file:`gltf_desc_xxx.txt` and :file:`gltf_desc_xxx.bin` files will be generated. These files contain the parsed gltf, bin data, and the embedded texture data.


3D Model Generation
-------------------

Create Model
~~~~~~~~~~~~~

Both OBJ and GLTF models can be created using the function ``l3_create_model(void *desc_addr, L3_DRAW_TYPE draw_type, int16_t x, int16_t y, int16_t view_w, int16_t view_h)``.

**Parameter Description:**

- ``desc_addr``: Pointer to the address of the parsed data extracted by the script.
- ``draw_type``: The rendering method for the model:

   - ``L3_DRAW_FRONT_ONLY``: Renders only the front of the model, suitable for scenarios like a butterfly model where the back needs to be hidden.
   - ``L3_DRAW_FRONT_AND_BACK``: Renders both the front and the back of the model, suitable for scenarios where both sides need to be visible, like a prism model.
   - ``L3_DRAW_FRONT_AND_SORT``: Renders the front of the model with sorting, suitable for scenarios with foreground and background occlusion, like a face model.

- ``x``, ``y``: The top-left coordinates of the model's viewport on the screen.
- ``view_w``, ``view_h``: The width and height of the model's viewport.

Transformation Control
~~~~~~~~~~~~~~~~~~~~~~

Global Transformation
^^^^^^^^^^^^^^^^^^^^^
Global transformation applies a uniform matrix transformation to all vertices of the model, commonly used to implement the overall rotation, translation, and scaling of the model.

.. code-block:: c

   void l3_set_global_transform(l3_model_base_t *_this, l3_global_transform_cb cb);


The user needs to implement a callback function of type ``l3_global_transform_cb``, and define the transformation logic within it, such as the ``world`` transformation for world coordinates and the ``camera`` projection for the camera view.

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
Supports applying local transformations to the model.

.. code-block:: c

   void l3_set_face_transform(l3_model_base_t *_this, l3_face_transform_cb cb);

The callback function of type ``l3_face_transform_cb`` can set a different shape transformation for each face of the object, where ``face_index`` specifies the face to be transformed.

**Features:**

+ Supports independent control by face index.
+ Use ``l3_calculator_4x4_matrix(l3_4x4_matrix_t *matrix, float tx, float ty, float tz, l3_4d_point_t point, l3_4d_vector_t vector, float degrees, float scale)`` to generate different matrices for each face, enabling customized local animations.



3D Model Rendering
--------------------

Create Widget
~~~~~~~~~~~~~
The Lite3D library has been integrated into HoneyGUI and is encapsulated as the ``gui_lite3d`` widget. The :cpp:any:`gui_lite3d_create` function can be used to create widgets for 3D models.

Set Click Event
~~~~~~~~~~~~~~~
The :cpp:any:`gui_lite3d_on_click` function can set a click event for the 3D model widget. When a user clicks the model, a callback function is triggered.

Set Animations
~~~~~~~~~~~~~~~
The function :cpp:any:`gui_obj_create_timer` can be used to set animation properties for the 3D model widget, where ``callback`` is the animation update callback function.


Example
--------

.. _3D Butterfly:

3D Butterfly
~~~~~~~~~~~~~

This model consists of 8 rectangular faces, each with a corresponding texture map, and uses the ``L3_DRAW_FRONT_ONLY`` rendering method. By calling the function ``l3_set_face_transform(l3_model_base_t *_this, l3_face_transform_cb cb)``, you can set local transformations for different faces to achieve animation effects.

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

To run this example, enable the Kconfig option via ``menuconfig`` and save to ``win32_sim/.config``. The model consists of 774 triangular surfaces and supports filling with a custom material base color.

.. literalinclude:: ../../../example/widget/3d/example_3d_dog.c
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

This interface consists of 6 3D application icons. By calling the function ``l3_set_face_image(l3_model_base_t *_this, uint8_t face_index, void *image_addr)``, you can replace the texture map of a specified face of the 3D model.

.. literalinclude:: ../../../example/application/screen_410_502/app_3d_applist.c
   :language: c
   :start-after: /* 3d applist demo start*/
   :end-before: /* 3d applist demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/Lite3D/3d_applist.gif" width= "400" /></div>
   <br>


.. _3D Robot:

3D Robot
~~~~~~~~

This model is in GLTF format with skeletal animation. Lite3D plays this animation in a loop by default.

.. literalinclude:: ../../../example/widget/3d/example_3d_robot_gltf.c
   :language: c
   :start-after: /* 3d robot demo start*/
   :end-before: /* 3d robot demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/Lite3D/3d_robot.gif" width= "400" /></div>
   <br>


FPS Benchmark
--------------

The table below shows the frame rate performance of various examples on different chip platforms. The compilation environment uses ``ARMCLANG V6.22`` with the ``-O2 LTO`` compilation option.

.. csv-table:: FPS Benchmark Results
   :header: Chip Model,CPU Frequency,Resolution,:ref:`3D Butterfly`,:ref:`3D Face`,:ref:`3D Dog`,:ref:`3D App List`,:ref:`3D Robot`
   :align: center
   :name: FPS Benchmark Results

   RTL8773E,100MHz,410 x 502,38 FPS,13 FPS,22 FPS,28 FPS,7 FPS
   RTL8773G,200MHz,410 x 502,61 FPS,27 FPS,50 FPS,61 FPS,13 FPS


API
---

.. doxygenfile:: gui_lite3d.h
