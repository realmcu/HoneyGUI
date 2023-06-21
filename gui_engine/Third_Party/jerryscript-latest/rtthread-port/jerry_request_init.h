#ifndef JERRY_REQUEST_INIT_H__
#define JERRY_REQUEST_INIT_H__

#include <rtthread.h>

#ifdef PKG_USING_WEBCLIENT

#include <jerry_util.h>
#include <jerry_event.h>
#include <jerry_callbacks.h>
#include <webclient.h>
#include <jerry_buffer.h>

#define READ_MAX_SIZE 50*1024
#define HEADER_BUFSZ 1024

struct request_callback_info
{
    jerry_value_t target_value;
    jerry_value_t return_value;
    jerry_value_t data_value;
    char *callback_name;
} typedef request_cbinfo_t;

struct request_config_info
{
    char *url;
    char *data;
    struct webclient_session *session;
    int method;
    int response;
} typedef request_config_t;

struct request_thread_info
{
    jerry_value_t target_value;
    request_config_t *config;
    struct js_callback *request_callback;
    struct js_callback *close_callback;
} typedef request_tdinfo_t;

int jerry_request_init(jerry_value_t obj);

#endif

#endif