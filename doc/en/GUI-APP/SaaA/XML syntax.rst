==========
XML Syntax
==========

Element
-------

- Element corresponding widget.
- Element's attributes corresponding widget's attributes (0 value can be ignored).
- Text content is the widget instance's name.

.. code-block:: xml

   <type a1="xx" a2="xx" a3="xx" a4="xx">name</type> 

Nesting
-------

The nesting structure of elements is consistent with the nesting structure of actual widgets.

.. raw:: html

   <style>
   .center {
       display: flex;
       justify-content: center;
   }
   </style>

.. raw:: html

   <div class="center">

.. mermaid::

   graph TD;
       fatherName --> childName1;
       fatherName --> childName2;
       childName2 --> childName3;
       childName2 --> childName4;

.. raw:: html

   </div>

.. code-block:: xml

   <fatherType a1="xx" a2="xx" a3="xx" a4="xx">fatherName 

       <childType a1="xx" a2="xx" a3="xx">childName1</childType> 
       <childType a1="xx" a2="xx" a3="xx">childName2
           <childType a1="xx" a2="xx" a3="xx">childName3</childType> 
           <childType a1="xx" a2="xx" a3="xx">childName4</childType> 
       </childType> 
   </fatherType> 

Specifications
==============


.. list-table::
   :header-rows: 1
   :widths: 10 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8

   * - Element
     - Attributes1
     - Attributes2
     - Attributes3
     - Attributes4
     - Attributes5
     - Attributes6
     - Attributes7
     - Attributes8
     - Attributes9
     - Attributes10
     - Attributes11
     - Attributes12
     - Attributes13
     - Attributes14
     - Attributes15
     - Attributes16
     - Attributes17
   * - win
     - x
     - y
     - w
     - h
     - hidden
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - textbox
     - x
     - y
     - w
     - h
     - text
     - font
     - fontSize
     - color
     - mode
     - inputable
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - img
     - x
     - y
     - w
     - h
     - scaleX
     - scaleY
     - rotationAngle
     - blendMode
     - opacity
     - file
     - folder
     - duration
     - 
     - 
     - 
     - 
     - 
   * - seekbar
     - x
     - y
     - w
     - h
     - folder
     - picture
     - orientation
     - centralX
     - centralY
     - startDegree
     - endDegree
     - reverse
     - blendMode
     - opacity
     - 
     - 
     - 
   * - tabview
     - x
     - y
     - w
     - h
     - transition
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - tab
     - x
     - y
     - w
     - h
     - idx
     - idy
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - curtainview
     - x
     - y
     - w
     - h
     - transition
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - curtain
     - x
     - y
     - w
     - h
     - scope
     - orientation
     - transition
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - icon
     - x
     - y
     - w
     - h
     - font
     - picture
     - highlightPicture
     - fontColor
     - fontSize
     - text
     - textX
     - textY
     - pictureX
     - pictureY
     - mode
     - blendMode
     - opacity
   * - script
     - file
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - switch
     - x
     - y
     - w
     - h
     - picture
     - highlightPicture
     - clickedPicture
     - clickedHighlightPicture
     - pictureX
     - pictureY
     - blendMode
     - opacity
     - mode
     - duration
     - 
     - 
     - 
   * - page
     - x
     - y
     - w
     - h
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - screen
     - w
     - h
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - grid
     - x
     - y
     - rowNumber
     - colNumber
     - rowGap
     - colGap
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - gallery
     - x
     - y
     - w
     - h
     - folder
     - mainBg
     - centerBg
     - centerPercent
     - sideScale
     - sidePosPercent
     - blendMode
     - opacity
     - 
     - 
     - 
     - 
     - 
   * - animateTransform
     - type
     - from
     - to
     - dur
     - repeatCount
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - motorizedCurtain
     - x
     - y
     - w
     - h
     - switchOpen
     - switchClose
     - switchPause
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - keyboard
     - ime
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - multiLevel
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - onClick
     - type
     - to
     - id1
     - id2
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
   * - backIcon
     - x
     - y
     - w
     - h
     - picture
     - highlightPicture
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 
     - 



.. list-table::
   :header-rows: 1
   :widths: 6 10 20

   * - Attribute
     - Description
     - Values
   * - x
     - Relative left coordinate
     - number
   * - y
     - Relative top coordinate
     - number
   * - w
     - Width
     - number
   * - h
     - Height
     - number
   * - hidden
     - Hidden
     - hidden
   * - text
     - Text string
     - string
   * - font
     - Font file
     - file path
   * - fontSize
     - Font size
     - number
   * - color
     - RGB hex color
     - #RRGGBB
   * - mode(textbox)
     - Text effect
     - truncate, verticalscroll, horizontalscroll, transition
   * - mode(icon)
     - Highlight effect on press
     - normal, fade, scale, fadeScale, array
   * - mode(switch)
     - Highlight effect on press
     - array
   * - inputable
     - Soft keyboard
     - boolean
   * - scaleX
     - Horizontal scaling ratio
     - number
   * - scaleY
     - Vertical scaling ratio
     - number
   * - rotationAngle
     - Rotation angle
     - number
   * - blendMode
     - Image blend mode
     - imgBypassMode, imgFilterBlack, imgSrcOverMode, imgCoverMode
   * - opacity
     - Opacity from 0 to 255
     - number
   * - file
     - File path
     - string
   * - folder
     - Folder path
     - string
   * - duration
     - Animation duration (milliseconds)
     - number
   * - picture
     - Image file path
     - string
   * - orientation(seekbar)
     - Orientation
     - vertical, V, horizontal, H, arc
   * - orientation(curtain)
     - Direction
     - middle, up, down, left, right
   * - centralX
     - Arc center x-coordinate
     - number
   * - centralY
     - Arc center y-coordinate
     - number
   * - startDegree
     - Arc starting angle
     - number
   * - endDegree
     - Arc ending angle
     - number
   * - transition
     - Transformation effect
     - normal, fade, scale, fadeScale
   * - idx
     - Horizontal index
     - number
   * - idy
     - Vertical index
     - number
   * - scope
     - Range (from 0 to 1)
     - number
   * - highlightPicture
     - Highlight image file path
     - string
   * - fontColor
     - RGB hex color
     - #RRGGBB
   * - textX
     - Relative x-coordinate of text
     - number
   * - textY
     - Relative y-coordinate of text
     - number
   * - pictureX
     - Relative x-coordinate of image
     - number
   * - pictureY
     - Relative y-coordinate of image
     - number
   * - rowNumber
     - Number of rows
     - number
   * - colNumber
     - Number of columns
     - number
   * - rowGap
     - Row spacing
     - number
   * - colGap
     - Column spacing
     - number
   * - mainBg
     - Main background image file path
     - string
   * - centerBg
     - Center background image file path
     - string
   * - centerPercent
     - Center area percentage
     - number
   * - sideScale
     - Default scaling ratio for side images
     - number
   * - sidePosPercent
     - Side image position percentage
     - number
   * - type(animateTransform)
     - Animation type
     - rotate
   * - from
     - Starting value of animation
     - number
   * - to(animateTransform)
     - End value of animation
     - number
   * - dur
     - Animation duration
     - number
   * - repeatCount
     - Number of animation repetitions
     - number
   * - switchOpen
     - Motorized curtain open button name
     - string
   * - switchClose
     - Motorized curtain close button name
     - string
   * - pauseOpen
     - Motorized curtain pause button name
     - string
   * - ime
     - Input method
     - null, pinyin
   * - type(onClick)
     - Behavior type triggered by click event
     - jump, control
   * - to(onClick)
     - Action target
     - light, multiLevel
   * - id1
     - Main parameter
     - number
   * - id2
     - Secondary parameter
     - number




Example
=======

Win 
---

.. code-block:: xml

   <win
       x="0"
       y="0"
       w="480"
       h="480">cool_win
   </win>

Img 
---

.. code-block:: xml

   <img
       x="80"
       y="70"
       w="303"
       h="239"
       opacity="255"
       file="app/box/resource/new_folder/aa2.bin"
       blendMode="imgFilterBlack"
       rotationAngle="0"
       scaleX="1"
       scaleY="1">image3
   </img>