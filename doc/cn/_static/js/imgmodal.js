document.addEventListener("DOMContentLoaded", function () {
    var imgModal = document.getElementById("imgModal");
    var imgInModal= document.getElementById("imgInModal");
    var imgContainer = imgInModal.parentElement;
    var closeImgModal = document.getElementsByClassName("img-modal-close")[0];
    var currentScale = 1;

    // 初始化图片大小，使其适应容器
    function initModalImageSize() {
        const containerWidth = imgContainer.clientWidth;
        const containerHeight = imgContainer.clientHeight;
        const imgNaturalWidth = imgInModal.naturalWidth;
        const imgNaturalHeight = imgInModal.naturalHeight;

        const widthRatio = containerWidth / imgNaturalWidth;
        const heightRatio = containerHeight / imgNaturalHeight;
        const initScale = Math.min(widthRatio, heightRatio);

        imgInModal.style.width = `${imgNaturalWidth * initScale}px`;
        imgInModal.style.height = `${imgNaturalHeight * initScale}px`;
    }

    document.querySelectorAll('.rst-content img').forEach(function(imgItem) {
        imgItem.addEventListener('click', function (event) {
            var parentElement = imgItem.parentElement;
            if (parentElement && parentElement.tagName.toLowerCase() === 'a') {
                // 阻止父a标签的点击事件
                parentElement.addEventListener('click', function(aEvent) {
                    aEvent.preventDefault();
                });
            }

            imgModal.style.display = "flex";
            imgInModal.src = this.src;
            initModalImageSize();
            document.documentElement.style.overflow = "hidden"; // Disable scrolling
        });
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

    // Zoom in/out
    imgInModal.addEventListener("wheel", function(event) {
        event.preventDefault();
        if (event.deltaY < 0) {
            // Scroll up (zoom in)
            currentScale += 0.1;
        } else {
            // Scroll down (zoom out)
            currentScale = Math.max(1, currentScale - 0.1); // Avoid scaling below the original size
        }
        imgInModal.style.transform = `scale(${currentScale})`;
    });
});