## 概述
RTKIOT可视化设计工具是一个用于为Realtek系列IC制作图形界面设计的工具，目前支持的IC列表如下表所示。

| 序号 | 支持的IC  |
|------|-----------|
| 1    | RTL8762D  |
| 2    | RTL8762G  |
| 3    | RTL8763E  |
| 4    | RTL8772G  |
| 5    | TBD       |


RTKIOT 可视化设计工具支持以下功能：
+ 从工具箱中拖拽控件并将其放置在设计视图中。
+ 拖放控件以在设计视图中更改其位置，或通过属性视图修改控件的位置和外观。
+ 导出用户设计的GUI项目为```.bin```文件，将```.bin```文件烧录到IC上以显示图形界面。
+ 在PC上模拟GUI项目。

本文档主要包括以下内容：
+ [功能面板](#function_panel_anchor)
+ [资源管理](#resource_manage_anchor)
+ [菜单栏](#menu_anchor)
+ [快速入门教程](#quick_start_anchor)
+ [GUI演示项目](#gui_demo_anchor)

为了简化文档，下文中使用‘工具’来指代‘RTKIOT可视化设计工具’。


<div STYLE="page-break-after: always;"></div> 

<a id="function_panel_anchor"></a>

## 功能面板

### 工具箱/控件
+ 非容器化控件
    + 可作为其他控件的父控件。
    + 父控件与子控件之间存在坐标跟随关系。
    + 当子控件超出父控件范围时仍可显示。
+ 容器化控件
    + 可作为其他控件的父控件。
    + 父控件与子控件之间存在坐标跟随关系。
    + 当子控件超出父控件范围时仍可显示。
    + 可从工具箱中将控件拖放到容器控件中。

本节列出了小部件支持的属性，并用'Y'或'N'标记是否IC支持该属性。

#### 非容器化控件
##### 文本 (Text)
仅用于文本显示，不支持用户输入。属性如下表所示。

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名称 | Y | Y | Y |
| Size (Height)  | 控件高度 | Y | Y | Y |
| Size (Width)   | 控件宽度 | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |
| Text     	     | 显示文本              | Y | Y | Y |
| Display Mode   | 长文本（超出控件范围的文本内容）<br/>的显示模式可使用以下支持的类型：<br/>截断显示模式（truncate）：截断文本显示模式<br/>垂直滚动显示模式（verticalscroll）：垂直滚动文本显示模式<br/>水平滚动显示模式（horizontalscroll）：水平滚动文本显示模式 | Y  | Y | Y |
| Font           | 字体转换设置请参考 [字体转换设置](#font_setting_anchor). | Y | Y | Y |
| Font Color (RGBA) | 字体颜色设置，使用RGBA | Y | Y | Y |

##### 按钮 (Button)
可点击的控件，具有文本和背景图片。属性如下表所示。

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名称    | Y | Y | Y |
| Size (Height)  | 控件高度    | Y | Y | Y |
| Size (Width)   | 控件宽度    | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |
| Text           | 显示文本              | Y | Y | Y |
| Text X         | 相对于按钮控件的水平坐标 | Y | Y | Y |
| Text Y         | 相对于按钮控件的垂直坐标 | Y | Y | Y |
| Display Mode   | 水平或垂直显示         | Y | Y | Y |
| Font           | 字体转换设置请参考 [字体转换设置](#font_setting_anchor) | Y | Y | Y |
| Text Color (RGB) | 文本颜色设置，使用RGB | Y | Y | Y |
| Transition       | 图像转换模式有以下选项：<br/>normal: 无效果<br/>fade: 淡入/淡出<br/>scale: 缩放<br/>fadeScale: 淡入/淡出和缩放<br/>注意：只有在设置了默认和高亮背景图片时，变换模式才会生效，否则默认为normal | N | Y | Y |
| BG Image (Default)     | 默认的背景图像       | Y | Y | Y |
| BG Image (Highlight)   | 选定/高亮显示的背景图像 | Y | Y | Y |
| BG Imge Rotation Angle | 背景图像旋转角度，范围：0~360度 | Y | Y | Y |

##### 图像 (Image)
能够设置图像的控件，其属性如下表所示。

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名称     | Y | Y | Y |
| Size (Height)  | 控件高度     | Y | Y | Y |
| Size (Width)   | 控件宽度     | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |
| Image        | 图像路径<br/> 注意：图像必须预先导入到项目中。详细请参考 [图像资源管理](#img_res_m_anchor) | Y | Y | Y |
| Image Rotation Angle | 图像旋转角度 | Y | Y | Y |
| Image Scale X  | 图像水平缩放程度，是一个倍数/百分比。<br/> 例如，设置比例x 为0.5表示图像的实际显示宽度是原始图像宽度的一半  | Y | Y | Y |
| Image Scale Y  | 图像垂直缩放程度，是一个倍数/百分比 | Y | Y | Y |


```{note}
1.	在导出时，工具将转换导入的图像。可以在‘菜单栏 -> 设置 -> 图像转换设置’中设置图像转换参数，详细请参考 [图像转换设置](#img_conv_setting_anchor)。
2.	如果导入的图像大小与控件的大小不匹配，工具不会对图像进行缩放或裁剪。
```


##### 滑动条 (SeekBar)
滑动控件，可以响应用户滑动手势，并改变进度值。其属性如下表所示。

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名      | Y | Y | Y |
| Size (Height)  | 控件高度    | Y | Y | Y |
| Size (Width)   | 控件宽度    | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |
| Color(Highlight) (RGBA) | 进度条完成部分的背景色 | N | Y | N |
| Color (RGBA)            | 整个进度条的背景色     | N | Y | N |
| Orientation             | 控件显示方向和手势响应方向的类型如下：<br/> vertical/V: 垂直方向<br/> arc: 曲线的方向<br/> horizontal/H: 水平方向        | Y | Y | Y |

<div id="figure_2_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817870508689816/f43215ff_12407535.png" style="width: 400px"></div>

##### 图像滑动条 (Image SeekBar)
具有多个图像作为背景的滑动控件，用户滑动时可以切换到不同的图像，其属性如下表所示。

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名      | Y | Y | Y |
| Size (Height)  | 控件高度    | Y | Y | Y |
| Size (Width)   | 控件宽度    | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |
| Degree (Start) | 起始角度（如果方向是曲线则无效）| Y | Y | Y |
| Degree (End)   | 结束角度（如果方向是曲线则无效）| Y | Y | Y |
| Image Directory | 包含要在此控件上显示的图像的文件夹<br/> 注意：<br/> 1. 请按名称对图像进行排序；<br/> 2. 当用户在控件上滑动时，控件将根据当前进度切换背景图像。| Y | Y | Y |
| Central X      | 弧的中心相对于父控件的水平坐标 | Y | Y | Y |
| Central Y      | 弧的中心相对于父控件的垂直坐标   | Y | Y | Y |
| Orientation    | 控件显示方向和手势响应方向的类型如下：<br/> vertical/V: 垂直方向<br/> arc: 曲线的方向<br/> horizontal/H: 水平方向                                                 |      Y      |      Y      |  Y   |

##### 开关 (Switch)
具有‘已选中’和‘未选中’状态的开关控件，其属性如下表所示。

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名称    | Y | Y | Y |
| Size (Height)  | 控件高度    | Y | Y | Y |
| Size (Width)   | 控件宽度    | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |
| BG Image (Checked) | 已选中状态的背景图像 | Y | Y | Y |
| BG Image (Default) | 未选中状态的背景图像 | Y | Y | Y |

##### 圆弧 (Arc)
弧形控件，暂时不支持手势，其属性如下表所示。

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名称     | Y | Y | N |
| Size (Height)  | 控件高度     | Y | Y | N |
| Size (Width)   | 控件宽度     | Y | Y | N |
| X              | 相对于父控件的水平坐标 | Y | Y | N |
| Y              | 相对于父控件的垂直坐标 | Y | Y | N |
| Central X      | 圆弧的中心相对于父控件的水平坐标 | N | Y | N |
| Central Y      | 圆弧的中心相对于父控件的垂直坐标 | N | Y | N |
| BG Color       | 圆弧背景颜色  | N | Y |  N  |
| Cap Mode       | 圆弧端点样式，支持以下选项：<br/>圆形/平头/方形 | N | Y | N |
| Degree (End)   | 圆弧的结束度数 | N | Y | N |
| Degree (Start) | 圆弧的开始度数 | N | Y | N |
| Radius         | 圆弧的半径     | N | Y | N |
| Stroke Width   | 圆弧的描边宽度 | N | Y | N |

<div id = "figure_2_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817869829442326/90524d4c_12407535.png"/></div>

#### 容器控件
##### 屏幕 (Screen)
屏幕控件，对应于物理屏幕，是GUI项目的根控件，其属性如下表所示。


```{note}
只能修改Name属性。
```

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名称     | Y | Y | Y |
| Size (Height)  | 控件高度     | Y | Y | Y |
| Size (Width)   | 控件宽度     | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |

##### 选项卡视图 (TabView) 和选项卡 (Tab)
选项卡视图控件作为父控件，支持上下左右滑动来在选项卡之间切换。选项卡视图控件具有以下属性，如下表所示。


| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名称     | Y | Y | Y |
| Size (Height)  | 控件高度     | Y | Y | Y |
| Size (Width)   | 控件宽度     | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |
| Transition     | 选项卡切换模式支持以下类型：<br/>normal: 无特效<br/>fade: 淡入/淡出<br/>scale: 缩放<br/>fadeScale: 淡入/淡出和缩放 | N | Y | Y |
<br/>

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名称     | Y | Y | Y |
| Size (Height)  | 控件高度     | Y | Y | Y |
| Size (Width)   | 控件宽度     | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |
| Index(X-Axis)  | 选项卡视图中选项卡的水平索引 | Y | Y | Y |
| Index(Y-Axis)  | 选项卡视图中选项卡的垂直索引 | Y | Y | Y |

<div id = "figure_2_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817871000642675/ca6cda22_12407535.png" width="300"/></div>
<br/>

```{note}
1.	选项卡视图的宽度和高度不能被修改，会默认设置为屏幕的宽度和高度。
2.	选项卡视图的水平和垂直坐标不能被修改，始终为0。
3.	选项卡视图只能作为屏幕控件的子控件使用。
4.	选项卡视图的子控件只能是选项卡。
5.	选项卡的宽度和高度不能被修改，会默认设置为选项卡视图的宽度和高度。
6.	选项卡的水平和垂直坐标不能被修改，始终为0。
```

##### 页面 (Page)
具有可滚动内容的容器控件。

```{note}
1.	页面只支持垂直滚动。
2.	页面控件的宽度和高度只定义了对滑动手势响应的界面区域，是否允许滚动取决于是否将其添加到超出屏幕范围的子控件中。
```

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名称     | Y | Y | Y |
| Size (Height)  | 控件高度     | Y | Y | Y |
| Size (Width)   | 控件宽度     | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |

##### 窗口 (Win)
在窗口的宽度和高度定义的区域内，可以响应各种手势，包括点击、长按、按下、释放和滑动，其属性如下表所示。

| 属性 | 描述 | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|------|-----|:-----------:|:-----------:|:---:|
| Name           | 控件名称     | Y | Y | Y |
| Size (Height)  | 控件高度     | Y | Y | Y |
| Size (Width)   | 控件宽度     | Y | Y | Y |
| X              | 相对于父控件的水平坐标 | Y | Y | Y |
| Y              | 相对于父控件的垂直坐标 | Y | Y | Y |
| Hidden		 | 表示是否隐藏窗口及其子控件 | Y | Y | Y |

### 设计视图/画布
用户可以从工具箱面板中拖放控件到设计视图中，调整控件的布局并设置属性，以设计一个可以在瑞昱 IC中渲染的图形界面。

<div style="text-align:center;"><img src="https://foruda.gitee.com/images/1721627839639298235/033d3a5b_12407535.png" width="680"></div>


#### 选项卡视图 - 创建/删除/插入选项卡
从工具箱中拖放选项卡视图控件到设计视图中，创建出一个只包含一个主页选项卡（坐标（0,0））的选项卡视图，如下图所示。

<div style="text-align:center;"><img src="https://foruda.gitee.com/images/1721627964036015694/3b5ee0d4_12407535.png" width="680"></div>

##### 创建选项卡
可以通过点击设计视图周围的按钮来创建新的选项卡。


```{note}
1.	如果idx为0，则上下按钮可用。
2.	如果idy为0，则左右按钮可用。
```


##### 删除选项卡
选择要删除的选项卡，在菜单栏上单击‘编辑->删除’或按键盘上的Delete键，然后再次确认是否要删除它。

<div id = "figure_2_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628091462645405/beca8dd5_12407535.png" width="220"></div>

##### 插入选项卡
目前，选项卡的插入只支持通过修改现有选项卡的坐标并创建新选项卡来实现。

例如，需要在坐标（1,0）和（2,0）的选项卡之间插入一个选项卡，步骤如下。
1.	将选项卡（2,0）及其右侧的选项卡的idx加一，如下图所示；
2.	切换到选项卡（1,0），点击 创建新的选项卡（2,0）。

<div id="figure_2_7_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726815120686817698/cdf44743_12407535.png" width="680"></div>
<br/>
<div id="figure_2_7_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726815280073982051/68230d3d_12407535.png" width="680"></div>
<br/>
<div id="figure_2_7_c" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628460504196478/3446e06e_12407535.png" width="680"></div>

#### 选项卡视图概览图
请点击 ![icon](https://foruda.gitee.com/images/1710818993805990437/796af0dc_12407535.png)  查看选项卡视图概览图。


```{note}
1.	在概览图中，高亮显示的选项卡表示当前在设计视图中正在编辑的选项卡。
2.	概览图使用选项卡的坐标进行标注。当在IC中进行模拟或渲染时，坐标为（0, 0）的选项卡显示在主页上，用户可以上下左右滑动以显示其他选项卡。
```


<div id="figure_2_8_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628617214746327/0711439d_12407535.png" width="680"></div>
<br/>
<div id="figure_2_8_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628633709549622/a41c6011_12407535.png" width="680"></div>

#### 设计视图的缩放
有三种方式可以对设计视图进行缩放。
1.	按住```Ctrl```键，并滚动鼠标滚轮。
2.	点击‘-’和‘+’按钮。
3.	拖动滑动条。

<div id="figure_2_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628744851323697/d56f9725_12407535.png" width="680"></div>

### 属性视图
在控件树或设计视图中选择一个控件，可以显示出所有控件的属性值，用户可以根据需要进行修改。

<div id="figure_2_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726814067710788486/a6564f40_12407535.png" width="680"></div>

### 控件树
控件树用于向用户展示当前布局中控件之间的父子关系和兄弟关系。我们遵循以下约定：
1.	子控件位于父控件的上方，即当父控件和子控件重叠时，子控件会覆盖父控件。
2.	兄弟控件的图层顺序与添加顺序相关，先添加的控件在底部，后添加的控件在顶部。

下图展示了主页选项卡和灯选项卡中的所有子控件，其中主页选项卡只有一个背景图片控件，而灯选项卡包含一个图片控件和多个开关控件。

<div id="figure_2_11" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628970571915820/15772594_12407535.png" width="680"></div>
<br/>
<div id="figure_2_11_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721628988181629026/d9ff763a_12407535.png" width="680"></div>
<br/>

控件树支持以下操作：
1.	选择控件：如果在控件树中选择一个控件，设计视图中对应的控件会聚焦，并在属性视图中显示其属性。
2.	修改父子关系：在控件树中选择一个控件（除了选项卡/选项卡视图/屏幕），并将其拖放到目标控件项上，该控件将成为目标控件的子控件。
3.	修改控件图层：在控件树中选择一个控件（除了选项卡/选项卡视图/屏幕），将其拖放到目标控件项的上方或下方，在设计视图中，该控件将在目标控件的上方或下方。
4.	锁定控件：点击锁定按钮，锁定一个或多个控件。
    + 如果点击屏幕的锁定按钮，所有屏幕的子控件将被锁定，用户将无法在设计视图中拖动或调整控件大小。
    + 如果点击选项卡的锁定按钮，所有选项卡的子控件将被锁定，用户将无法在设计视图中拖动或调整控件大小。

<div id="figure_2_12" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726816487543506275/da6ebe9d_12407535.png"></div>
<br/>
<div id="figure_2_12_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726816816056161349/efcdd833_12407535.png"></div>

<div STYLE="page-break-after: always;"></div> 

<a id="resource_manage_anchor"></a>

## 资源管理

只有预导入的图像和字体文件可以被GUI项目引用。本章重点介绍如何管理图像和字体资源。
图像和字体资源管理器位于设计视图正下方，如下图所示。

<div id="figure_3_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629333990020010/27b6b02c_12407535.png" width="680"></div>

<a id="img_res_m_anchor"></a>

### 图像资源管理

点击 ![icon](https://foruda.gitee.com/images/1710824254768634573/d752fbd6_12407535.png)  打开图像管理器，如下图所示。

<div id="figure_3_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726815968480737363/e2f46e96_12407535.png" width="400"></div>

##### 添加图像
通过以下步骤可以将图像添加到GUI项目中。
1.	点击![icon](https://foruda.gitee.com/images/1710824254786867861/174e524d_12407535.png) ，创建一个新的图像文件夹并输入文件夹名称。创建的文件夹位于GUI项目目录下的```Resource\image```文件夹中。

<div id="figure_3_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629445428238950/c3e216ca_12407535.png" width="400"></div>
<br/>

2.	选择创建的图像文件夹，然后点击 ![icon](https://foruda.gitee.com/images/1710824254769021374/55b6c303_12407535.png) 选择图像（支持多选），将它们添加到文件夹中。添加完成后，图像会被复制到```Resource\image\home```文件夹中。

<div id="figure_3_4_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629572036008240/98958d66_12407535.png" width="400"></div>
<br/>

<div id="figure_3_4_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1725412988097005313/b591b7ca_12407535.png" width="600"></div>
<br/>
<div id="figure_3_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629616915297638/de3a62a7_12407535.png" width="400"></div>

##### 移除图像/图像文件夹
选择要移除的图像或图像文件夹，然后点击 ![icon](https://foruda.gitee.com/images/1710824483012674071/c0c62b91_12407535.png)。

##### 重命名图像文件夹
选择图像文件夹，双击并输入一个新名称。

#### 预览图像
选择图像文件夹，右侧区域将显示该文件夹中的所有图像。

<div id="figure_3_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824483098291044/697fd630_12407535.png"></div>

#### 刷新
如果用户在本地操作图像资源而不是通过工具进行操作，可以点击   刷新。


```{note}
不推荐的做法。
```


### 字体资源管理

#### 添加第三方字体
如果需要使用第三方字体（.ttf），请先点击 ![icon](https://foruda.gitee.com/images/1710824482944711702/8b73005d_12407535.png) 导入资源，否则将使用本地安装的字体。

<div id="figure_3_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824483191984010/283b1f70_12407535.png" width="600"></div>

#### 移除第三方字体
选择要移除的字体，然后点击  ![icon](https://foruda.gitee.com/images/1710824483012674071/c0c62b91_12407535.png)。

<div STYLE="page-break-after: always;"></div> 

<a id="menu_anchor"></a>

## 菜单栏

### 文件
#### 起始页
如果要关闭当前项目并打开一个已存在的项目或新建一个项目，可以通过点击‘文件 -> 起始页’来打开起始页。
点击‘打开项目’或选择一个```.rtkprj```文件并双击打开已存在的项目，或者点击‘创建项目’来创建新项目，请参考 [如何创建项目](#how_to_create_anchor) 和 [如何打开项目](#how_to_open_anchor)。


<div id="figure_4_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629731453730155/b3e315a3_12407535.png" width="680"></div>

#### 保存
保存项目的所有UI更改，快捷键是```Ctrl + S```。

#### 退出保存
当关闭项目时会弹出提示窗口，如下所示。请点击‘确定’进行保存，否则更改将会丢失。

<div id="figure_4_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721629849823263224/4adf7819_12407535.png" width="300"></div>

### 编辑
#### 复制/粘贴
1.	点击‘编辑 -> 复制’以复制所选控件，快捷键是```Ctrl + C```。
2.	点击‘编辑 -> 粘贴’以在设计视图中创建所选控件的副本，快捷键是```Ctrl + V```。

#### 删除
点击‘编辑 -> 删除’以删除所选控件，或按下键盘上的‘Delete’键。

#### 撤销/重做
撤销：撤销对控件所做的更改，快捷键是```Ctrl + Z```。
重做：重新对控件进行更改，快捷键是```Ctrl + Y```。

#### 转换项目
转换项目窗口用于转换当前项目的IC类型和屏幕尺寸/分辨率。

<div id="figure_4_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630083752972113/2f32ba1a_12407535.png" width="400"></div>

#### 修改项目名称
修改项目名称窗口用于修改当前项目的名称。请在输入框中输入新名称。
 
<div id="figure_4_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630055522858677/e4245c27_12407535.png" width="400"></div>

### 设置

{#img_conv_setting_anchor}
#### 图像转换设置
图像必须转换才能在IC上正确显示，因此用户需要设置正确的转换参数。所有可选参数如下图所示。

<div id="figure_4_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630165031575995/1be72944_12407535.png" width="400"></div>
<br/>

参数描述如下。

##### 扫描模式
可用的选项如下表所示。

| 扫描模式 | 描述     |
|----------|----------|
| *水平*   | 水平扫描 |
| *垂直*   | 垂直扫描 |

##### 颜色空间
图像的颜色空间，可用的选项如下表所示。

| 颜色空间  | 描述                                       |
|----------|--------------------------------------------|
| RGB565  | 16位RGB模式<br>位 4:0 表示蓝色；位 10:5 表示绿色；位 15:11 表示红色。 |
| RTKARGB | 16位ARGB模式<br>位 4:0 表示蓝色；位 9:5 表示绿色；位 14:10 表示红色; 位 15 表示透明度。 |
| RTKRGAB | 16位RGAB模式<br>位 4:0 表示蓝色；位 5 表示透明度；位 10:6 表示绿色；位 15:11 表示红色。 |
| RGB     | 24 位 RGB mode<br>位 7:0 表示蓝色；位 15:8 表示绿色；位 23:16 表示红色。 |
| RGBA    | 32 位 RGBA mode<br>位 7:0 表示蓝色；位 15:8 表示绿色；位 23:16 表示红色; 位 31:24 表示透明度。 |
| BINARY  | 2-值 (0 或 1) 图像  |

##### 压缩
如果勾选‘压缩’，请根据需要设置压缩参数。
可选的压缩模式如下：
1.	行程长度编码（Run-Length Encoding）

    一种无损压缩算法。 如果选择RLE作为压缩模式，则需要设置RLE级别和RLE行程长度参数。

<div id="figure_4_6_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726818028936954872/d4977120_12407535.png" width="350"></div>
<br/>
<div id="figure_4_6_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726818316484821859/eeb87b7f_12407535.png" width="350"></div>
<br/>

| RLE 级别 |                      描述                      |
|---------|-----------------------------------------------|
| Level1 | 1级压缩                                         |
| Level2 | 2级压缩，基于1级压缩的次级压缩                    |

| RLE 行程长度 |           描述          |
|-------------|-------------------------|
| Byte_1     | 1个字节，最大为255     |
| Byte_2     | 2个字节，最大为255     |

```{note}
RLE行程长度：每个行程（Run）在压缩过程中允许的最大重复字符长度。
```

2.	FastLz: 

    一种基于字典和滑动窗口的无损压缩算法，用于压缩具有大量重复值的数据。

3.	YUV_Sample_Blur: 

    一种结合YUV采样和模糊处理的有损压缩算法。

    YUV 采样：保留图像的亮度信息，并只对色度信息进行采样。

    模糊处理：在YUV采样后丢弃每个字节的低位，以达到数据压缩的目的。

| YUV 采样模式 | 描述           |
|-------------|----------------|
| YUV444     | 采样4个像素数据到4个Y、4个U和4个V，即每个Y对应一组UV分量，完全保留YUV数据。|
| YUV422     | 每4个像素数据采样为4个Y、2个U和2个V，即每2个Y对应一组UV分量，数据大小为原始数据的75%。|
| YUV411     | 每4个像素数据采样为4个Y、1个U和1个V，即每4个Y对应一组UV分量，数据大小为原始数据的50%。|
| YUV422     | Y - 亮度； V - 色度 |

| 模糊处理模式 | 描述                                       |
|--------------|--------------------------------------------|
| Bit0         | 不丢弃低位的保存方式                      |
| Bit1         | 每个字节丢弃位0 (保留 [位7:位1])          |
| Bit2         | 每个字节丢弃 [位1:位0] (保留 [位7:位2])   |
| Bit4         | 每个字节丢弃 [位3:位0] (保留 [位7:位4])   |

4.	YUV_Sample_Blur+FastLz

    该算法结合了YUV_Sample_Blur和FastLz算法。

{#font_setting_anchor}
#### 字体转换设置
包括位图字体和矢量字体。支持Realtek系列IC的字体在下表中显示。


```{note}
您至少需要创建一个字体转换设置，否则无法在属性视图中为文本类型控件选择字体。
```


| 字体    | RTL8762D/RTL8763E | RTL8762G/RTL8772G | TBD |
|---------|--------------|-------------|-----|
| Vector（矢量）  |      N       |      N      |  Y  |
| Bitmap（位图）  |      Y       |      Y      |  Y  |

要使用位图字体，您需要设置以下参数。

<div id="figure_4_7" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630232320631022/466a71af_12407535.png" width="400"></div>
<br/>

下表列出了每个参数的描述。

| 参数           | 描述               |
|---------------|--------------------|
| Font Setting Name | 用户定义的字体设置名称，<br>请确保每次创建不同的字体设置名称。|
| Font Size         | 字体大小 |
| Bold              | 是否为粗体 |
| Italic            | 是否为斜体 |
| Render Mode       | 渲染模式，用于表示在转换的```.bin```文件中像素的位数 |
| Scan Mode         | 扫描模式，保存```.bin```时有两种扫描方式<br/>H: 水平扫描<br/>V: 垂直扫描 |
| Index Method      | 索引方式，转换的```.bin```文件重新索引区域的索引方法 |
| Code Page         | 字库代码页，支持多个代码页 |
| Text Type         | 文本类型，有以下几种。<br/>范围：如果文本的Unicode范围可以预先确定，请选择此类型，并在范围文本框中输入范围。支持多个范围，请将每个范围单独设置在一行中。<br/>注意：只有范围内的字符将被转换并保存到```.bin```文件中，可以节省存储空间。<br/>随机：如果文本的Unicode范围无法预先确定，请选择此类型。<br/>注意：字体的所有字符都将被转换并保存到```.bin```文件中。|

有关矢量字体的参数，请参考下图。

<div id="figure_4_8" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630326763929958/7a1b4010_12407535.png" width="400"></div>

### 导出
如果您已经完成了GUI项目的设计，并且想将其烧录到IC中，请点击‘导出’，然后工具将执行以下操作：
1.	图像转换
2.	字体转换
3.	将```.xml```、```.js```、图像和字体打包到输出的```.bin```文件中。

当上述操作完成后，会弹出一个消息框。

<div id="figure_4_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630355854380192/60bbc0af_12407535.png" width="400"></div>
<br/>

```.bin```文件可以烧录进您的IC中。

### 模拟
在UI上模拟项目。


```{note}
当您第一次模拟项目时，请先点击‘导出’，然后再点击‘模拟’。如果您没有修改任何图像或字体设置，则无需再次点击‘导出’。
```


下图显示为模拟器运行效果。

<div id="figure_4_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828505193590425/a8c6e2c0_12407535.png" width="350"></div>

<div STYLE="page-break-after: always;"></div> 

<a id="quick_start_anchor"></a>

## 快速入门教程

<a id="how_to_create_anchor"></a>

### 如何创建项目

<div id="figure_5_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721630502223709854/341d9828_12407535.png" width="680"></div>
<br/>

双击运行```RVisualDesigner.exe```，然后按照步骤（1~4）配置项目，点击‘创建项目’（5）。
创建项目后，GUI设计窗口将弹出。左侧是组件区域，中间是设计区域，右侧是部件属性设置区域。

<div id="figure_5_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1726818698027723674/8962d02e_12407535.png" width="680"></div>
<br>

```{note}
新创建的项目文件位于解决方案文件夹下的项目文件夹中。下图展示了一个示例。
```

<div id="figure_5_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1725505762180091944/39be3e5a_12407535.png" width="330"></div>
<br/>

当在设计视图上拖放一个控件，并点击‘文件 - 保存’或按下```Ctrl + S```时，将创建一个```.rtkui```文件。

<div id="figure_5_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1725505619255178068/e89d3b76_12407535.png" width="330"></div>


### 如何编写 JavaScript 代码
项目创建完成后，会生成一个 ```xxx.js``` 文件，打开该文件，在其中编写 JavaScript 代码以实现控件的事件回调函数。

<a id="how_to_open_anchor"></a>

### 如何打开项目

<div id="figure_5_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636293745281159/b5fbf3b0_12407535.png" width="680"></div>
<br/>

有两种打开项目的方式：
1.	点击‘打开项目’，选择一个```.rtkprj```文件进行打开。

<div id="figure_5_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1724911708175163337/8513c36b_12407535.png" width="680"></div>
<br/>

2.	在‘最近项目’区域选择一个```.rtkprj```文件进行打开。 如果最近项目中有所需的项目，则会弹出一个消息窗口。

<div id="figure_5_7" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636512948372776/7f7820d5_12407535.png" width="400"></div>
<br/>

### 如何打开/关闭项目
点击菜单栏上的 ‘文件 -> 起始页’。


### 如何导出/打包项目
<div id="figure_5_8" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636632023023311/9f459245_12407535.png" width="680"></div>
<br/>

点击菜单栏中的 "导出"，导出结果会显示在下图中。
<div id="figure_5_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636703955653703/deacfb05_12407535.png" width="400"></div>
<br/>

### 如何进行模拟
<div id="figure_5_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636770252167619/8cfcafa3_12407535.png" width="680"></div>
<br/>

点击菜单栏上的 ‘模拟’ 按钮。

<div STYLE="page-break-after: always;"></div> 

<a id="gui_demo_anchor"></a>

## GUI演示项目

```RVisualDesigner-vx.x.x.x.zip``` 中包含一个 Demo 项目。

文件夹 454x454 包含分辨率为 454*454 的项目。

文件夹 480x480 包含分辨率为 480*480 的项目。

<div id="figure_6_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636869409777562/bc385a52_12407535.png" width="580"></div>
<br/>

请按以下步骤运行项目：
1.	根据您的 IC 的屏幕尺寸/分辨率打开相应的项目；
2.	点击菜单栏中的‘编辑 -> 转换项目’，检查 IC 类型。有关详细信息，请参考 ‘转换项目’。 如果当前项目的 IC 类型与您的 IC 不匹配，请选择目标 IC 类型，输入目标分辨率，然后点击 ‘转换’。

<div id="figure_6_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721636942634993243/11b1e212_12407535.png" width="400"></div>
<br/>

3.	点击菜单栏中的 ‘导出’，等待导出结果弹出框的出现。

<div id="figure_6_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1721637000052375270/5e3f1873_12407535.png" width="400"></div>
<br/>

将导出的 ```.bin``` 文件烧录到您的 IC 中。