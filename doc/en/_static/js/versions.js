/* ============= Support Multiple Versions ============= */

function add_version_selector()
{
    return fetch("https://docs.realmcu.com/HoneyGUI/versionlist.txt")
        .then(res => res.text())
        .then(text => {
            const versions = text.split(/[(\r\n)\r\n]+/).filter(version => version.trim().length > 0);
            let ele = document.getElementById("version-selector");
            ele.innerHTML = `
            ${versions.map(version => {
                return `<option value="${version}">${version}</option>`;
            })}
            `;
    });
}

function change_version()
{
    var cur_ver = window.location.pathname.split('/')[3];
    var next_ver = document.getElementById("version-selector").value;
    window.location.href = location.href.replace(cur_ver, next_ver);
}

document.addEventListener('DOMContentLoaded', (event) => {
    add_version_selector().then(() => {
        var cur_ver = window.location.pathname.split('/')[3];
        document.getElementById("version-selector").value = cur_ver;
    });
})