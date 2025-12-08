/**
 * @file honeygui_cmd.c
 * @brief HoneyGUI shell commands
 */

#if defined(_WIN32) || defined(__linux__)

#include "shell.h"
#include "gui_api_os.h"
#include "gui_vfs.h"

/**
 * @brief List directory contents with details
 */
static void cmd_vls(int argc, char *argv[])
{
    const char *path = (argc > 1) ? argv[1] : "/";
    gui_vfs_dir_t *dir = gui_vfs_opendir(path);

    if (!dir)
    {
        gui_log("vls: cannot access '%s'\n", path);
        return;
    }

    gui_vfs_stat_t stat;
    gui_log("%-30s %12s %s\n", "Name", "Size", "Type");
    gui_log("--------------------------------------------------------\n");

    while (gui_vfs_readdir(dir, &stat) == 0)
    {
        if (stat.type == GUI_VFS_TYPE_DIR)
        {
            gui_log("\033[1;34m%-30s\033[0m %12s %s\n", stat.name, "-", "DIR");
        }
        else
        {
            gui_log("%-30s %12u %s\n", stat.name, (unsigned int)stat.size, "FILE");
        }
    }

    gui_vfs_closedir(dir);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    vls, cmd_vls, list VFS directory\r\nvls [path]);

/**
 * @brief Display file info
 */
static void cmd_stat(int argc, char *argv[])
{
    if (argc < 2)
    {
        gui_log("Usage: stat <path>\n");
        return;
    }

    gui_vfs_stat_t stat;
    if (gui_vfs_stat(argv[1], &stat) == 0)
    {
        gui_log("File: %s\n", stat.name);
        gui_log("Type: %s\n", stat.type == GUI_VFS_TYPE_DIR ? "Directory" : "File");
        gui_log("Size: %u bytes\n", (unsigned int)stat.size);
    }
    else
    {
        gui_log("stat: cannot access '%s'\n", argv[1]);
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    stat, cmd_stat, display file info\r\nstat <path>);

/**
 * @brief Copy file or directory
 */
static void cmd_cp(int argc, char *argv[])
{
    if (argc < 3)
    {
        gui_log("Usage: cp <source> <dest>\n");
        return;
    }

    gui_vfs_stat_t stat;
    if (gui_vfs_stat(argv[1], &stat) == 0 && stat.type == GUI_VFS_TYPE_DIR)
    {
        if (gui_vfs_copy_dir(argv[1], argv[2]) == 0)
        {
            gui_log("cp: copied directory '%s' to '%s'\n", argv[1], argv[2]);
        }
        else
        {
            gui_log("cp: failed to copy directory '%s'\n", argv[1]);
        }
    }
    else
    {
        if (gui_vfs_copy_file(argv[1], argv[2]) == 0)
        {
            gui_log("cp: copied '%s' to '%s'\n", argv[1], argv[2]);
        }
        else
        {
            gui_log("cp: failed to copy '%s'\n", argv[1]);
        }
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    cp, cmd_cp, copy file or directory\r\ncp <source> <dest>);

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
