#ifndef _RVD_H_
#define _RVD_H_

#define RVD_ERROR_NO_APP 1        /**< Error code indicating no applications were found */
#define RVD_ERROR_INDEX_INVALID 2 /**< Error code indicating an invalid application index */
#define XML_ARRAY_SIZE 10         /**< Maximum number of XML files that can be stored in the array */

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
#include "guidef.h"
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
typedef struct
{
    int x;
    int y;
    int translate_from_x;
    int translate_from_y;
    int translate_to_x;
    int translate_to_y;
    int rotate_x;
    int rotate_y;
    int rotate_from;
    int rotate_to;
    float textContent_from;
    float textContent_to;
    float scale_x;
    float scale_y;
    float scale_x_from;
    float scale_y_from;
    int scale_x_center;
    int scale_y_center;
    uint8_t opacity;
    uint8_t opacity_from;
    void **image_arr;
    int image_count;
    char *img_name;
    char *animate_type;
    gui_img_t *img;
} image_animate_params_t;
#endif /* _RVD_H_ */