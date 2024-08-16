# 水果忍者应用程序

本示例演示了如何开发一个简单的"水果忍者"APP，您可以从中学习和了解开发ui 应用程序的基本方法和流程。切割水果获得分数，直到切到炸弹游戏结束，观看下面的演示视频，了解其全部功能。

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723621216978994553/76b62adc_10737458.gif" width = "450" /></div>
<br>

## 源文件
为了帮助您学习和熟悉开发，您可以在 sdk\src\app\gui\gui_engine\example 路径下找到您可能需要的所有源文件。本演示的源文件是 app_fruit_ninja_box2d.cpp，您可以在上述路径中找到它，了解更多详情。

## 调用步骤

__步骤 1:  Declare the app ui design function__
```c
/** 
 * @brief start Fruit_Ninja APP by creating window,
 *        set the animation function of the window 
 *        and initialize some variables.
 * @param obj the father widget the APP's window nested in.
 */
void fruit_ninja_design(gui_obj_t *obj)

void app_fruit_ninja_design(gui_obj_t *obj)
{
    app_fruit_ninja::fruit_ninja_design(obj);
}
```

__步骤 2:  Call function__
```c
extern void app_fruit_ninja_design(gui_obj_t *obj);
app_fruit_ninja_design(GUI_APP_ROOT_SCREEN);
```	

## 设计思路及注意事项
* 在该app中，使用到了box2d创建固体模拟在重力环境中的物体运动，在初始化时给定一个x轴与y轴的初速度。
* 因为相互碰撞不利于游戏的游玩，为了减小物体间相互碰撞的影响，将固体的半径设置为一个较小的值。
* 在回调函数中利用固体的中心点映射更新水果（及炸弹）的位置与旋转角度并用图片组件显示。
* 在水果位置在显示界面之外会对固体的位置与初速度进行复位。
* 切割水果使用了touch_info这个结构体，检测到触控点释放说明完成了一次切割（得到触屏初始点与x轴与y轴的位移），对切割内容进行判断。
* 若切割线与图片矩形有两个交点，则说明切割有效。
* 注意在计算交点时图片的旋转后端点信息需要将旋转角度带入计算才会与显示一致，如此可以提高切割判断准确度。
* 将水果图片更新为切割后的两张图（对应两个gui_img_t指针），并计分，一次切割可以切到多个不同物体。
* 注意可以使用flag标记水果的切割状况，防止计分错误以及方便更新切割后的图片位置。
* 当切割后的水果移动到显示界面之外会对固体的位置与初速度进行复位，并将切割效果复原。


