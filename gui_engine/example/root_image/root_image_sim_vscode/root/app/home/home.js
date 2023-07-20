console.log('enter box3')

/**
 * lamp
 */

var P1_1 = 9
var P1_0 = 8
var P2_5 = 21
var lamp1 = P1_1
var lamp2 = P1_0
var lamp3 = P2_5
var lamp4 = P1_1
var lamp5 = P1_0
var lamp6 = P2_5
var LED1 = new Gpio(P1_0, 'out');
var LED2 = new Gpio(P1_1, 'out');
var LED3 = new Gpio(P2_5, 'out');
console.log(LED1.gpio+';'+LED1.direction)


function led1OnFunc(params) {
    console.log('led1OnFunc')
    LED1.writeSync(0)
}
function led1OffFunc(params) {
    console.log('led1OffFunc')
    LED1.writeSync(1)
}
function led2OnFunc(params) {
    console.log('led2OnFunc')
    LED2.writeSync(0)
}
function led2OffFunc(params) {
    console.log('led2OffFunc')
    LED2.writeSync(1)
}
function led3OnFunc(params) {
    console.log('led3OnFunc')
    LED3.writeSync(1)
}
function led3OffFunc(params) {
    console.log('led3OffFunc')
    LED3.writeSync(0)
}
function lampSwitchOnFunc(params) {
    console.log('lampSwitchOnFunc')
    sw.getElementById('Kitchen')
    sw.turnOn()
    sw.getElementById('Parlor')
    sw.turnOn()
    sw.getElementById('porch')
    sw.turnOn()   
    sw.getElementById('bedroom')
    sw.turnOn()
    sw.getElementById('bedroom2')
    sw.turnOn()
    sw.getElementById('bedroom3')
    sw.turnOn()       
}
function lampSwitchOffFunc(params) {
    console.log('lampSwitchOffFunc')
    sw.getElementById('Kitchen')
    sw.turnOff()
    sw.getElementById('Parlor')
    sw.turnOff()
    sw.getElementById('porch')
    sw.turnOff()   
    sw.getElementById('bedroom')
    sw.turnOff()
    sw.getElementById('bedroom2')
    sw.turnOff()
    sw.getElementById('bedroom3')
    sw.turnOff()  
}
sw.getElementById('Kitchen')
sw.onOn(led1OnFunc)
sw.onOff(led1OffFunc)
sw.getElementById('Parlor')
sw.onOn(led2OnFunc)
sw.onOff(led2OffFunc)
sw.getElementById('bedroom')
sw.onOn(led3OnFunc)
sw.onOff(led3OffFunc)
sw.getElementById('bedroom2')
sw.onOn(led1OnFunc)
sw.onOff(led1OffFunc)
sw.getElementById('bedroom3')
sw.onOn(led2OnFunc)
sw.onOff(led2OffFunc)
sw.getElementById('porch')
sw.onOn(led3OnFunc)
sw.onOff(led3OffFunc)
sw.getElementById('lampSwitch')
sw.onOn(lampSwitchOnFunc)
sw.onOff(lampSwitchOffFunc)

/**
 * ac
 */
var acStatus = 1
var acOnOffStatus = 0;
function coolingFunc(params) {
    console.log('coolingFunc')
    win.getElementById('coolAC')
    hid = win.getAttribute("hidden")
    if (hid) {
        console.log("true")
    }
    if (!hid) {
        console.log("false")
    }
    if (hid) {
        win.removeAttribute("hidden");
    }
    win.getElementById('heatAC')
    hid = win.getAttribute("hidden")
    console.log(hid)
    if (hid) {
        console.log("true")
    }
    if (!hid) {
        console.log("false")
    }
    if (!hid) {
        win.setAttribute("hidden", "hidden");
    }
    sw.getElementById('heatingSwitch')
    sw.turnOff()
    acStatus = 1
    acOnOffStatus = 1
    sw.getElementById('acSwitch')
    sw.turnOn()
}
function heatingFunc(params) {
    console.log('heatingFunc')
    win.getElementById('heatAC')
    hid = win.getAttribute("hidden")
    if (hid) {
        console.log("true")
    }
    if (!hid) {
        console.log("false")
    }
    if (hid) {
        win.removeAttribute("hidden");
    }
    win.getElementById('coolAC')
    hid = win.getAttribute("hidden")
    console.log(hid)
    if (hid) {
        console.log("true")
    }
    if (!hid) {
        console.log("false")
    }
    if (!hid) {
        win.setAttribute("hidden", "hidden");
    }
    sw.getElementById('coolingSwitch')
    sw.turnOff()
    acStatus = 2
    acOnOffStatus = 1
    sw.getElementById('acSwitch')
    sw.turnOn()
}
function acOffFunc(params) {
    console.log('heatingFunc')
    win.getElementById('coolAC')
    hid = win.getAttribute("hidden")
    console.log(hid)
    if (hid) {
        console.log("true")
    }
    if (!hid) {
        console.log("false")
    }
    if (!hid) {
        win.setAttribute("hidden", "hidden");
    }
    win.getElementById('heatAC')
    hid = win.getAttribute("hidden")
    console.log(hid)
    if (hid) {
        console.log("true")
    }
    if (!hid) {
        console.log("false")
    }
    if (!hid) {
        win.setAttribute("hidden", "hidden");
    }
    sw.getElementById('coolingSwitch')
    sw.turnOff()
    sw.getElementById('heatingSwitch')
    sw.turnOff()
    acOnOffStatus = 0;
}
function acOnFunc(params) {
    console.log('heatingFunc')
    if (!acOnOffStatus) {
        
    
    if (acStatus == 1) {
        sw.getElementById('coolingSwitch')
        sw.turnOn()
    } else if (acStatus == 2) {
        sw.getElementById('heatingSwitch')
        sw.turnOn()
    }
    }

}
function acOffFunc1(params) {
    console.log('heatingFunc')

}
sw.getElementById('coolingSwitch')
sw.onOn(coolingFunc)
sw.onOff(acOffFunc1)


sw.getElementById('heatingSwitch')
sw.onOn(heatingFunc)
sw.onOff(acOffFunc1)

sw.getElementById('acSwitch')
sw.onOn(acOnFunc)
sw.onOff(acOffFunc)
sw.turnOn()

/**
 * curtain
 */
var curtainAnimateTiming = {
    duration: 2000,
    iterations:1,
    from: 1,
    to: 0
}
var curtain_open = 0;
function curtainFrame(params) {

    console.log('curtainFrame')
    seekbar.getElementById('curtain')
    var animate = seekbar.getAttribute("animate")
    console.log(animate.progress)
    if (curtain_open==1) {
        seekbar.setAttribute("progress", 1.0-animate.progress)
    } else {
        seekbar.setAttribute("progress", animate.progress)
    }
    
}
function swCurtain(params) {
    console.log('swCurtain')
    seekbar.getElementById('curtain')
    seekbar.palyAnimate()
    if (curtain_open==1) {
        curtain_open = 0;
    } else {
        curtain_open = 1;
    }
}

seekbar.getElementById('curtain')
seekbar.setAnimate(curtainFrame, curtainAnimateTiming)
sw.getElementById('curtainSwitch1')
sw.onOn(swCurtain)
sw.onOff(swCurtain)
sw.getElementById('curtainSwitch2')
sw.onOn(swCurtain)
sw.onOff(swCurtain)
sw.getElementById('curtainSwitch3')
sw.onOn(swCurtain)
sw.onOff(swCurtain)




