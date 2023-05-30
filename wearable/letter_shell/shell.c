/**
 * @file shell.c
 * @author Letter (NevermindZZT@gmail.com)
 * @version 3.0.0
 * @date 2019-12-30
 *
 * @copyright (c) 2020 Letter
 *
 */

#include "shell.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "shell_ext.h"


#if SHELL_USING_CMD_EXPORT == 1
/**
 * @brief
 */
const char shellCmdDefaultUser[] = SHELL_DEFAULT_USER;
const char shellPasswordDefaultUser[] = SHELL_DEFAULT_USER_PASSWORD;
const char shellDesDefaultUser[] = "default user";
SHELL_USED const ShellCommand shellUserDefault SHELL_SECTION("shellCommand") =
{
    .attr.value = SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_USER),
    .data.user.name = shellCmdDefaultUser,
    .data.user.password = shellPasswordDefaultUser,
    .data.user.desc = shellDesDefaultUser
};
#endif

#if SHELL_USING_CMD_EXPORT == 1
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
extern const unsigned int shellCommand$$Base;
extern const unsigned int shellCommand$$Limit;
#elif defined(__ICCARM__) || defined(__ICCRX__)
#pragma section="shellCommand"
#elif defined(__GNUC__)
extern const unsigned int _shell_command_start;
extern const unsigned int _shell_command_end;
#endif
#else
extern const ShellCommand shellCommandList[];
extern const unsigned short shellCommandCount;
#endif
extern Shell shell_user;

/**
 * @brief shell
 */
enum
{
#if SHELL_SHOW_INFO == 1
    SHELL_TEXT_INFO,
#endif
    SHELL_TEXT_CMD_TOO_LONG,
    SHELL_TEXT_CMD_LIST,
    SHELL_TEXT_VAR_LIST,
    SHELL_TEXT_USER_LIST,
    SHELL_TEXT_KEY_LIST,
    SHELL_TEXT_CMD_NOT_FOUND,
    SHELL_TEXT_POINT_CANNOT_MODIFY,
    SHELL_TEXT_VAR_READ_ONLY_CANNOT_MODIFY,
    SHELL_TEXT_NOT_VAR,
    SHELL_TEXT_VAR_NOT_FOUND,
    SHELL_TEXT_HELP_HEADER,
    SHELL_TEXT_PASSWORD_HINT,
    SHELL_TEXT_PASSWORD_ERROR,
    SHELL_TEXT_CLEAR_CONSOLE,
    SHELL_TEXT_CLEAR_LINE,
    SHELL_TEXT_TYPE_CMD,
    SHELL_TEXT_TYPE_VAR,
    SHELL_TEXT_TYPE_USER,
    SHELL_TEXT_TYPE_KEY,
    SHELL_TEXT_TYPE_NONE,
#if SHELL_EXEC_UNDEF_FUNC == 1
    SHELL_TEXT_PARAM_ERROR,
#endif
};


static const char *shellText[] =
{
#if SHELL_SHOW_INFO == 1
    [SHELL_TEXT_INFO] =
    "\r\n"
    " _         _   _                  _          _ _ \r\n"
    "| |    ___| |_| |_ ___ _ __   ___| |__   ___| | |\r\n"
    "| |   / _ \\ __| __/ _ \\ '__| / __| '_ \\ / _ \\ | |\r\n"
    "| |__|  __/ |_| ||  __/ |    \\__ \\ | | |  __/ | |\r\n"
    "|_____\\___|\\__|\\__\\___|_|    |___/_| |_|\\___|_|_|\r\n"
    "\r\n"
    "Build:       "__DATE__" "__TIME__"\r\n"
    "Version:     "SHELL_VERSION"\r\n"
    "Copyright:   (c) 2020 Letter\r\n",
#endif
    [SHELL_TEXT_CMD_TOO_LONG] =
    "\r\nWarning: Command is too long\r\n",
    [SHELL_TEXT_CMD_LIST] =
    "\r\nCommand List:\r\n",
    [SHELL_TEXT_VAR_LIST] =
    "\r\nVar List:\r\n",
    [SHELL_TEXT_USER_LIST] =
    "\r\nUser List:\r\n",
    [SHELL_TEXT_KEY_LIST] =
    "\r\nKey List:\r\n",
    [SHELL_TEXT_CMD_NOT_FOUND] =
    "Command not Found\r\n",
    [SHELL_TEXT_POINT_CANNOT_MODIFY] =
    "can't set pointer\r\n",
    [SHELL_TEXT_VAR_READ_ONLY_CANNOT_MODIFY] =
    "can't set read only var\r\n",
    [SHELL_TEXT_NOT_VAR] =
    " is not a var\r\n",
    [SHELL_TEXT_VAR_NOT_FOUND] =
    "Var not Fount\r\n",
    [SHELL_TEXT_HELP_HEADER] =
    "command help of ",
    [SHELL_TEXT_PASSWORD_HINT] =
    "\r\nPlease input password:",
    [SHELL_TEXT_PASSWORD_ERROR] =
    "\r\npassword error\r\n",
    [SHELL_TEXT_CLEAR_CONSOLE] =
    "\033[2J\033[1H",
    [SHELL_TEXT_CLEAR_LINE] =
    "\033[2K\r",
    [SHELL_TEXT_TYPE_CMD] =
    "CMD ",
    [SHELL_TEXT_TYPE_VAR] =
    "VAR ",
    [SHELL_TEXT_TYPE_USER] =
    "USER",
    [SHELL_TEXT_TYPE_KEY] =
    "KEY ",
    [SHELL_TEXT_TYPE_NONE] =
    "NONE",
#if SHELL_EXEC_UNDEF_FUNC == 1
    [SHELL_TEXT_PARAM_ERROR] =
    "Parameter error\r\n",
#endif
};


/**
 * @brief
 */
static Shell *shellList[SHELL_MAX_NUMBER] = {NULL};


static void shellAdd(Shell *shell);
static void shellWritePrompt(Shell *shell, unsigned char newline);
static void shellWriteReturnValue(Shell *shell, int value);
static int shellShowVar(Shell *shell, ShellCommand *command);
void shellSetUser(Shell *shell, const ShellCommand *user);
ShellCommand *shellSeekCommand(Shell *shell,
                               const char *cmd,
                               ShellCommand *base,
                               unsigned short compareLength);
static void shellWriteCommandHelp(Shell *shell, char *cmd);

/**
 * @brief shell
 *
 * @param shell
 */
void shellInit(Shell *shell, char *buffer, unsigned short size)
{
    shell->parser.length = 0;
    shell->parser.cursor = 0;
    shell->info.user = NULL;
    shell->status.isChecked = 1;

    shell->parser.buffer = buffer;
    shell->parser.bufferSize = size / (SHELL_HISTORY_MAX_NUMBER + 1);

#if SHELL_HISTORY_MAX_NUMBER > 0
    shell->history.offset = 0;
    shell->history.number = 0;
    shell->history.record = 0;
    for (short i = 0; i < SHELL_HISTORY_MAX_NUMBER; i++)
    {
        shell->history.item[i] = buffer + shell->parser.bufferSize * (i + 1);
    }
#endif /** SHELL_HISTORY_MAX_NUMBER > 0 */

#if SHELL_USING_CMD_EXPORT == 1
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
    shell->commandList.base = (ShellCommand *)(&shellCommand$$Base);
    shell->commandList.count = ((unsigned int)(&shellCommand$$Limit)
                                - (unsigned int)(&shellCommand$$Base))
                               / sizeof(ShellCommand);

#elif defined(__ICCARM__) || defined(__ICCRX__)
    shell->commandList.base = (ShellCommand *)(__section_begin("shellCommand"));
    shell->commandList.count = ((unsigned int)(__section_end("shellCommand"))
                                - (unsigned int)(__section_begin("shellCommand")))
                               / sizeof(ShellCommand);
#elif defined(__GNUC__)
    shell->commandList.base = (ShellCommand *)(&_shell_command_start);
    shell->commandList.count = ((unsigned int)(&_shell_command_end)
                                - (unsigned int)(&_shell_command_start))
                               / sizeof(ShellCommand);
#else
#error not supported compiler, please use command table mode
#endif
#else
    shell->commandList.base = (ShellCommand *)shellCommandList;
    shell->commandList.count = shellCommandCount;
#endif

    shellAdd(shell);

    shellSetUser(shell, shellSeekCommand(shell,
                                         SHELL_DEFAULT_USER,
                                         shell->commandList.base,
                                         0));
    shellWritePrompt(shell, 1);
}


/**
 * @brief
 *
 * @param shell
 */
static void shellAdd(Shell *shell)
{
    for (short i = 0; i < SHELL_MAX_NUMBER; i++)
    {
        if (shellList[i] == NULL)
        {
            shellList[i] = shell;
            return;
        }
    }
}

/**
 * @brief
 *
 * @param shell
 *
 */
void shellRemove(Shell *shell)
{
    for (short i = 0; i < SHELL_MAX_NUMBER; i++)
    {
        if (shellList[i] == shell)
        {
            shellList[i] = NULL;
            return;
        }
    }
}

/**
 * @brief

 * @return Shell*
 */
Shell *shellGetCurrent(void)
{
    for (short i = 0; i < SHELL_MAX_NUMBER; i++)
    {
        if (shellList[i] && shellList[i]->status.isActive)
        {
            return shellList[i];
        }
    }
    return NULL;
}


/**
 * @brief
 *
 * @param shell
 * @param data
 */
static void shellWriteByte(Shell *shell, char data)
{
    shell->write(&data, 1);
}


/**
 * @brief shell
 *
 * @param shell
 * @param string
 *
 * @return unsigned short
 */
unsigned short shellWriteString(Shell *shell, const char *string)
{
    unsigned short count = 0;
    const char *p = string;
    SHELL_ASSERT(shell->write, return 0);
    while (*p++)
    {
        count ++;
    }
    return shell->write((char *)string, count);
}


/**
 * @brief shell
 *
 * @param shell
 * @param string
 *
 * @return unsigned short
 */
static unsigned short shellWriteCommandDesc(Shell *shell, const char *string)
{
    unsigned short count = 0;
    const char *p = string;
    SHELL_ASSERT(shell->write, return 0);
    while (*p && *p != '\r' && *p != '\n')
    {
        p++;
        count++;
    }

    if (count > 36)
    {
        shell->write((char *)string, 36);
        shell->write("...", 3);
    }
    else
    {
        shell->write((char *)string, count);
    }
    return count > 36 ? 36 : 39;
}


/**
 * @brief
 *
 * @param shell
 * @param newline
 *
 */
static void shellWritePrompt(Shell *shell, unsigned char newline)
{
    if (shell->status.isChecked)
    {
        if (newline)
        {
            shellWriteString(shell, "\r\n");
        }
        shellWriteString(shell, shell->info.user->data.user.name);
        shellWriteString(shell, ":");
        shellWriteString(shell, shell->info.path ? shell->info.path : "/");
        shellWriteString(shell, "$ ");
    }
    else
    {
        shellWriteString(shell, shellText[SHELL_TEXT_PASSWORD_HINT]);
    }
}


#if SHELL_PRINT_BUFFER > 0
/**
 * @brief
 *
 * @param shell
 * @param fmt
 * @param ...
 */
void shellPrint(char *fmt, ...)
{
    char buffer[SHELL_PRINT_BUFFER];
    va_list vargs;

    SHELL_ASSERT(&shell_user, return);

    va_start(vargs, fmt);
    vsnprintf(buffer, SHELL_PRINT_BUFFER - 1, fmt, vargs);
    va_end(vargs);

    shellWriteString(&shell_user, buffer);
}
#endif


#if SHELL_SCAN_BUFFER > 0
/**
 * @brief
 *
 * @param shell
 * @param fmt
 * @param ...
 */
void shellScan(Shell *shell, char *fmt, ...)
{
    char buffer[SHELL_SCAN_BUFFER];
    va_list vargs;
    short index = 0;

    SHELL_ASSERT(shell, return);

    if (shell->read)
    {
        do
        {
            if (shell->read(&buffer[index], 1) == 1)
            {
                shell->write(&buffer[index], 1);
                index++;
            }
        }
        while (buffer[index - 1] != '\r' && buffer[index - 1] != '\n' && index < SHELL_SCAN_BUFFER);
        shellWriteString(shell, "\r\n");
        buffer[index] = '\0';
    }

    va_start(vargs, fmt);
    vsscanf(buffer, fmt, vargs);
    va_end(vargs);
}
#endif


/**
 * @brief shell
 *
 * @param shell
 * @param command ShellCommand
 *
 * @return signed char 0
 * @return signec char -1
 */
signed char shellCheckPermission(Shell *shell, ShellCommand *command)
{
    return ((!command->attr.attrs.permission
             || command->attr.attrs.type == SHELL_TYPE_USER
             || (command->attr.attrs.permission
                 & shell->info.user->attr.attrs.permission))
            && (shell->status.isChecked
                || command->attr.attrs.enableUnchecked))
           ? 0 : -1;
}


/**
 * @brief
 *
 * @param value
 * @param buffer
 *
 * @return signed char
 */
signed char shellToHex(unsigned int value, char *buffer)
{
    char byte;
    unsigned char i = 8;
    buffer[8] = 0;
    while (value)
    {
        byte = value & 0x0000000F;
        buffer[--i] = (byte > 9) ? (byte + 87) : (byte + 48);
        value >>= 4;
    }
    return 8 - i;
}


/**
* @brief
 *
 * @param value
 * @param buffer
 *
 * @return signed char
 */
signed char shellToDec(int value, char *buffer)
{
    unsigned char i = 11;
    int v = value;
    if (value < 0)
    {
        v = -value;
    }
    buffer[11] = 0;
    while (v)
    {
        buffer[--i] = v % 10 + 48;
        v /= 10;
    }
    if (value < 0)
    {
        buffer[--i] = '-';
    }
    if (value == 0)
    {
        buffer[--i] = '0';
    }
    return 11 - i;
}


/**
 * @brief
 *
 * @param dest
 * @param src
 * @return unsigned short
 */
static unsigned short shellStringCopy(char *dest, char *src)
{
    unsigned short count = 0;
    while (*(src + count))
    {
        *(dest + count) = *(src + count);
        count++;
    }
    *(dest + count) = 0;
    return count;
}


/**
 * @brief
 *
 * @param dest
 * @param src
 * @return unsigned short
 */
static unsigned short shellStringCompare(char *dest, char *src)
{
    unsigned short match = 0;
    unsigned short i = 0;

    while (*(dest + i) && *(src + i))
    {
        if (*(dest + i) != *(src + i))
        {
            break;
        }
        match ++;
        i++;
    }
    return match;
}


/**
 * @brief
 *
 * @param command
 * @return const char*
 */
static const char *shellGetCommandName(ShellCommand *command)
{
    static char buffer[9];
    for (unsigned char i = 0; i < 9; i++)
    {
        buffer[i] = '0';
    }
    if (command->attr.attrs.type <= SHELL_TYPE_CMD_FUNC)
    {
        return command->data.cmd.name;
    }
    else if (command->attr.attrs.type <= SHELL_TYPE_VAR_NODE)
    {
        return command->data.var.name;
    }
    else if (command->attr.attrs.type <= SHELL_TYPE_USER)
    {
        return command->data.user.name;
    }
    else
    {
        shellToHex(command->data.key.value, buffer);
        return buffer;
    }
}


/**
 * @brief
 *
 * @param command
 * @return const char*
 */
static const char *shellGetCommandDesc(ShellCommand *command)
{
    if (command->attr.attrs.type <= SHELL_TYPE_CMD_FUNC)
    {
        return command->data.cmd.desc;
    }
    else if (command->attr.attrs.type <= SHELL_TYPE_VAR_NODE)
    {
        return command->data.var.desc;
    }
    else if (command->attr.attrs.type <= SHELL_TYPE_USER)
    {
        return command->data.user.desc;
    }
    else
    {
        return command->data.key.desc;
    }
}

/**
 * @brief shell
 *
 * @param shell
 * @param item
 */
void shellListItem(Shell *shell, ShellCommand *item)
{
    short spaceLength;

    spaceLength = 22 - shellWriteString(shell, shellGetCommandName(item));
    spaceLength = (spaceLength > 0) ? spaceLength : 4;
    do
    {
        shellWriteByte(shell, ' ');
    }
    while (--spaceLength);
    if (item->attr.attrs.type <= SHELL_TYPE_CMD_FUNC)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_TYPE_CMD]);
    }
    else if (item->attr.attrs.type <= SHELL_TYPE_VAR_NODE)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_TYPE_VAR]);
    }
    else if (item->attr.attrs.type <= SHELL_TYPE_USER)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_TYPE_USER]);
    }
    else if (item->attr.attrs.type <= SHELL_TYPE_KEY)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_TYPE_KEY]);
    }
    else
    {
        shellWriteString(shell, shellText[SHELL_TEXT_TYPE_NONE]);
    }
#if SHELL_HELP_SHOW_PERMISSION == 1
    shellWriteString(shell, "  ");
    for (signed char i = 7; i >= 0; i--)
    {
        shellWriteByte(shell, item->attr.attrs.permission & (1 << i) ? 'x' : '-');
    }
#endif
    shellWriteString(shell, "  ");
    shellWriteCommandDesc(shell, shellGetCommandDesc(item));
    shellWriteString(shell, "\r\n");
}


/**
 * @brief
 *
 * @param shell
 */
void shellListCommand(Shell *shell)
{
    ShellCommand *base = (ShellCommand *)shell->commandList.base;
    shellWriteString(shell, shellText[SHELL_TEXT_CMD_LIST]);
    for (short i = 0; i < shell->commandList.count; i++)
    {
        if (base[i].attr.attrs.type <= SHELL_TYPE_CMD_FUNC
            && shellCheckPermission(shell, &base[i]) == 0)
        {
            shellListItem(shell, &base[i]);
        }
    }
}


/**
 * @brief
 *
 * @param shell
 */
void shellListVar(Shell *shell)
{
    ShellCommand *base = (ShellCommand *)shell->commandList.base;
    shellWriteString(shell, shellText[SHELL_TEXT_VAR_LIST]);
    for (short i = 0; i < shell->commandList.count; i++)
    {
        if (base[i].attr.attrs.type > SHELL_TYPE_CMD_FUNC
            && base[i].attr.attrs.type <= SHELL_TYPE_VAR_NODE
            && shellCheckPermission(shell, &base[i]) == 0)
        {
            shellListItem(shell, &base[i]);
        }
    }
}


/**
 * @brief
 *
 * @param shell
 */
void shellListUser(Shell *shell)
{
    ShellCommand *base = (ShellCommand *)shell->commandList.base;
    shellWriteString(shell, shellText[SHELL_TEXT_USER_LIST]);
    for (short i = 0; i < shell->commandList.count; i++)
    {
        if (base[i].attr.attrs.type > SHELL_TYPE_VAR_NODE
            && base[i].attr.attrs.type <= SHELL_TYPE_USER
            && shellCheckPermission(shell, &base[i]) == 0)
        {
            shellListItem(shell, &base[i]);
        }
    }
}


/**
 * @brief
 *
 * @param shell
 */
void shellListKey(Shell *shell)
{
    ShellCommand *base = (ShellCommand *)shell->commandList.base;
    shellWriteString(shell, shellText[SHELL_TEXT_KEY_LIST]);
    for (short i = 0; i < shell->commandList.count; i++)
    {
        if (base[i].attr.attrs.type > SHELL_TYPE_USER
            && base[i].attr.attrs.type <= SHELL_TYPE_KEY
            && shellCheckPermission(shell, &base[i]) == 0)
        {
            shellListItem(shell, &base[i]);
        }
    }
}


/**
 * @brief
 *
 * @param shell
 */
void shellListAll(Shell *shell)
{
#if SHELL_HELP_LIST_USER == 1
    shellListUser(shell);
#endif
    shellListCommand(shell);
#if SHELL_HELP_LIST_VAR == 1
    shellListVar(shell);
#endif
#if SHELL_HELP_LIST_KEY == 1
    shellListKey(shell);
#endif
}


/**
 * @brief
 *
 * @param shell
 * @param length
 */
void shellDeleteCommandLine(Shell *shell, unsigned char length)
{
    while (length--)
    {
        shellWriteString(shell, "\b \b");
    }
}


/**
 * @brief shell
 *
 * @param shell
 */
void shellClearCommandLine(Shell *shell)
{
    for (short i = shell->parser.length - shell->parser.cursor; i > 0; i--)
    {
        shellWriteByte(shell, ' ');
    }
    shellDeleteCommandLine(shell, shell->parser.length);
}


/**
 * @brief
 *
 * @param shell
 * @param data
 */
void shellInsertByte(Shell *shell, char data)
{
    /*  */
    if (shell->parser.length >= shell->parser.bufferSize - 1)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_CMD_TOO_LONG]);
        shellWritePrompt(shell, 1);
        shellWriteString(shell, shell->parser.buffer);
        return;
    }

    /*  */
    if (shell->parser.cursor == shell->parser.length)
    {
        shell->parser.buffer[shell->parser.length++] = data;
        shell->parser.buffer[shell->parser.length] = 0;
        shell->parser.cursor++;
        shellWriteByte(shell, data);
    }
    else if (shell->parser.cursor < shell->parser.length)
    {
        for (short i = shell->parser.length - shell->parser.cursor; i > 0; i--)
        {
            shell->parser.buffer[shell->parser.cursor + i] =
                shell->parser.buffer[shell->parser.cursor + i - 1];
        }
        shell->parser.buffer[shell->parser.cursor++] = data;
        shell->parser.buffer[++shell->parser.length] = 0;
        for (short i = shell->parser.cursor - 1; i < shell->parser.length; i++)
        {
            shellWriteByte(shell, shell->parser.buffer[i]);
        }
        for (short i = shell->parser.length - shell->parser.cursor; i > 0; i--)
        {
            shellWriteByte(shell, '\b');
        }
    }
}


/**
 * @brief shell
 *
 * @param shell
 * @param direction
 */
void shellDeleteByte(Shell *shell, signed char direction)
{
    char offset = (direction == -1) ? 1 : 0;

    if ((shell->parser.cursor == 0 && direction == 1)
        || (shell->parser.cursor == shell->parser.length && direction == -1))
    {
        return;
    }
    if (shell->parser.cursor == shell->parser.length && direction == 1)
    {
        shell->parser.cursor--;
        shell->parser.length--;
        shell->parser.buffer[shell->parser.length] = 0;
        shellDeleteCommandLine(shell, 1);
    }
    else
    {
        for (short i = offset; i < shell->parser.length - shell->parser.cursor; i++)
        {
            shell->parser.buffer[shell->parser.cursor + i - 1] =
                shell->parser.buffer[shell->parser.cursor + i];
        }
        shell->parser.length--;
        if (!offset)
        {
            shell->parser.cursor--;
            shellWriteByte(shell, '\b');
        }
        shell->parser.buffer[shell->parser.length] = 0;
        for (short i = shell->parser.cursor; i < shell->parser.length; i++)
        {
            shellWriteByte(shell, shell->parser.buffer[i]);
        }
        shellWriteByte(shell, ' ');
        for (short i = shell->parser.length - shell->parser.cursor + 1; i > 0; i--)
        {
            shellWriteByte(shell, '\b');
        }
    }
}


/**
 * @brief shell
 *
 * @param shell
 */
static void shellParserParam(Shell *shell)
{
    unsigned char quotes = 0;
    unsigned char record = 1;

    for (short i = 0; i < SHELL_PARAMETER_MAX_NUMBER; i++)
    {
        shell->parser.param[i] = NULL;
    }

    shell->parser.paramCount = 0;
    for (unsigned short i = 0; i < shell->parser.length; i++)
    {
        if (quotes != 0
            || (shell->parser.buffer[i] != ' '
                && shell->parser.buffer[i] != 0))
        {
            if (shell->parser.buffer[i] == '\"')
            {
                quotes = quotes ? 0 : 1;
            }
            if (record == 1)
            {
                if (shell->parser.paramCount < SHELL_PARAMETER_MAX_NUMBER)
                {
                    shell->parser.param[shell->parser.paramCount++] =
                        &(shell->parser.buffer[i]);
                }
                record = 0;
            }
            if (shell->parser.buffer[i] == '\\'
                && shell->parser.buffer[i + 1] != 0)
            {
                i++;
            }
        }
        else
        {
            shell->parser.buffer[i] = 0;
            record = 1;
        }
    }
}


/**
 * @brief
 *
 * @param shell
 */
static void shellRemoveParamQuotes(Shell *shell)
{
    unsigned short paramLength;
    for (unsigned short i = 0; i < shell->parser.paramCount; i++)
    {
        if (shell->parser.param[i][0] == '\"')
        {
            shell->parser.param[i][0] = 0;
            shell->parser.param[i] = &shell->parser.param[i][1];
        }
        paramLength = strlen(shell->parser.param[i]);
        if (shell->parser.param[i][paramLength - 1] == '\"')
        {
            shell->parser.param[i][paramLength - 1] = 0;
        }
    }
}


/**
 * @brief
 *
 * @param shell
 * @param cmd
 * @param base
 * @param compareLength
 * @return ShellCommand*
 */
ShellCommand *shellSeekCommand(Shell *shell,
                               const char *cmd,
                               ShellCommand *base,
                               unsigned short compareLength)
{
    const char *name;
    unsigned short count = shell->commandList.count -
                           ((int)base - (int)shell->commandList.base) / sizeof(ShellCommand);
    for (unsigned short i = 0; i < count; i++)
    {
        if (base[i].attr.attrs.type == SHELL_TYPE_KEY
            || shellCheckPermission(shell, &base[i]) != 0)
        {
            continue;
        }
        name = shellGetCommandName(&base[i]);
        if (!compareLength)
        {
            if (strcmp(cmd, name) == 0)
            {
                return &base[i];
            }
        }
        else
        {
            if (strncmp(cmd, name, compareLength) == 0)
            {
                return &base[i];
            }
        }
    }
    return NULL;
}


/**
 * @brief shell
 *
 * @param shell
 * @param command
 * @return int
 */
int shellGetVarValue(Shell *shell, ShellCommand *command)
{
    int value = 0;
    switch (command->attr.attrs.type)
    {
    case SHELL_TYPE_VAR_INT:
        value = *((int *)(command->data.var.value));
        break;
    case SHELL_TYPE_VAR_SHORT:
        value = *((short *)(command->data.var.value));
        break;
    case SHELL_TYPE_VAR_CHAR:
        value = *((char *)(command->data.var.value));
        break;
    case SHELL_TYPE_VAR_STRING:
    case SHELL_TYPE_VAR_POINT:
        value = (int)(command->data.var.value);
        break;
    case SHELL_TYPE_VAR_NODE:
        value = ((ShellNodeVarAttr *)command->data.var.value)->get ?
                ((ShellNodeVarAttr *)command->data.var.value)
                ->get(((ShellNodeVarAttr *)command->data.var.value)->var) : 0;
        break;
    default:
        break;
    }
    return value;
}


/**
 * @brief
 *
 * @param shell
 * @param command
 * @param value
 * @return int
 */
int shellSetVarValue(Shell *shell, ShellCommand *command, int value)
{
    if (command->attr.attrs.readOnly)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_VAR_READ_ONLY_CANNOT_MODIFY]);
    }
    else
    {
        switch (command->attr.attrs.type)
        {
        case SHELL_TYPE_VAR_INT:
            *((int *)(command->data.var.value)) = value;
            break;
        case SHELL_TYPE_VAR_SHORT:
            *((short *)(command->data.var.value)) = value;
            break;
        case SHELL_TYPE_VAR_CHAR:
            *((char *)(command->data.var.value)) = value;
            break;
        case SHELL_TYPE_VAR_STRING:
            shellStringCopy(((char *)(command->data.var.value)), (char *)value);
            break;
        case SHELL_TYPE_VAR_POINT:
            shellWriteString(shell, shellText[SHELL_TEXT_POINT_CANNOT_MODIFY]);
            break;
        case SHELL_TYPE_VAR_NODE:
            if (((ShellNodeVarAttr *)command->data.var.value)->set)
            {
                if (((ShellNodeVarAttr *)command->data.var.value)->var)
                {
                    ((ShellNodeVarAttr *)command->data.var.value)
                    ->set(((ShellNodeVarAttr *)command->data.var.value)->var, value);
                }
                else
                {
                    ((ShellNodeVarAttr *)command->data.var.value)->set(value);
                }
            }
            break;
        default:
            break;
        }
    }
    return shellShowVar(shell, command);
}


/**
 * @brief
 *
 * @param shell
 * @param command
 * @return int
 */
static int shellShowVar(Shell *shell, ShellCommand *command)
{
    char buffer[12] = "00000000000";
    int value = shellGetVarValue(shell, command);

    shellWriteString(shell, command->data.var.name);
    shellWriteString(shell, " = ");

    switch (command->attr.attrs.type)
    {
    case SHELL_TYPE_VAR_STRING:
        shellWriteString(shell, "\"");
        shellWriteString(shell, (char *)value);
        shellWriteString(shell, "\"");
        break;
    // case SHELL_TYPE_VAR_INT:
    // case SHELL_TYPE_VAR_SHORT:
    // case SHELL_TYPE_VAR_CHAR:
    // case SHELL_TYPE_VAR_POINT:
    default:
        shellWriteString(shell, &buffer[11 - shellToDec(value, buffer)]);
        shellWriteString(shell, ", 0x");
        for (short i = 0; i < 11; i++)
        {
            buffer[i] = '0';
        }
        shellToHex(value, buffer);
        shellWriteString(shell, buffer);
        break;
    }

    shellWriteString(shell, "\r\n");
    return value;
}


/**
 * @brief
 *
 * @param name
 * @param value
 * @return int
 */
int shellSetVar(char *name, int value)
{
    Shell *shell = shellGetCurrent();
    if (shell == NULL)
    {
        return 0;
    }
    ShellCommand *command = shellSeekCommand(shell,
                                             name,
                                             shell->commandList.base,
                                             0);
    if (!command)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_VAR_NOT_FOUND]);
        return 0;
    }
    if (command->attr.attrs.type < SHELL_TYPE_VAR_INT
        || command->attr.attrs.type > SHELL_TYPE_VAR_NODE)
    {
        shellWriteString(shell, name);
        shellWriteString(shell, shellText[SHELL_TEXT_NOT_VAR]);
        return 0;
    }
    return shellSetVarValue(shell, command, value);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    setVar, shellSetVar, set var);


/**
 * @brief
 *
 * @param shell
 * @param command
 *
 * @return unsigned int
 */
unsigned int shellRunCommand(Shell *shell, ShellCommand *command)
{
    int returnValue = 0;
    shell->status.isActive = 1;
    if (command->attr.attrs.type == SHELL_TYPE_CMD_MAIN)
    {
        shellRemoveParamQuotes(shell);
        returnValue = command->data.cmd.function(shell->parser.paramCount,
                                                 shell->parser.param);
        if (!command->attr.attrs.disableReturn)
        {
            shellWriteReturnValue(shell, returnValue);
        }
    }
    else if (command->attr.attrs.type == SHELL_TYPE_CMD_FUNC)
    {
        returnValue = shellExtRun(shell,
                                  command,
                                  shell->parser.paramCount,
                                  shell->parser.param);
        if (!command->attr.attrs.disableReturn)
        {
            shellWriteReturnValue(shell, returnValue);
        }
    }
    else if (command->attr.attrs.type >= SHELL_TYPE_VAR_INT
             && command->attr.attrs.type <= SHELL_TYPE_VAR_NODE)
    {
        shellShowVar(shell, command);
    }
    else if (command->attr.attrs.type == SHELL_TYPE_USER)
    {
        shellSetUser(shell, command);
    }
    shell->status.isActive = 0;

    return returnValue;
}


/**
 * @brief
 *
 * @param shell
 */
static void shellCheckPassword(Shell *shell)
{
    if (strcmp(shell->parser.buffer, shell->info.user->data.user.password) == 0)
    {
        shell->status.isChecked = 1;
#if SHELL_SHOW_INFO == 1
        shellWriteString(shell, shellText[SHELL_TEXT_INFO]);
#endif
    }
    else
    {
        shellWriteString(shell, shellText[SHELL_TEXT_PASSWORD_ERROR]);
    }
    shell->parser.length = 0;
    shell->parser.cursor = 0;
}


/**
 * @brief
 *
 * @param shell
 * @param user
 */
void shellSetUser(Shell *shell, const ShellCommand *user)
{
    shell->info.user = user;
    shell->status.isChecked =
        ((user->data.user.password && strlen(user->data.user.password) != 0)
         && (shell->parser.paramCount < 2
             || strcmp(user->data.user.password, shell->parser.param[1]) != 0))
        ? 0 : 1;

#if SHELL_CLS_WHEN_LOGIN == 1
    shellWriteString(shell, shellText[SHELL_TEXT_CLEAR_CONSOLE]);
#endif
#if SHELL_SHOW_INFO == 1
    if (shell->status.isChecked)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_INFO]);
    }
#endif
}


/**
 * @brief
 *
 * @param shell
 * @param value
 */
static void shellWriteReturnValue(Shell *shell, int value)
{
    char buffer[12] = "00000000000";
    shellWriteString(shell, "Return: ");
    shellWriteString(shell, &buffer[11 - shellToDec(value, buffer)]);
    shellWriteString(shell, ", 0x");
    for (short i = 0; i < 11; i++)
    {
        buffer[i] = '0';
    }
    shellToHex(value, buffer);
    shellWriteString(shell, buffer);
    shellWriteString(shell, "\r\n");
#if SHELL_KEEP_RETURN_VALUE == 1
    shell->info.retVal = value;
#endif
}


#if SHELL_HISTORY_MAX_NUMBER > 0
/**
 * @brief
 *
 * @param shell
 */
static void shellHistoryAdd(Shell *shell)
{
    shell->history.offset = 0;
    if (shell->history.number > 0
        && strcmp(shell->history.item[(shell->history.record == 0 ?
                                       SHELL_HISTORY_MAX_NUMBER : shell->history.record) - 1],
                  shell->parser.buffer) == 0)
    {
        return;
    }
    if (shellStringCopy(shell->history.item[shell->history.record],
                        shell->parser.buffer) != 0)
    {
        shell->history.record++;
    }
    if (++shell->history.number > SHELL_HISTORY_MAX_NUMBER)
    {
        shell->history.number = SHELL_HISTORY_MAX_NUMBER;
    }
    if (shell->history.record >= SHELL_HISTORY_MAX_NUMBER)
    {
        shell->history.record = 0;
    }
}


/**
 * @brief
 *
 * @param shell
 * @param dir
 */
static void shellHistory(Shell *shell, signed char dir)
{
    if (dir > 0)
    {
        if (shell->history.offset-- <=
            -((shell->history.number > shell->history.record) ?
              shell->history.number : shell->history.record))
        {
            shell->history.offset = -((shell->history.number > shell->history.record)
                                      ? shell->history.number : shell->history.record);
        }
    }
    else if (dir < 0)
    {
        if (++shell->history.offset > 0)
        {
            shell->history.offset = 0;
            return;
        }
    }
    else
    {
        return;
    }
    shellClearCommandLine(shell);
    if (shell->history.offset == 0)
    {
        shell->parser.cursor = shell->parser.length = 0;
    }
    else
    {
        if ((shell->parser.length = shellStringCopy(shell->parser.buffer,
                                                    shell->history.item[(shell->history.record + SHELL_HISTORY_MAX_NUMBER
                                                                         + shell->history.offset) % SHELL_HISTORY_MAX_NUMBER])) == 0)
        {
            return;
        }
        shell->parser.cursor = shell->parser.length;
        shellWriteString(shell, shell->parser.buffer);
    }

}
#endif /** SHELL_HISTORY_MAX_NUMBER > 0 */


/**
 * @brief shell
 *
 * @param shell
 * @param data
 */
void shellNormalInput(Shell *shell, char data)
{
    shell->status.tabFlag = 0;
    shellInsertByte(shell, data);
}


/**
 * @brief
 *
 * @param shell
 */
void shellExec(Shell *shell)
{

    if (shell->parser.length == 0)
    {
        return;
    }

    shell->parser.buffer[shell->parser.length] = 0;

    if (shell->status.isChecked)
    {
#if SHELL_HISTORY_MAX_NUMBER > 0
        shellHistoryAdd(shell);
#endif /** SHELL_HISTORY_MAX_NUMBER > 0 */
        shellParserParam(shell);
        shell->parser.length = shell->parser.cursor = 0;
        if (shell->parser.paramCount == 0)
        {
            return;
        }
        shellWriteString(shell, "\r\n");

        ShellCommand *command = shellSeekCommand(shell,
                                                 shell->parser.param[0],
                                                 shell->commandList.base,
                                                 0);
        if (command != NULL)
        {
            shellRunCommand(shell, command);
        }
        else
        {
            shellWriteString(shell, shellText[SHELL_TEXT_CMD_NOT_FOUND]);
        }
    }
    else
    {
        shellCheckPassword(shell);
    }
}


#if SHELL_HISTORY_MAX_NUMBER > 0
/**
 * @brief
 *
 * @param shell
 */
void shellUp(Shell *shell)
{
    shellHistory(shell, 1);
}
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0), 0x1B5B4100, shellUp, up);


/**
 * @brief
 *
 * @param shell
 */
void shellDown(Shell *shell)
{
    shellHistory(shell, -1);
}
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0), 0x1B5B4200, shellDown, down);
#endif /** SHELL_HISTORY_MAX_NUMBER > 0 */


/**
 * @brief
 *
 * @param shell
 */
void shellRight(Shell *shell)
{
    if (shell->parser.cursor < shell->parser.length)
    {
        shellWriteByte(shell, shell->parser.buffer[shell->parser.cursor++]);
    }
}
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0) | SHELL_CMD_ENABLE_UNCHECKED,
                 0x1B5B4300, shellRight, right);


/**
 * @brief
 *
 * @param shell
 */
void shellLeft(Shell *shell)
{
    if (shell->parser.cursor > 0)
    {
        shellWriteByte(shell, '\b');
        shell->parser.cursor--;
    }
}
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0) | SHELL_CMD_ENABLE_UNCHECKED,
                 0x1B5B4400, shellLeft, left);


/**
 * @brief shell
 *
 * @param shell
 */
void shellTab(Shell *shell)
{
    unsigned short maxMatch = shell->parser.bufferSize;
    unsigned short lastMatchIndex = 0;
    unsigned short matchNum = 0;
    unsigned short length;

    if (shell->parser.length == 0)
    {
        shellListAll(shell);
        shellWritePrompt(shell, 1);
    }
    else if (shell->parser.length > 0)
    {
        shell->parser.buffer[shell->parser.length] = 0;
        ShellCommand *base = (ShellCommand *)shell->commandList.base;
        for (short i = 0; i < shell->commandList.count; i++)
        {
            if (shellCheckPermission(shell, &base[i]) == 0
                && shellStringCompare(shell->parser.buffer,
                                      (char *)shellGetCommandName(&base[i]))
                == shell->parser.length)
            {
                if (matchNum != 0)
                {
                    if (matchNum == 1)
                    {
                        shellWriteString(shell, "\r\n");
                    }
                    shellListItem(shell, &base[lastMatchIndex]);
                    length =
                        shellStringCompare((char *)shellGetCommandName(&base[lastMatchIndex]),
                                           (char *)shellGetCommandName(&base[i]));
                    maxMatch = (maxMatch > length) ? length : maxMatch;
                }
                lastMatchIndex = i;
                matchNum++;
            }
        }
        if (matchNum == 0)
        {
            return;
        }
        if (matchNum == 1)
        {
            shellClearCommandLine(shell);
        }
        if (matchNum != 0)
        {
            shell->parser.length =
                shellStringCopy(shell->parser.buffer,
                                (char *)shellGetCommandName(&base[lastMatchIndex]));
        }
        if (matchNum > 1)
        {
            shellListItem(shell, &base[lastMatchIndex]);
            shellWritePrompt(shell, 1);
            shell->parser.length = maxMatch;
        }
        shell->parser.buffer[shell->parser.length] = 0;
        shell->parser.cursor = shell->parser.length;
        shellWriteString(shell, shell->parser.buffer);
    }

    if (SHELL_GET_TICK())
    {
        if (matchNum == 1
            && shell->status.tabFlag
            && SHELL_GET_TICK() - shell->info.activeTime < SHELL_DOUBLE_CLICK_TIME)
        {
#if SHELL_QUICK_HELP == 1
            shellWriteString(shell, "\r\n");
            shellWriteCommandHelp(shell, shell->parser.buffer);
            shellWritePrompt(shell, 1);
            shellWriteString(shell, shell->parser.buffer);
#else
            shellClearCommandLine(shell);
            for (short i = shell->parser.length; i >= 0; i--)
            {
                shell->parser.buffer[i + 5] = shell->parser.buffer[i];
            }
            shellStringCopy(shell->parser.buffer, "help");
            shell->parser.buffer[4] = ' ';
            shell->parser.length += 5;
            shell->parser.cursor = shell->parser.length;
            shellWriteString(shell, shell->parser.buffer);
#endif
        }
        else
        {
            shell->status.tabFlag = 1;
        }
    }
}
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0), 0x09000000, shellTab, tab);


/**
 * @brief shell
 *
 * @param shell
 */
void shellBackspace(Shell *shell)
{
    shellDeleteByte(shell, 1);
}
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0) | SHELL_CMD_ENABLE_UNCHECKED,
                 0x08000000, shellBackspace, backspace);
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0) | SHELL_CMD_ENABLE_UNCHECKED,
                 0x7F000000, shellBackspace, backspace);


/**
 * @brief shell
 *
 * @param shell
 */
void shellDelete(Shell *shell)
{
    shellDeleteByte(shell, -1);
}
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0) | SHELL_CMD_ENABLE_UNCHECKED,
                 0x1B5B337E, shellDelete, delete);


/**
 * @brief shell
 *
 * @param shell
 */
void shellEnter(Shell *shell)
{
    shellExec(shell);
    shellWritePrompt(shell, 1);
}
#if SHELL_ENTER_LF == 1
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0) | SHELL_CMD_ENABLE_UNCHECKED,
                 0x0A000000, shellEnter, enter);
#endif
#if SHELL_ENTER_CR == 1
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0) | SHELL_CMD_ENABLE_UNCHECKED,
                 0x0D000000, shellEnter, enter);
#endif
#if SHELL_ENTER_CRLF == 1
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0) | SHELL_CMD_ENABLE_UNCHECKED,
                 0x0D0A0000, shellEnter, enter);
#endif

/**
 * @brief shell
 *
 * @param shell
 * @param cmd
 */
static void shellWriteCommandHelp(Shell *shell, char *cmd)
{
    ShellCommand *command = shellSeekCommand(shell,
                                             cmd,
                                             shell->commandList.base,
                                             0);
    if (command)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_HELP_HEADER]);
        shellWriteString(shell, shellGetCommandName(command));
        shellWriteString(shell, "\r\n");
        shellWriteString(shell, shellGetCommandDesc(command));
        shellWriteString(shell, "\r\n");
    }
    else
    {
        shellWriteString(shell, shellText[SHELL_TEXT_CMD_NOT_FOUND]);
    }
}

/**
 * @brief shell help
 *
 * @param argc
 * @param argv
 */
void shellHelp(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    SHELL_ASSERT(shell, return);
    if (argc == 1)
    {
        shellListAll(shell);
    }
    else if (argc > 1)
    {
        shellWriteCommandHelp(shell, argv[1]);
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
    help, shellHelp, show command info\r\nhelp [cmd]);

/**
 * @brief shell
 *
 * @param shell
 * @param data
 */
void shellHandler(Shell *shell, char data)
{
    SHELL_ASSERT(data, return);
    SHELL_LOCK(shell);

#if SHELL_LOCK_TIMEOUT > 0
    if (shell->info.user->data.user.password
        && strlen(shell->info.user->data.user.password) != 0
        && SHELL_GET_TICK())
    {
        if (SHELL_GET_TICK() - shell->info.activeTime > SHELL_LOCK_TIMEOUT)
        {
            shell->status.isChecked = 0;
        }
    }
#endif

    /*  */
    char keyByteOffset = 24;
    int keyFilter = 0x00000000;
    if ((shell->parser.keyValue & 0x0000FF00) != 0x00000000)
    {
        keyByteOffset = 0;
        keyFilter = 0xFFFFFF00;
    }
    else if ((shell->parser.keyValue & 0x00FF0000) != 0x00000000)
    {
        keyByteOffset = 8;
        keyFilter = 0xFFFF0000;
    }
    else if ((shell->parser.keyValue & 0xFF000000) != 0x00000000)
    {
        keyByteOffset = 16;
        keyFilter = 0xFF000000;
    }

    /*  */
    ShellCommand *base = (ShellCommand *)shell->commandList.base;
    for (short i = 0; i < shell->commandList.count; i++)
    {
        /*  */
        if (base[i].attr.attrs.type == SHELL_TYPE_KEY
            && shellCheckPermission(shell, &(base[i])) == 0)
        {
            /*  */
            if ((base[i].data.key.value & keyFilter) == shell->parser.keyValue
                && (base[i].data.key.value & (0xFF << keyByteOffset))
                == (data << keyByteOffset))
            {
                shell->parser.keyValue |= data << keyByteOffset;
                data = 0x00;
                if (keyByteOffset == 0
                    || (base[i].data.key.value & (0xFF << (keyByteOffset - 8)))
                    == 0x00000000)
                {
                    if (base[i].data.key.function)
                    {
                        base[i].data.key.function(shell);
                    }
                    shell->parser.keyValue = 0x00000000;
                    break;
                }
            }
        }
    }

    if (data != 0x00)
    {
        shell->parser.keyValue = 0x00000000;
        shellNormalInput(shell, data);
    }

    if (SHELL_GET_TICK())
    {
        shell->info.activeTime = SHELL_GET_TICK();
    }
    SHELL_UNLOCK(shell);
}


#if SHELL_SUPPORT_END_LINE == 1
void shellWriteEndLine(Shell *shell, char *buffer, int len)
{
    SHELL_LOCK(shell);
    if (!shell->status.isActive)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_CLEAR_LINE]);
    }
    shell->write(buffer, len);

    if (!shell->status.isActive)
    {
        shellWritePrompt(shell, 0);
        if (shell->parser.length > 0)
        {
            shellWriteString(shell, shell->parser.buffer);
            for (short i = 0; i < shell->parser.length - shell->parser.cursor; i++)
            {
                shellWriteByte(shell, '\b');
            }
        }
    }
    SHELL_UNLOCK(shell);
}
#endif /** SHELL_SUPPORT_END_LINE == 1 */

/**
 * @brief shell
 *
 * @param param ()
 *
 */
void shellTask(void *param)
{
    Shell *shell = (Shell *)param;
    char data;
#if SHELL_TASK_WHILE == 1
    while (1)
    {
#endif
        if (shell->read && shell->read(&data, 1) == 1)
        {
            shellHandler(shell, data);
        }
#if SHELL_TASK_WHILE == 1
    }
#endif
}


/**
 * @brief shell ()
 */
void shellUsers(void)
{
    Shell *shell = shellGetCurrent();
    if (shell)
    {
        shellListUser(shell);
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    users, shellUsers, list all user);


/**
 * @brief shell ()
 */
void shellCmds(void)
{
    Shell *shell = shellGetCurrent();
    if (shell)
    {
        shellListCommand(shell);
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    cmds, shellCmds, list all cmd);


/**
 * @brief shell ()
 */
void shellVars(void)
{
    Shell *shell = shellGetCurrent();
    if (shell)
    {
        shellListVar(shell);
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    vars, shellVars, list all var);


/**
 * @brief shell ()
 */
void shellKeys(void)
{
    Shell *shell = shellGetCurrent();
    if (shell)
    {
        shellListKey(shell);
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    keys, shellKeys, list all key);


/**
 * @brief shell ()
 */
void shellClear(void)
{
    Shell *shell = shellGetCurrent();
    if (shell)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_CLEAR_CONSOLE]);
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    clear, shellClear, clear console);


/**
 * @brief
 *
 * @param shell
 * @param cmd
 * @return int
 */
int shellRun(Shell *shell, const char *cmd)
{
    SHELL_ASSERT(shell && cmd, return -1);
    char active = shell->status.isActive;
    if (strlen(cmd) > shell->parser.bufferSize - 1)
    {
        shellWriteString(shell, shellText[SHELL_TEXT_CMD_TOO_LONG]);
        return -1;
    }
    else
    {
        shell->parser.length = shellStringCopy(shell->parser.buffer, (char *)cmd);
        shellExec(shell);
        shell->status.isActive = active;
        return 0;
    }
}
//extern Shell shell;
//void func(int i, char ch, char *str)
//{
//    shellPrint(&shell, "input int: %d, char: %c, string: %s\r\n", i, ch, str);
//}
//SHELL_EXPORT_CMD(
//    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
//    io, func, test);


#if SHELL_EXEC_UNDEF_FUNC == 1
/**
 * @brief
 *
 * @param argc
 * @param argv
 * @return int
 */
int shellExecute(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    if (shell && argc >= 2)
    {
        int (*func)() = (int (*)())shellExtParsePara(shell, argv[1]);
        ShellCommand command =
        {
            .attr.value = SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)
            | SHELL_CMD_DISABLE_RETURN,
            .data.cmd.function = func,
        };
        return shellExtRun(shell, &command, argc - 1, &argv[1]);
    }
    else
    {
        shellWriteString(shell, shellText[SHELL_TEXT_PARAM_ERROR]);
        return -1;
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
    exec, shellExecute, execute function undefined);
#endif

#if SHELL_KEEP_RETURN_VALUE == 1
/**
 * @brief
 *
 *
 * @return int
 */
static int shellRetValGet()
{
    Shell *shell = shellGetCurrent();
    return shell ? shell->info.retVal : 0;
}
static ShellNodeVarAttr shellRetVal =
{
    .get = shellRetValGet
};
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_VAR_NODE) |
                 SHELL_CMD_READ_ONLY,
                 RETVAL, &shellRetVal, return value of last command);
#endif /** SHELL_KEEP_RETURN_VALUE == 1 */
