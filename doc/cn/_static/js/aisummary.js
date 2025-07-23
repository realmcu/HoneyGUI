/* ================= get curren url info ================ */
function getRootUrl() {
    const contentRoot = document.documentElement.getAttribute('data-content_root');
    const dirMatches = contentRoot.match(/\.\.\//g);
    const upDirCount = dirMatches ? dirMatches.length : 0;

    // create URL Obiect
    const urlObj = new URL(window.location.href);
    // get url path not include protocol, host, anchor and query params
    let urlPaths = urlObj.pathname.split('/');

    // delete empty string and doc name
    if (urlPaths[urlPaths.length - 1] === '' || urlPaths[urlPaths.length - 1].includes('.')) {
        urlPaths.pop();
    }
    // folder back off
    urlPaths = urlPaths.slice(0, urlPaths.length - upDirCount);
    // root path
    let rootPath = urlPaths.join('/') + '/';
    // create root url
    const rootUrl = `${urlObj.origin}${rootPath}`;
    return rootUrl;
}

function getAISummaryFile() {
    // create URL Obiect
    const urlObj = new URL(window.location.href);
    // get url path not include protocol, host, anchor and query params
    const urlPath = urlObj.pathname;

    // get root path index
    const rootUrlPart = getRootUrl().slice(-15);
    const rootIndex = urlPath.indexOf(rootUrlPart);

    if (rootIndex === -1) {
        return null;
    }

    // 生成新的路径：在指定字符串之后插入新字符串
    const beforeRoot = urlPath.slice(0, rootIndex + rootUrlPart.length);
    const afterRoot = urlPath.slice(rootIndex + rootUrlPart.length);
    const targetPath = beforeRoot + "ai_summary/" + afterRoot;

    // 构建完整的新 URL
    const targetUrl = (urlObj.origin + targetPath).replace(/\.html$/, '.txt');
    return targetUrl;
}

/* ============== init ai summary component ============== */
function initAISummaryWidget(title) {
    let flag = false;

    const aiSummaryTitle = title || 'AI summary';
    const aiSummaryWidgetHTML = `
    <div class="ai-summary-container">
        <button type="button" id="AISummaryButton">Summarize</button>
        <div class="ai-summary-content">
            <div class="ai-summary-header">${aiSummaryTitle}</div>
            <div class="ai-summary-text-box"><p class="ai-summary-text text-hidden" id="AISummaryText"></p></div>
        </div>
    </div>
    `;

    const eleMainDocument = document.querySelector('#AISummaryWidget');
    // const eleMainDocument = document.querySelector('[role="main"].document');
    if (eleMainDocument) {
        // 使用 insertAdjacentHTML 方法
        eleMainDocument.insertAdjacentHTML('afterbegin', aiSummaryWidgetHTML);
        flag = true;
    } else {
        console.error('Element with id="AISummaryWidget" not found.');
    }
    return flag;
}

/* ============================= fetch ai summary ============================= */
function handleAISummaryButtonClick() {
    const summaryButton = document.getElementById("AISummaryButton");
    const eleAIText = document.getElementById('AISummaryText');

    function typeWriter(element, text, speed = 14) {
        element.innerHTML = '';

        const options = {
            strings: [text],
            typeSpeed: speed, // 打字速度
            loop: false, // 是否循环
            showCursor: false, // 显示光标
        };
        const typed = new Typed(element, options);
    }

    function handleAISummaryResponse(text) {
        // cancel ai summary text loading status effect
        eleAIText.classList.remove('text-loading');

        // show ai response
        let ouputHtml = "Invalid response data!";
        if (text) {
            ouputHtml = text;
        }
        eleAIText.innerHTML = ouputHtml;
        // typeWriter(eleAIText, ouputHtml, 1);

        // enable ai summary fetch button
        summaryButton.classList.remove('disabled-button');
    }
    function handleAISummaryError(error) {
        // cancel ai summary text loading status effect
        eleAIText.classList.remove('text-loading');
        // show ai request error
        eleAIText.innerHTML = error;
        // enable ai summary fetch button
        summaryButton.classList.remove('disabled-button');
    }

    /* -------------- query ai summary by ajax -------------- */
    summaryButton.addEventListener("click", async () => {
        const hideAIText = eleAIText.classList.toggle('text-hidden');
        if (!hideAIText) {
            summaryButton.innerText = "Hidden";

            const isAITextEmpty = eleAIText.innerText.trim().length == 0;
            if(isAITextEmpty) {
                summaryButton.classList.add('disabled-button');
                eleAIText.classList.add('text-loading');
                // delay 3s
                setTimeout(function() {
                    const fetchURL = getAISummaryFile();
                    if(!fetchURL) {
                        handleAISummaryError("AI summary file parse error, please refresh and try again!");
                        return;
                    }

                    return fetch(fetchURL)
                        .then(res => {
                            if (!res.ok) {
                                throw new Error(`HTTP error! status: ${res.status}`);
                            }
                            return res.text();
                        })
                        .then(text => {
                            handleAISummaryResponse(text);
                        })
                        .catch(error => {
                            handleAISummaryError("Network error, please refresh and try again!");
                        });
                }, 3000);
            }
        }
        else {
            summaryButton.innerText = "Summarize";
            eleAIText.classList.remove('text-loading');
        }
    });
}

$(document).ready(function () {
    let initAIWidgetFlag = initAISummaryWidget("AI summary");
    if(initAIWidgetFlag) {
        handleAISummaryButtonClick();
    }
})
