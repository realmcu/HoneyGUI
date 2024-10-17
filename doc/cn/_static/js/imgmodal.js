document.addEventListener("DOMContentLoaded", function () {
    var imgModal = document.getElementById("imgModal");
    var imgInModal= document.getElementById("imgInModal");
    var closeImgModal = document.getElementsByClassName("img-modal-close")[0];
    var currentScale = 1;

    // 初始化图片大小，使其适应容器
    function initModalImageSize() {
        const imgNaturalWidth = imgInModal.naturalWidth;
        const imgNaturalHeight = imgInModal.naturalHeight;
        const imgRatio = imgNaturalWidth / imgNaturalHeight;

        const innerWidth = window.innerWidth;
        const innerHeight = window.innerHeight;
        const winRatio = innerWidth / innerHeight;

        var imgRenderWidth = imgNaturalWidth;
        var imgRenderHeight = imgNaturalHeight;
        if(imgRatio >= winRatio) {
            imgRenderWidth = Math.min(imgNaturalWidth, innerWidth);
            imgRenderHeight = imgRenderWidth / imgRatio;
        }
        else {
            imgRenderHeight = Math.min(imgNaturalHeight, innerHeight);
            imgRenderWidth = imgRenderHeight * imgRatio;
        }

        imgInModal.style.width = `${imgRenderWidth}px`;
        imgInModal.style.height = `${imgRenderHeight}px`;

        imgInModal.style.transformOrigin = "center center";
        imgInModal.style.transform = 'scale(1)'; // Reset scale and translation on each open
        currentScale = 1; // Reset scale value
    }

    function handleImageClick(event) {
        imgModal.style.display = "flex";
        imgModal.style.justifyContent = 'center';
        imgModal.style.alignItems = 'center';

        imgInModal.src = this.src;
        initModalImageSize();
        document.documentElement.style.overflow = "hidden"; // Disable scrolling
    }

    document.querySelectorAll('.rst-content img').forEach(function(imgItem) {
        var validImageRegex = /\.(jpe?g|png|gif|bmp|webp)$/i;
        var parentElement = imgItem.parentElement;
        var parentHref = parentElement.getAttribute('href');
        // 父标签有 href 属性
        if (parentHref) {
            if(validImageRegex.test(parentHref.toLowerCase())) { // 父标签 href 属性为图片地址
                // 阻止父a标签的点击事件
                parentElement.addEventListener('click', function(aEvent) {
                    aEvent.preventDefault();
                });
                parentElement.classList.add('no-cursor-link');
                imgItem.addEventListener('click', handleImageClick);
            }
        }
        else { // 父标签没有 href 属性
            imgItem.addEventListener('click', handleImageClick);
        }
    });

    closeImgModal.onclick = function() {
        imgModal.style.display = "none";
        document.documentElement.style.overflow = "auto"; // Enable scrolling
    }

    window.onclick = function(event) {
        if (event.target === imgModal) {
            imgModal.style.display = "none";
            document.documentElement.style.overflow = "auto"; // Enable scrolling
        }
    }

    function listenImgModalcroll() {
        const hasHorScrollBar = (imgInModal.clientWidth * currentScale) > imgModal.clientWidth;
        const hasVerScrollBar = (imgInModal.clientHeight * currentScale) > imgModal.clientHeight;

        if (hasHorScrollBar) {
            imgModal.style.justifyContent = 'start';
            if (hasVerScrollBar) {
                imgModal.style.alignItems = 'start';
                imgInModal.style.transformOrigin = 'left top';
            } else {
                imgModal.style.alignItems = 'center';
                imgInModal.style.transformOrigin = 'left center';
            }
        }
        else {
            imgModal.style.justifyContent = 'center';
            if (hasVerScrollBar) {
                imgModal.style.alignItems = 'start';
                imgInModal.style.transformOrigin = 'center top';
            } else {
                imgModal.style.alignItems = 'center';
                imgInModal.style.transformOrigin = 'center center';
            }
        }
    }

    // Zoom in/out
    imgInModal.addEventListener("wheel", function(event) {
        event.preventDefault();

        if (event.deltaY < 0) {
            // Scroll up (zoom in)
            currentScale = Math.min(3, currentScale + 0.06); // Avoid scaling below the original size
        } else {
            // Scroll down (zoom out)
            currentScale = Math.max(0.7, currentScale - 0.06); // Avoid scaling below the original size
        }

        imgInModal.style.transform = `scale(${currentScale})`;
        listenImgModalcroll();
    });
});