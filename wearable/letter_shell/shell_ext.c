/**
 * @file shell_ext.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell extensions
 * @version 3.0.0
 * @date 2019-12-31
 *
 * @copyright (c) 2019 Letter
 *
 */

#include "shell_cfg.h"
#include "shell.h"
#include "shell_ext.h"

extern ShellCommand *shellSeekCommand(Shell *shell,
                                      const char *cmd,
                                      ShellCommand *base,
                                      unsigned short compareLength);
extern int shellGetVarValue(Shell *shell, ShellCommand *command);

/**
 * @brief
 *
 * @param string
 * @return ShellNumType
 */
static ShellNumType shellExtNumType(char *string)
{
    char *p = string;
    ShellNumType type = NUM_TYPE_DEC;

    if ((*p == '0') && ((*(p + 1) == 'x') || (*(p + 1) == 'X')))
    {
        type = NUM_TYPE_HEX;
    }
    else if ((*p == '0') && ((*(p + 1) == 'b') || (*(p + 1) == 'B')))
    {
        type = NUM_TYPE_BIN;
    }
    else if (*p == '0')
    {
        type = NUM_TYPE_OCT;
    }

    while (*p++)
    {
        if (*p == '.' && *(p + 1) != 0)
        {
            type = NUM_TYPE_FLOAT;
            break;
        }
    }

    return type;
}


/**
 * @brief
 *
 * @param code
 * @return char
 */
static char shellExtToNum(char code)
{
    if ((code >= '0') && (code <= '9'))
    {
        return code - '0';
    }
    else if ((code >= 'a') && (code <= 'f'))
    {
        return code - 'a' + 10;
    }
    else if ((code >= 'A') && (code <= 'F'))
    {
        return code - 'A' + 10;
    }
    else
    {
        return 0;
    }
}


/**
 * @brief
 *
 * @param string
 * @return char
 */
static char shellExtParseChar(char *string)
{
    char *p = string + 1;
    char value = 0;

    if (*p == '\\')
    {
        switch (*(p + 1))
        {
        case 'b':
            value = '\b';
            break;
        case 'r':
            value = '\r';
            break;
        case 'n':
            value = '\n';
            break;
        case 't':
            value = '\t';
            break;
        case '0':
            value = 0;
            break;
        default:
            value = *(p + 1);
            break;
        }
    }
    else
    {
        value = *p;
    }
    return value;
}


/**
 * @brief
 *
 * @param string
 * @return char*
 */
static char *shellExtParseString(char *string)
{
    char *p = string;
    unsigned short index = 0;

    if (*string == '\"')
    {
        p = ++string;
    }

    while (*p)
    {
        if (*p == '\\')
        {
            *(string + index) = shellExtParseChar(p - 1);
            p++;
        }
        else if (*p == '\"')
        {
            *(string + index) = 0;
        }
        else
        {
            *(string + index) = *p;
        }
        p++;
        index ++;
    }
    *(string + index) = 0;
    return string;
}


/**
 * @brief
 *
 * @param string
 * @return unsigned int
 */
static unsigned int shellExtParseNumber(char *string)
{
    ShellNumType type = NUM_TYPE_DEC;
    char radix = 10;
    char *p = string;
    char offset = 0;
    signed char sign = 1;
    unsigned int valueInt = 0;
    float valueFloat = 0.0;
    unsigned int devide = 0;

    if (*string == '-')
    {
        sign = -1;
    }

    type = shellExtNumType(string + ((sign == -1) ? 1 : 0));

    switch ((char)type)
    {
    case NUM_TYPE_HEX:
        radix = 16;
        offset = 2;
        break;

    case NUM_TYPE_OCT:
        radix = 8;
        offset = 1;
        break;

    case NUM_TYPE_BIN:
        radix = 2;
        offset = 2;
        break;

    default:
        break;
    }

    p = string + offset + ((sign == -1) ? 1 : 0);

    while (*p)
    {
        if (*p == '.')
        {
            devide = 1;
            p++;
            continue;
        }
        valueInt = valueInt * radix + shellExtToNum(*p);
        devide *= 10;
        p++;
    }
    if (type == NUM_TYPE_FLOAT && devide != 0)
    {
        valueFloat = (float)valueInt / devide * sign;
        //return *(unsigned int *)(&valueFloat); for cpp check
        unsigned int valueUint = (unsigned int)valueFloat;
        return valueUint;
    }
    else
    {
        return valueInt * sign;
    }
}


/**
 * @brief
 *
 * @param shell
 * @param var
 * @return unsigned int
 */
static unsigned int shellExtParseVar(Shell *shell, char *var)
{
    ShellCommand *command = shellSeekCommand(shell,
                                             var + 1,
                                             shell->commandList.base,
                                             0);
    if (command)
    {
        return shellGetVarValue(shell, command);
    }
    else
    {
        return 0;
    }
}


/**
 * @brief
 *
 * @param shell
 * @param string
 * @return unsigned int
 */
unsigned int shellExtParsePara(Shell *shell, char *string)
{
    if (*string == '\'' && *(string + 1))
    {
        return (unsigned int)shellExtParseChar(string);
    }
    else if (*string == '-' || (*string >= '0' && *string <= '9'))
    {
        return (unsigned int)shellExtParseNumber(string);
    }
    else if (*string == '$' && *(string + 1))
    {
        return shellExtParseVar(shell, string);
    }
    else if (*string)
    {
        return (unsigned int)shellExtParseString(string);
    }
    return 0;
}


/**
 * @brief
 *
 * @param shell
 * @param command
 * @param argc
 * @param argv
 * @return int
 */
int shellExtRun(Shell *shell, ShellCommand *command, int argc, char *argv[])
{
    unsigned int params[SHELL_PARAMETER_MAX_NUMBER] = {0};
    int paramNum = command->attr.attrs.paramNum > (argc - 1) ?
                   command->attr.attrs.paramNum : (argc - 1);
    for (int i = 0; i < argc - 1; i++)
    {
        params[i] = shellExtParsePara(shell, argv[i + 1]);
    }
    switch (paramNum)
    {
#if SHELL_PARAMETER_MAX_NUMBER >= 1
    case 0:
        return command->data.cmd.function();
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 1 */
#if SHELL_PARAMETER_MAX_NUMBER >= 2
    case 1:
        return command->data.cmd.function(params[0]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 2 */
#if SHELL_PARAMETER_MAX_NUMBER >= 3
    case 2:
        return command->data.cmd.function(params[0], params[1]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 3 */
#if SHELL_PARAMETER_MAX_NUMBER >= 4
    case 3:
        return command->data.cmd.function(params[0], params[1],
                                          params[2]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 4 */
#if SHELL_PARAMETER_MAX_NUMBER >= 5
    case 4:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 5 */
#if SHELL_PARAMETER_MAX_NUMBER >= 6
    case 5:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 6 */
#if SHELL_PARAMETER_MAX_NUMBER >= 7
    case 6:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4], params[5]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 7 */
#if SHELL_PARAMETER_MAX_NUMBER >= 8
    case 7:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4], params[5],
                                          params[6]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 8 */
#if SHELL_PARAMETER_MAX_NUMBER >= 9
    case 8:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4], params[5],
                                          params[6], params[7]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 9 */
#if SHELL_PARAMETER_MAX_NUMBER >= 10
    case 9:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4], params[5],
                                          params[6], params[7],
                                          params[8]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 10 */
#if SHELL_PARAMETER_MAX_NUMBER >= 11
    case 10:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4], params[5],
                                          params[6], params[7],
                                          params[8], params[9]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 11 */
#if SHELL_PARAMETER_MAX_NUMBER >= 12
    case 11:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4], params[5],
                                          params[6], params[7],
                                          params[8], params[9],
                                          params[10]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 12 */
#if SHELL_PARAMETER_MAX_NUMBER >= 13
    case 12:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4], params[5],
                                          params[6], params[7],
                                          params[8], params[9],
                                          params[10], params[11]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 13 */
#if SHELL_PARAMETER_MAX_NUMBER >= 14
    case 13:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4], params[5],
                                          params[6], params[7],
                                          params[8], params[9],
                                          params[10], params[11],
                                          params[12]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 14 */
#if SHELL_PARAMETER_MAX_NUMBER >= 15
    case 14:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4], params[5],
                                          params[6], params[7],
                                          params[8], params[9],
                                          params[10], params[11],
                                          params[12], params[13]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 15 */
#if SHELL_PARAMETER_MAX_NUMBER >= 16
    case 15:
        return command->data.cmd.function(params[0], params[1],
                                          params[2], params[3],
                                          params[4], params[5],
                                          params[6], params[7],
                                          params[8], params[9],
                                          params[10], params[11],
                                          params[12], params[13],
                                          params[14]);
        // break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 16 */
    default:
        return -1;
        // break;
    }
}
