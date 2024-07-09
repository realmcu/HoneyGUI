/* ============= Support Multiple Versions ============= */

function add_version_selector()
{
    return fetch("https://docs.realmcu.com/HoneyGUI/versionlist.txt")
        .then(res => res.text())
        .then(text => {
            const versions = text.split(/[(\r\n)\r\n]+/).filter(version => version.trim().length > 0);
            let p = document.getElementById("rtd-search-form").parentElement;
            p.innerHTML = `
            <select name="versions" id="versions" title="versions" onchange="change_version()" style="border-radius:5px; margin-bottom:15px">
            ${versions.map(version => {
                return `<option value="${version}">${version}</option>`;
            })}
            </select>` + p.innerHTML;
    });
}

function change_version()
{
    var cur_ver = window.location.pathname.split('/')[3];
    var next_ver = document.getElementById("versions").value;
    window.location.href = location.href.replace(cur_ver, next_ver);
}

document.addEventListener('DOMContentLoaded', (event) => {
    add_version_selector().then(() => {
        var cur_ver = window.location.pathname.split('/')[3];
        document.getElementById("versions").value = cur_ver;
    });
})