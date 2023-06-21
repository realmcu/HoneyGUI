#include "jerry_request_init.h"

#ifdef PKG_USING_WEBCLIENT
void request_callback_func(const void *args, uint32_t size)
{
    request_cbinfo_t *cb_info = (request_cbinfo_t *)args;
    if (cb_info->return_value != RT_NULL)
    {
        js_emit_event(cb_info->target_value, cb_info->callback_name, &cb_info->return_value, 1);
    }
    else
    {
        js_emit_event(cb_info->target_value, cb_info->callback_name, RT_NULL, 0);
    }
    if (cb_info->return_value != RT_NULL)
    {
        jerry_release_value(cb_info->return_value);
    }
    if (cb_info->data_value != RT_NULL)
    {
        jerry_release_value(cb_info->data_value);
    }
    if (cb_info->callback_name != RT_NULL)
    {
        free(cb_info->callback_name);
    }
    free(cb_info);
}

void request_callback_free(const void *args, uint32_t size)
{
    request_tdinfo_t *rp = (request_tdinfo_t *)args;
    if (rp->config)
    {
        if (rp->config->session)
        {
            webclient_close(rp->config->session);
        }
        if (rp->config->url)
        {
            free(rp->config->url);
        }
        if (rp->config->data)
        {
            free(rp->config->data);
        }
        free(rp->config);
    }
    js_remove_callback(rp->request_callback);
    js_remove_callback(rp->close_callback);
    js_destroy_emitter(rp->target_value);
    jerry_release_value(rp->target_value);
    free(rp);
}

bool request_combine_header(struct webclient_session *session, char *host, char *user_agent,
                            char *content_type)
{
    bool ret = false;
    if (host != RT_NULL)
    {
        webclient_header_fields_add(session, "Host: %s\r\n", host);
        free(host);
        ret = true;
    }
    if (user_agent != RT_NULL)
    {
        webclient_header_fields_add(session, "User-Agent: %s\r\n", user_agent);
        free(user_agent);
        ret = true;
    }
    if (content_type != RT_NULL)
    {
        webclient_header_fields_add(session, "Content-Type: %s\r\n", content_type);
        free(content_type);
        ret = true;
    }
    return ret;
}

void request_create_header(struct webclient_session *session, jerry_value_t header_value)
{
    int enter_index = 0;
    int colon_index = 0;
    int per_enter_index = -1;
    char header_type[256];
    char header_info[256];

    for (int i = 0 ; i < session->header->length ; i++)
    {
        if (session->header->buffer[i] == ':' && per_enter_index != enter_index)
        {
            colon_index = i;
            per_enter_index = enter_index;
            memset(header_type, 0, 64);
            memset(header_info, 0, 128);
            strncpy(header_type, session->header->buffer + enter_index + 1, colon_index - enter_index - 1);
            strcpy(header_info, session->header->buffer + colon_index + 2);
            jerry_value_t header_info_value = jerry_create_string((jerry_char_t *)header_info);
            js_set_property(header_value, header_type, header_info_value);
            jerry_release_value(header_info_value);
        }
        if (session->header->buffer[i] == '\0')
        {
            enter_index = i;
        }
    }
}

bool request_get_header(struct webclient_session *session, jerry_value_t header_value)
{
    if (jerry_value_is_object(header_value))
    {
        char *host = RT_NULL, *user_agent = RT_NULL, *content_type = RT_NULL;

        jerry_value_t Host_value = js_get_property(header_value, "Host");
        if (!jerry_value_is_undefined(Host_value))
        {
            host = js_value_to_string(Host_value);
        }
        jerry_release_value(Host_value);

        jerry_value_t User_Agent_value = js_get_property(header_value, "User-Agent");
        if (!jerry_value_is_undefined(User_Agent_value))
        {
            user_agent = js_value_to_string(User_Agent_value);
        }
        jerry_release_value(User_Agent_value);

        jerry_value_t Content_Type_value = js_get_property(header_value, "Content-Type");
        if (!jerry_value_is_undefined(Content_Type_value))
        {
            content_type = js_value_to_string(Content_Type_value);
        }
        jerry_release_value(Content_Type_value);

        return request_combine_header(session, host, user_agent, content_type);
    }
    else
    {
        return false;
    }
}

void request_read_entry(void *p)
{
    request_tdinfo_t *rp = (request_tdinfo_t *)p;

    if (rp->config->session != RT_NULL && rp->config->method == WEBCLIENT_GET)
    {
        rp->config->response = webclient_get(rp->config->session, rp->config->url);
    }
    else if (rp->config->session != RT_NULL && rp->config->method == WEBCLIENT_POST)
    {
        webclient_header_fields_add(rp->config->session, "Content-Length: %d\r\n",
                                    strlen(rp->config->data));
        webclient_header_fields_add(rp->config->session, "Content-Type: application/octet-stream\r\n");
        rp->config->response = webclient_post(rp->config->session, rp->config->url, rp->config->data);
    }

    free(rp->config->data);
    rp->config->data = RT_NULL;
    free(rp->config->url);
    rp->config->url = RT_NULL;

    if (rp->config->session == RT_NULL || rp->config->response != 200)
    {
        //do fail callback
        request_cbinfo_t *fail_info  = (request_cbinfo_t *)malloc(sizeof(request_cbinfo_t));
        fail_info->target_value = rp->target_value;
        fail_info->callback_name = rt_strdup("fail");
        fail_info->return_value = RT_NULL;
        fail_info->data_value = RT_NULL;
        js_send_callback(rp->request_callback, fail_info, sizeof(request_cbinfo_t));
        free(fail_info);

        //do complete callback
        request_cbinfo_t *complete_info  = (request_cbinfo_t *)malloc(sizeof(request_cbinfo_t));;
        complete_info->target_value = rp->target_value;
        complete_info->callback_name = rt_strdup("complete");
        complete_info->return_value = RT_NULL;
        complete_info->data_value = RT_NULL;
        js_send_callback(rp->request_callback, complete_info, sizeof(request_cbinfo_t));
        free(complete_info);

        goto __exit;
    }

    unsigned char *buffer = RT_NULL;
    int ret_read = 0;
    int content_length = webclient_content_length_get(rp->config->session);
    int per_read_length = 0;
    buffer = (unsigned char *)malloc(READ_MAX_SIZE + 1);
    if (!buffer)
    {
        rt_kprintf("no more memory to create read buffer\n");
        return;
    }
    memset(buffer, 0, READ_MAX_SIZE + 1);

    if (content_length != -1)
    {
        while (ret_read < content_length)
        {
            per_read_length += webclient_read(rp->config->session, buffer + ret_read,
                                              READ_MAX_SIZE + 1 - ret_read);
            if (per_read_length < 0)
            {
                ret_read = -1;
                break;
            }
            ret_read += per_read_length;
        }
    }
    else    //Transfer-Encoding: chunked
    {
        while (ret_read < READ_MAX_SIZE)
        {
            per_read_length = webclient_read(rp->config->session, buffer + ret_read,
                                             READ_MAX_SIZE + 1 - ret_read);
            if (per_read_length <= 0)
            {
                break;
            }
            ret_read += per_read_length;
        }
    }

    //If file's size is bigger than buffer's,
    //give up reading and send a fail callback
    if (ret_read > READ_MAX_SIZE || ret_read < 0)
    {
        request_cbinfo_t *fail_info = (request_cbinfo_t *)malloc(sizeof(request_cbinfo_t));
        fail_info->target_value = rp->target_value;
        fail_info->callback_name = rt_strdup("fail");
        fail_info->return_value = RT_NULL;
        fail_info->data_value = RT_NULL;
        js_send_callback(rp->request_callback, fail_info, sizeof(request_cbinfo_t));
        free(fail_info);
    }
    else
    {
        jerry_value_t return_value = jerry_create_object();

        /*  create the jerry_value_t of res  */
        js_buffer_t *js_buffer;
        jerry_value_t data_value = jerry_buffer_create(ret_read, &js_buffer);
        if (js_buffer)
        {
            rt_memcpy(js_buffer->buffer, buffer, ret_read);
        }
        js_set_property(return_value, "data", data_value);

        jerry_value_t statusCode_value = jerry_create_number(webclient_resp_status_get(
                                                                 rp->config->session));
        js_set_property(return_value, "statusCode", statusCode_value);
        jerry_release_value(statusCode_value);

        /*** header's data saved as Object ***/
        jerry_value_t header_value = jerry_create_object();
        request_create_header(rp->config->session, header_value);
        js_set_property(return_value, "header", header_value);
        jerry_release_value(header_value);

        //do success callback
        request_cbinfo_t *success_info  = (request_cbinfo_t *)malloc(sizeof(request_cbinfo_t));
        memset(success_info, 0, sizeof(request_cbinfo_t));
        success_info->target_value = rp->target_value;
        success_info->callback_name = rt_strdup("success");
        success_info->return_value = return_value;
        success_info->data_value = data_value;
        js_send_callback(rp->request_callback, success_info, sizeof(request_cbinfo_t));
        free(success_info);
    }
    free(buffer);
    //do complete callback
    request_cbinfo_t *complete_info  = (request_cbinfo_t *)malloc(sizeof(request_cbinfo_t));;
    complete_info->target_value = rp->target_value;
    complete_info->callback_name = rt_strdup("complete");
    complete_info->return_value = RT_NULL;
    complete_info->data_value = RT_NULL;
    js_send_callback(rp->request_callback, complete_info, sizeof(request_cbinfo_t));
    free(complete_info);

__exit:
    //create a callback function to free manager and close webClient session
    js_send_callback(rp->close_callback, rp, sizeof(request_cbinfo_t));
    free(rp);
}

void requeset_add_event_listener(jerry_value_t js_target, jerry_value_t requestObj)
{
    jerry_value_t success_func = js_get_property(requestObj, "success");
    if (jerry_value_is_function(success_func))
    {
        js_add_event_listener(js_target, "success", success_func);
    }
    jerry_release_value(success_func);

    jerry_value_t fail_func = js_get_property(requestObj, "fail");
    if (jerry_value_is_function(fail_func))
    {
        js_add_event_listener(js_target, "fail", fail_func);
    }
    jerry_release_value(fail_func);

    jerry_value_t complete_func = js_get_property(requestObj, "complete");
    if (jerry_value_is_function(complete_func))
    {
        js_add_event_listener(js_target, "complete", complete_func);
    }
    jerry_release_value(complete_func);
}

void reqeuset_get_config(request_config_t *config, jerry_value_t requestObj)
{
    /*get url*/
    jerry_value_t js_url = js_get_property(requestObj, "url");
    if (jerry_value_is_string(js_url))
    {
        config->url = js_value_to_string(js_url);
    }
    jerry_release_value(js_url);

    /*get data*/
    jerry_value_t js_data = js_get_property(requestObj, "data");
    if (jerry_value_is_object(js_data))
    {
        jerry_value_t stringified = jerry_json_stringfy(js_data);
        config->data = js_value_to_string(stringified);
        jerry_release_value(stringified);
    }
    else if (jerry_value_is_string(js_data))
    {
        config->data = js_value_to_string(js_data);
    }
    jerry_release_value(js_data);

    /*get header*/
    config->session = webclient_session_create(HEADER_BUFSZ);;
    jerry_value_t js_header = js_get_property(requestObj, "header");
    request_get_header(config->session, js_header);
    jerry_release_value(js_header);

    /*get method*/
    jerry_value_t method_value = js_get_property(requestObj, "method");
    if (jerry_value_is_string(method_value))
    {
        char *method_str = js_value_to_string(method_value);
        if (method_str)
        {
            if (strcmp(method_str, "POST") == 0)
            {
                config->method = WEBCLIENT_POST;
            }
            free(method_str);
        }
    }
    jerry_release_value(method_value);
}

DECLARE_HANDLER(request)
{
    if (args_cnt != 1 || !jerry_value_is_object(args[0]))
    {
        return jerry_create_undefined();
    }

    jerry_value_t requestObj = args[0];
    jerry_value_t rqObj = jerry_create_object();
    js_make_emitter(rqObj, jerry_create_undefined());
    struct js_callback *request_callback = js_add_callback(request_callback_func);
    struct js_callback *close_callback = js_add_callback(request_callback_free);

    requeset_add_event_listener(rqObj, requestObj);

    request_config_t *config = (request_config_t *)malloc(sizeof(request_config_t));
    config->method = WEBCLIENT_GET;
    config->url = RT_NULL;
    config->data = RT_NULL;
    config->response = 0;
    config->session = RT_NULL;

    reqeuset_get_config(config, requestObj);

    request_tdinfo_t *rp = (request_tdinfo_t *)malloc(sizeof(request_tdinfo_t));
    memset(rp, 0, sizeof(request_tdinfo_t));
    rp->request_callback = request_callback;
    rp->close_callback = close_callback;
    rp->target_value = rqObj;
    rp->config = config;
    rt_thread_t read_request = rt_thread_create("requestRead", request_read_entry, rp, 4096, 20, 5);
    rt_thread_startup(read_request);
    return jerry_acquire_value(rqObj);
}

int jerry_request_init(jerry_value_t obj)
{
    REGISTER_METHOD(obj, request);
    return 0;
}

static jerry_value_t _jerry_request_init()
{
    jerry_value_t js_requset = jerry_create_object();
    REGISTER_METHOD_NAME(js_requset, "request", request);
    return (js_requset);
}

JS_MODULE(http, _jerry_request_init)
#endif
