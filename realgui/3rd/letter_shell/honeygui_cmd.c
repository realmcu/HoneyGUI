/**
 * @file honeygui_cmd.c
 * @brief HoneyGUI shell commands
 */

#if defined(_WIN32) || defined(__linux__)

#include "shell.h"
#include "gui_api_os.h"

/**
 * @brief Display HoneyGUI version
 */
static void cmd_version(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    gui_log("\033[1;32mHoneyGUI Framework\033[0m\n");
    gui_log("Platform: %s\n",
#ifdef _WIN32
            "Windows"
#else
            "Linux"
#endif
           );
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    version, cmd_version, show HoneyGUI version);

#endif
