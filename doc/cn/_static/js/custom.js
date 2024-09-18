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


document.addEventListener("DOMContentLoaded", function () {
    function showHiddenElement(element) {
        const isHidden = getComputedStyle(element).display !== 'block';
        if (isHidden) {
            // 修改目标元素的display属性
            const parentDls = $(window.location.hash).parents('dl.unexpanded'); // jQuery
            parentDls.each(function(index, pitem) {
                pitem.classList.remove('unexpanded');
                pitem.classList.add('expanded');
            });
        }     
    }

    function adjustScroll() {
        const eleFixedNav = document.querySelector(".wy-nav-top");
        const fixedNavDisplay = window.getComputedStyle(eleFixedNav).display;
        const fixedNavHeight = fixedNavDisplay === "block" ? 65 : 0;
        const hash = window.location.hash.substring(1);
        const eleTarget = document.getElementById(hash);
        if (eleTarget) {
            showHiddenElement(eleTarget);
            const offsetTop = eleTarget.getBoundingClientRect().top + window.pageYOffset - fixedNavHeight;
            window.scrollTo({
                top: offsetTop,
                behavior: 'instant' /* Optional: for smooth scrolling */
            });
        }
    }

    document.addEventListener('click', function(event) {
        if (event.target.tagName === 'A' && event.target.hash) {
            setTimeout(adjustScroll, 0);  // Delay adjustment to allow default behavior to complete
        }
    });

    window.addEventListener("load", adjustScroll); // Adjust on page load if there's a hash in the URL
    window.addEventListener("hashchange", adjustScroll); // When URL hash changes
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
            e.preventDefault(); // 阻止默认的文本选中行为

            // 给body添加类，避免选中内容
            // $('body').css('user-select', 'none');

            leftDownX = e.clientX;
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
