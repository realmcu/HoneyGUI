function setAuthUser() {
    const rmcuBase = window.location.href.toLowerCase().includes("docsqa.realmcu.com")
        ? "https://wwwqa.realmcu.com"
        : "https://www.realmcu.com";
    $.ajax({
        url: `${rmcuBase}/Docs/getUserInfo`,
        type: 'GET',
        dataType: "json",
        xhrFields: { withCredentials: true },
        error: function (err) {
            window.kapaSettings = {
                user: {
                    uniqueClientId: 'anonymous',
                    email: 'anonymous'
                }
            };
        },
        success: function (resp) {
            window.kapaSettings = {
                user: {
                    uniqueClientId: resp.username,
                    email: resp.email
                }
            };
        }
    });
}

function curLangIsEN(url) {
    let isEN = true;
    const curUrl = url || window.location.href;
    const lowerUrl = curUrl.toLowerCase();
    if (lowerUrl.indexOf('/cn/') !== -1) {
        isEN = false;
    }

    return isEN;
}

document.addEventListener('DOMContentLoaded', function () {
    if (document.querySelector('script[data-website-id="0772149c-78b9-46aa-99c5-9db5f483c398"]')) {
        setAuthUser();
        return;
    }

    const isEN = curLangIsEN();

    var script = document.createElement("script");
    // common script attributes
    script.async = true;
    script.src = "https://widget.kapa.ai/kapa-widget.bundle.js";
    script.setAttribute("data-website-id", "0772149c-78b9-46aa-99c5-9db5f483c398");
    script.setAttribute("data-source-group-ids-include", "9f0877a0-77ad-4dd3-8bb0-d2637fffdfa9");
    script.setAttribute("data-project-name", "RealMCU AI");
    script.setAttribute("data-bot-protection-mechanism", "hcaptcha");
    script.setAttribute("data-project-color", "#1464FF");
    script.setAttribute("data-project-logo", "/imgs/logo-ai.png");
    script.setAttribute("data-font-family", "Lato,proxima-nova,Helvetica Neue,Arial,sans-serif");
    // button
    script.setAttribute("data-button-image", "/imgs/logo-ai.png");
    script.setAttribute("data-button-text", "Ask AI");
    script.setAttribute("data-button-text-font-size", "0px");
    script.setAttribute("data-button-padding", "0");
    script.setAttribute("data-button-width", "45px");
    script.setAttribute("data-button-height", "45px");
    script.setAttribute("data-button-image-height", "100%");
    script.setAttribute("data-button-image-width", "100%");
    script.setAttribute("data-button-position-right", "18px");
    script.setAttribute("data-button-position-bottom", "25px");
    script.setAttribute("data-button-border-radius", "4px");
    script.setAttribute("data-button-bg-color", "#FFF");
    script.setAttribute("data-button-border", "#FFF");
    script.setAttribute("data-button-animation-enabled", "false");
    script.setAttribute("data-button-hover-animation-enabled", "false");
    script.setAttribute("data-button-box-shadow", "0px 6px 12px 1px rgba(0,0,0,0.16)");
    // modal
    script.setAttribute("data-modal-title", "RealMCU AI");
    script.setAttribute("data-modal-title-font-size", "1.375rem");
    script.setAttribute("data-modal-image-width", "40px");
    script.setAttribute("data-modal-image-height", "40px");
    script.setAttribute("data-modal-z-index", "10000");
    script.setAttribute("data-modal-disclaimer-bg-color", "#F5F7F8");
    script.setAttribute("data-modal-disclaimer-text-color", "#666");
    script.setAttribute("data-modal-example-questions-col-span", "12");
    script.setAttribute("data-example-question-button-padding-x", "1rem");
    script.setAttribute("data-example-question-button-text-color", "#333");
    // feedback,copy,clear buttons
    script.setAttribute("data-answer-feedback-button-text-color", "#444");
    script.setAttribute("data-answer-copy-button-text-color", "#444");
    script.setAttribute("data-thread-clear-button-text-color", "#444");

    if (isEN) {
        script.setAttribute("data-language", "en");
        script.setAttribute(
            "data-modal-disclaimer",
            "This custom large language model (LLM), trained on official documentation from RealMCU, is designed to provide technical support and answers related to RealMCU’s products and services. Give it a try, share your thoughts, and let us know your feedback—we truly appreciate it!\n\n*Note*: Responses are provided by Realtek's AI chatbot and may contain inaccuracies. Realtek is not liable for any damages from its use and offers no warranties."
        );
        script.setAttribute(
            "data-modal-example-questions",
            "Where can I find the official bluetooth technical documentation?,Give me a specific use case of BLE/2.4GHz/Zigbee/Thread/Matter."
        );
        script.setAttribute("data-modal-ask-ai-input-placeholder", "Please type your question");
    } else {
        script.setAttribute("data-language", "zh");
        script.setAttribute(
            "data-modal-disclaimer",
            "欢迎使用RealMCU智能问答助手！本助手基于RealMCU的公开文档，旨在为您提供产品和服务的技术支持与解答。如有任何意见或建议，欢迎留下反馈！\n\n*注意*：Realtek的AI聊天机器人提供的回复可能包含不准确之处。Realtek不对其使用所导致的损害承担责任，也不提供任何保证。"
        );
        script.setAttribute(
            "data-modal-example-questions",
            "哪里可以找到蓝牙技术的官方文档？,请提供一个关于BLE/2.4GHz/Zigbee/Thread/Matter应用的具体示例"
        );
        script.setAttribute("data-modal-ask-ai-input-placeholder", "输入任何问题，与AI互动问答");
    }
    // insert to head
    document.head.appendChild(script);

    setAuthUser();
});
