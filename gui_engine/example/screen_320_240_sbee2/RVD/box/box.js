console.log('\n----- Enter box.js ------\n')


// common func
var Id_prefix = 'sw';
function sw_getId(params) {
	var id_str = Id_prefix + params.toString();
	// console.log(id_str)
	return id_str;
}
//-------------------------------------------
// switch io
// console.log('[JS] switch io')
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

var gpio_val_on = 0;
var gpio_val_off = 1;
var gpio_en = true;
function sw_gpio_turnOn(params) {
	// console.log('sw_gpio_turnOn ', params)
	if(gpio_en)
	{
		swArray[params].writeSync(gpio_val_on) 
	}
	// console.log('sw_gpio_turnOn end', params)
}
function sw_gpio_turnOff(params) {
	// console.log('sw_gpio_turnOff ', params)
	if(gpio_en)
	{
		swArray[params].writeSync(gpio_val_off) 
	}
	// console.log('sw_gpio_turnOff end', params)
}
function sw_gpio_en(params) {
	// console.log('sw_gpio_en');
	gpio_en = true;
}

function icon_gpio_turnAllOn(params) {
	// console.log('icon_gpio_turnAllOn ', params)

	swArray[params].writeSync(gpio_val_on)

	gpio_en = false;
	Id_prefix = 'sw_1_';
	for (var i = 0; i < 6; i++){
		sw.getElementById(sw_getId(i))
		sw.turnOn();
	}
	Id_prefix = 'sw_2_';
	for (var i = 0; i < 5; i++){
		sw.getElementById(sw_getId(i))
		sw.turnOn();
	}
}

function icon_gpio_turnAllOff(params) {
	// console.log('icon_gpio_turnAllOn ', params)

	swArray[params].writeSync(gpio_val_off) 

	gpio_en = false;
	Id_prefix = 'sw_1_';
	for (var i = 0; i < 6; i++){
		sw.getElementById(sw_getId(i))
		sw.turnOff();
	}
	Id_prefix = 'sw_2_';
	for (var i = 0; i < 5; i++){
		sw.getElementById(sw_getId(i))
		sw.turnOff();
	}
}
// console.log('[JS] switch init')

// button all on/off
icon.getElementById('btn_all_on')
icon.onClick(icon_gpio_turnAllOn, 11)
icon.getElementById('btn_all_off')
icon.onClick(icon_gpio_turnAllOff, 11)

// switch light
sw.getElementById('sw_1_0')
sw.onOn(sw_gpio_turnOn, 0)
sw.onOff(sw_gpio_turnOff, 0)
sw.onPress(sw_gpio_en)

sw.getElementById('sw_1_1')
sw.onOn(sw_gpio_turnOn, 1)
sw.onOff(sw_gpio_turnOff, 1)
sw.onPress(sw_gpio_en)

sw.getElementById('sw_1_2')
sw.onOn(sw_gpio_turnOn, 2)
sw.onOff(sw_gpio_turnOff, 2)
sw.onPress(sw_gpio_en)

sw.getElementById('sw_1_3')
sw.onOn(sw_gpio_turnOn, 3)
sw.onOff(sw_gpio_turnOff, 3)
sw.onPress(sw_gpio_en)

sw.getElementById('sw_1_4')
sw.onOn(sw_gpio_turnOn, 4)
sw.onOff(sw_gpio_turnOff, 4)
sw.onPress(sw_gpio_en)

sw.getElementById('sw_1_5')
sw.onOn(sw_gpio_turnOn, 5)
sw.onOff(sw_gpio_turnOff, 5)
sw.onPress(sw_gpio_en)

sw.getElementById('sw_2_0')
sw.onOn(sw_gpio_turnOn, 6)
sw.onOff(sw_gpio_turnOff, 6)
sw.onPress(sw_gpio_en)

sw.getElementById('sw_2_1')
sw.onOn(sw_gpio_turnOn, 7)
sw.onOff(sw_gpio_turnOff, 7)
sw.onPress(sw_gpio_en)

// switch plug
sw.getElementById('sw_2_2')
sw.onOn(sw_gpio_turnOn, 8)
sw.onOff(sw_gpio_turnOff, 8)
sw.onPress(sw_gpio_en)

sw.getElementById('sw_2_3')
sw.onOn(sw_gpio_turnOn, 9)
sw.onOff(sw_gpio_turnOff, 9)
sw.onPress(sw_gpio_en)

sw.getElementById('sw_2_4')
sw.onOn(sw_gpio_turnOn, 10)
sw.onOff(sw_gpio_turnOff, 10)
sw.onPress(sw_gpio_en)
// console.log('[JS] end switch')
//-------------------------------------------
// ac control
// ac_displayStatus: 0-off, 1-cold(default), 2-heat
var ac_displayStatus = 1;
function sw_ac_displayControl(params) {
	// console.log('sw_ac_control ', params)
	switch(ac_displayStatus)
	{
		case 0:
			// console.log('ac_displayStatus off')
			win.getElementById('win_ac')
			win.setAttribute("hidden");
			win.getElementById('win_cold')
			win.setAttribute("hidden");
			win.getElementById('win_heat')
			win.setAttribute("hidden");
			break;
		case 1:
			// console.log('ac_displayStatus cold')
			win.getElementById('win_ac')
			win.removeAttribute("hidden");
			win.getElementById('win_cold')
			win.removeAttribute("hidden");
			win.getElementById('win_heat')
			win.setAttribute("hidden");
			break;
		case 2:
			// console.log('ac_displayStatus heating')
			win.getElementById('win_ac')
			win.removeAttribute("hidden");
			win.getElementById('win_cold')
			win.setAttribute("hidden");
			win.getElementById('win_heat')
			win.removeAttribute("hidden");
			break;
		default:
			break;
	}
	// console.log('sw_ac_control end')
}

// ac mode: 0-heating, 1-cold, 2-fan, 3-dry, 4-off(default)
var ac_mode = 4;
var ac_sw_id =[
	'sw_heat',
	'sw_cold',
	'sw_fan',
	'sw_dry'
];

function sw_ac_Oncb(params) {
    // console.log('sw_ac_Oncb ', params)

	// turnoff last switch
	if(ac_mode != 4)
	{
		sw.getElementById(ac_sw_id[ac_mode])
		sw.turnOff();
	}

	ac_mode = params;
	switch(params)
	{
		case 0:
			ac_displayStatus = 2;
			break;
		case 1:
			ac_displayStatus = 1;
			break;
		case 2:
		case 3:
			ac_displayStatus = 0;
			break;
	}
	sw_ac_displayControl(ac_displayStatus);

}
function sw_ac_Offcb(params) {
    // console.log('sw_ac_Offcb ', params)
	// turn off ac
	if(params == ac_mode)
	{
		ac_mode = 4;
		ac_displayStatus = 0;
		sw_ac_displayControl(ac_displayStatus);
	}
}

sw.getElementById('sw_heat')
sw.onOn(sw_ac_Oncb, 0)
sw.onOff(sw_ac_Offcb, 0)

sw.getElementById('sw_cold')
sw.onOn(sw_ac_Oncb, 1)
sw.onOff(sw_ac_Offcb, 1)
// turn on cold mode
sw.turnOn()

sw.getElementById('sw_fan')
sw.onOn(sw_ac_Oncb, 2)
sw.onOff(sw_ac_Offcb, 2)

sw.getElementById('sw_dry')
sw.onOn(sw_ac_Oncb, 3)
sw.onOff(sw_ac_Offcb, 3)
// console.log('[JS] end ac')

//-------------------------------------------
// curtain control
var curtainAnimateTiming = {
    duration: 2000,
    iterations:1,
    from: 1,
    to: 0
}

var cur_actOpen = 0;
// animate[0] is closed, so progress 0% is closed.
function curtainFrame(params) {
    animate = seekbar.animateProgress()
	// console.log('curtainFrame' , animate, ' ', cur_isOpen)
	seekbar.setAttribute("progress", cur_actOpen ? animate : (1.0 - animate))
}

function swCurtain(params) {
    seekbar.palyAnimate()
	cur_actOpen = cur_actOpen ? 0 : 1;
}

// console.log('[JS] curtain')
seekbar.getElementById('skbr_curtain')
seekbar.setAnimate(curtainFrame, curtainAnimateTiming)
sw.getElementById('sw_curtain')
sw.onOn(swCurtain)
sw.onOff(swCurtain)
// console.log('[JS] curtain end')

//-------------------------------------------
//// tab  control
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

//-------------------------------------------
// tab wifi speed
// console.log('[JS] wifi speed')
textbox.getElementById('txt_wifi')
textbox.write('Click')
textbox.getElementById('txt_unit')
textbox.write(' ')

var textAnimateConfig = {
    duration: 2000,
    iterations:-1,
    from: 1,
    to: 0
}
var speed = 888;
var txtPos = {
    x: 140,
    y: 64
}
function textAnimateCallback(params) {
    speed=speed + 1
	
	textbox.getElementById('txt_wifi')
	textbox.write(speed.toString())
}

function startSpeed(params) {
	textbox.getElementById('txt_unit')
	textbox.write('kbit/s')
	
	textbox.getElementById('txt_wifi')
	txtPos.x = 120
	textbox.setPosition(txtPos)
	textbox.setAnimate(textAnimateCallback, textAnimateConfig)
	textbox.palyAnimate()
	wifi.startSpeed()
}
function stopSpeed(params) {
	textbox.getElementById('txt_unit')
	textbox.write(' ')
	
	textbox.getElementById('txt_wifi')
    textbox.setAnimate(textAnimateCallback, textAnimateConfig)
    textbox.pauseAnimate()
	wifi.stopSpeed()
	
	txtPos.x = 140
	textbox.setPosition(txtPos)
	textbox.write('Click')
	speed = 0
}
sw.getElementById('sw_wifi')
sw.onOn(startSpeed)
sw.onOff(stopSpeed)

console.log('[JS] end')
