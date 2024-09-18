# JavaScript 语法




## 窗口控件

- 这是一个容器控件。
- 对窗口控件的操作将影响嵌套在容器中的控件。
- 隐藏窗口将隐藏嵌套的控件。
- 当窗口进行图形变换（例如平移和缩放）时，嵌套的控件将进行一致的变换。
- 这个控件可以监控多个手势。

### 隐藏窗口
- 这个变量 ```win``` 被赋值为 ```win``` 标签 ```heat_win``` 的句柄。
- 变量 ```hid```被赋值为 ```win``` 标签的 ```hidden``` 属性的句柄。
- 将 ```hidden``` 属性的值设置为 ```hidden``` 来实现隐藏。
``` javascript
win.getElementById('heat_win') //win 变成 heat_win 的句柄 
hid = win.getAttribute("hidden") //get attribute handle hid
console.log(hid)
if (!hid) {
    win.setAttribute("hidden", "hidden");
}
```

### 监听手势
- ```win.onPress``` 函数使 ```win``` 控件能够监控手指触摸屏幕的事件。如果手指在窗口区域内触摸屏幕，将执行参数函数。
- ```win.onRelease``` 函数使 ```win``` 控件能够监控手指离开屏幕的事件。
- 当手指触摸屏幕时，将执行 ```winNormalOnPressFunc``` 函数。
- 当手指离开屏幕时，将执行 ```winNormalOnReleaseFunc``` 函数。
```javascript
win.getElementById('tab7Win')
function winNromalOnPressFunc(params) {
    console.log('winNromalOnPressFunc')
}
win.onPress(winNromalOnPressFunc)

function winNromalOnReleaseFunc(params) {
    console.log('winNromalOnReleaseFunc')
}
win.onRelease(winNromalOnReleaseFunc)
```


### 切换窗口示例
- 实现逻辑是点击当前窗口将隐藏当前窗口并显示另一个窗口。
- 点击在 ```'cool_win'``` 和 ```'heat_win'``` 窗口之间进行切换。
- ```win.onClick``` 函数使 ```win``` 控件能够监控手指点击屏幕的事件。
- ```win.removeAttribute``` 这个函数用于移除 ```win``` 标签的一个属性。当移除了 ```hidden``` 属性时，```win``` 标签对应的控件将被显示。
- 在触摸设备上，当用户在 ```win``` 区域内触摸一个元素并在短时间内抬起手指时，通常会触发一个点击事件。


```javascript
win.getElementById('cool_win')
function hideCool(params) {
    console.log('hideCool')
    win.getElementById('cool_win')
    win.setAttribute("hidden", "hidden");
    win.getElementById('heat_win')
    win.removeAttribute("hidden") 
}
win.onClick(hideCool)
win.getElementById('heat_win')
function hideHeat(params) {
    console.log('hideHeat')
    win.getElementById('heat_win')
    win.setAttribute("hidden", "hidden");
    win.getElementById('cool_win')
    win.removeAttribute("hidden") 
}
win.onClick(hideHeat)
```

* API
```javascript
    getElementById : function (win_name : string) {}
    onClick : function (callback_func) {}
    onRight : function (callback_func) {}
    onLeft : function (callback_func) {}
    onUp : function (callback_func) {}
    onDown : function (callback_func) {}
    onPress : function (callback_func) {}
    onRelease : function (callback_func) {}
    onHold : function (callback_func) {}
    getAttribute : function(attributeName : string) {},   //return attribute value //support "hidden"
    removeAttribute : function (attribute : string) {} //support "hidden"
    setAttribute :function(attributeName : string, value : any) {},  //support "hidden"

```


## 按钮
### 监听按下事件的示例
- 可以用于开发按键按下高亮效果或者需要快速响应的按钮。
- 监听按压手势，当手指在按钮区域内触摸屏幕时，将触发 ```iconNromalOnPressFunc``` 函数。

```javascript
icon.getElementById('iconNormal')

function iconNromalOnPressFunc(params) {
    console.log('iconNromalOnPressFunc')
}
icon.onPress(iconNromalOnPressFunc)
```

* API
```javascript
    getElementById : function (win_name : string) {},
    onClick : function (callback_func) {},
    onPress : function (callback_func) {},
    onRelease : function (callback_func) {},
    onHold : function (callback_func) {},
    getChildElementByTag : function (tag : string) {},
    write : function (text : string) {},
```



## 文本

### 编辑文本 
* Using ```textbox.write``` function.

```javascript
    textbox.getElementById('tab10text1')
    textbox.write('progress:'+seekbar.progress())
```
### API

```javascript
    getElementById : function (win_name : string) {},
    write : function (text : string) {},
    setPosition : function (position : object) {}, //var position={x:0,y:0}
```
## 拖动条
### 显示当前进度的示例
* 拖动进度条，文本将显示当前进度。
* ```seekbar.progress()``` 函数可以读取和写入进度。
* ```seekbar.onPressing``` 函数将监听手指在屏幕上保持按压的事件。当手指与屏幕接触时，此参数函数将在每一帧中执行。
```javascript
seekbar.getElementById('tab10Seek1')
function seekbarOnPress(params) {
    console.log('seekbarOnPress')
}
seekbar.onPress(seekbarOnPress)
function seekbarOnrelease(params) {
    console.log('seekbarOnrelease')
}
seekbar.onRelease(seekbarOnrelease)
function seekbarOnPressing(params) {
    console.log('seekbarOnPressing')
    textbox.getElementById('tab10text1')
    textbox.write('progress:'+seekbar.progress())
}
seekbar.onPressing(seekbarOnPressing)
```
### 一个拖动进度条从0增加到100%的动画
- 进度条将显示一个从开始到结束不断进度，然后循环回到起点，创建一个永远移动的进度条。
- ```seekbar.setAnimate``` 这个函数设置进度条的帧动画，传递的参数是帧动画的回调函数和动画持续时间属性。
- 定义一个对象 ```curtainAnimateTiming``` 来指定动画的时序属性。```duration``` 设置动画一个周期的持续时间，以毫秒为单位。 ```iterations``` 是动画应该重复的次数，-1 表示动画应该无限次重复。
```javascript
var curtainAnimateTiming = {
    duration: 2000,      // The duration of the animation in milliseconds (2000ms = 2 seconds)
    iterations: -1,      // The number of times the animation should repeat
                         // -1 indicates the animation should repeat indefinitely
};
var curtain_open = 0;
seekbar.getElementById('curtain_bar')
function curtainFrame(params) {
    animate= seekbar.animateProgress()
    seekbar.setAttribute("progress", animate)
}
seekbar.setAnimate(curtainFrame, curtainAnimateTiming)
seekbar.palyAnimate()
```

### API

```javascript
    getElementById : function (win_name : string) {},
    progress : function (progressToSet : number){},//get or set progress//return progress
    onPress : function (callback_func) {}, //gesture press
    onPressing : function (callback_func) {},//gesture pressing
    onRelease : function (callback_func) {},//gesture release
    setAnimate : function (frameCallback : function, config : object) {},// frameCallback function will be executed once every frame // var curtainAnimateTiming = {duration: 2000,iterations:1,}
    setAttribute :function(attributeName : string, value : any) {},  //support "hidden"
    getAttribute : function(attributeName : string) {},   //return attribute value //support "hidden"
    palyAnimate : function () {}, //Start animation
```
## 开关

### 监听两个事件
- 开关控件有两个事件，即被打开触发和被关闭触发。
- ```sw.onOn``` 这个函数用于注册被打开的事件。
- ```sw.onOff``` 这个函数用于注册被关闭的事件。
```javascript

sw.getElementById('tab8Switch')
function swOnOnFunc(params) {
    console.log('swOnOnFunc')
}
sw.onOn(swOnOnFunc)
function swOnOffFunc(params) {
    console.log('swOnOffFunc')
}
sw.onOff(swOnOffFunc)
sw.turnOn();
```
### 打开一个LED (P1_1)
```javascript
var P1_1 = 9
var LED1 = new Gpio(P1_1, 'out');
function led1OnFunc(params) {
    console.log('led1OnFunc')
    LED1.writeSync(0)
}
sw.getElementById('living_switch')
sw.turnOn()
```
- 这个是 ```writeSync``` 函数控制 gpio led 在RTL87X2G上的实现。
- 首先获取GPIO值和方向，然后使用特定的驱动操作GPIO LED。
- 参考 [https://www.npmjs.com/package/onoff#usage](https://www.npmjs.com/package/onoff#usage)。
```c
DECLARE_HANDLER(writeSync)
{
    gui_log("enter writeSync:%d\n", args[0]);
    if (args_cnt >= 1 && jerry_value_is_number(args[0]))
    {

        int write_value = jerry_get_number_value(args[0]);
        int gpio = -1;
        jerry_value_t v1;
        jerry_value_t v2;
        v1 = js_get_property(this_value, "gpio");
        v2 = js_get_property(this_value, "direction");
        gpio = jerry_get_number_value(v1);
        jerry_release_value(v1);
        char *direction = js_value_to_string(v2);
        jerry_release_value(v2);
        int mode = 0;
#ifdef RTL8762G

        if (!strcmp(direction, "out"))
        {
            mode = PIN_MODE_OUTPUT;
        }
        else if (!strcmp(direction, "in"))
        {
            mode = PIN_MODE_INPUT;
        }
        if (gpio >= 0)
        {
            gui_log("gpio%d, %d, %d", gpio, mode, write_value);
            drv_pin_mode(gpio, mode);
            drv_pin_write(gpio, write_value);
```
### API

```javascript
    getElementById : function (win_name : string) {},
    onOn : function (func) {},
    onOff : function (func) {},
    onPress : function (func) {},
    turnOn : function (func) {},//turn on the switch
    turnOff : function (func) {},//turn off the switch

```


## 图片

### API
```javascript
    getElementById : function (widget_name : string) {},
    rotation : function (degree:number, centerX:number, centerY:number) {},
    scale : function (scaleRateX:number, scaleRateY:number) {},
    setMode : function (modeIndex:number) {},
```
## APP

### API
```javascript
    open : function (appXML : string) {},
    close : function () {},
```
## 进度条

### API
```javascript
    getElementById : function (widget_name : string) {},
    progress : function (progressToSet : number):{},//get or set progress//return progress
```
## 切屏页面（TAB）

### API
```javascript
    getElementById : function (widget_name : string) {},
    jump : function (tabIndex : number) {}, //jump to horizontal tab 
    OnChange : function (func) {},//Listen for events where the index value changes
    getCurTab : function () {},//return x,y,z property
```
