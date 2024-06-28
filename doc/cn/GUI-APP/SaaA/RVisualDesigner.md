## 概述
RTKIOT可视化设计工具是一个用于为Realtek系列IC制作图形界面设计的工具，目前支持的IC列表如 [表 1-1](#table_1_1) 所示。

RTKIOT 可视化设计工具支持以下功能：
+ 从工具箱中拖拽控件并将其放置在设计视图中。
+ 拖放控件以在设计视图中更改其位置，或通过属性视图修改控件的位置和外观。
+ 导出用户设计的GUI项目为.bin文件，将.bin文件烧录到IC上以显示图形界面。
+ 在PC上模拟GUI项目。

<div id = "table_1_1" style="text-align:center;"><b>表 1-1 支持的IC列表</b></div>

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
		<th width="100">序号</th>
		<th width="200">支持的IC</th>
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

本文档主要包括以下内容：
+ 功能面板介绍
+ 资源管理介绍
+ 菜单栏介绍
+ 快速入门教程
+ GUI演示项目

为了简化文档，下文中使用“工具”来指代“RTKIOT可视化设计工具”。


<div STYLE="page-break-after: always;"></div> 


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

本节列出了小部件支持的属性，并用"Y"或"N"标记是否IC支持该属性。
</br>

#### 非容器化控件
##### 文本 (Text)
仅用于文本显示，不支持用户输入。属性如 [表 2-1](#table_2_1) 所示。

<div id = "table_2_1" style="text-align:center;"><b>表 2-1 文本控件属性</b></div>

<table align="center">
    <tr>
		<th width="130">属性</th>
		<th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
		<td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
		<td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
		<td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>相对于父控件的水平坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>相对于父控件的垂直坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>文本</I></b></td>
		<td>显示文本</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>显示文本</I></b></td>
		<td>长文本（超出控件范围的文本内容）</br>
        的显示模式可使用以下支持的类型：</br>
        截断显示模式（truncate）：截断文本显示模式</br>
        垂直滚动显示模式（verticalscroll）：垂直滚动文本显示模式</br>
        水平滚动显示模式（horizontalscroll）：水平滚动文本显示模式。
        </td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
        <td><b><I>字体</I></b></td>
		<td>字体设置请参考
        <a href="#font_setting_anchor">字体设置</a>.
        </td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>字体颜色 (RGBA)</I></b></td>
		<td>字体颜色设置，使用RGBA</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>
</br>

##### 按钮 (Button)
可点击的控件，具有文本和背景图片。属性如 [表 2-2](#table_2_2) 所示。

<div id = "table_2_2" style="text-align:center;"><b>表 2-2 按钮属性</b></div>

<table align="center">
    <tr>
		<th width="130">属性</th>
		<th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
		<td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
		<td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
		<td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>相对于父控件的水平坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>相对于父控件的垂直坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>文本</I></b></td>
		<td>显示文本</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>文本 X</I></b></td>
		<td>相对于按钮控件的水平坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
        <td><b><I>文本 Y</I></b></td>
		<td>相对于按钮控件的垂直坐标</a>
        </td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>显示模式</I></b></td>
		<td>水平或垂直显示</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
        <td><b><I>字体</I></b></td>
		<td>字体设置请参考
        <a href="#font_setting_anchor">字体设置</a>
        </td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>文本颜色 (RGB)</I></b></td>
		<td>文本颜色设置，使用RGB</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
        <td><b><I>转换</I></b></td>
		<td>图像转换模式有以下选项：</br>
        normal: 无效果</br>
        fade: 淡入/淡出</br>
        scale: 缩放</br>
        fadeScale: 淡入/淡出和缩放</br>
        注意：只有在设置了默认和高亮背景图片时，变换模式才会生效，否则默认为normal。
        </td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
        <td><b><I>BG 图像 (默认)</b></I></td>
        <td>默认的背景图像</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
        <td><b><I>BG 图像</br>
        (高亮)</b></I></td>
        <td>选定/高亮显示的背景图像</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
        <td><b><I>BG 图像</br>
        旋转角度</b></I></td>
        <td>背景图像旋转角度，范围：0~360度</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
    </tr>
</table>
</br>

##### 图像 (Image)
能够设置图像的控件，其属性如 [表 2-3](#table_2_3) 所示。

<div id = "table_2_3" style="text-align:center;"><b>表 2-3 图像属性</b></div>

<table align="center">
    <tr>
		<th width="130">属性</th>
		<th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
		<td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
		<td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
		<td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>相对于父控件的水平坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>相对于父控件的垂直坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>图像</b></I></td>
	    <td>图像路径</br>
        注意：图像必须预先导入到项目中。详细请参考
        <a href="#img_res_m_anchor">图像资源管理</a>。
        </td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b><I>图像旋转角度</b></I></td>
	    <td>图像旋转角度</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>图像比例 X</b></I></td>
	    <td>图像水平缩放程度，是一个倍数/百分比。</br>
        例如，设置比例x 为0.5表示图像的实际显示宽度是原始图像宽度的一半。
        </td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b><I>图像比例 Y</b></I></td>
	    <td>图像垂直缩放程度，是一个倍数/百分比</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
</table>

**注意:**
1.	在导出时，工具将转换导入的图像。可以在“菜单栏 -> 设置 -> 图像转换设置”中设置图像转换参数，详细请参考 <a href="#img_conv_setting_anchor">图像转换设置</a>。
2.	如果导入的图像大小与控件的大小不匹配，工具不会对图像进行缩放或裁剪。

</br>

##### 滑动条 (SeekBar)
滑动控件，可以响应用户滑动手势，并改变进度值。其属性如 [表 2-4](#table_2_4) 所示。

<div id = "table_2_4" style="text-align:center;"><b>表 2-4 滑动条属性</b></div>

<table align="center">
	<tr>
		<th width="130">属性</th>
		<th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
		<td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
		<td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
		<td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>相对于父控件的水平坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>相对于父控件的垂直坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
	    <td><b><I>颜色 (高亮) (RGBA)</b></I></td>
	    <td>进度条完成部分的背景色，请参考
        <a href="#figure_2_1">图 2-1</a>.
        </td>
	    <td style="text-align: center">N</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">N</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>颜色 (RGBA)</b></I></td>
	    <td>整个进度条的背景色，请参考
        <a href="#figure_2_1">图 2-1</a>.
        </td>
	    <td style="text-align: center">N</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">N</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b><I>方向</b></I></td>
	    <td>控件显示方向和手势响应方向的类型如下：</br>
        vertical/V: 垂直方向</br>
        arc: 曲线的方向</br>
        horizontal/H: 水平方向</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
</table>

<div id="figure_2_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817870508689816/f43215ff_12407535.png" style="width: 400px"></div>
<div style="text-align:center;"><b>图 2-1 Seekbar</b></div>
</br>

##### 图像滑动条 (Image SeekBar)
具有多个图像作为背景的滑动控件，用户滑动时可以切换到不同的图像，其属性如表 [表 2-5](#table_2_5) 所示。

<div id = "table_2_5" style="text-align:center;"><b>表 2-5 图像滑动条属性</b></div>

<table align="center">
	<tr>
		<th width="130">属性</th>
		<th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
		<td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
		<td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
		<td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>相对于父控件的水平坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>相对于父控件的垂直坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
	    <td><b><I>角度 (起始)</b></I></td>
	    <td>起始角度（如果方向是曲线则无效）</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>角度 (结束)</b></I></td>
	    <td>结束角度（如果方向是曲线则无效）</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b><I>图像目录</b></I></td>
	    <td>包含要在此控件上显示的图像的文件夹</br>
        注意： </br>
        1.	请按名称对图像进行排序；</br>
        2.	当用户在控件上滑动时，控件将根据当前进度切换背景图像。
        </td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>中心 X</b></I></td>
	    <td>弧的中心相对于父控件的水平坐标</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b>中心 Y<I></b></I></td>
	    <td>弧的中心相对于父控件的垂直坐标</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>方向</b></I></td>
	    <td>控件显示方向和手势响应方向的类型如下：</br>
        vertical/V: 垂直方向</br>
        arc: 曲线的方向</br>
        horizontal/H: 水平方向</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
</table>
</br>

##### 开关 (Switch)
具有“已选中”和“未选中”状态的开关控件，其属性如 [表 2-6](#table_2_6) 所示。

<div id = "table_2_6" style="text-align:center;"><b>表 2-6 开关属性</b></div>

<table align="center">
	<tr>
		<th width="130">属性</th>
		<th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
        <td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
        <td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
        <td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="font-size: 14px;">
        <td><b><I>X</I></b></td>
        <td>相对于父控件的水平坐标</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
        <td><b><I>Y</I></b></td>
        <td>相对于父控件的垂直坐标</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
        <td style="text-align: center">Y</td>
    </tr>
    <tr style="font-size: 14px;">
	    <td><b><I>BG 图像 (已选中)</b></I></td>
	    <td>已选中状态的背景图像</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
	    <td><b><I>BG 图像 (未选中)</b></I></td>
	    <td>未选中状态的背景图像</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
	    <td style="text-align: center">Y</td>
    </tr>
</table>
</br>

##### 弧形 (Arc)
弧形控件，暂时不支持手势，其属性如 [表 2-7](#table_2_7) 所示。

<div id = "table_2_7" style="text-align:center;"><b>表 2-7 弧形属性</b></div>

<table align="center">
	<tr>
		<th width="130">属性</th>
        <th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
		<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
        <td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
        <td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
        <td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>相对于父控件的水平坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>相对于父控件的垂直坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>中心 X</b></I></td>
	    <td>弧的中心相对于父控件的水平坐标</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b>中心 Y<I></b></I></td>
	    <td>弧的中心相对于父控件的垂直坐标</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
        <td><b><I>BG 颜色</I></b></td>
		<td>弧形背景颜色</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
    </tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>末端样式</I></b></td>
		<td>弧度末端样式，支持以下选项：</br>
        圆形/平头/方形，请参考
        <a href="#figure_2_2">图 2-2</a>
        </td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>度数 (结束)</b></I></td>
		<td>弧形的结束度数</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
    </tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>度数 (开始)</b></I></td>
		<td>弧形的开始度数</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>半径</b></I></td>
		<td>弧形的半径</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
    </tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>描边宽度</b></I></td>
		<td>弧形的描边宽度</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">N</td>
	</tr>
</table>

<div id = "figure_2_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817869829442326/90524d4c_12407535.png"/></div>
<div style="text-align:center;"><b>图 2-2 末端样式</b></div>
</br>


#### 容器控件
##### 屏幕 (Screen)
屏幕控件，对应于物理屏幕，是GUI项目的根控件，其属性显示在 [表 2-8](#table_2_8) 中。

**注意：**
只能修改“名称”属性。

<div id = "table_2_8" style="text-align:center;"><b>表 2-8 屏幕属性</b></div>

<table align="center">
	<tr>
		<th width="130">属性</th>
        <th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
        <td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
        <td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
        <td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>水平坐标，始终为0</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>垂直坐标，始终为0</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>
</br>

##### 选项卡视图 (TabView) 和选项卡 (Tab)
选项卡视图控件作为父控件，支持上下左右滑动来在选项卡之间切换。选项卡视图控件具有以下属性，如 [表 2-9](#table_2_9) 所示，选项卡控件具有以下属性，如 [表 2-10](#table_2_10) 所示。

<div id = "table_2_9" style="text-align:center;"><b>表 2-9 选项卡视图属性</b></div>

<table align="center">
	<tr>
		<th width="130">属性</th>
        <th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
        <td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
        <td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
        <td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>相对于父控件的水平坐标，始终为0</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>相对于父控件的垂直坐标，始终为0</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Transition</b></I></td>
		<td>选项卡切换模式支持以下类型：</br>
        normal: 无特效</br>
        fade: 淡入/淡出</br>
        scale:  缩放</br>
        fadeScale: 淡入/淡出和缩放
		</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>

<div id = "table_2_10" style="text-align:center;"><b>表 2-10 选项卡属性</b></div>

<table align="center">
	<tr>
		<th width="130">属性</th>
        <th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
        <td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
        <td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
        <td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>相对于父控件的水平坐标，始终为0</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>相对于父控件的垂直坐标，始终为0</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>索引 (X轴)</b></I></td>
		<td>选项卡视图中选项卡的水平索引，请参见
		<a href="#figure_2_3">图 2-3</a>.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>索引 (Y轴)</b></I></td>
		<td>选项卡视图中选项卡的垂直索引，请参见
		<a href="#figure_2_3">如 2-3</a>.</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>
	
<div id = "figure_2_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817871000642675/ca6cda22_12407535.png" width="300"/></div>
	
<center><div><b>图 2-3 选项卡索引</b></div></center>

**注意：**
1.	选项卡视图的宽度和高度不能被修改，会默认设置为屏幕的宽度和高度。
2.	选项卡视图的水平和垂直坐标不能被修改，始终为0。
3.	选项卡视图只能作为屏幕控件的子控件使用。
4.	选项卡视图的子控件只能是选项卡。
5.	选项卡的宽度和高度不能被修改，会默认设置为选项卡视图的宽度和高度。
6.	选项卡的水平和垂直坐标不能被修改，始终为0。

</br>

##### 页面 (Page)
具有可滚动内容的容器控件。

**注意：**
1.	页面只支持垂直滚动。
2.	页面控件的宽度和高度只定义了对滑动手势响应的界面区域，是否允许滚动取决于是否将其添加到超出屏幕范围的子控件中。

<div id = "table_2_11" style="text-align:center;"><b>表 2-11 页面属性</b></div>

<table align="center">
	<tr>
		<th width="130">属性</th>
        <th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
        <td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
        <td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
    <tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
        <td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>相对于父控件的水平坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>相对于父控件的垂直坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>
</br>


##### 窗口 (Win)
在窗口的宽度和高度定义的区域内，可以响应各种手势，包括点击、长按、按下、释放和滑动，其属性如 [表 2-12](#table_2_12) 所示。

<div id = "table_2_12" style="text-align:center;"><b>表 2-12 窗口属性</b></div>

<table align="center">
	<tr>
		<th width="130">属性</th>
        <th width="350" style="text-align: center;">描述</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>名称</I></b></td>
        <td>控件名称</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>大小 (高度)</I></b></td>
        <td>控件高度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>大小 (宽度)</I></b></td>
        <td>控件宽度</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>X</I></b></td>
		<td>相对于父控件的水平坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Y</I></b></td>
		<td>相对于父控件的垂直坐标</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>隐藏</b></I></td>
		<td>表示是否隐藏窗口及其子控件</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>
</br>


### 设计视图/画布
用户可以从工具箱面板中拖放控件到设计视图中，调整控件的布局并设置属性，以设计一个可以在瑞昱 IC中渲染的图形界面。

<img src="https://foruda.gitee.com/images/1710817870929335945/6991cdc9_12407535.png">
<div id = "figure_2_4" style="text-align:center;"><b>图 2-4 设计视图</b></div>
</br>

#### 选项卡视图 - 创建/删除/插入选项卡
从工具箱中拖放选项卡视图控件到设计视图中，创建出一个只包含一个主页选项卡（坐标（0,0））的选项卡视图，如 [图 2-5](#figure_2_5) 所示。


<img src="https://foruda.gitee.com/images/1710817870419593506/9ab927b9_12407535.png">
<div id = "figure_2_5" style="text-align:center;"><b>图 2-5 创建选项卡视图</b></div>
</br>


##### 创建选项卡
可以通过点击设计视图周围的按钮来创建新的选项卡。

**注意：**
1.	如果idx为0，则上下按钮可用。
2.	如果idy为0，则左右按钮可用。


###### 删除选项卡
选择要删除的选项卡，在菜单栏上单击“编辑->删除”或按键盘上的Delete键，然后再次确认是否要删除它。

<div id = "figure_2_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817870328087578/e0915700_12407535.png"></div>
<div style="text-align:center;"><b>图 2-6  删除选项卡再次确认</b></div>
</br>

##### 插入选项卡
目前，选项卡的插入只支持通过修改现有选项卡的坐标并创建新选项卡来实现。

例如，需要在坐标（1,0）和（2,0）的选项卡之间插入一个选项卡，步骤如下。
1.	将选项卡（2,0）及其右侧的选项卡的idx加一，如 [图 2-7 (b)](#figure_2_7_b) 所示。
2.	切换到选项卡（1,0），点击 创建新的选项卡（2,0）。

<div id="figure_2_7_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817875276200848/d238bf5a_12407535.png"></div>
<div style="text-align:center;">a. 选项卡插入位置</div>
<div id="figure_2_7_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710817875726752431/e704d18a_12407535.png"></div>
<div style="text-align:center;">b. 修改已存在的选项卡坐标</div>
<div id="figure_2_7_c"><img src="https://foruda.gitee.com/images/1710818987429423477/280e6384_12407535.png"></div>
<div style="text-align:center;">c. 创建选项卡</div>
<div style="text-align:center;"><b>图 2-7 插入选项卡</b></div>
</br>


#### 选项卡视图概览图
请点击 ![icon](https://foruda.gitee.com/images/1710818993805990437/796af0dc_12407535.png)  查看选项卡视图概览图。

**注意：**
1.	在概览图中，高亮显示的选项卡表示当前在设计视图中正在编辑的选项卡。
2.	概览图使用选项卡的坐标进行标注。当在IC中进行模拟或渲染时，坐标为（0, 0）的选项卡显示在主页上，用户可以上下左右滑动以显示其他选项卡。

<div id="figure_2_8_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818988334796749/309f47b2_12407535.png"></div>
<div id="figure_2_8_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818988855366691/0a431fa4_12407535.png"></div>
<div style="text-align:center;"><b>图 2-8 选项卡视图概览图</b></div>
</br>


#### 设计视图的缩放
有三种方式可以对设计视图进行缩放。
1.	按住Ctrl键，并滚动鼠标滚轮。
2.	点击“-”和“+”按钮。
3.	拖动滑动条。

<div id="figure_2_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818993215663271/c0d56791_12407535.png"></div>
<div style="text-align:center;"><b>图 2-9 设计视图的缩放</b></div>
</br>


### 属性视图
在控件树或设计视图中选择一个控件，可以显示出所有控件的属性值，用户可以根据需要进行修改，如 [图 2-10](#figure_2_10) 所示。

<div id="figure_2_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818988161964594/40fa2436_12407535.png"></div>
<div style="text-align:center;"><b>图 2-10 属性视图</b></div>
</br>


### 控件树
控件树用于向用户展示当前布局中控件之间的父子关系和兄弟关系。我们遵循以下约定：
1.	子控件位于父控件的上方，即当父控件和子控件重叠时，子控件会覆盖父控件。
2.	兄弟控件的图层顺序与添加顺序相关，先添加的控件在底部，后添加的控件在顶部。

[图 2-11](#figure_2_11) 展示了主页选项卡和灯选项卡中的所有子控件，其中主页选项卡只有一个背景图片控件，而灯选项卡包含一个图片控件和多个开关控件。

<div id="figure_2_11" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818987846178523/1b27419f_12407535.png"></div>
<div style="text-align:center;">a. 主页选项卡</div>
<div id="figure_2_11_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710818987413289439/46a6c801_12407535.png"></div>
<div style="text-align:center;">b. 灯选项卡</div>
<div style="text-align:center;"><b>图 2-11 控件树视图</b></div>
</br>

控件树支持以下操作：
1.	选择控件：如果在控件树中选择一个控件，设计视图中对应的控件会聚焦，并在属性视图中显示其属性。
2.	修改父子关系：在控件树中选择一个控件（除了选项卡/选项卡视图/屏幕），并将其拖放到目标控件项上，该控件将成为目标控件的子控件。
3.	修改控件图层：在控件树中选择一个控件（除了选项卡/选项卡视图/屏幕），将其拖放到目标控件项的上方或下方，在设计视图中，该控件将在目标控件的上方或下方。
4.	锁定控件：点击锁定按钮，锁定一个或多个控件，如 [图 2-12](#figure_2_12) 所示。
    + 如果点击屏幕的锁定按钮，所有屏幕的子控件将被锁定，用户将无法在设计视图中拖动或调整控件大小。
    + 如果点击选项卡的锁定按钮，所有选项卡的子控件将被锁定，用户将无法在设计视图中拖动或调整控件大小。

<div id="figure_2_12" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824254850042256/002159e9_12407535.png"></div>
<div style="text-align:center;">a. 未锁定</div>
<div id="figure_2_12_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824254887179488/09fabec5_12407535.png"></div>
<div style="text-align:center;">b. 已锁定</div>
<div style="text-align:center;"><b>图 2-12 锁定控件</b></div>
</br>


<div STYLE="page-break-after: always;"></div> 


## 资源管理
只有预导入的图像和字体文件可以被GUI项目引用。本章重点介绍如何管理图像和字体资源。
图像和字体资源管理器位于设计视图正下方，如 [图 3-1](#figure_3_1) 所示。

<div id="figure_3_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824260408871745/edfdf9bd_12407535.png"></div>
<div style="text-align:center;"><b>图 3-1 图像和字体资源管理</b></div>
</br>


### <div id="img_res_m_anchor">图像资源管理</div>
点击 ![icon](https://foruda.gitee.com/images/1710824254768634573/d752fbd6_12407535.png)  打开图像管理器，如 [图 3-2](#figure_3_2) 所示。

<div id="figure_3_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824254901030282/9705cea5_12407535.png" width="300"></div>
<div style="text-align:center;"><b>图 3-2 图像资源管理窗口</b></div>
</br>


##### 添加图像
通过以下步骤可以将图像添加到GUI项目中。
1.	点击 ，创建一个新的图像文件夹并输入文件夹名称。创建的文件夹位于GUI项目目录下的Resouce\image文件夹中。

<div id="figure_3_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824260010283421/eb1636f0_12407535.png" width="300"></div>
<div style="text-align:center;"><b>图 3-3 创建图像文件夹</b></div>

2.	选择创建的图像文件夹，然后点击 ![icon](https://foruda.gitee.com/images/1710824254769021374/55b6c303_12407535.png) 选择图像（支持多选），将它们添加到文件夹中。添加完成后，如 [图 3-4](#figure_3_4)所示，图像会被复制到Resource\image\home文件夹中。

<div id="figure_3_4_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824488209842923/05efbb41_12407535.png" width="300"></div>
<div style="text-align:center;">a. 选择图像文件夹</div>
<div id="figure_3_4_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824483196685183/7e19707a_12407535.png" width="500"></div>
<div style="text-align:center;">b. 选择需要添加的图像</div>
<div id="figure_3_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824488369529087/4940a684_12407535.png" width="300"></div>
<div style="text-align:center;">c. 添加图像</div>
<div style="text-align:center;"><b>图 3-4 添加图像</b></div>
</br>


##### 移除图像/图像文件夹
选择要移除的图像或图像文件夹，然后点击 ![icon](https://foruda.gitee.com/images/1710824483012674071/c0c62b91_12407535.png)。
</br>


##### 重命名图像文件夹
选择图像文件夹，双击并输入一个新名称。
</br>


#### 预览图像
选择图像文件夹，右侧区域将显示该文件夹中的所有图像。

<div id="figure_3_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824483098291044/697fd630_12407535.png"></div>
<div style="text-align:center;"><b>图 3-5 预览图像</b></div>
</br>


#### 刷新
如果用户在本地操作图像资源而不是通过工具进行操作，可以点击   刷新。

**注意：**
不推荐的做法。
</br>


### 字体资源管理
请参考 [图 3-6](#figure_3_6)。
</br>


#### 添加第三方字体
如果需要使用第三方字体（.ttf），请先点击 ![icon](https://foruda.gitee.com/images/1710824482944711702/8b73005d_12407535.png) 导入资源，否则将使用本地安装的字体。

<div id="figure_3_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824483191984010/283b1f70_12407535.png" width="500"></div>
<div style="text-align:center;"><b>图 3-6 字体管理</b></div>
</br>


#### 移除第三方字体
选择要移除的字体，然后点击  ![icon](https://foruda.gitee.com/images/1710824483012674071/c0c62b91_12407535.png)。
</br>


<div STYLE="page-break-after: always;"></div> 


## 菜单栏
### 文件
#### 起始页
如果要关闭当前项目并打开一个已存在的项目或新建一个项目，可以通过点击“文件 -> 起始页”来打开起始页。
点击“打开项目”或选择一个.rtkprj文件并双击打开已存在的项目，或者点击“创建项目”来创建新项目，请参考 [如何创建项目](#how_to_create_anchor) 和 [如何打开项目](#how_to_open_anchor)。


<div id="figure_4_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824561456305634/8d4384a4_12407535.png"></div>
<div style="text-align:center;"><b>图 4-1 文件 - 起始页</b></div>
</br>


#### 保存
保存项目的所有UI更改，快捷键是Ctrl + S。
</br>


##### 退出保存
当关闭项目时会弹出提示窗口，如下所示。请点击“确定”进行保存，否则更改将会丢失。

<div id="figure_4_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555938151074/9cb2be23_12407535.png" width="260"></div>
<div style="text-align:center;"><b>图 4-2 关闭并保存项目</b></div>
</br>


### 编辑
#### 复制/粘贴
1.	点击“编辑 -> 复制”以复制所选控件，快捷键是Ctrl + C。
2.	点击“编辑 -> 粘贴”以在设计视图中创建所选控件的副本，快捷键是Ctrl + V。
</br>


#### 删除
点击“编辑 -> 删除”以删除所选控件，或按下键盘上的“Delete”键。
</br>


#### 撤销/重做
撤销：撤销对控件所做的更改，快捷键是Ctrl + Z。
重做：重新对控件进行更改，快捷键是Ctrl + Y。
</br>


#### 转换项目
转换项目窗口用于转换当前项目的IC类型和屏幕尺寸/分辨率。

<div id="figure_4_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555929917469/fcbb0ff3_12407535.png" width="300"></div>
<div style="text-align:center;"><b>图 4-3 转换项目窗口</b></div>
</br>


#### 修改项目名称
修改项目名称窗口用于修改当前项目的名称。请在输入框中输入新名称。
 
<div id="figure_4_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555934977455/0a13f4fd_12407535.png" width="300"></div>
<div style="text-align:center;"><b>图 4-4 修改项目名称窗口</b></div>
</br>


### 设置

#### <div id="img_conv_setting_anchor">图像转换设置</div>
图像必须转换才能在IC上正确显示，因此用户需要设置正确的转换参数。所有可选参数如 [图 4-5](#figure_4_5) 所示。

<div id="figure_4_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555965501168/ec76c1b1_12407535.png" width="260"></div>
<div style="text-align:center;"><b>图 4-5 图像转换窗口</b></div>

参数描述如下。
</br>


##### 扫描模式
可用的选项如 [表 4-1](#table_4_1) 所示。

<div id="table_4_1" style="text-align:center;"><b>表 4-1 扫描模式选项</b></div>
<table align="center">
	<tr>
		<th width="130">扫描模式</th>
		<th width="600" style="text-align: center;">描述</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>水平</I></b></td>
		<td>水平扫描</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>垂直</I></b></td>
		<td>垂直扫描</td>
	</tr>
</table>
</br>

##### 颜色空间
图像的颜色空间，可用的选项如 [表 4-2](#table_4_2) 所示。

<div id="table_4_2" style="text-align:center;"><b>表 4-2 颜色空间选项</b></div>
<table align="center">
	<tr>
		<th width="130">颜色空间</th>
		<th width="600" style="text-align: center;">描述</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>RGB565</I></b></td>
		<td>16位RGB模式</br>
        位 4:0 表示蓝色；位 10:5 表示绿色；位 15:11 表示红色。
        </td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>RTKARGB</I></b></td>
		<td>16位ARGB模式</br>
        位 4:0 表示蓝色；位 9:5 表示绿色；位 14:10 表示红色; 位 15 表示透明度。
		</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>RTKRGAB</I></b></td>
		<td>16位RGAB模式</br>
        位 4:0 表示蓝色；位 5 表示透明度；位 10:6 表示绿色；位 15:11 表示红色。
		</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>RGB</I></b></td>
		<td>24 位 RGB mode</br>
        位 7:0 表示蓝色；位 15:8 表示绿色；位 23:16 表示红色。
		</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>RGBA</I></b></td>
		<td>32 位 RGBA mode</br>
        位 7:0 表示蓝色；位 15:8 表示绿色；位 23:16 表示红色; 位 31:24 表示透明度。
        </td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>二进制</I></b></td>
		<td>2-值 (0 或 1) 图像</td>
	</tr>
</table>
</br>


##### 压缩
如果勾选“压缩”，请根据需要设置压缩参数。
可选的压缩模式如下：
1.	行程长度编码（Run-Length Encoding），一种无损压缩算法。 如果选择RLE作为压缩模式，则需要设置RLE级别和RLE行程长度参数。

<div id="figure_4_6_a" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555889664193/c33b99ad_12407535.png" width="260"></div>
<div style="text-align:center;">a. RLE 级别 - Level1</div>
<div id="figure_4_6_b" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824561230699040/f2dbfb4a_12407535.png" width="260"></div>
<div style="text-align:center;">b.	RLE 级别 - Level2</div>
<div style="text-align:center;"><b>图 4-6 RLE 参数</b></div>
</br>

<div id="table_4_3" style="text-align:center;"><b>表 4-3 RLE 级别</b></div>
<table class="center">
	<tr>
		<th width="150">RLE 级别</th>
		<th width="600" style="text-align: center;">描述</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Level1</I></b></td>
		<td>1级压缩</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Level2</I></b></td>
		<td>2级压缩，基于1级压缩的次级压缩</td>
	</tr>
</table>

<div id="table_4_4" style="text-align:center;"><b>表 4-4 RLE 行程长度</b></div>
<table align="center">
	<tr>
		<th width="150">RLE 行程长度</th>
		<th width="580" style="text-align: center;">描述</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>Byte_1</I></b></td>
		<td>1个字节，最大为255</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>Byte_2</I></b></td>
		<td>2个字节，最大为255</td>
	</tr>
</table>

每个行程（Run）在压缩过程中允许的最大重复字符长度。

2.	FastLz: 
一种基于字典和滑动窗口的无损压缩算法，用于压缩具有大量重复值的数据。
3.	YUV_Sample_Blur: 
一种结合YUV采样和模糊处理的有损压缩算法。
YUV 采样：保留图像的亮度信息，并只对色度信息进行采样。
模糊处理：在YUV采样后丢弃每个字节的低位，以达到数据压缩的目的。


<div id="table_4_5" style="text-align:center;"><b>表 4-5 YUV 采样模式</b></div>
<table align="center">
	<tr>
		<th width="160">YUV 采样模式</th>
		<th width="570" style="text-align: center;">描述</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>YUV444</I></b></td>
		<td>采样4个像素数据到4个Y、4个U和4个V，即每个Y对应一组UV分量，完全保留YUV数据。</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>YUV422</I></b></td>
		<td>每4个像素数据采样为4个Y、2个U和2个V，即每2个Y对应一组UV分量，数据大小为原始数据的75%。</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>YUV411</I></b></td>
		<td>每4个像素数据采样为4个Y、1个U和1个V，即每4个Y对应一组UV分量，数据大小为原始数据的50%。</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>YUV422</I></b></td>
		<td>Y - 亮度； V - 色度</td>
	</tr>
</table>

<div id="table_4_6" style="text-align:center;"><b>表 4-6 模糊处理模式</b></div>
<table align="center">
	<tr>
		<th width="150">模糊处理模式</th>
		<th width="580" style="text-align: center;">描述</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>位0</I></b></td>
		<td>不丢弃低位的保存方式</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>位1</I></b></td>
		<td>每个字节丢弃位0 (保留 [位7:位1]).</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>位2</I></b></td>
		<td>每个字节丢弃 [位1:位0] (保留 [位7:位2]).</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>位4</I></b></td>
		<td>每个字节丢弃 [位3:位0] (保留 [位7:位4]).</td>
	</tr>
</table>

4.	YUV_Sample_Blur+FastLz: 该算法结合了YUV_Sample_Blur和FastLz算法。


#### <div id="font_setting_anchor">字体转换设置</div>
包括位图字体和矢量字体。支持Realtek系列IC的字体在 [表 4-7](#table_4_7) 中显示。

<u><b>注意：您至少需要创建一个字体转换设置，否则无法在属性视图中为文本类型控件选择字体。</b></u>

<div id="table_4_7" style="text-align:center;"><b>表 4-7 Realtek支持的字体</b></div>
<table align="center">
	<tr>
		<th width="130">字体</th>
		<th width="60">8762D/8763E</th>
		<th width="60">8762G/8772G</th>
		<th width="60">TBD</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>矢量</I></b></td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">N</td>
		<td style="text-align: center">Y</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>位图</I></b></td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
		<td style="text-align: center">Y</td>
	</tr>
</table>

要使用位图字体，您需要设置以下参数。

<div id="figure_4_7" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824555984723993/d86cef5d_12407535.png" width="260"></div>
<div style="text-align:center;"><b>图 4-7 位图字体的转换设置</b></div>

[表 4-8](#table_4_8) 列出了每个参数的描述。

<div id="table_4_8" style="text-align:center;"><b>表 4-8 字体转换参数</b></div>
<table align="center">
	<tr>
		<th width="150">参数</th>
		<th width="580" style="text-align: center;">描述</th>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>字体设置名称</I></b></td>
		<td>用户定义的字体设置名称，</br>
        请确保每次创建不同的字体设置名称。
		</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>字体大小</I></b></td>
		<td>字体大小</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>粗体</I></b></td>
		<td>是否为粗体</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>斜体</I></b></td>
		<td>是否为斜体</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>渲染模式</I></b></td>
		<td>用于表示在转换的.bin文件中像素的位数</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>扫描模式</I></b></td>
		<td>保存.bin时有两种扫描方式</br>
        H: 水平扫描</br>
        V: 垂直扫描
		</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>索引方法</I></b></td>
		<td>转换的.bin文件重新索引区域的索引方法</td>
	</tr>
	<tr style="font-size: 14px;">
		<td><b><I>代码页</I></b></td>
		<td>支持多个代码页</td>
	</tr>
	<tr style="background-color: #D9E2F3; font-size: 14px;">
		<td><b><I>文本类型</I></b></td>
		<td>有以下几种类型。</br>
        范围：如果文本的Unicode范围可以预先确定，请选择此类型，并在范围文本框中输入范围。支持多个范围，请将每个范围单独设置在一行中。</br>
        注意：只有范围内的字符将被转换并保存到.bin文件中，可以节省存储空间。</br>
        随机：如果文本的Unicode范围无法预先确定，请选择此类型。</br>
        注意：字体的所有字符都将被转换并保存到.bin文件中。
		</td>
	</tr>
</table>

有关矢量字体的参数，请参考 [图 4-8](#figure_4_8)。

<div id="figure_4_8" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633144604007/ae5d772b_12407535.png" width="300"></div>
<div style="text-align:center;"><b>图 4-8 矢量字体参数</b></div>


### 导出
如果您已经完成了GUI项目的设计，并且想将其烧录到IC中，请点击“导出”，然后工具将执行以下操作：
1.	图像转换
2.	字体转换
3.	将.xml、.js、图像和字体打包到输出的.bin文件中。

当上述操作完成后，会弹出一个消息框。

<div id="figure_4_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633121026710/337d046a_12407535.png" width="300"></div>
<div style="text-align:center;"><b>图 4-9 导出成功</b></div>

.bin文件可以烧录进您的IC中。


### 模拟
在UI上模拟项目。
注意：当您第一次模拟项目时，请先点击“导出”，然后再点击“模拟”。如果您没有修改任何图像或字体设置，则无需再次点击“导出”。
[图 4-10](#figure_4_10) 显示了运行中的模拟器。

<div id="figure_4_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828505193590425/a8c6e2c0_12407535.png" width="260"></div>
<div style="text-align:center;"><b>图 4-10 运行模拟器</b></div>


<div STYLE="page-break-after: always;"></div> 


## 快速开始教程
### <div id="how_to_create_anchor">如何创建项目</div>
<div id="figure_5_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633374956290/299fb9f5_12407535.png"></div>
<div style="text-align:center;"><b>图 5-1 开始页面</b></div>

双击运行"RVisualDesigner.exe"，然后按照步骤（1~4）配置项目，点击"创建项目"（5），如 [图 5-1](#figure_5_1) 所示。
创建项目后，GUI设计窗口将弹出，如 [图 5-2](#figure_5_2) 所示。左侧是组件区域，中间是设计区域，右侧是部件属性设置区域。

<div id="figure_5_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633484403086/7932a505_12407535.png"></div>
<div style="text-align:center;"><b>图 5-2 GUI设计窗口</b></div>

注意：新创建的项目文件位于解决方案文件夹下的项目文件夹中 ([图 5-1](#figure_5_1) 中的3)。 [图 5-3](#figure_5_3) 中展示了一个示例。

<div id="figure_5_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633124706596/fe5653b6_12407535.png" width="400"></div>
<div style="text-align:center;"><b>图 5-3 项目文件夹</b></div>

当在设计视图上拖放一个控件，并点击"文件 - 保存"或按下Ctrl + S时，将创建一个.rtkui文件，如 [Figure 5-4](#figure_5_4).

<div id="figure_5_4" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828526032663800/81e3057a_12407535.png" width="400"></div>
<div style="text-align:center;"><b>图 5-4 .rtkui文件</b></div>


### 如何编写 JavaScript 代码
项目创建完成后，会生成一个 xxx.js 文件，如 [图 5-4](#figure_5_4) 所示。打开该文件，在其中编写 JavaScript 代码以实现控件的事件回调函数。


### <div id="how_to_open_anchor">如何打开项目</div>
<div id="figure_5_5" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710824633354019948/7c6804dc_12407535.png" width="400"></div>
<div style="text-align:center;"><b>图 5-5 打开项目</b></div>

有两种打开项目的方式：
1.	点击 "打开项目"，选择一个 .rtkprj 文件进行打开。

<div id="figure_5_6" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828552787091298/4e1ccc92_12407535.png" width="400"></div>
<div style="text-align:center;"><b>图 5-6 选择 .rtkprj 文件打开项目</b></div>

2.	在 "最近项目" 区域选择一个 .rtkprj 文件进行打开。 如果最近项目中有所需的项目，则会弹出一个消息窗口。

<div id="figure_5_7" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828557068088923/e418bb99_12407535.png" width="300"></div>
<div style="text-align:center;"><b>图 5-7 消息窗口</b></div>


### 如何打开/关闭项目
点击菜单栏上的 "文件 -> 起始页"。


### 如何导出/打包项目
<div id="figure_5_8" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828565511074049/27af6150_12407535.png"></div>
<div style="text-align:center;"><b>图 5-8 导出</b></div>
<div id="figure_5_9" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828593609008235/ca49b9cd_12407535.png" width="300"></div>
<div style="text-align:center;"><b>图 5-9 导出成功</b></div>

点击菜单栏中的 "导出"，导出结果会显示在 [图 5-9](#figure_5_9)中。


### 如何进行模拟
<div id="figure_5_10" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828603947574032/f90b802b_12407535.png"></div>
<div style="text-align:center;"><b>图 5-10 模拟</b></div>

点击菜单栏上的 "模拟" 按钮。


<div STYLE="page-break-after: always;"></div> 


## GUI Demo 项目
RVisualDesigner-vx.x.x.x.zip 中包含一个 Demo 项目，如 [图 6-1](#figure_6_1) 所示。

文件夹 454x454 包含分辨率为 454*454 的项目。

文件夹 480x480 包含分辨率为 480*480 的项目。

<div id="figure_6_1" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828619648299243/6ddfa28a_12407535.png" width="400"></div>
<div style="text-align:center;"><b>图 6-1 Demo</b></div>

请按以下步骤运行项目：
1.	根据您的 IC 的屏幕尺寸/分辨率打开相应的项目。
2.	点击菜单栏中的 "编辑 -> 转换项目"，检查 IC 类型。有关详细信息，请参考 "转换项目"。 如果当前项目的 IC 类型与您的 IC 不匹配，请选择目标 IC 类型，输入目标分辨率，然后点击 "转换"。

<div id="figure_6_2" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828634079822366/790c6381_12407535.png" width="300"></div>
<div style="text-align:center;"><b>图 6-2 转换项目</b></div>

3.	点击菜单栏中的 "导出"，等待导出结果弹出框的出现。

<div id="figure_6_3" style="text-align:center;"><img src="https://foruda.gitee.com/images/1710828658401238872/d2f38d9e_12407535.jpeg" width="300"></div>
<div style="text-align:center;"><b>图 6-3 输出 .bin 文件</b></div>

将导出的 .bin 文件烧录到您的 IC 中。