#ifndef _GUI_QRCODE_GEN_H_
#define _GUI_QRCODE_GEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "math.h"

#define qrcodegen_VERSION_MIN   2  // The minimum version number supported in the QR Code Model 2 standard
#define qrcodegen_VERSION_MAX  15
/*
 * Returns the side length of the given QR Code, assuming that encoding succeeded.
 * The result is in the range [21, 177]. Note that the length of the array buffer
 * is related to the side length - every 'uint8_t qrcode[]' must have length at least
 * qrcodegen_BUFFER_LEN_FOR_VERSION(version), which equals ceil(size^2 / 8 + 1).
 */
int qrcodegen_getSize(const uint8_t qrcode[]);


/*
 * Returns the color of the module (pixel) at the given coordinates, which is false
 * for light or true for dark. The top left corner has the coordinates (x=0, y=0).
 * If the given coordinates are out of bounds, then false (light) is returned.
 */
bool qrcodegen_getModule(const uint8_t qrcode[], int x, int y);

uint8_t *qrcode_get_data(unsigned char *data, uint16_t data_length, uint8_t flag);

#ifdef __cplusplus
}
#endif

#endif /* _GUI_QRCODE_GEN_H_ */
