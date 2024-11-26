#ifndef JERRY_NET_H__
#define JERRY_NET_H__

#ifdef RT_USING_LWIP

#include <rtthread.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jerry_util.h>
#include <jerry_event.h>
#include <jerry_callbacks.h>
#include <jerry_buffer.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>

#define  BUFFER_SIZE 1024
#define SERVER_MAX_CONNECTIONS (RT_LWIP_TCP_PCB_NUM - 3)

struct socket_node
{
    struct socket_node *next;
    struct socket_node *prev;
    jerry_value_t js_socket;
} typedef socket_list_t;

struct socket_info
{
    int socket_id ;

    int pipe_write_fd;
    int pipe_read_fd;
    struct rt_pipe_device *pipe;

    bool allowHalfOpen ;
    bool writable ;
    bool readable ;

    bool isClosing;

    struct js_callback *event_callback;
    struct js_callback *fun_callback;
    struct js_callback *close_callback;

    rt_thread_t readData_thread;
    rt_thread_t connect_thread;

    jerry_value_t js_server;
    jerry_value_t this_value;
    rt_sem_t socket_sem;
    struct timeval timeout;
} typedef net_socoketInfo_t;


struct server_info
{
    int server_id;

    bool allowHalfOpen;

    struct js_callback *event_callback;
    struct js_callback *close_callback;

    rt_thread_t listen_thread;

    socket_list_t *socket_list;
    jerry_value_t this_value;

    rt_sem_t server_sem;
} typedef net_serverInfo_t;

struct read_thread_info
{
    jerry_value_t js_socket;

} typedef net_readInfo_t;

struct connect_info
{
    struct sockaddr_in *socket_fd;
    int socket_id;
    jerry_value_t js_socket;
} typedef net_connectInfo_t;

struct listen_thread_info
{
    jerry_value_t js_server;
    int backlog;
} typedef net_listenInfo_t;

struct write_data_info
{
    jerry_value_t js_data;
    jerry_value_t js_callback;
    bool stop_read;
    rt_sem_t stop_sem;
} typedef net_writeInfo_t;

struct event_callback_info
{
    char *event_name;
    jerry_value_t js_return;
    jerry_value_t js_target;
} typedef net_event_info;

struct fun_callback_info
{
    void *args;
    int args_cnt;
    jerry_value_t js_run;
    jerry_value_t js_this;
} typedef net_funcInfo_t;

struct close_callback_info
{
    jerry_value_t js_server;
    jerry_value_t js_socket;
} typedef net_closeInfo_t;

#endif

#endif
