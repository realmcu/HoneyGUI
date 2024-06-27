## Overview
RTKIOT Visual Designer is a tool to make graphical interface design for Realtek series ICs, its currently supported ICs are shown in [Table 1-1](#table_1_1). 

RTKIOT Visual Designer supports:
+ Drag and widgets from the toolbox and drop them in the Design View.
+ Drag and drop the widget to change its position in the Design View, or modify the position and appearance of the widget via the Property View.
+ Export the user designed GUI project to .bin, and the .bin can be programmed into the IC to display graphical interface.
+ Simulate the GUI project on PC.

<div id = "table_1_1" style="text-align:center;"><b>Table 1-1 Supported ICs</b></div>

<!-- <div class="center">

|No.          |Supported ICs          |
|:-----------:|:---------------------:|
|1            |RTL8762D               |
|2            |RTL8762G               |
|3            |RTL8763E               |
|4            |TBD	                  |

</div> -->

<table align="center">
	<tr>
		<th width="100">No.</th>
		<th width="200">Supported ICs</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px; text-align: center;">
		<td>1</td>
		<td>RTL8762D</td>
	</tr>
    <tr style="font-size: 14px; text-align: center;">
		<td>2</td>
		<td>RTL8762G</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px; text-align: center;">
		<td>3</td>
		<td>RTL8763E</td>
	</tr>
    <tr style="font-size: 14px; text-align: center;">
		<td>4</td>
		<td>RTL8772G</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px; text-align: center;">
		<td>5</td>
		<td>TBD</td>
	</tr>
</table>

This document mainly consists of:
+ Indroduction to Function Panels
+ Indroduction to Resource Management
+ Indroduction to the Menu Bar
+ Quick start to tutorials
+ GUI demo project

To simplify the document, "Tool" is used below to refer to "RTKIOT Visual Designer".


<div STYLE="page-break-after: always;"></div> 


## Function Panels
### Toolkit/Widgets
+ Non-containerized widget
    + Can be used as the parent of other widget.
    + There is coordinate following relationship between parent and child widgets.
    + Visible when child widget is out of range of parent widget.
+ Container widget
    + Can be used as the parent of other widget.
    + There is coordinate following relationship between parent and child widgets.
    + Visible when child widget is out of range of parent widget.
    + Can drag and drop a widget from the toolbox into the container widget.

This section lists the properties supported by the widget in tables and mark with "Y" or "N" to indicate whether the IC supports them or not. 
</br>

#### Non-containerized Widget
##### Text
Used only for text display and does not support user input. The properties are shown in [Table 2-1](#table_2_1).

<div id = "table_2_1" style="text-align:center;"><b>Table 2-1 Text widget properties</b></div>

<table align="center">
    <tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to the parent widget.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to the parent control.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>Text</I></b></td>
		<td>Displayed text.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Display Mode</I></b></td>
		<td>Long text (text content beyond the widget's range) </br>
        display mode with the following supported types.</br>
        truncate: Truncated display mode</br>
        verticalscroll: Vertical scrolling display mode</br>
        horizontalscroll: Horizontal scrolling display mode.
        </td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
        <td><b><I>Font</I></b></td>
		<td>Font setting, please refer to
        <a href="#font_setting_anchor">Font Setting</a>.
        </td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Font Color (RGBA)</I></b></td>
		<td>Font color setting, use RGBA./td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>
</br>

##### Button
Clickable widget with text and background image. The properties are shown in [Table 2-2](#table_2_2).

<div id = "table_2_2" style="text-align:center;"><b>Table 2-2 Button properties</b></div>

<table align="center">
    <tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to the parent widget.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to the parent control.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>Text</I></b></td>
		<td>Displayed text</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Text X</I></b></td>
		<td>Horizontal coordinate relative to the Button widget.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
        <td><b><I>Text Y</I></b></td>
		<td>Vertical coordinate relative to the Button widget.</a>
        </td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Display Mode</I></b></td>
		<td>Horizontal or Vertical display.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
        <td><b><I>Font</I></b></td>
		<td>Font setting, please refer to
        <a href="#font_setting_anchor">Font Setting</a>.
        </td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Text Color (RGB)</I></b></td>
		<td>Text color setting, use RGB.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
        <td><b><I>Transition</I></b></td>
		<td>Image transition mode with the following options.
        normal: No effect</br>
        fade: Fade-in/out</br>
        scale: Scalling</br>
        fadeScale: Fade-in/out and scaling</br>
        Note: Set the transition mode is effective only if set
        the default and highlight background image, otherwise all normal.
        </td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
        <td><b><I>BG Image (Default)</b></I></td>
        <td>Default background image.</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
        <td><b><I>BG Image</br>
        (Highlight)</b></I></td>
        <td>Selected/Highlight background image.</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
        <td><b><I>BG Image</br>
        Rotation Angle</b></I></td>
        <td>Background image rotation angle, range: 0~360 degree.</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
    </tr>
</table>
</br>

##### Image
Widget that can set image. The properties are shown in [Table 2-3](#table_2_3).

<div id = "table_2_3" style="text-align:center;"><b>Table 2-3 Image properties</b></div>

<table align="center">
    <tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to the parent widget.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to the parent control.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>Image</b></I></td>
	    <td>Image Path</br>
        Note: The image must be pre-imported into the project. Please refer to
        <a href="#img_res_m_anchor">Image Resource Management</a> for details.
        </td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b><I>Image Rotation Angle</b></I></td>
	    <td>Image rotation angle.</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>Image Scale X</b></I></td>
	    <td>Image horizontal scaling degree, is a multiplier/percentage.</br>
        For example, set scale x 0.5 means that the actual display width of the image is half of the original image width.
        </td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b><I>Image Scale Y</b></I></td>
	    <td>Image vertical scaling degree, is a multiplier/percentage.</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
</table>

**Note:**
1.	When exporting, tool will convert the imported images. And the image conversion parameters can be set in "Menu Bar -> Setting -> Image Convert Setting", please refer to <a href="#img_conv_setting_anchor">Image Convert Setting</a> for details.
2.	If the size of the imported image does not match the size of the widget, tool doesn't scale or crop the image.

</br>

##### SeekBar
Sliding widget that can respond to user swipe gesture with the widget and change the progress value. The properties are shown in [Table 2-4](#table_2_4).

<div id = "table_2_4" style="text-align:center;"><b>Table 2-4 Text widget properties</b></div>

<table align="center">
	<tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to the parent widget.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to the parent control.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
	    <td><b><I>Color(Highlight) (RGBA)</b></I></td>
	    <td>Background color of partially completed part of progress bar, please refer to
        <a href="#figure_2_1">Figure 2-1</a>.
        </td>
	    <td style="text-align: center">N</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">N</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>Color (RGBA)</b></I></td>
	    <td>Background color of the whole progress bar, please refer to
        <a href="#figure_2_1">Figure 2-1</a>.
        </td>
	    <td style="text-align: center">N</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">N</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b><I>Orientation</b></I></td>
	    <td>Widget display orientation and gesture response orientation with the following types:</br>
        vertical/V: Vertical orientation</br>
        arc: Direction of a curve</br>
        horizontal/H: Horizontal orientation</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
</table>

<div id="figure_2_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817870508689816/f43215ff_12407535.png" style="width: 400px"></div>
<div style="text-align:center;"><b>Figure 2-1 Seekbar</b></div>
</br>

##### Image SeekBar
Sliding widget with multi-images as background, and switch to different images as user swipes. The properties are shown in Table 2-5.

<div id = "table_2_5" style="text-align:center;"><b>Table 2-5 Image Seekbar properties</b></div>

<table align="center">
	<tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to the parent widget.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to the parent control.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
	    <td><b><I>Degree(Start)</b></I></td>
	    <td>Start degree (Invalid if orientation is arc).</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>Degree(End)</b></I></td>
	    <td>End degree (Invalid if orientation is arc).</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b><I>Image Directory</b></I></td>
	    <td>Folder that contains only the images to be displayed on this widget.</br>
        Notes: </br>
        1.	Please sort the images by name;</br>
        2.	When the user swipes on the widget, the widget will switch the background image according to the current progress.
        </td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>Central X</b></I></td>
	    <td>Horizontal coordinate of the center of the arc relative to the parent widget.</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b>Central Y<I></b></I></td>
	    <td>Vertical coordinate of the center of the arc relative to the parent widget.</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>Orientation</b></I></td>
	    <td>Widget display orientation and gesture response orientation with the following types:</br>
        vertical/V: Vertical orientation</br>
        arc: Direction of a curve</br>
        horizontal/H: Horizontal orientation
        </td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
</table>
</br>

##### Switch
Switch widget with "Checked" and "Unchecked" states. The properties are shown in [Table 2-6](#table_2_6).

<div id = "table_2_6" style="text-align:center;"><b>Table 2-6 Switch properties</b></div>

<table align="center">
	<tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to the parent widget.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to the parent control.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
	    <td><b><I>BG Image(Checked)</b></I></td>
	    <td>Checked state background image.</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>BG Image(Default)</b></I></td>
	    <td>Unchecked state background image.</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
</table>
</br>

##### Arc
Arc widget, no gesture support yet. The properties are shown in [Table 2-7](#table_2_7).

<div id = "table_2_7" style="text-align:center;"><b>Table 2-7 Arc properties</b></div>

<table align="center">
	<tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
		<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to the parent widget.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to the parent control.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Central X</I></b></td>
		<td>Horizontal coordinate of the center of the arc relative to the parent widget.</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Central Y</I></b></td>
		<td>Vertical coordinate of the center of the arc relative to the parent widget.</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
        <td><b><I>BG Color</I></b></td>
		<td>Arc background color</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
    </tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Cap Mode</I></b></td>
		<td>Arc cap mode, the following options are supported:</br>
        round/butt/square, please refer to
        <a href="#figure_2_2">Figure 2-2</a>
        </td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Degree(End)</b></I></td>
		<td>End degree of arc.</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
    </tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Degree(Start)</b></I></td>
		<td>Start degree of arc.</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Radius</b></I></td>
		<td>Radius of arc.</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
    </tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Stroke Width</b></I></td>
		<td>Width of arc stroke.</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
</table>

<div id = "figure_2_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817869829442326/90524d4c_12407535.png"/></div>
<div style="text-align:center;"><b>Figure 2-2 Cap mode</b></div>
</br>


#### Container widget
##### Screen
Screen widget, corresponds to the physical screen, is the root widget of a GUI project. The properties are shown in [Table 2-8](#table_2_8).

**Note:**
Only "Name" property can be modified.

<div id = "table_2_8" style="text-align:center;"><b>Table 2-8 Screen properties</b></div>

<table align="center">
	<tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate, always 0.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate, always 0.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>
</br>

##### TabView and Tab
With Tab widget as child widget, it supports up/down/left/right swiping to switch among Tabs. 
The properties of TabView are shown in [Table 2-9](#table_2_9), and the properties of Tab are shown in [Table 2-10](#table_2_10).

<div id = "table_2_9" style="text-align:center;"><b>Table 2-9 TabView properties</b></div>

<table align="center">
	<tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to the parent widget, always 0.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to the parent widget, always 0.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Transition</b></I></td>
		<td>Tab transition mode with the following supported types:</br>
        normal: No effect</br>
        fade: Fade-in/out</br>
        scale: Scalling</br>
        fadeScale: Fade-in/out and scaling
		</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>

<div id = "table_2_10" style="text-align:center;"><b>Table 2-10 Tab properties</b></div>

<table align="center">
	<tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to TabView widget, always 0.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to TabView widget, always 0.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Index(X-Axis)</b></I></td>
		<td>Horizontal index of Tabs in TabView, please refer to
		<a href="#figure_2_3">Figure 2-3</a>.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Index(Y-Axis)</b></I></td>
		<td>Vertical index of Tabs in TabView, please refer to
		<a href="#figure_2_3">Figure 2-3</a>.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>
	
<div id = "figure_2_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817871000642675/ca6cda22_12407535.png" width="300"/></div>
	
<center><div><b>Figure 2-3 Image Seekbar properties</b></div></center>

**Notes:**
1.	TabView width and height could not be modified, defaulting to Screen's width and height. 
2.	TabView horizontal and vertical coordinate could not be modified, is always 0.
3.	TabView can only be used as a child of Screen widget.
4.	TabView's child widgets can only be Tabs.
5.	Tab's width and height could not be modified, defaulting to TabView's width and height.
6.	Tab's horizontal and vertical coordinate could not be modified and is always 0.

</br>

##### Page
Container widget with scrollable content.

**Notes:**
1.	Page only supports vertical scrolling.
2.	The width and height of the Page widget only defines the area of the interface that can response to a swipe gesture. Whether scrolling is allowed depends on whether or not the child widget added to it are outside the scope of the screen.

<div id = "table_2_11" style="text-align:center;"><b>Table 2-11 Page  properties</b></div>

<table align="center">
	<tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to the parent widget.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to the parent control.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>
</br>


##### Win
Within the area defined by Win width and height, it can response to variable gestures, including click, long click, press, press release and swipe. The properties are shown in [Table 2-12](#table_2_12).

<div id = "table_2_12" style="text-align:center;"><b>Table 2-12 Win  properties</b></div>

<table align="center">
	<tr>
		<th width="130">Property</th>
		<th width="350" style="text-align: center;">Description</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Name</I></b></td>
		<td>Widget name.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Size (Height)</I></b></td>
		<td>Widget height.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Size (Width)</I></b></td>
		<td>Widget width.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>Horizontal coordinate relative to the parent widget.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>Vertical coordinate relative to the parent control.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Hidden</b></I></td>
		<td>Indicate whether Win and its child widget need to be hidden.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>
</br>


### Design View/Canvas
Users can drag and drop widgets from the Toolbox panel into the Design View, adjust the widgets layout and set properterties to design a graphical interface that can be rendered in the Realtek ICs.

<img src="https://foruda.gitee.com/images/1710817870929335945/6991cdc9_12407535.png">
<div id = "figure_2_4" style="text-align:center;"><b>Figure 2-4 Design View</b></div>
</br>

#### TabView - Create/Delete/Insert Tab
Drag and drop TabView widget from the Toolbox into the Design View, then a TabView that contains only a home tab (coordinates (0,0)) is created, as shown in [Figure 2-5](#figure_2_5).


<img src="https://foruda.gitee.com/images/1710817870419593506/9ab927b9_12407535.png">
<div id = "figure_2_5" style="text-align:center;"><b>Figure 2-5 Create TabView</b></div>
</br>


##### Create Tab
New Tabs can be created by clicking the buttons around the Design View.

**Notes:**
1.	If idx is 0, up and down button is enabled.
2.	If idy is 0, left and right button is enabled.


###### Delete Tab
Select the Tab to be deleted, click "Edit -> Delete" on the menu bar or press the Delete key on the keyboard. Then double-check if you want to delete it.

<div id = "figure_2_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817870328087578/e0915700_12407535.png"></div>
<div style="text-align:center;"><b>Figure 2-6 Delete tab double-check</b></div>
</br>

##### Insert Tab
Currently, Tab insertion is only supported by modifying the coordinates of an existing Tab and creating a new one.

For example, Tab need to be inserted between Tabs with coordinates (1, 0) and (2, 0), the steps are as follow.
1.	Increase the idx of Tab (2,0) and the Tabs to its right by 1, as shown in [Figure 2-7 (b)](#figure_2_7_b).
2.	Switch to Tab (1,0) and click  to create new Tab (2, 0).

<div id="figure_2_7_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817875276200848/d238bf5a_12407535.png"></div>
<div style="text-align:center;">a. Tab insertion position</div>
<div id="figure_2_7_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817875726752431/e704d18a_12407535.png"></div>
<div style="text-align:center;">b. Modify existing Tab coordinates</div>
<div id="figure_2_7_c"><img src="https://foruda.gitee.com/images/1710818987429423477/280e6384_12407535.png"></div>
<div style="text-align:center;">c. Create Tab</div>
<div style="text-align:center;"><b>Figure 2-7 Insert Tab</b></div>
</br>


#### Tabview Overview Chart
Please click ![icon](https://foruda.gitee.com/images/1710818993805990437/796af0dc_12407535.png)  to show TabView Overview Chart.

**Notes:**
1.	The highlight Tab in the Overview Chart indicates the Tab that is currently being edited in Design View.
2.	The Overview Chart label each Tab with its coordinates. When simulate or rendered in ICs, the Tab with coordinates (0,0) is displayed on the Home page, and users can swipe up/down/left/right to display other Tabs.

<div id="figure_2_8_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818988334796749/309f47b2_12407535.png"></div>
<div id="figure_2_8_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818988855366691/0a431fa4_12407535.png"></div>
<div style="text-align:center;"><b>Figure 2-8 TabView Overview Chart</b></div>
</br>


#### Zoom of Design View
There are 3 ways to zoom the Design View.
1.	Press Ctrl key and wheel mouse.
2.	Click "-" and "+" button.
3.	Drag the slider bar.

<div id="figure_2_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818993215663271/c0d56791_12407535.png"></div>
<div style="text-align:center;"><b>Figure 2-9 Zoom of Design View</b></div>
</br>


### Property View
Selecting a widget in the Widget Tree or Design View exposes all of the widget's property values, which the users can modify as needed, as shown in [Figure 2-10](#figure_2_10).

<div id="figure_2_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818988161964594/40fa2436_12407535.png"></div>
<div style="text-align:center;"><b>Figure 2-10 Property View</b></div>
</br>


### Widget Tree
The Widget Tree is used to present to the users the parent/child/sibling relationship of the currently laid out widgets. And we have the following convention here.
1.	The child widget layer is on top of the parent widget layer, i.e., when the parent and child widget overlap, the child widget will cover the parent widget.
2.	The layer of sibling widgets is related to the order in which the widgets are added, with widgets add first at bottom and widgets added later at the top.

[Figure 2-11](#figure_2_11) shows all the child widgets of the Home tab and Lamp tab, where the Home tab has only one Image child widget for setting the background, and the Lamp tab contains an Image widget and a number of Switch widgets.

<div id="figure_2_11" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818987846178523/1b27419f_12407535.png"></div>
<div style="text-align:center;">a. Home Tab</div>
<div id="figure_2_11_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818987413289439/46a6c801_12407535.png"></div>
<div style="text-align:center;">b. Lamp Tab</div>
<div style="text-align:center;"><b>Figure 2-11 Property View</b></div>
</br>

Widget Tree supports the following operations.
1.	Select widget
If you select a widget on the Widget Tree, the corresponding widget in the Design View focuses and its properties are shown on Property View.
2.	Modify the parent-child relationship 
Select a widget on the Widget Tree (except Tab/TabView/Screen) and drag-and-drop it on the target widget item. Then the widget will be a child widget of the target widget.
3.	Modify widget layers
Select a widget on the Widget Tree (except Tab/TabView/Screen) and drag-and-drop it to the upper or lower edge of the target widget item. Then on the Design View, the widget will be placed over or under the target widget.
4.	Lock widgets
Click the button and lock the widget/widgets, as shown in [Figure 2-12](#figure_2_12).
    + If click the lock button of screen, all the screen's child widgets will be locked, then user could not drag or resize the widgets on Design View.
    + If clik the lock button of Tab, all the tab's child widgets will be locked, then user could not drag or resize the widgets on Design View.

<div id="figure_2_12" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824254850042256/002159e9_12407535.png"></div>
<div style="text-align:center;">a. Un-Locked</div>
<div id="figure_2_12_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824254887179488/09fabec5_12407535.png"></div>
<div style="text-align:center;">b. Locked</div>
<div style="text-align:center;"><b>Figure 2-12 Lock widget/widgets</b></div>
</br>


<div STYLE="page-break-after: always;"></div> 


## Resource Mangement
Only pre-imported image and font files can be referenced by the GUI project. This chapter focus on how to manage image and font resource.
The image and font explorer is located directly below the design view, as shown in [Figure 3-1](#figure_3_1).

<div id="figure_3_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824260408871745/edfdf9bd_12407535.png"></div>
<div style="text-align:center;"><b>Figure 3-1 Image and font resource management</b></div>
</br>


### <div id="img_res_m_anchor">Image Resource Management</div>
Click ![icon](https://foruda.gitee.com/images/1710824254768634573/d752fbd6_12407535.png)  to bring up the Image Management view as shown in [Figure 3-2](#figure_3_2).

<div id="figure_3_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824254901030282/9705cea5_12407535.png" width="300"></div>
<div style="text-align:center;"><b>Figure 3-2 Image resource management window</b></div>
</br>


##### Add Images
Images can be added to the GUI project by following the process below.
1.	Click   to create a new image folder and enter the folder name.
The created folder is located in the Resouce\image folder under the GUI project directory.

<div id="figure_3_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824260010283421/eb1636f0_12407535.png" width="300"></div>
<div style="text-align:center;"><b>Figure 3-3 Create image folder</b></div>

2.	Select the created image folder and click ![icon](https://foruda.gitee.com/images/1710824254769021374/55b6c303_12407535.png) to select images (multiple selection is possible) to add them to the folder.
As shown in [Figure 3-4](#figure_3_4), the images are copied to the Resource\image\home folder after the addition is completed.

<div id="figure_3_4_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824488209842923/05efbb41_12407535.png" width="300"></div>
<div style="text-align:center;">a. Select image folder</div>
<div id="figure_3_4_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824483196685183/7e19707a_12407535.png" width="500"></div>
<div style="text-align:center;">b. Select the images to be added</div>
<div id="figure_3_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824488369529087/4940a684_12407535.png" width="300"></div>
<div style="text-align:center;">c. Add images</div>
<div style="text-align:center;"><b>Figure 3-4 Add Image(s)</b></div>
</br>


##### Remove Images/Image Folder
Select the image or image folder to be removed, and click ![icon](https://foruda.gitee.com/images/1710824483012674071/c0c62b91_12407535.png).
</br>


##### Rename Image Folder
Select the image folder, double click and enter a new name.
</br>


#### Preview Images
Select the image folder and all images in this folder will be displayed in the right area.

<div id="figure_3_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824483098291044/697fd630_12407535.png"></div>
<div style="text-align:center;"><b>Figure 3-5 Preview images</b></div>
</br>


#### Refresh
If the user locally operates the image resources, not via Tool, you can click   to refresh.

**Note:**
Not recommended.
</br>


### Font Resource Management
Please refer to [Figure 3-6](#figure_3_6).
</br>


#### Add Third-Party Font
If need use a third-party font (.ttf), please click ![icon](https://foruda.gitee.com/images/1710824482944711702/8b73005d_12407535.png) to import the resource first, otherwise the locally installed font will be used.

<div id="figure_3_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824483191984010/283b1f70_12407535.png" width="500"></div>
<div style="text-align:center;"><b>Figure 3-6 Fonts management</b></div>
</br>


#### Remove Third-Party Font
Select the font to be removed and click ![icon](https://foruda.gitee.com/images/1710824483012674071/c0c62b91_12407535.png).
</br>


<div STYLE="page-break-after: always;"></div> 


## Menu Bar
### File
#### Start Page
If you want to close the current project and open an existed project or create a new project, you can open the Start Page by click "File -> Start Page".
Click "Open Project" or select a .rtkprj and double click to open the existed project, or click "Create Project" to create a new project, please refer to [How to Create Project](#how_to_create_anchor) and [How to Open Project](#how_to_open_anchor).


<div id="figure_4_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824561456305634/8d4384a4_12407535.png"></div>
<div style="text-align:center;"><b>Figure 4-1 File - Start Page</b></div>
</br>


#### Save
Save all the UI changes of the project, the shortcut is Ctrl + S.
</br>


##### Exit Save
Prompt window will pop-up when you close the project, as shown below. Please click "OK" to save, or the changes will be abandoned.

<div id="figure_4_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555938151074/9cb2be23_12407535.png" width="260"></div>
<div style="text-align:center;"><b>Figure 4-2 Close and save the project</b></div>
</br>


### Edit
#### Copy/Paste
1.	Click "Edit -> Copy" to copy the selected widget, the shortcut is Ctrl + C.
2.	Click "Edit -> Paste" to create a copy of the selected widget on the Design View, the shortcut is Ctrl + V.
</br>


#### Delete
Click "Edit -> Delete" to delete the selected widget, or press "Delete" key on the keyboard.
</br>


#### Undo/Redo
Undo: Undo the change made to the widget, the shortcut is Ctrl + Z.
Redo: Do the change to the widget again, the shortcut is Ctrl + Y.
</br>


#### Convert Project
The Convert Project window is used to convert the IC type and screen size/resolution for current project.

<div id="figure_4_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555929917469/fcbb0ff3_12407535.png" width="300"></div>
<div style="text-align:center;"><b>Figure 4-3 Convert Project window</b></div>
</br>


#### Project Name Modification
The Project Name window is used to modify current project name. Please enter the new name in the input box.
 
<div id="figure_4_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555934977455/0a13f4fd_12407535.png" width="300"></div>
<div style="text-align:center;"><b>Figure 4-4 Project Name window</b></div>
</br>


### Setting

#### <div id="img_conv_setting_anchor">Image Convert Setting</div>
The images must be converted to be displayed correctly on the IC, so users need to set the correct convert parameters. All the optional parameters are shown in [Figure 4-5](#figure_4_5).

<div id="figure_4_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555965501168/ec76c1b1_12407535.png" width="260"></div>
<div style="text-align:center;"><b>Figure 4-5 Image convert window</b></div>

The parameters are described as follows.
</br>


##### Scan Mode
The available options are shown in [Table 4-1](#table_4_1).

<div id="table_4_1" style="text-align:center;"><b>Table 4-1 Scan mode options</b></div>
<table align="center">
	<tr>
		<th width="130">Scan Mode</th>
		<th width="600" style="text-align: center;">Description</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Horizental</I></b></td>
		<td>Horizontal scan.</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Vertical</I></b></td>
		<td>Vertical scan.</td>
	</tr>
</table>
</br>

##### Color Space
Color space of Image, the available options are shown in [Table 4-2](#table_4_2).

<div id="table_4_2" style="text-align:center;"><b>Table 4-2 Color space options</b></div>
<table align="center">
	<tr>
		<th width="130">Color Space</th>
		<th width="600" style="text-align: center;">Description</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>RGB565</I></b></td>
		<td>16 bit RGB mode</br>
        Bit 4:0 represent blue; Bit 10:5 represent green; Bit 15:11 represent red.
        </td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>RTKARGB</I></b></td>
		<td>16 bit ARGB mode</br>
        Bit 4:0 represent blue; Bit 9:5 represent green; Bit 14:10 represent red; Bit 15 represent alpha.
		</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>RTKRGAB</I></b></td>
		<td>16 bit RGAB mode</br>
        Bit 4:0 represent blue; Bit 5 represent alpha; Bit 10:6 represent green; Bit 15:11 represent red.
		</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>RGB</I></b></td>
		<td>24 bit RGB mode</br>
        Bit 7:0 represent blue; Bit 15:8 represent green; Bit 23:16 represent red.
		</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>RGBA</I></b></td>
		<td>32 bit RGBA mode</br>
        Bit 7:0 represent blue; Bit 15:8 represent green; Bit 23:16 represent red; Bit 31:24 represent alpha.
        </td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>BINARY</I></b></td>
		<td>2-value (0 or 1) image.</td>
	</tr>
</table>
</br>


##### Compress
If check "Compress", please set the compression parameter as needed.
The optional Compress Mode is as follows:
1.	RLE: Run-Length Encoding, a lossless compression algorithm.
If you select RLE as the Compress Mode, RLE Level and RLE Run Length parameter are mandatory to set.

<div id="figure_4_6_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555889664193/c33b99ad_12407535.png" width="260"></div>
<div style="text-align:center;">a. RLE Level - Level1</div>
<div id="figure_4_6_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824561230699040/f2dbfb4a_12407535.png" width="260"></div>
<div style="text-align:center;">b.	RLE Level - Level2</div>
<div style="text-align:center;"><b>Figure 4-6 RLE parameters</b></div>
</br>

<div id="table_4_3" style="text-align:center;"><b>Table 4-3 RLE Level</b></div>
<table class="center">
	<tr>
		<th width="150">RLE Level</th>
		<th width="600" style="text-align: center;">Description</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Level1</I></b></td>
		<td>1-level compress.</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Level2</I></b></td>
		<td>2-level compress, secondary compress based on the 1-level compress.</td>
	</tr>
</table>

<div id="table_4_4" style="text-align:center;"><b>Table 4-4 RLE Run Length</b></div>
<table align="center">
	<tr>
		<th width="150">RLE Run Length</th>
		<th width="580" style="text-align: center;">Description</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Byte_1</I></b></td>
		<td>1byte, Maximum 255.</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Byte_2</I></b></td>
		<td>2byte, Maximum 255.</td>
	</tr>
</table>

Maximum length of duplicate characters allowed per stroke (Run) during compression.

2.	FastLz: 
A dictionary-and-sliding-window based lossless compression algorithm for compressing data with a large number of repetitive values.
3.	YUV_Sample_Blur: 
A lossy compression algorithm combining YUV sampling and blurring.
YUV Sample: Keep the luminance information of the image and only sample the chrominance information.
Blur: Discard the lower bit of each byte after YUV sampling to achieve the purpose of data compression.


<div id="table_4_5" style="text-align:center;"><b>Table 4-5 YUV Sample Mode</b></div>
<table align="center">
	<tr>
		<th width="160">YUV Sample Mode</th>
		<th width="570" style="text-align: center;">Description</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>YUV444</I></b></td>
		<td>4 pixel data are sampled to 4 Y, 4 U and 4, i.e., each Y corresponds to a set of UV components, fully preserving the YUV data.</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>YUV422</I></b></td>
		<td>Every 4 pixel data are sampled to 4 Y, 2 U and 2 V, i.e., every 2 Y corresponds to a set of UV components, data size is 75% of the original.</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>YUV411</I></b></td>
		<td>Every 4 pixel data are sampled to 4 Y, 1 U and 1 U, i.e., every 4 Y corresponds to a set of UV components, data size if 50% of the original.</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>YUV422</I></b></td>
		<td>Y - luminance; V - chrominance.</td>
	</tr>
</table>

<div id="table_4_6" style="text-align:center;"><b>Table 4-6 Blur Mode</b></div>
<table align="center">
	<tr>
		<th width="150">YUV Sample Mode</th>
		<th width="580" style="text-align: center;">Description</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Bit0</I></b></td>
		<td>Saving without discarding lower bit.</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Bit1</I></b></td>
		<td>Each byte discards bit0 (preserve [bit7:bit1]).</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Bit2</I></b></td>
		<td>Each byte discards [bit1:bit0] (preserve [bit7:bit2]).</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Bit4</I></b></td>
		<td>Each byte discards [bit3:bit0] (preserve [bit7:bit4]).</td>
	</tr>
</table>

4.	YUV_Sample_Blur+FastLz: Algorithm combines YUV_Sample_Blur and FastLz.


#### <div id="font_setting_anchor">Font Convert Setting</div>
Include Bitmap Fonts and Vector Fonts. Fonts supported by Realtek series ICs are shown in [Table 4-7](#table_4_7).

<u><b>Attention: You should create at least one Font Convert Setting, otherwise you could not select font for the text-type widget in the Property View.</b></u>

<div id="table_4_7" style="text-align:center;"><b>Table 4-7 Fonts supported by Realtek</b></div>
<table align="center">
	<tr>
		<th width="130">Font</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Vector</I></b></td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Bitmap</I></b></td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>

To use Bitmap Fonts, you need to set the following parameters.

<div id="figure_4_7" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555984723993/d86cef5d_12407535.png" width="260"></div>
<div style="text-align:center;"><b>Figure 4-7 Convert settings of Bitmap Fonts</b></div>

[Table 4-8](#table_4_8) lists the description of each parameter.

<div id="table_4_8" style="text-align:center;"><b>Table 4-8 Font convert parameters</b></div>
<table align="center">
	<tr>
		<th width="150">Parameters</th>
		<th width="580" style="text-align: center;">Description</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Font Setting Name</I></b></td>
		<td>User-defined font setting name.</br>
        Please make sure that you create a different font setting name each time.
		</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Font Size</I></b></td>
		<td>Font size.</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Bold</I></b></td>
		<td>Bold or not.</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Italic</I></b></td>
		<td>Italic or not.</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Render Mode</I></b></td>
		<td>Bit number used to represent a pixel in the converted .bin file.</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Scan Mode</I></b></td>
		<td>There are two ways to scan when saving .bin.</br>
        H: Horizontal scanning</br>
        V: Vertical scanning
		</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Index Method</I></b></td>
		<td>Index method of the converted .bin's re-indexing area.</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Code Page</I></b></td>
		<td>Support multiple code page.</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Text Type</I></b></td>
		<td>The types are as following.</br>
        Range: If the text's Unicode range can be pre-determined, please select this type and entered the range in the Range TextBox. Multiple ranges are supported, please set each range in a separate line.</br>
        Note: Only the characters within the ranges will be converted and save to .bin file, which can save storage space.</br>
        Random: If the text's Unicode range cannot be pre-determined, please select this type.</br>
        Note: All characters of the Font will be converted and save to .bin file.
		</td>
	</tr>
</table>

Vector Font parameters are shown in [Figure 4-8](#figure_4_8).

<div id="figure_4_8" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633144604007/ae5d772b_12407535.png" width="300"></div>
<div style="text-align:center;"><b>Figure 4-8 Vector font parameters</b></div>


### Export
If you have finished designing the GUI project and want to program it to the IC, please click "Export", then the Tool performs the following actions:
1.	Image convert
2.	Font convert
3.	Pack the .xml, .js, images and fonts into the output .bin.

When the above actions are done, a message box pops up.

<div id="figure_4_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633121026710/337d046a_12407535.png" width="300"></div>
<div style="text-align:center;"><b>Figure 4-9 Vector font parameters</b></div>

The .bin can be programmed into your IC.


### Simulate
Simulate the project on UI.
Attention: When you simulate the project at the first time, please click "Export" before clicking "Simulate". And then, there is no need to click "Export" again if you didn't modify any image or font setting.
[Figure 4-10](#figure_4_10) hows the running simulator.

<div id="figure_4_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828505193590425/a8c6e2c0_12407535.png" width="260"></div>
<div style="text-align:center;"><b>Figure 4-10 Running simulator</b></div>


<div STYLE="page-break-after: always;"></div> 


## Quick Start To Tutorials
### <div id="how_to_create_anchor">How to Create Project</div>
<div id="figure_5_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633374956290/299fb9f5_12407535.png"></div>
<div style="text-align:center;"><b>Figure 5-1 Start Page</b></div>

Double click and run "RVisualDesigner.exe", and then configure the project step by step (1~4) and click "Create Project" (5), as shown in [Figure 5-1](#figure_5_1).
After creation, the GUI design window pops up as shown in [Figure 5-2](#figure_5_2). The left side is the component area, the center is the design area, and the right side is the widget property setting area. 

<div id="figure_5_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633484403086/7932a505_12407535.png"></div>
<div style="text-align:center;"><b>Figure 5-2 GUI design window</b></div>

Note: The newly created project file is located in the project folder under the Solution Folder (3 in [Figure 5-1](#figure_5_1)). There is an example in [Figure 5-3](#figure_5_3).

<div id="figure_5_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633124706596/fe5653b6_12407535.png" width="400"></div>
<div style="text-align:center;"><b>Figure 5-3 Project folder</b></div>

After you drag and drop a widget on Design View, and click "File - Save" or press Ctrl + S, the .rtkui file will be created, as shown in [Figure 5-4](#figure_5_4).

<div id="figure_5_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828526032663800/81e3057a_12407535.png" width="400"></div>
<div style="text-align:center;"><b>Figure 5-4 The .rtkui file</b></div>


### How to Write Javascript Code
After the project is created, the xxx.js file is created, as shown in [Figure 5-4](#figure_5_4). The xxx.js file is empty, please code here to implement the widgets' event callback.


### <div id="how_to_open_anchor">How to Open Project</div>
<div id="figure_5_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633354019948/7c6804dc_12407535.png" width="400"></div>
<div style="text-align:center;"><b>Figure 5-5 Open Project</b></div>

There are two ways to open project.
1.	Click "Open Project" and select a .rtkprj file.

<div id="figure_5_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828552787091298/4e1ccc92_12407535.png" width="400"></div>
<div style="text-align:center;"><b>Figure 5-6 Open project by selecting .rtkprj</b></div>

2.	Select a .rtkprj in Recent Project area.
If the project listed on Recent Project area, a message window pops up.

<div id="figure_5_7" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828557068088923/e418bb99_12407535.png" width="300"></div>
<div style="text-align:center;"><b>Figure 5-7 Message window</b></div>


### How to Open/Close Project
Click "File -> Start Page" on Menu Bar.


### How to Export/Pack Project
<div id="figure_5_8" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828565511074049/27af6150_12407535.png"></div>
<div style="text-align:center;"><b>Figure 5-8 Export</b></div>
<div id="figure_5_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828593609008235/ca49b9cd_12407535.png" width="300"></div>
<div style="text-align:center;"><b>Figure 5-9 Export OK</b></div>

Click "Export" on Menu Bar, the output are shown in [Figure 5-9](#figure_5_9).


### How to Simulate
<div id="figure_5_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828603947574032/f90b802b_12407535.png"></div>
<div style="text-align:center;"><b>Figure 5-10 Simulate</b></div>

Click on the "Simulate" button in the menu bar.


<div STYLE="page-break-after: always;"></div> 


## GUI Demo Project
There is a Demo in RVisualDesigner-vx.x.x.x.zip, as shown in [Figure 6-1](#figure_6_1). 

The folder - 454x454 contains project with resolution 454*454. 

The folder - 480x480 contains project with resolution 480*480.

<div id="figure_6_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828619648299243/6ddfa28a_12407535.png" width="400"></div>
<div style="text-align:center;"><b>Figure 6-1 Demo</b></div>

Please follow the steps to demo the project.
1.	Open the project according to the screen size/resolution of your IC.
2.	Check the IC type by clicking "Edit - Convert Project" on Menu Bar, please refer to "Convert Project" for details. 
If current IC type of the project does not match your IC, please select target IC type, enter the target resolution and click "Convert".

<div id="figure_6_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828634079822366/790c6381_12407535.png" width="300"></div>
<div style="text-align:center;"><b>Figure 6-2 Convert Project</b></div>

3.	Click "Export" on Menu Bar and wait until the export ok/fail message box pops up.

<div id="figure_6_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828658401238872/d2f38d9e_12407535.jpeg" width="300"></div>
<div style="text-align:center;"><b>Figure 6-3 Output .bin</b></div>

Program the output .bin into your IC.