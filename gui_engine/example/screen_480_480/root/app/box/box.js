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
//0x6c
var LED1 = new Gpio(0, 'out');
var LED2 = new Gpio(1, 'out');
var LED3 = new Gpio(2, 'out');
var LED4 = new Gpio(3, 'out');
var LED5 = new Gpio(4, 'out');
var LED6 = new Gpio(5, 'out');
var LED7 = new Gpio(6, 'out');
var LED8 = new Gpio(7, 'out');
var LED9 = new Gpio(49052, 'out');

//console.log(LED1.gpio+';'+LED1.direction)

var sleep_flag = true;
function sleepflagTrue(params) {
    
    sleep_flag = true;
    //console.log('sleep_flag;'+sleep_flag)
}
function led1OnFunc(params) {
    //console.log('led1OnFunc')
    //console.log('sleep_flag;'+sleep_flag)
    if (sleep_flag) {
        LED1.writeSync(0)
    }
    
}
function led1OffFunc(params) {
    //console.log('led1OffFunc')
    
    if (sleep_flag) {
        LED1.writeSync(1)
    }
}
function led2OnFunc(params) {
    //console.log('led2OnFunc')
    
    if (sleep_flag) {
        LED2.writeSync(0)
    }
}
function led2OffFunc(params) {
    //console.log('led2OffFunc')
    
    if (sleep_flag) {
        LED2.writeSync(1)
    }    
}
function led3OnFunc(params) {
    //console.log('led3OnFunc')
    
    if (sleep_flag) {
        LED3.writeSync(0)
    }
}
function led3OffFunc(params) {
    //console.log('led3OffFunc')
    
    if (sleep_flag) {
        LED3.writeSync(1)
    }
}
function led4OnFunc(params) {
    //console.log('led1OnFunc')
    
    if (sleep_flag) {
        LED4.writeSync(0)
    }
}
function led4OffFunc(params) {
    //console.log('led1OffFunc')
    
    if (sleep_flag) {
        LED4.writeSync(1)
    }
}
function led5OnFunc(params) {
    //console.log('led2OnFunc')
    
    if (sleep_flag) {
        LED5.writeSync(0)
    }
}
function led5OffFunc(params) {
    //console.log('led2OffFunc')
    
    if (sleep_flag) {
        LED5.writeSync(1)
    }
}
function led6OnFunc(params) {
    //console.log('led3OnFunc')
    
    if (sleep_flag) {
        LED6.writeSync(0)
    }
}
function led6OffFunc(params) {
    //console.log('led3OffFunc')
    
    if (sleep_flag) {
        LED6.writeSync(1)
    }
}
function led7OnFunc(params) {
    //console.log('led2OnFunc')
    
    if (sleep_flag) {
        LED7.writeSync(0)
    }
}
function led7OffFunc(params) {
    //console.log('led2OffFunc')
    
    if (sleep_flag) {
        LED7.writeSync(1)
    }
}
function led8OnFunc(params) {
    //console.log('led3OnFunc')
    
    if (sleep_flag) {
        LED8.writeSync(0)
    }
}
function led8OffFunc(params) {
    //console.log('led3OffFunc')
    
    if (sleep_flag) {
        LED8.writeSync(1)
    }
}
function lampSwitchOnFunc(params) {
    sleep_flag = false;
    //console.log('lampSwitchOnFunc')
    sw.getElementById('living_switch')
    sw.turnOn()
    sw.getElementById('dining_switch')
    sw.turnOn()
    sw.getElementById('study_switch')
    sw.turnOn()   
	sw.getElementById('wash_switch')
    sw.turnOn()
    sw.getElementById('kitchen_switch')
    sw.turnOn()
    sw.getElementById('bathroom_switch')
    sw.turnOn()  
    sw.getElementById('bedroom_switch')
    sw.turnOn()
    sw.getElementById('children_switch')
    sw.turnOn()     
    LED9.writeSync(0) 

}
function lampSwitchOffFunc(params) {
    sleep_flag = false;
    //console.log('lampSwitchOffFunc')
    sw.getElementById('living_switch')
    sw.turnOff()
    sw.getElementById('dining_switch')
    sw.turnOff()
    sw.getElementById('study_switch')
    sw.turnOff()  
	sw.getElementById('wash_switch')
    sw.turnOff()
    sw.getElementById('kitchen_switch')
    sw.turnOff()
    sw.getElementById('bathroom_switch')
    sw.turnOff()      
    sw.getElementById('bedroom_switch')
    sw.turnOff()
    sw.getElementById('children_switch')
    sw.turnOff()  
    LED9.writeSync(1) 

}
sw.getElementById('living_switch')
sw.onOn(led1OnFunc)
sw.onOff(led1OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('dining_switch')
sw.onOn(led2OnFunc)
sw.onOff(led2OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('study_switch')
sw.onOn(led3OnFunc)
sw.onOff(led3OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('wash_switch')
sw.onOn(led4OnFunc)
sw.onOff(led4OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('kitchen_switch')
sw.onOn(led5OnFunc)
sw.onOff(led5OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('bathroom_switch')
sw.onOn(led6OnFunc)
sw.onOff(led6OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('bedroom_switch')
sw.onOn(led7OnFunc)
sw.onOff(led7OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('children_switch')
sw.onOn(led8OnFunc)
sw.onOff(led8OffFunc)
sw.onPress(sleepflagTrue)
icon.getElementById('allTurnOn_button')
icon.onClick(lampSwitchOnFunc)
icon.getElementById('sleep_button')
icon.onClick(lampSwitchOffFunc)
/**
 * ac
 */
var acStatus = 1
var acOnOffStatus = 0;
function coolingFunc(params) {
    //console.log('coolingFunc')
    win.getElementById('cool_win')
    hid = win.getAttribute("hidden")
    if (hid) {
       // console.log("true")
    }
    if (!hid) {
       // console.log("false")
    }
    if (hid) {
        win.removeAttribute("hidden");
    }
    win.getElementById('heat_win')
    hid = win.getAttribute("hidden")
    //console.log(hid)
    if (hid) {
       // console.log("true")
    }
    if (!hid) {
       // console.log("false")
    }
    if (!hid) {
        win.setAttribute("hidden", "hidden");
    }
    sw.getElementById('heat_switch')
    sw.turnOff()
    acStatus = 1
    acOnOffStatus = 1
    sw.getElementById('ac_on_off_switch')
    sw.turnOn()
}
function heatingFunc(params) {
    //console.log('heatingFunc')
    win.getElementById('heat_win')
    hid = win.getAttribute("hidden")
    if (hid) {
      //  console.log("true")
    }
    if (!hid) {
       // console.log("false")
    }
    if (hid) {
        win.removeAttribute("hidden");
    }
    win.getElementById('cool_win')
    hid = win.getAttribute("hidden")
   // console.log(hid)
    if (hid) {
      //  console.log("true")
    }
    if (!hid) {
        //console.log("false")
    }
    if (!hid) {
        win.setAttribute("hidden", "hidden");
    }
    sw.getElementById('cool_switch')
    sw.turnOff()
    acStatus = 2
    acOnOffStatus = 1
    sw.getElementById('ac_on_off_switch')
    sw.turnOn()
}
function acOffFunc(params) {
   // console.log('heatingFunc')
    win.getElementById('cool_win')
    hid = win.getAttribute("hidden")
    //console.log(hid)
    if (hid) {
      //  console.log("true")
    }
    if (!hid) {
        //console.log("false")
    }
    if (!hid) {
        win.setAttribute("hidden", "hidden");
    }
    win.getElementById('heat_win')
    hid = win.getAttribute("hidden")
   // console.log(hid)
    if (hid) {
       // console.log("true")
    }
    if (!hid) {
      //  console.log("false")
    }
    if (!hid) {
        win.setAttribute("hidden", "hidden");
    }
    sw.getElementById('cool_switch')
    sw.turnOff()
    sw.getElementById('heat_switch')
    sw.turnOff()
    acOnOffStatus = 0;
}
var test_open = 0;
function testFrame(params) {

	test_open = test_open+1
	//console.log(test_open)
	//1.7s
	if(test_open==50)
	{
        lampSwitchOnFunc()
	}
	if(test_open==100)
	{
        lampSwitchOffFunc()
		test_open = 0;
	}
     
 }
function swTest(params) {
    // console.log('swCurtain')
     seekbar.getElementById('cool_bar')
     seekbar.palyAnimate()
 }
function acOnFunc(params) {
   // console.log('heatingFunc')
   //swTest()
    if (!acOnOffStatus) {
        
    
    if (acStatus == 1) {
        sw.getElementById('cool_switch')
        sw.turnOn()
    } else if (acStatus == 2) {
        sw.getElementById('heat_switch')
        sw.turnOn()
    }
    }

}
function acOffFunc1(params) {
    //console.log('heatingFunc')

}
sw.getElementById('cool_switch')
sw.onOn(coolingFunc)
sw.onOff(acOffFunc1)


sw.getElementById('heat_switch')
sw.onOn(heatingFunc)
sw.onOff(acOffFunc1)

sw.getElementById('ac_on_off_switch')
sw.onOn(acOnFunc)
sw.onOff(acOffFunc)
//sw.turnOn()

/**
 * curtain
 */
var curtainAnimateTiming = {
    duration: 2000,
    iterations:1,
    from: 1,
    to: 0
}
var testAnimateTiming = {
    duration: 1000,
    iterations:-1,
    from: 1,
    to: 0
}
var curtain_open = 0;
function curtainFrame(params) {

   // console.log('curtainFrame')
    seekbar.getElementById('curtain_bar')
    var animate = seekbar.getAttribute("animate")
   // console.log(animate.progress)
    if (curtain_open==1) {
        seekbar.setAttribute("progress", 1.0-animate.progress)
    } else {
        seekbar.setAttribute("progress", animate.progress)
    }
    
}

function swCurtain(params) {
   // console.log('swCurtain')
    seekbar.getElementById('curtain_bar')
    seekbar.palyAnimate()
    if (curtain_open==1) {
        curtain_open = 0;
    } else {
        curtain_open = 1;
    }
}
tab.getElementById('tabview0')
icon.getElementById('icon123')
function icon123OnPressFunc(params) {
  //  console.log('iconNromalOnPressFunc')
    tab.jump(0);
}
icon.onPress(icon123OnPressFunc)
icon.getElementById('icon1231')
function icon1231OnPressFunc(params) {
   // console.log('iconNromalOnPressFunc')
    tab.jump(1);
}
icon.onPress(icon1231OnPressFunc)

icon.getElementById('icon1232')
function icon1232OnPressFunc(params) {
  //  console.log('iconNromalOnPressFunc')
    tab.jump(2);
}
icon.onPress(icon1232OnPressFunc)

icon.getElementById('icon1233')
function icon1233OnPressFunc(params) {
  //  console.log('iconNromalOnPressFunc')
    tab.jump(3);
}
icon.onPress(icon1233OnPressFunc)

icon.getElementById('icon1234')
function icon1234OnPressFunc(params) {
  //  console.log('iconNromalOnPressFunc')
    tab.jump(4);
}
icon.onPress(icon1234OnPressFunc)
seekbar.getElementById('curtain_bar')
seekbar.setAnimate(curtainFrame, curtainAnimateTiming)

seekbar.getElementById('cool_bar')
seekbar.setAnimate(testFrame, testAnimateTiming)
sw.getElementById('open_switch')
sw.onOn(swCurtain)
sw.onOff(swCurtain)
sw.getElementById('curtain_on_off_switch')
sw.onOn(swCurtain)
sw.onOff(swCurtain)
sw.getElementById('close_switch')
sw.onOn(swCurtain)
sw.onOff(swCurtain)




