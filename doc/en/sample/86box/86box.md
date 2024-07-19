#  86box Application

This example demostrates how to develop a SmartUI 86BOX APP, from which you can learn and understand the basic methods and processes of developing a ui application.

## Source File
- APP package `gui_engine\example\screen_480_480\root\app\box`
- UI Design project `RVisualDesigner-v1.0.5.0\Demo\480x480\box\box480x480.rtkprj`

## UI design
### RVisualDesigner
- SmartUI 86box utilizes `RVisualDesigner` to complete UI design. For the first-time usage of `RVisualDesigner`, please refer to `RVisualDesigner-v1.0.5.0\RTKIOT Visual Designer User Guide EN.pdf` to obtain a detailed development guide.

- Find and open the example UI design project in the specified path.
<br/>
<div style="text-align: center"><img width="600" src="https://foruda.gitee.com/images/1701067827418608218/7c54ffbf_10088396.png "></div>
<br/>

- Click on `Export` and then `Simulate` in succession to complete the export and launch the simulation. Once the simulator window is launched, the 86box APP icon will be displayed. Clicking on it will take you to the corresponding APP.

<br/>
<div style="text-align: center"><img img width="600" src="https://foruda.gitee.com/images/1701068361132692754/85acb896_10088396.png "></div>
<br/>
<br/>
<div style="text-align: center"><img img width="300" src="https://foruda.gitee.com/images/1701068989928620648/71684794_10088396.png "></div>
<br/>

- When entering the APP, you will see the same UI content as in the `RVisualDesigner` design project in the simulator window. Therefore, this design mode has the feature of "What You See Is What You Get" (WYSIWYG). Developers can drag widgets from the ToolBox to the canvas to create widgets for the current page. After adding image resources to the project, the widgets can be configured and linked to custom images. The hierarchy relationship between widgets will be displayed through the Widget tree.

<br/>
<div style="text-align: center"><img img width="600" src="https://foruda.gitee.com/images/1701069129985417739/5558648f_10088396.png "></div>
<br/>

- This tool requires adding pictures in advance, and then dragging the widgets in the `ToolBox` to the middle screen to lay out the same UI as the current `Widget tree`.
## Javascript

- Non-default effects and logic for controls currently need to be implemented by developers using JavaScript in the current version. For example, control interactions include switch widgets switching images on click, tab widgets sliding , etc. Please refer to `JavaScript syntax` to learn more about the JavaScript-based UI development approach.



### Gestures
In the JS file `gui_engine\example\screen_480_480\root\app\box\box.js`, the control and interaction logic of the UI is implemented.

#### Light control switch

<br/>
<div style="text-align: center"><img img width="300" src="https://foruda.gitee.com/images/1701075114325492203/ca9769d1_10088396.png "></div>
<br/>

- The callback functions for opening and closing the switch control named "kitchen_switch" are registered sequentially. When the switch control "kitchen_switch" is opened, its callback function `led1OnFunc` will be triggered and called.
- The control of the lights in this example is abstracted as a `Gpio` object. Each light corresponds to a `Gpio` object, and its value is assigned using the `writeSync` function, which is defined in the underlying layer to accommodate different smart home communication control protocols and control methods.

```javascript
// define an Gpio object
var LED1 = new Gpio(0, 'out');
function led1OnFunc(params) {
    if (sleep_flag) {
        LED1.writeSync(0)
    }   
}

// register callback function
sw.getElementById('living_switch')
sw.switch_on(led1OnFunc)
sw.switch_off(led1OffFunc)
```

#### Tab jumping switch

<br/>
<div style="text-align: center"><img width= "300" src="https://docs.realmcu.com/HoneyGUI/image/sample/86box/86box.gif"/></div>
<br/>


1. Register a tab slide callback for the tabview widget. When the tab is changed by sliding, update the current tab index and synchronize the UI display state.
2. Register a jump control callback function for each switch that controls the navigation. When called back, pass the index value as a parameter to indicate the tab to be navigated to.
3. In the callback function, use the `jump` function to navigate and synchronize the UI display state.

```javascript
tab.getElementById('tabview0')
var tabJump = {
    cur_tab_x: 0,
    nxt_tab_x: 0,
	cur_tab_y: 0,
    nxt_tab_y: 0
}
function sw_jump_tab(params) {
	// console.log('jump', params)
	tabJump.nxt_tab_x = params
	
	if(tabJump.nxt_tab_x != tabJump.cur_tab_x)
	{
		sw_jump_turnoff();
		tab.jump(params);
		tabJump.cur_tab_x = tabJump.nxt_tab_x
	}
}

function sw_jump_keep_on(params) {
	// console.log('sw_jump_keep_on ', params)
	Id_prefix = 'sw_tab';
	if(params == tabJump.nxt_tab_x)
	{
		sw.getElementById(sw_getId(params));
		sw.turnOn();
	}
}

function tab_slide(params) {
	// console.log('tab_slide')
	var cur_tab = tab.getCurTab()

	tabJump.nxt_tab_x = cur_tab.x;
	sw_turnOn(tabJump.nxt_tab_x);
	sw_turnOff(tabJump.cur_tab_x);
	tabJump.cur_tab_x = cur_tab.x;
}

// tab change
tab.onChange(tab_slide)

// jump tab0
sw.getElementById('sw_tab0')
sw.onOn(sw_jump_tab, 0)
sw.onOff(sw_jump_keep_on, 0)
```
