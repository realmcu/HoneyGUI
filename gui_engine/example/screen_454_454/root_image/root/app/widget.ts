var win = {
    getElementById : function (win_name : string) {},
    onClick : function (click_callback_func) {
        
    },
    onRight : function (click_callback_func) {
        
    },
    onLeft : function (click_callback_func) {
        
    },
    onUp : function (click_callback_func) {
        
    },
    onDown : function (click_callback_func) {
        
    },
    onPress : function (callback_func) {},
    onRelease : function (click_callback_func) {},
    onHold : function (callback_func) {},
}
var icon = {
    getElementById : function (win_name : string) {},
    onClick : function (click_callback_func) {},
    onPress : function (callback_func) {},
    onRelease : function (click_callback_func) {},
    onHold : function (callback_func) {},
    getAttribute : function(attributeName : string) {},
    setAttribute :function(attributeName : string, value : any) {},
    getChildElementByTag : function (tag:string) {},
    write : function (txt_string) {},
}
var textbox = {
    getElementById : function (win_name : string) {},
    write : function (txt_string) {},
}
var img = {
    getElementById : function (win_name : string) {},
    rotation : function (degree :number , center_x :number, center_y :number) {
        
    },
    scale : function (scale_x :number , scale_y) {},
    setMode : function(mode){},
    set_src :function(src : string){},
}
var iconlist = {
    getElementById : function (win_name : string) {},
    append : function (iconname : string, txt : string, imgSrc :string){},
    clean : function name() {}
}
var bluetooth = {
    open : function () {},
    stop : function (){},
    scan : function () {},
    onFound : function (callback : Function){},    
}
var sw = {
    getElementById : function (win_name : string) {},
    onOn : function (func) {},
    onOff : function (func) {},
    turnOn : function (func) {},
    turnOff : function (func) {},
}
var progressbar = {
    getElementById : function (win_name : string) {},
    progress : function (progressToSet : number):number ,

}
var seekbar = {
    getElementById : function (win_name : string) {},
    progress : function (progressToSet : number):{},
    onPress : function (callback_func) {},
    onPressing : function (callback_func) {},
    onRelease : function (click_callback_func) {},
}
/**
 * @link https://www.npmjs.com/package/onoff#gpiogpio-direction--edge--options
 * @param gpio 
 * @param direction 
 */
function Gpio(gpio, direction) {
    this.gpio = gpio;
    this.direction = direction;
    this.writeSync;
  }
