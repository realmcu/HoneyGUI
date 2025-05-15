/* ================ Add left-sider draggable component ================ */
function addLefTocSlider() {
    const initLeftTocSliderWidth = function() {
        if (window.matchMedia('(min-width: 768px)').matches) {
            let leftSiderBarWidth = localStorage.getItem('docs.bee4.siderbar.width');
            if (leftSiderBarWidth) {
                $('.wy-nav-side').css('width', leftSiderBarWidth + 'px');
                $('.wy-nav-content-wrap').css('margin-left', leftSiderBarWidth + 'px');
            }
        } else {
            $('.wy-nav-side').css('width', '');
            $('.wy-nav-content-wrap').css('margin-left', '');
        }
    };

    // init left toc slider width if stored width exists in localStorage
    initLeftTocSliderWidth();

    let leftResizing = false;

    $('.wy-nav-side')
        .append('<span class="resize-handle-left"> || </span>')
        .on('mousedown', 'span.resize-handle-left', function(e) {
            leftResizing = true;
            e.preventDefault(); // 阻止默认的文本选中行为
            // 给body添加类，避免选中内容
            // $('body').css('user-select', 'none');
        });

    $(document).on('mousemove', function(e) {
        if (!leftResizing)
            return;

        e.preventDefault(); // 阻止默认的文本选中行为

        var offsetNew = e.clientX - document.body.offsetLeft;
        // 限制导航栏宽度
        if (offsetNew > 200 && offsetNew < 800) {
            $('.wy-nav-side').css('width', offsetNew + 'px');
            $('.wy-nav-content-wrap').css('margin-left', offsetNew + 'px');
        }
    }).on('mouseup', function(e) {
        if (leftResizing) {
            leftResizing = false;
            // 解除body的类，允许选中内容
            // $('body').css('user-select', '');

            // Store width in localStorage
            var newWidth = $('.wy-nav-side').width();
            localStorage.setItem('docs.bee4.siderbar.width', newWidth);
        }
    });

    // Also adjust width on window resize
    $(window).resize(function() {
        initLeftTocSliderWidth();
    });
}

/* ================ Add right-sider draggable component ================ */
function addRightContentSlider() {
    var rightResizing = false,
        rightDownX = 0,
        originalWidth = 0;

    $('.contents')
        .append('<span class="resize-handle-right"> || </span>')
        .on('mousedown', 'span.resize-handle-right', function(e) {
            rightResizing = true;
            originalWidth = $('.contents').width();
            rightDownX = e.clientX;
        });

    $(document).on('mousemove', function(e) {
        if (!rightResizing)
            return;

        var offsetNew = rightDownX - e.clientX + originalWidth;
        if (offsetNew > 200 && offsetNew < 600) { // 设置宽度上下限，可根据需求调整
            $('.contents').css('width', offsetNew + 'px');
        }
    }).on('mouseup', function(e) {
        rightResizing = false;
    });
}

/* =================== Add API collapse button =================== */
function addCollapseForAPI() {
    document.querySelectorAll("dl.cpp,dl.c").forEach(cppListing => {
        cppListing.classList.add("expanded");
        const dt = cppListing.querySelector("dt");
        const button = document.createElement("span");
        button.classList.add("rtk-api-expansion-button");
        button.addEventListener("click", () => {
            cppListing.classList.toggle("unexpanded");
            cppListing.classList.toggle("expanded");
        });

        dt.insertBefore(button, dt.firstChild);
    });
}

/* ============== expand sample list sider navigation ============== */
function setSampleListPaddingLeft(tocLi, childUL) {
    const tocPaddingLeft = {
        toctreel2: "0.809em",
        toctreel3: "3.236em",
        toctreel4: "4.854em",
        toctreel5: "6.472em",
        toctreel6: "9.708em",
    };
    for (let className of tocLi.classList) {
        if (className.startsWith('toctree-')) {
            const classKey = className.replace('-', '');
            if (tocPaddingLeft[classKey]) {
                childUL.style.paddingLeft = tocPaddingLeft[classKey];
                break;
            }
        }
    }
}
function updateSampleListPaddingLeft(tocLI, childUL) {
    // Configuration for the observer (attributes are being observed)
    const observerConfig = { attributes: true, attributeFilter: ['class'] };
    // Create an observer instance linked to the callback function
    const tocCurrentClassObserver = new MutationObserver(function (mutationsList) {
        for (let mutation of mutationsList) {
            if (tocLI.classList.contains('current')) {
                childUL.style.paddingLeft = ""; // Reset padding
            } else {
                setSampleListPaddingLeft(tocLI, childUL);
            }
        }
    });
    tocCurrentClassObserver.observe(tocLI, observerConfig);
}
function setSampleListTocCurrent(targetHref) {
    let targetA = document.querySelector(`a[href="#${targetHref}"]`);
    if (targetA) {
        let targetLi = targetA.parentElement;
        if (targetLi && targetLi.tagName.toLowerCase() === 'li') {
            let targetASiblingUL = targetA.nextElementSibling;
            if(targetASiblingUL) {
                targetASiblingUL.style.display = "block";
                if (targetLi.classList.contains('current')) {
                    targetASiblingUL.style.paddingLeft = "";
                } else {
                    setSampleListPaddingLeft(targetLi, targetASiblingUL);
                }
                updateSampleListPaddingLeft(targetLi, targetASiblingUL);
            }
        }
        // modify sample list toc tree control button always expand
        document.querySelector(`a[href="#${targetHref}"]`).classList.add("always-expand-toc");
    }
}
function initSampleListToc() {
    setSampleListTocCurrent("doc-sample-list-label");

    const sampleListSpan = document.querySelector("span#doc-sample-list-label");
    const sampleListSection = sampleListSpan ? sampleListSpan.parentElement : null;
    if (sampleListSection && sampleListSection.tagName.toLowerCase() === 'section' && sampleListSection.id) {
        const sampleListSectionId = sampleListSection.id;
        setSampleListTocCurrent(sampleListSectionId);
    }  
}

/* ===================== hilight current API ===================== */
let lastHighlightedAPI = null;
function highlightAPI() {
    // 清除之前的高亮效果
    if (lastHighlightedAPI) {
        lastHighlightedAPI.classList.remove('dt-highlighted');
        lastHighlightedAPI = null;
    }

    const apiHash = window.location.hash;
    if (apiHash) {
        // 转义id中可能存在的 . 字符以便于使用 querySelector
        const safeHash = apiHash.replace(/\./g, '\\.');
        let apiDT = null;
        let apiSpan = document.querySelector(`span${safeHash}.target`);
        if(apiSpan) {
            apiDT = apiSpan.parentElement;
        }
        else {
            apiDT = document.querySelector(`dt${safeHash}[class*='c'], dt${safeHash}[class*='cpp']`);
        }
        // 如果找到了该元素，并且元素标签为 dt
        if (apiDT) {
            // 给该元素添加highlighted class
            apiDT.classList.add('dt-highlighted');
            // 缓存该元素，避免重复操作
            lastHighlightedAPI = apiDT;
        }
    }
}

$(document).ready(function () {
    addLefTocSlider();
    addCollapseForAPI();
    initSampleListToc();
    highlightAPI();
    // 监听hashchange事件，只在hash变化时执行高亮操作
    window.addEventListener('hashchange', highlightAPI);
})


/* ===================== back-to-top func ===================== */
function backToTop() {
    window.scrollTo({ top: 0, behavior: 'smooth' });
    // $('html,body').animate({
    //     scrollTop: 0
    // }, 300);
}

/* ============= Toggle Languages ============= */
document.addEventListener("DOMContentLoaded", function () {
    let currentUrl = window.location.href;
    let newEnUrl = currentUrl.replace(/(\/|_)cn\//gi, '$1en/');
    let newCnUrl = currentUrl.replace(/(\/|_)en\//gi, '$1cn/');

    document.getElementById('toEN').href = newEnUrl;
    document.getElementById('toCN').href = newCnUrl;
})

/* ================= Add AI ASK Fetch request ================= */
document.addEventListener('DOMContentLoaded', function () {
    function fetchChatAnwser(chatInputText, chatHistory) {
        let curVersion = (document.querySelector("#version-selector")?.value) || "latest";
        // If the current version is "latest", check and update it from LatestVersion
        if (curVersion === "latest") {
            const latestEle = document.querySelector('#LatestVersion');
            if (latestEle && latestEle.value) {
                curVersion = latestEle.value;
            }
        }
        return new Promise((resolve, reject) => {
            const rawBody = {
                rawData: chatInputText, // required, 要詢問 AI 的問句
                docBase: "2039", // required, 知識庫id(測試區請使用 2039)
                docVersion: curVersion, // optional, 文檔版本號，若沒有則 AI 會以最新的版本回答
                // optional, 過去的對話紀錄，請符合範例的格式，目前可以輸入長度最多為 6 (三組 user-assistant 組合)，若無對話紀錄則不需輸入
                historyMsg: chatHistory,
            };
            $.ajax({
                url: 'https://wwwdev.realmcu.com/docs/aichat',
                type: 'POST',
                dataType: "json",
                contentType: "application/x-www-form-urlencoded",
                async: true,
                data: JSON.stringify(rawBody),
                success: function (resp) {
                    resolve(resp);
                },
                error: function (err) {
                    if(err) {
                        reject(err);
                    }
                    else {
                        reject("Realmcu Server Network Error!");
                    }
                }
            });
        });
    }

    chatWidgetInit({
        makeRequest: fetchChatAnwser,
        chatWidgetTitle: "AI Ask",
        chatWidgetPlaceholder: "Please type your question"
    });
});
