console.log('enter watch')


icon.getElementById('en')

function enOnPressFunc(params) {
    console.log('iconNromalOnPressFunc')
    icon.getElementById('LAMP')
    en = icon.getChildElementByTag('en')
    console.log("att")
    att = en.getAttribute('text')
    console.log(att)
    icon.write(att)
}
icon.onPress(enOnPressFunc)
icon.getElementById('zh-CN')

function zhCNOnPressFunc(params) {
    console.log('iconNromalOnPressFunc')
    icon.getElementById('LAMP')
    en = icon.getChildElementByTag('zh-CN')
    console.log("att")
    att = en.getAttribute('text')
    console.log(att)
    icon.write(att)
}
icon.onPress(zhCNOnPressFunc)
icon.getElementById('it')

function itOnPressFunc(params) {
    console.log('iconNromalOnPressFunc')
    icon.getElementById('LAMP')
    en = icon.getChildElementByTag('it')
    console.log("att")
    att = en.getAttribute('text')
    console.log(att)
    icon.write(att)
}
icon.onPress(itOnPressFunc)
icon.getElementById('ja')

function jaOnPressFunc(params) {
    console.log('iconNromalOnPressFunc')
    icon.getElementById('LAMP')
    en = icon.getChildElementByTag('ja')
    console.log("att")
    att = en.getAttribute('text')
    console.log(att)
    icon.write(att)
}
icon.onPress(jaOnPressFunc)
icon.getElementById('ko')

function koOnPressFunc(params) {
    console.log('iconNromalOnPressFunc')
    icon.getElementById('LAMP')
    en = icon.getChildElementByTag('ko')
    console.log("att")
    att = en.getAttribute('text')
    console.log(att)
    icon.write(att)
}
icon.onPress(koOnPressFunc)
icon.getElementById('es')

function esOnPressFunc(params) {
    console.log('iconNromalOnPressFunc')
    icon.getElementById('LAMP')
    en = icon.getChildElementByTag('es')
    console.log("att")
    att = en.getAttribute('text')
    console.log(att)
    icon.write(att)
}
icon.onPress(esOnPressFunc)
//icon.setAttribute('text', att)