# JavaScript syntax




## Win

* Hide a window

``` javascript
win.getElementById('heat_win')
hid = win.getAttribute("hidden")
console.log(hid)
if (!hid) {
    win.setAttribute("hidden", "hidden");
}
```
* Listen to gestures

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

* click to swap windows between ```'cool_win'``` and ```'heat_win'```
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


## Button
* Listen to press gesture, the function ```iconNromalOnPressFunc``` will trigger when finger touchs screen.

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



## Text

* Change text content

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
## ImageSeekbar

* Drag the progress bar and then the text shows the current progress.
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
* Animation based on imageSeekbar
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
    progress : function (progressToSet : number):{},//get or set progress//return progress
    onPress : function (callback_func) {}, //gesture press
    onPressing : function (callback_func) {},//gesture pressing
    onRelease : function (click_callback_func) {},//gesture release
    setAnimate : function (frameCallback : function, config : object) {},// frameCallback function will be executed once every frame // var curtainAnimateTiming = {duration: 2000,iterations:1,}
    setAttribute :function(attributeName : string, value : any) {},  //support "hidden"
    getAttribute : function(attributeName : string) {},   //return attribute value //support "hidden"
    palyAnimate : function () {}, //Start animation
```
## Switch

* Listen to 2 gestures
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
This is the ```writeSync``` 's control gpio led implementation for RTL87X2G. First get gpio value and direction value, then use specify driver api to operate led.
Refer to [https://www.npmjs.com/package/onoff#usage](https://www.npmjs.com/package/onoff#usage) for more information.
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
    onOn : function (func) {},
    onOff : function (func) {},
    onPress : function (func) {},
    turnOn : function (func) {},//turn on the switch
    turnOff : function (func) {},//turn off the switch

```


## IMAGE

* API
```javascript
    getElementById : function (widget_name : string) {},
    rotation : function (degree:number, centerX:number, centerY:number) {},
    scale : function (scaleRateX:number, scaleRateY:number) {},
    setMode : function (modeIndex:number) {},
```
## APP

* API
```javascript
    open : function (appXML : string) {},
    close : function () {},
```
## PROGRESSBAR

* API
```javascript
    getElementById : function (widget_name : string) {},
    progress : function (progressToSet : number):{},//get or set progress//return progress
```
## TAB

* API
```javascript
    getElementById : function (widget_name : string) {},
    jump : function (tabIndex : number) {}, //jump to horizontal tab 
    OnChange : function (func) {},//Listen for events where the index value changes
    getCurTab : function () {},//return x,y,z property
```
