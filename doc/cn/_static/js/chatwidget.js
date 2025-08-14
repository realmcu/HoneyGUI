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
                chat-widget .chat-content a,
                chat-widget .chat-content a:visited {
                    color: #2980b9;
                }
                chat-widget .chat-content a:hover,
                chat-widget .chat-content a:focus {
                    color: #3091d1;
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
                chat-widget .btn-hidden {
                    display: none;
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
                    white-space: pre-line;
                    border: none;
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
                            <button class="chat-send-btn" id="ChatSendBtn">
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
            <button class="toggle-chat-modal" id="ToggleChatModal">
                <svg t="1753839593628" class="icon" viewBox="0 0 1218 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="17319" width="24" height="24">
                    <path d="M326.183722 206.500257c19.568414 0 36.788619 9.523295 47.529504 24.177863a41.528523 41.528523 0 0 1 17.394146 18.698707l1.348046 3.391858 249.910392 711.072687c8.088278 22.916787-6.348863 48.529667-32.17917 57.22674-24.525746 8.262219-50.529994-1.652444-60.009803-22.351478l-1.304561-3.348373-64.793194-184.247491H160.460996l-64.706223 184.247491c-8.088278 22.916787-35.527543 34.440409-61.314364 25.699851-24.569231-8.262219-38.788945-31.787802-33.222819-53.791396l1.000164-3.47883L252.08466 252.81217a43.267938 43.267938 0 0 1 30.439756-26.91744A58.574787 58.574787 0 0 1 326.140237 206.500257h0.043485z m459.553336 201.511181c25.960763 0 47.225106 12.958639 49.312404 29.396106l0.130456 2.609122v549.046217c0 17.698544-22.177536 32.005229-49.44286 32.005229-25.960763 0-47.225106-12.915153-49.268918-29.396107l-0.217427-2.609122V440.060152c0-17.698544 22.177536-32.005229 49.486345-32.005229zM322.226554 350.741212l-127.760002 363.494165h255.563489L322.226554 350.697727zM1038.430514 105.52724a14.958966 14.958966 0 0 1 9.697236 9.56678l38.658489 117.801853 120.889315 42.87657a14.958966 14.958966 0 0 1-0.869708 28.482914l-118.410648 33.266304-36.658163 116.018954a14.958966 14.958966 0 0 1-28.439428 0.130456l-38.701975-117.758368-117.323515-37.223473a14.958966 14.958966 0 0 1-0.217426-28.395943l115.975468-39.049858 36.658162-115.975468a14.958966 14.958966 0 0 1 18.742193-9.740721zM772.387051 0.553569a10.436488 10.436488 0 0 1 6.653261 6.827202l22.04708 70.489776 72.490103 24.569232a10.436488 10.436488 0 0 1-0.391368 19.959782l-72.185706 21.48177-23.569067 70.794174a10.436488 10.436488 0 0 1-19.872812-0.173941l-22.04708-70.489777-70.228865-21.220858a10.436488 10.436488 0 0 1-0.434853-19.872811l70.794174-24.873629 23.525582-70.881145a10.436488 10.436488 0 0 1 13.219551-6.609775z" p-id="17320" fill="#ffffff"></path>
                </svg>
            </button>
        </chat-widget>
        `;

        document.body.insertAdjacentHTML('beforeend', chatWidgetHTML);
    }

    function textAreaResize(element) {
        element.style.height = 'auto';
        element.style.height = element.scrollHeight + 'px';
    }

    function chatWidgetInit(config) {
        const chatTitle = config.chatWidgetTitle || '智能小客服';
        const chatPlaceholder = config.chatWidgetPlaceholder || 'Type your question';
        const chatAIBase = config.chatAIBase;
        const isMultilingual = window.isMultilingual == "True";

        inserChatWidgetHTML(chatTitle, chatPlaceholder);

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
        const toggleChatModalButton = document.getElementById('ToggleChatModal');
        const wyGridContainer = document.querySelector('.wy-grid-for-nav');
        toggleChatModalButton.onclick = function() {
            if(chatModalMaskNode.style.display === 'none' || chatModalMaskNode.style.display === '') {
                chatModalMaskNode.style.display = 'block';
                wyGridContainer.classList.add("prevent-manipulation");
                updateChatNoteFont();
            }
            else {
                chatModalMaskNode.style.display = 'none';
            }
        };

        /* =========== close modal button click handler =========== */
        const closeChatModalButton = document.getElementById('CloseChatModal');
        closeChatModalButton.onclick = function() {
            chatModalMaskNode.style.display = 'none';
            wyGridContainer.classList.remove("prevent-manipulation");
        };

        /* ====== update textarea height accrording to input ====== */
        userInputNode.addEventListener('input', function() {
            textAreaResize(userInputNode);
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
        /* ================= clear all ai content ================= */
        const chatRefreshButton = document.getElementById('ChatRefresh');
        chatRefreshButton.addEventListener('click', () => {
            if (chatContentNode) {
                chatContentNode.innerHTML = '';
            }
            chatHistoryList = [];
            docRefsList = [];
        });

        /* =============== handle chat message send =============== */
        const docDomain = "https://docs.realmcu.com/";
        function linkListToHTML(list) {
            docRefsList = [];
            if (!list || list.length === 0) {
                return `<p class="chatRefDocs"></p>`;
            }

            let linksHtml = '';
            list.forEach(link => {
                const lowerTitle = link.title.toLowerCase().trim();
                const itemExists = docRefsList.some(obj => obj.title.toLowerCase().trim() === lowerTitle);
                if (!itemExists) {
                    docRefsList.push({"title": link.title, "url": link.url});
                    linksHtml += `<a href="${docDomain}${link.url}" target="_blank">${link.title}</a><br>`;
                }
            });

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
            let refsHtml = linkListToHTML(refs);
            element.insertAdjacentHTML('beforeend', refsHtml);
            addChatHistory("assistant", mdContent);
        }
        function onChatError(error, element) {
            element.innerHTML = `
            <div class="chatResp wy-text-danger">${error}</div>
            `;
            docRefsList = [];
            addChatHistory("assistant", error);
        }

        // Configure marked
        marked.setOptions({
            renderer: new marked.Renderer(),
            pedantic: false, //尽可能遵循原始 Markdown 语法，而不是 GitHub Flavored Markdown。如果设置为 true，则偏向于更严格的解析
            gfm: true, //启用 GitHub Flavored Markdown，如果需要 GFM 功能（如表格、任务列表等）可将其启用
            breaks: true, //启用 GFM 换行符解析。设置为 true 时，Markdown 文本中的新行符被视为 <br>
            smartypants: true, //使用智能排版标记替换引号和破折号为直观形式
            headerIds: false,
            mangle: true //对嵌入的电子邮件地址进行加密处理（防止抓取），实现简单的防邮件爬虫
        });

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

                element.innerHTML += `<div class="wy-text-danger">${error}</div>`;
                docRefsList = [];
                hasError = true;
                abortCtrl.abort();
            }

            try {
                sendButton.classList.add("btn-hidden");
                abortButton.classList.remove("btn-hidden");
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
                            const sanitizedHtml = DOMPurify.sanitize(rawHtml);
                            asstNode.innerHTML = marked.parse(sanitizedHtml);
                        } else if (parsedChunk.status === "error") {
                            // append html content in asstNode
                            appendErrorMsg(`Internal server error: ${parsedChunk.error.error_type}, ${parsedChunk.error.message}, please refresh and try again!`, asstNode);
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
                            appendErrorMsg(`Read data from the stream error: ${error}, please refresh and try again!`, asstNode);
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
                    onChatError(`An error occured with the ai chat fetch operation: ${error}. Please refresh and try again!`, messageNode);
                }
            }
            finally {
                abortButton.classList.add("btn-hidden");
                sendButton.classList.remove("btn-hidden");
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

            let curVersion = "";
            const rawBody = {
                aiEnv: "QA",          // aiEnv is PROD or QA
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
            let chatUrl = "https://wwwdev.realmcu.com/docs/aichatstream";
            if (rawBody.aiEnv.toLowerCase() == "prod") {
                chatUrl = "https://wwwqa.realmcu.com/docs/aichatstream";
            }
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
            if (event.key === 'Enter' && !event.shiftKey) {
                event.preventDefault(); // Prevents newline in textarea
                sendChatMessage();
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
