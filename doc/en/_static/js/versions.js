/* ============= Support Multiple Versions ============= */
const  versionArr = [];

function fetch_versions()
{
    var fetchURL = "https://docs.realmcu.com/gui/versionlist.txt";
    return fetch(fetchURL)
        .then(res => res.text())
        .then(text => {
            const list = text.split(/[(\r\n)\r\n]+/).filter(item => item.trim().length > 0);
            list.map(item => {
                versionArr.push(item);
            })
    });
}

function gen_version_selector()
{
    let ele = document.getElementById("version-selector");
    ele.innerHTML = `
    ${versionArr.map(item => {
        return `<option value="${item}">${item}</option>`;
    })}
    `;
}

function extract_current_version() {
    const urlpath = window.location.pathname;
    // 正则表达式用于匹配以/开头和结尾的子字符串
    const regex = (substring) => new RegExp(`/${substring}/`);

    for (let i = 0; i < versionArr.length; i++) {
        if (regex(versionArr[i]).test(urlpath)) {
            return versionArr[i];
        }
    }

    return "";
}

function change_version()
{
    var cur_ver = extract_current_version();
    var next_ver = document.getElementById("version-selector").value;
    window.location.href = location.href.replace(cur_ver, next_ver);
}

document.addEventListener('DOMContentLoaded', (event) => {
    fetch_versions().then(() => {
        gen_version_selector();
        var cur_ver = extract_current_version();
        document.getElementById("version-selector").value = cur_ver;
    });
})
