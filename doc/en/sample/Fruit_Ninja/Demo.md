# Fruit Ninja

This example demostrates how to develop a simple "Fruit Ninja APP", from which you can learn and understand the basic methods and processes of developing a ui application. Cut the fruits to gain points until you get to the bomb game over. Watch the demo video below to see its full functionality.

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1723621216978994553/76b62adc_10737458.gif" width = "450" /></div>
<br>

## Source File
To help learn and be familiar with the development, you can find all source files you may need in path sdk\src\app\gui\gui_engine\example\. The source file for this demostration is app_fruit_ninja_box2d.cpp, you can find it in the path mentioned for more details.

## Usage Steps

__Step 1:  Declare the app ui design function__
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

__Step 2:  Call function__
```c
extern void app_fruit_ninja_design(gui_obj_t *obj);
    app_fruit_ninja_design(GUI_APP_ROOT_SCREEN);
```	

## Design ideas & Notes
* In this app, box2d was used to create solids to simulate the movement of objects in a gravitational environment, given an initial velocity of the x-axis and y-axis during initialization. 
* The radius of the solid is set to a small value in order to minimize the effect of objects colliding with each other, since mutual collisions are detrimental to the gameplay. 
* The position and rotation angle of the fruits (and bomb) are updated in the callback function using the solid's center point mapping and displayed in the image component. 
* The position and initial velocity of the solid is reset when the position of the fruit is outside the display interface.
* Cutting fruit uses the structure touch_info, detecting the touch point release indicates the completion of a cut (to get the initial point of the touch screen and the displacement of the x-axis and y-axis), and the content of the cut is judged. 
* If there are two intersection points between the cut line and the picture rectangle, it means that the cut is valid. 
* Note that when calculating the intersection point, the rotated endpoint information of the picture needs to be brought into the calculation of the rotation angle to be consistent with the display, so that the accuracy of the cutting judgment can be improved. 
* Update the fruit picture to two pictures after cutting (corresponding to two gui_img_t pointers) and count the score. Multiple different objects can be cut in a single cut. 
* Note that a flag can be used to mark the cut status of the fruit to prevent scoring errors as well as to facilitate updating the position of the cut picture. 
* When the cut fruit moves outside the display it will reset the position and initial velocity of the solid and restore the cutting effect.


