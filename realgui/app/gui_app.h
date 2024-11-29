/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_app.h
  * @brief app api
  * @details application startup and close
  * @author howie_wang@realsil.com.cn
  * @date
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_APP_H__
#define __GUI_APP_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_fb.h>
#include "gui_obj.h"
#include "gui_win.h"
/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  APP structure */
typedef struct gui_app gui_app_t;
struct gui_app
{
    gui_obj_t screen;               //!< widget tree root
    const char *xml;                //!< widget tree design file
    gui_win_t *window;      //!< app transition window
    uint32_t active_ms;             //!< screen shut down delay
    uint32_t start_ms;             //!< screen shut down delay
    void *thread_id;                //!< thread handle(optional)
    void (* thread_entry)(void *_this); //!< thread entry
    void (* ctor)(void *_this);      //!< constructor
    void (* dtor)(void *_this);      //!< destructor
    void (* ui_design)(gui_app_t *); //!< ui create entry
    bool lvgl;
    bool arm2d;
    bool close;
    bool next;
    bool close_sync;
    unsigned char startup_animation_flag;
    unsigned char shutdown_animation_flag;
};

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

/**
 * @brief Macro to define a GUI application.
 *
 * This macro creates a new GUI application by defining a static function pointer to the UI design function,
 * initializing the GUI application structure, and providing a way to retrieve a handle to this application.
 *
 * @param APP_NAME The name of the application.
 * @param UI_DESIGN The function to design the UI of the application.
 */
#define GUI_APP_DEFINE(APP_NAME, UI_DESIGN) \
    static void UI_DESIGN(gui_app_t*); \
    static gui_app_t _app_##APP_NAME = \
                                       { \
                                         .screen = \
                                                   { \
                                                     .name = #APP_NAME, /**< The screen name is set to the application name. */ \
                                                     .type = SCREEN,    /**< The screen type is initialized to SCREEN. */ \
                                                   }, \
                                         .ui_design = UI_DESIGN,   /**< The UI design function is assigned. */ \
                                         .active_ms = 1000000,     /**< The active duration is set to 1,000,000 milliseconds. */ \
                                       }; \
    /**
     * @brief Function to get the handle of the application.
     *
     * @return A pointer to the application instance.
     */ \
    gui_app_t *_get_app_##APP_NAME##_handle(void) \
    { \
        return &_app_##APP_NAME; \
    }

/**
 * @brief Macro to get the handle of a GUI application by its name.
 *
 * @param APP_NAME The name of the application.
 * @return A pointer to the application instance.
 */
#define GUI_APP_HANDLE(APP_NAME) _get_app_##APP_NAME##_handle()

/**
 * @brief Macro to shut down a GUI application.
 *
 * This macro shuts down the application by calling the external function gui_app_shutdown
 * with the application's handle.
 *
 * @param APP_NAME The name of the application.
 */
#define GUI_APP_SHUTDOWN(APP_NAME) extern gui_app_t *_get_app_##APP_NAME##_handle(void); gui_app_shutdown(_get_app_##APP_NAME##_handle());

/**
 * @brief Macro to start up a GUI application.
 *
 * This macro starts up the application by calling the external function gui_app_startup
 * with the application's handle.
 *
 * @param APP_NAME The name of the application.
 */
#define GUI_APP_STARTUP(APP_NAME) extern gui_app_t *_get_app_##APP_NAME##_handle(void); gui_app_startup(_get_app_##APP_NAME##_handle());

/**
 * @brief Macro to swap between two GUI applications.
 *
 * This macro allows switching from one application to another by calling gui_switch_app
 * and passing the handles of the current and next application.
 *
 * @param APP_NAME The name of the current application.
 * @param APP_NAME_NEXT The name of the next application.
 */
#define GUI_APP_SWAP(APP_NAME, APP_NAME_NEXT) gui_switch_app(_get_app_##APP_NAME##_handle(), _get_app_##APP_NAME##_handle_next());

/**
 * @brief Macro to get a pointer to the root screen of the current application.
 *
 * @return A pointer to the root screen of the current application.
 */
#define GUI_APP_ROOT_SCREEN ((void *)(app->window))

/**
 * @brief Macro to swap between two GUI applications using their handle functions.
 *
 * This macro swaps between two applications using their external handle functions.
 *
 * @param HANDLE_FUNC The handle function of the current application.
 * @param HANDLE_NEXT_FUNC The handle function of the next application.
 */
#define GUI_APP_SWAP_HANDLE(HANDLE_FUNC, HANDLE_NEXT_FUNC) \
    extern gui_app_t *HANDLE_FUNC; \
    extern gui_app_t *HANDLE_NEXT_FUNC; \
    gui_switch_app(HANDLE_FUNC, HANDLE_NEXT_FUNC);

/**
 * @brief Macro to define a GUI application with a specific name.
 *
 * This macro works similarly to GUI_APP_DEFINE but with a naming convention for the UI design function.
 *
 * @param APP_NAME The name of the application.
 */
#define GUI_APP_DEFINE_NAME(APP_NAME) \
    static void _##APP_NAME##_ui_design(gui_app_t*); \
    static gui_app_t _app_##APP_NAME = \
                                       { \
                                         .screen = \
                                                   { \
                                                     .name = #APP_NAME, /**< The screen name is set to the application name. */ \
                                                     .magic = GUI_MAGIC_NUMBER, /**< check number. */ \
                                                   }, \
                                         .ui_design = _##APP_NAME##_ui_design, /**< The UI design function is assigned with the modified name. */ \
                                         .active_ms = 1000000, /**< The active duration is set to 1,000,000 milliseconds. */ \
                                       }; \
    /**
     * @brief Function to get the handle of the application.
     *
     * @return A pointer to the application instance.
     */ \
    gui_app_t *_get_app_##APP_NAME##_handle(void) \
    { \
        return &_app_##APP_NAME; \
    }
/**
 * @brief Defines a GUI application with a startup and shutdown animation.
 *
 * This macro creates a definition for a GUI application:
 * - `APP_NAME`: The name of the application.
 * - `STARTUP_ANIMATION`: The animation used when the application starts.
 * - `SHUTDOWN_ANIMATION`: The animation used when the application shuts down.
 *
 * @param APP_NAME Name of the application.
 * @param STARTUP_ANIMATION The animation type used for startup (of type `gui_app_animation_t`).
 * @param SHUTDOWN_ANIMATION The animation type used for shutdown (of type `gui_app_animation_t`).
 */
#define GUI_APP_DEFINE_NAME_ANIMATION(APP_NAME, STARTUP_ANIMATION, SHUTDOWM_ANIMATION) \
    static void _##APP_NAME##_ui_design(gui_app_t*); \
    static gui_app_t _app_##APP_NAME = \
                                       { \
                                         .screen = \
                                                   { \
                                                     .name = #APP_NAME, /**< The screen name is set to the application name. */ \
                                                     .magic = GUI_MAGIC_NUMBER, /**< check number. */ \
                                                   }, \
                                         .ui_design = _##APP_NAME##_ui_design, /**< The UI design function is assigned with the modified name. */ \
                                         .active_ms = 1000000, /**< The active duration is set to 1,000,000 milliseconds. */ \
                                         .startup_animation_flag = STARTUP_ANIMATION,\
                                         .shutdown_animation_flag = SHUTDOWM_ANIMATION,\
                                       }; \
    /**
     * @brief Function to get the handle of the application.
     *
     * @return A pointer to the application instance.
     */ \
    gui_app_t *_get_app_##APP_NAME##_handle(void) \
    { \
        return &_app_##APP_NAME; \
    }
/**
 * @brief Macro to declare the entry point of a GUI application's UI design function.
 *
 * This macro declares the UI design function for the application.
 *
 * @param APP_NAME The name of the application.
 */
#define GUI_APP_ENTRY(APP_NAME) static void _##APP_NAME##_ui_design(gui_app_t* app)
typedef unsigned char gui_app_animation_t;
#define GUI_APP_ANIMATION_NULL (gui_app_animation_t)0
#define GUI_APP_ANIMATION_1 (gui_app_animation_t)1 ///< Recommended for startup
#define GUI_APP_ANIMATION_2 (gui_app_animation_t)2 ///< Recommended for startup
#define GUI_APP_ANIMATION_3 (gui_app_animation_t)3 ///< Recommended for startup
#define GUI_APP_ANIMATION_4 (gui_app_animation_t)4 ///< Recommended for startup
#define GUI_APP_ANIMATION_5 (gui_app_animation_t)5 ///< Recommended for shutdown
#define GUI_APP_ANIMATION_6 (gui_app_animation_t)6 ///< Recommended for shutdown
#define GUI_APP_ANIMATION_7 (gui_app_animation_t)7 ///< Recommended for shutdown
#define GUI_APP_ANIMATION_8 (gui_app_animation_t)8 ///< Recommended for shutdown
#define GUI_APP_ANIMATION_9 (gui_app_animation_t)9 ///< Recommended for startup
#define GUI_APP_ANIMATION_10 (gui_app_animation_t)10 ///< Recommended for shutdown
/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief get current app pointer
*/
gui_app_t *gui_current_app(void);
/**
 * @brief get nect app pointer if there are two apps exist.
*/
gui_app_t *gui_next_app(void);

void gui_app_install(gui_app_t *app, void *ui_design, void *gui_app_entry);

void gui_app_startup(gui_app_t *app);

void gui_app_shutdown(gui_app_t *app);

gui_app_t *gui_app_create(const char *app_name, void *ui_design, void *gui_app_entry);

/**
 * @brief switch app from A to B
 *
 * @param from A pointer
 * @param to B pointer
 */
void gui_switch_app(gui_app_t *from, gui_app_t *to);
/**
 * @brief Set next app top layer, call this function before the context of the next app startup.
 *
 */
void gui_app_layer_top(void);
/**
 * @brief Set next app button layer, call this function before the context of the next app startup.
 *
 */
void gui_app_layer_buttom(void);
/**
 * @brief get next app layer
 *
 * @return true top layer
 * @return false button layer
 */
bool gui_app_get_layer(void);
/**
 * @brief set app active ms time
 *
 * @param app app set
 * @param active_ms active ms times
 */
void gui_set_app_active_time(gui_app_t *app, uint32_t active_ms);
/**
 * @brief Retrieves the application object associated with the specified GUI object.
 *
 * This function traverses the object tree starting from the given GUI object until
 * it finds the root object, and then returns the application object associated with
 * that root.
 *
 * @param obj A pointer to a GUI object.
 *
 * @return A pointer to the application object if found. If the input object is NULL
 *         or if no application object is found, the function returns NULL.
 */
gui_app_t *gui_obj_tree_get_app(gui_obj_t *obj);
void gui_app_append(gui_app_t *app);
gui_app_t *gui_app_get_by_name(const char *name);

#ifdef __cplusplus
}
#endif
#endif















