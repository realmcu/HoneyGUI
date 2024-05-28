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
var plug1 = new Gpio(8, 'out');
var plug2 = new Gpio(9, 'out');
var plug3 = new Gpio(10, 'out');
//console.log(LED1.gpio+';'+LED1.direction)

var sleep_flag = true;
function sleepflagTrue(params) {
    
    sleep_flag = true;
    //console.log('sleep_flag;'+sleep_flag)
}
// var sh_test = new SMTHM;

function led1OnFunc(params) {
    //console.log('led1OnFunc')
    //console.log('sleep_flag;'+sleep_flag)
    if (sleep_flag) {
        LED1.writeSync(0)
    }
    // sh_test.test();
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
//plug
function plug1OnFunc(params) {
    //console.log('led3OnFunc')
    
    if (sleep_flag) {
        plug1.writeSync(0)
    }
}
function plug1OffFunc(params) {
    //console.log('led3OffFunc')
    
    if (sleep_flag) {
        plug1.writeSync(1)
    }
}
function plug2OnFunc(params) {
    //console.log('led3OnFunc')
    
    if (sleep_flag) {
        plug2.writeSync(0)
    }
}
function plug2OffFunc(params) {
    //console.log('led3OffFunc')
    
    if (sleep_flag) {
        plug2.writeSync(1)
    }
}
function plug3OnFunc(params) {
    //console.log('led3OnFunc')
    
    if (sleep_flag) {
        plug3.writeSync(0)
    }
}
function plug3OffFunc(params) {
    //console.log('led3OffFunc')
    
    if (sleep_flag) {
        plug3.writeSync(1)
    }
}
function lampSwitchOnFunc(params) {
    sleep_flag = false;
    //console.log('lampSwitchOnFunc')
    sw.getElementById('kitchen_switch')
    sw.turnOn()
	LED1.writeSync(0)
    sw.getElementById('parlor_switch')
    sw.turnOn()
	LED2.writeSync(0)
    sw.getElementById('bedroom_switch')
    sw.turnOn()   
	LED3.writeSync(0)
	sw.getElementById('bedroom_switch1')
    sw.turnOn()
	LED4.writeSync(0)
    sw.getElementById('bedroom_switch2')
    sw.turnOn()
	LED5.writeSync(0)
    sw.getElementById('porch_switch')
    sw.turnOn() 
	LED6.writeSync(0)	
    sw.getElementById('bedroom_off')
    sw.turnOn()
	LED7.writeSync(0)
    sw.getElementById('children')
    sw.turnOn()
	LED8.writeSync(0)	
    sw.getElementById('socket1')
    sw.turnOn()
	plug1.writeSync(0)	
    sw.getElementById('socket3')
    sw.turnOn()
	plug2.writeSync(0)	
    sw.getElementById('switch0')
    sw.turnOn()
	plug3.writeSync(0)	
    //LED9.writeSync(0) 
}
function lampSwitchOffFunc(params) {
    sleep_flag = false;
    //console.log('lampSwitchOffFunc')
    sw.getElementById('kitchen_switch')
    sw.turnOff()
	LED1.writeSync(1)
    sw.getElementById('parlor_switch')
    sw.turnOff()
	LED2.writeSync(1)
    sw.getElementById('bedroom_switch')
    sw.turnOff()
	LED3.writeSync(1)	
	sw.getElementById('bedroom_switch1')
    sw.turnOff()
	LED4.writeSync(1)
    sw.getElementById('bedroom_switch2')
    sw.turnOff()
	LED5.writeSync(1)
    sw.getElementById('porch_switch')
    sw.turnOff()
	LED6.writeSync(1)	
    sw.getElementById('bedroom_off')
    sw.turnOff()
	LED7.writeSync(1)
    sw.getElementById('children')
    sw.turnOff()
	LED8.writeSync(1)	
    sw.getElementById('socket1')
    sw.turnOff()
	plug1.writeSync(1)	
    sw.getElementById('socket3')
    sw.turnOff()
	plug2.writeSync(1)	
    sw.getElementById('switch0')
    sw.turnOff()
	plug3.writeSync(1)	
    //LED9.writeSync(1) 

}
sw.getElementById('kitchen_switch')
sw.switch_on(led1OnFunc)
sw.switch_off(led1OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('parlor_switch')
sw.switch_on(led2OnFunc)
sw.switch_off(led2OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('bedroom_switch')
sw.switch_on(led3OnFunc)
sw.switch_off(led3OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('bedroom_switch1')
sw.switch_on(led4OnFunc)
sw.switch_off(led4OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('bedroom_switch2')
sw.switch_on(led5OnFunc)
sw.switch_off(led5OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('porch_switch')
sw.switch_on(led6OnFunc)
sw.switch_off(led6OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('bedroom_off')
sw.switch_on(led7OnFunc)
sw.switch_off(led7OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('children')
sw.switch_on(led8OnFunc)
sw.switch_off(led8OffFunc)
sw.onPress(sleepflagTrue)

sw.getElementById('socket1')
sw.switch_on(plug1OnFunc)
sw.switch_off(plug1OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('switch0')
sw.switch_on(plug2OnFunc)
sw.switch_off(plug2OffFunc)
sw.onPress(sleepflagTrue)
sw.getElementById('socket3')
sw.switch_on(plug3OnFunc)
sw.switch_off(plug3OffFunc)
sw.onPress(sleepflagTrue)
icon.getElementById('allturnon')
icon.onClick(lampSwitchOnFunc)
icon.getElementById('sleep')
icon.onClick(lampSwitchOffFunc)
/**
 * ac
 */
console.log('ac start')
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
 
function acOnFunc(params) {
   // console.log('heatingFunc')
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
sw.switch_on(coolingFunc)
sw.switch_off(acOffFunc1)


sw.getElementById('heat_switch')
sw.switch_on(heatingFunc)
sw.switch_off(acOffFunc1)

sw.getElementById('ac_on_off_switch')
sw.switch_on(acOnFunc)
sw.switch_off(acOffFunc)
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
seekbar.getElementById('curtain_bar')
function curtainFrame(params) {
   // console.log('curtainFrame')
     animate= seekbar.animateProgress()
    if (curtain_open==1) {
        seekbar.setAttribute("progress", animate)
    } else {
        seekbar.setAttribute("progress", 1.0-animate)
    }
}

function swCurtain(params) {
   // console.log('swCurtain')
    seekbar.palyAnimate()
    if (curtain_open==1) {
        curtain_open = 0;
    } else {
        curtain_open = 1;
    }
}
seekbar.setAnimate(curtainFrame, curtainAnimateTiming)
sw.getElementById('open_switch')
sw.switch_on(swCurtain)
sw.switch_off(swCurtain)
sw.getElementById('curtain_on_off_switch')
sw.switch_on(swCurtain)
sw.switch_off(swCurtain)
sw.getElementById('close_switch')
sw.switch_on(swCurtain)
sw.switch_off(swCurtain)


// reset button
icon.getElementById('resetBtn')
function resetPressFunc(params) {
    //console.log('resetPressFunc')
    matter.dataRst();
}
icon.onHold(resetPressFunc)



//-------------------------------------------
//// tab  control

// common func
var Id_prefix = 'sw';
function sw_getId(params) {
	var id_str = Id_prefix + params.toString();
	// console.log(id_str)
	return id_str;
}

sw.getElementById('sw_tab0')
sw.turnOn()

tab.getElementById('tabview0')
var tabJump = {
    cur_tab_x: 0,
    nxt_tab_x: 0,
	cur_tab_y: 0,
    nxt_tab_y: 0
}

function sw_turnOff(params) {
	Id_prefix = 'sw_tab';
	sw.getElementById(sw_getId(params));
	sw.turnOff()
}
function sw_turnOn(params) {
	Id_prefix = 'sw_tab';
	sw.getElementById(sw_getId(params));
	sw.turnOn()
}
function sw_jump_turnoff(params) {
	Id_prefix = 'sw_tab';
	sw.getElementById(sw_getId(tabJump.cur_tab_x));
	sw.turnOff()
}

function sw_jump_tab(params) {
	// console.log('jump', params)
	tabJump.nxt_tab_x = params
	
	if(tabJump.nxt_tab_x != tabJump.cur_tab_x)
	{
		sw_jump_turnoff();
		tab.jump(params);
		tabJump.cur_tab_x = tabJump.nxt_tab_x
	}
}

function sw_jump_keep_on(params) {
	// console.log('sw_jump_keep_on ', params)
	Id_prefix = 'sw_tab';
	if(params == tabJump.nxt_tab_x)
	{
		sw.getElementById(sw_getId(params));
		sw.turnOn();
	}
}

function tab_slide(params) {
	// console.log('tab_slide')
	var cur_tab = tab.getCurTab()

	tabJump.nxt_tab_x = cur_tab.x;
	sw_turnOn(tabJump.nxt_tab_x);
	sw_turnOff(tabJump.cur_tab_x);
	tabJump.cur_tab_x = cur_tab.x;
}

// console.log('[JS] change')
// tab change
tab.onChange(tab_slide)

// console.log('[JS] jump')
// jump tab0
sw.getElementById('sw_tab0')
sw.switch_on(sw_jump_tab, 0)
sw.switch_off(sw_jump_keep_on, 0)

// jump tab1
sw.getElementById('sw_tab1')
sw.switch_on(sw_jump_tab, 1)
sw.switch_off(sw_jump_keep_on, 1)

// jump tab2
sw.getElementById('sw_tab2')
sw.switch_on(sw_jump_tab, 2)
sw.switch_off(sw_jump_keep_on, 2)

// jump tab3
sw.getElementById('sw_tab3')
sw.switch_on(sw_jump_tab, 3)
sw.switch_off(sw_jump_keep_on, 3)

// jump tab4
sw.getElementById('sw_tab4')
sw.switch_on(sw_jump_tab, 4)
sw.switch_off(sw_jump_keep_on, 4)

// jump tab5
sw.getElementById('sw_tab5')
sw.switch_on(sw_jump_tab, 5)
sw.switch_off(sw_jump_keep_on, 5)

// console.log('[JS] end jump')


///////////////////////////////////
console.log('wifi speed')

textbox.getElementById('txt_click')
textbox.write('Click')
textbox.getElementById('txt_speed')
textbox.write(' ')
textbox.getElementById('txt_unit')
textbox.write(' ')
var textAnimateConfig = {
    duration: 2000,
    iterations:-1,
    from: 1,
    to: 0
}
var speed = 888;
function textAnimateCallback(params) {
    textbox.getElementById('txt_speed')
    speed=wifi.readSpeed()
    // textbox.write(wifi.readSpeed())
	textbox.write(speed.toString())
}

function startSpeed(params) {
    textbox.getElementById('txt_click')
    textbox.write(' ')
    textbox.getElementById('txt_unit')
    textbox.write('Kbit/s')

    textbox.getElementById('txt_speed')
    textbox.setAnimate(textAnimateCallback, textAnimateConfig)
    textbox.palyAnimate()
    wifi.startSpeed()
}
function stopSpeed(params) {
    textbox.getElementById('txt_speed')
    textbox.setAnimate(textAnimateCallback, textAnimateConfig)
    textbox.pauseAnimate()
	wifi.stopSpeed()
	speed = 0

    textbox.getElementById('txt_click')
    textbox.write('Click')
    textbox.getElementById('txt_speed')
    textbox.write(' ')
    textbox.getElementById('txt_unit')
    textbox.write(' ')
}
sw.getElementById('sw_wifi')
sw.switch_on(startSpeed)
sw.switch_off(stopSpeed)
console.log('end wifi speed')

//-------------------------------------------
// wifi sync switch

var sh_wifi = new SMTHM;

var all_sw_id =[
	'kitchen_switch',
	'parlor_switch',
	'bedroom_switch',
	'bedroom_switch1',
    'bedroom_switch2',
    'porch_switch',
	'bedroom_off',
	'children',
	'socket1',
    'switch0',
    'socket3'
];

function Wifi_sw(params) {
    // console.log('enter  Wifi_sw')

    var idx = params.idx;
    var val = params.val;

    // console.log(idx, '  ', val, ' ', all_sw_id[idx])

    sw.getElementById(all_sw_id[idx]);
    if(val)
    {
        sw.turnOn()
    }
    else
    {
        sw.turnOff()
    }

}
sh_wifi.OnSyncSW(Wifi_sw);

console.log('end js')