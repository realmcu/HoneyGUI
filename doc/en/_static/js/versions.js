let targetsData = {};
let hasTarget;
let targetSelector;
let versionSelector;

function parseUrlParts() {
    const urlParts = window.location.pathname.toLowerCase().split('/');
    let currentTarget = null;
    let currentVersion = null;

    if (hasTarget) {
        currentTarget = Object.keys(targetsData).find(target => 
            urlParts.includes(target.toLowerCase())
        );
        if (currentTarget) {
            currentVersion = targetsData[currentTarget].find(version => 
                urlParts.includes(version.toLowerCase())
            );
        }
    } else {
        currentVersion = targetsData.find(version => 
            urlParts.includes(version.toLowerCase())
        );
    }

    return { currentTarget, currentVersion };
}

// 当目标选择改变
function change_target() {
    const { currentTarget, currentVersion } = parseUrlParts();
    const selectedTarget = targetSelector.value;
    const versionsList = targetsData[selectedTarget] || [];
    const defaultVersion = versionsList.length > 0 ? versionsList[0] : "latest";

    let newUrl = window.location.href;
    newUrl = newUrl.replace(/\/(cn|en)\//i, '/en/');
    if (currentTarget) {
        newUrl = newUrl.replace(new RegExp(currentTarget, 'i'), selectedTarget);
    }
    if (currentVersion) {
        newUrl = newUrl.replace(new RegExp(currentVersion, 'i'), defaultVersion);
    }
    newUrl = newUrl.replace(new RegExp(`/${defaultVersion}/.*`, 'i'), `/${defaultVersion}/index.html`);
    window.location.href = newUrl;
}

// 当版本选择改变
function change_version() {
    const { currentVersion } = parseUrlParts();
    const selectedVersion = versionSelector.value;
    let newUrl = window.location.href;
    if (currentVersion) {
        newUrl = newUrl.replace(new RegExp(currentVersion, 'i'), selectedVersion);
    }
    window.location.href = newUrl;
}

// 批量填充选择器选项
function populateSelector(selector, items, defaultValue) {
    let optionsHTML = items.map(item => 
        `<option value="${item}" ${item === defaultValue ? 'selected' : ''}>${item}</option>`).join('');
    selector.innerHTML = optionsHTML;
}

// 更新版本选择器
function updateVersionSelector(target, defaultVersion) {
    const versions = hasTarget ? targetsData[target] || [] : targetsData;
    populateSelector(versionSelector, versions, defaultVersion);
}

// 初始化选择器
function initSelectors(targetValue, versionValue) {
    if (hasTarget) {
        const targetKeys = Object.keys(targetsData);
        populateSelector(targetSelector, targetKeys, targetValue);
        updateVersionSelector(targetValue, versionValue);
    } else {
        populateSelector(versionSelector, targetsData, versionValue);
    }
}

async function fetchVersionData() {
    let fetchUrl = "https://docs.realmcu.com/sdk/version.txt";
    try {
        const response = await fetch(fetchUrl);
        const text = await response.text();
        
        if (hasTarget) {
            targetsData = JSON.parse(text);
        } else {
            targetsData = text.split(/[\r\n]+/).map(item => item.trim()).filter(Boolean);
        }

        const { currentTarget, currentVersion } = parseUrlParts();
        initSelectors(currentTarget, currentVersion);
    } catch (error) {
        console.error('Fail to fetch data:', error);
    }
}

/* ============= Init version selector ============= */
document.addEventListener('DOMContentLoaded', (event) => {
	hasTarget = window.isMultitarget == "True";
    targetSelector = document.getElementById("target-selector");
    versionSelector = document.getElementById("version-selector");
    fetchVersionData();
})
