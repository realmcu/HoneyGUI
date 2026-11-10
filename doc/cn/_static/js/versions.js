/* ============= 状态管理 ============= */
const docConfig = {
    isMultiTarget: false,
    fetchedData: {},
};
const eleSelectors = {
    starget: null,
    sversion: null,
};


/* ============= URL 解析 ============= */

/**
 * parse current URL and extract target & version
 * @returns {{ currentTarget: string|null, currentVersion: string|null }}
 */
function parseUrlParts() {
    let currentTarget = null;
    let currentVersion = null;
    const urlSegments = window.location.pathname.toLowerCase().split('/');

    if (docConfig.isMultiTarget) {
        currentTarget = Object.keys(docConfig.fetchedData).find(t =>
            urlSegments.includes(t.toLowerCase())
        ) || null;

        if (currentTarget) {
            const currentTargetObj = docConfig.fetchedData[currentTarget];
            const currentVersionList = (currentTargetObj && currentTargetObj.version) ? currentTargetObj.version : [];
            currentVersion = currentVersionList.find(v =>
                urlSegments.includes(v.toLowerCase())
            ) || null;
        }
    } else {
        currentVersion = docConfig.fetchedData.find(v =>
            urlSegments.includes(v.toLowerCase())
        ) || null;
    }

    return { currentTarget, currentVersion };
}


/* ============= 选择器操作 ============= */

/**
 * generate <select> inner html
 * @param {HTMLSelectElement} selector
 * @param {Array<string | {optionValue: string, optionLabel: string, optionUrl?: string}>} dataSource
 * @param {string} selectedValue
 */
function populateSelector(selector, dataSource, selectedValue) {
    const optionsHTML = dataSource.map(item => {
        const optionValue = typeof item === 'string' ? item : item.optionValue;
        const optionLabel = typeof item === 'string' ? item : item.optionLabel;
        const dataUrl = item.optionUrl ? `data-url="${item.optionUrl}"` : '';
        return `<option value="${optionValue}" ${dataUrl} ${optionValue === selectedValue ? 'selected' : ''}>${optionLabel}</option>`;
    }).join('');
    selector.innerHTML = optionsHTML;
}

/**
 * update version selector by current target
 */
function updateVersionSelector(target, selectedVersion) {
    let versionList = [];
    if (docConfig.isMultiTarget) {
        const targetObj = docConfig.fetchedData[target];
        versionList = (targetObj && targetObj.version) ? targetObj.version : [];
    } else {
        versionList = docConfig.fetchedData;
    }
    populateSelector(eleSelectors.sversion, versionList, selectedVersion);
}

/**
 * init target and version selector
 */
function initSelectors(currentTarget, currentVersion) {
    if (docConfig.isMultiTarget) {
        const targetObjs = Object.entries(docConfig.fetchedData).map(([key, data]) => ({
            optionValue: key,
            optionLabel: data.name,
            optionUrl: data.url,
        }));
        populateSelector(eleSelectors.starget, targetObjs, currentTarget);
        updateVersionSelector(currentTarget, currentVersion);
    } else {
        populateSelector(eleSelectors.sversion, docConfig.fetchedData, currentVersion);
    }
}


/* ============= 工具函数 ============= */

/**
 * replace specified segments of URL pathname（ignore case）
 * @param {string} href - 原始 URL
 * @param {Record<string, string>} replacements - { 旧值: 新值 }
 * @returns {string} 新 URL
 */
function replaceUrlSegments(href, replacements) {
    const url = new URL(href);
    const segments = url.pathname.split('/').map(seg => {
        for (const [from, to] of Object.entries(replacements)) {
            if (from && seg.toLowerCase() === from.toLowerCase()) return to;
        }
        return seg;
    });
    url.pathname = segments.join('/');
    return url.toString();
}

/**
 * Reset the path after version segment in the URL to index.html
 * @param {string} href
 * @param {string} version
 * @returns {string}
 */
function resetToIndexPath(href, version) {
    const url = new URL(href);
    const newPathname = url.pathname.replace(
        new RegExp(`/${version}/.*`, 'i'), `/${version}/index.html`
    );
    url.pathname = newPathname;
    return url.toString();
}


/* ============= 事件处理 ============= */

/**
 * on target change
 */
function onTargetChange() {
    if (!docConfig.isMultiTarget || !eleSelectors.starget) return;

    // get selected <option> element
    const selectedOption = eleSelectors.starget.selectedOptions[0];
    // get selected option data-url (non-empty)
    const targetUrl = (selectedOption && selectedOption.dataset && selectedOption.dataset.url)
        ? selectedOption.dataset.url.trim()
        : '';

    if (targetUrl) { // go to targetUrl
        window.location.href = targetUrl;
        return;
    }

    // when targetUrl is null or empty
    const { currentTarget, currentVersion } = parseUrlParts();
    const selectedTarget = eleSelectors.starget.value;
    const selectedTargetObj = docConfig.fetchedData[selectedTarget];
    const selectedVersionList = (selectedTargetObj && selectedTargetObj.version) ? selectedTargetObj.version : [];
    const defaultVersion = selectedVersionList.length > 0 ? selectedVersionList[0] : 'latest';

    // handle language path segment
    let newUrl = replaceUrlSegments(window.location.href, { cn: 'en', en: 'en' });
    // handle target/version path segment
    const replacements = {};
    if (currentTarget) replacements[currentTarget] = selectedTarget;
    if (currentVersion) replacements[currentVersion] = defaultVersion;
    newUrl = replaceUrlSegments(newUrl, replacements);
    // handle index.html path segment
    newUrl = resetToIndexPath(newUrl, defaultVersion);
    window.location.href = newUrl;
}

/**
 * on version change
 */
function onVersionChange() {
    if (!eleSelectors.sversion) return;

    const { currentVersion } = parseUrlParts();
    if (!currentVersion) return;

    const selectedVersion = eleSelectors.sversion.value;
    const newUrl = replaceUrlSegments(window.location.href, {
        [currentVersion]: selectedVersion,
    });
    window.location.href = newUrl;
}


/* ============= 数据获取 ============= */

async function fetchVersionData() {
    const fetchUrl = "https://docs.realmcu.com/sdk/version.txt";
    try {
        const response = await fetch(fetchUrl);
        const text = await response.text();

        if (docConfig.isMultiTarget) {
            /** version.txt demo:
            {
                "ios_ota_app": {
                    "name": "IOS OTA APP",
                    "url": "https://docs.realmcu.com/xxx/xxx.html",
                    "version": ["latest", "v2.3"]
                },
                ...
            }
            */
            docConfig.fetchedData = JSON.parse(text);
        }
        else {
            /** version.txt demo:
            latest
            v1.4.5.0
            v1.5.0.0
            ...
            */
            docConfig.fetchedData = text.split(/[\r\n]+/).map(s => s.trim()).filter(Boolean);
        }

        const { currentTarget, currentVersion } = parseUrlParts();
        initSelectors(currentTarget, currentVersion);
    } catch (error) {
        console.error('Fail to fetch [VersionSelector] data:', error);
    }
}


/* ============= 初始化入口 ============= */

document.addEventListener('DOMContentLoaded', () => {
    docConfig.isMultiTarget = window.isMultitarget === "True";
    eleSelectors.starget = document.getElementById("target-selector");
    eleSelectors.sversion = document.getElementById("version-selector");

    // if (eleSelectors.starget) eleSelectors.starget.addEventListener('change', onTargetChange);
    // if (eleSelectors.sversion) eleSelectors.sversion.addEventListener('change', onVersionChange);

    fetchVersionData();
});
