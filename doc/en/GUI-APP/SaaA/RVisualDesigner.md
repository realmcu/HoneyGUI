## Overview
RTKIOT Visual Designer is a tool to create graphical interface designs for Realtek series ICs; its currently supported ICs are shown in the table below.

|  No | Supported ICs |
|-----|---------------|
|  1  |    RTL8762D   |
|  2  |    RTL8762G   |
|  3  |    RTL8763E   |
|  4  |    RTL8772G   |
|  5  |    TBD        |

RTKIOT Visual Designer supports:
+ Drag the widgets from the toolbox and drop them in the Design View.
+ Drag and drop the widget to change its position in the Design View, or modify the position and appearance of the widget via the Property View.
+ Export the user-designed GUI project to ```.bin```, and the ```.bin``` can be programmed into the IC to display the graphical interface.
+ Simulate the GUI project on a PC.

This document mainly consists of:
+ [Function Panels](#function_panel_anchor)
+ [Resource Management](#resource_manage_anchor)
+ [Menu Bar](#menu_anchor)
+ [Quick Start to Tutorials](#quick_start_anchor)
+ [GUI Demo Project](#gui_demo_anchor)

To simplify the document, 'Tool' is used below to refer to the 'RTKIOT Visual Designer'.

<div STYLE="page-break-after: always;"></div>

<a id="function_panel_anchor"></a>

## Function Panels

### Toolkit/Widgets
+ Non-containerized widget
    + Can be used as the parent of other widgets.
    + There is a coordinate-following relationship between parent and child widgets.
    + Visible when the child widget is out of range of the parent widget.
+ Container widget
    + Can be used as the parent of other widgets.
    + There is a coordinate-following relationship between parent and child widgets.
    + Visible when the child widget is out of range of the parent widget.
    + Can drag and drop a widget from the toolbox into the container widget.

This section lists the properties supported by the widget in tables and marks with 'Y' or 'N' to indicate whether the IC supports them or not.


#### Non-containerized Widget
##### Text
Used only for text display and does not support user input. The properties are shown in the table below.

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name          | Widget name.   | Y	    | Y			  | Y	|
| Size (Height) | Widget height. | Y		| Y			  | Y	|
| Size (Width)  | Widget width.  | Y		| Y			  | Y	|
| X | Horizontal coordinate relative to the parent widget. | Y | Y | Y |
| Y | Vertical coordinate relative to the parent widget.   | Y | Y | Y |
| Text | Display text. | Y | Y | Y |
| Display Mode | Long text (text content beyond the widget's range) display mode with the following supported types.<br/>truncate: Truncated display mode<br/>verticalscroll: Vertical scrolling display mode<br/>horizontalscroll: Horizontal scrolling display mode. | Y | Y | Y |
| Font | Font convert setting, please refer to [Font Convert Setting](#font_setting_anchor). | Y | Y | Y |
| Font Color (RGBA) | Font color setting, use RGBA. | Y | Y | Y |


##### Button
Clickable widget with text and background image. The properties are shown in the table below.

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | Y |
| Size (Height) | Widget height. | Y | Y | Y |
| Size (Width) | Widget width. | Y | Y | Y |
| X | Horizontal coordinate relative to the parent widget. | Y | Y | Y |
| Y | Vertical coordinate relative to the parent control. | Y | Y | Y |
| Text | Displayed text. | Y | Y | Y |
| Text X | Horizontal coordinate relative to the Button widget. | Y | Y | Y |
| Text Y | Vertical coordinate relative to the Button widget. | Y | Y | Y |
| Display Mode | Horizontal or Vertical display. | Y | Y | Y |
| Font | Font convert setting, please refer to [Font Convert Setting](#font_setting_anchor). | Y | Y | Y |
| Text Color (RGB) | Text color setting, use RGB. | Y | Y | Y |
| Transition | Image transition mode with the following options:<br/>normal: No effect<br/>fade: Fade-in/out<br/>scale: Scaling<br/>fadeScale: Fade-in/out and scaling<br/>Note: Set the transition mode is effective only if set the default and highlight background image, otherwise all normal. | N | Y | Y |
| BG Image (Default) | Default background image. | Y | Y | Y |
| BG Image (Highlight) | Selected/Highlight background image. | Y | Y | Y |
| BG Image Rotation Angle | Background image rotation angle, range: 0~360 degrees. | Y | Y | Y |


##### Image
Widget that can set image. The properties are shown in the table below.

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | Y |
| Size (Height) | Widget height. | Y | Y | Y |
| Size (Width) | Widget width. | Y | Y | Y |
| X | Horizontal coordinate relative to the parent widget. | Y | Y | Y |
| Y | Vertical coordinate relative to the parent control. | Y | Y | Y |
| Image | Image Path<br/>Note: The image must be pre-imported into the project. Please refer to [Image Resource Management](#img_res_m_anchor) for details. | Y | Y | Y |
| Image Rotation Angle | Image rotation angle. | Y | Y | Y |
| Image Scale X | Image horizontal scaling degree, is a multiplier/percentage.<br/>For example, set scale x 0.5 means that the actual display width of the image is half of the original image width. | Y | Y | Y |
| Image Scale Y | Image vertical scaling degree, is a multiplier/percentage. | Y | Y | Y |

```{note}
1.	When exporting, tool will convert the imported images. And the image conversion parameters can be set in 'Menu Bar -> Setting -> Image Convert Setting', please refer to [Image Convert Setting](#img_conv_setting_anchor) for details.
2.	If the size of the imported image does not match the size of the widget, tool doesn't scale or crop the image.
```

##### SeekBar
Sliding widget that can respond to user swipe gesture with the widget and change the progress value. The properties are shown in the table below.

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | Y |
| Size (Height) | Widget height. | Y | Y | Y |
| Size (Width) | Widget width. | Y | Y | Y |
| X | Horizontal coordinate relative to the parent widget. | Y | Y | Y |
| Y | Vertical coordinate relative to the parent control. | Y | Y | Y |
| Color(Highlight) (RGBA) | Background color of partially completed part of progress bar. | N | Y | N |
| Color (RGBA) | Background color of the whole progress bar. | N | Y | N |
| Orientation | Widget display orientation and gesture response orientation with the following types:<br/>vertical/V: Vertical orientation<br/>arc: Direction of a curve<br/>horizontal/H: Horizontal orientation | Y | Y | Y |

<div id="figure_2_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817870508689816/f43215ff_12407535.png" style="width: 400px"></div>

##### Image SeekBar
Sliding widget with multi-images as background, and switch to different images as user swipes. The properties are shown in the table below.

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | Y |
| Size (Height) | Widget height. | Y | Y | Y |
| Size (Width) | Widget width. | Y | Y | Y |
| X | Horizontal coordinate relative to the parent widget. | Y | Y | Y |
| Y | Vertical coordinate relative to the parent control. | Y | Y | Y |
| Degree (Start) | Start degree (Invalid if orientation is arc). | Y | Y | Y |
| Degree (End) | End degree (Invalid if orientation is arc). | Y | Y | Y |
| Image Directory | Folder that contains only the images to be displayed on this widget.<br/>Notes:<br/>1. Please sort the images by name;<br/>2. When the user swipes on the widget, the widget will switch the background image according to the current progress. | Y | Y | Y |
| Central X | Horizontal coordinate of the center of the arc relative to the parent widget. | Y | Y | Y |
| Central Y | Vertical coordinate of the center of the arc relative to the parent widget. | Y | Y | Y |
| Orientation | Widget display orientation and gesture response orientation with the following types:<br/>vertical/V: Vertical orientation<br/>arc: Direction of a curve<br/>horizontal/H: Horizontal orientation | Y | Y | Y |

##### Switch
Switch widget with 'Checked' and 'Unchecked' states. The properties are shown in the table below.

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | Y |
| Size (Height) | Widget height. | Y | Y | Y |
| Size (Width) | Widget width. | Y | Y | Y |
| X | Horizontal coordinate relative to the parent widget. | Y | Y | Y |
| Y | Vertical coordinate relative to the parent control. | Y | Y | Y |
| BG Image (Checked) | Checked state background image. | Y | Y | Y |
| BG Image (Default) | Unchecked state background image. | Y | Y | Y |

##### Arc
Arc widget, no gesture support yet. The properties are shown in the table below.

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | N |
| Size (Height) | Widget height. | Y | Y | N |
| Size (Width) | Widget width. | Y | Y | N |
| X | Horizontal coordinate relative to the parent widget. | Y | Y | N |
| Y | Vertical coordinate relative to the parent control. | Y | Y | N |
| Central X | Horizontal coordinate of the center of the arc relative to the parent widget. |  N | Y | N |
| Central Y | Vertical coordinate of the center of the arc relative to the parent widget. | N | Y | N |
| BG Color | Arc background color. | N | Y | N |
| Cap Mode | Arc cap mode, the following options are supported:<br/>round/butt/square. | N | Y | N |
| Degree (End) | End degree of arc. | N | Y | N |
| Degree (Start) | Start degree of arc. | N | Y | N |
| Radius | Radius of arc. | N | Y | N |
| Stroke Width | Width of arc stroke. | N | Y | N |

<div id = "figure_2_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817869829442326/90524d4c_12407535.png"/></div>

#### Container Widget
##### Screen
Screen widget, corresponding to the physical screen, is the root widget of a GUI project. The properties are shown in the table below.

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | Y |
| Size (Height) | Widget height. | Y | Y | Y |
| Size (Width) | Widget width. | Y | Y | Y |
| X | Horizontal coordinate, always 0. | Y | Y | Y |
| Y | Vertical coordinate, always 0. | Y | Y | Y |

```{note}
Only 'Name' property can be modified.
```

##### TabView and Tab
With the Tab widget as a child widget, it supports up/down/left/right swiping to switch among Tabs. 
The properties of TabView and Tab are shown in the table below.

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | Y |
| Size (Height) | Widget height. | Y | Y | Y |
| Size (Width) | Widget width. | Y | Y | Y |
| X | Horizontal coordinate relative to the parent widget, always 0. | Y | Y | Y |
| Y | Vertical coordinate relative to the parent widget, always 0. | Y | Y | Y |
| Transition | Tab transition mode with the following supported types:<br/>- normal: No effect<br/>- fade: Fade-in/out<br/>- scale: Scaling<br/>- fadeScale: Fade-in/out and scaling | N | Y | Y |
<br/>

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | Y |
| Size (Height) | Widget height. | Y | Y | Y |
| Size (Width) | Widget width. | Y | Y | Y |
| X | Horizontal coordinate relative to the TabView widget, always 0. | Y | Y | Y |
| Y | Vertical coordinate relative to the TabView widget, always 0. | Y | Y | Y |
| Index(X-Axis) | Horizontal index of Tabs in TabView. | Y | Y | Y |
| Index(Y-Axis) | Vertical index of Tabs in TabView. | Y | Y | Y |

<div id = "figure_2_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817871000642675/ca6cda22_12407535.png" width="300"/></div>
<br/>

```{note}
1.	TabView width and height cannot be modified, defaulting to the Screen's width and height. 
2.	TabView horizontal and vertical coordinates cannot be modified, always being 0.
3.	TabView can only be used as a child of the Screen widget.
4.	TabView's child widgets can only be Tabs.
5.	Tab's width and height cannot be modified, defaulting to TabView's width and height.
6.	Tab's horizontal and vertical coordinates cannot be modified and are always 0.
```

##### Page
Container widget with scrollable content.

```{note}
1.	Page only supports vertical scrolling.
2.	The width and height of the Page widget only define the area of the interface that can respond to a swipe gesture. Whether scrolling is allowed depends on whether or not the child widget added to it is outside the scope of the screen.
```

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | Y |
| Size (Height) | Widget height. | Y | Y | Y |
| Size (Width) | Widget width. | Y | Y | Y |
| X | Horizontal coordinate relative to the parent widget. | Y | Y | Y |
| Y | Vertical coordinate relative to the parent control. | Y | Y | Y |

##### Win
Within the area defined by Win width and height, it can respond to various gestures, including click, long click, press, press release, and swipe. The properties are shown in the table below.

| Property | Description | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|----------|-------------|:-----------:|:-----------:|:---:|
| Name | Widget name. | Y | Y | Y |
| Size (Height) | Widget height. | Y | Y | Y |
| Size (Width) | Widget width. | Y | Y | Y |
| X | Horizontal coordinate relative to the parent widget. | Y | Y | Y |
| Y | Vertical coordinate relative to the parent control. | Y | Y | Y |
| Hidden | Indicate whether Win and its child widget need to be hidden. | Y | Y | Y |

### Design View/Canvas
Users can drag and drop widgets from the Toolbox panel into the Design View, adjust the widgets' layout, and set properties to design a graphical interface that can be rendered in the Realtek ICs.

<div style="text-align:center;"><img src="https://foruda.gitee.com/images/1721627839639298235/033d3a5b_12407535.png" width="750"></div>

#### TabView - Create/Delete/Insert Tab
Drag and drop the TabView widget from the Toolbox into the Design View, then a TabView that contains only a home tab (coordinates (0,0)) is created, as shown in the figure below.

<div style="text-align:center;"><img src="https://foruda.gitee.com/images/1721627964036015694/3b5ee0d4_12407535.png" width="750"></div>

##### Create Tab
New tabs can be created by clicking the buttons around the Design View.

```{note}
1.	If idx is 0, up and down button is enabled.
2.	If idy is 0, left and right button is enabled.
```

##### Delete Tab
Select the tab to be deleted, click 'Edit -> Delete' on the menu bar or press the Delete key on the keyboard. Then double-check if the deletion is intended.

<div id = "figure_2_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628091462645405/beca8dd5_12407535.png" width="220"></div>

##### Insert Tab
Currently, tab insertion is only supported by modifying the coordinates of an existing tab and creating a new one.

For example, if a tab needs to be inserted between tabs with coordinates (1, 0) and (2, 0), the steps are as follows.
1.	Increase the idx of Tab (2, 0) and the tabs to its right by 1, as shown in the figure below;
2.	Switch to Tab (1, 0) and click to create the new Tab (2, 0).

<div id="figure_2_7_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726815120686817698/cdf44743_12407535.png" width="750"></div>
<br/>
<div id="figure_2_7_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726815280073982051/68230d3d_12407535.png" width="750"></div>
<br/>
<div id="figure_2_7_c" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628460504196478/3446e06e_12407535.png" width="750"></div>

#### TabView Overview Window
Please click ![icon](https://foruda.gitee.com/images/1710818993805990437/796af0dc_12407535.png)  to show the 'TabView Overview Window'.

```{note}
1.	The highlighted Tab in the Overview Chart indicates the Tab that is currently being edited in Design View.
2.	The Overview Chart labels each Tab with its coordinates. When simulated or rendered in ICs, the Tab with coordinates (0,0) is displayed on the Home page, and users can swipe up/down/left/right to display other Tabs.
```

<div id="figure_2_8_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628617214746327/0711439d_12407535.png" width="750"></div>
<br/>
<div id="figure_2_8_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628633709549622/a41c6011_12407535.png" width="750"></div>


#### Zoom of Design View
There are 3 ways to zoom in the Design View.
1.	Press the ```Ctrl``` key and wheel mouse.
2.	Click the '-' and '+' buttons.
3.	Drag the slider bar.

<div id="figure_2_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628744851323697/d56f9725_12407535.png" width="750"></div>

### Property View
Selecting a widget in the Widget Tree or Design View exposes all of the widget's property values, which users can modify as needed.

<div id="figure_2_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726814067710788486/a6564f40_12407535.png" width="750"></div>

### Widget Tree
The Widget Tree is used to present to the users the parent/child/sibling relationship of the currently laid out widgets. And we have the following convention here.
1.	The child widget layer is on top of the parent widget layer, i.e., when the parent and child widget overlap, the child widget will cover the parent widget.
2.	The layer of sibling widgets is related to the order in which the widgets are added, with widgets added first at the bottom and widgets added later at the top.

The figure shows all the child widgets of the Home tab and Lamp tab, where the Home tab has only one Image child widget for setting the background, and the Lamp tab contains an Image widget and several Switch widgets.

<div id="figure_2_11" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628970571915820/15772594_12407535.png" width="750"></div>
<br/>
<div id="figure_2_11_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628988181629026/d9ff763a_12407535.png" width="750"></div>

Widget Tree supports the following operations.
1.	Select widget:
If a widget is selected on the Widget Tree, the corresponding widget in the Design View focuses and its properties are shown on Property View.
2.	Modify the parent-child relationship: 
Select a widget on the Widget Tree (except Tab/TabView/Screen) and drag-and-drop it on the target widget item. Then the widget will be a child widget of the target widget.
3.	Modify widget layers: 
Select a widget on the Widget Tree (except Tab/TabView/Screen) and drag-and-drop it to the upper or lower edge of the target widget item. Then on the Design View, the widget will be placed over or under the target widget.
4.	Lock widgets:
Click the button and lock the widget/widgets.
    + If the lock button of the screen is clicked, all the screen's child widgets will be locked, and the user could not drag or resize the widgets on Design View.
    + If the lock button of the Tab is clicked, all the tab's child widgets will be locked, and the user could not drag or resize the widgets on Design View.

<div id="figure_2_12" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726816487543506275/da6ebe9d_12407535.png"></div>
<br/>
<div id="figure_2_12_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726816816056161349/efcdd833_12407535.png"></div>

<div STYLE="page-break-after: always;"></div>

<a id="resource_manage_anchor"></a>

## Resource Management

Only pre-imported image and font files can be referenced by the GUI project. This chapter focuses on how to manage image and font resources.
The image and font explorer is located directly below the design view, as shown in the figure below.

<div id="figure_3_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629333990020010/27b6b02c_12407535.png" width="750"></div>

<a id="img_res_m_anchor"></a>

### Image Resource Management

Click ![icon](https://foruda.gitee.com/images/1710824254768634573/d752fbd6_12407535.png) to bring up the Image Management view.

<div id="figure_3_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726815968480737363/e2f46e96_12407535.png" width="500"></div>

##### Add Images
Images can be added to the GUI project by following the process below.
1.	Click ![icon](https://foruda.gitee.com/images/1710824254786867861/174e524d_12407535.png) to create a new image folder and enter the folder name.
The created folder is located in the ```Resource\image``` folder under the GUI project directory.

<div id="figure_3_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629445428238950/c3e216ca_12407535.png" width="500"></div>
<br/>

2.	Select the created image folder and click ![icon](https://foruda.gitee.com/images/1710824254769021374/55b6c303_12407535.png) to select images (multiple selections are possible) to add them to the folder.
As shown in the figure below, the images are copied to the ```Resource\image\home``` folder after the addition is completed.

<div id="figure_3_4_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629572036008240/98958d66_12407535.png" width="500"></div>
<br/>
<div id="figure_3_4_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1725412988097005313/b591b7ca_12407535.png" width="750"></div>
<br/>
<div id="figure_3_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629616915297638/de3a62a7_12407535.png" width="500"></div>

##### Remove Images/Image Folder
Select the image or image folder to be removed and click ![icon](https://foruda.gitee.com/images/1710824483012674071/c0c62b91_12407535.png).
</br>

##### Rename Image Folder
Select the image folder, double-click, and enter a new name.

#### Preview Images
Select the image folder and all images in this folder will be displayed in the right area.

<div id="figure_3_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1727330892031779189/262d761a_12407535.jpeg"></div>

#### Refresh
If the user locally operates the image resources, not via Tool, click ![icon](https://foruda.gitee.com/images/1710824483020058086/3ecd6f6d_12407535.png) to refresh.

```{note}
Not recommended.
```

### Font Resource Management

#### Add Third-Party Font
If a third-party font (```.ttf```) is needed, click ![icon](https://foruda.gitee.com/images/1710824482944711702/8b73005d_12407535.png) to import the resource first; otherwise, the locally installed font will be used.

<div id="figure_3_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824483191984010/283b1f70_12407535.png" width="750"></div>

#### Remove Third-Party Font
Select the font to be removed and click ![icon](https://foruda.gitee.com/images/1710824483012674071/c0c62b91_12407535.png).

<div STYLE="page-break-after: always;"></div>

<a id="menu_anchor"></a>

## Menu Bar

### File
#### Start Page
To close the current project and open an existing project or create a new project, open the Start Page by clicking 'File -> Start Page'. Click 'Open Project' or select a ```.rtkprj``` and double-click to open the existing project, or click 'Create Project' to create a new project. Please refer to [How to Create Project](#how_to_create_anchor) and [How to Open Project](#how_to_open_anchor).


<div id="figure_4_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629731453730155/b3e315a3_12407535.png" width="750"></div>

#### Save
Save all the UI changes of the project, the shortcut is ```Ctrl + S```.

#### Exit Save
A prompt window will pop up when closing the project, as shown below. Please click 'OK' to save, or the changes will be abandoned.

<div id="figure_4_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629849823263224/4adf7819_12407535.png" width="300"></div>

### Edit
#### Copy/Paste
1.	Click 'Edit -> Copy' to copy the selected widget, the shortcut is ```Ctrl + C```.
2.	Click 'Edit -> Paste' to create a copy of the selected widget on the Design View, the shortcut is ```Ctrl + V```.

#### Delete
Click 'Edit -> Delete' to delete the selected widget, or press the 'Delete' key on the keyboard.

#### Undo/Redo
Undo: Undo the change made to the widget, the shortcut is ```Ctrl + Z```.
Redo: Do the change to the widget again, the shortcut is ```Ctrl + Y```.

#### Convert Project
The Convert Project window is used to convert the IC type and screen size/resolution for the current project.

<div id="figure_4_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630083752972113/2f32ba1a_12407535.png" width="500"></div>

#### Project Name Modification
The Project Name window is used to modify the current project name. Please enter the new name in the input box.
 
<div id="figure_4_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630055522858677/e4245c27_12407535.png" width="500"></div>

### Setting

{#img_conv_setting_anchor}
#### Image Convert Setting

The images must be converted to be displayed correctly on the IC, so users need to set the correct convert parameters. All the optional parameters are shown in the figure below.

<div id="figure_4_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630165031575995/1be72944_12407535.png" width="500"></div>

The parameters are described as follows.

##### Scan Mode
The available options are shown in the table.

| Scan Mode | Description |
|-----------|-------------|
| Horizontal | Horizontal scan.|
| Vertical | Vertical scan.  |

##### Color Space
Color space of Image, the available options are shown in the table below.

| Color Space | Description |
|-------------|-------------|
| RGB565 | 16 bit RGB mode<br>Bit 4:0 represent blue; Bit 10:5 represent green; Bit 15:11 represent red. |
| RTKARGB | 16 bit ARGB mode<br>Bit 4:0 represent blue; Bit 9:5 represent green; Bit 14:10 represent red; Bit 15 represent alpha. |
| RTKRGAB | 16 bit RGAB mode<br>Bit 4:0 represent blue; Bit 5 represent alpha; Bit 10:6 represent green; Bit 15:11 represent red. |
| RGB | 24 bit RGB mode<br>Bit 7:0 represent blue; Bit 15:8 represent green; Bit 23:16 represent red. |
| RGBA | 32 bit RGBA mode<br>Bit 7:0 represent blue; Bit 15:8 represent green; Bit 23:16 represent red; Bit 31:24 represent alpha. |
| BINARY | 2-value (0 or 1) image. |

##### Compress
If checked 'Compress', please set the compression parameter as needed.
The optional Compress Mode is as follows:
1.	RLE

    Run-Length Encoding, a lossless compression algorithm.
If selecting RLE as the Compress Mode, RLE Level and RLE Run Length parameters are mandatory to set.

<div id="figure_4_6_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726818028936954872/d4977120_12407535.png" width="400"></div>
<br/>
<div id="figure_4_6_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726818316484821859/eeb87b7f_12407535.png" width="400"></div>
<br/>

| RLE Level | Description |
|-----------|-------------|
| Level1 | 1-level compress. |
| Level2 | 2-level compress, secondary compress based on the 1-level compress. |

| RLE Run Length | Description |
|----------------|-------------|
| Byte_1 | 1 byte, Maximum 255. |
| Byte_2 | 2 bytes, Maximum 255. |

```{note}
RLE Run Length: Maximum length of duplicate characters allowed per stroke (Run) during compression.
```

2.	FastLz

    A dictionary-and-sliding-window based lossless compression algorithm for compressing data with a large number of repetitive values.

3.	YUV_Sample_Blur

    A lossy compression algorithm combining YUV sampling and blurring.

    YUV Sample: Keep the luminance information of the image and only sample the chrominance information.

    Blur: Discard the lower bit of each byte after YUV sampling to achieve the purpose of data compression.

| YUV Sample Mode | Description |
|-----------------|-------------|
| YUV444 | 4 pixel data are sampled to 4 Y, 4 U, and 4 V, i.e., each Y corresponds to a set of UV components, fully preserving the YUV data. |
| YUV422 | Every 4 pixel data are sampled to 4 Y, 2 U, and 2 V, i.e., every 2 Y corresponds to a set of UV components, data size is 75% of the original. |
| YUV411 | Every 4 pixel data are sampled to 4 Y, 1 U, and 1 V, i.e., every 4 Y corresponds to a set of UV components, data size is 50% of the original. |
| YUV422 | Y - luminance; V - chrominance. |

| Blur Mode | Description |
|-----------|-------------|
| Bit0 | Saving without discarding the lower bit. |
| Bit1 | Each byte discards bit0 (preserve [bit7:bit1]). |
| Bit2 | Each byte discards [bit1:bit0] (preserve [bit7:bit2]). |
| Bit4 | Each byte discards [bit3:bit0] (preserve [bit7:bit4]). |

4.	YUV_Sample_Blur+FastLz

    The algorithm combines YUV_Sample_Blur and FastLz.

{#font_setting_anchor}
#### Font Convert Setting
Include Bitmap Fonts and Vector Fonts. Fonts supported by Realtek series ICs are shown in the table below.
```{note}
A Font Convert Setting should be created, otherwise selecting a font for the text-type widget in the Property View is not possible.
```

| Font   | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|--------|:-----------:|:-----------:|:---:|
| Vector | N           | N           | Y   |
| Bitmap | Y           | Y           | Y   |

To use Bitmap Fonts, set the following parameters.

<div id="figure_4_7" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630232320631022/466a71af_12407535.png" width="500"></div>

The following table lists the description of each parameter.

| Parameters        | Description |
|-------------------|-------------------------------------------------------------------|
| Font Setting Name | User-defined font setting name.<br/>Please make sure that you create a different font setting name each time. |
| Font Size         | Font size. |
| Bold              | Bold or not. |
| Italic            | Italic or not. |
| Render Mode       | Bit number used to represent a pixel in the converted ```.bin``` file. |
| Scan Mode         | There are two ways to scan when saving ```.bin```.<br/>H: Horizontal scanning<br/>V: Vertical scanning |
| Index Method      | Index method of the converted .bin's re-indexing area. |
| Code Page         | Support multiple code pages. |
| Text Type         | The types are as follows.<br/>Range: If the text's Unicode range can be pre-determined, please select this type and enter the range in the Range TextBox. Multiple ranges are supported, please set each range in a separate line.<br/>Note: Only the characters within the ranges will be converted and saved to ```.bin``` file, which can save storage space.<br/>Random: If the text's Unicode range cannot be pre-determined, please select this type.<br/>Note: All characters of the Font will be converted and saved to ```.bin``` file. |

Vector Font parameters are shown in the figure below.

<div id="figure_4_8" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630326763929958/7a1b4010_12407535.png" width="500"></div>

### Export
If you have finished designing the GUI project and want to program it to the IC, please click 'Export', then the Tool performs the following actions:
1.	Image convert
2.	Font convert
3.	Pack the ```.xml```, ```.js```, images and fonts into the output ```.bin```.

When the above actions are done, a message box pops up.

<div id="figure_4_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630355854380192/60bbc0af_12407535.png" width="500"></div>

The ```.bin ```can be programmed into your IC.

### Simulate
Simulate the project on UI.

```{note}
When simulating the project for the first time, please click 'Export' before clicking 'Simulate'. Then, there is no need to click 'Export' again if no image or font setting is modified.
```

<div id="figure_4_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828505193590425/a8c6e2c0_12407535.png" width="400"></div>

<div STYLE="page-break-after: always;"></div> 

<a id="quick_start_anchor"></a>

## Quick Start to Tutorials

<a id="how_to_create_anchor"></a>

### How to Create Project


<div id="figure_5_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630502223709854/341d9828_12407535.png" width="750"></div>
<br/>

Double click and run ```RVisualDesigner.exe```, and then configure the project step by step (1~4) and click 'Create Project' (5).
After creation, the GUI design window pops up. The left side is the component area, the center is the design area, and the right side is the widget property setting area.

<div id="figure_5_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726818698027723674/8962d02e_12407535.png" width="750"></div>
<br/>

```{note}
The newly created project file is located in the project folder under the Solution Folder. There is an example as shown in the figure below.
```

<div id="figure_5_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1725505762180091944/39be3e5a_12407535.png" width="400"></div>

After you drag and drop a widget on Design View, and click 'File - Save' or press ```Ctrl + S```, the ```.rtkui``` file will be created.

<div id="figure_5_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1725505619255178068/e89d3b76_12407535.png" width="400"></div>

### How to Write Javascript Code
After the project is created, the ```xxx.js``` file is created. The ```xxx.js``` file is empty, please code here to implement the widgets' event callback.

<a id="how_to_open_anchor"></a>

### How to Open Project

<div id="figure_5_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636293745281159/b5fbf3b0_12407535.png" width="750"></div>

There are two ways to open a project.
1.	Click 'Open Project' and select a ```.rtkprj``` file.

<div id="figure_5_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1724911708175163337/8513c36b_12407535.png" width="750"></div>

2.	Select a ```.rtkprj``` in the Recent Project area.
If the project is listed in the Recent Project area, a message window pops up.

<div id="figure_5_7" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636512948372776/7f7820d5_12407535.png" width="400"></div>


### How to Open/Close Project
Click 'File -> Start Page' on Menu Bar.


### How to Export/Pack Project
<div id="figure_5_8" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636632023023311/9f459245_12407535.png" width="750"></div>
<br/>

Click 'Export' on Menu Bar. The output is shown in the figure below.
<div id="figure_5_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636703955653703/deacfb05_12407535.png" width="400"></div>


### How to Simulate
<div id="figure_5_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636770252167619/8cfcafa3_12407535.png" width="750"></div>

Click on the 'Simulate' button in the menu bar.

<div STYLE="page-break-after: always;"></div> 

<a id="gui_demo_anchor"></a>

## GUI Demo Project

There is a Demo in ```RVisualDesigner-vx.x.x.x.zip```. 

The folder - 454x454 contains a project with resolution 454*454. 

The folder - 480x480 contains a project with resolution 480*480.

<div id="figure_6_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636869409777562/bc385a52_12407535.png" width="580"></div>

Please follow the steps to demo the project.
1.	Open the project according to the screen size/resolution of your IC.
2.	Check the IC type by clicking 'Edit - Convert Project' on the Menu Bar. Please refer to 'Convert Project' for details. 
If the current IC type of the project does not match your IC, please select the target IC type, enter the target resolution, and click 'Convert'.

<div id="figure_6_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636942634993243/11b1e212_12407535.png" width="400"></div>

3.	Click 'Export' on the Menu Bar and wait until the export ok/fail message box pops up.

<div id="figure_6_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721637000052375270/5e3f1873_12407535.png" width="400"></div>

Program the output ```.bin``` into your IC.