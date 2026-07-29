/* ================= get curren url info ================ */
function getRootUrl() {
    const contentRoot = document.documentElement.getAttribute('data-content_root') ?? './';
    const upDirCount = (contentRoot.match(/\.\.\//g) ?? []).length;

    const urlObj = new URL(window.location.href);
    const segments = urlObj.pathname.split('/');
    if (segments.at(-1) === '' || segments.at(-1).includes('.')) {
        segments.pop();
    }

    // back upDirCount levels
    const rootPath = segments.slice(0, segments.length - upDirCount).join('/') + '/';

    return `${urlObj.origin}${rootPath}`;
}
function getAISummaryFile() {
    const { origin, pathname } = new URL(window.location.href);

    const rootUrl = getRootUrl();
    const rootPath = new URL(rootUrl).pathname;

    const rootIndex = pathname.indexOf(rootPath);
    if (rootIndex === -1) {
        return null;
    }

    // build ai_summary folder
    const splitIndex = rootIndex + rootPath.length;
    const beforeRoot = pathname.slice(0, splitIndex);
    const afterRoot  = pathname.slice(splitIndex);
    const targetPath = `${beforeRoot}ai_summary/${afterRoot}`;

    return `${origin}${targetPath}`.replace(/\.html$/, '.txt');
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
