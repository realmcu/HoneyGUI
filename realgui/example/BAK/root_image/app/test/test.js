console.log('enter test')
icon.getElementById('iconNormal')

function iconNromalOnPressFunc(params) {
    console.log('iconNromalOnPressFunc')
}
icon.onPress(iconNromalOnPressFunc)

function iconNromalOnReleaseFunc(params) {
    console.log('iconNromalOnReleaseFunc')
}
icon.onRelease(iconNromalOnReleaseFunc)

function iconNromalOnHoldFunc(params) {
    console.log('iconNromalOnHoldFunc')
}
icon.onHold(iconNromalOnHoldFunc)

function iconNromalOnClickFunc(params) {
    console.log('iconNromalOnClickFunc')
}
icon.onClick(iconNromalOnClickFunc)


win.getElementById('tab7Win')
function winNromalOnPressFunc(params) {
    console.log('winNromalOnPressFunc')
}
win.onPress(winNromalOnPressFunc)

function winNromalOnReleaseFunc(params) {
    console.log('winNromalOnReleaseFunc')
}
win.onRelease(winNromalOnReleaseFunc)

function winNromalOnHoldFunc(params) {
    console.log('winNromalOnHoldFunc')
}
win.onHold(winNromalOnHoldFunc)

function winNromalOnClickFunc(params) {
    console.log('winNromalOnClickFunc')
}
win.onClick(winNromalOnClickFunc)

function winNromalOnUpFunc(params) {
    console.log('winNromalOnUpFunc')
}
win.onUp(winNromalOnUpFunc)

function winNromalOnLeftFunc(params) {
    console.log('winNromalOnLeftFunc')
}
win.onLeft(winNromalOnLeftFunc)

function winNromalOnRightFunc(params) {
    console.log('winNromalOnRightFunc')
}
win.onRight(winNromalOnRightFunc)

function winNromalOnDownFunc(params) {
    console.log('winNromalOnDownFunc')
}
win.onDown(winNromalOnDownFunc)

sw.getElementById('tab8Switch')
function swOnOnFunc(params) {
    console.log('swOnOnFunc')
}
sw.onOn(swOnOnFunc)
function swOnOffFunc(params) {
    console.log('swOnOffFunc')
}
sw.switch_off(swOnOffFunc)
sw.turnOn();

progressbar.getElementById('tab9Pro1')
console.log('progress:'+progressbar.progress(0.912))
progressbar.getElementById('tab9Pro2')
progressbar.progress(0.5)
console.log('progress:'+progressbar.progress())

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
