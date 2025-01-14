
==========
XML 语法
==========


元素
-------

- 元素对应控件
- 元素的属性对应控件的属性（0 值可以忽略）
- 文本内容对应控件实例的名称

.. code-block:: xml

   <type a1="xx" a2="xx" a3="xx" a4="xx">name</type> 


嵌套
-----

元素的嵌套结构与实际控件的嵌套结构一致



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

规格 
==========

.. list-table::
   :header-rows: 1
   :widths: 10 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8

   * - 元素
     - 属性1
     - 属性2
     - 属性3
     - 属性4
     - 属性5
     - 属性6
     - 属性7
     - 属性8
     - 属性9
     - 属性10
     - 属性11
     - 属性12
     - 属性13
     - 属性14
     - 属性15
     - 属性16
     - 属性17
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

   * - 属性
     - 简介
     - 值
   * - x
     - 相对左坐标
     - number
   * - y
     - 相对上坐标
     - number
   * - w
     - 宽度
     - number
   * - h
     - 高度
     - number
   * - hidden
     - 隐藏
     - hidden
   * - text
     - 文本字符串
     - string
   * - font
     - 字体文件
     - file path
   * - fontSize
     - 字号
     - number
   * - color
     - 三原色十六进制色彩
     - #RRGGBB
   * - mode(textbox)
     - 文本特效
     - truncate, verticalscroll, horizontalscroll, transition
   * - mode(icon)
     - 按压高亮效果
     - normal, fade, scale, fadeScale, array
   * - mode(switch)
     - 按压高亮效果
     - array
   * - inputable
     - 软键盘
     - boolean
   * - scaleX
     - 水平缩放比例
     - number
   * - scaleY
     - 竖直缩放比例
     - number
   * - rotationAngle
     - 旋转角度
     - number
   * - blendMode
     - 图片混合模式
     - imgBypassMode, imgFilterBlack, imgSrcOverMode, imgCoverMode
   * - opacity
     - 不透明度 从零到二百五十五
     - number
   * - file
     - 文件路径
     - string
   * - folder
     - 文件夹路径
     - string
   * - duration
     - 动画时长（毫秒）
     - number
   * - picture
     - 图片文件路径
     - string
   * - orientation(seekbar)
     - 方向
     - vertical, V, horizontal, H, arc
   * - orientation(curtain)
     - 方向
     - middle, up, down, left, right
   * - centralX
     - 圆弧中心横坐标
     - number
   * - centralY
     - 圆弧中心纵坐标
     - number
   * - startDegree
     - 圆弧起始角
     - number
   * - endDegree
     - 圆弧结束角
     - number
   * - transition
     - 变换特效
     - normal, fade, scale, fadeScale
   * - idx
     - 水平索引
     - number
   * - idy
     - 竖直索引
     - number
   * - scope
     - 范围（从零到一）
     - number
   * - highlightPicture
     - 高亮图片文件路径
     - string
   * - fontColor
     - 三原色十六进制色彩
     - #RRGGBB
   * - textX
     - 文字相对横坐标
     - number
   * - textY
     - 文字相对纵坐标
     - number
   * - pictureX
     - 图片相对横坐标
     - number
   * - pictureY
     - 图片相对纵坐标
     - number
   * - rowNumber
     - 行数
     - number
   * - colNumber
     - 列数
     - number
   * - rowGap
     - 行间距
     - number
   * - colGap
     - 列间距
     - number
   * - mainBg
     - 主要背景图片文件路径
     - string
   * - centerBg
     - 中央背景图片文件路径
     - string
   * - centerPercent
     - 中央区域占比
     - number
   * - sideScale
     - 侧边图片默认缩放比例
     - number
   * - sidePosPercent
     - 侧边图片位置占比
     - number
   * - type(animateTransform)
     - 动画类型
     - rotate
   * - from
     - 动画起始值
     - number
   * - to(animateTransform)
     - 动画终点值
     - number
   * - dur
     - 动画时长
     - number
   * - repeatCount
     - 动画重复次数
     - number
   * - switchOpen
     - 电动窗帘开启按钮名称
     - string
   * - switchClose
     - 电动窗帘关闭按钮名称
     - string
   * - pauseOpen
     - 电动窗帘暂停按钮名称
     - string
   * - ime
     - 输入法
     - null, pinyin
   * - type(onClick)
     - 点击事件触发的行为类型
     - jump, control
   * - to(onClick)
     - 行为目标
     - light, multiLevel
   * - id1
     - 主参数
     - number
   * - id2
     - 次参数
     - number


示例
=======

窗口 
------

.. code-block:: xml

   <win
       x="0"
       y="0"
       w="480"
       h="480">cool_win
   </win>

图片
------

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







