sw.getElementById('switch1')
sw1Flag = false;
function swOnFunc(params) {
    if (sw1Flag) {
        console.log('swFunc Off')
        sw1Flag = false
    } else {
        console.log('swFunc On')
        sw1Flag = true
    }
    
}
sw.onOn(swOnFunc)
sw.onOff(swOnFunc)
icon.getElementById('button0')
function iconNromalOnPressFunc(params) {
    console.log('iconNromalOnPressFunc')
}
icon.onClick(iconNromalOnPressFunc)
