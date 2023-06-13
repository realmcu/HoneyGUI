//#include "ifaa_log.h"
//#include "quickjs.h"
//#include "cutils.h"
#include "cJSON.h"
#include "alipay_common.h"

static void js_dump_obj(JSContext *ctx, FILE *f, JSValueConst val)
{
    const char *str;
    str = JS_ToCString(ctx, val);
    if (str)
    {
        LOG_ERROR("%s", str);
        JS_FreeCString(ctx, str);
    }
    else
    {
        LOG_ERROR("[exception]");
    }
}
static void js_std_dump_error1(JSContext *ctx, JSValueConst exception_val)
{
    JSValue val;
    bool is_error;

    is_error = JS_IsError(ctx, exception_val);
    js_dump_obj(ctx, stderr, exception_val);
    if (is_error)
    {
        val = JS_GetPropertyStr(ctx, exception_val, "stack");
        if (!JS_IsUndefined(val))
        {
            js_dump_obj(ctx, stderr, val);
        }
        JS_FreeValue(ctx, val);
    }
}
static void js_std_dump_error(JSContext *ctx)
{
    JSValue exception_val;

    exception_val = JS_GetException(ctx);
    js_std_dump_error1(ctx, exception_val);
    JS_FreeValue(ctx, exception_val);
}

static JSRuntime *js_engine_rt;
static JSContext *js_engine_ctx;
static JSValue js_engine_global_obj;
static bool js_engine_hasprepare_flag = true;

extern const JSCFunctionListEntry js_std_console_funcs[1];
extern const JSCFunctionListEntry js_std_dpe_method_router_funcs[1];
extern const JSCFunctionListEntry js_std_re_callback_funcs[2];
bool js_engine_init(void)
{
    js_engine_rt = JS_NewRuntime();
    if (js_engine_rt == NULL)
    {
        LOG_ERROR("JS_NewRuntime failure");
        return false;
    }
    js_engine_ctx = JS_NewContext(js_engine_rt);
    if (js_engine_ctx == NULL)
    {
        JS_FreeRuntime(js_engine_rt);
        LOG_ERROR("JS_NewContext failure");
        return false;
    }

    JSValue console, dpe_method_router, ret_route;
    js_engine_global_obj = JS_GetGlobalObject(js_engine_ctx);

    console = JS_NewObject(js_engine_ctx);
    JS_SetPropertyFunctionList(js_engine_ctx, console, js_std_console_funcs,
                               sizeof(js_std_console_funcs) / sizeof(js_std_console_funcs[0]));
    JS_SetPropertyStr(js_engine_ctx, js_engine_global_obj, "console", console);

    dpe_method_router = JS_NewObject(js_engine_ctx);
    JS_SetPropertyFunctionList(js_engine_ctx, dpe_method_router, js_std_dpe_method_router_funcs,
                               sizeof(js_std_dpe_method_router_funcs) / sizeof(js_std_dpe_method_router_funcs[0]));
    JS_SetPropertyStr(js_engine_ctx, js_engine_global_obj, "dpe_method_router", dpe_method_router);

    ret_route = JS_NewObject(js_engine_ctx);
    JS_SetPropertyFunctionList(js_engine_ctx, ret_route, js_std_re_callback_funcs,
                               sizeof(js_std_re_callback_funcs) / sizeof(js_std_re_callback_funcs[0]));
    JS_SetPropertyStr(js_engine_ctx, js_engine_global_obj, "re_callback", ret_route);

    return true;
}

void js_engine_destroy(void)
{
    JS_FreeValue(js_engine_ctx, js_engine_global_obj);
    JS_FreeContext(js_engine_ctx);
    JS_FreeRuntime(js_engine_rt);
}

bool js_engine_hasPrepared(void)
{
    return js_engine_hasprepare_flag;
}

bool js_engine_eval_script(char *eval_pack, uint32_t len_eval_pack)
{
    JSValue js_retval = JS_Eval(js_engine_ctx, eval_pack, len_eval_pack, "getCode",
                                JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(js_retval))
    {
        js_std_dump_error(js_engine_ctx);
        JS_FreeValue(js_engine_ctx, js_retval);
        return false;
    }

    return true;
}