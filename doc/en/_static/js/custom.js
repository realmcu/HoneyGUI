/* API collapsing */
document.addEventListener('DOMContentLoaded', (event) => {
    document.querySelectorAll("dl.cpp,dl.c").forEach(cppListing => {
        const curHash = location.hash.substring(1);
        const dt = cppListing.querySelector("dt");
        const span = cppListing.querySelector(`[id="${curHash}" ]`);
        let shouldBeExpanded = false;

        if(dt.id.toLocaleLowerCase() == curHash.toLocaleLowerCase() || span != null) {
            shouldBeExpanded = true;
        }    
        cppListing.classList.add(shouldBeExpanded ? "expanded" : "unexpanded");
        const button = document.createElement("span");
        button.classList.add("rtk-api-expansion-button");
        button.addEventListener("click", () => {
            cppListing.classList.toggle("unexpanded");
            cppListing.classList.toggle("expanded");
        });
        
        dt.insertBefore(button, dt.firstChild);
    });
})


document.addEventListener('DOMContentLoaded', () => {
    // 获取页面上所有的锚点链接,添加点击事件
    document.querySelectorAll('a[href^="#"]').forEach(anchorItem => {
        anchorItem.addEventListener('click', function(event) {
            const targetHref = anchorItem.getAttribute('href');
            const targetElement = document.getElementById(targetHref.slice(1));

            if(targetElement){
                // event.preventDefault(); // 阻止默认行为

                // 使用 window.getComputedStyle 获取实际 display 属性
                if(window.getComputedStyle(targetElement).display !== 'block'){
                    // 修改目标元素的display属性
                    const parents = $(targetHref).parents('dl.unexpanded'); // jQuery
                    parents.each(function(index, parent) {
                        parent.classList.remove('unexpanded');
                        parent.classList.add('expanded');
                    });
                }

                setTimeout(() => {
                    history.pushState(null, null, ' ');
                    history.pushState(null, null, targetHref);

                    const eleNavTop = document.querySelector(".wy-nav-top");
                    const eleDisplay = window.getComputedStyle(eleNavTop).display;
                    const stickyOffset = eleDisplay === "block" ? -60 : 0;
                    const targetPosition = targetElement.getBoundingClientRect().top + window.scrollY + stickyOffset;

                    // 使用平滑滚动，并考虑 sticky 元素的高度
                    window.scrollTo({
                        top: targetPosition,
                        behavior: 'auto'
                    });
                }, 0);
            }
        })
    });
});


/* ========= Add left-sider and right-sider draggable component ========= */
$(function() {
    var leftResizing = false,
        leftDownX = 0;

    // Checks if stored width exists in localStorage
    var leftSiderBarWidth = localStorage.getItem('docs.honeygui.siderbar.width');
    if (window.matchMedia('(min-width: 768px)').matches && leftSiderBarWidth) {
        $('.wy-nav-side').css('width', leftSiderBarWidth + 'px');
        $('.wy-nav-content-wrap').css('margin-left', leftSiderBarWidth + 'px');
    }

    $('.wy-nav-side')
        .append('<span class="resize-handle-left"> || </span>')
        .on('mousedown', 'span.resize-handle-left', function(e) {
            leftResizing = true;
            leftDownX = e.clientX;
        });

    $(document).on('mousemove', function(e) {
        if (!leftResizing)
            return;

        var offsetNew = e.clientX - document.body.offsetLeft;
        // 限制导航栏宽度
        if (offsetNew > 200 && offsetNew < 800) {
            $('.wy-nav-side').css('width', offsetNew + 'px');
            $('.wy-nav-content-wrap').css('margin-left', offsetNew + 'px');
        }
    }).on('mouseup', function(e) {
        if (leftResizing) {
            leftResizing = false;
            // Store width in localStorage
            var newWidth = $('.wy-nav-side').width();
            localStorage.setItem('docs.honeygui.siderbar.width', newWidth);
        }
    });

    // Also adjust width on window resize
    $(window).resize(function() {
        if (window.matchMedia('(min-width: 768px)').matches) {
            var leftSiderBarWidth = localStorage.getItem('docs.honeygui.siderbar.width');
            if (leftSiderBarWidth) {
                $('.wy-nav-side').css('width', leftSiderBarWidth + 'px');
                $('.wy-nav-content-wrap').css('margin-left', leftSiderBarWidth + 'px');
            }
        } else {
            $('.wy-nav-side').css('width', '');
            $('.wy-nav-content-wrap').css('margin-left', '');
        }
    });
});


$( function() {
    var rightResizing = false,
        rightDownX = 0,
        originalWidth = 0;

    $( '.contents' )
        .append( '<span class="resize-handle-right"> || </span>' )
        .on( 'mousedown', 'span.resize-handle-right', function( e ) {
            rightResizing = true;
            originalWidth = $( '.contents' ).width();
            rightDownX = e.clientX;
        });

    $( document ).on( 'mousemove', function( e ) {
        if (!rightResizing)
            return;

        var offsetNew = rightDownX - e.clientX + originalWidth;
        if (offsetNew > 200 && offsetNew < 600) { // 设置宽度上下限，可根据需求调整
            $( '.contents' ).css( 'width', offsetNew + 'px' );
        }
    }).on( 'mouseup', function(e) {
        rightResizing = false;
    });
});

/* ============= back-to-top func ============= */
function backToTop() {
    window.scrollTo({ top: 0, behavior: 'smooth' });
    // $('html,body').animate({
    //     scrollTop: 0
    // }, 300);
}

/* ============= Toggle Languages ============= */
function change_language(lang)
{
    var url = window.location.pathname;
    var newUrl = "";
    if(lang.toLowerCase() == 'en')
    {
        newUrl = url.replace(/\/en\//gi, '/cn/');
    }
    else {
        newUrl = url.replace(/\/cn\//gi, '/en/');
    }  
    window.location.href = newUrl;
}
