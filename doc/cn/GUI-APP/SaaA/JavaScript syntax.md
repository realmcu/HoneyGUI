# JavaScript 语法




## 窗口 (Win)

* 隐藏窗口

``` javascript
win.getElementById('heat_win')
hid = win.getAttribute("hidden")
console.log(hid)
if (!hid) {
    win.setAttribute("hidden", "hidden");
}
```
* 监听手势

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

* 点击交换 ```'cool_win'``` 和 ```'heat_win'``` 之间的窗口
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
    onClick : function (click_callback_func) {}
    onRight : function (click_callback_func) {}
    onLeft : function (click_callback_func) {}
    onUp : function (click_callback_func) {}
    onDown : function (click_callback_func) {}
    onPress : function (callback_func) {}
    onRelease : function (click_callback_func) {}
    onHold : function (callback_func) {}
    getAttribute : function(attributeName : string) {},   //return attribute value //support "hidden"
    removeAttribute : function (attribute : string) {} //support "hidden"
    setAttribute :function(attributeName : string, value : any) {},  //support "hidden"

```


## 按钮 (Button)
* 监听按下手势，当手指触摸屏幕时，触发函数```iconNromalOnPressFunc```。

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
    onClick : function (click_callback_func) {},
    onPress : function (callback_func) {},
    onRelease : function (click_callback_func) {},
    onHold : function (callback_func) {},
    getChildElementByTag : function (tag : string) {},
    write : function (text : string) {},
```



## 文本 (Text)

* 修改文本内容

```javascript
    textbox.getElementById('tab10text1')
    textbox.write('progress:'+seekbar.progress())
```
* API

```javascript
    getElementById : function (win_name : string) {},
    write : function (text : string) {},
    setPosition : function (position : object) {}, //var position={x:0,y:0}
```
## 图像滑动条 (ImageSeekbar)

* 拖动进度条，文本显示当前进度。
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
* 基于图像滑动条的动画
```javascript
var curtainAnimateTiming = {
    duration: 2000,
    iterations:-1,
}
var curtain_open = 0;
seekbar.getElementById('curtain_bar')
function curtainFrame(params) {
    animate= seekbar.animateProgress()
    seekbar.setAttribute("progress", animate)
}
seekbar.setAnimate(curtainFrame, curtainAnimateTiming)
seekbar.palyAnimate()
```

* API

```javascript
    getElementById : function (win_name : string) {},
    progress : function (progressToSet : number):{},//获取或设置进度//返回进度
    onPress : function (callback_func) {}, //手势按下
    onPressing : function (callback_func) {},//手势按下时
    onRelease : function (click_callback_func) {},//手势释放
    setAnimate : function (frameCallback : function, config : object) {},// 每帧执行frameCallback函数 // var curtainAnimateTiming = {duration: 2000,iterations:1,}
    setAttribute :function(attributeName : string, value : any) {},  //支持 "hidden"
    getAttribute : function(attributeName : string) {},   //返回属性值 //支持 "hidden"
    palyAnimate : function () {}, //开始动画
```
## 开关 (Switch)

* 监听两种手势
```javascript

sw.getElementById('tab8Switch')
function swOnOnFunc(params) {
    console.log('swOnOnFunc')
}
sw.switch_on(swOnOnFunc)
function swOnOffFunc(params) {
    console.log('swOnOffFunc')
}
sw.switch_off(swOnOffFunc)
sw.turnOn();
```
* turn on a led (P1_1)
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
这是针对RTL87X2G的GPIO LED的 ```writeSync``` 控制的实现。首先获取GPIO值和方向值，然后使用特定的驱动程序API操作LED。详细信息请参考[https://www.npmjs.com/package/onoff#usage](https://www.npmjs.com/package/onoff#usage)。
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
* API

```javascript
    getElementById : function (win_name : string) {},
    switch_on : function (func) {},
    switch_off : function (func) {},
    onPress : function (func) {},
    turnOn : function (func) {},//打开开关
    turnOff : function (func) {},//关闭开关

```


## 图像 (IMAGE)

* API
```javascript
    getElementById : function (widget_name : string) {},
    rotation : function (degree:number, centerX:number, centerY:number) {},
    scale : function (scaleRateX:number, scaleRateY:number) {},
    setMode : function (modeIndex:number) {},
```
## 应用程序 (APP)

* API
```javascript
    open : function (appXML : string) {},
    close : function () {},
```
## 进度条 (PROGRESSBAR)

* API
```javascript
    getElementById : function (widget_name : string) {},
    progress : function (progressToSet : number):{},//获取或设置进度//返回进度
```
## 选项卡 (TAB)

* API
```javascript
    getElementById : function (widget_name : string) {},
    jump : function (tabIndex : number) {}, //跳转到指定水平选项卡 
    OnChange : function (func) {},//监听索引值变化的事件
    getCurTab : function () {},//返回当前选项卡的属性（x、y、z等）
```
