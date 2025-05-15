(function() {
    function inserChatWidgetHTML(title, placeholder) {
        const chatWidgetHTML = `
        <chat-widget>
            <style>
                chat-widget * {
                    box-sizing: border-box;
                }
                .chat-modal-mask {
                    display: none; /* Initially hidden */
                    width: 100%;
                    height: 100%;
                    background-color: rgba(0,0,0,0.4);
                    position: fixed;
                    top: 0;
                    left: 0;
                    z-index: 1001;
                }
                chat-widget .chat-modal {
                    position: absolute;
                    top: 50%;
                    left: 50%;
                    transform: translate(-50%, -50%);
                    width: 450px;
                    height: 80%;
                    max-width: 100%;
                    max-height: 100%;
                    min-width: 300px;
                    min-height: 450px;
                }
                chat-widget .fullscreen-chat-modal {
                    width: 100vw !important;
                    height: 100vh !important;
                }
                chat-widget .resize-modal {
                    position: absolute;
                    bottom: -6px;
                    right: -2px;
                    cursor: se-resize;
                }
                chat-widget .chat-box {
                    width: 100%;
                    height: 100%;
                    display: flex;
                    flex-direction: column;
                    border-radius: 8px;
                    box-shadow: rgba(0, 0, 0, 0.16) 0px 5px 40px;
                    padding-bottom: 20px;
                    background-color: #fff;
                }
                chat-widget .chat-title {
                    display: flex;
                    align-items: center;
                    justify-content: space-between;
                    height: 60px;
                    padding: 15px 20px;
                    border-radius: 8px 8px 0 0;
                    background-color: #0068b6;
                    color: #fff;
                    font-weight: bold;
                }
                chat-widget .chat-title-buttons {
                    display: flex;
                    align-items: center;
                    gap: 6px; /* Space between the buttons */
                }
                chat-widget .chat-refresh,
                chat-widget .close-chat-modal,
                chat-widget .full-screen-control {
                    cursor: pointer;
                    background: none;
                    border: none;
                    padding: 5px;
                }
                chat-widget .chat-refresh:hover svg {
                    stroke: #d8d8d8;
                }
                chat-widget .close-chat-modal:hover path {
                    fill: #d8d8d8;
                }
                chat-widget .chat-content {
                    display: flex;
                    flex-direction: column;
                    flex-grow: 1;
                    padding: 20px 10px;
                    background-color: #f8f8f8;
                    overflow-y: auto;
                }
                chat-widget .chat-input-box {
                    padding: 10px 0 0;
                }
                chat-widget .chat-input {
                    position: relative;
                    width: 92%;
                    margin: auto;
                    height: auto;
                    min-height: 56px;
                    max-height: 192px;
                    border: 1px solid #eee;
                    border-radius: 6px;
                    box-shadow: 0 2px 6px -1px rgba(0,0,0,.1);
                }
                chat-widget .chat-input textarea {
                    resize: none;
                    width: calc(100% - 40px);
                    min-height: 56px;
                    max-height: 128px;
                    overflow-y: auto;
                    border: none;
                    outline: none;
                    font-size: 16px;
                    color: #666;
                }
                chat-widget .chat-input button {
                    width: 30px;
                    position: absolute;
                    bottom: 2px;
                    right: 2px;
                    cursor: pointer;
                    border: none;
                    background: none;
                }
                chat-widget .chat-message {
                    display: flex;
                    justify-content: flex-start;
                    margin: 5px 0;
                }
                chat-widget .flex-justify-end {
                    justify-content: flex-end;
                }
                chat-widget .userText,
                chat-widget .chatResp {
                    display: block;
                    padding: 8px 12px;
                    border-radius: 5px;
                }
                chat-widget .userText p, 
                chat-widget .chatResp p {
                    margin: 3px 0;
                }
                chat-widget .userText {
                    white-space: pre-wrap;
                    margin-left: 40px;
                    background-color: #0068b6;
                    color: #fff;
                }
                chat-widget .chatResp {
                    margin-right: 40px;
                    background-color: #F1F5FF;
                }
                .chat-loading {
                    width: 10px;
                    height: 10px;
                    background-color: #2980b9;
                    border-radius: 100%;
                    animation: chatLoding infinite 1s linear;
                }
                /* 闪烁动画 */
                @keyframes chatLoding {
                    0%, 100% {
                        opacity: 0;
                    }
                    50% {
                        opacity: 1;
                    }
                }
                chat-widget .chatResp pre,
                chat-widget .chatResp code {
                    white-space: pre-line;
                    border: none;
                }
                .chat-based-refs {
                    padding: 10px 0 5px;
                    font-weight: 600;
                    font-size: 18px;
                    color: #666;
                }
                .chat-note {
                    width: 92%;
                    margin: 10px auto 0;
                    font-size: 14px;
                    color: #888;
                    line-height: 18px;
                }
                .font-small {
                    font-size: 12px;
                }
                chat-widget .toggle-chat-modal {
                    position: fixed;
                    bottom: 20px;
                    right: 20px;
                    background-color: #2980b9;
                    border: none;
                    border-radius: 50%;
                    width: 41px;
                    height: 41px;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    cursor: pointer;
                    z-index: 1001;
                }
                chat-widget .toggle-chat-modal:hover {
                    /* transform: scale(1.1); */
                    background-color: #0068B6;
                }
                chat-widget .toggle-chat-modal:hover svg {
                    fill: #0068B6;
                }
            </style>
            <div class="chat-modal-mask" id="ChatModalMask">
            <div class="chat-modal" id="ChatModal">
                <div class="chat-box">
                    <div class="chat-title">
                        ${title}
                        <div class="chat-title-buttons">
                            <button class="chat-refresh" id="ChatRefresh">
                                <svg t="1744946723034" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="4686" width="22" height="22">
                                    <path d="M799.300267 881.220267c0 34.474667-27.306667 64-62.600534 64H287.300267c-33.723733 0-62.600533-27.886933-62.600534-64V205.141333h574.600534v676.078934zM348.296533 90.248533c0-3.2768 1.604267-6.5536 3.208534-9.8304 1.604267-1.6384 4.778667-3.2768 9.6256-3.2768h300.1344c6.417067 0 12.834133 4.9152 12.834133 13.1072V128h-325.802667V90.248533zM987.067733 128H749.568V90.248533C749.568 41.028267 711.031467 0 661.265067 0H361.130667C313.002667 0 272.861867 39.389867 272.861867 90.248533V128H36.932267c-12.868267 0-25.7024 6.5536-32.085334 19.694933-6.4512 11.4688-6.4512 26.282667 0 37.751467 6.382933 11.4688 19.217067 19.694933 32.085334 19.694933h112.3328v677.717334C149.265067 960 210.261333 1024 287.300267 1024h449.399466c75.434667 0 138.0352-62.3616 138.0352-141.141333V205.141333h112.3328C1007.957333 205.141333 1024 188.7232 1024 167.389867c0-21.333333-17.646933-39.389867-36.932267-39.389867zM512 830.3616c20.855467 0 36.932267-16.384 36.932267-37.751467V384c0-13.141333-6.4512-26.282667-19.285334-32.836267a37.0688 37.0688 0 0 0-36.898133 0c-11.264 6.587733-19.285333 19.694933-19.285333 32.836267v408.610133c0 21.333333 17.681067 37.751467 38.536533 37.751467z m-174.933333 0c9.6256 0 19.2512-3.2768 27.272533-11.502933 6.417067-6.5536 11.264-16.384 11.264-27.886934V384c0-13.141333-6.4512-26.282667-19.285333-32.836267a37.0688 37.0688 0 0 0-36.932267 0c-11.229867 6.587733-19.2512 19.694933-19.2512 32.836267v408.610133c-1.604267 21.333333 16.042667 37.751467 36.932267 37.751467z m349.866666 0c20.8896 0 36.932267-16.384 36.932267-37.751467V384c0-13.141333-6.417067-26.282667-19.2512-32.836267a37.0688 37.0688 0 0 0-36.932267 0c-11.229867 6.587733-19.2512 19.694933-19.2512 32.836267v408.610133c0 21.333333 17.646933 37.751467 38.5024 37.751467z" fill="#ffffff" p-id="4687"></path>
                                </svg>
                            </button>
                            <button class="full-screen-control" id="FullScreenControl">
                                <svg t="1743058599883" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="3635" width="24" height="24">
                                    <path d="M896.22 926.22H127.78a30 30 0 0 1-30-30V127.78a30 30 0 0 1 30-30h768.44a30 30 0 0 1 30 30v768.44a30 30 0 0 1-30 30z m-738.44-60h708.44V157.78H157.78z" p-id="3636" fill="#ffffff"></path>
                                    <path d="M253.28 383.73a16 16 0 0 0 16-16v-82.45a16 16 0 0 1 16-16h82.79a16 16 0 0 0 16-16v-32.56a16 16 0 0 0-16-16H253c-8.8 0-22.57 2.95-30.6 6.54l-11.14 11.14c-3.59 8-6.54 21.8-6.54 30.6v114.73a16 16 0 0 0 16 16zM269.28 655.78a16 16 0 0 0-16-16h-32.56a16 16 0 0 0-16 16V771c0 8.8 2.95 22.57 6.54 30.6l11.14 11.14c8 3.6 21.8 6.54 30.6 6.54h115.07a16 16 0 0 0 16-16v-32.56a16 16 0 0 0-16-16h-82.79a16 16 0 0 1-16-16zM770.72 639.78a16 16 0 0 0-16 16v82.94a16 16 0 0 1-16 16h-82.6a16 16 0 0 0-16 16v32.56a16 16 0 0 0 16 16H771c8.8 0 22.57-2.94 30.6-6.54l11.14-11.14c3.6-8 6.54-21.8 6.54-30.6V655.78a16 16 0 0 0-16-16zM754.72 367.73a16 16 0 0 0 16 16h32.56a16 16 0 0 0 16-16V253c0-8.8-2.94-22.57-6.54-30.6l-11.14-11.14c-8-3.59-21.8-6.54-30.6-6.54H656.12a16 16 0 0 0-16 16v32.56a16 16 0 0 0 16 16h82.6a16 16 0 0 1 16 16z" p-id="3637" fill="#ffffff"></path>
                                </svg>
                            </button>
                            <button class="close-chat-modal" id="CloseChatModal" aria-label="Close chat">
                                <svg viewBox="0 0 24 24" width="24" height="24">
                                    <path d="M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12 19 6.41z" fill="white"></path>
                                </svg>
                            </button>
                        </div>
                    </div>

                    <div class="chat-content">
                        <div class="chat-message">
                            <div class="chatResp">Hi there! How can I help?</div>
                        </div>
                    </div>

                    <div class="chat-input-box">
                        <div class="chat-input">
                            <textarea placeholder="${placeholder}"></textarea>
                            <button>
                                <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="19px" fill="#0068b6">
                                    <path d="M476.59 227.05l-.16-.07L49.35 49.84A23.56 23.56 0 0027.14 52 24.65 24.65 0 0016 72.59v113.29a24 24 0 0019.52 23.57l232.93 43.07a4 4 0 010 7.86L35.53 303.45A24 24 0 0016 327v113.31A23.57 23.57 0 0026.59 460a23.94 23.94 0 0013.22 4 24.55 24.55 0 009.52-1.93L476.4 285.94l.19-.09a32 32 0 000-58.8z"></path>
                                </svg>
                            </button>
                        </div>
                    </div>
                    <div class="chat-note">
                        <span>Responses are provided by Realtek's AI chatbot and may contain inaccuracies. Realtek is not liable for any damages from its use and offers no warranties.</span>
                    </div>
                </div>
                <div class="resize-modal">
                    <svg t="1742452608688" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="1577" id="mx_n_1742452608690" width="16" height="16">
                        <path d="M962 952.325l-900 9.675 900-890.325z" p-id="1578" fill="#0068b6" data-spm-anchor-id="a313x.search_index.0.i3.7a413a81ji8lWy"></path>
                    </svg>
                </div>
            </div>
            </div>
            <button class="toggle-chat-modal" id="ToggleChatModal">
                <svg viewBox="0 0 24 24" width="28.8" height="28.8" fill="#2980b9" stroke="white" stroke-width="2">
                    <path d="M21 11.5a8.38 8.38 0 0 1-.9 3.8 8.5 8.5 0 0 1-7.6 4.7 8.38 8.38 0 0 1-3.8-.9L3 21l1.9-5.7a8.38 8.38 0 0 1-.9-3.8 8.5 8.5 0 0 1 4.7-7.6 8.38 8.38 0 0 1 3.8-.9h.5a8.48 8.48 0 0 1 8 8v.5z"></path>
                </svg>
            </button>
        </chat-widget>
        `;

        document.body.insertAdjacentHTML('beforeend', chatWidgetHTML);
    }

    function textAreaResize(ele) {
        ele.style.height = 'auto';
        ele.style.height = ele.scrollHeight + 'px';
    }

    function chatWidgetInit(config) {
        const fetchChatAPI = config.makeRequest;
        const chatTitle = config.chatWidgetTitle || '智能小客服';
        const chatPlaceholder = config.chatWidgetPlaceholder || 'Type your question';

        inserChatWidgetHTML(chatTitle, chatPlaceholder);

        const userInput = document.querySelector('.chat-input textarea');
        const chatModal = document.getElementById('ChatModal');

        /* ============== update chat-note font-size ============== */
        const chatNote = chatModal.querySelector('.chat-note');
        function updateChatNoteFont() {
            if(chatModal.offsetWidth < 520) {
                chatNote.classList.add("font-small");
            }
            else {
                chatNote.classList.remove("font-small");
            }
        }

        const chatModalMask = document.getElementById('ChatModalMask');
        /* ============= show or hide ai chat modal ============= */
        const toggleChatModalButton = document.getElementById('ToggleChatModal');
        const wyGridContainer = document.querySelector('.wy-grid-for-nav');
        toggleChatModalButton.onclick = function() {
            if(chatModalMask.style.display === 'none' || chatModalMask.style.display === '') {
                chatModalMask.style.display = 'block';
                wyGridContainer.classList.add("prevent-manipulation");
                updateChatNoteFont();
            }
            else {
                chatModalMask.style.display = 'none';
            }
        };

        /* =========== close modal button click handler =========== */
        const closeChatModalButton = document.getElementById('CloseChatModal');
        closeChatModalButton.onclick = function() {
            chatModalMask.style.display = 'none';
            wyGridContainer.classList.remove("prevent-manipulation");
        };

        /* ====== update textarea height accrording to input ====== */
        userInput.addEventListener('input', function() {
            textAreaResize(userInput);
        });
        textAreaResize(userInput);

        /* ================ save ai chat hiatory ================ */
        let chatHistoryList = [{"role": "assistant", "content": "Hi there! How can I help?"}];
        function addChatHistory(role, text) {
            let forwardText = text.replace(/(\\n|\n)+/g, '').trim();
            let newChat = {
                "role":role,
                "content": forwardText
            };
            chatHistoryList.push(newChat);
            if (chatHistoryList.length > 6) {
                chatHistoryList.shift();
            }
        }

        const chatContent = document.querySelector('.chat-content');
        /* ================= clear all ai content ================= */
        const chatRefreshButton = document.getElementById('ChatRefresh');
        chatRefreshButton.addEventListener('click', () => {
            if (chatContent) {
                chatContent.innerHTML = '';
            }
            chatHistoryList = [];
        });

        /* =============== handle chat message send =============== */
        function linkListToHTML(linkList) {
            if(!linkList || linkList.length==0) {
                return `<p class="chatRefDocs"></p>`;
            }
            const linksHtml = linkList.map(link => {
                return `<a href="${link.url}" target="_blank">${link.title}</a>`
            }).join('<br>');
            return `
            <div class="chatRefDocs">
                <p class="chat-based-refs">Answer based on the following sources: </p>
                ${linksHtml}
            </div>`;
        }

        function handleChatResponse(response, element) {
            if(response) {
                if (response.status === "success") {
                    // console.time("executionTime");
                    let md2html = marked.parse(response.content);
                    // console.timeEnd("executionTime");
                    let linksHtml = linkListToHTML(response.docs);
                    element.innerHTML = `
                    <div class="chatResp">
                        ${md2html}
                        ${linksHtml}
                    </div>
                    `;
                    addChatHistory("assistant", response.content);
                } else if(response.status === "error") {
                    let msg = `${response.error.error_type}: ${response.error.message}`;
                    element.innerHTML = `
                    <div class="chatResp">${msg}</div>
                    `;
                    addChatHistory("assistant", `${msg}`);
                }
                else {
                    let msg = "Invalid AI chat response!";
                    element.innerHTML = `
                    <div class="chatResp">${msg}</div>
                    `;
                    addChatHistory("assistant", `${msg}`);
                }
            }
        }
        function handleChatError(error, element) {
            element.innerHTML = `
            <div class="chatResp">${error}</div>
            `;
            addChatHistory("assistant", error);
        }
        
        function sendChatMessage() {
            const inputText = userInput.value;
            if (inputText.trim() === '') return;
            userInput.value = '';

            const userMessage = document.createElement('div');
            userMessage.className = 'chat-message flex-justify-end';
            userMessage.innerHTML = `
            <div class="userText">${inputText}</div>
            `;
            chatContent.appendChild(userMessage);
            const chatHistoryCopy = [...chatHistoryList];
            addChatHistory("user", inputText);

            const chatMessage = document.createElement('div');
            chatMessage.className = 'chat-message';
			chatContent.appendChild(chatMessage);
            // Use the passed fetchChatAPI function
            if (typeof fetchChatAPI === 'function') {
                chatMessage.innerHTML = `
                <div class="chatResp"><p class="chat-loading"></p></div>
                `;
                fetchChatAPI(inputText.trim(), chatHistoryCopy)
                    .then(response => handleChatResponse(response, chatMessage))
                    .catch(error => handleChatError(error, chatMessage));
            }
            else {
                chatMessage.innerHTML = `
                <div class="chatResp">Invalid AI chat fetch function!</div>
                `;
                addChatHistory("assistant", "Invalid AI chat fetch function!");
            }
            chatMessage.scrollIntoView({ behavior: 'smooth' });
        }
        /* =============== triggle chat message send =============== */
        const sendButton = document.querySelector('.chat-input button');
        sendButton.onclick = sendChatMessage;
        userInput.addEventListener('keydown', function(event) {
            if (event.key === 'Enter' && !event.shiftKey) {
                event.preventDefault(); // Prevents newline in textarea
                sendChatMessage();
            }
        });

        /* ============== handle chat modal draggle ============= */
        const dragBar = chatModal.querySelector('.chat-title');
        let isDragging = false;
        let dragX, dragY;

        dragBar.addEventListener('mousedown', (e) => {
            e.preventDefault(); // 阻止默认行为，防止文本选择
            isDragging = true;
            dragX = e.clientX - chatModal.offsetLeft;
            dragY = e.clientY - chatModal.offsetTop;

            document.addEventListener('mousemove', startDrag);
            document.addEventListener('mouseup', stopDrag);
        });

        function startDrag(e) {
            if (!isDragging) return;
            chatModal.style.left = e.clientX - dragX + 'px';
            chatModal.style.top = e.clientY - dragY + 'px';
        }

        function stopDrag() {
            isDragging = false;
            document.removeEventListener('mousemove', startDrag);
            document.removeEventListener('mouseup', stopDrag);
        }

        /* ============= handle chat modal full screen ============= */
        const fullScreenControl = document.getElementById('FullScreenControl');
        fullScreenControl.addEventListener('click', function() {
            chatModal.style.top = '';
            chatModal.style.left = '';
            chatModal.classList.toggle('fullscreen-chat-modal');
            updateChatNoteFont();
        });

        /* =============== handle chat modal resize =============== */
        const resizeModalHandle = chatModal.querySelector('.resize-modal');
        let isResizing = false;
        let resizeX;
        let resizeY;
        let startWidth;
        let startHeight;

        function startResize(e) {
            if (!isResizing) return;
            $('body').css('user-select', 'none');
            chatModal.style.width = startWidth + (e.clientX - resizeX) + 'px';
            chatModal.style.height = startHeight + (e.clientY - resizeY) + 'px';
        }
        function stopResize() {
            isResizing = false;
            updateChatNoteFont();
            $('body').css('user-select', '');
            document.removeEventListener('mousemove', startResize);
            document.removeEventListener('mouseup', stopResize);
        }
        resizeModalHandle.addEventListener('mousedown', function(e) {
            isResizing = true;
            resizeX = e.clientX;
            resizeY = e.clientY;
            startWidth = parseInt(window.getComputedStyle(chatModal).width, 10);
            startHeight = parseInt(window.getComputedStyle(chatModal).height, 10);
            document.addEventListener('mousemove', startResize);
            document.addEventListener('mouseup', stopResize);
        });
    }

    // Expose the init function
    window.chatWidgetInit = chatWidgetInit;
})();
