#  86Box

本示例演示了如何开发一个 RealUI 86Box APP，开发者可以从中学习和理解开发 UI APP 的基本方法和流程。

## 源文件
- APP package `realgui\example\screen_480_480\root\app\box`
- UI 设计工程 `RVisualDesigner-v1.0.5.0\Demo\480x480\box\box480x480.rtkprj`

## UI设计
### RVisualDesigner
- RealUI 86Box 使用 RVisualDesigner 来完成 UI 设计， 首次使用 RVisualDesigner 请参阅 `RVisualDesigner-v1.0.5.0\RTKIOT Visual Designer User Guide EN.pdf` 获取详尽的使开发指南。

- 在图示路径中找到并打开示例的 UI 设计工程：
<br/>
<div style="text-align: center"><img width="750" src="https://foruda.gitee.com/images/1725515761517919126/033a0990_9218678.png"></div>
<br/>

- 依次点击 `Export` 和 `Simulate` 来完成导出和启动模拟，模拟器窗口启动后将显示 86Box APP 图标，鼠标点击后进入对应的 APP。
<br/>
<div style="text-align: center"><img img width="750" src="https://foruda.gitee.com/images/1725515804906725239/f2b55fe1_9218678.png"></div>
<br/>
<br/>
<div style="text-align: center"><img img width="400" src="https://foruda.gitee.com/images/1725515818032367801/cd5c125c_9218678.png"></div>
<br/>

- 进入 APP，在模拟器窗口中将看到与 RVisualDesigner 设计工程中一致的 UI 内容，因而该设计模式具有 “所见即所得” 特点。开发者从 ```ToolBox``` 中拖拽控件到画布中来创建控件到当前页面，添加图片资源到工程后，控件可以配置并链接到自定义的图片。控件间的层级关系将通过 ```Widget tree``` 来展示。

<br/>
<div style="text-align: center"><img img width="750" src="https://foruda.gitee.com/images/1725515836928906868/bf7c4fa0_9218678.png"></div>
<br/>


## Javascript
- 控件非默认的效果和逻辑在当前版本中需要开发者通过 JavaScript 来实现，如控件之间的联动，控件默认交互效果包括 switch 控件的点击切换图片、tab 控件的滑动切换等。请参阅 `JavaScript syntax` 了解基于 JavaScript 的 UI 开发方法。


### 交互
在 JS 文件 `realgui\example\screen_480_480\root\app\box\box.js` 中，实现了 UI 的控制和交互逻辑。


#### 灯光控制 switch

<br/>
<div style="text-align: center"><img img width="400" src="https://foruda.gitee.com/images/1725515850170490894/67696ea2_9218678.png"></div>
<br/>

- 为名为 "kitchen_switch" 的 switch 控件依次注册了打开和关闭 switch 时的回调函数。当 switch 控件  "kitchen_switch" 被打开时，其回调函数 `led1OnFunc` 将会被触发调用。
- 灯光的控制在本示例中被抽象为一个 `Gpio` 对象，每盏灯对应一个 `Gpio` 对象，通过 `writeSync` 函数为其赋值，其行为在底层中被定义，以适应不同的智能家居通信控制协议和控制方式。

```javascript
// 定义一个 Gpio 对象
var LED1 = new Gpio(0, 'out');
function led1OnFunc(params) {
    if (sleep_flag) {
        LED1.writeSync(0)
    }   
}

// 为控件 'kitchen_switch' 注册开关动作回调
sw.getElementById('kitchen_switch')
sw.switch_on(led1OnFunc)
sw.switch_off(led1OffFunc)
```

#### Tab 跳转 switch

<br/>
<div style="text-align: center"><img width= "400" src="https://docs.realmcu.com/HoneyGUI/image/sample/86box/86box.gif"/></div>
<br/>

1. 为 tabview 控件注册 tab 切换回调，当 tab 滑动切换时，更新当前 tab 索引值并同步 UI 显示状态。
2. 为每个控制跳转的 switch 注册跳转控制回调函数，回调时传入索引值作为参数表征需要跳转的 tab。
3. 在回调函数中通过 `jump` 函数来跳转，并同步 UI 显示状态。



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




