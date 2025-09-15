const ReturnUrl = window.location.href.toLowerCase();
const language = ReturnUrl.toString().includes("/cn/") ? "zh" : "en";
const docEnv = "PROD"; // docEnv is PROD or QA
let rmcuBase = docEnv.toLowerCase() === "qa" ? "https://wwwqa.realmcu.com" : "https://www.realmcu.com";
$.ajax({
    url: `${rmcuBase}/Docs/CheckDocsPermission`,
    type: 'GET',
    dataType: "json",
    xhrFields: {
        withCredentials: true // 发送请求时需携带cookie
    },
    error: function (err) {   
        window.location.href = `${rmcuBase}/${language}/Home/Error`;
    },
    success: function (resp) {
        var data = resp.Data;
        if(data.cls == "0") {
            if (data.code === "401" || data.code === "405") {
                if (data.code === "401") {
                    // console.log("Please Login ... ");
                    window.location.href = `${rmcuBase}/${language}/Account/Login?ReturnUrl=${ReturnUrl}`;
                } else if (data.code === "405") {
                    // console.log("Sorry, You don't have permission to view documents.");
                    window.location.href = `${rmcuBase}/${language}/Account/MyInfo`;
                }
            }
        }
    }
});