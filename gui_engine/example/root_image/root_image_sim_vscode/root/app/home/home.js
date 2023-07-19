console.log('enter box3')



var P1_1 = 9
var P1_0 = 8
var P2_5 = 21
var LED1 = new Gpio(P1_0, 'out');
var LED2 = new Gpio(P1_1, 'out');
var LED3 = new Gpio(P2_5, 'out');
console.log(LED1.gpio+';'+LED1.direction)
LED1.writeSync(0)
LED2.writeSync(0)
LED3.writeSync(1)
/*
sw.getElementById('P1_0')
function swOnOnFunc(params) {
    console.log('swOnOnFunc')
    LED1.writeSync(0)
}
sw.onOn(swOnOnFunc)

function swOnOffFunc(params) {
    console.log('swOnOffFunc')
    LED1.writeSync(1)
}
sw.onOff(swOnOffFunc)

sw.getElementById('P2_5')
function swOnOnFunc2(params) {
    console.log('swOnOnFunc2')
    LED3.writeSync(1)
}
sw.onOn(swOnOnFunc2)

function swOnOffFunc2(params) {
    console.log('swOnOffFunc2')
    LED3.writeSync(0)
}
sw.onOff(swOnOffFunc2)
*/

