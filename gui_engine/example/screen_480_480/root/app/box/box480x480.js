console.log('enter 86box')

// var sh = new SMTHM;


//-------------------------------------------
// device switch io

var Pin_sw = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 49052];
var swArray = [
	new Gpio(Pin_sw[0], 'out'),
	new Gpio(Pin_sw[1], 'out'),
	new Gpio(Pin_sw[2], 'out'),
	new Gpio(Pin_sw[3], 'out'),
	new Gpio(Pin_sw[4], 'out'),
	new Gpio(Pin_sw[5], 'out'),
	new Gpio(Pin_sw[6], 'out'),
	new Gpio(Pin_sw[7], 'out'),
	new Gpio(Pin_sw[8], 'out'),
	new Gpio(Pin_sw[9], 'out'),
	new Gpio(Pin_sw[10], 'out'),
	new Gpio(Pin_sw[11], 'out'),
  ];

var all_sw_id =[
	'sw_living',
	'sw_dining',
	'sw_study',
	'sw_wash',
    'sw_kitchen',
    'sw_bath',
	'sw_bed',
	'sw_child',
	'sw_skt_0',
    'sw_skt_1',
    'sw_skt_2'
];

var Id_prefix_sw_sub = 'sw_sub_';
var Id_prefix_sw_img = 'sw_img_';
var gpio_val_on = 0;
var gpio_val_off = 1;

function sw_device_online(params) {
    sw.getElementById(all_sw_id[params])
    sw.setShow(1)
    
    img.getElementById(Id_prefix_sw_img + params.toString())
    img.setShow(0)
}
function sw_device_offline(params) {
    img.getElementById(Id_prefix_sw_img + params.toString())
    img.setShow(1)
    
    sw.getElementById(all_sw_id[params])
    sw.setShow(0)
}
function sw_device_turnOn(params) {
	// console.log('sw_gpio_turnOn ', params)
    swArray[params].writeSync(gpio_val_on) 
    // swArray[params].state = 1;
}

function sw_device_turnOff(params) {
	// console.log('sw_gpio_turnOff ', params)
    swArray[params].writeSync(gpio_val_off) 
    // swArray[params].state = 0;
}

// switch light & socket init
for (var i = 0; i < 11; i++){
    sw.getElementById(all_sw_id[i]);
    // console.log(all_sw_id[i])

    sw_device_online(i)
    sw.onOn(sw_device_turnOn, i)
    sw.onOff(sw_device_turnOff, i)
}


// button all on/off
var gpio_val_group_on = 0x10;  
var gpio_val_group_off = 0x11; 
function icon_gpio_turnAllOn(params) {
	// console.log('icon_gpio_turnAllOn ', params)
	swArray[params].writeSync(gpio_val_group_on)

	for (var i = 0; i < 11; i++){
		sw.getElementById(all_sw_id[i])
		// sw.turnOn();
        sw.setState(1);
        // swArray[params].state = 1;
	}
}

function icon_gpio_turnAllOff(params) {
	// console.log('icon_gpio_turnAllOn ', params)
	swArray[params].writeSync(gpio_val_group_off) 

	for (var i = 0; i < 11; i++){
		sw.getElementById(all_sw_id[i])
		// sw.turnOff();
        sw.setState(0);
        // swArray[params].state = 0;
	}

}

icon.getElementById('btn_allon')
icon.onClick(icon_gpio_turnAllOn, 11)
icon.getElementById('btn_alloff')
icon.onClick(icon_gpio_turnAllOff, 11)


// sw subscribe manage
var gpio_val_sub = 0x20;   
var gpio_val_unsub = 0x30; 
function sw_sub_turnOn(params) {
	// console.log('sw_gpio_turnOn ', params)
    swArray[params].writeSync(gpio_val_sub) 
}

function sw_sub_turnOff(params) {
	// console.log('sw_gpio_turnOff ', params)
    swArray[params].writeSync(gpio_val_unsub) 
}

for (var i = 0; i < 11; i++){
    var id_str = Id_prefix_sw_sub + i.toString();
    sw.getElementById(id_str);
    console.log(id_str)

    // default state is ON 
    sw.onOn(sw_sub_turnOff, i)
    sw.onOff(sw_sub_turnOn, i)
}


console.log('[JS] end switch')
//-------------------------------------------
// ac control

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
    if(params == acStatus)
    {
        if(params == 1)
        {
            sw.getElementById('cool_switch')
        }
        else
        {
            sw.getElementById('heat_switch')
        }
        sw.setState(1)
    }
}
sw.getElementById('cool_switch')
sw.onOn(coolingFunc)
sw.onOff(acOffFunc1, 1)
sw.setState(1)


sw.getElementById('heat_switch')
sw.onOn(heatingFunc)
sw.onOff(acOffFunc1, 2)

sw.getElementById('ac_on_off_switch')
sw.onOn(acOnFunc)
sw.onOff(acOffFunc)
sw.setState(1)
//sw.turnOn()

/**
 * curtain
 */
var curtainAnimateTiming = {
    duration: 2000,
    iterations:0,
    from: 1,
    to: 0
}
var cur_actOpen = 0;
seekbar.getElementById('curtain_bar')
function curtainFrame(params) {
   // console.log('curtainFrame')
    seekbar.getElementById('curtain_bar')
    var animate = seekbar.animateProgress()
    seekbar.setAttribute("progress", cur_actOpen ?  (1.0 - animate) : animate)
}

function swCurtain(params) {
   // console.log('swCurtain')
    seekbar.getElementById('curtain_bar')
    cur_actOpen = cur_actOpen ? 0 : 1;
    seekbar.playAnimate()

    sw.getElementById('open_switch')
    sw.setState(cur_actOpen)
    sw.getElementById('close_switch')
    sw.setState(cur_actOpen)
    sw.getElementById('curtain_on_off_switch')
    sw.setState(cur_actOpen)
}
seekbar.setAttribute("progress", 1) // closed
seekbar.setAnimate(curtainFrame, curtainAnimateTiming)
sw.getElementById('open_switch')
sw.onOn(swCurtain)
sw.onOff(swCurtain)
sw.getElementById('curtain_on_off_switch')
sw.onOn(swCurtain)
sw.onOff(swCurtain)
sw.getElementById('close_switch')
sw.onOn(swCurtain)
sw.onOff(swCurtain)


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
sw.onOn(sw_jump_tab, 0)
sw.onOff(sw_jump_keep_on, 0)

// jump tab1
sw.getElementById('sw_tab1')
sw.onOn(sw_jump_tab, 1)
sw.onOff(sw_jump_keep_on, 1)

// jump tab2
sw.getElementById('sw_tab2')
sw.onOn(sw_jump_tab, 2)
sw.onOff(sw_jump_keep_on, 2)

// jump tab3
sw.getElementById('sw_tab3')
sw.onOn(sw_jump_tab, 3)
sw.onOff(sw_jump_keep_on, 3)

// jump tab4
sw.getElementById('sw_tab4')
sw.onOn(sw_jump_tab, 4)
sw.onOff(sw_jump_keep_on, 4)

// jump tab5
sw.getElementById('sw_tab5')
sw.onOn(sw_jump_tab, 5)
sw.onOff(sw_jump_keep_on, 5)

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
    duration:  40,
    iterations:-1,
    from: 1,
    to: 0
}
var speed = 888;
function textAnimateCallback(params) {
    textbox.getElementById('txt_speed')
    speed = wifi.readSpeed()
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
    textbox.playAnimate()
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
sw.onOn(startSpeed)
sw.onOff(stopSpeed)
console.log('end wifi speed')

//-------------------------------------------
// wifi sync switch

var sh = new SMTHM;

function IOT_sw(params) {
    // console.log('enter  Wifi_sw')

    var idx = params.idx;
    var val = params.val;

    // console.log(idx, '  ', val, ' ', all_sw_id[idx])

    sw.getElementById(all_sw_id[idx]);
    if(val == 0 || val == 1) // on/off
    {
        sw.setState(val);
        sw_device_online(idx);
    }
    else if(val == 2) // offline
    {
        sw_device_offline(idx);
    }
    else if(val == 3) // online
    {
        sw_device_online(idx);
    }

}
sh.OnSyncSW(IOT_sw);

console.log('end js')