document.addEventListener("DOMContentLoaded", function () {
    var imgModal = document.getElementById("imgModal");
    var imgInModal= document.getElementById("imgInModal");
    var closeImgModal = document.getElementsByClassName("img-modal-close")[0];
    var currentScale = 1;

    document.querySelectorAll('.rst-content img').forEach(function(imgItem) {
        imgItem.addEventListener('click', function () {
            imgModal.style.display = "flex";
            imgInModal.src = this.src;
            imgInModal.style.transform = 'scale(1)'; // Reset scale on each open
            currentScale = 1; // Reset scale value
            document.documentElement.style.overflow = "hidden"; // Disable scrolling
        });
    });

    closeImgModal.onclick = function() { 
        imgModal.style.display = "none";
        document.documentElement.style.overflow = "auto"; // Enable scrolling
    }

    // window.onclick = function(event) {
    //     if (event.target === imgModal) {
    //         imgModal.style.display = "none";
    //         document.documentElement.style.overflow = "auto"; // Enable scrolling
    //     }
    // }

    // Zoom in/out
    imgInModal.addEventListener("wheel", function(event) {
        event.preventDefault();
        if (event.deltaY < 0) {
            // Scroll up (zoom in)
            currentScale += 0.1;
        } else {
            // Scroll down (zoom out)
            currentScale = Math.max(0.5, currentScale - 0.1); // Avoid scaling below the half original size
        }
        imgInModal.style.transform = `scale(${currentScale})`;
    });
});