document.addEventListener('DOMContentLoaded', () => {
    let lastHighlightedElement = null;

    function highlightElement() {
        // 清除之前的高亮效果
        if (lastHighlightedElement) {
            lastHighlightedElement.classList.remove('glossary-highlighted');
            lastHighlightedElement = null;
        }

        // 获取当前URL中的锚点（即#后面的部分）
        const anchor = window.location.hash;

        // 判断是否存在锚点
        if (anchor) {
            // 使用锚点查找页面中的元素
            const element = document.querySelector(anchor);
            
            // 如果找到了该元素
            if (element) {
                // 给该元素添加highlighted class
                element.classList.add('glossary-highlighted');
                // 缓存该元素，避免重复操作
                lastHighlightedElement = element;
            }
        }
    }

    // 初始化高亮效果
    highlightElement();

    // 监听hashchange事件，只在hash变化时执行高亮操作
    window.addEventListener('hashchange', highlightElement);
});
