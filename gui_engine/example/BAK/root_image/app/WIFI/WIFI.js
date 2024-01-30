textbox.getElementById('text1')
textbox.write('Click')
var textAnimateConfig = {
    duration: 2000,
    iterations:-1,
    from: 1,
    to: 0
}
var speed = 888;
function textAnimateCallback(params) {
    speed=speed+1
    textbox.write(wifi.readSpeed()+'bit/s')
}

function startSpeed(params) {
textbox.setAnimate(textAnimateCallback, textAnimateConfig)
textbox.palyAnimate()
wifi.startSpeed()
}
function stopSpeed(params) {
    textbox.setAnimate(textAnimateCallback, textAnimateConfig)
    textbox.pauseAnimate()
	wifi.stopSpeed()
    }
sw.getElementById('switch2')
sw.onOn(startSpeed)
sw.onOff(stopSpeed)
