#include "js_user.h"

#if defined ENABLE_MESH_SWITCH
#include "app_msg.h"
#if (defined RTL87x2G)||(defined RTL8762D)
T_IO_MSG cmd_msg = {.type = IO_MSG_TYPE_UART, .subtype = 0x0d};
#endif
#endif

static void send_mesh_cmd(char *cmd, uint32_t len)
{
#if defined ENABLE_MESH_SWITCH
#if (defined RTL87x2G)||(defined RTL8762D)
    extern bool app_send_msg_to_apptask(T_IO_MSG * p_msg);
    uint32_t i = 0;

    while (i < len)
    {
        cmd_msg.subtype = cmd[i];
        app_send_msg_to_apptask(&cmd_msg);
        i++;
    }

    cmd_msg.subtype = 0x0d;
    app_send_msg_to_apptask(&cmd_msg);
#endif
#endif
}


DECLARE_HANDLER(test)
{
    gui_log("enter test \n");
    char *cmd = "goos ?";

    send_mesh_cmd(cmd, strlen(cmd));
    return jerry_create_undefined();
}

DECLARE_HANDLER(Mesh)
{
    gui_log("enter Mesh\n");
    REGISTER_METHOD(this_value, test);
    return jerry_create_undefined();
}


void js_mesh_init(void)
{
    jerry_value_t global_obj = jerry_get_global_object();
    REGISTER_METHOD(global_obj, Mesh);
}
