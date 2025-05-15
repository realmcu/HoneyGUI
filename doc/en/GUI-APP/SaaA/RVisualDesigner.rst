.. .. raw:: html

..    <style>
..        table.docutils {
..            width: 100%;
..            table-layout: fixed;
..        }
..        table.docutils th, table.docutils td {
..            word-wrap: break-word;
..        }
..    </style>


==============================
Realtek Visual Designer
==============================

Overview
--------

Realtek Visual Designer is a tool to create graphical interface designs
for Realtek series ICs; its currently supported ICs are shown in the
table below.

.. table:: Supported ICs
   :widths: 25 50
   :align: center
   :name: {9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}-Supported_ICs

   +----+---------------+
   | No | Supported ICs |
   +====+===============+
   | 1  | RTL8762D      |
   +----+---------------+
   | 2  | RTL8762G      |
   +----+---------------+
   | 3  | RTL8763E      |
   +----+---------------+
   | 4  | RTL8772G      |
   +----+---------------+
   | 5  | RTL8773E      |
   +----+---------------+
   | 6  | TBD           |
   +----+---------------+

Realtek Visual Designer supports:
  * Drag the widgets from the toolbox and drop them in the Design View.
  * Drag and drop the widget to change its position in the Design View, or modify the position and appearance of the widget via the Property View.
  * Export the user-designed GUI project to ``.bin``, and the ``.bin`` can be programmed into the IC to display the graphical interface.
  * Simulate the GUI project on a PC.

This document mainly consists of:

  * :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Examples`
  * :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Event_Setting`
  * :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Animation_Setting`  
  * :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Function_Panels`
  * :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Resource_Management`
  * :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Menu_Bar`
  * :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Quick_Start_Tutorials`
  * :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_GUI_Demo_Project`

To simplify the document, *Tool* is used below to refer to the *Realtek Visual Designer*.

.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Examples:

Examples
---------------

Example1
~~~~~~~~~~~~~~~

- **Layout**: The left side features three radio buttons, and the right side displays an image.
- **Functionality**: Pressing different buttons on the Evaluation Board (:term:`EVB`) triggers the switching of the image content on the right.

.. figure:: https://foruda.gitee.com/images/1745737972534824178/c62da3af_10088396.png
   :align: center
   :width: 400px
   :name: First UI Effect

   Example1 UI Effect

**Creating the Example1**


1. **Create a New RVD Project**

   - After opening the :term:`RVD` tool, you can see this page.
   - Click the yellow folder icon to create a project.

   .. figure:: https://foruda.gitee.com/images/1745738269558331190/7495790a_10088396.png
      :align: center
      :width: 400px
      :name: Create a New RVD Project

      Create a New RVD Project

2. **Add Image Resources**

   - Add image resources based on the steps shown in the image.

   .. figure:: https://foruda.gitee.com/images/1745738540104414413/e31300d8_10088396.png
      :align: center
      :width: 400px
      :name: Add Image Resources

      Add Image Resources

   - After adding the image resources, the interface displays similarly to the figure below.

   .. figure:: https://foruda.gitee.com/images/1745738620568950463/af89cae9_10088396.png
      :align: center
      :width: 400px
      :name: add images done

      Add images done

3. **Add Three Radio Buttons**:

   - Drag the :guilabel:`RadioButton` widget to the central canvas.
   - Set the background image (:guilabel:`BG Image`) for the radio buttons, with the highlighted image displayed when pressed.


   .. figure:: https://foruda.gitee.com/images/1745808568359598574/8232c321_10088396.png
      :align: center
      :width: 400px
      :name: Radio Buttons Setting

      Radio Buttons Setting

   - Adjust the position and size of the three radio buttons, aligning them on the left.
   - Right-click a radio button to set its size to match the background image.


   .. figure:: https://foruda.gitee.com/images/1745808941071192592/f29f7f07_10088396.png
      :align: center
      :width: 400px
      :name: Adjust the position and size

      Adjust the position and size

4. **Add Two Image Pages**:

   - Drag the :guilabel:`TabView` widget to the canvas.
   - Click the blue arrow on the right side of the canvas to create a second tab.
   - Select :guilabel:`tab0` and :guilabel:`tab1`, and add an image to each tab.

   .. figure:: https://foruda.gitee.com/images/1745809454174368310/c380f8ca_10088396.png
      :align: center
      :width: 400px
      :name: Add New Tab

      Add New Tab



5. **Add Radio Button Selection to Switch Images**:

   - Configure the radio buttons' selection events.
   - In the :menuselection:`Widget Tree`, select :guilabel:`radiobutton0`. 
   - In the :menuselection:`Property` panel's yellow lightning :guilabel:`Event Setting` interface, set the trigger to :guilabel:`OnSelect` and the action to jump to :guilabel:`tab1`.
   - Similarly, set :guilabel:`radiobutton1` to jump to :guilabel:`tab0` and :guilabel:`radiobutton2` to jump to the launcher.

   .. figure:: https://foruda.gitee.com/images/1745809610213583909/3d0065d2_10088396.png
      :align: center
      :width: 400px
      :name: Radio Button Selection Events

      Radio Button Selection Events

6. **Add Keyboard-Based Image Switching**:

   - Drag two :guilabel:`Peripheral Device Key` widgets to the canvas.
   - Configure the key click events.
   - In the :menuselection:`Widget Tree`, select :guilabel:`key0`. 
   - In the :menuselection:`Property` panel's yellow lightning :guilabel:`Event Setting` interface, set the trigger to :guilabel:`OnClick` and the action to jump to :guilabel:`tab1`.
   - Set :guilabel:`key1` to jump to :guilabel:`tab0`.
   - In the :menuselection:`Property` panel's :menuselection:`Property Setting` interface, set :guilabel:`key0`'s ID to 49 and :guilabel:`key1`'s ID to 50 (corresponding to keyboard keys 1 and 2 in the PC environment).

   .. figure:: https://foruda.gitee.com/images/1745809853776415161/ac8f0d54_10088396.png
      :align: center
      :width: 400px
      :name: Key Click Events

      Key Click Events


7. **Preview and Export**:

   - Click the :guilabel:`Export` and :guilabel:`Simulate` options in the top menu to preview the effect.
   - Check the exported files in the directory: ``<project_folder>\Export\root``.


Example2
~~~~~~~~~~~~~~~

- **Left Upper**: Displays two images with mixed effects (transparent and black background).
- **Left Lower**: Shows text in two different font sizes.
- **Right**: Features an image with an up-and-down reciprocating animation.

.. figure:: https://foruda.gitee.com/images/1745739513678951688/bcb3b0f2_10088396.png
   :align: center
   :width: 400px
   :name: Second UI Effect

   Example2 UI Effect

**Creating the Example2**



1. **Create a New RVD Project**

   - After opening the RVD tool, you can see this page.
   - Click the yellow folder icon to create a project.

.. figure:: https://foruda.gitee.com/images/1745739395361023371/1dfd00b4_10088396.png
   :align: center
   :width: 400px
   :name: Create a New RVD Project for Example2

   Create a New RVD Project

2. **Add Image Resources**

   - Please refer to Example1.

3. **Add Four Images**:

   - Drag the :guilabel:`Image` widget from the :guilabel:`Widget` panel to the canvas.
   - Set the :guilabel:`Image` property in the :menuselection:`Property` panel to the specified image resource.
   - Right-click the image widget on the canvas to set its size to match the image resource.
   - Adjust the layout of the image widgets.

4. **Set Image Export Format**:

   - By default, image conversion uses :guilabel:`GlobalSetting` (with :guilabel:`Color Space` set to :guilabel:`RGB565`). 
   - To apply a different conversion for specific image resources (e.g., to retain transparency effects), a new conversion setting must be used for that image resource.
   - Go to :menuselection:`Setting --> Image Convert --> Create` to create a new conversion setting.
   - Configure options such as :guilabel:`Color Space`, :guilabel:`Compress`, and :guilabel:`Mix Alpha Channel`.

   .. figure:: https://foruda.gitee.com/images/1745740074880329292/510c8669_10088396.png
      :align: center
      :width: 400px
      :name: Set Image Export Format

      Set Image Export Format

   - Close the dialog, then in the :guilabel:`Images` window, select the image resource and apply the newly created conversion setting in the :menuselection:`Image Convert Setting` section.
   
   .. figure:: https://foruda.gitee.com/images/1745740381172650681/cbbdb633_10088396.png
      :align: center
      :width: 400px
      :name: Apply the conversion setting

      Apply the conversion setting 

5. **Handle PNG Transparent Images**:

   - For :term:`PNG` images with transparency effects, it is recommended to use the following two settings.
   - Transparent Effect Setting:

     - For images with non-pure black background layers.
     - Set the image conversion format's :guilabel:`Color Space` to :guilabel:`ARGB8565` or :guilabel:`ARGB`.
     - Set the image widget's :guilabel:`Blending Mode` to :guilabel:`SrcOverMode`.

   .. figure:: https://foruda.gitee.com/images/1745745800531690844/7ea2f80b_10088396.png
      :align: center
      :width: 400px
      :name: Transparent Effect Setting

      Transparent Effect Setting

   .. figure:: https://foruda.gitee.com/images/1745745582092188954/316866ba_10088396.png
      :align: center
      :width: 400px
      :name: Blending Mode Setting

      Blending Mode Setting  

   - Pre-mixed Black Background Effect Setting:
   
     - For images with pure black background layers (to optimize file size and display efficiency).
     - Set the image conversion format's :guilabel:`Color Space` to :guilabel:`RGB565` or :guilabel:`RGB`.
     - Check :guilabel:`Mix Alpha Channel`.
     - Set the image widget's :guilabel:`Blending Mode` to :guilabel:`BypassMode`.

   .. figure:: https://foruda.gitee.com/images/1745745835243226253/ecb3a5c0_10088396.png
      :align: center
      :width: 400px
      :name: Black Background Effect Setting

      Black Background Effect Setting    

   - Apply transparent effect to :guilabel:`image3` and pre-mixed black background to :guilabel:`image2`.
   - In the figure below, Effect 1 is the pre-mixed black background effect, and Effect 2 is the transparent effect.

   .. figure:: https://foruda.gitee.com/images/1745741856777234666/786ff313_10088396.png
      :align: center
      :width: 400px
      :name: 2 Effects

      Two Image Effects 


6. **Add Image Animation** (for :guilabel:`image1` with up-and-down reciprocating motion):

   - **Create an Animation**:

     - In the :menuselection:`Animation` menu, set the type to :guilabel:`AdvancedTranslate` and click :guilabel:`Add`.
     - This is a keyframe animation allowing operations at multiple time points.
     - Design the animation to move the image to the bottom of the screen at 30% progress and return to the top at 100% progress.
     - Set :guilabel:`Key Times` to :kbd:`0;0.3;1` (animation start, 30% progress, 100% progress).
     - Set :guilabel:`Values` to :kbd:`0,0;0,300;0,0` (three sets of 2D translations: no horizontal movement, vertical movement of 300 pixels down, then back up).
     - Set :guilabel:`Duration` to the animation cycle duration (in milliseconds). Set :guilabel:`Repeat` to :kbd:`0` for infinite looping.
   
   .. figure:: https://foruda.gitee.com/images/1745747629263669367/c7c911d5_10088396.png
      :align: center
      :width: 400px
      :name: Create an Animation

      Create an Animation

   - **Apply Animation to Image**:

     - Select :guilabel:`image1` and go to the :menuselection:`Property` panel.
     - In the yellow lightning :guilabel:`Event Setting` interface, set the trigger to :guilabel:`OnLoad` (triggered when the image widget is displayed).
     - Click :guilabel:`Add Event`, set the :guilabel:`Action` to :guilabel:`Animation`, and select the newly created animation (:guilabel:`animate0`).

   .. figure:: https://foruda.gitee.com/images/1745753304113520373/abcbabfa_10088396.png
      :align: center
      :width: 400px
      :name: Apply Animation to Image

      Apply Animation to Image   

7. **Add Two Text Boxes**:

   - Drag the :guilabel:`Text` widget from the :menuselection:`Widget` panel to the canvas.
   - Set the text content in the :menuselection:`Property` panel's :menuselection:`Content --> Text` field.
   - Configure the font in :menuselection:`Appearance --> Font`. By default, no font is set.

   .. figure:: https://foruda.gitee.com/images/1745754152945794898/312ceea5_10088396.png
      :align: center
      :width: 400px
      :name: Text setting

      Text Setting
   
   - **Create a Font Setting**:

     - Go to :menuselection:`Setting --> Font Convert --> Create` to create a new conversion setting.
     - Configure the font file, font size, anti-aliasing level, and character range.
     - For :guilabel:`Text type` set to :guilabel:`Random`, specify the :guilabel:`Code Page` (e.g., :guilabel:`CP037` for English, :guilabel:`CP936` for Chinese).
     - For :guilabel:`Text type` set to :guilabel:`Range`, specify the :guilabel:`Unicode Range`. :guilabel:`Code Page` is optional and, if set, combines with the range.
     - Available fonts include PC system fonts and user-added fonts.
     
      .. figure:: https://foruda.gitee.com/images/1745754869649440065/606a2221_10088396.png
         :align: center
         :width: 400px
         :name: Font Setting

         Font Setting     
     
     - If your font size is large and the number of characters to display is small, using the Range method is an effective optimization strategy for reducing the size of exported resource files.
     - For example, the configuration in the figure below is used for a text widget that only needs to display digits and a colon, with Unicode ranges of 0x0030-0x003A and 0x003A-0x003B (Start Unicode to End Unicode + 1).

      .. figure:: https://foruda.gitee.com/images/1745756420938128273/7afd8a6c_10088396.png
         :align: center
         :width: 400px
         :name: Font Setting optimization

         Font Setting optimization 

   - **Add a Custom Font**:

     - In the :menuselection:`Fonts` section, click the plus sign to add a TTF font file.
     - The added font appears at the top of the :guilabel:`Font` dropdown in the font settings.

      .. figure:: https://foruda.gitee.com/images/1745756985069687194/c26b25cf_10088396.png
         :align: center
         :width: 400px
         :name: Add a Custom Font

         Add a Custom Font
     

8. **Preview and Export**:

   - Click the :menuselection:`Export` and :menuselection:`Simulate` options in the top menu to preview the effect.
   - Check the exported files in the directory: ``<project_folder>\Export\root``.


Flashing to EVB
~~~~~~~~~~~~~~~

To view the UI effects on the EVB, the RVD export pack need to be flashed.

1. **Pre-Flashing the RVD Project firmware for EVB**

   - The pre-download operation needs to be performed once. The downloaded firmware can parse and display the RVD exported package.
   - Different EVBs and chips correspond to different firmware, which can be found in the corresponding SDK along with the firmware projects.

2. **Flashing the RVD Exported Package**

   - Check the exported folder: ``<project_folder>\Export\root``.
   - To package the root folder using the image packaging tool corresponding to the EVB, you generally need to pay attention to the address settings or flash partition configuration. 
   - Refer to the packaging tool's documentation for detailed operational instructions. The image packaging tool can typically be found in the SDK corresponding to the EVB.
   - Flash the file.
   - Restart the EVB to view the screen display effect.

.. figure:: https://foruda.gitee.com/images/1745833726498330826/59e236a8_10088396.jpeg
   :align: center
   :width: 400px
   :name: screen display effect

   screen display effect




.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Function_Panels:

Function Panels
---------------

Toolkit/Widgets
~~~~~~~~~~~~~~~

- Non-containerized widget

  - Can be used as the parent of other widgets.
  - There is a coordinate-following relationship between parent and
    child widgets.
  - Visible when the child widget is out of range of the parent widget.

- Container widget

  - Can be used as the parent of other widgets.
  - There is a coordinate-following relationship between parent and
    child widgets.
  - Visible when the child widget is out of range of the parent widget.
  - Can drag and drop a widget from the toolbox into the container
    widget.

This section lists the properties supported by the widget in tables and
marks with **Y** or **N** to indicate whether the IC supports them or not.

Non-containerized Widget
^^^^^^^^^^^^^^^^^^^^^^^^

Text
''''

Used only for text display and does not support user input. The
properties are shown in the table below.

.. table:: Text Widget Properties
   :align: center
   :name: {9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}-Table_Text_Widget_Properties
   :width: 100%

   +-------------------+-----------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Property          | Description                                                                                         | 8762D/8763E | 8762G/8772G | TBD |
   +===================+=====================================================================================================+=============+=============+=====+
   | Name              | Widget name.                                                                                        |      Y      |      Y      |  Y  |
   +-------------------+-----------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Height)     | Widget height.                                                                                      |      Y      |      Y      |  Y  |
   +-------------------+-----------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)      | Widget width.                                                                                       |      Y      |      Y      |  Y  |
   +-------------------+-----------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | X                 | Horizontal coordinate relative to the parent widget.                                                |      Y      |      Y      |  Y  |
   +-------------------+-----------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Y                 | Vertical coordinate relative to the parent widget.                                                  |      Y      |      Y      |  Y  |
   +-------------------+-----------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Text              | Display text.                                                                                       |      Y      |      Y      |  Y  |
   +-------------------+-----------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Display Mode      | Long text (text content beyond the widget’s range) display mode with the following supported types. |      Y      |      Y      |  Y  |
   |                   |                                                                                                     |             |             |     |
   |                   | truncate: Truncated display mode;                                                                   |             |             |     |
   |                   |                                                                                                     |             |             |     |
   |                   | verticalscroll: Vertical scrolling display mode;                                                    |             |             |     |
   |                   |                                                                                                     |             |             |     |
   |                   | horizontalscroll: Horizontal scrolling display mode.                                                |             |             |     |
   +-------------------+-----------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Font              | Font setting, please refer to :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Font_Convert_Setting`.   |      Y      |      Y      |  Y  |
   +-------------------+-----------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Font Color (RGBA) | Font color setting, use RGBA.                                                                       |      Y      |      Y      |  Y  |
   +-------------------+-----------------------------------------------------------------------------------------------------+-------------+-------------+-----+

Button
''''''

Clickable widget with text and background image. The properties are
shown in the table below.

.. table:: Button Widget Properties
   :align: center
   :width: 100%

   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Property                | Description                                                                                                              | 8762D/8763E | 8762G/8772G | TBD |
   +=========================+==========================================================================================================================+=============+=============+=====+
   | Name                    | Widget name.                                                                                                             |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Height)           | Widget height.                                                                                                           |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)            | Widget width.                                                                                                            |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | X                       | Horizontal coordinate relative to the parent widget.                                                                     |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Y                       | Vertical coordinate relative to the parent widget.                                                                       |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Text                    | Displayed text.                                                                                                          |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Text X                  | Horizontal coordinate relative to the Button widget.                                                                     |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Text Y                  | Vertical coordinate relative to the Button widget.                                                                       |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Display Mode            | Horizontal or Vertical display.                                                                                          |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Font                    | Font setting, please refer to :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Font_Convert_Setting`.                        |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Text Color (RGB)        | Text color setting, use RGB.                                                                                             |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Transition              | Image transition mode with the following options:                                                                        |      N      |      Y      |  Y  |
   |                         |                                                                                                                          |             |             |     |
   |                         | normal: No effect                                                                                                        |             |             |     |
   |                         |                                                                                                                          |             |             |     |
   |                         | fade: Fade-in/out                                                                                                        |             |             |     |
   |                         |                                                                                                                          |             |             |     |
   |                         | scale: Scaling                                                                                                           |             |             |     |
   |                         |                                                                                                                          |             |             |     |
   |                         | fadeScale: Fade-in/out and scaling                                                                                       |             |             |     |
   |                         |                                                                                                                          |             |             |     |
   |                         | Note: Set the transition mode is effective only if set the default and highlight background image, otherwise all normal. |             |             |     |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | BG Image (Default)      | Default background image.                                                                                                |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | BG Image (Highlight)    | Selected/Highlight background image.                                                                                     |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | BG Image Rotation Angle | Background image rotation angle, range: 0~360 degree.                                                                    |      Y      |      Y      |  Y  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+


RadioButton
''''''''''''

- A radio button is a UI element that allows users to select one option from a group of mutually exclusive choices.
- The properties are shown in the table below.


.. figure:: https://foruda.gitee.com/images/1745894565905113170/8ba3d87a_10088396.gif
   :align: center
   :width: 400px
   :name: RadioButton

   RadioButton


.. table:: RadioButton Widget Properties
   :align: center
   :width: 100%

   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | Property                | Description                                                                                                              |
   +=========================+==========================================================================================================================+
   | Name                    | Widget name.                                                                                                             |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | Group Name              | links this button to a group where only one can be selected at a time.                                                   |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | Size (Height)           | Widget height.                                                                                                           |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | Size (Width)            | Widget width.                                                                                                            |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | X                       | Horizontal coordinate relative to the parent widget.                                                                     |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | Y                       | Vertical coordinate relative to the parent widget.                                                                       |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | BG Image X              | Horizontal Position of the background image.                                                                             |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | BG Image Y              | Vertical Position of the background image.                                                                               |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | BG Image (Default)      | Default background image.                                                                                                |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | BG Image (Highlight)    | Selected/Highlight background image.                                                                                     |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | Image Rotation Angle    | Background image rotation angle, range: 0~360 degree.                                                                    |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | Blend Mode              | Blending mode determines how the pixels of a UI element combine with the pixels of the underlying layers or background.  |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+
   | Opacity                 | Transparency level 0~255                                                                                                 |
   +-------------------------+--------------------------------------------------------------------------------------------------------------------------+




Image
'''''

.. table:: Image Widget Properties
   :align: center
   :width: 100%

   +----------------------+--------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Property             | Description                                                                                                        | 8762D/8763E | 8762G/8772G | TBD |
   +======================+====================================================================================================================+=============+=============+=====+
   | Name                 | Widget name.                                                                                                       |      Y      |      Y      |  Y  |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Height)        | Widget height.                                                                                                     |      Y      |      Y      |  Y  |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)         | Widget width.                                                                                                      |      Y      |      Y      |  Y  |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | X                    | Horizontal coordinate relative to the parent widget.                                                               |      Y      |      Y      |  Y  |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Y                    | Vertical coordinate relative to the parent widget.                                                                 |      Y      |      Y      |  Y  |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Image                | Image Path                                                                                                         |      Y      |      Y      |  Y  |
   |                      |                                                                                                                    |             |             |     |
   |                      | Note: The image must be pre-imported into the project.                                                             |             |             |     |
   |                      | Please refer to :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Image_Resource_Management` for details.               |             |             |     |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Image Rotation Angle | Image rotation angle.                                                                                              |      Y      |      Y      |  Y  |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Image Scale X        | Image horizontal scaling degree, is a multiplier/percentage.                                                       |      Y      |      Y      |  Y  |
   |                      |                                                                                                                    |             |             |     |
   |                      | For example, set scale x 0.5 means that the actual display width of the image is half of the original image width. |             |             |     |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Image Scale Y        | Image vertical scaling degree, is a multiplier/percentage.                                                         |      Y      |      Y      |  Y  |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Blend Mode           |  how the pixels of a UI element combine with the pixels of the underlying layers or background.                    |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Opacity              |  Transparency level 0~255                                                                                          |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+

ImageMovie
''''''''''

- ImageMovie achieves a video playback effect by sequentially displaying a series of image frames. 
- This requires setting up a folder containing the image frame sequence, with frames sorted and displayed based on their filenames.



.. table:: ImageMovie Widget Properties
   :align: center
   :width: 100%

   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Property             | Description                                                                                                        |
   +======================+====================================================================================================================+
   | Name                 | Widget name.                                                                                                       |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Size (Height)        | Widget height.                                                                                                     |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Size (Width)         | Widget width.                                                                                                      |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | X                    | Horizontal coordinate relative to the parent widget.                                                               |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Y                    | Vertical coordinate relative to the parent widget.                                                                 |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Image Directory      | Image Directory Path                                                                                               |
   |                      |                                                                                                                    |
   |                      | Note: The image must be pre-imported into the project.                                                             |
   |                      | Please refer to :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Image_Resource_Management` for details.               |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Image Rotation Angle | Image rotation angle.                                                                                              |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Image Scale X        | Image horizontal scaling degree, is a multiplier/percentage.                                                       |
   |                      |                                                                                                                    |
   |                      | For example, set scale x 0.5 means that the actual display width of the image is half of the original image width. |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Image Scale Y        | Image vertical scaling degree, is a multiplier/percentage.                                                         |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Blend Mode           |  how the pixels of a UI element combine with the pixels of the underlying layers or background.                    |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Opacity              |  Transparency level 0~255                                                                                          |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Duration             |  Duration of the video effect                                                                                      |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+


.. note::
   
   1.  When exporting, the tool will convert the imported images. And the image conversion parameters can be set in :menuselection:`Menu Bar --> Setting --> Image Convert Setting`, please refer to :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Image_Convert_Setting` for details;
   2.  If the size of the imported image does not match the size of the widget, the tool doesn't scale or crop the image.

SeekBar
'''''''

Sliding widget that can respond to user swipe gesture with the widget
and change the progress value. The properties are shown in the table
below.

.. figure:: https://foruda.gitee.com/images/1710817870508689816/f43215ff_12407535.png
   :align: center
   :width: 400px
   :name: {9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}-Figure_SeekBar

   SeekBar

.. table:: SeekBar Widget Properties
   :align: center
   :width: 100%

   +-------------------------+---------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Property                | Description                                                                           | 8762D/8763E | 8762G/8772G | TBD |
   +=========================+=======================================================================================+=============+=============+=====+
   | Name                    | Widget name.                                                                          |      Y      |      Y      |  Y  |
   +-------------------------+---------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Height)           | Widget height.                                                                        |      Y      |      Y      |  Y  |
   +-------------------------+---------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)            | Widget width.                                                                         |      Y      |      Y      |  Y  |
   +-------------------------+---------------------------------------------------------------------------------------+-------------+-------------+-----+
   | X                       | Horizontal coordinate relative to the parent widget.                                  |      Y      |      Y      |  Y  |
   +-------------------------+---------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Y                       | Vertical coordinate relative to the parent widget.                                    |      Y      |      Y      |  Y  |
   +-------------------------+---------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Color(Highlight) (RGBA) | Background color of partially completed part of the progress bar.                     |      N      |      Y      |  N  |
   +-------------------------+---------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Color (RGBA)            | Background color of the whole progress bar.                                           |      N      |      Y      |  N  |
   +-------------------------+---------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Orientation             | Widget display orientation and gesture response orientation with the following types: |      Y      |      Y      |  Y  |
   |                         |                                                                                       |             |             |     |
   |                         | vertical/V: Vertical orientation                                                      |             |             |     |
   |                         |                                                                                       |             |             |     |
   |                         | arc: Direction of a curve                                                             |             |             |     |
   |                         |                                                                                       |             |             |     |
   |                         | horizontal/H: Horizontal orientation                                                  |             |             |     |
   +-------------------------+---------------------------------------------------------------------------------------+-------------+-------------+-----+

Image SeekBar
'''''''''''''

Sliding widget with multi-images as background, and switch to different
images as the user swipes. The properties are shown in the table below.

.. table:: Image SeekBar Widget Properties
   :align: center
   :width: 100%

   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Property        | Description                                                                                                           | 8762D/8763E | 8762G/8772G | TBD |
   +=================+=======================================================================================================================+=============+=============+=====+
   | Name            | Widget name.                                                                                                          |      Y      |      Y      |  Y  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Height)   | Widget height.                                                                                                        |      Y      |      Y      |  Y  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)    | Widget width.                                                                                                         |      Y      |      Y      |  Y  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | X               | Horizontal coordinate relative to the parent widget.                                                                  |      Y      |      Y      |  Y  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Y               | Vertical coordinate relative to the parent widget.                                                                    |      Y      |      Y      |  Y  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Degree (Start)  | Start degree (Invalid if orientation is arc).                                                                         |      Y      |      Y      |  Y  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Degree (End)    | End degree (Invalid if orientation is arc).                                                                           |      Y      |      Y      |  Y  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Image Directory | Folder that contains only the images to be displayed on this widget.                                                  |      Y      |      Y      |  Y  |
   |                 |                                                                                                                       |             |             |     |
   |                 | Notes:                                                                                                                |             |             |     |
   |                 |                                                                                                                       |             |             |     |
   |                 | 1. Please sort the images by name;                                                                                    |             |             |     |
   |                 |                                                                                                                       |             |             |     |
   |                 | 2. When the user swipes on the widget, the widget will switch the background image according to the current progress. |             |             |     |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Central X       | Horizontal coordinate of the center of the arc relative to the parent widget.                                         |      Y      |      Y      |  Y  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Central Y       | Vertical coordinate of the center of the arc relative to the parent widget.                                           |      Y      |      Y      |  Y  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Orientation     | Widget display orientation and gesture response orientation with the following types:                                 |      Y      |      Y      |  Y  |
   |                 |                                                                                                                       |             |             |     |
   |                 | vertical/V: Vertical orientation                                                                                      |             |             |     |
   |                 |                                                                                                                       |             |             |     |
   |                 | arc: Direction of a curve                                                                                             |             |             |     |
   |                 |                                                                                                                       |             |             |     |
   |                 | horizontal/H: Horizontal orientation                                                                                  |             |             |     |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+-------------+-------------+-----+


SingleImage SeekBar
'''''''''''''''''''

Sliding widget with a image as background, and change the display scope as the user swipes. The properties are shown in the table below.



.. figure:: https://foruda.gitee.com/images/1745910726644453215/24d73f89_10088396.gif
   :align: center
   :width: 400px
   :name: {9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}-SingleImage

   SingleImage


.. table:: SingleImage SeekBar Widget Properties
   :align: center
   :width: 100%

   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Property        | Description                                                                                                           |
   +=================+=======================================================================================================================+
   | Name            | Widget name.                                                                                                          |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Size (Height)   | Widget height.                                                                                                        |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Size (Width)    | Widget width.                                                                                                         |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | X               | Horizontal coordinate relative to the parent widget.                                                                  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Y               | Vertical coordinate relative to the parent widget.                                                                    |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | BG Image        | The image file.                                                                                                       |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Orientation     | Widget display orientation and gesture response orientation with the following types:                                 |
   |                 |                                                                                                                       |
   |                 | vertical/V: Vertical orientation                                                                                      |
   |                 |                                                                                                                       |
   |                 | arc: Direction of a curve                                                                                             |
   |                 |                                                                                                                       |
   |                 | horizontal/H: Horizontal orientation                                                                                  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Blend Mode      |  how the pixels of a UI element combine with the pixels of the underlying layers or background.                       |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Opacity         |  Transparency level 0~255                                                                                             |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+



ThumbSeekBar
'''''''''''''''''''

- ThumbSeekBar is a widget where a thumb image follows the touch point during sliding to indicate progress.
- When the progress reaches 100%, the thumb image switches to a highlighted image for display.
- The properties are shown in the table below.


.. table:: ThumbSeekBar Widget Properties
   :align: center
   :width: 100%

   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Property        | Description                                                                                                           |
   +=================+=======================================================================================================================+
   | Name            | Widget name.                                                                                                          |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Size (Height)   | Widget height.                                                                                                        |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Size (Width)    | Widget width.                                                                                                         |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | X               | Horizontal coordinate relative to the parent widget.                                                                  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Y               | Vertical coordinate relative to the parent widget.                                                                    |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Thumb X         | Thumb image offset.                                                                                                   |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Thumb Y         | Thumb image offset.                                                                                                   |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Background      | The background image file                                                                                             |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Thumb           | The Thumb image file                                                                                                  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Thumb(highlight)| The highlighted thumb image will be displayed when the progress reaches 100%.                                         |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Orientation     | Widget display orientation and gesture response orientation with the following types:                                 |
   |                 |                                                                                                                       |
   |                 | vertical/V: Vertical orientation                                                                                      |
   |                 |                                                                                                                       |
   |                 | arc: Direction of a curve                                                                                             |
   |                 |                                                                                                                       |
   |                 | horizontal/H: Horizontal orientation                                                                                  |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Blend Mode      |  how the pixels of a UI element combine with the pixels of the underlying layers or background.                       |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+
   | Opacity         |  Transparency level 0~255                                                                                             |
   +-----------------+-----------------------------------------------------------------------------------------------------------------------+



SideBar
''''''''''

- SideBar is a widget that can slide in from any of the four sides of the screen. 
- It allows you to specify the proportion of the screen it occupies after sliding in.


.. table:: SideBar Widget Properties
   :align: center
   :width: 100%

   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Property             | Description                                                                                                        |
   +======================+====================================================================================================================+
   | Name                 | Widget name.                                                                                                       |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Size (Height)        | Widget height.                                                                                                     |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Size (Width)         | Widget width.                                                                                                      |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | X                    | Horizontal coordinate relative to the parent widget.                                                               |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Y                    | Vertical coordinate relative to the parent widget.                                                                 |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Orientation          | Slide in from which side of the screen.                                                                            |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Scope                | The proportion of the screen it occupies.                                                                          |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+

Roller
''''''''''

- Roller is a widget that scrolls through rows of text based on upward or downward swipe gestures. 
- The text in the middle row is highlighted in a distinct color. 
- The scrolling features inertia and alignment effects for a smooth user experience.

.. figure:: https://foruda.gitee.com/images/1745914810308863014/d9025424_10088396.gif
   :align: center
   :width: 400px
   :name: {9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}-Roller

   Roller


.. table:: Roller Widget Properties
   :align: center
   :width: 100%

   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Property             | Description                                                                                                        |
   +======================+====================================================================================================================+
   | Name                 | Widget name.                                                                                                       |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Size (Height)        | Widget height.                                                                                                     |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Size (Width)         | Widget width.                                                                                                      |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | X                    | Horizontal coordinate relative to the parent widget.                                                               |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Y                    | Vertical coordinate relative to the parent widget.                                                                 |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Row Count            | Row count                                                                                                          |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Row Space            | Height of one row                                                                                                  |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Font                 | Font setting, please refer to :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Font_Convert_Setting`.                  |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Font Color (RGBA)    | Font color setting, use RGBA.                                                                                      |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Highlight Font Color | Middle row's Font color setting, use RGBA.                                                                         |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+
   | Content alignment    | Text layout                                                                                                        |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+   
   | Loop scrolling       | If loop while scrolling.                                                                                           |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+ 
   | Items                | Texts for each row                                                                                                 |
   +----------------------+--------------------------------------------------------------------------------------------------------------------+ 




Chart
''''''''''

- Chart is a widget that supports three styles: waveform, bar, and line. 
- It allows customization of the chart's vertical range and color.



.. figure:: https://foruda.gitee.com/images/1745917347774483906/2412b17b_10088396.png
   :align: center
   :width: 400px
   :name: {9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}-Chart

   Chart

ComboBox
''''''''''

- ComboBox is a dropdown widget that expands when clicked. 
- Upon selecting an option, the dropdown collapses, displaying the chosen option. 
- Both clicking and selecting options feature a highlight effect.

.. figure:: https://foruda.gitee.com/images/1745918646533630134/034c1b3b_10088396.gif
   :align: center
   :width: 400px
   :name: {9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}-ComboBox

   ComboBox

Calendar 
''''''''''

- Calendar is a perpetual calendar widget that displays the dates of a month. 
- Users can swipe left or right to switch between months, with the current date highlighted.



.. figure:: https://foruda.gitee.com/images/1745921254906327275/87799f1b_10088396.gif
   :align: center
   :width: 400px
   :name: {9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}-Calendar

   Calendar


Menu
''''''''''

- Menu is a multi-level menu container widget where each menu is mutually exclusive in display. 
- By navigating through the menu, users can switch between them.


.. figure:: https://foruda.gitee.com/images/1747020603782294715/2419a981_10088396.gif
   :align: center
   :width: 400px
   :name: Navigating Through Menus

   Navigating Through Menus

- Setting RadioButton OnSelect event for navigating.

.. figure:: https://foruda.gitee.com/images/1747020682739326978/3585cd1e_10088396.png
   :align: center
   :width: 400px
   :name: Setting RadioButton OnSelect

   Setting RadioButton OnSelect

- Each image nested in a menu.

.. figure:: https://foruda.gitee.com/images/1747020732982303314/1f42f79a_10088396.png
   :align: center
   :width: 400px
   :name: Widget Tree for Menu

   Widget Tree for Menu

Key
''''''''''

- Key is a widget for physical button. 
- On PC, Key's ID maps to QWERTY keyboard character's ASCII value.
- On EVB, Key's ID mapping is based on porting in SDK.

.. figure:: https://foruda.gitee.com/images/1746619930224908218/7fe6fcb1_10088396.gif
   :align: center
   :width: 400px
   :name: Clicking Key to Navigate to Tab 

   Clicking Key to Navigate to Tab

Switch
''''''

Switch widget with **Checked** and **Unchecked** states. The properties are
shown in the table below.

.. table:: Switch Widget Properties
   :align: center
   :width: 100%

   +--------------------+------------------------------------------------------+-------------+-------------+-----+
   | Property           | Description                                          | 8762D/8763E | 8762G/8772G | TBD |
   +====================+======================================================+=============+=============+=====+
   | Name               | Widget name.                                         |      Y      |      Y      |  Y  |
   +--------------------+------------------------------------------------------+-------------+-------------+-----+
   | Size (Height)      | Widget height.                                       |      Y      |      Y      |  Y  |
   +--------------------+------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)       | Widget width.                                        |      Y      |      Y      |  Y  |
   +--------------------+------------------------------------------------------+-------------+-------------+-----+
   | X                  | Horizontal coordinate relative to the parent widget. |      Y      |      Y      |  Y  |
   +--------------------+------------------------------------------------------+-------------+-------------+-----+
   | Y                  | Vertical coordinate relative to the parent widget.   |      Y      |      Y      |  Y  |
   +--------------------+------------------------------------------------------+-------------+-------------+-----+
   | BG Image (Checked) | Checked state background image.                      |      Y      |      Y      |  Y  |
   +--------------------+------------------------------------------------------+-------------+-------------+-----+
   | BG Image (Default) | Unchecked state background image.                    |      Y      |      Y      |  Y  |
   +--------------------+------------------------------------------------------+-------------+-------------+-----+

Arc
'''

Arc widget, no gesture support yet. The properties are shown in the
table below.

.. table:: Arc Widget Properties
   :align: center
   :width: 100%

   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Property       | Description                                                                         | 8762D/8763E | 8762G/8772G | TBD |
   +================+=====================================================================================+=============+=============+=====+
   | Name           | Widget name.                                                                        |      Y      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Height)  | Widget height.                                                                      |      Y      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)   | Widget width.                                                                       |      Y      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | X              | Horizontal coordinate relative to the parent widget.                                |      Y      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Y              | Vertical coordinate relative to the parent widget.                                 |      Y      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Central X      | Horizontal coordinate of the center of the arc relative to the parent widget.       |      N      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Central Y      | Vertical coordinate of the center of the arc relative to the parent widget.         |      N      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | BG Color       | Arc background color.                                                               |      N      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Cap Mode       | Arc cap mode, the following options are supported:                                  |      N      |      Y      |  N  |
   |                |                                                                                     |             |             |     |
   |                | round/butt/square.                                                                  |             |             |     |
   |                |                                                                                     |             |             |     |
   |                |.. image:: https://foruda.gitee.com/images/1710817869829442326/90524d4c_12407535.png |             |             |     |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Degree (End)   | End degree of arc.                                                                  |      N      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Degree (Start) | Start degree of arc.                                                                |      N      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Radius         | Radius of arc.                                                                      |      N      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+
   | Stroke Width   | Width of arc stroke.                                                                |      N      |      Y      |  N  |
   +----------------+-------------------------------------------------------------------------------------+-------------+-------------+-----+

Container Widget
^^^^^^^^^^^^^^^^

Screen
''''''

Screen widget, corresponding to the physical screen, is the root widget
of a GUI project. The properties are shown in the table below.

.. table:: Screen Properties
   :align: center
   :width: 100%

   +---------------+----------------------------------+-------------+-------------+-----+
   | Property      | Description                      | 8762D/8763E | 8762G/8772G | TBD |
   +===============+==================================+=============+=============+=====+
   | Name          | Widget name.                     |      Y      |      Y      |  Y  |
   +---------------+----------------------------------+-------------+-------------+-----+
   | Size (Height) | Widget height.                   |      Y      |      Y      |  Y  |
   +---------------+----------------------------------+-------------+-------------+-----+
   | Size (Width)  | Widget width.                    |      Y      |      Y      |  Y  |
   +---------------+----------------------------------+-------------+-------------+-----+
   | X             | Horizontal coordinate, always 0. |      Y      |      Y      |  Y  |
   +---------------+----------------------------------+-------------+-------------+-----+
   | Y             | Vertical coordinate, always 0.   |      Y      |      Y      |  Y  |
   +---------------+----------------------------------+-------------+-------------+-----+

.. note:: Only 'Name' property can be modified.

TabView and Tab
'''''''''''''''

With the Tab widget as a child widget, it supports up/down/left/right
swiping to switch among Tabs. The properties of TabView and Tab are
shown in the table below.

.. figure:: https://foruda.gitee.com/images/1710817871000642675/ca6cda22_12407535.png
   :align: center
   :width: 300px

   TabView and Tabs

.. table:: TabView Properties
   :align: center
   :width: 100%

   +---------------+----------------------------------------------------------------+-------------+-------------+-----+
   | Property      | Description                                                    | 8762D/8763E | 8762G/8772G | TBD |
   +===============+================================================================+=============+=============+=====+
   | Name          | Widget name.                                                   |      Y      |      Y      |  Y  |
   +---------------+----------------------------------------------------------------+-------------+-------------+-----+
   | Size (Height) | Widget height.                                                 |      Y      |      Y      |  Y  |
   +---------------+----------------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)  | Widget width.                                                  |      Y      |      Y      |  Y  |
   +---------------+----------------------------------------------------------------+-------------+-------------+-----+
   | X             | Horizontal coordinate relative to the parent widget, always 0. |      Y      |      Y      |  Y  |
   +---------------+----------------------------------------------------------------+-------------+-------------+-----+
   | Y             | Vertical coordinate relative to the parent widget, always 0.   |      Y      |      Y      |  Y  |
   +---------------+----------------------------------------------------------------+-------------+-------------+-----+
   | Transition    | Tab transition mode with the following supported types:        |      N      |      Y      |  Y  |
   |               |                                                                |             |             |     |
   |               | normal: No effect                                              |             |             |     |
   |               |                                                                |             |             |     |
   |               | fade: Fade-in/out                                              |             |             |     |
   |               |                                                                |             |             |     |
   |               | scale: Scaling                                                 |             |             |     |
   |               |                                                                |             |             |     |
   |               | fadeScale: Fade-in/out and scaling                             |             |             |     |
   +---------------+----------------------------------------------------------------+-------------+-------------+-----+

.. table:: Tab Properties
   :align: center
   :width: 100%

   +---------------+-------------------------------------------------------------+-------------+-------------+-----+
   | Property      | Description                                                 | 8762D/8763E | 8762G/8772G | TBD |
   +===============+=============================================================+=============+=============+=====+
   | Name          | Widget name.                                                |      Y      |      Y      |  Y  |
   +---------------+-------------------------------------------------------------+-------------+-------------+-----+
   | Size (Height) | Widget height.                                              |      Y      |      Y      |  Y  |
   +---------------+-------------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)  | Widget width.                                               |      Y      |      Y      |  Y  |
   +---------------+-------------------------------------------------------------+-------------+-------------+-----+
   | X             | Horizontal coordinate relative to TabView widget, always 0. |      Y      |      Y      |  Y  |
   +---------------+-------------------------------------------------------------+-------------+-------------+-----+
   | Y             | Vertical coordinate relative to TabView widget, always 0.   |      Y      |      Y      |  Y  |
   +---------------+-------------------------------------------------------------+-------------+-------------+-----+
   | Index(X-Axis) | Horizontal index of Tabs in TabView.                        |      Y      |      Y      |  Y  |
   +---------------+-------------------------------------------------------------+-------------+-------------+-----+
   | Index(Y-Axis) | Vertical index of Tabs in TabView.                          |      Y      |      Y      |  Y  |
   +---------------+-------------------------------------------------------------+-------------+-------------+-----+

.. note::
   
   1. TabView width and height cannot be modified, defaulting to the Screen's width and height;
   2. TabView horizontal and vertical coordinates cannot be modified, always being 0;
   3. TabView can only be used as a child of the Screen widget;
   4. TabView's child widgets can only be Tabs;
   5. Tab's width and height cannot be modified, defaulting to TabView's width and height;
   6. Tab's horizontal and vertical coordinates cannot be modified and are always 0.

Page
''''

Container widget with scrollable content.

.. table:: Page Properties
   :align: center
   :width: 100%

   +---------------+------------------------------------------------------+-------------+-------------+-----+
   | Property      | Description                                          | 8762D/8763E | 8762G/8772G | TBD |
   +===============+======================================================+=============+=============+=====+
   | Name          | Widget name.                                         |      Y      |      Y      |  Y  |
   +---------------+------------------------------------------------------+-------------+-------------+-----+
   | Size (Height) | Widget height.                                       |      Y      |      Y      |  Y  |
   +---------------+------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)  | Widget width.                                        |      Y      |      Y      |  Y  |
   +---------------+------------------------------------------------------+-------------+-------------+-----+
   | X             | Horizontal coordinate relative to the parent widget. |      Y      |      Y      |  Y  |
   +---------------+------------------------------------------------------+-------------+-------------+-----+
   | Y             | Vertical coordinate relative to the parent widget.   |      Y      |      Y      |  Y  |
   +---------------+------------------------------------------------------+-------------+-------------+-----+

.. note::

   1. Page only supports vertical scrolling;
   2. The width and height of the Page widget only define the area of the interface that can respond to a swipe gesture. Whether scrolling is allowed depends on whether or not the child widget added to it is outside the scope of the screen.

Win
'''

Within the area defined by Win width and height, it can respond to
various gestures, including click, long click, press, press release, and
swipe. The properties are shown in the table below.

.. table:: Win Properties
   :align: center
   :width: 100%

   +---------------+---------------------------------------------------------------+-------------+-------------+-----+
   | Property      | Description                                                   | 8762D/8763E | 8762G/8772G | TBD |
   +===============+===============================================================+=============+=============+=====+
   | Name          | Widget name.                                                  |      Y      |      Y      |  Y  |
   +---------------+---------------------------------------------------------------+-------------+-------------+-----+
   | Size (Height) | Widget height.                                                |      Y      |      Y      |  Y  |
   +---------------+---------------------------------------------------------------+-------------+-------------+-----+
   | Size (Width)  | Widget width.                                                 |      Y      |      Y      |  Y  |
   +---------------+---------------------------------------------------------------+-------------+-------------+-----+
   | X             | Horizontal coordinate relative to the parent widget.          |      Y      |      Y      |  Y  |
   +---------------+---------------------------------------------------------------+-------------+-------------+-----+
   | Y             | Vertical coordinate relative to the parent widget.            |      Y      |      Y      |  Y  |
   +---------------+---------------------------------------------------------------+-------------+-------------+-----+
   | Hidden        | Indicates whether Win and its child widget need to be hidden. |      Y      |      Y      |  Y  |
   +---------------+---------------------------------------------------------------+-------------+-------------+-----+

Design View/Canvas
~~~~~~~~~~~~~~~~~~

Users can drag and drop widgets from the Toolbox panel into the Design
View, adjust the widgets' layout, and set properties to design a
graphical interface that can be rendered in the Realtek ICs.

.. figure:: https://foruda.gitee.com/images/1721627839639298235/033d3a5b_12407535.png
   :align: center
   :width: 750px

   Design View

TabView - Create/Delete/Insert Tab
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Drag and drop the TabView widget from the Toolbox into the Design View,
then a TabView that contains only a home tab (coordinates (0,0)) is
created, as shown in the figure below.

.. figure:: https://foruda.gitee.com/images/1721627964036015694/3b5ee0d4_12407535.png
   :align: center
   :width: 750px

   Create TabView

Create Tab
''''''''''

New tabs can be created by clicking the buttons around the Design View.

.. note::
   
   1. If idx is 0, the up and down button is enabled;
   2. If idy is 0, the left and right button is enabled.

Delete Tab
''''''''''

Select the tab to be deleted, click :menuselection:`Edit --> Delete` on the menu bar or
press the :kbd:`Delete` key on the keyboard. Then double-check if the deletion
is intended.

.. figure:: https://foruda.gitee.com/images/1721628091462645405/beca8dd5_12407535.png
   :align: center
   :width: 220px

   Delete Tab Double-Check

Insert Tab
''''''''''

Currently, tab insertion is only supported by modifying the coordinates
of an existing tab and creating a new one.

For example, if a tab needs to be inserted between tabs with coordinates
(1, 0) and (2, 0), the steps are as follows.

1. Increase the idx of Tab (2, 0) and the tabs to its right by 1, as shown in the figure below;
2. Switch to Tab (1, 0) and click to create the new Tab (2, 0).

.. figure:: https://foruda.gitee.com/images/1726815120686817698/cdf44743_12407535.png
   :align: center
   :width: 750px

   Tab Insertion Position

.. figure:: https://foruda.gitee.com/images/1726815280073982051/68230d3d_12407535.png
   :align: center
   :width: 750px

   Modify Existing Tab Index X and Y

.. figure:: https://foruda.gitee.com/images/1721628460504196478/3446e06e_12407535.png
   :align: center
   :width: 750px

   Insert Tab

TabView Overview Window
^^^^^^^^^^^^^^^^^^^^^^^

Please click |icon| to show the *TabView Overview Window*.

.. note::

   1. The highlighted Tab in the Overview Chart indicates the Tab that is currently being edited in Design View;
   2. The Overview Chart labels each Tab with its coordinates. When simulated or rendered in ICs, the Tab with coordinates (0,0) is displayed on the Home page, and users can swipe up/down/left/right to display other Tabs.

.. figure:: https://foruda.gitee.com/images/1721628617214746327/0711439d_12407535.png
   :align: center
   :width: 750px

   TabView Overview Chart

.. figure:: https://foruda.gitee.com/images/1721628633709549622/a41c6011_12407535.png
   :align: center
   :width: 750px

   TabView Overview Chart

Zoom of Design View
^^^^^^^^^^^^^^^^^^^

There are 3 ways to zoom in the Design View.

1. Press the :kbd:`Ctrl` key and wheel mouse;
2. Click the :guilabel:`-` and :guilabel:`+` buttons;
3. Drag the slider bar.

.. figure:: https://foruda.gitee.com/images/1721628744851323697/d56f9725_12407535.png
   :align: center
   :width: 750px

   Zoom of Design View

Property View
~~~~~~~~~~~~~

Selecting a widget in the Widget Tree or Design View exposes all of the
widget's property values, which users can modify as needed.

.. figure:: https://foruda.gitee.com/images/1726814067710788486/a6564f40_12407535.png
   :align: center
   :width: 750px

   Property View

Widget Tree
~~~~~~~~~~~

The Widget Tree is used to present to the users the parent/child/sibling
relationship of the currently laid out widgets. And we have the
following convention here.

1. The child widget layer is on top of the parent widget layer, i.e., when the parent and child widget overlap, the child widget will cover the parent widget;
2. The layer of sibling widgets is related to the order in which the widgets are added, with widgets added first at the bottom and widgets added later at the top.

The figure shows all the child widgets of the Home tab and Lamp tab,
where the Home tab has only one Image child widget for setting the
background, and the Lamp tab contains an Image widget and several Switch
widgets.

.. figure:: https://foruda.gitee.com/images/1721628970571915820/15772594_12407535.png
   :align: center
   :width: 750px

   Home Tab

.. figure:: https://foruda.gitee.com/images/1721628988181629026/d9ff763a_12407535.png
   :align: center
   :width: 750px

   Lamp Tab

Widget Tree supports the following operations.

1. Select widget: If a widget is selected on the Widget Tree, the corresponding widget in the Design View focuses and its properties are shown on Property View;
2. Modify the parent-child relationship: Select a widget on the Widget Tree (except Tab/TabView/Screen) and drag-and-drop it on the target widget item. Then the widget will be a child widget of the target widget;
3. Modify widget layers: Select a widget on the Widget Tree (except Tab/TabView/Screen) and drag-and-drop it to the upper or lower edge of the target widget item. Then on the Design View, the widget will be placed over or under the target widget;
4. Lock widgets: Click the button and lock the widget/widgets.

   1. If the lock button of the screen is clicked, all the screen's child widgets will be locked, and the user could not drag or resize the widgets on Design View;
   2. If the lock button of the Tab is clicked, all the tab's child widgets will be locked, and the user could not drag or resize the widgets on Design View.

.. figure:: https://foruda.gitee.com/images/1726816487543506275/da6ebe9d_12407535.png
   :align: center

   Un-Locked

.. figure:: https://foruda.gitee.com/images/1726816816056161349/efcdd833_12407535.png
   :align: center

   Locked

.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Event_Setting:

Event Setting
---------------

Trigger Events Brief
~~~~~~~~~~~~~~~~~~~~~

.. list-table:: Trigger Events
   :widths: 20 40 40
   :header-rows: 1

   * - Name
     - Description
     - Supported Controls
   * - OnClick
     - Triggered when a widget is clicked.
     - Button, Win, Key
   * - OnSelect
     - Triggered when an option or item is selected.
     - RadioButton
   * - OnLoad
     - Triggered when a widget or page is loaded.
     - Image, SeekBar set, Win
   * - OnValueChange
     - Triggered when a widget's value or state changes.
     - ThumbSeekBar, ImageSeekBar, SingleImageSeekBar
   * - OnOn
     - Triggered when a widget is turned on or activated.
     - Switch
   * - OnOff
     - Triggered when a widget is turned off or deactivated.
     - Switch
   * - OnTime
     - Triggered by real time or data.
     - Image(type: hour, minute, second), Text
   * - OnPeripheral
     - Triggered by a peripheral device.
     - Text, Chart, Arc
   * - OnComplete
     - Triggered when an action or process is completed.
     - ThumbSeekBar, ImageSeekBar, SingleImageSeekBar


Action Brief
~~~~~~~~~~~~~~

.. list-table:: Action
   :widths: 20 40 40
   :header-rows: 1

   * - Name
     - Description
     - Supported Trigger Events
   * - Animation
     - Start or pause a animation.
     - OnClick, OnLoad, OnOff, OnOn
   * - Set Time
     - A widget displays real time or date.
     - OnTime
   * - Set Peripheral
     - A widget displays peripheral's data.
     - OnPeripheral
   * - Jump
     - Navigate to a display effect(Tab, Menu, App, launcher).
     - OnClick, OnSelect, OnComplete
   * - Set Text Property
     - Update a Text's content.
     - OnValueChange


Event Setting Example
~~~~~~~~~~~~~~~~~~~~~

Text - OnTime - Set Time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- The Text shows real time in :kbd:`00:00` format.
- Select a Text widget, click the yellow lightning icon in Property panel, Choose :guilabel:`OnTime` Trigger, and click :guilabel:`Add Event` icon.

.. figure:: https://foruda.gitee.com/images/1746534321715090170/409729a5_10088396.png
   :align: center
   :width: 400px
   :name: Text - OnTime - Set Time

   Text - OnTime - Set Time

.. figure:: https://foruda.gitee.com/images/1746534115101519614/2a71cc0b_10088396.png
   :align: center
   :width: 400px
   :name: "00:00" Format

   "00:00" Format


Text - OnPeripheral - Set Peripheral
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- The Text shows Temperture value.
- Select a Text widget, click the yellow lightning icon in Property panel, Choose :guilabel:`OnPeripheral` Trigger, and click :guilabel:`Add Event` icon.

.. figure:: https://foruda.gitee.com/images/1746536661568009147/585acc2b_10088396.png
   :align: center
   :width: 400px
   :name: Text - OnPeripheral - Set Peripheral

   Text - OnPeripheral - Set Peripheral

.. figure:: https://foruda.gitee.com/images/1746536593655244781/847265cf_10088396.png
   :align: center
   :width: 400px
   :name: Temperture value

   Temperture Value Effect 


Button - OnClick - Jump
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Click the Button to navigate to the Launcher
- Select a Button widget, click the yellow lightning icon in Property panel, Choose :guilabel:`OnClick` Trigger, and click :guilabel:`Add Event` icon.

.. figure:: https://foruda.gitee.com/images/1746537843864983728/aeb22ac7_10088396.png
   :align: center
   :width: 400px
   :name: Button - OnClick - Jump

   Button - OnClick - Jump

.. figure:: https://foruda.gitee.com/images/1746537471468564505/73316780_10088396.gif
   :align: center
   :width: 400px
   :name: Navigate to the Launcher

   Navigate to Launcher

RadioButton - OnSelect - Jump
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Click the RadioButton to navigate to the specific Tab or Launcher
- Select a RadioButton widget, click the yellow lightning icon in Property panel, Choose :guilabel:`OnSelect` Trigger, and click :guilabel:`Add Event` icon.

.. figure:: https://foruda.gitee.com/images/1746538686942083962/221748a3_10088396.png
   :align: center
   :width: 400px
   :name: RadioButton - OnSelect - Jump

   RadioButton - OnSelect - Jump

.. figure:: https://foruda.gitee.com/images/1746538630891468492/dad921e7_10088396.gif
   :align: center
   :width: 400px
   :name: Navigate to Tab or Launcher

   Navigate to Tab or Launcher

Image - OnTime - Set Time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Image rotates as a watch's hand.
- Select a Image widget, click the yellow lightning icon in Property panel, Choose :guilabel:`OnTime` Trigger, and click :guilabel:`Add Event` icon.

.. figure:: https://foruda.gitee.com/images/1746541398866452997/e2383aac_10088396.png
   :align: center
   :width: 400px
   :name: Image - OnTime - Set Time

   Image - OnTime - Set Time

.. figure:: https://foruda.gitee.com/images/1746541349306828139/be90966b_10088396.gif
   :align: center
   :width: 400px
   :name: Rotating as Watch's Hands

   Rotating as Watch's Hands

Win - OnClick - Jump
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Click in Win's scope to navigate to the specific Tab.
- Select a Win widget, click the yellow lightning icon in Property panel, Choose :guilabel:`OnClick` Trigger, and click :guilabel:`Add Event` icon.

.. figure:: https://foruda.gitee.com/images/1746541398866452997/e2383aac_10088396.png
   :align: center
   :width: 400px
   :name: Win - OnClick - Jump

   Win - OnClick - Jump

.. figure:: https://foruda.gitee.com/images/1746583144736863278/8534f639_10088396.gif
   :align: center
   :width: 400px
   :name: Navigate to Tab by Clicking Win

   Navigate to Tab by Clicking Win

ImageSeekBar - OnComplete - Jump
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Drag SeekBar to 100% to navigate to the specific Tab.
- Select a ImageSeekBar widget, click the yellow lightning icon in Property panel, Choose :guilabel:`OnComplete` Trigger, and click :guilabel:`Add Event` icon.

.. figure:: https://foruda.gitee.com/images/1746587643812005158/1bbdb703_10088396.png
   :align: center
   :width: 400px
   :name: ImageSeekBar - OnComplete - Jump

   ImageSeekBar - OnComplete - Jump

.. figure:: https://foruda.gitee.com/images/1746587570975949917/26ce7d0a_10088396.gif
   :align: center
   :width: 400px
   :name: Navigate to Tab by SeekBar to 100%

   Navigate to Tab by SeekBar to 100%

ImageSeekBar - OnValueChange - Set Text Property
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Drag SeekBar, and text show the current progress.
- Select a ImageSeekBar widget, click the yellow lightning icon in Property panel, Choose :guilabel:`OnValueChange` Trigger, and click :guilabel:`Add Event` icon.

.. figure:: https://foruda.gitee.com/images/1746596718815338059/ac4bd71f_10088396.png
   :align: center
   :width: 400px
   :name: ImageSeekBar - OnValueChange - Set Text Property

   ImageSeekBar - OnValueChange - Set Text Property

.. figure:: https://foruda.gitee.com/images/1746596678776826816/3fc7f1a9_10088396.gif
   :align: center
   :width: 400px
   :name: Current Progress

   Current Progress

Switch - OnOn - Animation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Turn on a Switch to start a image animation.
- Select a Switch widget, click the yellow lightning icon in Property panel, Choose :guilabel:`OnOn` Trigger, and click :guilabel:`Add Event` icon.
- Onoff setting is similar to OnOn's.

.. figure:: https://foruda.gitee.com/images/1746618191372066531/edcb2463_10088396.png
   :align: center
   :width: 400px
   :name: Switch - OnOn - Animation

   Switch - OnOn - Animation

.. figure:: https://foruda.gitee.com/images/1746618094829096897/d9c77001_10088396.gif
   :align: center
   :width: 400px
   :name: Start a Image Animation

   Start a Image Animation

Key - OnClick - Jump
^^^^^^^^^^^^^^^^^^^^^^^^^

- Click the Key on the keyboard to navigate to the specific Tab.
- Select a Key widget, click the yellow lightning icon in Property panel, Choose :guilabel:`OnClick` Trigger, and click :guilabel:`Add Event` icon.

.. figure:: https://foruda.gitee.com/images/1746619969278276032/5298b16e_10088396.png
   :align: center
   :width: 400px
   :name: Key - OnClick - Jump

   Key - OnClick - Jump

.. figure:: https://foruda.gitee.com/images/1746619930224908218/7fe6fcb1_10088396.gif
   :align: center
   :width: 400px
   :name: Navigate to Tab by Clicking Key

   Navigate to Tab by Clicking Key





.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Animation_Setting:

Animation Setting
--------------------
- Animations in RVD work by changing a specific widget's properties over time. 
- They come with attributes like loop count, duration, target parameters
- And value control methods such as **from-to** or **key-frames**, along with interpolation options.
- The **from-to** method involves specifying the start and end values of a property within a single duration. 
- **Key-frames** build on this by allowing you to define a sequence of percentage-based progress nodes within that duration, with each node tied to a specific property value. 
- Events as OnClick, OnLoad, OnOff, OnOn are able to trigger animation start or pause.

Animation Type Brief
~~~~~~~~~~~~~~~~~~~~~

.. list-table:: Animation Types
   :widths: 20 50 30
   :header-rows: 1

   * - Type
     - Description
     - Supported Controls
   * - Rotation
     - Rotates the element around a specified axis or point.
     - Image
   * - Opacity
     - Changes the transparency level of the element.
     - Image
   * - AdvancedOpacity
     - Changes the transparency level in Key-frames type. 
     - Image
   * - Translation
     - Moves the element along a specified direction.
     - Image
   * - AdvancedTranslate
     - Moves the element in Key-frames type.
     - Image
   * - Scale
     - Resizes the element, either enlarging or shrinking it.
     - Image
   * - Progress
     - Animates a progress indicator, often used for loading or completion bars.
     - ProgressBar, SeekBar

Animation Example
~~~~~~~~~~~~~~~~~~~~~


Rotation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- The Image rotating around it's center in a 2000ms loop infinitely.
- Select the :menuselection:`Animation` panel, Choose :guilabel:`Rotation` Type, and click :guilabel:`Add` icon.
- Angle unit is degree.
- Rotating :guilabel:`Start Central X`, :guilabel:`Start Central Y`, :guilabel:`End Central X`, :guilabel:`End Central Y` are all 150px, as the image resolution is :math:`300px * 300px`, for a around it's center effect.
- Duration uint is millisecond.
- :guilabel:`Repeat 0` means indefinite.

.. figure:: https://foruda.gitee.com/images/1746625013725409821/5120844e_10088396.png
   :align: center
   :width: 400px
   :name: Rotation Setting

   Rotation Setting

.. figure:: https://foruda.gitee.com/images/1746625315257430404/231e36ab_10088396.gif
   :align: center
   :width: 400px
   :name: Rotating around It's Center

   Rotating around It's Center

- Configure the :guilabel:`OnLoad` event of Image to trigger the animation playback immediately upon its appearance.
- Choose :guilabel:`animate2` which is the Rotation as :guilabel:`Animation`.
- Choose :guilabel:`image0` which is the Image itself as :guilabel:`Target Widget`.

.. figure:: https://foruda.gitee.com/images/1746626898865085480/4a10b3d2_10088396.png
   :align: center
   :width: 400px
   :name: Setting Image's OnLoad

   Setting Image's OnLoad for Rotation


Opacity
^^^^^^^^^^^^

- Changes the transparency level of the image in a 2000ms loop infinitely from 255 opacity to 100 opacity.
- Select the :menuselection:`Animation` panel, Choose :guilabel:`Opacity` Type, and click :guilabel:`Add` icon.
- Set :guilabel:`Start Value` and :guilabel:`End Value` to range from :kbd:`255` to :kbd:`100`.
- Specify :guilabel:`Duration` in milliseconds.
- Set :guilabel:`Repeat` to :kbd:`0` for indefinite looping.

.. figure:: https://foruda.gitee.com/images/1746625850523787642/1602b1d2_10088396.png
   :align: center
   :width: 400px
   :name: Opacity Setting

   Opacity Setting

.. figure:: https://foruda.gitee.com/images/1746626034581611764/af8a8a36_10088396.gif
   :align: center
   :width: 400px
   :name: Changes the transparency level

   Changes the transparency level

- Configure the :guilabel:`OnLoad` event of Image to trigger the animation playback immediately upon its appearance.
- Choose :guilabel:`animate1` which is the :guilabel:`Opacity` as Animation.
- Choose :guilabel:`image0` which is the Image itself as :guilabel:`Target Widget`.

.. figure:: https://foruda.gitee.com/images/1746626159340482410/e8bee870_10088396.png
   :align: center
   :width: 400px
   :name: Setting Image's OnLoad for Opacity

   Setting Image's OnLoad for Opacity

AdvancedTranslate
^^^^^^^^^^^^^^^^^^^^^^^^

- Moves the image along a rectangle path.
- Path is relative 2D coordinate: :kbd:`0,0;0,150;400,150;400,0;0,0`, means down 150, then right 400, then up 150, then left 400.
- Key Times are :guilabel:`0;0.2;0.5;0.7;1`, means at these progress node within a duration, reaching the corresponding coordinate.
- Coordinate unit is pixel.
- Repeat 0 means indefinite.
- Calc Mode is :guilabel:`Linear` means linear moving.
- Configure the :guilabel:`OnLoad` event of Image to trigger the animation playback immediately upon its appearance.

.. figure:: https://foruda.gitee.com/images/1746628004131074559/b2312fba_10088396.png
   :align: center
   :width: 400px
   :name: AdvancedTranslate Setting

   AdvancedTranslate Setting

.. figure:: https://foruda.gitee.com/images/1746627400796126478/27364d84_10088396.gif
   :align: center
   :width: 400px
   :name: Rectangle Path

   Rectangle Path

- Calc Mode Discrete effect.

.. figure:: https://foruda.gitee.com/images/1746628523731815603/e8632d15_10088396.gif
   :align: center
   :width: 400px
   :name: Discrete Effect

   Discrete Effect


AdvancedOpacity
^^^^^^^^^^^^^^^^^^^^^^^^

- Modify the transparency of four images in sequence to achieve a marquee effect.
- Image P: :guilabel:`Values` are :kbd:`255;50;50`, :guilabel:`Key Times` are :kbd:`0;0.25;1`, meaning it starts fully opaque, at 25% progress the transparency becomes 50, and the image darkens.
- Image R: :guilabel:`Values` are :kbd:`50;255;50;50`, :guilabel:`Key Times` are :kbd:`0;0.25;0.5;1`, meaning it starts with transparency 50, becomes fully opaque at 25% progress, returns to transparency 50 at 50% progress, and remains at transparency 50 until the end.
- Image N: :guilabel:`Values` are :kbd:`50;255;50;50`, :guilabel:`Key Times` are :kbd:`0;0.5;0.75;1`, meaning it starts with transparency 50, becomes fully opaque at 50% progress, returns to transparency 50 at 75% progress, and remains at transparency 50 until the end.
- Image D: :guilabel:`Values` are :kbd:`50;255;50`, :guilabel:`Key Times` are :kbd:`0;0.75;1`, meaning it starts with transparency 50, becomes fully opaque at 75% progress, and returns to transparency 50 at the end.
- :guilabel:`Calc Mode` is :guilabel:`Discrete` means the transparency values change instantly at the specified key times without smooth interpolation between them.
- Configure the :guilabel:`OnLoad` event of Image to trigger the animation playback immediately upon its appearance.

.. figure:: https://foruda.gitee.com/images/1746687165856460117/17efb850_10088396.png
   :align: center
   :width: 400px
   :name: AdvancedOpacity Setting

   AdvancedOpacity Setting

.. figure:: https://foruda.gitee.com/images/1746684786170372465/827aa5f3_10088396.gif
   :align: center
   :width: 400px
   :name: Marquee Effect

   Marquee Effect

- :guilabel:`Calc Mode` is :guilabel:`Linear` means the transparency values change smoothly and linearly between the specified key times, creating a gradual transition effect.


.. figure:: https://foruda.gitee.com/images/1746687410946462671/7ce203ac_10088396.gif
   :align: center
   :width: 400px
   :name: AdvancedOpacity Linear Effect 

   AdvancedOpacity Linear Effect 

Translation
^^^^^^^^^^^^

- The Image moves to the right-down corner.
- :guilabel:`Start X` :kbd:`0`, :guilabel:`End X` :kbd:`800`, :guilabel:`Start Y` :kbd:`0`, :guilabel:`End Y` :kbd:`480`, meaning the image starts at its original position (:kbd:`0,0`) relative to its initial location and moves :kbd:`800` pixels to the right and :kbd:`480` pixels down to the bottom-right corner relative to its original position.
- Configure the :guilabel:`OnLoad` event of Image to trigger the animation playback immediately upon its appearance.

.. figure:: https://foruda.gitee.com/images/1746687959723538000/85b82b79_10088396.png
   :align: center
   :width: 400px
   :name: Translation Setting

   Translation Setting

.. figure:: https://foruda.gitee.com/images/1746687915633385177/757fd07c_10088396.gif
   :align: center
   :width: 400px
   :name: Right-Down Effect

   Right-Down Effect

Scale
^^^^^^^^^^^^

- The Image scales toward its center.
- :guilabel:`Start Scale X` :kbd:`1`, :guilabel:`End Scale X` :kbd:`0.10`, :guilabel:`Start Scale Y` :kbd:`1`, :guilabel:`End Scale Y` :kbd:`0.10`, meaning the image begins at its original size (100% scale) and uniformly shrinks to 10% of its original width and height, scaling down symmetrically from its center point.
- :guilabel:`Start Central X` :kbd:`400`, :guilabel:`End Central X` :kbd:`400`, :guilabel:`Start Central Y` :kbd:`240`, :guilabel:`End Central Y` :kbd:`240`, meaning the image's center point remains fixed at coordinates (400, 240), which is the center of the 800x480 resolution image, throughout the scaling process.
- Configure the :guilabel:`OnLoad` event of Image to trigger the animation playback immediately upon its appearance.

.. figure:: https://foruda.gitee.com/images/1746688570327220104/3d53ca69_10088396.png
   :align: center
   :width: 400px
   :name: Scale Setting

   Scale Setting

.. figure:: https://foruda.gitee.com/images/1746688538540976083/6035cc85_10088396.gif
   :align: center
   :width: 400px
   :name: Scale Effect

   Scale Effect

Progress
^^^^^^^^^^^^

- The progress of this :guilabel:`ImageSeekBar` increases from :kbd:`0%` to :kbd:`100%`.
- :guilabel:`Start Value` :kbd:`0.00` means the initial progress is :kbd:`0%`, and :guilabel:`End Value` :kbd:`1.00` means the progress reaches :kbd:`100%` at the end of one animation cycle.


.. figure:: https://foruda.gitee.com/images/1746689450841370398/8b4c10f8_10088396.png
   :align: center
   :width: 400px
   :name: Progress Setting

   Progress Setting

.. figure:: https://foruda.gitee.com/images/1746689390015636943/e654d618_10088396.gif
   :align: center
   :width: 400px
   :name: 0 - 100% Effect

   0 - 100% Effect

- Configure the :guilabel:`OnLoad` event of :guilabel:`ImageSeekBar` to trigger the animation playback immediately upon its appearance.

.. figure:: https://foruda.gitee.com/images/1746689431861966468/858c665b_10088396.png
   :align: center
   :width: 400px
   :name: Setting ImageSeekBar's OnLoad

   Setting ImageSeekBar's OnLoad

Multiple Animation Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- The image simultaneously rotates, changes transparency, and moves from the top-left corner to the bottom-right corner.

.. figure:: https://foruda.gitee.com/images/1746690669445832106/ef75fb51_10088396.gif
   :align: center
   :width: 400px
   :name: Multiple Animation Effect

   Multiple Animation Effect

- Configure the :guilabel:`OnLoad` event of :guilabel:`Image` to trigger the multiple animation playback immediately upon its appearance.

.. figure:: https://foruda.gitee.com/images/1746690703380172709/4942fe98_10088396.png
   :align: center
   :width: 400px
   :name: Multiple Animation Effect Trigger

   Multiple Animation Effect Trigger


.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Resource_Management:

Resource Management
-------------------

Only pre-imported image and font files can be referenced by the GUI
project. This chapter focuses on how to manage image and font resources.
The image and font explorer is located directly below the design view,
as shown in the figure below.

.. figure:: https://foruda.gitee.com/images/1727330892031779189/262d761a_12407535.jpeg
   :align: center
   :width: 750px

   Image Resource Management

.. figure:: https://foruda.gitee.com/images/1721629333990020010/27b6b02c_12407535.png
   :align: center
   :width: 750px

   Font Resource Management


.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Image_Resource_Management:

Image Resource Management
~~~~~~~~~~~~~~~~~~~~~~~~~

Click |image1| to bring up the Image Management view.

.. figure:: https://foruda.gitee.com/images/1726815968480737363/e2f46e96_12407535.png
   :align: center
   :width: 450px

Image Resource Management Window

Add Images
^^^^^^^^^^

Images can be added to the GUI project by following the process below.

1. Click |image2| to create a new image folder and enter the folder name. The created folder is located in the ``Resource\image`` folder under the GUI project directory.

.. figure:: https://foruda.gitee.com/images/1721629445428238950/c3e216ca_12407535.png
   :align: center
   :width: 500px

   Create Image Folder

2. Select the created image folder and click |image3| to select images (multiple selections are possible) to add them to the folder. As shown in the figure below, the images are copied to the ``Resource\image\home`` folder after the addition is completed.

.. figure:: https://foruda.gitee.com/images/1721629572036008240/98958d66_12407535.png
   :align: center
   :width: 500px

   Select Image Folder

.. figure:: https://foruda.gitee.com/images/1725412988097005313/b591b7ca_12407535.png
   :align: center
   :width: 750px

   Select Images

.. figure:: https://foruda.gitee.com/images/1721629616915297638/de3a62a7_12407535.png
   :align: center
   :width: 500px

   Add Image(s)

Remove Images/Image Folder
''''''''''''''''''''''''''

Select the image or image folder to be removed and click |image4|.

Rename Image Folder
'''''''''''''''''''

Select the image folder, double-click, and enter a new name.

Preview Images
^^^^^^^^^^^^^^

Select the image folder and all images in this folder will be displayed
in the right area.

.. figure:: https://foruda.gitee.com/images/1727330892031779189/262d761a_12407535.jpeg
   :align: center

   Preview Images

Refresh
^^^^^^^

If the user locally operates the image resources, not via Tool, click
|image5| to refresh.

.. note:: Not recommended.

Font Resource Management
~~~~~~~~~~~~~~~~~~~~~~~~

Add Third-Party Font
^^^^^^^^^^^^^^^^^^^^

If a third-party font (``.ttf``) is needed, click |image6| to import the
resource first; otherwise, the locally installed font will be used.

.. figure:: https://foruda.gitee.com/images/1710824483191984010/283b1f70_12407535.png
   :align: center
   :width: 750px

   Font Management

Remove Third-Party Font
^^^^^^^^^^^^^^^^^^^^^^^

Select the font to be removed and click |image7|.

.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Menu_Bar:

Menu Bar
--------

File
~~~~

Start Page
^^^^^^^^^^

To close the current project and open an existing project or create a new project, open the Start Page by clicking :menuselection:`File --> Start Page`. Click :guilabel:`Open Project` or select a ``.rtkprj`` and double-click to open the existing project, or click :guilabel:`Create Project` to create a new project. Please refer to :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_How_To_Create_Project` and :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_How_To_Open_Project`.

.. figure:: https://foruda.gitee.com/images/1721629731453730155/b3e315a3_12407535.png
   :align: center
   :width: 750px

   Start Page

Save
^^^^

Save all the UI changes of the project, the shortcut is :kbd:`Ctrl + S` .

Exit Save
^^^^^^^^^

A prompt window will pop up when closing the project, as shown below. Please click :guilabel:`OK` to save, or the changes will be abandoned.

.. figure:: https://foruda.gitee.com/images/1721629849823263224/4adf7819_12407535.png
   :align: center
   :width: 300px

   Close and Save Project

Edit
~~~~

Copy/Paste
^^^^^^^^^^

1. Click :menuselection:`Edit --> Copy` to copy the selected widget, the shortcut is
   :kbd:`Ctrl + C`.
2. Click :menuselection:`Edit --> Paste` to create a copy of the selected widget on the
   Design View, the shortcut is :kbd:`Ctrl + V`.

Delete
^^^^^^

Click :menuselection:`Edit --> Delete` to delete the selected widget, or press the
:kbd:`Delete` key on the keyboard.

Undo/Redo
^^^^^^^^^

Undo: Undo the change made to the widget, the shortcut is :kbd:`Ctrl + Z`.
Redo: Do the change to the widget again, the shortcut is :kbd:`Ctrl + Y`.

.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Convert_Project:

Convert Project
^^^^^^^^^^^^^^^

The Convert Project window is used to convert the IC type and screen
size/resolution for the current project.

.. figure:: https://foruda.gitee.com/images/1721630083752972113/2f32ba1a_12407535.png
   :align: center
   :width: 400px

   Convert Project

Project Name Modification
^^^^^^^^^^^^^^^^^^^^^^^^^

The Project Name window is used to modify the current project name.
Please enter the new name in the input box.

.. figure:: https://foruda.gitee.com/images/1721630055522858677/e4245c27_12407535.png
   :align: center
   :width: 400px

   Project Name

Setting
~~~~~~~

.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Image_Convert_Setting:

Image Convert Setting
~~~~~~~~~~~~~~~~~~~~~

The images must be converted to be displayed correctly on the IC, so
users need to set the correct convert parameters. All the optional
parameters are shown in the figure below.

.. figure:: https://foruda.gitee.com/images/1721630165031575995/1be72944_12407535.png
   :align: center
   :width: 500px

   Image Convert

The parameters are described as follows.

Scan Mode
^^^^^^^^^

The available options are shown in the table.

.. table:: Scan Mode Options
   :widths: 25 50
   :align: center 

   +------------+------------------+
   | Scan Mode  | Description      |
   +============+==================+
   | Horizontal | Horizontal scan. |
   +------------+------------------+
   | Vertical   | Vertical scan.   |
   +------------+------------------+

Color Space
^^^^^^^^^^^

Color space of Image, the available options are shown in the table
below.

.. table:: Color Space Options
   :align: center 

   +-------------+----------------------------------------------------------------------------------------------------------+
   | Color Space | Description                                                                                              |
   +=============+==========================================================================================================+
   | RGB565      | 16 bit RGB mode                                                                                          |
   |             |                                                                                                          |
   |             | Bit 4:0 represents blue; Bit 10:5 represents green; Bit 15:11 represents red.                            |
   +-------------+----------------------------------------------------------------------------------------------------------+
   | RTKARGB     | 16 bit ARGB mode                                                                                         |
   |             |                                                                                                          |
   |             | Bit 4:0 represents blue; Bit 9:5 represents green; Bit 14:10 represents red; Bit 15 represents alpha.    |
   +-------------+----------------------------------------------------------------------------------------------------------+
   | RTKRGAB     | 16 bit RGAB mode                                                                                         |
   |             |                                                                                                          |
   |             | Bit 4:0 represents blue; Bit 5 represents alpha; Bit 10:6 represents green; Bit 15:11 represents red.    |
   +-------------+----------------------------------------------------------------------------------------------------------+
   | RGB         | 24 bit RGB mode                                                                                          |
   |             |                                                                                                          |
   |             | Bit 7:0 represents blue; Bit 15:8 represents green; Bit 23:16 represents red.                            |
   +-------------+----------------------------------------------------------------------------------------------------------+
   | RGBA        | 32 bit RGBA mode                                                                                         |
   |             |                                                                                                          |
   |             | Bit 7:0 represents blue; Bit 15:8 represents green; Bit 23:16 represents red; Bit 31:24 represents alpha.|
   +-------------+----------------------------------------------------------------------------------------------------------+
   | BINARY      | 2-value (0 or 1) image.                                                                                  |
   +-------------+----------------------------------------------------------------------------------------------------------+

Compress
^^^^^^^^

If checked :guilabel:`Compress`, please set the compression parameter as needed.
The optional Compress Mode is as follows:

1. RLE

Run-Length Encoding, a lossless compression algorithm.

If selecting RLE as the Compress Mode, RLE Level and RLE Run Length
parameters are mandatory to set.

.. figure:: https://foruda.gitee.com/images/1726818028936954872/d4977120_12407535.png
   :align: center
   :width: 400px

   RLE Level - Level 1

.. figure:: https://foruda.gitee.com/images/1726818316484821859/eeb87b7f_12407535.png
   :align: center
   :width: 400px

   RLE Level - Level 2

.. table:: RLE Level
   :align: center

   +--------------------------------+--------------------------------------+
   | RLE Level                      | Description                          |
   +================================+======================================+
   | Level 1                        | 1-level compress.                    |
   +--------------------------------+--------------------------------------+
   | Level 2                        | 2-level compress, secondary compress |
   |                                | based on the 1-level compress.       |
   +--------------------------------+--------------------------------------+

.. table:: RLE Run Length
   :align: center

   +----------------+-----------------------+
   | RLE Run Length | Description           |
   +================+=======================+
   | Byte_1         | 1 byte, Maximum 255.  |
   +----------------+-----------------------+
   | Byte_2         | 2 bytes, Maximum 255. |
   +----------------+-----------------------+

.. note:: RLE Run Length: Maximum length of duplicate characters allowed per stroke (Run) during compression.

2. FastLz

   A dictionary-and-sliding-window based lossless compression algorithm
   for compressing data with a large number of repetitive values.

3. YUV_Sample_Blur

   A lossy compression algorithm combining YUV sampling and blurring.

   YUV Sample: Keep the luminance information of the image and only
   sample the chrominance information.

   Blur: Discard the lower bit of each byte after YUV sampling to
   achieve the purpose of data compression.

.. table:: YUV Sample Mode
   :align: center

   +-----------------+----------------------------------------------------------------------------------------------------------------------------------------------+
   | YUV Sample Mode | Description                                                                                                                                  |
   +=================+==============================================================================================================================================+
   | YUV444          | 4 pixel data are sampled to 4 Y, 4 U and 4 V, i.e., each Y corresponds to a set of UV components, fully preserving the YUV data.             |
   +-----------------+----------------------------------------------------------------------------------------------------------------------------------------------+
   | YUV422          | Every 4 pixel data are sampled to 4 Y, 2 U and 2 V, i.e., every 2 Y corresponds to a set of UV components, data size is 75% of the original. |
   +-----------------+----------------------------------------------------------------------------------------------------------------------------------------------+
   | YUV411          | Every 4 pixel data are sampled to 4 Y, 1 U and 1 V, i.e., every 4 Y corresponds to a set of UV components, data size is 50% of the original. |
   +-----------------+----------------------------------------------------------------------------------------------------------------------------------------------+
   | YUV422          | Y - luminance; V - chrominance.                                                                                                              |
   +-----------------+----------------------------------------------------------------------------------------------------------------------------------------------+

.. table:: Blur Mode
   :align: center 

   +-----------+--------------------------------------------------------+
   | Blur Mode | Description                                            |
   +===========+========================================================+
   | Bit0      | Saving without discarding lower bit.                   |
   +-----------+--------------------------------------------------------+
   | Bit1      | Each byte discards bit0 (preserve [bit7:bit1]).        |
   +-----------+--------------------------------------------------------+
   | Bit2      | Each byte discards [bit1:bit0] (preserve [bit7:bit2]). |
   +-----------+--------------------------------------------------------+
   | Bit4      | Each byte discards [bit3:bit0] (preserve [bit7:bit4]). |
   +-----------+--------------------------------------------------------+

4. YUV_Sample_Blur+FastLz

The algorithm combines YUV_Sample_Blur and FastLz.

.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Font_Convert_Setting:

Font Convert Setting
~~~~~~~~~~~~~~~~~~~~

Include Bitmap Fonts
and Vector Fonts. Fonts supported by Realtek series ICs are shown in the
table below.

.. note:: A Font Convert Setting should be created, otherwise selecting a font for the text-type widget in the Property View is not possible.
   
.. table:: Supported Fonts Type
   :align: center 

   +--------+-------------+-------------+-----+
   | Font   | 8762D/8763E | 8762G/8772G | TBD |
   +========+=============+=============+=====+
   | Vector |      N      |      N      |  Y  |
   +--------+-------------+-------------+-----+
   | Bitmap |      Y      |      Y      |  Y  |
   +--------+-------------+-------------+-----+

To use Bitmap Fonts, set the following parameters.

.. figure:: https://foruda.gitee.com/images/1721630232320631022/466a71af_12407535.png
   :align: center
   :width: 500px

   Convert Settings of Bitmap Fonts

The following table lists the description of each parameter.
   
.. table:: Font Convert Parameters
   :align: center

   +-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
   | Parameters        | Description                                                                                                                                                                                            |
   +===================+========================================================================================================================================================================================================+
   | Font Setting Name | User-defined font setting name.                                                                                                                                                                        |
   |                   | Please make sure that you create a different font setting name each time.                                                                                                                              |
   +-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
   | Font Size         | Font size.                                                                                                                                                                                             |
   +-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
   | Bold              | Bold or not.                                                                                                                                                                                           |
   +-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
   | Italic            | Italic or not.                                                                                                                                                                                         |
   +-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
   | Render Mode       | Bit number used to represent a pixel in the converted ``.bin`` file.                                                                                                                                   |
   +-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
   | Scan Mode         | There are two ways to scan when saving ``.bin``.                                                                                                                                                       |
   |                   |                                                                                                                                                                                                        |
   |                   | H: Horizontal scanning                                                                                                                                                                                 |
   |                   |                                                                                                                                                                                                        |
   |                   | V: Vertical scanning                                                                                                                                                                                   |
   +-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
   | Index Method      | Index method of the converted .bin’s re-indexing area.                                                                                                                                                 |
   +-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
   | Code Page         | Support multiple code pages.                                                                                                                                                                           |
   +-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
   | Text Type         | The types are as follows.                                                                                                                                                                              |
   |                   |                                                                                                                                                                                                        |
   |                   | Range: If the text’s Unicode range can be pre-determined, please select this type and enter the range in the Range TextBox. Multiple ranges are supported, please set each range on a separate line.   |
   |                   |                                                                                                                                                                                                        |
   |                   | Note: Only the characters within the ranges will be converted and saved to ``.bin`` file, which can save storage space.                                                                                |
   |                   |                                                                                                                                                                                                        |
   |                   | Random: If the text’s Unicode range cannot be pre-determined, please select this type.                                                                                                                 |
   |                   |                                                                                                                                                                                                        |
   |                   | Note: All characters of the Font will be converted and saved to ``.bin`` file.                                                                                                                         |
   +-------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Vector Font parameters are shown in the figure below.

.. figure:: https://foruda.gitee.com/images/1721630326763929958/7a1b4010_12407535.png
   :align: center
   :width: 500px

   Vector Font Parameters

Export
~~~~~~

If you have finished designing the GUI project and want to program it to
the IC, please click :guilabel:`Export`, then the Tool performs the following
actions:

1. Image convert
2. Font convert
3. Pack the ``.xml``, ``.js``, images and fonts into the output ``.bin``.

When the above actions are done, a message box pops up.

.. figure:: https://foruda.gitee.com/images/1721630355854380192/60bbc0af_12407535.png
   :align: center
   :width: 500px

   Output .bin

The ``.bin`` can be programmed into your IC.

Simulate
~~~~~~~~

Simulate the project on UI.

.. note::
   
   When simulating the project for the first time, please click :guilabel:`Export` before clicking :guilabel:`Simulate`. Then, there is no need to click :guilabel:`Export` again if no image or font setting is modified.

.. figure:: https://foruda.gitee.com/images/1710828505193590425/a8c6e2c0_12407535.png
   :align: center
   :width: 400px

   Running Simulator

.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Quick_Start_Tutorials:

Quick Start to Tutorials
------------------------

.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_How_To_Create_Project:

How to Create Project
~~~~~~~~~~~~~~~~~~~~~

.. figure:: https://foruda.gitee.com/images/1721630502223709854/341d9828_12407535.png
   :align: center
   :width: 750px

   Start Page

Double click and run ``RVisualDesigner.exe``, and then configure the
project step by step (1~4) and click :guilabel:`Create Project` (5). After
creation, the GUI design window pops up. The left side is the component
area, the center is the design area, and the right side is the widget
property setting area.

.. figure:: https://foruda.gitee.com/images/1726818698027723674/8962d02e_12407535.png
   :align: center
   :width: 750px

   GUI Design

.. note:: The newly created project file is located in the project folder under the Solution Folder. There is an example as shown in the figure below.

.. figure:: https://foruda.gitee.com/images/1725505762180091944/39be3e5a_12407535.png
   :align: center
   :width: 400px

   Project Folder

After dragging and dropping a widget on Design View, and clicking :menuselection:`File --> Save`
or pressing :kbd:`Ctrl + S`, the ``.rtkui`` file will be created.

.. figure:: https://foruda.gitee.com/images/1725505619255178068/e89d3b76_12407535.png
   :align: center
   :width: 400px

   .rtkui File

How to Write Javascript Code
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After the project is created, the ``xxx.js`` file is created. The
``xxx.js`` file is empty, please code here to implement the widgets'
event callback.

.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_How_To_Open_Project:

How to Open Project
~~~~~~~~~~~~~~~~~~~

.. figure:: https://foruda.gitee.com/images/1721636293745281159/b5fbf3b0_12407535.png
   :align: center
   :width: 750px

   Open Project

There are two ways to open a project.

1. Click :guilabel:`Open Project` and select a ``.rtkprj`` file.

.. figure:: https://foruda.gitee.com/images/1724911708175163337/8513c36b_12407535.png
   :align: center
   :width: 750px

   Open Project via Selecting .rtkprj

2. Select a ``.rtkprj`` in the Recent Project area.

If the project is listed in the Recent Project area, a message window pops up.

.. figure:: https://foruda.gitee.com/images/1721636512948372776/7f7820d5_12407535.png
   :align: center
   :width: 400px

   Message Box

How to Open/Close Project
~~~~~~~~~~~~~~~~~~~~~~~~~

Click :menuselection:`File --> Start Page` on Menu Bar.

How to Export/Pack Project
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. figure:: https://foruda.gitee.com/images/1721636632023023311/9f459245_12407535.png
   :align: center
   :width: 750px

   Export

Click :guilabel:`Export` on Menu Bar. The output is shown in the figure below.

.. figure:: https://foruda.gitee.com/images/1721636703955653703/deacfb05_12407535.png
   :align: center
   :width: 400px

   Export OK

How to Simulate
~~~~~~~~~~~~~~~

.. figure:: https://foruda.gitee.com/images/1721636770252167619/8cfcafa3_12407535.png
   :align: center
   :width: 750px

   Simulate

Click on the :guilabel:`Simulate` button in the menu bar.

.. _{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_GUI_Demo_Project:

GUI Demo Project
----------------

There is a Demo in ``RVisualDesigner-vx.x.x.x.zip``.

The folder - 454x454 contains a project with resolution 454*454.

The folder - 480x480 contains a project with resolution 480*480.

.. figure:: https://foruda.gitee.com/images/1721636869409777562/bc385a52_12407535.png
   :align: center
   :width: 580px

   Demo

Please follow the steps to demo the project.

1. Open the project according to the screen size/resolution of your IC;
2. Check the IC type by clicking :menuselection:`Edit --> Convert Project` on the Menu Bar. Please refer to :ref:`{9B9FF45D-F6CE-434E-80EE-5B233D4AA1FB}_Convert_Project` for details. If the current IC type of the project does not match your IC, please select the target IC type, enter the target resolution, and click :guilabel:`Convert`.

.. figure:: https://foruda.gitee.com/images/1721636942634993243/11b1e212_12407535.png
   :align: center
   :width: 400px

   Convert Project

3. Click :guilabel:`Export` on the Menu Bar and wait until the export ok/fail message box pops up.

.. figure:: https://foruda.gitee.com/images/1721637000052375270/5e3f1873_12407535.png
   :align: center
   :width: 400px

   Output .bin

Program the output ``.bin`` into your IC.

.. |icon| image:: https://foruda.gitee.com/images/1710818993805990437/796af0dc_12407535.png
.. |image1| image:: https://foruda.gitee.com/images/1710824254768634573/d752fbd6_12407535.png
.. |image2| image:: https://foruda.gitee.com/images/1710824254786867861/174e524d_12407535.png
.. |image3| image:: https://foruda.gitee.com/images/1710824254769021374/55b6c303_12407535.png
.. |image4| image:: https://foruda.gitee.com/images/1710824483012674071/c0c62b91_12407535.png
.. |image5| image:: https://foruda.gitee.com/images/1710824483020058086/3ecd6f6d_12407535.png
.. |image6| image:: https://foruda.gitee.com/images/1710824482944711702/8b73005d_12407535.png
.. |image7| image:: https://foruda.gitee.com/images/1710824483012674071/c0c62b91_12407535.png
