document.addEventListener("DOMContentLoaded", function () {
    var imgModal = document.getElementById("imgModal");
    var imgInModal= document.getElementById("imgInModal");
    var imgModalCloseButton = document.getElementsByClassName("img-modal-close")[0];
    var curImgScale = 1;
 
    function addImgClickListener() {
        const initModalImgSize = function() {
            const innerWidth = window.innerWidth;
            const innerHeight = window.innerHeight;
        
            imgInModal.style.maxWidth = `${innerWidth}px`;
            imgInModal.style.maxHeight = `${innerHeight}px`;
            
            imgInModal.style.transformOrigin = "center center";
            imgInModal.style.transform = 'scale(1)'; // Reset scale and translation on each open
            curImgScale = 1; // Reset scale value
        }
 
        const handleImgClick = function() { // 改为普通的函数表达式
            imgModal.style.display = "flex";
            imgModal.style.justifyContent = 'center';
            imgModal.style.alignItems = 'center';
    
            imgInModal.src = this.src; // 这里 this 的引用会指向被点击的 img 元素
            initModalImgSize();
            document.documentElement.style.overflow = "hidden"; // 禁用滚动
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
            }
        });
    }
 
    function addImgZoomHandler() {
        const updateImgModalPositionStyle = function() {
            const hasHorScrollBar = (imgInModal.clientWidth * curImgScale) > imgModal.clientWidth;
            const hasVerScrollBar = (imgInModal.clientHeight * curImgScale) > imgModal.clientHeight;
    
            // set the horizontal alignment
            imgModal.style.justifyContent = hasHorScrollBar ? 'start' : 'center';
            // set the vertical alignment
            imgModal.style.alignItems = hasVerScrollBar ? 'start' : 'center';
            // set transform origin attribute: start position
            const horTransformOrigin = hasHorScrollBar ? 'left' : 'center';
            const verTransformOrigin = hasVerScrollBar ? 'top' : 'center';
            imgInModal.style.transformOrigin = `${horTransformOrigin} ${verTransformOrigin}`;
        }
 
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
 
            imgInModal.style.transform = `scale(${curImgScale})`;
            updateImgModalPositionStyle();
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
    
                // Calculate new scale and constrain it between 0.7 and 3
                curImgScale *= scaleChange;
                curImgScale = Math.max(0.7, Math.min(curImgScale, 3));
    
                // Apply new scale
                imgInModal.style.transform = `scale(${curImgScale})`;
                updateImgModalPositionStyle();
    
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