#ifndef _RVD_H_
#define _RVD_H_

#define RVD_ERROR_NO_APP 1        /**< Error code indicating no applications were found */
#define RVD_ERROR_INDEX_INVALID 2 /**< Error code indicating an invalid application index */
#define XML_ARRAY_SIZE 10         /**< Maximum number of XML files that can be stored in the array */
#define MACRO_CONNECTED 9001
#define MACRO_DISCONNECTED 9002
#define MACRO_CONNECTING 9003
#define MACRO_WIFI 9004
#define MACRO_FRAME 9005
#define MACRO_INTERACTION 9006
#define MACRO_FPS 9007

#define GUI_EVENT_WIN_FRAME_PARAM (0XFF-2)
#define GUI_EVENT_WIN_RADIO_HOVER (0XFF-3)
#include "gui_tabview.h"
#include "gui_page.h"
#include "gui_text.h"
#include "gui_app.h"
/**
 * @brief Scans the "/app" directory for XML files and stores their paths.
 *
 * This function traverses the "/app" directory, identifies XML files, and stores their paths
 * in a static array. The array is cleared before scanning.
 *
 * @return The number of XML files found, or 0 if an error occurs.
 */
int rvd_scan_xml_files(void);

/**
 * @brief Retrieves the array of XML file paths.
 *
 * Returns a pointer to the static array containing paths to XML files found during the scan.
 *
 * @return A pointer to the array of XML file paths.
 */
char **rvd_get_xml_array(void);

/**
 * @brief Starts an application based on the provided index.
 *
 * Attempts to start or switch to an application using the XML file at the specified index
 * in the XML array. Returns error codes if no apps are found or the index is invalid.
 *
 * @param[in] app_index The index of the application to start (0-based).
 * @return 0 on success, RVD_ERROR_NO_APP if no apps are found, or RVD_ERROR_INDEX_INVALID if the index is invalid.
 */
int rvd_startup_app(int app_index);

/**
 * @brief Frees the memory allocated for the XML file path array.
 *
 * This function iterates through the static XML array, freeing each non-null string
 * allocated by gui_strdup() during the rvd_scan_xml_files() process. After freeing,
 * each array element is set to NULL to prevent dangling pointers.
 *
 * @note This function should be called when the XML array is no longer needed to avoid memory leaks.
 */
void rvd_free_xml_array(void);

#define XML_DOM_KEY_COUINT (INT8_MAX + 1)

/**
 * @brief Updates the state of a key in the XML DOM key array.
 *
 * This function sets the `up` and `down` states of a key identified by its ID in the `xml_dom_key_array`.
 * It validates the ID to ensure it is within the valid range before updating the state.
 *
 * @param id The index of the key in the `xml_dom_key_array` (must be between 0 and XML_DOM_KEY_COUINT - 1).
 * @param up The state of the key release (true for released, false otherwise).
 * @param down The state of the key press (true for pressed, false otherwise).
 * @return gui_error_t Returns GUI_SUCCESS if the update is successful, or GUI_ERROR_NULL if the ID is out of range.
 */
gui_error_t gui_xml_dom_write_key_array(int id, bool up, bool down);
#include "gui_img.h"
/**
 * @brief Structure defining parameters for image-based animations in the GUI framework.
 *
 * This structure holds configuration data for animating an image widget, including
 * position, translation, rotation, scaling, opacity, and image array settings.
 */
typedef struct
{
    int x;                  /**< Current x-coordinate of the image. */
    int y;                  /**< Current y-coordinate of the image. */
    int translate_from_x;   /**< Starting x-coordinate for translation animation. */
    int translate_from_y;   /**< Starting y-coordinate for translation animation. */
    int translate_to_x;     /**< Ending x-coordinate for translation animation. */
    int translate_to_y;     /**< Ending y-coordinate for translation animation. */
    int rotate_x;           /**< X-coordinate of the rotation center. */
    int rotate_y;           /**< Y-coordinate of the rotation center. */
    int rotate_from;        /**< Starting angle (degrees) for rotation animation. */
    int rotate_to;          /**< Ending angle (degrees) for rotation animation. */
    float textContent_from; /**< Starting value for text content animation (if applicable). */
    float textContent_to;   /**< Ending value for text content animation (if applicable). */
    float scale_x;          /**< Current x-axis scale factor. */
    float scale_y;          /**< Current y-axis scale factor. */
    float scale_x_from;     /**< Starting x-axis scale factor for animation. */
    float scale_y_from;     /**< Starting y-axis scale factor for animation. */
    int scale_x_center;     /**< X-coordinate of the scaling center. */
    int scale_y_center;     /**< Y-coordinate of the scaling center. */
    uint8_t opacity;        /**< Current opacity level (0-255). */
    uint8_t opacity_from;   /**< Starting opacity level for animation (0-255). */
    void **image_arr;       /**< Array of image pointers for animation frames. */
    int image_count;        /**< Number of images in the image array. */
    char *img_name;         /**< Name of the image widget. */
    char *animate_type;     /**< Type of animation (e.g., translation, rotation). */
    gui_img_t *img;         /**< Pointer to the GUI image widget. */
} image_animate_params_t;

/**
 * @brief Structure defining parameters for a radio button widget.
 *
 * This structure holds configuration data for a radio button, including its images,
 * group name, and state flags for check, hover, click, and press events.
 */
typedef struct radio
{
    const uint8_t *pic;     /**< Pointer to the default image data for the radio button. */
    const uint8_t *pic_hl;  /**< Pointer to the highlighted image data for the radio button. */
    char *radio_name;       /**< Name of the radio button group. */
    gui_img_t *img;         /**< Pointer to the GUI image widget for the radio button. */
    uint8_t check : 1;      /**< Flag indicating if the radio button is checked (1 = checked). */
    uint8_t hover : 1;      /**< Flag indicating if the radio button is hovered (1 = hovered). */
    uint8_t click : 1;      /**< Flag indicating if the radio button is clicked (1 = clicked). */
    uint8_t press : 1;      /**< Flag indicating if the radio button is pressed (1 = pressed). */
} radio_t;
/**
 * @brief Retrieves the current hover index of a button.
 *
 * This function returns the global `button_hover_index`, which indicates the index of
 * the currently hovered button within a button group in the GUI application.
 *
 * @return int The current hover index of the button.
 */
int rvd_get_button_hover_index();
/**
 * @brief Sets the hover index for a button.
 *
 * This function updates the global `button_hover_index` to the specified value,
 * controlling which button is currently hovered in the GUI application.
 *
 * @param index The new hover index to set for the button.
 */
void rvd_set_button_hover_index(int index);
gui_app_t *get_current_rvd_app(void);
/**
 * @brief Handles the click event for a button in the current application context.
 *
 * This function processes a button click event, either for a radio button (APP_0, APP_C2)
 * or specific widgets in a tabview (APP_C3). For APP_0 and APP_C2, it retrieves the radio
 * button widget by index and group, sets its click state, and logs the event. For APP_C3,
 * it handles clicks based on the current tab, either triggering a window widget's click
 * override or clicking specific widgets (e.g., combobox or button).
 *
 * @return int Returns GUI_SUCCESS on successful execution, or GUI_ERROR_NULL if the
 *         application, screen, or target widget is not found.
 */
int rvd_click_button();
/**
 * @brief Sets the hover state for a radio button at the specified index within a group.
 *
 * This function navigates to the radio button at the given index within a button group,
 * ensuring the index is valid (within the range [0, button_count-1]). It retrieves the
 * radio button widget, triggers its hover state, and logs the event.
 *
 * @param index The index of the radio button to hover (0-based).
 * @param button_count The total number of radio buttons in the group.
 *
 * @return int Returns GUI_SUCCESS on successful execution, GUI_ERROR if button_count
 *         is invalid (<= 0), or GUI_ERROR_NULL if the radio button widget is not found.
 */
int rvd_hover_button(int index, int button_count);
/**
 * @brief Handles downward hover navigation for a button or page in a tabview.
 *
 * This function manages the hover state navigation to move down to the next button or
 * page within a tabview widget. It retrieves the current tab and page, then performs
 * navigation based on the application context (APP_C2, APP_0, APP_C3, or APP_C5).
 * For APP_C2, it checks if the hovered button is within a page and navigates accordingly.
 *
 * @return int The result of the hover navigation operation, typically indicating success
 *         (e.g., from rvd_hover_page or rvd_hover_down_default) or GUI_ERROR_NULL if
 *         the radio button widget is not found.
 */
int rvd_hover_down_button();
/**
 * @brief Handles the hover navigation for moving up to the previous button or page.
 *
 * This function manages the hover state navigation for a button within a tabview widget.
 * It retrieves the current tab and page, then navigates to the previous button or page
 * based on the application context (APP_C2, APP_0, APP_C3, or APP_C5).
 *
 * @return int The result of the hover navigation operation, typically indicating success
 *         or failure as defined by the called functions (e.g., rvd_hover_page,
 *         rvd_hover_up_default).
 */
int rvd_hover_up_button();


/**
 * @brief Counts the number of radio buttons in the specified group within the current application.
 *
 * This function retrieves the current application and its root screen, verifies the context,
 * and counts the number of radio buttons belonging to the same group as the first active
 * radio button. It logs the count for debugging purposes.
 *
 * @return int The number of radio buttons in the group, or 0 if the application context
 *         is invalid or no active radio button is found.
 */
int rvd_get_button_count();
/**
 * @brief Sets the force frame buffer change flag.
 *
 * This function updates the global `force_fb_change` flag to control whether the frame
 * buffer should be forcibly updated during screen rendering.
 *
 * @param if_change Boolean value to set the force frame buffer change flag.
 *                  True forces a frame buffer update; false allows conditional updates.
 */
void gui_update_screen_force(bool if_change);
/**
 * @brief Initiates the back navigation action for a tabview widget.
 */
void rvd_click_back();
/**
 * @brief Handles left navigation for hovering over radio buttons in a tabview.
 *
 * This function, conditionally compiled for APP_C4, navigates the hover state to the previous
 * radio button within predefined index ranges for specific tabs (cur_id.x = 1 or 4).
 * It retrieves the current hover index and updates it within the specified range.
 */
void rvd_hover_left();
/**
 * @brief Handles right navigation for hovering over radio buttons in a tabview.
 *
 * This function, conditionally compiled for APP_C4, navigates the hover state to the next
 * radio button within predefined index ranges for specific tabs (cur_id.x = 1 or 4).
 * It retrieves the current hover index and updates it within the specified range.
 */
void rvd_hover_right();
/**
 * @brief Creates a radio button widget with specified properties and animations.
 *
 * This function initializes a radio button widget as a child of the given parent, with
 * the specified name, group name, position, size, images, and blend mode. It sets up
 * event callbacks, animations, and image data for the radio button, including press and
 * hover interactions.
 *
 * @param parent Pointer to the parent GUI object (e.g., a window or container).
 * @param name Name of the radio button widget.
 * @param group_name Name of the radio button group for grouping related radio buttons.
 * @param x X-coordinate of the radio button's top-left corner.
 * @param y Y-coordinate of the radio button's top-left corner.
 * @param w Width of the radio button.
 * @param h Height of the radio button.
 * @param pic Pointer to the default image data for the radio button.
 * @param hl_pic Pointer to the highlighted image data for the radio button.
 * @param blendMode Blend mode for rendering the radio button image (e.g., SRC_OVER).
 *
 * @return gui_obj_t* Pointer to the created radio button widget, or NULL if creation fails.
 */
gui_obj_t *gui_create_radiobutton(gui_obj_t *parent, const char *name, const char *group_name,
                                  int x, int y, int w, int h, const unsigned char *pic, const unsigned char *hl_pic,
                                  BLEND_MODE_TYPE blendMode);
/**
 * @brief Get the address of the image404 data.
 *
 * This function returns the address of the image404 data stored in the program.
 *
 * @return The address of the image404 data.
 */
const unsigned char *rvd_get_image404_address(void);
/**
 * @brief Initializes the custom UI for the given app.
 *
 * This function checks if one of the APP_C* macros is defined and calls
 * the corresponding initialization function if so.
 *
 * @note This function is only available if one of the APP_C* macros is
 *         defined.
 */
void rvd_custom_init(void);
/**
 * @brief Get the radio data from the given radio object.
 *
 * @param radio The radio object from which to get the data.
 * @return The radio data if the radio object is valid and has
 * animation data, otherwise NULL.
 */
radio_t *rvd_get_radio_data(gui_obj_t *radio);
/**
 * @brief Returns the number of RADIO buttons in the given root object.
 *
 * @param root The root object to search for RADIO buttons.
 *
 * @return The number of RADIO buttons in the given root object.
 */
int rvd_get_button_count_root(gui_obj_t *root);
/**
 * @brief Handles the press event for a radio button, initiating the press animation.
 *
 * This function retrieves the current application and the active radio button based on
 * the hover index and group name. It sets the press flag in the radio button's data
 * to trigger the associated press animation (e.g., shrinking effect).
 *
 * @return int Returns GUI_SUCCESS on successful execution, or GUI_ERROR_NULL if the
 *         application, screen, or radio button widget is not found.
 */
int rvd_click_button_down();
/**
 * @brief Synchronizes the hover state of a radio button based on the given index.
 *
 * This function updates the hover state of a radio button widget within a specified group.
 * It validates the input parameters, adjusts the index if necessary, retrieves the corresponding
 * radio button widget, and sets its hover state. The function also updates global hover indices
 * based on the radio button group name.
 *
 * @param index The index of the radio button to set as hovered.
 * @param button_count The total number of buttons in the group.
 * @return GUI_SUCCESS on success, GUI_ERROR if button_count is invalid, or GUI_ERROR_NULL if the widget is not found.
 */
int rvd_hover_button_sync(int index, int button_count);
/**
 * @brief Retrieves an array of radio button widgets belonging to a specified group.
 *
 * This function searches for radio button widgets of a given type within the provided GUI object tree
 * that match the specified group name. It populates an array with pointers to the matching widgets
 * and returns the count of found widgets.
 *
 * @param root The root GUI object to start the search from.
 * @param type The type of widget to search for (e.g., RADIO).
 * @param output_array Pointer to store the array of matching widget pointers.
 * @param length Pointer to store the number of widgets found.
 * @param group_name The name of the radio button group to filter by.
 * @return 0 on success, -1 if input parameters are invalid.
 */
int radio_group_get_array(gui_obj_t *root,
                          T_OBJ_TYPE type,
                          gui_obj_t ***output_array,
                          int *length, const char *group_name);

/**
 * @brief Sets the initial hover state for a tab in a tabview widget.
 *
 * This function checks the current tab in a tabview widget and sets the hover state for a specific
 * radio button or page based on the tab's identifier. It is conditionally compiled for APP_C2.
 *
 * @return The result of the hover operation (e.g., GUI_SUCCESS or an error code).
 */
int rvd_tab_first(void);



/**
 * @brief Stops the animation of an image movie widget.
 *
 * This function halts any ongoing frame-by-frame animation associated with
 * an IMAGE_FROM_MEM widget that has been configured as an image movie
 * (i.e., created from a folder of image files).
 *
 * @param[in] obj Pointer to the GUI object (must be an image widget created
 *                from a folder, i.e. type IMAGE_FROM_MEM with animation).
 *
 * @note The function is safe to call on a non-animated image - it simply
 *       does nothing in that case.
 * @warning The object must be of type IMAGE_FROM_MEM, otherwise a runtime
 *          exception is triggered via GUI_WIDGET_TYPE_TRY_EXCEPT().
 *
 * @par Usage example - Stop animation and jump to the last frame
 * @code
 *      // Get widget created in XML as: <img folder="..." >imagemovie4</img>
 *      GUI_WIDGET_POINTER_BY_NAME(movie, "imagemovie4");
 *
 *      // Stop any running animation
 *      rvd_ImageMovie_stop(movie);
 *
 *      // Optionally go to the last frame (useful for "pause on end" effect)
 *      int index = rvd_ImageMovie_frame_count(movie);
 *      if (index > 0)
 *      {
 *          rvd_ImageMovie_frame(movie, index - 1);  // show last frame
 *      }
 * @endcode
 *
 * @see rvd_ImageMovie_frame()
 * @see rvd_ImageMovie_frame_count()
 */
void rvd_ImageMovie_stop(gui_obj_t *obj);

/**
 * @brief Sets a specific frame of an image movie widget.
 *
 * Immediately stops any running animation and displays the requested frame
 * identified by zero-based index. Useful for implementing frame-by-frame
 * control, seeking, thumbnails, or manual animation.
 *
 * @param[in] obj         Pointer to the image movie widget.
 * @param[in] frame_index Zero-based index of the frame to display
 *                        (0 <= frame_index < frame_count).
 *
 * @pre The widget must have been created with a "folder" attribute containing
 *      one or more image files.
 * @note If the widget has no animation or the index is out of bounds,
 *       the function returns silently without changing the displayed image.
 *
 * @par Usage example - Jump to last frame (see full example in rvd_ImageMovie_stop())
 *
 * @see rvd_ImageMovie_stop()
 * @see rvd_ImageMovie_frame_count()
 */
void rvd_ImageMovie_frame(gui_obj_t *obj, int frame_index);


/**
 * @brief Returns the total number of frames in an image movie widget.
 *
 * @param[in] obj Pointer to the image movie widget (created from a folder).
 *
 * @return Number of frames in the animation sequence.
 *         Returns 0 if the widget has no animation data.
 *
 * @par Usage example - Used together with the other functions to seek to a specific frame
 * @code
 *      GUI_WIDGET_POINTER_BY_NAME(movie, "imagemovie4");
 *      rvd_ImageMovie_stop(movie);
 *      int index = rvd_ImageMovie_frame_count(movie);
 *      rvd_ImageMovie_frame(movie, index - 1);  // display last frame
 * @endcode
 *
 * @see rvd_ImageMovie_frame()
 * @see rvd_ImageMovie_stop()
 */
int rvd_ImageMovie_frame_count(gui_obj_t *obj);
/**
 * @brief Starts (or restarts) the animation of an image movie widget.
 *
 * This function resumes or restarts the frame-by-frame animation for an
 * IMAGE_FROM_MEM widget that was created from a folder of images
 * (commonly referred to as an "image movie" or animated sequence).
 *
 * If the animation is already running, it is restarted from the first frame.
 * If it was previously stopped (e.g. via rvd_ImageMovie_stop()), this call
 * will resume playback from the beginning.
 *
 * @param[in] obj Pointer to the image movie widget (must be of type
 *                IMAGE_FROM_MEM and have an associated animation).
 *
 * @return None
 *
 * @warning Triggers a runtime exception via GUI_WIDGET_TYPE_TRY_EXCEPT()
 *          if obj is not an IMAGE_FROM_MEM widget.
 * @note   Safe to call on a widget that has no animation - the call is ignored.
 *
 * @par Usage example - Start animation (typically after stopping or seeking)
 * @code
 *      // Retrieve the widget defined in XML as: <img folder="..." >imagemovie4</img>
 *      GUI_WIDGET_POINTER_BY_NAME(movie, "imagemovie4");
 *
 *      // Example: Stop on last frame, then later restart from the beginning
 *      rvd_ImageMovie_stop(movie);
 *      int count = rvd_ImageMovie_frame_count(movie);
 *      if (count > 0)
 *      {
 *          rvd_ImageMovie_frame(movie, count - 1);  // show final frame
 *      }
 *
 *      rvd_ImageMovie_start(movie);  // restart animation from frame 0
 * @endcode
 *
 * @see rvd_ImageMovie_stop()
 * @see rvd_ImageMovie_frame()
 * @see rvd_ImageMovie_frame_count()
 */
void rvd_ImageMovie_start(gui_obj_t *obj);
#endif /* _RVD_H_ */