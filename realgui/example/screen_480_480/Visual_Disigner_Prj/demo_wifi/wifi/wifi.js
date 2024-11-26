
console.log('demo:wifi connect')

// widget id prefix
var Id_prefix_win = 'win';
var Id_prefix_btn = 'btn_';
var Id_prefix_ssid = 'wifi_';
var Id_prefix_rssi = 'img_rssi_';
var Id_prefix_lock = 'img_lk_';

// file
var Path_dir_wifi = '/app/wifi/resource/wifi/'
var Path_dir_loading = '/app/wifi/resource/loading/'

// var
var wifi_scan_num = 0;
var wifi_connect_index = 0;
var wifi_ssid_tab = [];
var wifi_rssi_tab = [];
var wifi_scrt_tab = [];  // 1/0


//--------------------------------------------------
// init UI
page.getElementById('page_wifi')
page.setRebound(true)

var num_wifi = 8;
var txt_pos_ssid={
    x: 60,
    y: 25,
}
for(var i=0; i < num_wifi; i++)
{
    id = Id_prefix_btn + (i>9?null:'0') + i.toString()
    // console.log(id)
    icon.getElementById(id)
    icon.setTxtPos(txt_pos_ssid)

    id = Id_prefix_win + i.toString()
    // console.log(id)
    win.getElementById(id)
    win.setShow(0);
}

win.getElementById('win_save')
win.setShow(0);
icon.getElementById('btn_save')
icon.setTxtPos(txt_pos_ssid)

win.getElementById('win8')
win.setShow(0);
win.getElementById('win_con')
win.setShow(0);

img.getElementById('img_con')
img.setBlock(1)

img.getElementById('img_bg')
img.setBlock(1)
img.setShow(0);

img.getElementById('img_loading')
img.setShow(0);




// loading animate init
var loadingAnimateConfig = {
    duration: 500,
    iterations: -1,
    from: 0,
    to: 1
}

var loading_per = 0;
var loading_cnt = 0;
function imgAnimateLoading(params) {
    loading_cnt ++;
    loading_per = Math.floor(loading_cnt / 10) % 18;
    // console.log('imgAnimateLoading', loading_per)
    loading_setImg(loading_per);
}
img.getElementById('img_loading')
img.setAnimate(imgAnimateLoading, loadingAnimateConfig)
//--------------------------------------------------

var turnOn_cb_test;
var Scan_cb_test;
var turnOff_cb_test;
var Connect_cb_test;

turnOn_cb_test = function()
{
    wifi.test(0);
}
turnOff_cb_test = function()
{
    wifi.test(2);
}
Scan_cb_test = function()
{
    wifi.test(3);
}
Connect_cb_test = function()
{
    wifi.test(4);
}

// config
console.log('sw wifi')

// common
function wifi_loading_start()
{
    img.getElementById('img_bg')
    img.setShow(1)

    img.getElementById('img_loading')
    loading_setImg(0)
    img.setShow(1)
    img.playAnimate()
}

function wifi_loading_end()
{
    img.getElementById('img_bg')
    img.setShow(0)

    img.getElementById('img_loading')
    img.setShow(0)
    img.pauseAnimate()
    loading_per = 0;
}

function rssi_setImg(id, rssi)
{
    console.log(id, rssi)
    rssi = (rssi >= -55)? 4: (rssi >= -77)? 3: (rssi >= -88)? 2: 1
    img.getElementById(id)
    img.setImg(Path_dir_wifi + 'rssi_' + rssi.toString() + '.bin')
}

function loading_setImg(percent)
{
    img.getElementById('img_loading')
    img.setImg(Path_dir_loading + 'loading_000' + ((loading_per < 10)? '0':'') + loading_per.toString() + '.bin')
}

function wifi_all_hiden()
{
    for(var i=0; i < num_wifi; i++)
    {
        id = Id_prefix_win + i.toString()
        win.getElementById(id)
        win.setShow(0);
    }
}


//--------------------------------------------------
// action cb
// turn ON/OFF
function sw_wifi_turnOn()
{
    wifi.turnOn();

    // test
    // setTimeout(turnOn_cb_test, 500)
}
function sw_wifi_turnOff()
{
    wifi.turnOff();

    wifi_num = 0;
    wifi_ssid_tab = [];
    wifi_rssi_tab = [];
    wifi_scrt_tab = [];

    // test
    // setTimeout(turnOff_cb_test, 500)
}


sw.getElementById('sw_wifi')
sw.onOn(sw_wifi_turnOn)
sw.onOff(sw_wifi_turnOff)


// scan
function icon_wifi_scan()
{
    wifi_all_hiden();
    wifi.scan();

    // test
    // setTimeout(Scan_cb_test, 500)
}
icon.getElementById('btn_scan')
icon.onClick(icon_wifi_scan)



//--------------------------------------------------
// disconnect
function icon_wifi_disconnect(param)
{
    win.getElementById('win_save')
    win.setShow(0)

    wifi.disconnect()
}
icon.getElementById('btn_save')
icon.onClick(icon_wifi_disconnect)

// connect
function wifi_connect_box(param)
{
    console.log(param, wifi_ssid_tab[param])

    img.getElementById('img_bg')
    img.setShow(1)

    wifi_connect_index = param;
    wifi.setConIndex(wifi_connect_index)
    if(wifi_scrt_tab[param] == 0)
    {
        
        wifi.connect();
        wifi_loading_start();

        // test
        // setTimeout(Connect_cb_test, 5000)
    }
    else
    {
        wifi.initPasswd();
        textbox.getElementById('txt_ssid')
        textbox.write(wifi_ssid_tab[param])
        textbox.getElementById('txt_pw')
        textbox.write(' ')
        textbox.getElementById('txt_err')
        textbox.setShow(0)
    
        win.getElementById('win_con')
        win.setShow(1);
    }

}

for(var i=0; i < num_wifi; i++)
{
    id = Id_prefix_btn + (i>9?null:'0') + i.toString()
    // console.log(id)
    icon.getElementById(id)
    icon.onClick(wifi_connect_box, i);
}



// message box: connect
function icon_wifi_con()
{
    wifi.connect();

    win.getElementById('win_con')
    win.setShow(0);

    wifi_loading_start();

    // test
    // setTimeout(Connect_cb_test, 1000)
}
function icon_wifi_can()
{
    win.getElementById('win_con')
    win.setShow(0);
    img.getElementById('img_bg')
    img.setShow(0)
}
icon.getElementById('btn_con')
icon.onClick(icon_wifi_con);
icon.getElementById('btn_can')
icon.onClick(icon_wifi_can);



function pswd_visible(param)
{
    wifi.setPasswdVisible('txt_pw', param)
}
sw.getElementById('sw_visib')
sw.onOn(pswd_visible, 1)
sw.onOff(pswd_visible, 0)



function wifi_setPasswd()
{
    console.log('to get pass wd')
    textbox.getElementById('txt_err')
    textbox.setShow(0);
    
    wifi.setConPasswd()
}
textbox.getElementById('txt_pw')
textbox.setPasswd(wifi_setPasswd)



//--------------------------------------------------
// msg cb
function wifi_on_cb(param)
{
    console.log(param)
    if(param == 0)
    {
        win.getElementById('win8')
        win.removeAttribute("hidden");
        wifi.scan();

        // test
        // setTimeout(Scan_cb_test, 500)
    }

}
function wifi_off_cb(param)
{
    console.log(param)
    if(param == 0)
    {
        for (var i = 0; i <= 8; i++){
            win.getElementById('win' + i)
            win.setAttribute("hidden");
        }
        win.getElementById('win_save')
        win.setAttribute("hidden");
    }

}

function wifi_scan_cb(param)
{
    wifi_scan_num = param.cnt;
    console.log(wifi_scan_num)
    wifi_ssid_tab = [];
    wifi_rssi_tab = [];
    wifi_scrt_tab = [];
    for (var i = 0; i < wifi_scan_num; i++){
        console.log(i, param.ssid[i], param.rssi[i], param.scrt[i])
        wifi_ssid_tab.push(param.ssid[i])
        wifi_rssi_tab.push(param.rssi[i])
        wifi_scrt_tab.push(param.scrt[i]>0? 1:0)
    }

    var wifi_num_display = (wifi_scan_num < num_wifi)? wifi_scan_num:num_wifi;
    for (var i = 0; i < wifi_num_display; i++)
    {
        id = Id_prefix_btn + (i>9?null:'0') + i.toString()
        console.log(id)
        icon.getElementById(id)
        icon.write(wifi_ssid_tab[i])

        id = Id_prefix_lock + (i>9?null:'0') + i.toString()
        console.log(id)
        img.getElementById(id)
        img.setShow(wifi_scrt_tab[i]>0? 1:0)

        id = Id_prefix_rssi + (i>9?null:'0') + i.toString()
        rssi_setImg(id, wifi_rssi_tab[i])
        
        id = Id_prefix_win + i.toString()
        console.log(id)
        win.getElementById(id)
        win.setShow(1);

    }


}
function wifi_connect_cb(param)
{
    console.log(param)

    if(param == 0)
    {
        var id = 'btn_save'
        icon.getElementById(id)
        icon.write(wifi_ssid_tab[wifi_connect_index])

        id = 'img_rssi'
        rssi_setImg(id, wifi_rssi_tab[wifi_connect_index])

        win.getElementById('win_save')
        win.setShow(1);
        wifi_all_hiden();
    }
    else
    {
        textbox.getElementById('txt_err')
        textbox.setShow(1);
    }

    wifi_loading_end();
}
function wifi_disconnect_cb(param)
{
    console.log(param)

    win.getElementById('win_save')
    win.setShow(0);
}
wifi.onOnDone(wifi_on_cb)
wifi.onOffDone(wifi_off_cb)
wifi.onScanDone(wifi_scan_cb)
wifi.onConnectDone(wifi_connect_cb)
wifi.onDisconnect(wifi_disconnect_cb)
//--------------------------------------------------





console.log('demo end')