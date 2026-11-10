/* ================================================================
* Left Sidebar Resize & Breadcrumb Bar Sync
* ================================================================ */
const STORAGE_SIDEBAR_WIDTH = window.leftNavWidth || 'docs.siderbar.width';
const TOC_PADDING_LEFT = {
    toctreel2: "0.809em",
    toctreel3: "3.236em",
    toctreel4: "4.854em",
    toctreel5: "6.472em",
    toctreel6: "9.708em",
};
const SIDEBAR = {
    WIDTH_MIN       : 200,      // 侧边栏最小拖拽宽度 (px)
    WIDTH_MAX       : 800,      // 侧边栏最大拖拽宽度 (px)
    NAVTOP_HEIGHT_DEFAULT: 64,  // wy-nav-top 找不到时的默认高度 (px)
    ELE_NAV_SIDE    : 'nav.wy-nav-side',
    ELE_NAV_TOP     : 'nav.wy-nav-top',
    ELE_CONTENT_WRAP: '.wy-nav-content-wrap',
    ELE_BREADCRUMB  : '.rst-content > div[role="navigation"]',
};

function syncBreadcrumbPosition() {
    const breadcrumbNav = document.querySelector(SIDEBAR.ELE_BREADCRUMB);
    if (!breadcrumbNav) return;

    if (window.matchMedia('(max-width: 768px)').matches) {
        const navTop = document.querySelector(SIDEBAR.ELE_NAV_TOP);
        breadcrumbNav.style.top  = (navTop ? navTop.offsetHeight : SIDEBAR.NAVTOP_HEIGHT_DEFAULT) + 'px';
        breadcrumbNav.style.left = '0px';
    } else {
        const navSide = document.querySelector(SIDEBAR.ELE_NAV_SIDE);
        breadcrumbNav.style.top  = '0px';
        breadcrumbNav.style.left = (navSide ? navSide.offsetWidth : 0) + 'px';
    }

    const nextEl = breadcrumbNav.nextElementSibling;
    if (nextEl) {
        nextEl.style.marginTop = breadcrumbNav.offsetHeight + 'px';
    }
}

function applySidebarWidth(width) {
    const navSide = document.querySelector(SIDEBAR.ELE_NAV_SIDE);
    const contentWrap = document.querySelector(SIDEBAR.ELE_CONTENT_WRAP);
    if (navSide) navSide.style.width = `${width}px`;
    if (contentWrap) contentWrap.style.marginLeft = `${width}px`;
    syncBreadcrumbPosition();
}

function adaptSidebarByScreen() {
    const navSide = document.querySelector(SIDEBAR.ELE_NAV_SIDE);
    const contentWrap = document.querySelector(SIDEBAR.ELE_CONTENT_WRAP);
    if (window.matchMedia('(max-width: 768px)').matches) {
        if (navSide) navSide.style.width       = '';
        if (contentWrap) contentWrap.style.marginLeft = '';
        syncBreadcrumbPosition();
        return;
    }

    const saved = localStorage.getItem(STORAGE_SIDEBAR_WIDTH);
    if (saved) {
        applySidebarWidth(parseInt(saved, 10));
    } else {
        syncBreadcrumbPosition();
    }
}

function initSidebarDrag() {
    let dragging = false;
    $(SIDEBAR.ELE_NAV_SIDE)
        .append('<span class="resize-handle-left"> || </span>')
        .on('mousedown', 'span.resize-handle-left', function (e) {
            dragging = true;
            e.preventDefault(); // 防止触发文本选中
        });

    // update left sidebar width
    $(document)
        .on('mousemove', function (e) {
            if (!dragging) return;
            e.preventDefault();
            const w = e.clientX - document.body.offsetLeft;
            if (w > SIDEBAR.WIDTH_MIN && w < SIDEBAR.WIDTH_MAX) {
                applySidebarWidth(w);
            }
        })
        .on('mouseup', function () {
            if (!dragging) return;
            dragging = false;
            const navSide = document.querySelector(SIDEBAR.ELE_NAV_SIDE);
            if (navSide) localStorage.setItem(STORAGE_SIDEBAR_WIDTH, navSide.offsetWidth);
        });
}

function initLeftSidebar() {
    adaptSidebarByScreen();
    initSidebarDrag();
}
/* ================================================================ */

/* ================ Add right-sider draggable component ================ */
function addRightContentSlider() {
    let rightResizing = false,
        rightDownX = 0,
        originalWidth = 0;

    $('.contents')
        .append('<span class="resize-handle-right"> || </span>')
        .on('mousedown', 'span.resize-handle-right', function (e) {
            rightResizing = true;
            originalWidth = $('.contents').width();
            rightDownX = e.clientX;
        });

    $(document).on('mousemove', function (e) {
        if (!rightResizing)
            return;

        const offsetNew = rightDownX - e.clientX + originalWidth;
        if (offsetNew > 200 && offsetNew < 600) { // 设置宽度上下限，可根据需求调整
            $('.contents').css('width', offsetNew + 'px');
        }
    }).on('mouseup', function (e) {
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
    for (let className of tocLi.classList) {
        if (className.startsWith('toctree-')) {
            const classKey = className.replace(/-/g, '');
            if (TOC_PADDING_LEFT[classKey]) {
                childUL.style.paddingLeft = TOC_PADDING_LEFT[classKey];
                break;
            }
        }
    }
}
function updateSampleListPaddingLeft(tocLI, childUL) {
    // Configuration for the observer (attributes are being observed)
    const observerConfig = { attributes: true, attributeFilter: ['class'] };
    // Create an observer instance linked to the callback function
    const tocCurrentClassObserver = new MutationObserver(function () {
        if (tocLI.classList.contains('current')) {
            childUL.style.paddingLeft = ""; // Reset padding
        } else {
            setSampleListPaddingLeft(tocLI, childUL);
        }
    });
    tocCurrentClassObserver.observe(tocLI, observerConfig);
}
function setSampleListTocCurrent(targetHref) {
    const targetA = document.querySelector(`a[href="#${targetHref}"]`);
    if (targetA) {
        const targetLi = targetA.parentElement;
        if (targetLi && targetLi.tagName.toLowerCase() === 'li') {
            const targetASiblingUL = targetA.nextElementSibling;
            if (targetASiblingUL) {
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
        targetA.classList.add("always-expand-toc");
    }
}
function initSampleListToc() {
    setSampleListTocCurrent("doc-sample-list-label");

    const sampleListSpan = document.querySelector("span#doc-sample-list-label");
    const sampleListSection = sampleListSpan ? sampleListSpan.parentElement : null;
    if (sampleListSection && sampleListSection.tagName.toLowerCase() === 'section' && sampleListSection.id) {
        setSampleListTocCurrent(sampleListSection.id);
    }
}

/* ===================== highlight current API ===================== */
const highlightAPI = (() => {
    let lastHighlighted = null;
    return () => {
        if (lastHighlighted) {
            lastHighlighted.classList.remove('dt-highlighted');
            lastHighlighted = null;
        }
        const apiHash = window.location.hash;
        if (!apiHash) return;

        // 转义id中可能存在的 . 字符以便于使用 querySelector
        const safeHash = apiHash.replace(/\./g, '\\.');
        const apiSpan = document.querySelector(`span${safeHash}.target`);
        const apiDT = apiSpan
            ? apiSpan.parentElement
            : document.querySelector(`dt${safeHash}[class*='c'], dt${safeHash}[class*='cpp']`);

        if (apiDT) {
            // 给该元素添加highlighted class
            apiDT.classList.add('dt-highlighted');
            // 缓存该元素，避免重复操作
            lastHighlighted = apiDT;
        }
    };
})();

/* ============= Toggle Languages ============= */
function toggleLanguage() {
    const currentUrl = window.location.href;
    const newEnUrl = currentUrl.replace(/(\/|_)cn(\/|\.html)/gi, '$1en$2');
    const newCnUrl = currentUrl.replace(/(\/|_)en(\/|\.html)/gi, '$1cn$2');

    document.getElementById('toEN').href = newEnUrl;
    document.getElementById('toCN').href = newCnUrl;
}

/* ============ fix anchor position ============ */
function getBreadcrumbBottom() {
    const breadcrumbNav = document.querySelector(SIDEBAR.ELE_BREADCRUMB);
    return breadcrumbNav ? breadcrumbNav.getBoundingClientRect().bottom : 0;
}
function adaptHtmlScrollPaddingTop() {
    const padTop = getBreadcrumbBottom();
    document.documentElement.style.scrollPaddingTop = padTop + 'px';
}
function scrollToHash(hash) {
    if (!hash) return;

    const target = document.getElementById(hash.replace(/^#/, ''));
    if (!target) return;

    const offset = getBreadcrumbBottom();
    const targetTop = target.getBoundingClientRect().top
                  + window.scrollY - offset;
    window.scrollTo({ top: Math.max(0, targetTop), behavior: 'instant' });
}

/* ===================== back-to-top func ===================== */
function backToTop() {
    window.scrollTo({ top: 0, behavior: 'smooth' });
}

/* ============ Rewrite default nav expand/collapse ============ */
function initNavToggle() {
    const navContainer = document.querySelector(".wy-menu-vertical");
    if (!navContainer) return;

    const toggleCurrent = (li) => {
        if (!li.querySelector(":scope > ul > li")) return;

        const isExpanded = li.getAttribute("aria-expanded") === "true";
        if (isExpanded) {
            li.classList.remove("current");
            li.setAttribute("aria-expanded", "false");
            li.querySelectorAll("li.current").forEach((child) => {
                child.classList.remove("current");
                child.setAttribute("aria-expanded", "false");
            });
        } else {
            li.classList.add("current");
            li.setAttribute("aria-expanded", "true");
        }
    };

    navContainer.addEventListener("click", (e) => {
        const btn = e.target.closest("button.toctree-expand");
        if (!btn) return;

        e.stopImmediatePropagation();
        e.preventDefault();

        const li = btn.closest("li");
        if (li) toggleCurrent(li);

    }, true);
}

/* ========================== Utils ========================== */
function debounce(fn, delay) {
    let timer;
    return function (...args) {
        clearTimeout(timer);
        timer = setTimeout(() => fn.apply(this, args), delay);
    };
}

/* ==================== Add event listener ==================== */
window.addEventListener('load', function () {
    scrollToHash(window.location.hash);
});

window.addEventListener('hashchange', function () {
    highlightAPI();
    scrollToHash(window.location.hash);
});

window.addEventListener('resize', debounce(function () {
    adaptSidebarByScreen();
    adaptHtmlScrollPaddingTop();
}, 100));

document.addEventListener("DOMContentLoaded", function () {
    const isMultilingual = window.isMultilingual == "True";
    if (isMultilingual) {
        toggleLanguage();
    }

    initLeftSidebar();
    adaptHtmlScrollPaddingTop();
    addCollapseForAPI();
    initSampleListToc();
    highlightAPI();
    initNavToggle();
})
