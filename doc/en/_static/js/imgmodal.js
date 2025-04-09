document.addEventListener("DOMContentLoaded", function () {
    const imgModal = document.getElementById("imgModal");
    const imgInModal= document.getElementById("imgInModal");
    const imgModalCloseButton = document.getElementsByClassName("img-modal-close")[0];
    // const wyGridContainer = document.querySelector(".wy-grid-for-nav");
    let curImgScale = 1;

    function addImgClickListener() {
        const initModalImgSize = function() {
            curImgScale = 1; // Reset scale value
            updateImgTransform(1);
        }

        const handleImgClick = function() { // 改为普通的函数表达式
            imgInModal.src = this.src; // 这里 this 的引用会指向被点击的 img 元素
            imgModal.style.display = "block";
            initModalImgSize();
            document.documentElement.style.overflow = "hidden"; // 禁用滚动
            // wyGridContainer.classList.add("prevent-manipulation");
        };

        document.querySelectorAll('.rst-content img').forEach(function(imgItem) {
            var validImgRegex = /\.(jpe?g|png|gif|bmp|webp)$/i;
            var parentElement = imgItem.parentElement;
            var parentHref = parentElement.getAttribute('href');
            // 父标签有 href 属性
            if (parentHref) {
                if(validImgRegex.test(parentHref.toLowerCase())) { // 父标签 href 属性为图片地址
                    // 阻止父a标签的点击事件
                    parentElement.addEventListener('click', function(aEvent) {
                        aEvent.preventDefault();
                    });
                    parentElement.classList.add('no-cursor-link');
                }
            }
            imgItem.addEventListener('click', handleImgClick);
        });
    }
    
    function addImgModalCloseHandler() {
        imgModal.addEventListener('click', function(event) {
            // 确保点击的仅是 imgModal 或者 imgModalCloseButton，而非内部其他子元素
            if (event.target === imgModal || event.target === imgModalCloseButton) {
                imgModal.style.display = "none";
                document.documentElement.style.overflow = "auto"; // 启用页面滚动
                // wyGridContainer.classList.remove("prevent-manipulation");
            }
        });
    }

    function updateImgTransform(scale) {
        let imgModalWid = imgModal.offsetWidth;
        let imgModalHei = imgModal.offsetHeight;
        let imgInModalWid= imgInModal.scrollWidth * scale;
        let imgInModalHei = imgInModal.scrollHeight * scale;

        const offsetX = (imgModalWid - imgInModalWid)/2;
        const offsetY = (imgModalHei - imgInModalHei)/2;

        let translateX = 0;
        let translateY = 0;
        if(offsetX > 0) {
            translateX = offsetX;
        }
        else {
            translateX = 0;
        }
        if(offsetY > 0) {
            translateY = offsetY;
        }
        else {
            translateY = 0;
        }

        imgInModal.style.transform = `translate(${translateX}px, ${translateY}px) scale(${scale})`;
    }

    function addImgZoomHandler() {
        // image zoom in/out
        imgInModal.addEventListener("wheel", function(event) {
            event.preventDefault();
            if (event.deltaY < 0) {
                // Scroll up (zoom in)
                curImgScale = Math.min(3, curImgScale + 0.06); // Avoid scaling below the original size
            } else {
                // Scroll down (zoom out)
                curImgScale = Math.max(0.7, curImgScale - 0.06); // Avoid scaling below the original size
            }

            updateImgTransform(curImgScale);
        });
    }

    function addImgTouchEventHandler() {
        let initialDistance = 0;
    
        const getDistance = (point1, point2) => {
            return Math.hypot(point2.x - point1.x, point2.y - point1.y);
        };
    
        imgInModal.addEventListener('touchstart', (event) => {
            if (event.touches.length === 2) {
                const touch1 = event.touches[0];
                const touch2 = event.touches[1];
                initialDistance = getDistance(
                    { x: touch1.pageX, y: touch1.pageY }, 
                    { x: touch2.pageX, y: touch2.pageY }
                );
            }
        });
    
        imgInModal.addEventListener('touchmove', (event) => {
            if (event.touches.length === 2) {
                event.preventDefault();
                const touch1 = event.touches[0];
                const touch2 = event.touches[1];
                const newDistance = getDistance(
                    { x: touch1.pageX, y: touch1.pageY }, 
                    { x: touch2.pageX, y: touch2.pageY }
                );
                const scaleChange = newDistance / initialDistance;
                if(scaleChange >= 1) {
                    curImgScale = Math.min(3, curImgScale + 0.06); // zoom in
                } else {
                    curImgScale = Math.max(0.7, curImgScale - 0.06); // zoom out
                }
    
                // Apply new transform
                updateImgTransform(curImgScale);
    
                // Update initial distance for the next move event
                initialDistance = newDistance;
            }
        }, { passive: false });
    }

    addImgClickListener();
    addImgModalCloseHandler();
    addImgZoomHandler();
    addImgTouchEventHandler();
});