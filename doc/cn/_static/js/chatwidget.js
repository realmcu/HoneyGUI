(function() {
    function inserChatWidgetHTML(title, placeholder, docSeries) {
        const chatWidgetHTML = `
        <chat-widget>
            <style>
                .overY-clip {
                    overflow-y: clip;
                }
                chat-widget * {
                    box-sizing: border-box;
                    font-family: Lato,proxima-nova,Helvetica Neue,Arial,sans-serif !important;
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
                    width: 42%;
                    height: 80%;
                    max-width: 100%;
                    max-height: 100%;
                    min-width: 340px;
                    min-height: 450px;
                }
                chat-widget .fullscreen-chat-modal {
                    width: 100vw !important;
                    height: 100vh !important;
                }
                chat-widget .chat-modal button[disabled] {
                    cursor: not-allowed;
                }
                chat-widget .chat-modal button[disabled] svg path {
                    fill: #b0b0b0;
                }
                chat-widget .btn-hidden {
                    display: none;
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
                chat-widget .chat-content-box {
                    position: relative;
                    display: flex;
                    flex-direction: column;
                    flex-grow: 1;
                    background-color: #f8f8f8;
                    overflow-y: auto;
                }
                chat-widget .chat-content {
                    padding: 20px 10px;
                    overflow-y: auto;
                }
                .chat-content-gobtm {
                    display: none;
                    position: absolute;
                    bottom: 20px;
                    left: 50%;
                    transform: translateX(-50%);
                    align-items: center;
                    justify-content: center;
                    width: 40px;
                    height: 40px;
                    border: 0.5px solid #ced4da;
                    border-radius: 50%;
                    background: #fff;
                    box-shadow: 0 5px 5px -5px rgba(0, 0, 0, 0.1);
                }
                chat-widget .chat-content a,
                chat-widget .chat-content a:visited {
                    color: #2980b9;
                }
                chat-widget .chat-content a:hover,
                chat-widget .chat-content a:focus {
                    color: #3091d1;
                }
                .chat-switch-box {
                    width: 92%;
                    margin: 10px auto 0;
                    display: flex;
                    align-items: center;
                    justify-content: space-between;
                    color: #666;
                }
                .chat-switch-checkbox {
                    display: none;
                }
                .chat-switch-label {
                    position: relative;
                    display: inline-block;
                    width: 40px;
                    height: 20px;
                    background-color: gray;
                    border-radius: 10px;
                    cursor: pointer;
                    margin-top: 10px;
                }
                .chat-switch-label::after {
                    content: '';
                    position: absolute;
                    top: 2px;
                    left: 2px;
                    width: 16px;
                    height: 16px;
                    background-color: white;
                    border-radius: 50%;
                    transition: 0.5s;
                }
                .chat-switch-checkbox:checked + .chat-switch-label {
                    background-color: #0068b6;
                }
                .chat-switch-checkbox:checked + .chat-switch-label::after {
                    transform: translateX(20px);
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
                    bottom: 4px;
                    right: 8px;
                    border: none;
                    background: none;
                }
                chat-widget .chat-message {
                    display: flex;
                    justify-content: flex-start;
                    margin: 5px 0;
                    overflow-x: auto;
                }
                chat-widget .flex-justify-end {
                    justify-content: flex-end;
                }
                chat-widget .userText,
                chat-widget .chatResp {
                    display: block;
                    padding: 8px 12px;
                    border-radius: 5px;
                    line-height: 24px;
                }
                chat-widget .userText p, 
                chat-widget .chatResp p {
                    margin: 3px 0;
                    line-height: 24px;
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
                chat-widget .chatResp ol { list-style-type: decimal; }
                chat-widget .chatResp ol ol { list-style-type: lower-alpha; }
                chat-widget .chatResp ol ol ol { list-style-type: lower-roman; }
                chat-widget .chatResp ol ol ol ol { list-style-type: lower-greek }
                chat-widget .chatResp ul { list-style-type: disc; }
                chat-widget .chatResp ul ul { list-style-type: circle; }
                chat-widget .chatResp ul ul ul { list-style-type: square; }
                chat-widget .chatResp ul ol ul ul { list-style-type: disclosure-closed }
                chat-widget .chatResp li {
                    list-style: inherit;
                    list-style-position: outside;
                    margin-left: 16px;
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
                    color: #c9302c;
                    white-space: pre;
                    border: none;
                }
                chat-widget .chatResp code,
                chat-widget .chatResp table * {
                    font-size: 14px;
                    background: transparent !important;
                    background-color: transparent !important;
                }
                chat-widget .chatResp table th,
                chat-widget .chatResp table td {
                    padding: 4px 10px;
                }
                .chat-feedback-box {
                    display: flex;
                    // justify-content: space-between;
                    column-gap: 8px;
                    row-gap: 8px;
                    margin: 8px 0;
                    font-size: 14px;
                }
                .chat-feedback-box .chat-feedback-btn:first-child {
                    // margin-right: auto;
                }
                .chat-feedback-btn {
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    width: 76px;
                    padding: 3px 6px;
                    border: 1px solid #222e6e;
                    border-radius: 6px;
                    color: #222e6e;
                    background: transparent;
                }
                .chat-feedback-btn.btn-scored svg path {
                    fill: #222e6e;
                }
                .chat-feedback-icon {
                    position: relative;
                    top: 2px;
                    margin-right: 6px; 
                }

                .chatRefDocs ul li {
                    list-style: none;
                    list-style-position: inside;
                    margin-left: 0;
                }
                .chatRefDocs ul li a svg {
                    position: relative;
                    top: 5px;
                }
                chat-widget .chat-based-refs {
                    padding: 10px 0 5px;
                    font-weight: 600;
                    font-size: 18px;
                    color: #666;
                }
                chat-widget .chat-note {
                    width: 92%;
                    margin: 10px auto 0;
                    font-size: 14px;
                    color: #888;
                    line-height: 18px;
                }
                chat-widget .chat-ref-note {
                    margin-top: 10px;
                    font-size: 14px;
                    color: #888;
                    line-height: 18px;
                }
                chat-widget .chat-ref-note::before {
                    font-family: FontAwesome;
                    content: "\\f0a4";
                    display: inline-block;
                    color: #0068B6;
                    font-size: 16px;
                    margin-right: 5px;
                }
                .font-small {
                    font-size: 12px;
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

                    <div class="chat-content-box">
                        <div class="chat-content">
                            <div class="chat-message">
                                <div class="chatResp">Hi there! How can I help?</div>
                            </div>
                        </div>
                        <button class="chat-content-gobtm">
                            <svg t="1755678131909" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="9760" width="20" height="20">
                                <path d="M840.832 627.498667l-298.666667 298.666666a42.666667 42.666667 0 0 1-60.330666 0l-298.666667-298.666666a42.666667 42.666667 0 0 1 60.330667-60.330667L469.333333 793.002667V128a42.666667 42.666667 0 0 1 85.333334 0v665.002667l225.834666-225.834667a42.666667 42.666667 0 0 1 60.330667 60.330667z" fill="#515151" p-id="9761"></path>
                            </svg>
                        </button>
                    </div>

                    <div class="chat-switch-box">
                        <div>Only based on ${docSeries} document for Q&A</div>
                        <div class="chat-switch-button">
                            <input type="checkbox" class="chat-switch-checkbox" id="DocSwitch" checked>
                            <label for="DocSwitch" class="chat-switch-label"></label>
                        </div>
                    </div>

                    <div class="chat-input-box">
                        <div class="chat-input">
                            <textarea id="ChatInputText" placeholder="${placeholder}"></textarea>
                            <button class="chat-send-btn" disabled id="ChatSendBtn">
                                <svg t="1752557991558" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="21997" width="24" height="24">
                                    <path d="M478.4128 491.7248l-202.1376-30.1056a81.92 81.92 0 0 1-64.67584-52.38784L125.52192 178.4832c-7.8848-21.17632 2.49856-44.8512 23.22432-52.92032a39.38304 39.38304 0 0 1 31.90784 1.47456L878.592 475.15648c19.90656 9.9328 28.18048 34.48832 18.432 54.82496-3.8912 8.21248-10.40384 14.848-18.432 18.8416L180.6336 896.96256a39.77216 39.77216 0 0 1-53.6576-18.8416 41.7792 41.7792 0 0 1-1.45408-32.58368l86.07744-230.74816a81.92 81.92 0 0 1 64.67584-52.38784l202.1376-30.1056a20.48 20.48 0 0 0 0-40.5504z" p-id="21998" fill="#0068b6"></path>
                                </svg>
                            </button>
                            <button class="chat-abort-btn btn-hidden" id="ChatAbortBtn">
                                <svg t="1752485733661" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="13243" width="22" height="22">
                                    <path d="M791.893333 0H232.106667C105.813333 0 0 105.813333 0 232.106667v559.786666C0 918.186667 105.813333 1024 232.106667 1024h559.786666c129.706667 0 232.106667-105.813333 232.106667-232.106667V232.106667C1024 105.813333 918.186667 0 791.893333 0zM716.8 699.733333c0 10.24-6.826667 17.066667-17.066667 17.066667h-375.466666c-10.24 0-17.066667-6.826667-17.066667-17.066667v-375.466666c0-10.24 6.826667-17.066667 17.066667-17.066667h375.466666c10.24 0 17.066667 6.826667 17.066667 17.066667v375.466666z" fill="#c9302c" p-id="13244"></path>
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
            <button class="toggle-chat-modal" id="ToggleChatModal"></button>
        </chat-widget>
        `;

        document.body.insertAdjacentHTML('beforeend', chatWidgetHTML);
    }

    function textAreaResize(element) {
        element.style.height = 'auto';
        element.style.height = element.scrollHeight + 'px';
    }

    function debounce(func, wait) {
        let timeout;
        return function (...args) {
            const context = this;
            clearTimeout(timeout);
            timeout = setTimeout(() => func.apply(context, args), wait);
        };
    }

    function chatWidgetInit(config) {
        const chatTitle = config.chatWidgetTitle || 'Real AI';
        const chatPlaceholder = config.chatWidgetPlaceholder || 'Type your question';
        const chatAIBase = config.chatAIBase;
        const isMultilingual = window.isMultilingual == "True";
        let docTarget = "rtl87xxx";

        const aiEnvString = "PROD"; // aiEnv is PROD or QA
        // docDomain is "https://docs.realmcu.com/" or "https://docsqa.realmcu.com/"
        const docDomain = "https://docs.realmcu.com/";      // default is formal document
        let baseProxy = "https://www.realmcu.com/docs/";    // default is formal realmcu site
        if (aiEnvString.toLowerCase() === "qa") {
            baseProxy = "https://wwwdev.realmcu.com/docs/";
        } else if (docDomain.toLowerCase().includes("docsqa.realmcu.com")) {
            baseProxy = "https://wwwqa.realmcu.com/docs/";
        }
        let chatUrl = `${baseProxy}aichatstream`;
        let feedbackUrl = `${baseProxy}AIFeedback`;

        inserChatWidgetHTML(chatTitle, chatPlaceholder, docTarget);

        const userInputNode = document.querySelector('.chat-input textarea');
        const chatModalNode = document.getElementById('ChatModal');

        /* ============== update chat-note font-size ============== */
        const chatNoteNode = chatModalNode.querySelector('.chat-note');
        function updateChatNoteFont() {
            if(chatModalNode.offsetWidth < 520) {
                chatNoteNode.classList.add("font-small");
            }
            else {
                chatNoteNode.classList.remove("font-small");
            }
        }

        const chatModalMaskNode = document.getElementById('ChatModalMask');
        /* ============= show or hide ai chat modal ============= */
        // click mask(not modal) to close
        let isMouseDownInsideModal = false;
        chatModalMaskNode.addEventListener('mousedown', function(event) {
            // 检查鼠标是否在 modal 内按下
            isMouseDownInsideModal = chatModalNode.contains(event.target);
        });
        chatModalMaskNode.addEventListener('mouseup', function(event) {
            // 在 mask 中释放鼠标，并且鼠标没有在 modal 中按下时关闭
            if (!isMouseDownInsideModal && event.target === chatModalMaskNode) {
                userInputNode.blur();
                chatModalMaskNode.style.display = 'none';
                document.documentElement.classList.remove('overY-clip');
                wyGridContainer.classList.remove("prevent-manipulation");
            }
        });

        const toggleChatModalButton = document.getElementById('ToggleChatModal');
        const wyGridContainer = document.querySelector('.wy-grid-for-nav');
        toggleChatModalButton.onclick = function() {
            if(chatModalMaskNode.style.display === 'none' || chatModalMaskNode.style.display === '') {
                document.documentElement.classList.add('overY-clip');
                chatModalMaskNode.style.display = 'block';
                userInputNode.focus();
                wyGridContainer.classList.add("prevent-manipulation");
                updateChatNoteFont();
            }
            else {
                userInputNode.blur();
                chatModalMaskNode.style.display = 'none';
                document.documentElement.classList.remove('overY-clip');
                wyGridContainer.classList.remove("prevent-manipulation");
            }
        };

        /* =========== close modal button click handler =========== */
        const closeChatModalButton = document.getElementById('CloseChatModal');
        closeChatModalButton.onclick = function() {
            userInputNode.blur();
            chatModalMaskNode.style.display = 'none';
            document.documentElement.classList.remove('overY-clip');
            wyGridContainer.classList.remove("prevent-manipulation");
        };

        /* ====== update textarea height accrording to input ====== */
        userInputNode.addEventListener('input', function() {
            textAreaResize(userInputNode);
            if (userInputNode.value.trim() === '') {
                sendButton.disabled = true;
            } else {
                sendButton.disabled = false;
            }
        });
        textAreaResize(userInputNode);

        /* ================ save ai chat hiatory ================ */
        let chatHistoryList = [{"role": "assistant", "content": "Hi there! How can I help?"}];
        let docRefsList = [];
        function addChatHistory(role, text) {
            // let forwardText = text.replace(/(\\n|\n)+/g, '').trim();
            let forwardText = text;
            let newChat = {
                "role":role,
                "content": forwardText
            };
            chatHistoryList.push(newChat);
            if (chatHistoryList.length > 6) {
                chatHistoryList.shift();
            }
        }

        const chatContentNode = document.querySelector('.chat-content');
        /* ============ listen chat content box scroll ============ */
        const scrollBtn = document.querySelector('.chat-content-gobtm');
        function onContentBoxScroll() {
            if (chatContentNode.scrollTop + chatContentNode.clientHeight >= chatContentNode.scrollHeight) {
                scrollBtn.style.display = 'none';
            } else {
                scrollBtn.style.display = 'flex';
            }
        }
        const debouncedContentBoxScroll = debounce(onContentBoxScroll, 100);
        chatContentNode.addEventListener('scroll', debouncedContentBoxScroll);
        // click chat-content-gobtm to chat-content-box bottom
        scrollBtn.addEventListener('click', function() {
            chatContentNode.scrollTo({
                top: chatContentNode.scrollHeight,
                behavior: 'smooth'
            });
        });

        /* ================= clear all ai content ================= */
        const chatRefreshButton = document.getElementById('ChatRefresh');
        chatRefreshButton.addEventListener('click', () => {
            if (chatContentNode) {
                chatContentNode.innerHTML = '';
            }
            chatHistoryList = [];
            docRefsList = [];
        });

        /* =============== add feedback buttons =============== */
        const copySvg = `
        <svg t="1755669476415" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="8208" width="16" height="16" data-spm-anchor-id="a313x.search_index.0.i18.16ad3a81O4DMba">
            <path d="M878.272 981.312H375.36a104.64 104.64 0 0 1-104.64-104.64V375.36c0-57.792 46.848-104.64 104.64-104.64h502.912c57.792 0 104.64 46.848 104.64 104.64v502.912c-1.6 56.192-48.448 103.04-104.64 103.04z m-502.912-616.96a10.688 10.688 0 0 0-10.944 11.008v502.912c0 6.208 4.672 10.88 10.88 10.88h502.976c6.208 0 10.88-4.672 10.88-10.88V375.36a10.688 10.688 0 0 0-10.88-10.944H375.36z" fill="#222e6e" p-id="8209"></path><path d="M192.64 753.28h-45.312a104.64 104.64 0 0 1-104.64-104.64V147.328c0-57.792 46.848-104.64 104.64-104.64h502.912c57.792 0 104.64 46.848 104.64 104.64v49.92a46.016 46.016 0 0 1-46.848 46.912 46.08 46.08 0 0 1-46.848-46.848v-49.984a10.688 10.688 0 0 0-10.944-10.944H147.328a10.688 10.688 0 0 0-10.944 10.88v502.976c0 6.208 4.672 10.88 10.88 10.88h45.312a46.08 46.08 0 0 1 46.848 46.912c0 26.496-21.824 45.248-46.848 45.248z" fill="#222e6e" p-id="8210"></path>
        </svg>
        `;
        const copyDoneSvg = `
        <svg t="1755756250746" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="10267" width="17" height="17">
            <path d="M784 266.666667h-373.333333c-92.885333 0-144 51.114667-144 144v373.333333c0 92.885333 51.114667 144 144 144h373.333333c92.885333 0 144-51.114667 144-144v-373.333333c0-92.885333-51.114667-144-144-144z m80 517.333333c0 56.832-23.168 80-80 80h-373.333333c-56.832 0-80-23.168-80-80v-373.333333c0-56.832 23.168-80 80-80h373.333333c56.832 0 80 23.168 80 80v373.333333zM160 239.786667v373.76c0 51.114667 20.608 63.701333 27.392 67.882666a32 32 0 1 1-33.450667 54.570667c-38.442667-23.552-57.941333-64.725333-57.941333-122.453333V239.786667c0-91.392 52.48-143.786667 143.786667-143.786667h373.76c71.893333 0 106.24 31.530667 122.453333 57.941333a32 32 0 0 1-54.613333 33.408c-4.096-6.784-16.725333-27.392-67.84-27.392H239.786667c-56.661333 0.042667-79.786667 23.168-79.786667 79.829334z m566.613333 263.808a32 32 0 0 1 0 45.269333l-142.208 142.208a31.957333 31.957333 0 0 1-45.226666 0L468.053333 619.946667a32 32 0 1 1 45.269334-45.269334l48.512 48.469334 119.594666-119.594667a31.957333 31.957333 0 0 1 45.184 0.042667z" fill="#222e6e" p-id="10268"></path>
        </svg>
        `;
        const copyFailSvg = `
        <svg t="1755756470581" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="21471" width="17" height="17">
            <path d="M640.79 54l0.25 0.004a32.569 32.569 0 0 1 0.643 0.015l0.216 0.009c3.454 0.141 6.767 0.83 9.854 1.983 0.02 0.009 0.04 0.016 0.061 0.024 0.131 0.048 0.261 0.098 0.39 0.149l0.092 0.036a30.076 30.076 0 0 1 1.103 0.458l0.106 0.047a31.824 31.824 0 0 1 8.214 5.266l0.123 0.11c0.325 0.288 0.645 0.585 0.96 0.89l0.378 0.372 213.437 213.25 0.063 0.062 0.262 0.266-0.325-0.328a32.645 32.645 0 0 1 0.69 0.71l0.075 0.08a31.898 31.898 0 0 1 4.865 6.793l0.049 0.093A31.865 31.865 0 0 1 886 298.72V896.35c0 41.235-33.444 74.65-74.688 74.65H213.688C172.446 971 139 937.585 139 896.35v-767.7C139 87.416 172.445 54 213.688 54h427.101z m-32.228 64H213.687C207.78 118 203 122.774 203 128.65v767.7c0 5.876 4.778 10.65 10.688 10.65h597.625c5.909 0 10.687-4.774 10.687-10.65v-565.1H640.562c-17.496 0-31.713-14.042-31.995-31.47l-0.005-0.53V118zM663.2 447.233c12.366 12.377 12.481 32.359 0.351 44.877l-0.371 0.377L557.774 597.8 663.18 703.113c12.502 12.49 12.511 32.752 0.02 45.254-12.367 12.378-32.348 12.51-44.878 0.391l-0.377-0.37L512.5 643.033 407.055 748.387c-12.502 12.492-32.764 12.483-45.255-0.02-12.366-12.377-12.481-32.359-0.351-44.877l0.371-0.377L467.225 597.8 361.82 492.487c-12.502-12.49-12.511-32.752-0.02-45.254 12.367-12.378 32.348-12.51 44.878-0.391l0.377 0.37L512.5 552.566l105.445-105.352c12.502-12.492 32.764-12.483 45.255 0.02z m9.362-284.027V267.25h104.135L672.562 163.206z" p-id="21472" fill="#222e6e"></path>
        </svg>
        `;
        const likeSvg = `
        <svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" aria-hidden="true" role="img" class="text-sm iconify iconify--icon-park-solid" width="18px" height="18px" viewBox="0 0 48 48">
            <path fill="none" stroke="#222e6e" stroke-linecap="round" stroke-linejoin="round" stroke-width="4" d="M4.189 22.173A2 2 0 0 1 6.181 20H10a2 2 0 0 1 2 2v19a2 2 0 0 1-2 2H7.834a2 2 0 0 1-1.993-1.827zM18 21.375c0-.836.52-1.584 1.275-1.94c1.649-.778 4.458-2.341 5.725-4.454c1.633-2.724 1.941-7.645 1.991-8.772c.007-.158.003-.316.024-.472c.271-1.953 4.04.328 5.485 2.74c.785 1.308.885 3.027.803 4.37c-.089 1.436-.51 2.823-.923 4.201l-.88 2.937h10.857a2 2 0 0 1 1.925 2.543l-5.37 19.016A2 2 0 0 1 36.986 43H20a2 2 0 0 1-2-2z"/>
        </svg>
        `;
        const dislikeSvg = `
        <svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" aria-hidden="true" role="img" class="text-sm iconify iconify--icon-park-outline" width="18px" height="18px" viewBox="0 0 48 48">
            <path fill="none" stroke="#222e6e" stroke-linecap="round" stroke-linejoin="round" stroke-width="4" d="M4.18 26.834A2 2 0 0 0 6.175 29H10a2 2 0 0 0 2-2V7a2 2 0 0 0-2-2H7.84a2 2 0 0 0-1.993 1.834zM18 26.626c0 .835.52 1.583 1.275 1.94c1.649.777 4.458 2.34 5.725 4.454c1.633 2.723 1.941 7.644 1.991 8.771c.007.158.003.316.024.472c.271 1.953 4.04-.328 5.485-2.74c.785-1.308.885-3.027.803-4.37c-.089-1.435-.51-2.823-.923-4.201l-.88-2.937h10.857a2 2 0 0 0 1.925-2.543l-5.37-19.016A2 2 0 0 0 36.986 5H20a2 2 0 0 0-2 2z"></path>
        </svg>
        `;
        const loadingSvg = `
        <svg t="1757316892265" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="4636" width="18" height="18">
            <path d="M469.333333 85.333333m42.666667 0l0 0q42.666667 0 42.666667 42.666667l0 128q0 42.666667-42.666667 42.666667l0 0q-42.666667 0-42.666667-42.666667l0-128q0-42.666667 42.666667-42.666667Z" fill="#13227a" opacity=".8" p-id="4637"></path><path d="M469.333333 725.333333m42.666667 0l0 0q42.666667 0 42.666667 42.666667l0 128q0 42.666667-42.666667 42.666667l0 0q-42.666667 0-42.666667-42.666667l0-128q0-42.666667 42.666667-42.666667Z" fill="#13227a" opacity=".4" p-id="4638" data-spm-anchor-id="a313x.search_index.0.i1.16ad3a81Tc0Y2F" class=""></path><path d="M938.666667 469.333333m0 42.666667l0 0q0 42.666667-42.666667 42.666667l-128 0q-42.666667 0-42.666667-42.666667l0 0q0-42.666667 42.666667-42.666667l128 0q42.666667 0 42.666667 42.666667Z" fill="#13227a" opacity=".2" p-id="4639"></path><path d="M298.666667 469.333333m0 42.666667l0 0q0 42.666667-42.666667 42.666667l-128 0q-42.666667 0-42.666667-42.666667l0 0q0-42.666667 42.666667-42.666667l128 0q42.666667 0 42.666667 42.666667Z" fill="#13227a" opacity=".6" p-id="4640"></path><path d="M783.530667 180.138667m30.169889 30.169889l0 0q30.169889 30.169889 0 60.339779l-90.509668 90.509668q-30.169889 30.169889-60.339779 0l0 0q-30.169889-30.169889 0-60.339779l90.509668-90.509668q30.169889-30.169889 60.339779 0Z" fill="#13227a" opacity=".1" p-id="4641"></path><path d="M330.965333 632.661333m30.16989 30.16989l0 0q30.169889 30.169889 0 60.339778l-90.509668 90.509668q-30.169889 30.169889-60.339779 0l0 0q-30.169889-30.169889 0-60.339778l90.509668-90.509668q30.169889-30.169889 60.339779 0Z" fill="#13227a" opacity=".5" p-id="4642"></path><path d="M843.861333 783.530667m-30.169889 30.169889l0 0q-30.169889 30.169889-60.339779 0l-90.509668-90.509668q-30.169889-30.169889 0-60.339779l0 0q30.169889-30.169889 60.339779 0l90.509668 90.509668q30.169889 30.169889 0 60.339779Z" fill="#13227a" opacity=".3" p-id="4643"></path><path d="M391.338667 330.965333m-30.16989 30.16989l0 0q-30.169889 30.169889-60.339778 0l-90.509668-90.509668q-30.169889-30.169889 0-60.339779l0 0q30.169889-30.169889 60.339778 0l90.509668 90.509668q30.169889 30.169889 0 60.339779Z" fill="#13227a" opacity=".7" p-id="4644"></path>
        </svg>
        `;
        const feeddbackHtml = `
        <div class="chat-feedback-box">
            <button class="chat-feedback-btn" id="CopyButton">
                <span class="chat-feedback-icon">${copySvg}</span>
                <span class="chat-feedback-text">Copy</span>
            </button>
            <button class="chat-feedback-btn" id="LikeButton" value="positive">
                <span class="chat-feedback-icon">${likeSvg}</span>
                <span class="chat-feedback-text">Like</span>
            </button>
            <button class="chat-feedback-btn" id="DislikeButton" value="negative">
                <span class="chat-feedback-icon">${dislikeSvg}</span>
                <span class="chat-feedback-text">Dislike</span>
            </button>
        </div>
        `;

        function updateFeedButton(element, icon, text) {
            element.html(`
                <span class="chat-feedback-icon">${icon}</span>
                <span class="chat-feedback-text">${text}</span>
            `);
        }

        /* =============== listen feedback button click =============== */
        $(".chat-content").on("click", "#CopyButton", function(e) {
            const copyBtn = $(this);
            const chatRespMain = copyBtn.closest('.chatResp');
            if (chatRespMain.length && chatRespMain[0].ansData) {
                const ansData = chatRespMain[0].ansData;
                navigator.clipboard.writeText(ansData).then(() => {
                    updateFeedButton(copyBtn, copyDoneSvg, "Copied");
                }).catch((error) => {
                    updateFeedButton(copyBtn, copyFailSvg, "Failed");
                }).finally(() => {
                    setTimeout(() => {
                        updateFeedButton(copyBtn, copySvg, "Copy");
                    }, 2000);
                });
            } else {
                updateFeedButton(copyBtn, copyFailSvg, "Failed");
                setTimeout(() => {
                    updateFeedButton(copyBtn, copySvg, "Copy");
                }, 2000);
            }
        });

        async function aiFeedback(messageId, userReaction) {
            let resFeedBack = {
                status: "error",
                massage: ""
            };

            const rawBody = {
                aiEnv: aiEnvString,     // aiEnv is PROD or QA
                messageId: messageId,   // 当前 session 的标识符
                userReaction: userReaction,     // 用户反馈，例如 "negative"
            };
            try {
                const response = await fetch(feedbackUrl, {
                    method: "POST",
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded'
                    },
                    body: JSON.stringify(rawBody)
                });
                if(response.ok) {
                    const parsedData = await response.json();
                    if(parsedData.status == "success") {
                        resFeedBack.status = "success";
                        resFeedBack.message = "feedback success!";
                    }
                    else {
                        resFeedBack.message = parsedData.error.message;
                    }
                } else {
                    resFeedBack.message = `Http error with status: ${response.status}, please refresh and try again!`;
                }
            } catch (error) {
                resFeedBack.message = "Internal server error, please refresh and try again!";
            }
            return resFeedBack;
        }
        $(".chat-content").on("click", "#LikeButton, #DislikeButton", function(e) {
            e.preventDefault();
            const opBtn = $(this);
            const chatRespMain = opBtn.closest('.chatResp');
            if (!chatRespMain.length) return;

            const opIcon = opBtn.find('.chat-feedback-icon').first();
            const opIconHTML = opIcon.html();
            const opText = opBtn.find('.chat-feedback-text').first();
            const opTextHTML = opText.html();

            updateFeedButton(opBtn, loadingSvg, opTextHTML);
            const messageId = chatRespMain[0].id;
            const userReaction = opBtn.val();
            aiFeedback(messageId, userReaction).then(resFeedBack => {
                if (resFeedBack.status == "success") {
                    opBtn.addClass("btn-scored").siblings().removeClass("btn-scored");
                    updateFeedButton(opBtn, opIconHTML, opTextHTML);
                } else {
                    updateFeedButton(opBtn, opIconHTML, "Failed");
                    setTimeout(() => {
                        updateFeedButton(opBtn, opIconHTML, opTextHTML);
                    }, 2000);
                }
            }).catch(error => {
                updateFeedButton(opBtn, opIconHTML, "Failed");
                setTimeout(() => {
                    updateFeedButton(opBtn, opIconHTML, opTextHTML);
                }, 2000);
            });
        });

        function linkListToHTML(list) {
            docRefsList = [];
            if (!list || list.length === 0) {
                return `<p class="chatRefDocs"></p>`;
            }

            let linksHtml = '<ul>';
            list.forEach(link => {
                const lowerTitle = link.title.toLowerCase().trim();
                const itemExists = docRefsList.some(obj => obj.title.toLowerCase().trim() === lowerTitle);
                if (!itemExists) {
                    docRefsList.push({"title": link.title, "url": link.url});
                    linksHtml += `
                    <li>
                        <a href="${docDomain}${link.url}" target="_blank">
                            <svg t="1755599908584" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="2773" width="18" height="18"><path d="M633.417143 429.007238a174.567619 174.567619 0 0 1 0 246.857143l-155.306667 155.306667a186.709333 186.709333 0 1 1-264.045714-264.045715l76.483048-76.507428 51.73638 51.736381-76.507428 76.507428a113.566476 113.566476 0 1 0 160.597333 160.597334l155.306667-155.306667a101.424762 101.424762 0 0 0 0-143.408762z m208.603428-225.816381a186.709333 186.709333 0 0 1 0 264.045714L765.561905 543.744l-51.736381-51.712 76.507428-76.507429a113.566476 113.566476 0 1 0-160.597333-160.597333l-155.306667 155.306667a101.424762 101.424762 0 0 0 0 143.408762l-51.736381 51.736381a174.567619 174.567619 0 0 1 0-246.857143l155.306667-155.306667a186.709333 186.709333 0 0 1 264.045714 0z" p-id="2774" fill="#0068B6"></path></svg>
                            ${link.title}
                        </a>
                    </li>`;
                }
            });
            linksHtml += '</ul>';

            let refNoteHtml = "";
            if(isMultilingual) {
                refNoteHtml = `<div class="chat-ref-note">Please use the "EN/中文" button (if available) in the upper right corner of the document to switch language.</div>`;
            }

            return `
            <div class="chatRefDocs">
                <p class="chat-based-refs">Answer based on the following sources: </p>
                ${linksHtml}
                ${refNoteHtml}
            </div>`;
        }
        function onChatSuccess(mdContent, refs, element) {
            element.insertAdjacentHTML('beforeend', feeddbackHtml);
            let refsHtml = linkListToHTML(refs);
            element.insertAdjacentHTML('beforeend', refsHtml);
            addChatHistory("assistant", mdContent);
        }
        function onChatError(error, element) {
            element.innerHTML = `
            <div class="chatResp">${error}</div>
            `;
            docRefsList = [];
            addChatHistory("assistant", error);
        }

        // Configure marked
        const mdRenderer = new marked.Renderer();
        mdRenderer.link = function({href, title, text}) {
            return `<a href="${href}" target="_blank" rel="noopener noreferrer"${title ? ` title="${title}"` : ''}>${text}</a>`;
        };
        marked.setOptions({
            renderer: mdRenderer,
            pedantic: false, //尽可能遵循原始 Markdown 语法，而不是 GitHub Flavored Markdown。如果设置为 true，则偏向于更严格的解析
            gfm: true, //启用 GitHub Flavored Markdown，如果需要 GFM 功能（如表格、任务列表等）可将其启用
            breaks: true, //启用 GFM 换行符解析。设置为 true 时，Markdown 文本中的新行符被视为 <br>
            smartypants: true, //使用智能排版标记替换引号和破折号为直观形式
            headerIds: false,
            mangle: true //对嵌入的电子邮件地址进行加密处理（防止抓取），实现简单的防邮件爬虫
        });

        /* =============== handle chat message send =============== */
        let isSending = false; // sending message status param
        async function fetchChatAnwser(fetchConfig, messageNode) {
            const { url, options } = fetchConfig;
            const abortCtrl = new AbortController();
            let hasError = false;
            let mdChatText = "";
            let refList = [];

            /* ================ Interrupt chat request ================ */
            const abortButton = document.getElementById('ChatAbortBtn');
            abortButton.addEventListener('click', () => {
                abortCtrl.abort(); // 中止 fetch 请求
            });

            const appendErrorMsg = (error, element) => {
                if (hasError) return;

                element.innerHTML += `<div>${error}</div>`;
                docRefsList = [];
                hasError = true;
                abortCtrl.abort();
            }

            const updateSendingStatus = (startSending) => {
                if(startSending) {
                    isSending = true;
                    chatRefreshButton.disabled = true;
                    sendButton.classList.add("btn-hidden");
                    abortButton.classList.remove("btn-hidden");
                } else {
                    chatRefreshButton.disabled = false;
                    abortButton.classList.add("btn-hidden");
                    sendButton.classList.remove("btn-hidden");
                    isSending = false;
                }
            }

            try {
                updateSendingStatus(true);
                // Init AI chat request and add abort controller
                const response = await fetch(url, { 
                    ...options, 
                    signal: abortCtrl.signal 
                });
                if (!response.ok) {
                    return onChatError(`Http error with status: ${response.status}, please refresh and try again!`, messageNode);
                }

                // Init massage container
                messageNode.innerHTML = `<div class="chatResp"></div>`;
                const asstNode = messageNode.querySelector(".chatResp");
                if (!asstNode) {
                    console.error("Assistant feedback element not found, please refresh and try again!");
                    return;
                }

                /* ===============================
                    parsed data chunk example:
                    {
                        "role": "assistant",
                        "content": "xxxxxx",
                        "docs": [{"url": "xxxx", "title": "xxxx"}],
                        "status": "success",    // success or error
                        "error": null           // null or {"error_type": "xxxx", "error_message": "xxxx"}
                    }
                  ===============================*/
                const processChunk = (lineChunk) => {
                    try {
                        const parsedChunk = JSON.parse(lineChunk);
                        if (parsedChunk.status === "success") {
                            mdChatText = parsedChunk.content;
                            refList = parsedChunk.docs;
                            // replace asstNode html content
                            // asstNode.innerHTML = marked.parse(mdChatText);
                            const rawHtml = marked.parse(mdChatText);
                            // const sanitizedHtml = DOMPurify.sanitize(rawHtml);
                            // asstNode.innerHTML = sanitizedHtml;
                            asstNode.id = parsedChunk.message_id || "";
                            asstNode.innerHTML = rawHtml;
                            asstNode.ansData = mdChatText;
                        } else if (parsedChunk.status === "error") {
                            // append html content in asstNode
                            // appendErrorMsg(`${parsedChunk.error.error_type}: ${parsedChunk.error.message}`, asstNode);
                            appendErrorMsg(`Internal server error, please refresh and try again!`, asstNode);
                        }
                    } catch (error) {
                        // go on
                    }
                };

                const reader = response.body.getReader();
                const decoder = new TextDecoder("utf-8");
                let pendingBuffer = '';

                // Continuously read data from the stream
                while (true) {
                    const { done, value } = await reader.read().catch(error => {
                        if (error.name === 'AbortError') {
                            appendErrorMsg(`AI fetch request has been aborted!`, asstNode);
                        } else {
                            appendErrorMsg(`Failed to read data stream, please refresh and try again!`, asstNode);
                        }
                        return { done: true };
                    });
                    if (done || hasError) break;

                    // Decode the current chunk
                    const newChunk = decoder.decode(value, { stream: true });
                    processChunk(newChunk);

                    pendingBuffer += newChunk;
                    let lineEnd;
                    while ((lineEnd = pendingBuffer.indexOf('\n')) >= 0) {
                        // const lineChunk = pendingBuffer.slice(0, lineEnd).trim();
                        const lineChunk = pendingBuffer.slice(0, lineEnd);
                        pendingBuffer = pendingBuffer.slice(lineEnd + 1);

                        if (!lineChunk) continue;

                        processChunk(lineChunk);
                    }
                }

                if (pendingBuffer && !hasError) {
                    processChunk(pendingBuffer);
                }

                reader.releaseLock();
                if (!hasError) {
                    onChatSuccess(mdChatText, refList, asstNode);
                }
            } catch (error) {
                if (error.name === 'AbortError') {
                    onChatError(`AI fetch request has been aborted!`, messageNode);
                } else {
                    onChatError(`An error occured with the ai chat fetch operation, please refresh and try again!`, messageNode);
                }
            }
            finally {
                updateSendingStatus(false);
                onContentBoxScroll();
            }
        }

        function AIChatFetchConfig(question, histories) {
            // // get current doc version value
            // let curVersion = (document.querySelector("#version-selector")?.value) || "latest";
            // // If the current version is "latest", check and update it from LatestVersion
            // if (curVersion === "latest") {
            //     const versionNode = document.querySelector('#LatestVersion');
            //     if (versionNode && versionNode.value) {
            //         curVersion = versionNode.value;
            //     }
            // }

            let docSwitch = document.getElementById('DocSwitch');
            let curVersion = "";
            const rawBody = {
                aiEnv: aiEnvString,     // aiEnv is PROD or QA
                docTarget: docSwitch.checked ? docTarget : '',   // optional, 文档标识，用于文档分类
                rawData: question,      // required, 要詢問 AI 的問句
                docBase: chatAIBase,    // required, 知識庫id(測試區請使用 2039)
                docVersion: curVersion, // optional, 文檔版本號，若沒有則 AI 會以最新的版本回答
                // optional, 過去的對話紀錄，請符合範例的格式，
                // 目前可以輸入長度最多為 6 (三組 user-assistant 組合).若無對話紀錄則不需輸入
                historyMsg: histories,
                // previous reference doc links list
                docRefs: JSON.stringify(docRefsList).replace(/"/g, "'"),
            };
        
            const method = "POST";
            const headers = {
                'Content-Type': 'application/x-www-form-urlencoded'
            };
            const body = JSON.stringify(rawBody);
            // return fetch config
            return {
                url: chatUrl,
                options: {
                    method,
                    headers,
                    body
                }
            };
        }
        
        function sendChatMessage() {
            const userInputValue = userInputNode.value;
            if (userInputValue.trim() === '') return;
            userInputNode.value = '';

            const userMessageNode = document.createElement('div');
            userMessageNode.className = 'chat-message flex-justify-end';
            userMessageNode.innerHTML = `
            <div class="userText">${userInputValue}</div>
            `;
            chatContentNode.appendChild(userMessageNode);
            const chatHistoryCopy = [...chatHistoryList];
            addChatHistory("user", userInputValue);

            const chatMessageNode = document.createElement('div');
            chatMessageNode.className = 'chat-message';
            chatContentNode.appendChild(chatMessageNode);
            // Use the passed fetchChatAPI function
            chatMessageNode.innerHTML = `<div class="chatResp"><p class="chat-loading"></p></div>`;
            
            const fetchConfig = AIChatFetchConfig(userInputValue, chatHistoryCopy);
            fetchChatAnwser(fetchConfig, chatMessageNode);

            chatMessageNode.scrollIntoView({ behavior: 'smooth' });
        }
        /* =============== triggle chat message send =============== */
        const sendButton = document.getElementById('ChatSendBtn');
        sendButton.onclick = sendChatMessage;
        userInputNode.addEventListener('keydown', function(event) {
            if (event.key === 'Enter') {
                if(isSending) {
                    // add newline in textarea
                }
                else {
                    if (userInputNode.value.trim() === '') {
                        // add newline in textarea
                    } else {
                        event.preventDefault(); // Prevents newline in textarea
                        sendChatMessage();
                    }
                }
            }
        });

        /* ============== handle chat modal draggle ============= */
        const dragBar = chatModalNode.querySelector('.chat-title');
        let isDragging = false;
        let dragX, dragY;

        dragBar.addEventListener('mousedown', (e) => {
            e.preventDefault(); // 阻止默认行为，防止文本选择
            isDragging = true;
            dragX = e.clientX - chatModalNode.offsetLeft;
            dragY = e.clientY - chatModalNode.offsetTop;

            document.addEventListener('mousemove', startDrag);
            document.addEventListener('mouseup', stopDrag);
        });

        function startDrag(e) {
            if (!isDragging) return;
            chatModalNode.style.left = e.clientX - dragX + 'px';
            chatModalNode.style.top = e.clientY - dragY + 'px';
        }

        function stopDrag() {
            isDragging = false;
            document.removeEventListener('mousemove', startDrag);
            document.removeEventListener('mouseup', stopDrag);
        }

        /* ============= handle chat modal full screen ============= */
        const fullScreenCtrlNode = document.getElementById('FullScreenControl');
        fullScreenCtrlNode.addEventListener('click', function() {
            chatModalNode.style.top = '';
            chatModalNode.style.left = '';
            chatModalNode.classList.toggle('fullscreen-chat-modal');
            updateChatNoteFont();
        });

        /* =============== handle chat modal resize =============== */
        const resizeModalCtrlNode = chatModalNode.querySelector('.resize-modal');
        let isResizing = false;
        let resizeX;
        let resizeY;
        let startWidth;
        let startHeight;

        function startResize(e) {
            if (!isResizing) return;
            $('body').css('user-select', 'none');
            chatModalNode.classList.remove('fullscreen-chat-modal');
            chatModalNode.style.width = startWidth + (e.clientX - resizeX) + 'px';
            chatModalNode.style.height = startHeight + (e.clientY - resizeY) + 'px';
        }
        function stopResize() {
            isResizing = false;
            updateChatNoteFont();
            $('body').css('user-select', '');
            document.removeEventListener('mousemove', startResize);
            document.removeEventListener('mouseup', stopResize);
        }
        resizeModalCtrlNode.addEventListener('mousedown', function(e) {
            isResizing = true;
            resizeX = e.clientX;
            resizeY = e.clientY;
            startWidth = parseInt(window.getComputedStyle(chatModalNode).width, 10);
            startHeight = parseInt(window.getComputedStyle(chatModalNode).height, 10);
            document.addEventListener('mousemove', startResize);
            document.addEventListener('mouseup', stopResize);
        });
    }

    // Expose the init function
    window.chatWidgetInit = chatWidgetInit;
})();

/* ============== Init AI ASK Component ============== */
$(document).ready(function () {
    const chatAIBase = window.chatAIBase;
    chatWidgetInit({
        chatWidgetTitle: "Real AI",
        chatWidgetPlaceholder: "Please type your question",
        chatAIBase: chatAIBase
    });
})
