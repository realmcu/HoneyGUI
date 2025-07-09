
function open_click(name,name2) {
    console.log(name+"/"+name2);
    app.open(name+"/"+name2)
}
function getXml(params ) {
    fs.opendir(params, (err,dir) => {
        while(dirent = dir.readSync()) {
            if(dirent.isFile()) {
                if(dirent.name.match(/.xml/i))
                {
                    iconlist.getElementById("ic_launcher");
                    iconlist.append(dirent.name);
                    icon.getElementById(dirent.name);
                    icon.write(dirent.name);
                    icon.onClick(open_click, params, dirent.name);
                }
            }
        }
        dir.close(err => {
            if(err) {
                //console.error(err);
            }
        })
    })
}
var xmlname = new Array();
fs.opendir('app', (err,dir) => {
    while(dirent = dir.readSync()) {
        if(dirent.isDirectory()) {
            if ((dirent.name !== ".") && (dirent.name !== "..") && (dirent.name !== "system")) {
                xmlname.push(dirent.name);
            }
        }
    }
    dir.close(err => {
        if(err) {
            //console.error(err)
        }
    })
})
xmlname.forEach(element => {
    s = 'app/'+element;
    getXml(s);
});
