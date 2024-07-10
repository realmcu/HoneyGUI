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
    // 获取页面上所有的锚点链接
    var rstcontent = document.querySelector(".rst-content");
    var anchors = rstcontent.querySelectorAll('a[href^="#"]');

    for (var i = 0; i < anchors.length; i++) {
        anchors[i].addEventListener('click', function(event) {
            var href = this.getAttribute('href'); // 获取href值，形如"#target"
            var targetId = href.slice(1); // 切掉"#"，得到"target id"
            var targetElement = document.getElementById(targetId);

            // 阻止默认行为
            event.preventDefault();

            if(targetElement.style.display != "block"){
                // 修改目标元素的display属性
                const parents = $(`#${targetId}`).parents("dl.unexpanded");
                parents.each(function(index, parent) {
                    parent.classList.remove("unexpanded");
                    parent.classList.add("expanded");
                });
            }


            setTimeout(function() {
                // 修改location.hash来实现页面的跳转
                location.hash = href;
            }, 0);
            $('html, body').animate({
                scrollTop: targetElement.offsetTop
            }, 1000); // 动画滚动时间，这里设置为1000ms，可以根据需要调整
        });
    }

});

/* ========= Add left-sider and right-sider draggable component ========= */
$( function() {
    var leftResizing = false,
        leftDownX = 0;

    $( '.wy-nav-side' )
        // .wrapInner('<div class="in-nav-side"></div>')
        .append( '<span class="resize-handle-left"> || </span>' )
        .on( 'mousedown', 'span.resize-handle-left', function( e ) {
            leftResizing = true;
            leftDownX = e.clientX;
        });

    $( document ).on( 'mousemove', function( e ) {
        if (!leftResizing)
            return;

        var offsetNew = e.clientX - document.body.offsetLeft;

        //限制导航栏宽度
        if (offsetNew > 200 && offsetNew < 800) { 
            $( '.wy-nav-side' ).css( 'width', offsetNew + 'px' );
            // $( '.in-nav-side' ).css( 'width', offsetNew + 'px' );
            $( '.wy-nav-content-wrap' ).css( 'margin-left', offsetNew + 'px' );
        }
    }).on( 'mouseup', function(e) {
        leftResizing = false;
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

/* ================ Add back-to-top component ================ */
document.addEventListener("DOMContentLoaded", function() {
    var backToTopButton = document.createElement("div");
    backToTopButton.id = "back-to-top";
    backToTopButton.innerHTML = "↑";
    backToTopButton.onclick = function() {
        window.scrollTo({ top: 0, behavior: 'smooth' });
    };
    document.body.appendChild(backToTopButton);
});

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