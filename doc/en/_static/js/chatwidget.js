(function() {
    function inserChatWidgetHTML(logo, title, placeholder, docSeries) {
        const chatWidgetHTML = `
        <chat-widget>
            <style>
                chat-widget {
                    --rtk-blue: #0068b6;
                }
                .overY-clip {
                    overflow-y: clip;
                }
                chat-widget * {
                    box-sizing: border-box;
                    font-family: Lato,proxima-nova,Helvetica Neue,Arial,sans-serif !important;
                }
                chat-widget h1,chat-widget h2,chat-widget h3,chat-widget h4,chat-widget h5,chat-widget h6 {
                    font-weight: 700;
                    margin: 16px 0;
                }
                chat-widget h1 { font-size: 150% }
                chat-widget h2 { font-size: 125% }
                chat-widget h3 { font-size: 115% }
                chat-widget h4 { font-size: 110% }
                chat-widget h5,chat-widget h6 { font-size: 100% }

                .chat-modal-mask {
                    display: none; /* Initially hidden */
                    width: 100%;
                    height: 100%;
                    background-color: rgba(0,0,0,0.4);
                    position: fixed;
                    top: 0;
                    left: 0;
                    z-index: 10001;
                }
                chat-widget .chat-modal {
                    position: absolute;
                    top: 50%;
                    left: 50%;
                    transform: translate(-50%, -50%);
                    width: 50%;
                    height: 80%;
                    max-width: 100%;
                    max-height: 100%;
                    min-width: 375px;
                    min-height: 450px;
                }
                chat-widget .fullscreen-chat-modal {
                    width: 100vw !important;
                    height: 100vh !important;
                }
                chat-widget .chat-modal button[disabled] {
                    cursor: not-allowed;
                }
                chat-widget .btn-hidden {
                    display: none !important;
                }
                chat-widget .resize-modal {
                    position: absolute;
                    bottom: -8px;
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
                    font-size: 16px;
                    color: #333;
                }
                chat-widget .chat-title {
                    display: flex;
                    align-items: center;
                    justify-content: space-between;
                    height: 60px;
                    padding: 15px 20px;
                    border-radius: 8px 8px 0 0;
                    font-size: 24px;
                    color: #131313;
                    font-weight: bold;
                    box-shadow: 0px 3px 4px 0px rgba(0, 0, 0, 0.05);
                }
                .chat-logo-box {
                    display: inline-flex;
                    align-items: center;
                    column-gap: 10px;
                    line-height: 28px;
                }
                chat-widget .chat-title-buttons {
                    display: flex;
                    align-items: center;
                    gap: 24px; /* Space between the buttons */
                }
                chat-widget .close-chat-modal,
                chat-widget .full-screen-control {
                    cursor: pointer;
                    background: none;
                    border: none;
                    padding: 5px;
                }
                chat-widget .chat-title-buttons button:hover path {
                    fill: #666;
                }
                chat-widget .chat-content-box {
                    position: relative;
                    display: flex;
                    flex-direction: column;
                    flex-grow: 1;
                    overflow-y: auto;
                }
                chat-widget .chat-content {
                    padding: 20px;
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
                chat-widget .chat-content a:hover,
                chat-widget .chat-content a:focus,
                chat-widget .chat-content a:visited {
                    color: #0068B6;
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
                    background-color: #1461F6;
                }
                .chat-switch-checkbox:checked + .chat-switch-label::after {
                    transform: translateX(20px);
                }

                chat-widget .chat-input-box {
                    padding: 16px 0 0;
                    box-shadow: 0px -3px 4px 0px rgba(0, 0, 0, 0.05);
                }
                chat-widget .chat-input {
                    position: relative;
                    display: flex;
                    align-items: flex-start;
                    width: calc(100% - 40px);
                    height: auto;
                    margin: auto;
                    border: 1px solid #f0f0f0;
                    border-radius: 4px;
                    background: #f4f5f9;
                }
                chat-widget .chat-input textarea {
                    resize: none;
                    width: 100%;
                    min-height: 52px;
                    max-height: 148px;
                    padding: 16px 36px 0px 16px;
                    font-size: 16px;
                    line-height: 18px;
                    color: #333;
                    border: none;
                    outline: none;
                    background: #f4f5f9;
                    box-shadow: none;
                    box-sizing: border-box;
                    overflow-y: auto;
                }
                chat-widget .chat-input textarea::placeholder {
                    color: #999;
                }
                /* 针对 Webkit 浏览器的滚动条样式 */
               chat-widget .chat-input textarea::-webkit-scrollbar {
                    width: 8px; /* 设置竖向滚动条的宽度 */
                    height: 8px; /* 设置横向滚动条的高度 */
                }
                /* 滚动条的轨道 */
                chat-widget .chat-input textarea::-webkit-scrollbar-track {
                    background: #EEE; /* 轨道的背景色 */
                }
                /* 滚动条的滑块 */
                chat-widget .chat-input textarea::-webkit-scrollbar-thumb,
                chat-widget .chat-input textarea::-webkit-scrollbar-thumb:hover,
                chat-widget .chat-input textarea::-webkit-scrollbar-thumb:active {
                    background-color: #BBBABB; /* 滑块的基本颜色 */
                    background: #BBBABB; /* 滑块悬停时的颜色 */
                    border-radius: 8px; /* 滑块的圆角半径 */
                    border: 1px solid #EEE; /* 边框颜色同轨道背景用于视觉效果 */
                }
                /* 在 Firefox 浏览器中定制滚动条 */
                chat-widget .chat-input textarea {
                    scrollbar-width: thin;
                    scrollbar-color: #BBBABB #EEE;
                }
                chat-widget .chat-input button {
                    position: absolute;
                    bottom: 4px;
                    right: 0;
                    padding: 0;
                    padding-right: 8px;
                    border: none;
                    background: none;
                }
                chat-widget .chat-input button.chat-send-btn:not([disabled]):hover svg rect,
                chat-widget .chat-input button.chat-send-btn:not([disabled]):focus svg rect {
                    fill: #0070FF;
                }
                chat-widget .chat-message {
                    display: flex;
                    justify-content: flex-start;
                    margin: 5px 0;
                    // overflow-x: auto;
                }
                chat-widget .userText,
                chat-widget .chatResp {
                    display: block;
                    padding: 12px;
                    border-radius: 4px;
                    line-height: 24px;
                    word-break: break-word;
                    overflow-x: auto;
                }
                chat-widget .userText p, 
                chat-widget .chatResp p {
                    margin: 3px 0;
                    line-height: 24px;
                }
                chat-widget .userText {
                    padding-left: 4px;
                    white-space: pre-wrap;
                    font-weight: 700;
                }
                chat-widget .chatResp {
                    background: #f5f7f8;
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
                .chat-loading-box {
                    margin-top: -4px;
                    display: flex;
                    flex-direction: column;
                    width: 100%;
                    row-gap: 10px;
                    color: #999;
                }
                .chat-loading-text {
                     margin: 3px 0;
                }
                .chat-loading-outer {
                    position: relative;
                    overflow: hidden;
                    width: 100%;
                    height: 8px;
                    background-color: #e2e8f0;
                    border-radius: 4px;
                }
                .chat-loading-inner {
                    position: absolute;
                    left: 0;
                    top: 0;
                    /* 渐变条宽度 */
                    width: 100%;
                    height: 100%;
                    border-radius: 12px;
                    background: linear-gradient(
                        90deg,
                        transparent 0%,
                        rgba(211, 227, 255, 1) 15%,
                        rgba(21, 102, 255, 1) 50%,
                        rgba(211, 227, 255, 1) 85%,
                        transparent 100%
                    );
                    /* 动画起始位置 */
                    transform: translateX(-100%) translateZ(0);
                    /* 动画速度调为1.5秒一轮 */
                    animation: chatLoding 1.0s linear infinite;
                }
                @keyframes chatLoding {
                    0% {
                        transform: translateX(-100%) translateZ(0);
                    }
                    100% {
                        transform: translateX(100%) translateZ(0);
                    }
                }

                chat-widget .chatResp code,
                chat-widget .chatResp table * {
                    font-size: 14px;
                }
                chat-widget .chatResp pre,
                chat-widget .chatResp code {
                    position: relative;
                    white-space: pre;
                    line-height: 24px;
                    border: none;
                    border-radius: 8px;
                    background: transparent;
                }
                chat-widget .chatResp button.copybtn {
                    padding: 1px;
                }
                button.copybtn svg {
                    padding: 0;
                }
                chat-widget .chatResp code.hljs {
                    padding-right: 32px;
                    background: white;
                }
                chat-widget .chatResp table * {
                    background: transparent !important;
                    background-color: transparent !important;
                }
                chat-widget .chatResp table th,
                chat-widget .chatResp table td {
                    padding: 4px 10px;
                    border: 1px solid #e1e4e5;
                }

                chat-widget details, chat-widget details[open] {
                  display: block;
                }
                chat-widget details > summary {
                  /* 禁用 summary 的 pointer cursor */
                  cursor: default;
                  /* 可选：移除默认的小箭头 */
                  list-style: none;
                }
                /* 移除浏览器自带的箭头 */
               chat-widget details > summary::-webkit-details-marker {
                  display: none;
                }
                chat-widget .chat-guide-question li {
                    list-style-type: none;
                    width: fit-content;
                    margin: 10px 0;
                    padding: 4px 10px;
                    background-color: #EFF2F5;
                    border-radius: 8px;
                    border: 1px solid #D7DAE5;
                    cursor: pointer;
                }
                chat-widget .chat-guide-question li:hover {
                    background-color: #EAEDF0;
                }
                chat-widget .chat-guide-question li::after {
                    content: "→";
                    display: inline-block;
                }

                .chat-feedback-box {
                    display: flex;
                    justify-content: space-between;
                    column-gap: 16px;
                    row-gap: 8px;
                    margin: 10px 0 8px;
                }
                .chat-feedback-box .chat-feedback-item {
                    display: flex;
                    column-gap: 16px;
                }
                .chat-feedback-box .chat-feedback-item .chat-feedback-btn {
                    display: flex;
                    align-items: center;
                    column-gap: 4px;
                    white-space: nowrap;
                    padding: 7px 10px;
                    border-radius: 4px;
                    border: none;
                    transition: 0.1s;
                }
                .chat-feedback-btn.chat-copy-btn {
                    color: rgba(237, 77, 76, 1);
                    background-color: rgba(237, 77, 76, 0.09);
                }
                .chat-feedback-btn.chat-clear-btn {
                    color: rgba(86, 186, 51, 1);
                    background-color: rgba(86, 186, 51, 0.09);
                }
                .chat-feedback-btn.chat-like-btn {
                    color: rgba(255, 130, 20, 1);
                    background-color: rgba(255, 130, 20, 0.09);
                }
                .chat-feedback-btn.chat-dislike-btn {
                    color: rgba(20, 101, 255, 1);
                    background-color: rgba(20, 101, 255, 0.09);
                }
                .chat-copy-btn:hover { background-color: rgba(237, 77, 76, 1); }
                .chat-clear-btn:hover { background-color: rgba(86, 186, 51, 1); }
                .chat-like-btn:hover { background-color: rgba(255, 130, 20, 1); }
                .chat-dislike-btn:hover { background-color: rgba(20, 101, 255, 1); }
                .chat-feedback-btn:hover { color: #FFF; }
                .chat-feedback-icon {
                    display: inline-flex;
                    align-items: center;
                }
                .chat-feedback-btn.chat-copy-btn:hover svg path,
                .chat-feedback-btn.chat-clear-btn:hover svg path,
                .chat-feedback-btn.chat-like-btn:hover svg path:last-child,
                .chat-feedback-btn.chat-dislike-btn:hover svg path:last-child {
                    fill: #FFF;
                }
                .chat-like-btn.btn-scored svg path {
                    fill: rgba(255, 130, 20, 1);
                }
                .chat-dislike-btn.btn-scored svg path {
                    fill: rgba(20, 101, 255, 1);
                }
                .chat-like-btn.btn-scored svg path:first-child,
                .chat-dislike-btn.btn-scored svg path:first-child {
                    fill-opacity: 0.85;
                }

                .chatRefDocs ul li {
                    list-style: none;
                    list-style-position: inside;
                    margin-left: 0;
                }
                .chatRefDocs ul li a, .chatRefDocs ul li a:visited {
                    display: block;
                    margin: 8px 0;
                    padding: 6px 12px;
                    color: #333;
                    background-color: #FFF;
                    text-decoration: underline;
                }
                .chatRefDocs ul li a:hover, .chatRefDocs ul li a:focus {
                    color: #0068B6;
                }
                chat-widget .chat-based-refs {
                    padding: 10px 0 5px;
                    font-weight: 700;
                    font-size: 16px;
                    color: #333;
                }
                chat-widget .chat-note {
                    width: calc(100% - 40px);
                    margin: 10px auto 0;
                    font-size: 14px;
                    color: #999;
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
                    color: #1464FF;
                    font-size: 16px;
                    margin-right: 5px;
                }
                .font-small {
                    font-size: 12px !important;
                }
                chat-widget .toggle-chat-modal {
                    width: 45px;
                    height: 50px;
                    padding: 0;
                    position: fixed;
                    bottom: 25px;
                    right: 20px;
                    background: linear-gradient(to bottom, #4591FF, #1461F6);
                    border: none;
                    border-radius: 4px;
                    box-shadow: 0px 1px 4px 0px rgba(0, 0, 0, 0.5);
                    cursor: pointer;
                    z-index: 1001;
                }
                chat-widget .toggle-chat-modal svg {
                    width: 45px;
                    height: 50px;
                }
                chat-widget .toggle-chat-modal:hover {
                    background: #4591FF;
                }
            </style>
            <div class="chat-modal-mask" id="ChatModalMask">
            <div class="chat-modal" id="ChatModal">
                <div class="chat-box">
                    <div class="chat-title">
                        <div class="chat-logo-box">${logo} ${title}</div>
                        <div class="chat-title-buttons">
                            <button class="full-screen-control" id="FullScreenControl">
                                <svg viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="16.000000" height="16.000000" fill="none" customFrame="#000000">
	                                <path id="矢量 17" d="M9.65823 9.65815C9.72602 9.59026 9.80422 9.53794 9.89284 9.50118C9.98146 9.46443 10.0737 9.44604 10.1697 9.44603C10.3615 9.44603 10.5456 9.5224 10.6811 9.65815L14.5568 13.5324L14.5553 11.1921L14.567 11.0621C14.6335 10.694 14.9703 10.4376 15.343 10.4714C15.7156 10.5052 16.0006 10.8178 16 11.1921L16 15.2918C15.999 15.3223 15.9961 15.3526 15.9914 15.3829L15.9871 15.406C15.9826 15.4355 15.9758 15.4644 15.9668 15.4928L15.9552 15.526L15.9437 15.5549C15.9075 15.643 15.8555 15.7206 15.7877 15.7875L15.8311 15.7383L15.8282 15.7412L15.8094 15.7629L15.7877 15.7875L15.776 15.7991L15.7645 15.8108C15.6686 15.8968 15.5579 15.9541 15.4322 15.9826L15.36 15.9928L15.2777 16L11.191 16L11.0609 15.9871C10.7184 15.9244 10.4692 15.6261 10.4686 15.2779L10.4801 15.1478C10.5429 14.8047 10.8419 14.5555 11.1907 14.5557L13.5308 14.5528L9.65966 10.6811L9.57592 10.5799C9.37711 10.2931 9.41192 9.90528 9.65823 9.65815ZM6.34013 9.65815C6.58706 9.90487 6.62227 10.2927 6.42387 10.5797L6.34013 10.6809L2.46738 14.5522L4.80762 14.5551C5.1565 14.5549 5.45564 14.8043 5.51829 15.1472L5.52996 15.2772C5.52934 15.6255 5.28037 15.9238 4.93784 15.9865L4.80782 15.9994L0.722546 15.9994L0.635939 15.9908L0.56653 15.9808C0.429678 15.9493 0.311199 15.8843 0.211093 15.7858L0.18939 15.7627L0.169325 15.7396L0.18939 15.7613L0.149056 15.715L0.166459 15.7367L0.15786 15.7252C0.128611 15.6892 0.103086 15.6507 0.081285 15.6097L0.0581487 15.5549C0.0382183 15.5064 0.0238179 15.4564 0.0149475 15.4048L0.00921471 15.3817C0.00491493 15.3516 0.00245809 15.3211 0.00184388 15.2905L0.00184388 11.1909C0.00122967 10.8166 0.286235 10.504 0.65887 10.4702C1.03151 10.4364 1.36831 10.6927 1.43485 11.0609L1.44652 11.1909L1.44509 13.5324L5.32071 9.65815C5.60264 9.37621 6.06004 9.37621 6.34198 9.65815L6.34013 9.65815L6.34013 9.65815ZM4.81048 0L4.9405 0.0116706C5.28365 0.0743233 5.53282 0.37346 5.53262 0.722349L5.51972 0.852364C5.45686 1.19552 5.15794 1.4447 4.80905 1.44449L2.46882 1.44736L6.34157 5.31873L6.42531 5.41987C6.63538 5.7274 6.57723 6.14406 6.2912 6.38239C6.00497 6.62051 5.58463 6.60229 5.3203 6.34001L1.44325 2.46721L1.44468 4.80747L1.43301 4.93749C1.36647 5.30562 1.02966 5.56197 0.657027 5.52818C0.284392 5.4944 -0.000613273 5.18175 9.37248e-07 4.80747L9.37248e-07 0.707812C9.37248e-07 0.676077 0.00286735 0.645774 0.00716713 0.615471L0.0114669 0.592335C0.0163809 0.562851 0.0235468 0.533982 0.0331698 0.505727C0.0680996 0.392852 0.127339 0.294642 0.210889 0.211095L0.167483 0.260234L0.158884 0.271905L0.190619 0.234232L0.170349 0.255935L0.189186 0.235869L0.235458 0.188163C0.2977 0.132267 0.368952 0.0874275 0.446345 0.0552821L0.504084 0.0335789C0.57472 0.0116709 0.648224 0.000409739 0.722137 0.000409739L0.648428 0.00327619L0.658461 0.00327619L0.688763 0.000409739L4.81048 0.000409739L4.81048 0ZM15.3121 0L15.3409 0.00286645L15.3495 0.00286645C15.3993 0.00880415 15.4486 0.0190415 15.4967 0.0331692L15.5545 0.0548724L15.6079 0.079442L15.6702 0.115478C15.7129 0.142709 15.7527 0.17465 15.7885 0.21089L15.8116 0.23546L15.8276 0.255525L15.8319 0.258391L15.8419 0.271495L15.8896 0.3364C15.9378 0.414819 15.9711 0.501632 15.9879 0.59213L15.9922 0.615267C15.9965 0.645569 15.9994 0.675872 15.9994 0.706175L15.9994 4.8087C15.9961 5.17991 15.7121 5.48826 15.3426 5.52184C14.9728 5.55542 14.6378 5.30337 14.568 4.93872L14.5563 4.8087L14.5563 2.46598L10.6824 6.34021C10.4182 6.60536 9.99524 6.62502 9.70798 6.38526C9.42052 6.1455 9.36381 5.72597 9.57735 5.41844L9.65966 5.31893L13.5338 1.44757L11.1938 1.4447C10.8445 1.44552 10.5446 1.19614 10.4817 0.852569L10.4688 0.722555C10.4686 0.373665 10.7178 0.0745286 11.0611 0.0118759L11.1912 0.00020526L15.3121 0.00020526L15.3121 0Z" fill="rgb(0,0,0)" fill-rule="nonzero" />
                                </svg>
                            </button>
                            <button class="close-chat-modal" id="CloseChatModal" aria-label="Close chat">
                                <svg viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="16.000000" height="16.000000" fill="none" customFrame="#000000">
	                                <path id="矢量 16" d="M9.5 8L16 14.5L14.5 16L8 9.5L1.5 16L0 14.5L6.5 8L0 1.5L1.5 0L8 6.5L14.5 0L16 1.5L9.5 8Z" fill="#131313" fill-rule="nonzero" />
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
                                <svg viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="30.000000" height="30.000000" fill="none" customFrame="#000000">
                                    <rect id="矩形 4" width="30.000000" height="30.000000" x="0.000000" y="0.000000" rx="4.000000" fill="rgb(20,100,255)" />
                                    <path id="减去顶层" d="M11.3087 23.0091C10.6444 23.4565 9.75 22.9805 9.75 22.1797L9.75 15.7157L14.25 15.7157L14.25 14.2157L9.75 14.2157L9.75 7.75169C9.75 6.95084 10.6444 6.47489 11.3087 6.9223L22.0187 14.1363C22.6071 14.5327 22.6071 15.3987 22.0187 15.7951L11.3087 23.0091Z" fill="#fff" fill-rule="evenodd" />
                                </svg>
                            </button>
                            <button class="chat-abort-btn btn-hidden" id="ChatAbortBtn">
                                <svg viewBox="0 0 30 30" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="30.000000" height="30.000000" fill="none" customFrame="#000000">
                                    <g>
                                        <rect id="矩形 4" width="30.000000" height="30.000000" x="0.000000" y="0.000000" rx="4.000000" fill="rgb(20,100,255)" />
                                        <path id="减去顶层" d="M11.3087 23.0091C10.6444 23.4565 9.75 22.9805 9.75 22.1797L9.75 15.7157L14.25 15.7157L14.25 14.2157L9.75 14.2157L9.75 7.75169C9.75 6.95084 10.6444 6.47489 11.3087 6.9223L22.0187 14.1363C22.6071 14.5327 22.6071 15.3987 22.0187 15.7951L11.3087 23.0091Z" fill="#fff" fill-rule="evenodd" />
                                    </g>
                                    <rect id="矩形 34" width="18.000000" height="18.000000" x="6.000000" y="6.000000" rx="3.000000" fill="rgb(255,255,255)" />
                                </svg>
                            </button>
                        </div>
                    </div>
                    <div class="chat-note">
                        <span>Responses are provided by Realtek's AI chatbot and may contain inaccuracies. Realtek is not liable for any damages from its use and offers no warranties.</span>
                    </div>
                </div>
                <div class="resize-modal">
                    <svg t="1761545122573" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="6627" width="18" height="18">
                        <path d="M959.333 63.333v896h-896z" p-id="6628" fill="#1464FF"></path>
                    </svg>
                </div>
            </div>
            </div>
            <button class="toggle-chat-modal" id="ToggleChatModal">${logo}</button>
        </chat-widget>
        `;

        document.body.insertAdjacentHTML('beforeend', chatWidgetHTML);
    }

    /* ----------------------- svg icons ----------------------- */
    const logoSvg = `
    <svg viewBox="0 0 46 46" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="46.000000" height="46.000000" fill="none" customFrame="#000000">
	    <defs>
		    <g id="pixso_custom_effect_0">
			    <effect x="0.000000" y="0.000000" visibility="visible" fill="rgb(100,100,100)" fill-opacity="0.25" effectType="dropShadow" stdDeviation="3" radius="0" />
		    </g>
		    <filter id="filter_0" width="46.000000" height="46.000000" x="0.000000" y="0.000000" filterUnits="userSpaceOnUse" customEffect="url(#pixso_custom_effect_0)" color-interpolation-filters="sRGB">
			    <feFlood flood-opacity="0" result="BackgroundImageFix" />
			    <feOffset dx="0.000000" dy="0.000000" in="SourceAlpha" />
			    <feGaussianBlur stdDeviation="1" />
			    <feColorMatrix type="matrix" values="0 0 0 0 0.392157 0 0 0 0 0.392157 0 0 0 0 0.392157 0 0 0 0.25 0 " />
			    <feBlend result="effect_dropShadow_1" in2="BackgroundImageFix" mode="normal" />
			    <feBlend result="shape" in="SourceGraphic" in2="effect_dropShadow_1" mode="normal" />
		    </filter>
		    <linearGradient id="paint_linear_0" x1="23" x2="23" y1="3" y2="43" gradientUnits="userSpaceOnUse">
			    <stop stop-color="rgb(69,145,255)" offset="0" stop-opacity="1" />
			    <stop stop-color="rgb(20,97,246)" offset="0.995780587" stop-opacity="1" />
		    </linearGradient>
	    </defs>
	    <g id="组合 41">
		    <g filter="url(#filter_0)">
			    <rect id="矩形 25" width="40.000000" height="40.000000" x="3.000000" y="3.000000" rx="4.000000" fill="url(#paint_linear_0)" />
		    </g>
		    <path id="" d="M13.041 16L13.4187 14.8396L15.2559 14.8396L15.6335 16L16.7871 16L15.032 10.9089L13.646 10.9089L11.8875 16L13.041 16ZM13.6921 13.9988L14.3193 12.0728L14.3586 12.0728L14.9858 13.9988L13.6921 13.9988ZM18.4761 10.9089L17.3994 10.9089L17.3994 16L18.4761 16L18.4761 10.9089ZM21.8631 16L22.2408 14.8396L24.0779 14.8396L24.4556 16L25.6092 16L23.8541 10.9089L22.4681 10.9089L20.7095 16L21.8631 16ZM22.5142 13.9988L23.1414 12.0728L23.1807 12.0728L23.8079 13.9988L22.5142 13.9988ZM28.8639 12.3735L29.8961 12.3735C29.8807 11.4661 29.1339 10.8389 28.0008 10.8389C26.8849 10.8389 26.068 11.4558 26.0714 12.3804C26.0697 13.1306 26.5995 13.5613 27.4591 13.7681L28.0128 13.9065C28.5682 14.0415 28.8758 14.2004 28.8793 14.5439C28.8758 14.9165 28.5238 15.1694 27.9769 15.1694C27.4164 15.1694 27.0148 14.9114 26.9789 14.4021L25.9381 14.4021C25.9654 15.5027 26.7533 16.0718 27.9889 16.0718C29.2313 16.0718 29.9628 15.4788 29.9645 14.5457C29.9628 13.698 29.3236 13.2485 28.4383 13.0486L27.982 12.9392C27.5394 12.8384 27.1686 12.6743 27.1754 12.3086C27.1754 11.9805 27.4659 11.7395 27.994 11.7395C28.5084 11.7395 28.8246 11.9736 28.8639 12.3735ZM30.686 16L31.7626 16L31.7626 14.5525L32.2924 13.9065L33.6869 16L34.9738 16L33.0615 13.1887L34.9516 10.9089L33.6613 10.9089L31.8293 13.1545L31.7626 13.1545L31.7626 10.9089L30.686 10.9089L30.686 16Z" fill="rgb(255,255,255)" fill-rule="nonzero" />
		    <g id="组合 40">
			    <g id="组合 36">
				    <ellipse id="椭圆 7" rx="1.411350" ry="1.458711" cx="20.5248756" cy="27.2767048" fill="rgb(255,255,255)" />
			    </g>
			    <g id="组合 37">
				    <ellipse id="椭圆 7" rx="1.411350" ry="1.458711" cx="1.41135025" cy="1.45871091" fill="rgb(255,255,255)" transform="matrix(-1,0,0,1,26.6407,25.818)" />
			    </g>
			    <path id="矢量 53" d="M37.9011 36.1679C34.2939 31.9907 29.0448 29.3606 23.2013 29.3606C17.3579 29.3606 12.1088 31.9907 8.50171 36.1679L37.9011 36.1679Z" fill="rgb(255,255,255)" fill-rule="evenodd" />
			    <g id="组合 39">
				    <path id="矢量 54" d="M0.263256 6.23708C1.07998 2.8368 6.08315 0 6.08315 0C2.81572 2.33162 4.85785 8.25781 4.85785 8.25781C4.85785 8.25781 3.93913 8.18009 3.93909 9.1516C3.93905 10.2008 5.57282 10.6089 5.57282 10.6089C5.57282 10.6089 6.6528 11.0402 8.53262 10.2008C10.8824 8.66585 11.2908 7.28631 11.2908 7.28631C11.2908 7.28631 11.2908 11.0946 8.02342 13.0376C5.84313 14.1742 3.32584 14.1646 1.99887 13.1154C0.671898 12.0661 -0.553466 9.63735 0.263256 6.23708Z" fill="rgb(255,255,255)" fill-rule="evenodd" transform="matrix(-0.986438,-0.164134,-0.164134,0.986438,41,12.6168)" />
				    <path id="矢量 51" d="M4.23099 8.93665L6.96237 6.72711C4.01376 5.89528 3.53986 4.95948 3.2768 4.51757L3.77027 2.69795C2.66454 3.11386 0.460832 1.03978 0 0L0.545226 5.8173C3.12526 5.8173 4.23099 8.93665 4.23099 8.93665Z" fill="rgb(255,255,255)" fill-rule="evenodd" transform="matrix(0.995235,-0.0975013,0.0913265,0.995821,5.94092,23.6742)" />
				    <path id="矢量 52" d="M4.22988 8.93884L6.96055 6.72876C4.01271 5.89673 3.53894 4.9607 3.27594 4.51868L3.76928 2.69861C2.66384 3.11463 0.460711 1.04004 0 0L0.545083 5.81873C3.12444 5.81873 4.22988 8.93884 4.22988 8.93884Z" fill="rgb(255,255,255)" fill-rule="evenodd" transform="matrix(-0.999346,0.036167,0.0338594,0.999427,39.2234,23.6337)" />
				    <path id="矢量 55" d="M14.8093 23.1478C11.9383 26.2028 8.58866 24.4571 7.6317 23.5843C6.2587 22.3319 5.68623 20.8406 5.31235 19.5293C3.32645 12.5639 11.4597 7 11.4597 7C6.86606 10.8406 10.5028 17.9107 10.5028 17.9107C8.90504 18.6393 9.30654 19.22 9.30654 19.22C9.30654 19.22 9.83257 21.4021 13.8521 21.4021C15.43 21.4151 17.2018 20.5293 17.2018 20.5293C17.2018 20.5293 17.6803 20.0928 14.8093 23.1478Z" fill="rgb(255,255,255)" fill-rule="evenodd" />
			    </g>
		    </g>
		    <path id="矢量 56" d="M1.54488 0.10068C1.5447 0.0865947 1.54461 0.0724106 1.54461 0.0581267C1.54485 0.0705327 1.54498 0.0847462 1.54488 0.10068ZM1.53195 3.81566C1.55574 3.35233 1.7938 2.1692 3 2.07407C3 2.07407 1.56768 1.85628 1.54488 0.10068C1.54271 0.458926 1.4274 1.67182 0 2.07407C0 2.07407 1.38959 2.07405 1.53195 3.81566ZM1.53195 3.81566C1.5368 3.87505 1.54021 3.93649 1.54206 4C1.5332 4 1.5261 3.92959 1.53195 3.81566Z" fill="rgb(255,255,255)" fill-rule="evenodd" transform="matrix(-1,0,0,1,22,7)" />
		    <path id="矢量 57" d="M1.02992 0.0755104C1.0298 0.0649464 1.02974 0.0543081 1.02974 0.043595C1.0299 0.0528957 1.02999 0.0635661 1.02992 0.0755104ZM1.0213 2.86175C1.03716 2.51425 1.19587 1.6269 2 1.55556C2 1.55556 1.04512 1.39221 1.02992 0.0755104C1.02847 0.344176 0.951618 1.25386 0 1.55556C0 1.55556 0.926391 1.55554 1.0213 2.86175ZM1.0213 2.86175C1.02453 2.90629 1.0268 2.95237 1.02804 3C1.02213 3 1.0174 2.9472 1.0213 2.86175Z" fill="rgb(255,255,255)" fill-rule="evenodd" transform="matrix(-1,0,0,1,24,6)" />
	    </g>
    </svg>
    `;
    const copySvg = `
    <svg viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="16.000000" height="16.000000" fill="none" customFrame="#000000">
        <title>Copy to clipboard</title>
        <path id="矢量 12" d="M5.68421 0L14.5263 0C14.5746 0 14.6227 0.0023654 14.6708 0.00709619C14.7188 0.011827 14.7665 0.0189004 14.8138 0.0283164C14.8612 0.0377325 14.9079 0.0494458 14.9541 0.0634563C15.0003 0.0774669 15.0457 0.0937073 15.0903 0.112178C15.1349 0.130648 15.1784 0.151259 15.221 0.174011C15.2636 0.196763 15.3049 0.221546 15.345 0.248361C15.3852 0.275175 15.4239 0.303892 15.4612 0.334511C15.4985 0.36513 15.5342 0.397504 15.5684 0.431632C15.6025 0.465761 15.6349 0.50148 15.6655 0.538789C15.6961 0.576098 15.7248 0.614818 15.7516 0.654949C15.7785 0.69508 15.8032 0.736428 15.826 0.778994C15.8487 0.82156 15.8693 0.865139 15.8878 0.90973C15.9063 0.954321 15.9225 0.99971 15.9365 1.0459C15.9506 1.09208 15.9623 1.13885 15.9717 1.18618C15.9811 1.23352 15.9882 1.28121 15.9929 1.32924C15.9976 1.37727 16 1.42542 16 1.47368L16 10.3158C16 10.3641 15.9976 10.4122 15.9929 10.4602C15.9882 10.5083 15.9811 10.556 15.9717 10.6033C15.9623 10.6506 15.9506 10.6974 15.9365 10.7436C15.9225 10.7898 15.9063 10.8352 15.8878 10.8797C15.8694 10.9243 15.8487 10.9679 15.826 11.0105C15.8032 11.053 15.7785 11.0944 15.7516 11.1345C15.7248 11.1747 15.6961 11.2134 15.6655 11.2507C15.6349 11.288 15.6025 11.3237 15.5684 11.3578C15.5342 11.392 15.4985 11.4243 15.4612 11.455C15.4239 11.4856 15.3852 11.5143 15.345 11.5411C15.3049 11.5679 15.2636 11.5927 15.221 11.6155C15.1784 11.6382 15.1349 11.6588 15.0903 11.6773C15.0457 11.6958 15.0003 11.712 14.9541 11.726C14.9079 11.74 14.8612 11.7517 14.8138 11.7612C14.7665 11.7706 14.7188 11.7776 14.6708 11.7824C14.6227 11.7871 14.5746 11.7895 14.5263 11.7895L5.68421 11.7895C5.63595 11.7895 5.5878 11.7871 5.53976 11.7824C5.49173 11.7776 5.44405 11.7706 5.39671 11.7612C5.34937 11.7517 5.30261 11.74 5.25642 11.726C5.21024 11.712 5.16485 11.6958 5.12026 11.6773C5.07566 11.6588 5.03209 11.6382 4.98952 11.6155C4.94695 11.5927 4.90561 11.5679 4.86547 11.5411C4.82534 11.5143 4.78662 11.4856 4.74931 11.455C4.71201 11.4243 4.67629 11.392 4.64216 11.3578C4.60803 11.3237 4.57566 11.288 4.54504 11.2507C4.51442 11.2134 4.4857 11.1747 4.45889 11.1345C4.43207 11.0944 4.40729 11.053 4.38454 11.0105C4.36178 10.9679 4.34117 10.9243 4.3227 10.8797C4.30423 10.8352 4.28799 10.7898 4.27398 10.7436C4.25997 10.6974 4.24826 10.6506 4.23884 10.6033C4.22943 10.556 4.22235 10.5083 4.21762 10.4602C4.21289 10.4122 4.21053 10.3641 4.21053 10.3158L4.21053 1.47368C4.21053 1.42542 4.21289 1.37727 4.21762 1.32924C4.22235 1.28121 4.22943 1.23352 4.23884 1.18618C4.24826 1.13885 4.25997 1.09208 4.27398 1.0459C4.28799 0.99971 4.30423 0.954321 4.3227 0.90973C4.34117 0.865139 4.36179 0.82156 4.38454 0.778994C4.40729 0.736428 4.43207 0.69508 4.45889 0.654949C4.4857 0.614818 4.51442 0.576098 4.54504 0.538789C4.57566 0.50148 4.60803 0.465761 4.64216 0.431632C4.67629 0.397504 4.71201 0.36513 4.74932 0.334511C4.78662 0.303892 4.82535 0.275175 4.86548 0.248361C4.90561 0.221546 4.94696 0.196763 4.98952 0.174011C5.03209 0.151259 5.07567 0.130648 5.12026 0.112178C5.16485 0.0937073 5.21024 0.0774669 5.25642 0.0634564C5.30261 0.0494458 5.34937 0.0377325 5.39671 0.0283164C5.44405 0.0189004 5.49173 0.011827 5.53977 0.00709619C5.5878 0.0023654 5.63595 0 5.68421 0ZM5.68421 1.26316C5.65629 1.26316 5.62944 1.2685 5.60365 1.27918C5.57785 1.28987 5.55509 1.30508 5.53535 1.32482C5.51561 1.34456 5.50039 1.36733 5.48971 1.39312C5.47903 1.41891 5.47368 1.44577 5.47368 1.47368L5.47368 10.3158C5.47368 10.3437 5.47903 10.3706 5.48971 10.3964C5.50039 10.4221 5.5156 10.4449 5.53535 10.4647C5.55509 10.4844 5.57785 10.4996 5.60365 10.5103C5.62944 10.521 5.65629 10.5263 5.68421 10.5263L14.5263 10.5263C14.5542 10.5263 14.5811 10.521 14.6069 10.5103C14.6327 10.4996 14.6554 10.4844 14.6752 10.4647C14.6949 10.4449 14.7101 10.4221 14.7208 10.3964C14.7315 10.3706 14.7368 10.3437 14.7368 10.3158L14.7368 1.47368C14.7368 1.44577 14.7315 1.41891 14.7208 1.39312C14.7101 1.36733 14.6949 1.34456 14.6752 1.32482C14.6554 1.30508 14.6327 1.28987 14.6069 1.27918C14.5811 1.2685 14.5542 1.26316 14.5263 1.26316L5.68421 1.26316ZM10.5263 13.0526C10.5263 13.0112 10.5304 12.9701 10.5385 12.9294C10.5465 12.8887 10.5585 12.8492 10.5744 12.8109C10.5903 12.7726 10.6097 12.7362 10.6328 12.7017C10.6558 12.6673 10.682 12.6354 10.7113 12.606C10.7406 12.5767 10.7725 12.5505 10.807 12.5275C10.8415 12.5045 10.8779 12.485 10.9162 12.4691C10.9545 12.4533 10.994 12.4413 11.0347 12.4332C11.0754 12.4251 11.1164 12.4211 11.1579 12.4211C11.1994 12.4211 11.2404 12.4251 11.2811 12.4332C11.3218 12.4413 11.3613 12.4533 11.3996 12.4691C11.4379 12.485 11.4743 12.5045 11.5088 12.5275C11.5433 12.5505 11.5752 12.5767 11.6045 12.606C11.6338 12.6354 11.66 12.6673 11.683 12.7017C11.7061 12.7362 11.7255 12.7726 11.7414 12.8109C11.7573 12.8492 11.7692 12.8887 11.7773 12.9294C11.7854 12.9701 11.7895 13.0112 11.7895 13.0526L11.7895 14.5263C11.7895 14.5746 11.7871 14.6227 11.7824 14.6708C11.7776 14.7188 11.7706 14.7665 11.7612 14.8138C11.7517 14.8612 11.74 14.9079 11.726 14.9541C11.712 15.0003 11.6958 15.0457 11.6773 15.0903C11.6588 15.1349 11.6382 15.1784 11.6155 15.221C11.5927 15.2636 11.5679 15.3049 11.5411 15.345C11.5143 15.3852 11.4856 15.4239 11.455 15.4612C11.4243 15.4985 11.392 15.5342 11.3578 15.5684C11.3237 15.6025 11.288 15.6349 11.2507 15.6655C11.2134 15.6961 11.1747 15.7248 11.1345 15.7516C11.0944 15.7785 11.053 15.8032 11.0105 15.826C10.9679 15.8487 10.9243 15.8694 10.8797 15.8878C10.8352 15.9063 10.7898 15.9225 10.7436 15.9365C10.6974 15.9506 10.6506 15.9623 10.6033 15.9717C10.556 15.9811 10.5083 15.9882 10.4602 15.9929C10.4122 15.9976 10.3641 16 10.3158 16L1.47368 16C1.42542 16 1.37727 15.9976 1.32924 15.9929C1.28121 15.9882 1.23352 15.9811 1.18618 15.9717C1.13885 15.9623 1.09208 15.9506 1.0459 15.9365C0.99971 15.9225 0.954321 15.9063 0.90973 15.8878C0.865139 15.8694 0.82156 15.8487 0.778994 15.826C0.736428 15.8032 0.69508 15.7785 0.654949 15.7516C0.614818 15.7248 0.576098 15.6961 0.538789 15.6655C0.50148 15.6349 0.465761 15.6025 0.431632 15.5684C0.397504 15.5342 0.36513 15.4985 0.334511 15.4612C0.303892 15.4239 0.275175 15.3852 0.248361 15.3451C0.221546 15.3049 0.196763 15.2636 0.174011 15.221C0.151259 15.1784 0.130648 15.1349 0.112178 15.0903C0.0937073 15.0457 0.0774669 15.0003 0.0634563 14.9541C0.0494458 14.9079 0.0377325 14.8612 0.0283164 14.8138C0.0189004 14.7665 0.011827 14.7188 0.00709619 14.6708C0.0023654 14.6227 0 14.5746 0 14.5263L0 5.68421C-4.93169e-09 5.63595 0.00236539 5.5878 0.00709618 5.53976C0.011827 5.49173 0.0189004 5.44405 0.0283164 5.39671C0.0377324 5.34937 0.0494457 5.30261 0.0634563 5.25642C0.0774669 5.21024 0.0937073 5.16485 0.112177 5.12026C0.130648 5.07567 0.151259 5.03209 0.174011 4.98952C0.196763 4.94695 0.221546 4.90561 0.248361 4.86548C0.275175 4.82534 0.303892 4.78662 0.334511 4.74931C0.36513 4.71201 0.397504 4.67629 0.431632 4.64216C0.465761 4.60803 0.50148 4.57566 0.538789 4.54504C0.576098 4.51442 0.614818 4.4857 0.654949 4.45889C0.69508 4.43207 0.736428 4.40729 0.778994 4.38454C0.82156 4.36179 0.865139 4.34117 0.90973 4.3227C0.954321 4.30423 0.99971 4.28799 1.0459 4.27398C1.09208 4.25997 1.13885 4.24826 1.18618 4.23884C1.23352 4.22943 1.28121 4.22235 1.32924 4.21762C1.37727 4.21289 1.42542 4.21053 1.47368 4.21053L2.94737 4.21053C2.98884 4.21053 3.02991 4.21457 3.07058 4.22266C3.11126 4.23075 3.15075 4.24273 3.18906 4.2586C3.22738 4.27447 3.26377 4.29393 3.29825 4.31697C3.33274 4.34001 3.36464 4.36619 3.39396 4.39551C3.42329 4.42484 3.44947 4.45674 3.47251 4.49122C3.49555 4.5257 3.515 4.5621 3.53087 4.60041C3.54674 4.63872 3.55872 4.67822 3.56681 4.71889C3.5749 4.75956 3.57895 4.80064 3.57895 4.84211C3.57895 4.88358 3.5749 4.92465 3.56681 4.96532C3.55872 5.00599 3.54674 5.04549 3.53087 5.0838C3.515 5.12211 3.49555 5.15851 3.47251 5.19299C3.44947 5.22747 3.42329 5.25938 3.39396 5.2887C3.36464 5.31802 3.33274 5.3442 3.29825 5.36724C3.26377 5.39028 3.22738 5.40974 3.18906 5.42561C3.15075 5.44148 3.11126 5.45346 3.07058 5.46155C3.02991 5.46964 2.98884 5.47368 2.94737 5.47368L1.47368 5.47368C1.44577 5.47368 1.41891 5.47903 1.39312 5.48971C1.36733 5.50039 1.34456 5.5156 1.32482 5.53535C1.30508 5.55509 1.28987 5.57785 1.27918 5.60365C1.2685 5.62944 1.26316 5.65629 1.26316 5.68421L1.26316 14.5263C1.26316 14.5542 1.2685 14.5811 1.27918 14.6069C1.28987 14.6327 1.30508 14.6554 1.32482 14.6752C1.34456 14.6949 1.36733 14.7101 1.39312 14.7208C1.41891 14.7315 1.44577 14.7368 1.47368 14.7368L10.3158 14.7368C10.3437 14.7368 10.3706 14.7315 10.3964 14.7208C10.4222 14.7101 10.4449 14.6949 10.4647 14.6752C10.4844 14.6554 10.4996 14.6327 10.5103 14.6069C10.521 14.5811 10.5263 14.5542 10.5263 14.5263L10.5263 13.0526L10.5263 13.0526Z" fill="rgb(237,77,76)" fill-rule="nonzero" />
    </svg>
    `;
    const codeCopySvg = `
    <svg xmlns="http://www.w3.org/2000/svg" class="icon icon-tabler icon-tabler-copy" width="44" height="44" viewBox="0 0 24 24" stroke-width="1.5" stroke="#000000" fill="none" stroke-linecap="round" stroke-linejoin="round">
        <title>Copy to clipboard</title>
        <path stroke="none" d="M0 0h24v24H0z" fill="none"></path>
        <rect x="8" y="8" width="12" height="12" rx="2"></rect>
        <path d="M16 8v-2a2 2 0 0 0 -2 -2h-8a2 2 0 0 0 -2 2v8a2 2 0 0 0 2 2h2"></path>
    </svg>
    `;
    const codeCpDoneSvg = `
    <svg t="1760929694236" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="2383" width="22" height="22">
        <path d="M864.554667 268.501333a42.666667 42.666667 0 0 1 0 60.330667L412.032 781.397333a42.453333 42.453333 0 0 1-22.613333 11.818667l-5.034667 0.597333H379.306667a42.496 42.496 0 0 1-27.648-12.416l-211.2-211.2a42.666667 42.666667 0 1 1 60.330666-60.330666l180.992 180.992 422.4-422.4a42.666667 42.666667 0 0 1 60.330667 0z" fill="#22863a" p-id="2384"></path>
    </svg>
    `;
    const copyFailSvg = `
    <svg t="1755756470581" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="21471" width="17" height="17">
        <path d="M640.79 54l0.25 0.004a32.569 32.569 0 0 1 0.643 0.015l0.216 0.009c3.454 0.141 6.767 0.83 9.854 1.983 0.02 0.009 0.04 0.016 0.061 0.024 0.131 0.048 0.261 0.098 0.39 0.149l0.092 0.036a30.076 30.076 0 0 1 1.103 0.458l0.106 0.047a31.824 31.824 0 0 1 8.214 5.266l0.123 0.11c0.325 0.288 0.645 0.585 0.96 0.89l0.378 0.372 213.437 213.25 0.063 0.062 0.262 0.266-0.325-0.328a32.645 32.645 0 0 1 0.69 0.71l0.075 0.08a31.898 31.898 0 0 1 4.865 6.793l0.049 0.093A31.865 31.865 0 0 1 886 298.72V896.35c0 41.235-33.444 74.65-74.688 74.65H213.688C172.446 971 139 937.585 139 896.35v-767.7C139 87.416 172.445 54 213.688 54h427.101z m-32.228 64H213.687C207.78 118 203 122.774 203 128.65v767.7c0 5.876 4.778 10.65 10.688 10.65h597.625c5.909 0 10.687-4.774 10.687-10.65v-565.1H640.562c-17.496 0-31.713-14.042-31.995-31.47l-0.005-0.53V118zM663.2 447.233c12.366 12.377 12.481 32.359 0.351 44.877l-0.371 0.377L557.774 597.8 663.18 703.113c12.502 12.49 12.511 32.752 0.02 45.254-12.367 12.378-32.348 12.51-44.878 0.391l-0.377-0.37L512.5 643.033 407.055 748.387c-12.502 12.492-32.764 12.483-45.255-0.02-12.366-12.377-12.481-32.359-0.351-44.877l0.371-0.377L467.225 597.8 361.82 492.487c-12.502-12.49-12.511-32.752-0.02-45.254 12.367-12.378 32.348-12.51 44.878-0.391l0.377 0.37L512.5 552.566l105.445-105.352c12.502-12.492 32.764-12.483 45.255 0.02z m9.362-284.027V267.25h104.135L672.562 163.206z" p-id="21472" fill="#222e6e"></path>
    </svg>
    `;
    const likeSvg = `
    <svg t="1761805134075" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="51324" width="16" height="16">
        <path d="M933.160516 335.104584v-0.127988c-39.99625-10.431022-134.067431-10.23904-271.654532-13.950692 6.559385-30.013186 7.99925-57.082649 7.99925-105.110146C669.601225 101.046527 585.929069 0 511.920007 0c-52.283098 0-95.351061 42.715995-95.991 95.25507-0.735931 64.377965-20.47808 175.631535-127.988001 232.138237-7.871262 4.095616-30.333156 15.166578-33.628848 16.702434L255.976002 345.567603A101.302503 101.302503 0 0 0 191.982002 319.970003H95.991001a96.054995 96.054995 0 0 0-95.991001 95.927007v512.175983a96.086992 96.086992 0 0 0 95.991001 95.927007h95.991001a94.775115 94.775115 0 0 0 85.399993-55.258819l1.439865 0.383964 7.679281 2.047808c0.511952 0 0.863919 0.127988 1.407868 0.351967 18.462269 4.607568 53.946942 12.990782 129.715839 30.39715 16.414461 3.711652 102.166422 22.07793 191.214073 22.07793h174.895604c53.307002 0 92.791301-19.902134 114.549261-61.690217 21.310002-37.148517 125.748211-236.393838 125.620223-517.231509 0-40.444208-27.965378-92.023373-90.743493-109.941693"
          fill="transparent" p-id="51325">
        </path>
        <path d="M959.814017 444.982283c0 11.006968-0.223979 9.471112-0.63994 20.126113-13.310752 321.441865-117.301003 466.900228-117.301003 466.900228-9.727088 17.438365-25.213636 27.933381-58.23454 27.933381h-174.799613c-87.927757 0-175.087586-19.934131-177.359372-20.510077-132.851545-30.589132-139.826891-32.95691-148.146112-35.324688l4.575572-470.099928c0-48.603443 1.023904-36.284598 0-35.676655 146.226291-60.474331 190.734119-193.261882 191.982001-302.243665a32.124988 32.124988 0 0 1 32.028997-31.997c33.852826 0 93.591226 67.897635 93.591226 151.985751 0 75.832891-3.071712 88.85567-29.597225 167.920257 319.938006 0 317.730213 4.607568 345.951567 11.998875 35.100709 10.047058 37.948442 38.972346 37.948442 48.987408zM223.947005 927.913008c0 17.662344-14.366653 31.997-31.965003 31.997H95.991001c-17.662344 0-31.997-14.334656-31.997-31.997V415.89701c0-17.662344 14.334656-31.997 31.997-31.997001h95.991001c17.726338 0 31.965003 14.334656 31.965003 31.997001V927.913008z m709.213511-592.872418v-0.031997c-39.99625-10.495016-134.067431-10.23904-271.654532-13.982689 6.559385-30.07718 7.99925-57.114646 7.99925-105.078149C669.601225 101.078524 585.929069 0 511.920007 0c-52.283098 0-95.351061 42.779989-95.991 95.25507-0.735931 64.409962-20.47808 175.663532-127.988001 232.10624-7.871262 4.095616-30.333156 15.198575-33.628848 16.734431L255.976002 345.567603a101.622473 101.622473 0 0 0-63.994-25.629597H95.991001a96.118989 96.118989 0 0 0-95.991001 95.991001V927.913008c0 52.923038 43.067962 95.991001 95.991001 95.991001h95.991001a94.967097 94.967097 0 0 0 85.399993-55.258819l1.439865 0.415961 7.679281 2.047808a8363.151955 8363.151955 0 0 0 131.155704 30.749117c16.382464 3.711652 102.134425 22.045933 191.182076 22.045933h174.895604c53.307002 0 92.791301-19.870137 114.549261-61.754211 21.310002-37.084523 125.748211-236.329844 125.620223-517.167515 0-40.380214-27.965378-91.991376-90.743493-109.941693z"
          fill="#FF8214" p-id="51326">
        </path>
    </svg>
    `;
    const dislikeSvg = `
    <svg t="1761805134075" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="51324" width="16" height="16">
        <path d="M933.160516 335.104584v-0.127988c-39.99625-10.431022-134.067431-10.23904-271.654532-13.950692 6.559385-30.013186 7.99925-57.082649 7.99925-105.110146C669.601225 101.046527 585.929069 0 511.920007 0c-52.283098 0-95.351061 42.715995-95.991 95.25507-0.735931 64.377965-20.47808 175.631535-127.988001 232.138237-7.871262 4.095616-30.333156 15.166578-33.628848 16.702434L255.976002 345.567603A101.302503 101.302503 0 0 0 191.982002 319.970003H95.991001a96.054995 96.054995 0 0 0-95.991001 95.927007v512.175983a96.086992 96.086992 0 0 0 95.991001 95.927007h95.991001a94.775115 94.775115 0 0 0 85.399993-55.258819l1.439865 0.383964 7.679281 2.047808c0.511952 0 0.863919 0.127988 1.407868 0.351967 18.462269 4.607568 53.946942 12.990782 129.715839 30.39715 16.414461 3.711652 102.166422 22.07793 191.214073 22.07793h174.895604c53.307002 0 92.791301-19.902134 114.549261-61.690217 21.310002-37.148517 125.748211-236.393838 125.620223-517.231509 0-40.444208-27.965378-92.023373-90.743493-109.941693"
          fill="transparent" p-id="51325" transform="scale(1,-1) translate(0,-1024)">
        </path>
        <path d="M959.814017 444.982283c0 11.006968-0.223979 9.471112-0.63994 20.126113-13.310752 321.441865-117.301003 466.900228-117.301003 466.900228-9.727088 17.438365-25.213636 27.933381-58.23454 27.933381h-174.799613c-87.927757 0-175.087586-19.934131-177.359372-20.510077-132.851545-30.589132-139.826891-32.95691-148.146112-35.324688l4.575572-470.099928c0-48.603443 1.023904-36.284598 0-35.676655 146.226291-60.474331 190.734119-193.261882 191.982001-302.243665a32.124988 32.124988 0 0 1 32.028997-31.997c33.852826 0 93.591226 67.897635 93.591226 151.985751 0 75.832891-3.071712 88.85567-29.597225 167.920257 319.938006 0 317.730213 4.607568 345.951567 11.998875 35.100709 10.047058 37.948442 38.972346 37.948442 48.987408zM223.947005 927.913008c0 17.662344-14.366653 31.997-31.965003 31.997H95.991001c-17.662344 0-31.997-14.334656-31.997-31.997V415.89701c0-17.662344 14.334656-31.997 31.997-31.997001h95.991001c17.726338 0 31.965003 14.334656 31.965003 31.997001V927.913008z m709.213511-592.872418v-0.031997c-39.99625-10.495016-134.067431-10.23904-271.654532-13.982689 6.559385-30.07718 7.99925-57.114646 7.99925-105.078149C669.601225 101.078524 585.929069 0 511.920007 0c-52.283098 0-95.351061 42.779989-95.991 95.25507-0.735931 64.409962-20.47808 175.663532-127.988001 232.10624-7.871262 4.095616-30.333156 15.198575-33.628848 16.734431L255.976002 345.567603a101.622473 101.622473 0 0 0-63.994-25.629597H95.991001a96.118989 96.118989 0 0 0-95.991001 95.991001V927.913008c0 52.923038 43.067962 95.991001 95.991001 95.991001h95.991001a94.967097 94.967097 0 0 0 85.399993-55.258819l1.439865 0.415961 7.679281 2.047808a8363.151955 8363.151955 0 0 0 131.155704 30.749117c16.382464 3.711652 102.134425 22.045933 191.182076 22.045933h174.895604c53.307002 0 92.791301-19.870137 114.549261-61.754211 21.310002-37.084523 125.748211-236.329844 125.620223-517.167515 0-40.380214-27.965378-91.991376-90.743493-109.941693z"
          fill="#1465FF" p-id="51326" transform="scale(1,-1) translate(0,-1024)">
        </path>
    </svg>
    `;
    const clearSvg = `
    <svg viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="16.000000" height="16.000000" fill="none" customFrame="#000000">
        <g id="组合 52">
            <path id="矢量 61" d="M6.95426 1.61341L0.996021 7.5727C0.767321 7.80144 0.400197 7.80144 0.171497 7.5727C-0.057203 7.34396 -0.0541938 6.97677 0.171497 6.74803L6.12974 0.788743C6.35844 0.560002 6.72556 0.560002 6.95426 0.788743C7.18296 1.01748 7.18296 1.38467 6.95426 1.61341Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 62" d="M15.2116 9.86878L9.25335 15.8281C9.02465 16.0568 8.65752 16.0568 8.42882 15.8281C8.20012 15.5993 8.20313 15.2321 8.42882 15.0034L14.3871 9.04411C14.6158 8.81537 14.9829 8.81537 15.2116 9.04411C15.4403 9.27285 15.4373 9.64305 15.2116 9.86878Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 63" d="M6.93623 9.90492L5.13372 11.7078C4.90502 11.9365 4.53789 11.9365 4.30919 11.7078C4.08049 11.479 4.0835 11.1118 4.30919 10.8831L6.11171 9.08025C6.34041 8.85151 6.70753 8.85151 6.93623 9.08025C7.16493 9.30899 7.16493 9.67919 6.93623 9.90492Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 64" d="M4.26406 8.22865L2.95806 9.53487C2.72936 9.76361 2.36223 9.76361 2.13353 9.53487C1.90483 9.30613 1.90784 8.93895 2.13353 8.71021L3.43953 7.40398C3.66823 7.17524 4.03536 7.17524 4.26406 7.40398C4.48975 7.63272 4.48975 8.00292 4.26406 8.22865Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 65" d="M8.58223 12.5504L7.27623 13.8567C7.04753 14.0854 6.68041 14.0854 6.45171 13.8567C6.22301 13.6279 6.22602 13.2607 6.45171 13.032L7.75771 11.7258C7.98641 11.497 8.35353 11.497 8.58223 11.7258C8.81093 11.9545 8.81093 12.3217 8.58223 12.5504Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 66" d="M8.42579 15.8281L0.171525 7.57542C-0.057175 7.34668 -0.057175 6.97949 0.171525 6.75075C0.400225 6.52201 0.767349 6.52502 0.996049 6.75075L9.25032 15.0065C9.47902 15.2352 9.47902 15.6024 9.25032 15.8311C9.02162 16.0569 8.65449 16.0569 8.42579 15.8281Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 67" d="M11.8533 12.4032L3.59902 4.14744C3.37032 3.9187 3.37032 3.55151 3.59902 3.32277C3.82772 3.09403 4.19484 3.09704 4.42354 3.32277L12.6748 11.5785C12.9035 11.8072 12.9035 12.1744 12.6748 12.4032C12.4491 12.6319 12.079 12.6289 11.8533 12.4032Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 68" d="M15.004 2.88318L13.1202 0.996074C12.8915 0.767334 12.8915 0.400145 13.1202 0.171405C13.3489 -0.0573353 13.716 -0.0543256 13.9447 0.171405L15.8285 2.05851C16.0572 2.28725 16.0572 2.65444 15.8285 2.88318C15.5998 3.11192 15.2297 3.11192 15.004 2.88318Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 69" d="M9.3165 4.79456L6.13576 1.61326C5.90706 1.38452 5.90706 1.01733 6.13576 0.788593C6.36446 0.559852 6.73158 0.562862 6.96028 0.788593L10.144 3.9729C10.3727 4.20164 10.3727 4.56883 10.144 4.79757C9.91533 5.0233 9.5452 5.0233 9.3165 4.79456Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 70" d="M14.3871 9.86585L11.2063 6.68455C10.9776 6.45581 10.9776 6.08862 11.2063 5.85988C11.435 5.63114 11.8021 5.63415 12.0308 5.85988L15.2146 9.04419C15.4433 9.27293 15.4433 9.64012 15.2146 9.86886C14.9859 10.0946 14.6158 10.0946 14.3871 9.86585Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 71" d="M15.8254 2.88319L12.0278 6.68148C11.7991 6.91022 11.4319 6.91022 11.2032 6.68148C10.9745 6.45274 10.9775 6.08555 11.2032 5.85681L15.0009 2.05852C15.2296 1.82978 15.5967 1.82978 15.8254 2.05852C16.0541 2.28425 16.0541 2.65445 15.8254 2.88319Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
            <path id="矢量 72" d="M13.9417 0.996224L10.141 4.79452C9.91234 5.02326 9.54522 5.02326 9.31652 4.79452C9.08782 4.56578 9.09083 4.19859 9.31652 3.96985L13.1172 0.171555C13.3459 -0.0571851 13.713 -0.0571851 13.9417 0.171555C14.1704 0.400295 14.1674 0.767484 13.9417 0.996224Z" fill="rgb(86,186,51)" fill-rule="nonzero" />
        </g>
    </svg>
    `;
    const loadingSvg = `
    <svg t="1757316892265" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="4636" width="18" height="18">
        <path d="M469.333333 85.333333m42.666667 0l0 0q42.666667 0 42.666667 42.666667l0 128q0 42.666667-42.666667 42.666667l0 0q-42.666667 0-42.666667-42.666667l0-128q0-42.666667 42.666667-42.666667Z" fill="#13227a" opacity=".8" p-id="4637"></path><path d="M469.333333 725.333333m42.666667 0l0 0q42.666667 0 42.666667 42.666667l0 128q0 42.666667-42.666667 42.666667l0 0q-42.666667 0-42.666667-42.666667l0-128q0-42.666667 42.666667-42.666667Z" fill="#13227a" opacity=".4" p-id="4638" data-spm-anchor-id="a313x.search_index.0.i1.16ad3a81Tc0Y2F" class=""></path><path d="M938.666667 469.333333m0 42.666667l0 0q0 42.666667-42.666667 42.666667l-128 0q-42.666667 0-42.666667-42.666667l0 0q0-42.666667 42.666667-42.666667l128 0q42.666667 0 42.666667 42.666667Z" fill="#13227a" opacity=".2" p-id="4639"></path><path d="M298.666667 469.333333m0 42.666667l0 0q0 42.666667-42.666667 42.666667l-128 0q-42.666667 0-42.666667-42.666667l0 0q0-42.666667 42.666667-42.666667l128 0q42.666667 0 42.666667 42.666667Z" fill="#13227a" opacity=".6" p-id="4640"></path><path d="M783.530667 180.138667m30.169889 30.169889l0 0q30.169889 30.169889 0 60.339779l-90.509668 90.509668q-30.169889 30.169889-60.339779 0l0 0q-30.169889-30.169889 0-60.339779l90.509668-90.509668q30.169889-30.169889 60.339779 0Z" fill="#13227a" opacity=".1" p-id="4641"></path><path d="M330.965333 632.661333m30.16989 30.16989l0 0q30.169889 30.169889 0 60.339778l-90.509668 90.509668q-30.169889 30.169889-60.339779 0l0 0q-30.169889-30.169889 0-60.339778l90.509668-90.509668q30.169889-30.169889 60.339779 0Z" fill="#13227a" opacity=".5" p-id="4642"></path><path d="M843.861333 783.530667m-30.169889 30.169889l0 0q-30.169889 30.169889-60.339779 0l-90.509668-90.509668q-30.169889-30.169889 0-60.339779l0 0q30.169889-30.169889 60.339779 0l90.509668 90.509668q30.169889 30.169889 0 60.339779Z" fill="#13227a" opacity=".3" p-id="4643"></path><path d="M391.338667 330.965333m-30.16989 30.16989l0 0q-30.169889 30.169889-60.339778 0l-90.509668-90.509668q-30.169889-30.169889 0-60.339779l0 0q30.169889-30.169889 60.339778 0l90.509668 90.509668q30.169889 30.169889 0 60.339779Z" fill="#13227a" opacity=".7" p-id="4644"></path>
    </svg>
    `;

    const feeddbackHtml = `
    <div class="chat-feedback-box">
        <div class="chat-feedback-item">
            <button class="chat-feedback-btn chat-copy-btn">
                <span class="chat-feedback-icon">${copySvg}</span>
                <span class="chat-feedback-text">Copy</span>
            </button>
            <button class="chat-feedback-btn chat-clear-btn">
                <span class="chat-feedback-icon">${clearSvg}</span>
                <span class="chat-feedback-text">Clear</span>
            </button>
        </div>
        <div class="chat-feedback-item">
            <button class="chat-feedback-btn chat-like-btn" value="positive">
                <span class="chat-feedback-icon">${likeSvg}</span>
                <span class="chat-feedback-text">Like</span>
            </button>
            <button class="chat-feedback-btn chat-dislike-btn" value="negative">
                <span class="chat-feedback-icon">${dislikeSvg}</span>
                <span class="chat-feedback-text">Dislike</span>
            </button>
        </div>
    </div>
    `;

    function adjustEleHeight(element) {
        element.style.height = 'auto';
        if (element.value) {
            element.style.height = element.scrollHeight + 'px';
        }
        // 获取 max-height，可能在行内或 css 中
        let maxHeight = window.getComputedStyle(element).maxHeight;
        // 如果 maxHeight 未设置或为 none，不判断
        if (maxHeight && maxHeight !== 'none') {
            let max = parseFloat(maxHeight);
            if (element.scrollHeight > max) {
                element.parentElement.style.paddingBottom = '44px';
            } else {
                element.parentElement.style.paddingBottom = '0';
            }
        } else {
            element.parentElement.style.paddingBottom = '0';
        }
    }

    function debounce(func, wait) {
        let timeout;
        return function (...args) {
            const context = this;
            clearTimeout(timeout);
            timeout = setTimeout(() => func.apply(context, args), wait);
        };
    }

    // 判断元素是否已达底部
    function isEleAtBottom(node, threshold = 3) {
        return node.scrollTop + node.clientHeight >= node.scrollHeight - threshold;
    }

    function scrollToBottom(node) {
        node.scrollTo({
            top: node.scrollHeight,
            behavior: 'smooth'
        });
    }
    const debouncedScrollToBottom = debounce(scrollToBottom, 50);

    function initMarked() {
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
            mangle: true, //对嵌入的电子邮件地址进行加密处理（防止抓取），实现简单的防邮件爬虫
            highlight: function(code, lang) {
                // 如果指定了语言，并且hljs支持，按指定高亮
                if (lang && hljs.getLanguage(lang)) {
                    return hljs.highlight(code, { language: lang }).value;
                }
                // 否则自动检测语言高亮
                return hljs.highlightAuto(code).value;
            }
        });
    }

    function initClipboard(){
        const clipboard = new ClipboardJS('.copybtn');

        clipboard.on('success', function(e) {
            e.trigger.innerHTML = codeCpDoneSvg;
            setTimeout(function() {
                e.trigger.innerHTML = codeCopySvg;
            }, 1200);
        });

        clipboard.on('error', function(e) {
            e.trigger.innerHTML = copyFailSvg;
            setTimeout(function() {
                e.trigger.innerHTML = codeCopySvg;
            }, 1200);
        });
    }

    /**
     * 从源数组倒数第 n 个元素开始，向前复制指定数量的元素，并只保留每项的 role 和 content 属性
     * @param {Array} sourceArr 源数组
     * @param {number} count 要复制的元素个数
     * @param {number} n 倒数第 n 个元素作为起始（n >= 1）
     * @returns {Array} 新数组（只包含 role 和 content 属性）
     */
    function copyArrFromEnd(sourceArr, count, n) {
        if (!Array.isArray(sourceArr) || count <= 0 || n <= 0) {
            return [];
        }
        const arrLen = sourceArr.length;
        let endIdx = arrLen - n;
        let startIdx = endIdx - count + 1;

        // 防止越界，确保 startIdx >= 0
        startIdx = Math.max(0, startIdx);

        // slice 截取时需加1，因为 slice(开始, 结束)不包含结束，所以最后+1
        let subArr = sourceArr.slice(startIdx, endIdx + 1);
        // 只保留每项的 role 和 content 属性
        let result = subArr.map(item => ({
            role: item.role,
            content: item.content
        }));

        return result;
    }

    function chatWidgetInit(config) {
        const chatPlaceholder = config.chatWidgetPlaceholder || 'Type your question';
        const chatAIBase = config.chatAIBase;
        const isMultilingual = window.isMultilingual == "True";
        let docTarget = "rtl87xxx";

        const aiEnvString = "PROD"; // aiEnv is PROD or QA
        /* siteDomain is "https://www.realmcu.com/" or "https://wwwqa.realmcu.com/" */
        const siteDomain = "https://www.realmcu.com/";  // default is formal realmcu site
        if (aiEnvString.toLowerCase() === "qa") {
            siteDomain = "https://wwwdev.realmcu.com/";
        }
        const chatUrl = `${siteDomain}docs/aichatstream`;
        const feedbackUrl = `${siteDomain}docs/AIFeedback`;

        inserChatWidgetHTML(logoSvg, "RealMCU AI ASK", chatPlaceholder, docTarget.replace(/realAIfuel/gi, ''));
        initMarked();
        initClipboard();

        let lastAbortCtrl = null;
        let chatHistoryList = [{"id": `mdblock-${Date.now()}`, "role": "assistant", "content": "Hi there! How can I help?"}];
        let docRefsList = [];

        const chatModalNode = document.getElementById('ChatModal');
        const chatContentNode = document.querySelector('.chat-content');
        const userInputNode = document.querySelector('.chat-input textarea');
        adjustEleHeight(userInputNode);

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

        /* ============= show or hide ai chat modal ============= */
        const wyGridContainer = document.querySelector('.wy-grid-for-nav');
        const chatModalMaskNode = document.getElementById('ChatModalMask');
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
        toggleChatModalButton.onclick = function() {
            if(chatModalMaskNode.style.display === 'none' || chatModalMaskNode.style.display === '') {
                document.documentElement.classList.add('overY-clip');
                wyGridContainer.classList.add("prevent-manipulation");
                chatModalMaskNode.style.display = 'block';
                userInputNode.focus();
                updateChatNoteFont();
            }
            else {
                userInputNode.blur();
                chatModalMaskNode.style.display = 'none';
                document.documentElement.classList.remove('overY-clip');
                wyGridContainer.classList.remove("prevent-manipulation");
            }
        };

        /* ==================================================================
           header widgets: drag modal, clear screen, full screen, close modal
           ================================================================== */
        /* -------------------- drag modal -------------------- */
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

        let isDragging = false;
        let dragX, dragY;
        const dragBar = chatModalNode.querySelector('.chat-title');
        dragBar.addEventListener('mousedown', (e) => {
            e.preventDefault(); // 阻止默认行为，防止文本选择
            isDragging = true;
            dragX = e.clientX - chatModalNode.offsetLeft;
            dragY = e.clientY - chatModalNode.offsetTop;

            document.addEventListener('mousemove', startDrag);
            document.addEventListener('mouseup', stopDrag);
        });

        /* -------------------- clear screen --------------------- */
        function delAllClearBtns() {
            $('.chat-clear-btn').remove();
        }
        $('.chat-content').on('click', '.chat-clear-btn', function(e) {
            e.stopPropagation(); // 防止事件冒泡
            if (typeof chatContentNode !== "undefined" && chatContentNode) {
                chatContentNode.innerHTML = '';
            }
            lastAbortCtrl = null;
            chatHistoryList = [];
            docRefsList = [];
        });
        /* --------------------- full screen --------------------- */
        const fullScreenCtrlNode = document.getElementById('FullScreenControl');
        fullScreenCtrlNode.addEventListener('click', function() {
            chatModalNode.style.top = '';
            chatModalNode.style.left = '';
            chatModalNode.classList.toggle('fullscreen-chat-modal');
            updateChatNoteFont();
        });
        /* --------------------- close modal --------------------- */
        const closeChatModalButton = document.getElementById('CloseChatModal');
        closeChatModalButton.onclick = function() {
            userInputNode.blur();
            chatModalMaskNode.style.display = 'none';
            document.documentElement.classList.remove('overY-clip');
            wyGridContainer.classList.remove("prevent-manipulation");
        };

        /* =======================================================
           footer widgets: resize screen
           ======================================================= */
        /* -------------------- resize screen -------------------- */
        const resizeCtrl = chatModalNode.querySelector('.resize-modal');
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
        resizeCtrl.addEventListener('mousedown', function(e) {
            isResizing = true;
            resizeX = e.clientX;
            resizeY = e.clientY;
            startWidth = parseInt(window.getComputedStyle(chatModalNode).width, 10);
            startHeight = parseInt(window.getComputedStyle(chatModalNode).height, 10);
            document.addEventListener('mousemove', startResize);
            document.addEventListener('mouseup', stopResize);
        });

        /* ============== listen textarea input event ============== */
        userInputNode.addEventListener('input', function() {
            adjustEleHeight(this);
            sendButton.disabled = !this.value.trim();
        });

        /* =======================================================
           handle chat-content element scroll
           ======================================================= */
        /* -------- listen chat-content-gobtm click event -------- */
        let chatContentAutoScroll = true;
        /* ----- listen wheel and touch events in chat content ----- */
        function onChatContentWheelOrTouch(direction) {
            if (direction === 'up') {
                chatContentAutoScroll = false;
            } else if (direction === 'down') {
                if (isEleAtBottom(chatContentNode, 100)) {
                    chatContentAutoScroll = true;
                } else {
                    chatContentAutoScroll = false;
                }
            }
        }
        function handleWheelScroll(e) {
            if (e.deltaY < 0) {
                onChatContentWheelOrTouch("up");
            } else {
                onChatContentWheelOrTouch("down");
            }
        }

        let wheelY = 0;
        function handleTouchStart(e) {
            if (e.touches.length === 1) wheelY = e.touches[0].clientY;
        }
        function handleTouchEnd(e) {
            if (e.changedTouches.length === 1) {
                const endY = e.changedTouches[0].clientY;
                const deltaY = endY - wheelY;
                if (deltaY < -30) {
                    onChatContentWheelOrTouch("up");
                } else if (deltaY > 30) {
                    onChatContentWheelOrTouch("down");
                }
            }
        }

        const debouncedWheelScroll = debounce(handleWheelScroll, 60);
        const debouncedTouchStart = debounce(handleTouchStart, 60);
        const debouncedTouchEnd = debounce(handleTouchEnd, 60);
        chatContentNode.addEventListener('wheel', debouncedWheelScroll);
        chatContentNode.addEventListener('touchstart', debouncedTouchStart);
        chatContentNode.addEventListener('touchend', debouncedTouchEnd);

        /* ------- listen guide question item click event ------- */
        $(document).on('click', '.chat-guide-question li', function(){
            const liText = $(this).text().trim();
            sendChatMessage(liText);
        });

        /* =======================================================
           handle feedback buttons click event: copy, like, dislike
           ======================================================= */
        function updateFeedButton(element, icon, text) {
            element.html(`
                <span class="chat-feedback-icon">${icon}</span>
                <span class="chat-feedback-text">${text}</span>
            `);
        }
        /* ----------- listen copy button click event ----------- */
        $(".chat-content").on("click", ".chat-copy-btn", function() {
            const copyBtn = $(this);

            const resetCopyBtn = (text) => {
                updateFeedButton(copyBtn, copySvg, text);
                setTimeout(() => updateFeedButton(copyBtn, copySvg, "Copy"), 2000);
            };

            const chatRespMain = copyBtn.closest('.chatResp');
            if (!chatRespMain.length) return resetCopyBtn("Failed!");

            const matchedChat = chatHistoryList.find(item => item.id === chatRespMain[0].id);
            if (!matchedChat) return resetCopyBtn("Failed!");

            let aiMdData = matchedChat.content;
            // --- 用 DOM 方式解析 ---
            let parser = new DOMParser();
            let docText = parser.parseFromString(`<div>${aiMdData}</div>`, "text/html"); //外层div防止body干扰

            let guideLines = [];
            // 1. 查找 <Question>
            let questionEl = docText.querySelector("Question");
            if (questionEl) {
                // 检查 <Question> 里面是否有 ul.chat-guide-question
                let ulInQuestion = questionEl.querySelector(".chat-guide-question");
                if (ulInQuestion) {
                    // 只抽取 li 列表文本
                    ulInQuestion.querySelectorAll("li").forEach(li => {
                        let txt = li.innerText || li.textContent;
                        if (txt) guideLines.push(txt.trim());
                    });
                } else {
                    // <Question>内为纯文本
                    let qText = questionEl.innerText || questionEl.textContent;
                    if (qText && qText.trim()) guideLines.push(qText.trim());
                }
                questionEl.remove();
            }

            // 2. 查找 .chat-guide-question
            let guideUlEl = docText.querySelector(".chat-guide-question");
            if (guideUlEl) {
                guideUlEl.querySelectorAll("li").forEach(li => {
                    let txt = li.innerText || li.textContent;
                    if (txt) guideLines.push(txt.trim());
                });
                // 有些ul结构直接是纯文本，不是li
                if (!guideUlEl.querySelector("li")) {
                    let ulText = guideUlEl.innerText || guideUlEl.textContent;
                    if (ulText && ulText.trim()) guideLines.push(ulText.trim());
                }
                guideUlEl.remove();
            }

            // 3. 合成
            let textToCopy = docText.body.innerText.trim();
            if (guideLines.length) {
                textToCopy += "\n" + guideLines.join('\n');
            }

            navigator.clipboard.writeText(textToCopy).then(() => {
                    resetCopyBtn("Copied!");
                }).catch((error) => {
                    resetCopyBtn("Failed!");
                });
        });
        /* --------- listen like&dislike button click event --------- */
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
        $(".chat-content").on("click", ".chat-like-btn, .chat-dislike-btn", function(e) {
            e.preventDefault();
            const opBtn = $(this);
            const chatRespMain = opBtn.closest('.chatResp');
            if (!chatRespMain.length) return;

            let opIconHTML = "";
            if (opBtn.hasClass('chat-like-btn')) {
                opIconHTML = likeSvg;
            } else {
                opIconHTML = dislikeSvg;
            }
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
                    updateFeedButton(opBtn, opIconHTML, "Failed!");
                    setTimeout(() => {
                        updateFeedButton(opBtn, opIconHTML, opTextHTML);
                    }, 2000);
                }
            }).catch(error => {
                updateFeedButton(opBtn, opIconHTML, "Failed!");
                setTimeout(() => {
                    updateFeedButton(opBtn, opIconHTML, opTextHTML);
                }, 2000);
            });
        });

        /* =======================================================
           handle chat message send
           ======================================================= */
        function handleRefDocs(list) {
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

                    // 判断 http 和 https 协议
                    const isAbsoluteUrl = /^https?:\/\/.*/i.test(link.url);
                    // 不是绝对地址时自动加域名
                    const fullUrl = isAbsoluteUrl ? link.url : `https://docs.realmcu.com${link.url}`;
                    linksHtml += `
                    <li>
                        <a href="${fullUrl}" target="_blank">${link.title}</a>
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

        /* ----------------- save ai chat history ----------------- */
        function addChatHistory(id, role, text) {
            // let forwardText = text.replace(/(\\n|\n)+/g, '').trim();
            let forwardText = text;
            let newChat = {
                "id": id,
                "role":role,
                "content": forwardText
            };
            chatHistoryList.push(newChat);
        }
        /* ------------ handle chat message send success ------------ */
        function onChatSuccess(mdContent, refs, element) {
            const eleId = element.id && element.id.trim() !== '' ? element.id : `mdblock-${Date.now()}`;
            element.insertAdjacentHTML('beforeend', feeddbackHtml);
            let refsHtml = handleRefDocs(refs);
            element.insertAdjacentHTML('beforeend', refsHtml);
            addChatHistory(eleId, "assistant", mdContent);
        }
        /* ------------- handle chat message send fail -------------- */
        function onChatError(error, element) {
            const eleId = element.id && element.id.trim() !== '' ? element.id : `mdblock-${Date.now()}`;
            element.innerHTML = `
            <div class="chatResp">${error}</div>
            `;
            docRefsList = [];
            addChatHistory(eleId, "assistant", error);
        }

        /* ------------ generate ai chat fetch parameters ------------ */
        function AIChatFetchConfig(question, histories) {
            let docSwitch = document.getElementById('DocSwitch');
            let curVersion = "";
            const rawBody = {
                aiEnv: aiEnvString,     // aiEnv is PROD or QA
                docTarget: docSwitch.checked ? docTarget : '',   // optional, 文档标识，用于文档分类
                rawData: question,      // required, 要詢問 AI 的問句
                docBase: chatAIBase,    // required, 知識庫id(RS_realmcu_B)
                docVersion: curVersion, // optional, 文檔版本號，若沒有則 AI 會以最新的版本回答
                // optional, 過去的對話紀錄，請符合範例的格式，
                // 目前可以輸入長度最多為 6 (三組 user-assistant 組合).若無對話紀錄則不需輸入
                historyMsg: histories,
                fileName: "",
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
                    body,
                    credentials: 'include'
                }
            };
        }
        /* ------------- start ai chat fetch request ------------- */
        let isSending = false; // sending message status param
        async function fetchChatAnwser(fetchConfig, messageNode) {
            if (lastAbortCtrl) {
                lastAbortCtrl.abort();
            }

            const { url, options } = fetchConfig;
            const abortCtrl = new AbortController();
            lastAbortCtrl = abortCtrl;
            let hasChatError = false;
            let aiMdData = "";
            let aiRefList = [];

            // ============ Interrupt chat request ============
            const abortButton = document.getElementById('ChatAbortBtn');
            if (abortButton) {
                abortButton.onclick = null;
                abortButton.onclick = () => abortCtrl.abort();
            }

            function appendChatError(error, element) {
                if (hasChatError) return;

                element.innerHTML += `<div>${error}</div>`;
                docRefsList = [];
                abortCtrl.abort();
                hasChatError = true;
            }
            function updateSendingStatus(startSending) {
                isSending = startSending;
                sendButton.classList.toggle("btn-hidden", startSending);
                // 发送开始时移除所有清除按钮
                if (startSending) delAllClearBtns();
                // 发送停止时，根据输入内容设置发送按钮是否可用
                if (!startSending) sendButton.disabled = !userInputNode.value.trim();
                if (abortButton) abortButton.classList.toggle("btn-hidden", !startSending);
            }

            /**
             * 将 <Question>...</Question> 替换为 <div class="chat-guide-question">...</div>
             * 忽略标签大小写，支持多行内容
             * @param {string} md 原始Markdown字符串
             * @returns {string} 替换后的Markdown字符串
             */
            function processQuestionBlock(md) {
                if (typeof md !== 'string') return md;
                return md.replace(/<question>([\s\S]*?)<\/question>/gi, (_, content) => {
                    content = content.trim();
                    if (!content) return '';
                    return `\n<div class="chat-guide-question">\n${content}\n</div>\n`;
                });
            }
            /* ====================================================
                parsed data chunk example:
                {
                    "role": "assistant",
                    "content": "xxxxxx",
                    "docs": [{"url": "xxxx", "title": "xxxx"}],
                    "status": "success",    // success or error
                    "error": null           // null or {"error_type": "xxxx", "error_message": "xxxx"}
                }
               ====================================================== */
            function processChunk(chunkData, element) {
                try {
                    const parsedChunk = JSON.parse(chunkData);
                    if (parsedChunk.status === "success") {
                        aiMdData = parsedChunk.content;
                        aiRefList = parsedChunk.docs || [];

                        // replace element html content
                        const formatMdData = processQuestionBlock(aiMdData);
                        const rawHtml = marked.parse(formatMdData);
                        // const sanitizedHtml = DOMPurify.sanitize(rawHtml);
                        // element.innerHTML = sanitizedHtml;
                        element.id = parsedChunk.message_id || "";
                        element.innerHTML = rawHtml;

                    } else if (parsedChunk.status === "error") {
                        // append html content in element
                        appendChatError("Internal server error, please refresh and try again!", element);
                    }
                } catch (e) { /* skip, process next data chunk */ }

                if(chatContentAutoScroll) {
                    //scrollToBottom(chatContentNode);
                    debouncedScrollToBottom(chatContentNode);
                }
            }

            function addCodeCopyButtons(element) {
                try {
                    element.querySelectorAll('pre code').forEach((blk, i) => {
                        // 1. 高亮code block
                        hljs.highlightElement(blk);
                        // 2. 获取pre标签
                        const pre = blk.parentElement;
                        if (!pre || pre.tagName.toLowerCase() !== 'pre') return;
                        // 3. 构造 id：codeblock-时间戳
                        const codeId = `codeblock-${Date.now()}-${i}`;
                        blk.id = codeId;  // 把 id 赋值给 code 标签

                        const fragment = document.createDocumentFragment();
                        // 4. 创建按钮
                        const btn = document.createElement('button');
                        btn.className = 'copybtn o-tooltip--left';
                        // btn.setAttribute('data-tooltip', 'Copy');
                        btn.setAttribute('data-clipboard-target', `#${codeId}`);
                        btn.innerHTML = codeCopySvg;

                        pre.appendChild(btn);
                    });
                } catch (e) { /* skip, process next data chunk */ }
            }

            try {
                updateSendingStatus(true);
                chatContentAutoScroll = true;
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

                const reader = response.body.getReader();
                const decoder = new TextDecoder("utf-8");
                let pendingBuffer = '';

                // Continuously read data from the stream
                while (true) {
                    const { done, value } = await reader.read().catch(error => {
                        if (error.name === 'AbortError') {
                            appendChatError("AI fetch request has been aborted!", asstNode);
                        } else {
                            appendChatError("Failed to read data stream, please refresh and try again!", asstNode);
                        }
                        return { done: true };
                    });
                    if (done || hasChatError) break;

                    // Decode the current chunk
                    const newChunk = decoder.decode(value, { stream: true });
                    processChunk(newChunk, asstNode);

                    pendingBuffer += newChunk;
                    let lineEnd;
                    while ((lineEnd = pendingBuffer.indexOf('\n')) >= 0) {
                        // const lineChunk = pendingBuffer.slice(0, lineEnd).trim();
                        const lineChunk = pendingBuffer.slice(0, lineEnd);
                        pendingBuffer = pendingBuffer.slice(lineEnd + 1);

                        if (!lineChunk) continue;

                        processChunk(lineChunk, asstNode);
                    }
                }

                if (pendingBuffer && !hasChatError) {
                    processChunk(pendingBuffer, asstNode);
                }

                reader.releaseLock();

                addCodeCopyButtons(asstNode);

                if (!hasChatError) {
                    onChatSuccess(aiMdData, aiRefList, asstNode);
                }
            } catch (error) {
                if (error.name === 'AbortError') {
                    onChatError("AI fetch request has been aborted!", messageNode);
                } else {
                    onChatError("An error occured with the ai chat fetch operation, please refresh and try again!", messageNode);
                }
            }
            finally {
                if (abortCtrl === lastAbortCtrl) {
                    updateSendingStatus(false);
                }
                // scrolll to chatContentNode bottom
                if(chatContentAutoScroll) {
                    scrollToBottom(chatContentNode);
                }
            }
        }

        function sendChatMessage(text) {
            const userInputValue = (typeof text === "string" && text.trim() !== '') ? text : userInputNode.value;
            if (userInputValue.trim() === '') return;
            userInputNode.value = '';
            adjustEleHeight(userInputNode);

            const userMessageNode = document.createElement('div');
            userMessageNode.className = 'chat-message';
            userMessageNode.innerHTML = `<div class="userText">${userInputValue}</div>`;
            chatContentNode.appendChild(userMessageNode);
            const chatHistoryCopy = copyArrFromEnd(chatHistoryList, 6, 1);
            addChatHistory(`mdblock-${Date.now()}`, "user", userInputValue);

            const chatMessageNode = document.createElement('div');
            chatMessageNode.className = 'chat-message';
            chatContentNode.appendChild(chatMessageNode);
            // Use the passed fetchChatAPI function
            chatMessageNode.innerHTML = `<div class="chat-loading-box"><p class"chat-loading-text......">Gathering sources......</p><div class="chat-loading-outer"><div class="chat-loading-inner"></div></div></div>`;
            
            const fetchConfig = AIChatFetchConfig(userInputValue, chatHistoryCopy);
            fetchChatAnwser(fetchConfig, chatMessageNode);

            // chatMessageNode.scrollIntoView({ behavior: 'smooth' });
            scrollToBottom(chatContentNode);
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


    }

    // Expose the init function
    window.chatWidgetInit = chatWidgetInit;
})();

/* ============== Init AI ASK Component ============== */
$(document).ready(function () {
    chatWidgetInit({
        chatWidgetTitle: "RealMCU AI ASK",
        chatWidgetPlaceholder: "Please type your question",
        chatAIBase: window.chatAIBase
    });
})
