/**
 * @file honeygui_cmd_listener.c
 * @brief HoneyGUI message listener shell commands
 */

#if defined(_WIN32) || defined(__linux__)

#include "shell.h"
#include "gui_api_os.h"
#include "gui_listener.h"
#include <string.h>

/**
 * @brief Test listener callback
 */
static void test_listener_cb(gui_obj_t *obj, const char *topic, void *data, uint16_t len)
{
    gui_log("[Listener] obj=%p, topic='%s', data=%p, len=%u\n", obj, topic, data, len);
    if (data && len > 0)
    {
        gui_log("[Listener] data: %.*s\n", len, (char *)data);
    }
}

/**
 * @brief Subscribe to a topic
 */
static void cmd_subscribe(int argc, char *argv[])
{
    if (argc < 2)
    {
        gui_log("Usage: subscribe <topic>\n");
        return;
    }
    gui_msg_subscribe((gui_obj_t *)0x1234, argv[1], test_listener_cb);
    gui_log("Subscribed to topic: %s\n", argv[1]);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    subscribe, cmd_subscribe, subscribe to topic\r\nsubscribe <topic>);

/**
 * @brief Publish a message
 */
static void cmd_publish(int argc, char *argv[])
{
    if (argc < 2)
    {
        gui_log("Usage: publish <topic> [message]\n");
        return;
    }

    char *msg = (argc > 2) ? argv[2] : "test message";
    gui_msg_publish(argv[1], msg, strlen(msg));
    gui_log("Published to topic: %s\n", argv[1]);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    publish, cmd_publish, publish message to topic\r\npublish <topic> [message]);

/**
 * @brief Unsubscribe from a topic
 */
static void cmd_unsubscribe(int argc, char *argv[])
{
    if (argc < 2)
    {
        gui_log("Usage: unsubscribe <topic>\n");
        return;
    }
    gui_msg_unsubscribe((gui_obj_t *)0x1234, argv[1]);
    gui_log("Unsubscribed from topic: %s\n", argv[1]);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    unsubscribe, cmd_unsubscribe, unsubscribe from topic\r\nunsubscribe <topic>);

#endif
