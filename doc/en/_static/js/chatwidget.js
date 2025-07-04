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

    function textAreaResize(element) {
        element.style.height = 'auto';
        element.style.height = element.scrollHeight + 'px';
    }

    function chatWidgetInit(config) {
        const chatTitle = config.chatWidgetTitle || '智能小客服';
        const chatPlaceholder = config.chatWidgetPlaceholder || 'Type your question';
        const chatAIBase = config.chatAIBase;

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
        });

        /* =============== handle chat message send =============== */
        function linkListToHTML(list) {
            if (!list || list.length === 0) {
                return `<p class="chatRefDocs"></p>`;
            }

            const linkSet = new Set();
            let linksHtml = '';

            list.forEach(link => {
                const lowerTitle = link.title.toLowerCase().trim();
                if (!linkSet.has(lowerTitle)) {
                    linkSet.add(lowerTitle);
                    linksHtml += `<a href="${link.url}" target="_blank">${link.title}</a><br>`;
                }
            });

            return `
            <div class="chatRefDocs">
                <p class="chat-based-refs">Answer based on the following sources: </p>
                ${linksHtml}
            </div>`;
        }
        function onChatSuccess(mdContent, refs, element) {
            let refsHtml = linkListToHTML(refs);
            element.insertAdjacentHTML('beforeend', refsHtml);
            addChatHistory("assistant", mdContent);
        }
        function onChatError(error, element) {
            element.innerHTML = `
            <div class="chatResp">${error}</div>
            `;
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

            const appendErrorMsg = (error, element) => {
                if (hasError) return;
                element.innerHTML += `<div>${error}</div>`;
                hasError = true;
                abortCtrl.abort();
            }

            try {
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
                            appendErrorMsg(`${parsedChunk.error.error_type}: ${parsedChunk.error.message}`, asstNode);
                        }
                    } catch (error) {
                        appendErrorMsg(`JSON Parsing Error: ${error.message}`, asstNode);
                    }
                };

                const reader = response.body.getReader();
                const decoder = new TextDecoder("utf-8");
                let pendingBuffer = '';

                // Continuously read data from the stream
                while (true) {
                    const { done, value } = await reader.read().catch(error => {
                        appendErrorMsg(`Read data from the stream error: ${error}, please refresh and try again!`, asstNode);
                        return { done: true };
                    });
                    if (done || hasError) break;
    
                    // Decode the current chunk
                    const newChunk = decoder.decode(value, { stream: true });
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

                // Handle final remaining buffer content
                // if (pendingBuffer.trim() && !hasError) {
                //     processChunk(pendingBuffer.trim());
                // }
                if (pendingBuffer && !hasError) {
                    processChunk(pendingBuffer);
                }

                reader.releaseLock();
                if (!hasError) {
                    onChatSuccess(mdChatText, refList, asstNode);
                }
            } catch (error) {
                onChatError(`An error occured with the ai chat fetch operation: ${error}. Please refresh and try again!`, messageNode)
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
                aiEnv: "PROD",          // aiEnv is PROD or QA
                rawData: question,      // required, 要詢問 AI 的問句
                docBase: chatAIBase,    // required, 知識庫id(測試區請使用 2039)
                docVersion: curVersion, // optional, 文檔版本號，若沒有則 AI 會以最新的版本回答
                // optional, 過去的對話紀錄，請符合範例的格式，
                // 目前可以輸入長度最多為 6 (三組 user-assistant 組合).若無對話紀錄則不需輸入
                historyMsg: histories,
            };
        
            const method = "POST";
            const headers = {
                'Content-Type': 'application/x-www-form-urlencoded'
            };
            const body = JSON.stringify(rawBody);
        
            // return fetch config
            return {
                url: 'https://wwwdev.realmcu.com/docs/aichatstream',
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
        const sendButton = document.querySelector('.chat-input button');
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
// $(document).ready(function () {
//     const chatAIBase = window.chatAIBase;
//     chatWidgetInit({
//         chatWidgetTitle: "Real AI",
//         chatWidgetPlaceholder: "Please type your question",
//         chatAIBase: chatAIBase
//     });
// })
