/**
 * @file shell.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief letter shell
 * @version 3.0.0
 * @date 2019-12-30
 *
 * @copyright (c) 2020 Letter
 *
 */

#ifndef     __SHELL_H__
#define     __SHELL_H__

#include "shell_cfg.h"

#define     SHELL_VERSION               "3.2.3"                 /**< Version number */


/**
 * @brief Shell assert
 *
 * @param expr
 * @param action
 */
#define     SHELL_ASSERT(expr, action) \
    if (!(expr)) { \
        action; \
    }

#if SHELL_USING_LOCK == 1
#define     SHELL_LOCK(shell)           shell->lock(shell)
#define     SHELL_UNLOCK(shell)         shell->unlock(shell)
#else
#define     SHELL_LOCK(shell)
#define     SHELL_UNLOCK(shell)
#endif /** SHELL_USING_LOCK == 1 */
/**
 * @brief Shell command permissions
 *
 * @param permission
 */
#define     SHELL_CMD_PERMISSION(permission) \
    (permission & 0x000000FF)

/**
 * @brief Shell command type
 *
 * @param type
 */
#define     SHELL_CMD_TYPE(type) \
    ((type & 0x0000000F) << 8)

/**
 * @brief Enable command to be used without verifying password
 */
#define     SHELL_CMD_ENABLE_UNCHECKED \
    (1 << 12)

/**
 * @brief Disable return value printing
 */
#define     SHELL_CMD_DISABLE_RETURN \
    (1 << 13)

/**
 * @brief Read only attribute (only effective for variables)
 */
#define     SHELL_CMD_READ_ONLY \
    (1 << 14)

/**
 * @brief Number of command parameters
 */
#define     SHELL_CMD_PARAM_NUM(num) \
    ((num & 0x0000000F)) << 16

#ifndef SHELL_SECTION
#if defined(__CC_ARM) || defined(__CLANG_ARM)
#define SHELL_SECTION(x)                __attribute__((section(x), aligned(1)))
#elif defined (__IAR_SYSTEMS_ICC__)
#define SHELL_SECTION(x)                @ x
#elif defined(__GNUC__)
#define SHELL_SECTION(x)                __attribute__((section(x), aligned(1)))
#else
#define SHELL_SECTION(x)
#endif
#endif

#ifndef SHELL_USED
#if defined(__CC_ARM) || defined(__CLANG_ARM)
#define SHELL_USED                      __attribute__((used))
#elif defined (__IAR_SYSTEMS_ICC__)
#define SHELL_USED                      __root
#elif defined(__GNUC__)
#define SHELL_USED                      __attribute__((used))
#else
#define SHELL_USED
#endif
#endif

/**
 * @brief Float type parameter conversion
 */
#define     SHELL_PARAM_FLOAT(x)            (*(float *)(&x))

/**
 * @brief Agency function name
 */
#define     SHELL_AGENCY_FUNC_NAME(_func)   agency##_func

/**
 * @brief Agency function definition
 *
 * @param _func
 * @param ...
 */
#define     SHELL_AGENCY_FUNC(_func, ...) \
    void SHELL_AGENCY_FUNC_NAME(_func)(int p1, int p2, int p3, int p4, int p5, int p6, int p7) \
    { _func(__VA_ARGS__); }

#if SHELL_USING_CMD_EXPORT == 1

/**
 * @brief Command definition
 *
 * @param _attr
 * @param _name
 * @param _func
 * @param _desc
 * @param ...
 */
#define SHELL_EXPORT_CMD(_attr, _name, _func, _desc, ...) \
    const char shellCmd##_name[] = #_name; \
    const char shellDesc##_name[] = #_desc; \
    SHELL_USED const ShellCommand \
    shellCommand##_name SHELL_SECTION("shellCommand") =  \
                                                         { \
                                                           .attr.value = _attr, \
                                                           .data.cmd.name = shellCmd##_name, \
                                                           .data.cmd.function = (int (*)())_func, \
                                                           .data.cmd.desc = shellDesc##_name, \
                                                           ##__VA_ARGS__ \
                                                         }

#if SHELL_USING_FUNC_SIGNATURE == 1
/**
 * @brief Command definition
 *
 * @param _attr
 * @param _name
 * @param _func
 * @param _desc
 * @param _sign
 */
#define SHELL_EXPORT_CMD_SIGN(_attr, _name, _func, _desc, _sign) \
    const char shellCmd##_name[] = #_name; \
    const char shellDesc##_name[] = #_desc; \
    const char shellSign##_name[] = #_sign; \
    SHELL_USED const ShellCommand \
    shellCommand##_name SHELL_SECTION("shellCommand") =  \
                                                         { \
                                                           .attr.value = _attr, \
                                                           .data.cmd.name = shellCmd##_name, \
                                                           .data.cmd.function = (int (*)())_func, \
                                                           .data.cmd.desc = shellDesc##_name, \
                                                           .data.cmd.signature = shellSign##_name \
                                                         }
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */

/**
 * @brief Agency function definition
 *
 * @param _attr
 * @param _name
 * @param _func
 * @param _desc
 * @param ...
 */
#define SHELL_EXPORT_CMD_AGENCY(_attr, _name, _func, _desc, ...) \
    SHELL_AGENCY_FUNC(_func, ##__VA_ARGS__) \
    SHELL_EXPORT_CMD(_attr, _name, SHELL_AGENCY_FUNC_NAME(_func), _desc)

/**
 * @brief Variable definition
 *
 * @param _attr
 * @param _name
 * @param _value
 * @param _desc
 */
#define SHELL_EXPORT_VAR(_attr, _name, _value, _desc) \
    const char shellCmd##_name[] = #_name; \
    const char shellDesc##_name[] = #_desc; \
    SHELL_USED const ShellCommand \
    shellVar##_name SHELL_SECTION("shellCommand") =  \
                                                     { \
                                                       .attr.value = _attr, \
                                                       .data.var.name = shellCmd##_name, \
                                                       .data.var.value = (void *)_value, \
                                                       .data.var.desc = shellDesc##_name \
                                                     }

/**
 * @brief User definition
 *
 * @param _attr
 * @param _name
 * @param _password
 * @param _desc
 */
#define SHELL_EXPORT_USER(_attr, _name, _password, _desc) \
    const char shellCmd##_name[] = #_name; \
    const char shellPassword##_name[] = #_password; \
    const char shellDesc##_name[] = #_desc; \
    SHELL_USED const ShellCommand \
    shellUser##_name SHELL_SECTION("shellCommand") =  \
                                                      { \
                                                        .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
                                                        .data.user.name = shellCmd##_name, \
                                                        .data.user.password = shellPassword##_name, \
                                                        .data.user.desc = shellDesc##_name \
                                                      }

/**
 * @brief Key definiton
 *
 * @param _attr
 * @param _value
 * @param _func
 * @param _desc
 */
#define SHELL_EXPORT_KEY(_attr, _value, _func, _desc) \
    const char shellDesc##_value[] = #_desc; \
    SHELL_USED const ShellCommand \
    shellKey##_value SHELL_SECTION("shellCommand") =  \
                                                      { \
                                                        .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
                                                        .data.key.value = _value, \
                                                        .data.key.function = (void (*)(Shell *))_func, \
                                                        .data.key.desc = shellDesc##_value \
                                                      }

/**
 * @brief Agency key definition
 *
 * @param _attr
 * @param _value
 * @param _func
 * @param _desc
 * @param ...
 */
#define SHELL_EXPORT_KEY_AGENCY(_attr, _value, _func, _desc, ...) \
    SHELL_AGENCY_FUNC(_func, ##__VA_ARGS__) \
    SHELL_EXPORT_KEY(_attr, _value, SHELL_AGENCY_FUNC_NAME(_func), _desc)

#if SHELL_USING_FUNC_SIGNATURE == 1
/**
 * @brief Parameter Parser Definition
 *
 * @param _attr
 * @param _type
 * @param _parser
 * @param _cleaner
 */
#define SHELL_EXPORT_PARAM_PARSER(_attr, _type, _parser, _cleaner) \
    const char shellDesc##_parser[] = #_type; \
    SHELL_USED const ShellCommand \
    shellCommand##_parser SHELL_SECTION("shellCommand") = \
                                                          { \
                                                            .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_PARAM_PARSER), \
                                                            .data.paramParser.type = shellDesc##_parser, \
                                                            .data.paramParser.parser = (int (*)(char *, void **))_parser, \
                                                            .data.paramParser.cleaner = (int (*)(void *))_cleaner \
                                                          }
#endif

#else
/**
 * @brief Command item definition
 *
 * @param _attr
 * @param _name
 * @param _func
 * @param _desc
 */
#define SHELL_CMD_ITEM(_attr, _name, _func, _desc) \
    { \
        .attr.value = _attr, \
                      .data.cmd.name = #_name, \
                                       .data.cmd.function = (int (*)())_func, \
                                                            .data.cmd.desc = #_desc \
    }

/**
 * @brief Variable item definition
 *
 * @param _attr
 * @param _name
 * @param _value
 * @param _desc
 */
#define SHELL_VAR_ITEM(_attr, _name, _value, _desc) \
    { \
        .attr.value = _attr, \
                      .data.var.name = #_name, \
                                       .data.var.value = (void *)_value, \
                                                         .data.var.desc = #_desc \
    }

/**
 * @brief User item definition
 *
 * @param _attr
 * @param _name
 * @param _password
 * @param _desc
 */
#define SHELL_USER_ITEM(_attr, _name, _password, _desc) \
    { \
        .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
                      .data.user.name = #_name, \
                                        .data.user.password = #_password, \
                                                              .data.user.desc = #_desc \
    }

/**
 * @brief Key item definition
 *
 * @param _attr
 * @param _value
 * @param _func
 * @param _desc
 */
#define SHELL_KEY_ITEM(_type, _attr, _value, _func, _desc) \
    { \
        .attr.attrs.type = _type, \
                           .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
                                         .data.key.value = _value, \
                                                           .data.key.function = (void (*)(Shell *))_func, \
                                                                                .data.key.desc = #_desc \
    }

#if SHELL_USING_FUNC_SIGNATURE == 1
/**
 * @brief Parameter parser item definition
 *
 * @param _attr
 * @param _type
 * @param _parser
 * @param _cleaner
 */
#define SHELL_PARAM_PARSER_ITEM(_attr, _type, _parser, _cleaner) \
    { \
        .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_PARAM_PARSER), \
                      .data.paramParser.type = #_type, \
                                               .data.paramParser.parser = (int (*)(char *, void **))_parser, \
                                                                          .data.paramParser.cleaner = (int (*)(void *))_cleaner \
    }
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */

#define SHELL_EXPORT_CMD(_attr, _name, _func, _desc)
#if SHELL_USING_FUNC_SIGNATURE == 1
#define SHELL_EXPORT_CMD_SIGN(_attr, _name, _func, _desc, _sign)
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */
#define SHELL_EXPORT_CMD_AGENCY(_attr, _name, _func, _desc, ...)
#define SHELL_EXPORT_VAR(_attr, _name, _value, _desc)
#define SHELL_EXPORT_USER(_attr, _name, _password, _desc)
#define SHELL_EXPORT_KEY(_attr, _value, _func, _desc)
#define SHELL_EXPORT_KEY_AGENCY(_attr, _name, _func, _desc, ...)
#if SHELL_USING_FUNC_SIGNATURE == 1
#define SHELL_EXPORT_PARAM_PARSER(_attr, _type, _parser, _cleaner)
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */
#endif /** SHELL_USING_CMD_EXPORT == 1 */

/**
 * @brief Shell command type
 */
typedef enum
{
    SHELL_TYPE_CMD_MAIN = 0,                                    /**< Main form command */
    SHELL_TYPE_CMD_FUNC,                                        /**< C function formal command */
    SHELL_TYPE_VAR_INT,                                         /**< Int type variable */
    SHELL_TYPE_VAR_SHORT,                                       /**< Short type variable */
    SHELL_TYPE_VAR_CHAR,                                        /**< Char type variable */
    SHELL_TYPE_VAR_STRING,                                      /**< String type variable */
    SHELL_TYPE_VAR_POINT,                                       /**< Pointer type variable */
    SHELL_TYPE_VAR_NODE,                                        /**< Node variables */
    SHELL_TYPE_USER,                                            /**< User */
    SHELL_TYPE_KEY,                                             /**< Key */
#if SHELL_USING_FUNC_SIGNATURE == 1
    SHELL_TYPE_PARAM_PARSER,                                    /**< Parameter parser */
#endif
} ShellCommandType;


/**
 * @brief Shell definition
 */
typedef struct shell_def
{
    struct
    {
        const struct shell_command *user;                       /**< Current user */
        int activeTime;                                         /**< Shell activation time */
        char *path;                                             /**< Current shell path */
#if SHELL_USING_COMPANION == 1
        struct shell_companion_object *companions;              /**< Companion Object  */
#endif
#if SHELL_KEEP_RETURN_VALUE == 1
        int retVal;                                             /**< Return value */
#endif
    } info;
    struct
    {
        unsigned short length;                                  /**< Input data length */
        unsigned short cursor;                                  /**< Current cursor position */
        char *buffer;                                           /**< Input buffering */
        char *param[SHELL_PARAMETER_MAX_NUMBER];                /**< parameter */
        unsigned short bufferSize;                              /**< Input buffer size */
        unsigned short paramCount;                              /**< Number of parameters */
        int keyValue;                                           /**< Input key values */
    } parser;
#if SHELL_HISTORY_MAX_NUMBER > 0
    struct
    {
        char *item[SHELL_HISTORY_MAX_NUMBER];                   /**< History */
        unsigned short number;                                  /**< Number of historical records */
        unsigned short record;                                  /**< Current record location */
        signed short offset;                                    /**< Current history offset */
    } history;
#endif /** SHELL_HISTORY_MAX_NUMBER > 0 */
    struct
    {
        void *base;                                             /**< Command table base address */
        unsigned short count;                                   /**< Number of commands */
    } commandList;
    struct
    {
        unsigned char isChecked : 1;                            /**< Password verification passed */
        unsigned char isActive : 1;                             /**< Current activity shell */
        unsigned char tabFlag : 1;                              /**< Tab flag */
    } status;
    signed short(*read)(char *, unsigned short);                /**< Shell read function */
    signed short(*write)(char *, unsigned short);               /**< Shell write function */
#if SHELL_USING_LOCK == 1
    int (*lock)(struct shell_def *);                              /**< Shell locking */
    int (*unlock)(struct shell_def *);                            /**< Shell unlocking */
#endif
} Shell;


/**
 * @brief Shell command definition
 */
typedef struct shell_command
{
#pragma pack(1)
    union
    {
        struct
        {
            unsigned char permission : 8;                       /**< Command permission */
            ShellCommandType type : 4;                          /**< Command type */
            unsigned char enableUnchecked : 1;                  /**< Available without verifying password */
            unsigned char disableReturn : 1;                    /**< Disable return value output */
            unsigned char readOnly : 1;                         /**< Read-only */
            unsigned char reserve : 1;                          /**< Reserve */
            unsigned char paramNum : 4;                         /**< Number of parameters */
        } attrs;
        int value;
    } attr;                                                     /**< Attribute */
#pragma pack()
    union
    {
        struct
        {
            const char *name;                                   /**< Command name */
            int (*function)();                                  /**< Command execution function */
            const char *desc;                                   /**< Command description */
#if SHELL_USING_FUNC_SIGNATURE == 1
            const char *signature;                              /**< Function signature */
#endif
        } cmd;                                                  /**< Command definition */
        struct
        {
            const char *name;                                   /**< Variable name */
            void *value;                                        /**< Variable value */
            const char *desc;                                   /**< Variable description */
        } var;                                                  /**< Variable definition */
        struct
        {
            const char *name;                                   /**< User name */
            const char *password;                               /**< User password */
            const char *desc;                                   /**< User description */
        } user;                                                 /**< User definition */
        struct
        {
            int value;                                          /**< Key value */
            void (*function)(Shell *);                          /**< Key execution function */
            const char *desc;                                   /**< Key description */
        } key;                                                  /**< Key definition */
#if SHELL_USING_FUNC_SIGNATURE == 1
        struct
        {
            const char *type;                                   /**< Parameter type */
            int (*parser)(char *, void **);                     /**< Parser function */
            int (*cleaner)(void *);                             /**< Cleaner */
        } paramParser;                                          /**< Parameter Parser */
#endif
    } data;
} ShellCommand;

/**
 * @brief Node variable attributes
 */
typedef struct
{
    void *var;                                                  /**< Variable reference */
    int (*get)();                                               /**< Variable get method */
    int (*set)();                                               /**< Variable set method */
} ShellNodeVarAttr;


#define shellSetPath(_shell, _path)     (_shell)->info.path = _path
#define shellGetPath(_shell)            ((_shell)->info.path)

#define shellDeInit(shell)              shellRemove(shell)

void shellInit(Shell *shell, char *buffer, unsigned short size);
void shellRemove(Shell *shell);
unsigned short shellWriteString(Shell *shell, const char *string);
void shellPrint(Shell *shell, const char *fmt, ...);
void shellScan(Shell *shell, char *fmt, ...);
Shell *shellGetCurrent(void);
void shellHandler(Shell *shell, char data);
void shellWriteEndLine(Shell *shell, char *buffer, int len);
void shellTask(void *param);
int shellRun(Shell *shell, const char *cmd);



#if SHELL_USING_COMPANION == 1
/**
 * @brief Definition of accompanying objects
 */
typedef struct shell_companion_object
{
    int id;                                                     /**< Accompanying object ID */
    void *obj;                                                  /**< Companion Object  */
    struct shell_companion_object *next;                        /**< Next companion object */
} ShellCompanionObj;


signed char shellCompanionAdd(Shell *shell, int id, void *object);
signed char shellCompanionDel(Shell *shell, int id);
void *shellCompanionGet(Shell *shell, int id);
#endif

#endif
