=========
3D Model
=========

The widget supports loading 3D models composed of :file:`.obj` and :file:`.mtl` files, and supports adding animation effects.

GUI Load 3D Model
----------------------
1. Components of a 3D model

   + :file:`.obj` file: Stores the geometric data of the 3D model, including vertices, normals, texture coordinates, faces, etc.
   + :file:`.mtl` file: Describes the material properties of the 3D model, including color, glossiness, transparency, and texture mapping.
   + Image files: Textures used in the model.

   .. figure:: https://foruda.gitee.com/images/1735113754178839767/916a3f95_13408154.png
      :width: 800px
      :align: center

      Example of 3D Model Components

2. Parsing the 3D model and generating a 3D information descriptor

   + Invoke a script to process the :file:`.obj` file.

   .. figure:: https://foruda.gitee.com/images/1735540370568112173/cf1c0126_13408154.png
      :width: 800px
      :align: center

      Script Processing
   
   + Generate a 3D information descriptor, which includes parsed OBJ data, parsed MTL data, and texture maps.

   .. figure:: https://foruda.gitee.com/images/1735114445910760790/2a41eeab_13408154.png
      :width: 800px
      :align: center

      Generating Binary Arrays

3. GUI load descriptor

   Place the desc file into the project directory, and call the ``gui_get_3d_desc()`` function to load it into the ``gui_3d_description_t`` structure.

   **Example:**

   .. code-block:: c

      gui_3d_description_t *desc = gui_get_3d_desc((void *)_acdesc);

   The ``gui_3d_description_t`` structure is defined as follows:

   .. literalinclude:: ../../../realgui/widget/gui_3d/def_3d_common.h
      :language: c
      :start-after: /* gui_3d_description_t start*/
      :end-before: /* gui_3d_description_t end*/
   
   In this context, ``GUI_3D_FACE_TYPE`` represents the face type of a 3D object. Currently, it supports 3D models composed of ``GUI_3D_FACE_RECTANGLE`` (rectangle) or ``GUI_3D_FACE_TRIANGLE`` (triangle).

3D Widget Usage
----------------
Create Widget
~~~~~~~~~~~~~~
Use :cpp:any:`gui_3d_create` to create the 3D model. The imported ``desc`` is the parsed data extracted by the script.

Global Shape Transformation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Use :cpp:any:`gui_3d_set_global_shape_transform_cb` to apply a global transformation to the 3D model, where ``cb`` sets the same shape transformation for all faces of the object. In this function, ``world`` and ``camera`` represent the world coordinate transformation of the 3D object and the camera view projection, respectively. 
Additionally, rectangular faces support the setting of ``light`` information.


Local Shape Transformation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Use :cpp:any:`gui_3d_set_local_shape_transform_cb` to apply a local transformation to the 3D model, where ``cb`` allows setting different shape transformations for each face of the object, and ``face`` specifies the face to be transformed. In this function, ``world`` and ``camera`` represent the world coordinate transformation of the 3D object and the camera view projection, respectively. 
Additionally, rectangular faces support the setting of ``light`` information.

World Transformation
^^^^^^^^^^^^^^^^^^^^^^

The initialization function is ``gui_3d_world_inititalize(gui_3d_matrix_t *world, float x, float y, float z, float rotX, float rotY, float rotZ, float scale)``.

+ ``world``: A pointer to the world transformation matrix, it transforms the 3D object from model coordinates to world coordinates.

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
^^^^^^^^^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^^^^^^^^

The initialization function is ``gui_3d_light_inititalize(gui_3d_light_t *light, gui_point_4d_t lightPosition, gui_point_4d_t lightTarget, float included_angle, float blend_ratio, gui_3d_RGBAcolor_t color)``.

+ ``light``: A pointer to the light source structure, used to initialize the properties of the light source.

+ ``lightPosition``: The position of the light source in world coordinates.

+ ``lightTarget``: The target position of the light source, defining the direction of illumination.

+ ``included_angle``: The cone angle of the light (in degrees),  represented as angle :math:`\alpha` in the diagram. It determines the illumination range of the spotlight, which corresponds to the outer circle of the spotlight in the diagram.

+ ``blend_ratio``: The ratio of the light blending region, defining the softness of the spotlight's edge. It ranges from 0 to 1 and determines angle :math:`\beta` in the diagram. The value is calculated using the following formula:

   .. math::
   
      β = α (1 - ratio)

   The blending region extends from the inner circle to the outer circle of the spotlight. Within the inner circle, the light intensity is constant, while it gradually diminishes from the inner to the outer circle.

+ ``color``: The color of the light source and its transparency.

.. figure:: https://foruda.gitee.com/images/1735889400996762341/a4f7e0c8_13408154.png
   :width: 400px
   :align: center

   Example of Spotlight Effect


Purpose:

1. The light source type is a spotlight, and its properties include initial position, light direction, cone angle, blend ratio, and light color.
2. Adjusting lighting locally for each face or object can create different visual styles.


Set Animation
~~~~~~~~~~~~~~
The :cpp:any:`gui_3d_set_animate` function is used to set animation properties for a 3D object. The ``callback`` parameter is a callback function for animation updates, which will be called when each frame of the animation is updated.


Example
--------
3D Butterfly
~~~~~~~~~~~~~

The model is composed entirely of rectangular faces. By calling :cpp:any:`gui_3d_set_local_shape_transform_cb()`, you can set local transformations for different faces to create animation effects.

.. literalinclude:: ../../../realgui/example/demo/app_ui_realgui_3d.c
   :language: c
   :start-after: /* 3d butterfly demo start*/
   :end-before: /* 3d butterfly demo end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1734070660330786955/61e4ff9d_13408154.gif" width= "400" /></div>
   <br>


3D Prism
~~~~~~~~

The model is composed entirely of rectangular faces. By calling ``gui_3d_light_inititalize()``, you can add lighting effects.

.. code-block:: c

   #include "math.h"
   #include "cube3D/desc.txt"

   static float rot_angle = 0.0f;
   void update_cube_animation()
   {
      rot_angle++;
   }

   static void cube_cb(gui_3d_t *this, size_t face/*face offset*/, gui_3d_world_t *world,
                  gui_3d_camera_t *camera, gui_3d_light_t *light)
   {
      gui_dispdev_t *dc = gui_get_dc();
      gui_3d_matrix_t face_matrix;
      gui_3d_matrix_t object_matrix;

      gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 6, 15), gui_point_4d(0, 0, 0), 1, 32767, 90,
                                    dc->screen_width, dc->screen_height);

      gui_3d_world_inititalize(&object_matrix, 0, 22, 40, 90, 0, 0,
                              10);

      gui_3d_light_inititalize(light, gui_point_4d(0, 22, 45), gui_point_4d(0, 22, 40), 60, 0.6, (gui_3d_RGBAcolor_t){255, 215, 0, 255});

      gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1), rot_angle,
                                    1);
      
      *world = gui_3d_matrix_multiply(face_matrix, object_matrix);

   }
   static void app_ui_design(gui_app_t *app)
   {

      gui_3d_t *test_3d = gui_3d_create(&(app->screen), "3d-widget", (void *)_acdesc, 0, 0, 480, 480);

      gui_3d_set_shape_transform_cb(test_3d, 0, cube_cb);

      gui_3d_set_animate(test_3d, 10000, -1, update_cube_animation, NULL);

      return;

   }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/cube3d.gif" width= "400" /></div>
   <br>


3D Face
~~~~~~~~

The model is composed of 1,454 triangular faces.

.. literalinclude:: ../../../realgui/example/demo/app_ui_realgui_3d_face.c
   :language: c
   :start-after: /* 3d face demo start*/
   :end-before: /* 3d face demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/face.gif" width= "400" /></div>
   <br>


API
---

.. doxygenfile:: gui_3d.h