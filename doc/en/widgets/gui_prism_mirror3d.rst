========================
Prism Mirror 3D Model
========================

This control supports loading 3D prism mirror models consisting of OBJ and MTL files and supports adding animation effects, which can be displayed on app interfaces. Please read before use: :ref:`Prism Mirror Control Usage Notes`.

.. raw:: html

   <br>
   <div style="text-align: center;">
       <img src="https://docs.realmcu.com/HoneyGUI/image/widgets/prism_mirror.gif" width="400" alt="Image" />
   </div>
   <br>

Running the Prismatic Effect on the Simulator
----------------------------------------------

HoneyGUI Simulator is based on the scons tool and MinGW-w64 toolchain. It can be run and debugged in VScode. For specific environment setup and running instructions, please refer to the :ref:`Get Started` section.

After completing the environment setup for the HoneyGUI Simulator, when you start running it, you will see the default HoneyGUI project in the simulator.In :file:`menu_config.h` ( :file:`your HoneyGUI dir/win32_sim/menu_config.h`), enable the macro definition **CONFIG_REALTEK_BUILD_REAL_PRISM_MIRROR_3D** ;

.. figure:: https://foruda.gitee.com/images/1736838781027776182/64086e10_13406851.jpeg
   :align: center
   :width: 400px

   Simulator Runs the Prism Effect

1. If you need to modify the screen size, open the file :file:`SConscript` under the directory :file:`your HoneyGUI dir/realgui/example/demo/`, and modify the values of :c:macro:`DRV_LCD_WIDTH` and :c:macro:`DRV_LCD_HEIGHT` to the desired pixel values.

.. figure:: https://foruda.gitee.com/images/1727161740835693997/89fd9c57_9218678.png
   :align: center
   :width: 400

   Simulator changes screen size

.. _GUI Load Prism Mirror Model:

GUI Load Prism Mirror Model
-----------------------------
1. Add all necessary files for the prism model to a separate folder, including 6 PNG images, geometric information files for the 3D model (:file:`.obj`), and corresponding material information files (:file:`.mtl`).

   + OBJ file: Stores geometric data for the prism mirror model, including vertices, normals, texture coordinates, and faces.
   + MTL file: Describes the material properties of the prism mirror model, including color, glossiness, transparency, and texture mapping.
   + Image files: Textures used in the model.
   + Packaged executable file for generating prism info descriptor: :file:`extract_desc.exe` and :file:`png2c.py`.

2. Parse the prism mirror model and generate prism mirror information descriptor, using a script to process :file:`extract_desc.exe` obj files

   + In the prism mirror resource package directory, run the following command through the command line: :kbd:`extract_desc.exe prism.obj`. Please modify :file:`prism.obj` to the user's custom file name as needed.
   
   + Generate binary array of images and prism mirror information descriptor: :file:`desc.txt`

3. GUI Load Descriptor

   Place the :file:`desc.txt` file containing obj parsed data, mtl parsed data, and image data into the project directory, and load it in :cpp:any:`gui_prism_mirror3d_create`.

Prism Mirror Control Usage
---------------------------
Create Widget
~~~~~~~~~~~~~~
Create a prism model using the :cpp:any:`gui_prism_mirror3d_create` function. This function requires providing a parent object, name, descriptive data, as well as position and size parameters. The number of faces, automatic rotation, and input sensitivity of the prism can be specified through the optional configuration structure (`prism_mirror3d_comfig_t`). The parameter 'desc_add' file contains parsed data extracted from the script

**Parameters:**

+ ``parent``: The parent object, the new prism model will be attached to this GUI component.
+ ``name``: Used as a name to identify and manage 3D objects.
+ ``desc_add``: descriptor address, containing visualized parsing data.
+ ``x``: The x coordinate in the parent component coordinate system.
+ ``y``: The y coordinate in the parent component coordinate system.
+ ``w``: The width of the control.
+ ``h``: The height of the control.
+ ``config``: A pointer to the configuration structure used to configure features such as face count and rotation.

**Example:**

.. code-block:: c

    gui_obj_t *parent = &(app->screen);
    const char *name = "prism_demo";
    uint16_t x = 100, y = 100, w = 300, h = 300;
    prism_mirror3d_config_t config = {6, true, 0.05f};

    gui_prism_mirror3d_t *prism_demo = gui_prism_mirror3d_create(parent, name, desc_addr, x, y, w, h, &config);

Add Dynamic Effects
~~~~~~~~~~~~~~~~~~~
Use the function :cpp:any:`gui_prism_mirror3d_enter_animate` to add dynamic effects to the prism mirror model, such as auto-rotation and interactive rotation. This function takes the created ``prism_mirror3d`` as a parameter, and by default, the model will rotate around the y-axis automatically.

**Parameters:**

+ ``Prism-mirror3d``: A prism model object that has already been created.

**Example:**

.. code-block:: c

    if (prism_demo) {
        gui_prism_mirror3d_enter_animate(prism_demo);
    }

Add App Switch Effect
~~~~~~~~~~~~~~~~~~~~~
Use the function :cpp:any:`gui_prism_mirror3d_click_switch_app_add_event` to add click event response effects for the prism mirror model, enabling app switching. The parameter ``callback`` is the corresponding callback function.

**Parameters:**

+ ``Prism-mirror3d``: A prism model object that has already been created, waiting to add a click event.
+ ``Callback``: A callback function used to handle the application switching logic after a click event.

**Example:**

.. code-block:: c

    void onSwitchAppCallback(gui_event_t e) {
        //Handling application switching logic
    }

    gui_prism_mirror3d_click_switch_app_add_event(prism_demo, onSwitchAppCallback);

Set Size
~~~~~~~~
Set the size of the prism model using cpp:any:`gui_prism_mirror3d_det_scale`. Adjust the scaling factor to suit the requirements of the scene.

**Parameters:**

+ ``prism_mirror3d``: The prism mirror object.
+ ``scale``: The scale factor.

**Example:**

.. code-block::  c

   gui_prism_mirror3d_set_scale(prism_demo, 1.0f);

Set Location
~~~~~~~~~~~~~
Use cpp:any:`gui_prism_mirror3d_det_position` to set the position of the prism model. This function requires x, y. The z-coordinate is used to define the position of the model in 3D space.

**Parameters:**

+ ``prism_mirror3d``: The prism mirror object.
+ ``x``: The X coordinate.
+ ``y``: The Y coordinate.
+ ``z``: The Z coordinate.

**Example:**

.. code-block::  c   

   gui_prism_mirror3d_set_position(prism_demo, 0, 50, 0);

Set Orientation
~~~~~~~~~~~~~~~~
Use cpp: any to set the orientation of the prism model as cpp:any:`gui_prism_mirror3d_set_rotation_angles`. This feature helps adjust the orientation of the model in the 3D environment.

**Parameters:**

+ ``prism_mirror3d``: The prism mirror object.
+ ``x``: The rotation angle around the X axis.
+ ``y``: The rotation angle around the Y axis.
+ ``z``: The rotation angle around the Z axis.

**Example:**

.. code-block::  c

   gui_prism_mirror3d_set_rotation_angles(prism_demo, 0, 60, 0);

Set Original State
~~~~~~~~~~~~~~~~~~~
Use the cpp: any function to set the original state of the 3D prism model as :cpp:any:`gui_prism_mirror3d_det_raw_date`. This function sets the initial position, camera position, rotation angle, and scaling ratio of the prism in the 3D world. Usually called immediately after the control is initialized to define the initial display state.

**Parameters:**

+ ``prism-mirror3d``: The prism model object to be configured.
+ ``world_position``: A floating-point array of length 3, specifying the x of the prism in the world coordinate system, y. Z coordinate.
+ ``camera_position``: A floating-point array of length 3 that specifies the position of the camera relative to the prism, denoted as x, y. Z coordinate.
+ ``rot_x``: The rotation angle (in degrees) around the X-axis.
+ ``rot_y``: The rotation angle around the Y-axis (in degrees).
+ ``rot_z``: The rotation angle (in degrees) around the Z-axis.
+ ``scale``: The scaling ratio of a prism.

**Example:**

.. code-block::  c

   float raw_world_position[3] = {0, 25, 180};
   float raw_camera_position[3] = {0, 5, 60};
   gui_prism_mirror3d_set_raw_state(prism_demo, raw_world_position, raw_camera_position, 0, 0, 0, 13);

Set Target Status
~~~~~~~~~~~~~~~~~~

Use the cpp: any function to define the target state that the 3D prism model will achieve in animation or interaction. This feature is particularly suitable for creating smooth transition effects, such as changing from one position to another.

**Parameters:**

+ ``prism-mirror3d``: The prism model object to be adjusted.
+ ``world_position``: x in the target world coordinate system, y. Z coordinate (floating point array).
+ ``camera_position``: an array of target position coordinates for the camera, specifying x relative to the prism, y, z。
+ ``rot_x``: The rotation angle of the target around the X-axis.
+ ``rot_y``: The rotation angle of the target around the Y-axis.
+ ``rot_z``: The rotation angle of the target around the Z-axis.
+ ``scale``: The target scaling ratio of a prism.

**Example:**

.. code-block::  c

   float target_world_position[3] = {0, 35, 162};
   float target_camera_position[3] = {0, 0, 80};
   gui_prism_mirror3d_set_target_state(prism_demo, target_world_position, target_camera_position, 0, 0, 0, 14);
   
These two functions enable the prism model to have appropriate views and states during initialization, and ensure that the given target state is achieved during animation or interaction, providing users with a smooth visual experience.

Prism Mirror Configuration
----------------------------

For customizing the behavior of the 3D prism mirror, the following adjustments have been made directly to the configuration settings within the code. This section describes the effective parameters used for disabling automatic rotation and setting touch sensitivity.

Automatic Rotation and Sensitivity Settings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The prism mirror is configured with specific parameters to control its basic properties. Here's an overview of the settings applied:

+ **Number of Faces**: Defines the number of visible faces of the prism. This is set to `6` to have a hexagonal prism shape.

+ **Automatic Rotation**: By default, the prism mirror rotates automatically. In this configuration, automatic rotation is enabled. However, if you want to disable it, this should be done through additional logic in the application code if required, as the default setting here keeps it active.

+ **Touch Sensitivity**: This parameter controls how responsive the prism is to touch inputs. The sensitivity is set to `0.05f`, which specifies a medium level of responsiveness, allowing for smooth user interaction.

**Configuration Code Set:**

.. code-block:: c

    prism_demo->conf.auto_rotation = true;
    prism_demo->conf.face_nums = 6;
    prism_demo->conf.sensitivity = 0.05f;

Configuration Explanation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+ **Face Number**: Adjusts the geometric complexity of the prism mirror, currently supports 6-prism effect.

+ **Auto Rotation**: Enable or disable the feature where the prism rotates automatically. A value of `1` means the feature is active.

+ **Sensitivity**: Controls how the prism responds to touch gestures. Setting this to `0.05f` provides a well-balanced responsivenes

.. _Prism Mirror Control Usage Notes:

Prism Mirror Control Usage Notes
---------------------------------

1. If you use the current default 3D modeling effect, The default demo effect is a 480 by 480 square display design:

   1. All image resources must be in PNG format.

   2. Replace the demo hexagonal prism effect image. Please change the image name to "image1, image2, ..., image6".

   3. Please obtain the conversion file required for the prism information descriptor from the following path:

      + File path: :file:`HoneyGUI\\realgui\\example\\demo\\3d`, required files: :file:`extract_desc.exe` and :file:`png2c.py`.

      + File path: :file:`HoneyGUI\\realgui\\example\\demo\\3d`, required files: :file:`extract_desc.exe` and :file:`png2c.py`.

2. If using a display screen of other proportions, in order to achieve better visual effects, it is necessary to re model it using 3D software and export the corresponding OBJ file (reference: :ref:`Prism Widget to Modify 3D Model`: Create a description file that can be loaded into the GUI (please refer to: ref: GUI loading prism model for specific steps).

.. _Prism Widget to Modify 3D Model:

Prism Widget to Modify 3D Model
-------------------------------
This routine uses Blender software as a demonstration.

1. Use 3D modeling software to open the file: ` prism. obj `, switch view effects, shortcut key: English mode, press the top left corner of the keyboard: kbd:`·`， Select the view and press: kbd:`Bottom`。

.. figure:: https://foruda.gitee.com/images/1736508668989561574/99180bb7_13406851.jpeg
   :align: center
   :width: 700px

   Modify Prism Mirror 3D View

2. In object mode, select all objects for scaling (taking the 410 * 502 screen as an example, with an aspect ratio of 1:1.224), pull up the object, and the custom scaling will appear in the lower left corner. Modify the y value to 1.224.  

.. figure:: https://foruda.gitee.com/images/1736508578410723528/ba011e3a_13406851.jpeg
   :align: center
   :width: 700px

   Scale Prism Mirror 3D

3. Export file

.. figure:: https://foruda.gitee.com/images/1736732505811582125/f24394ef_13406851.jpeg
   :align: center
   :width: 850px

   Export Prism Mirror 3D Object Document

Example
-------
Prism Mirror
~~~~~~~~~~~~~~

.. literalinclude:: ../../../realgui/example/demo/app_ui_realgui_3d_prism_mirror.c
   :language: c
   :start-after: /* 3d prism mirror demo start*/
   :end-before: /* 3d prism mirror demo end*/



API
---

.. doxygenfile:: gui_prism_mirror3d.h