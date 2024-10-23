var ReturnUrl = window.location.href;
$.ajax({
    url: 'https://www.realmcu.com/Docs/CheckDocsPermission',
    type: 'GET',
    dataType: "json",
    xhrFields: {
        withCredentials: true // 发送请求时需携带cookie
    },
    error: function (err) {   
        let domain = window.location.hostname || "";
        if (domain.toLowerCase() == 'docs.realmcu.com') {
            window.location.href = `https://docs.realmcu.com/errorpage.html`;
        }
    },
    success: function (resp) {
        var data = resp.Data;
        if(data.cls == "0") {
            if(data.code == "401"){
                // console.log("Please Login ... ");
                window.location.href = `https://www.realmcu.com/cn/Account/Login?ReturnUrl=${ReturnUrl}`;
            }
            else if(data.code == "405") {
                // console.log("Sorry, You don't have permission to view documents.");
                window.location.href = `https://www.realmcu.com/cn/Account/MyInfo`;
            }
        }
    }
});