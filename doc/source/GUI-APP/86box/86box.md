#  Demo: Develop a "86Box APP"
## Introduction
This example demostrates how to develop a SmartUI 86BOX APP, from which you can learn and understand the basic methods and processes of developing a ui application.


```eval_rst
.. raw:: html

    <iframe src="https://drive.google.com/file/d/1W79eozpryLyD1ffKUxHUPL2ZUGMDcSe4/preview" width="640" height="480" allow="autoplay"></iframe>
```

## Source File
- APP package ```gui_engine\example\screen_480_480\root\app\box```
- Design project ```RVisualDesigner-v1.0.5.0\Demo\480x480\box\box480x480.rtkprj```

## UI design
### RVisualDesigner
- Use RVisualDesigner to design the UI display, you can get detailed usage instructions from ```RVisualDesigner-v1.0.5.0\RTKIOT Visual Designer User Guide EN.pdf```
- Open an already completed project
  ![Image description](https://foruda.gitee.com/images/1701067827418608218/7c54ffbf_10088396.png "ss")
- Click Export and Simulate to use this UI.
  ![Image description](https://foruda.gitee.com/images/1701068361132692754/85acb896_10088396.png "ss")
  ![Image description](https://foruda.gitee.com/images/1701068989928620648/71684794_10088396.png "ss")
- What you see is what you get
  ![Image description](https://foruda.gitee.com/images/1701069129985417739/5558648f_10088396.png "ss")
- This tool requires adding pictures in advance, and then dragging the widgets in the ```ToolBox``` to the middle screen to lay out the same UI as the current ```Widget tree```.
## Javascript
- Non-default control effects in the current version require JavaScript coding to implement, please look forward to subsequent updates.
- Please refer to the page ```JavaScript syntax``` for specific syntax.

### Gestures
#### Light control switch
![Image description](https://foruda.gitee.com/images/1701075114325492203/ca9769d1_10088396.png "ss")
- On line 192 ~ 194 of the file ```gui_engine\example\screen_480_480\root\app\box\box.js```, it registers functions that will be triggered by the turning on and turning off gestures of a switch widget called ```living_switch```.
```javascript
sw.getElementById('living_switch')
sw.onOn(led1OnFunc)
sw.onOff(led1OffFunc)
```
- The function triggered by turning on gesture is ```led1OnFunc```. 
- Define an instance of GPIO with an index value of 0 and a direction of output. 
- Use this GPIO to write a 0 value in the function to turn off the light.
```javascript
var LED1 = new Gpio(0, 'out');
function led1OnFunc(params) {
    if (sleep_flag) {
        LED1.writeSync(0)
    }   
}
```
#### Tab jumping switch
```eval_rst
.. raw:: html

  <iframe src="https://drive.google.com/file/d/15Y4EEu80GzwjadqK78vbQ3xBi2vgxf-k/preview" width="640" height="480" allow="autoplay"></iframe>
```

- On line 425 ~ 431 of the file ```gui_engine\example\screen_480_480\root\app\box\box.js```, it registers a function that will be triggered by the press gesture of a button widget called ```icon123```.
- In the function ```icon123OnPressFunc```, tab jumps to index 0.
```javascript
tab.getElementById('tabview0')
icon.getElementById('icon123')
function icon123OnPressFunc(params) {
  //  console.log('iconNromalOnPressFunc')
    tab.jump(0);
}
icon.onPress(icon123OnPressFunc)
```




