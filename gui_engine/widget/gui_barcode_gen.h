#ifndef _GUI_BARCODE_GEN_H_
#define _GUI_BARCODE_GEN_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Main symbol structure */
typedef struct barcode_symbol
{
    int symbology; /* Symbol to use (see BARCODE_XXX below) */
    float height;  /* Barcode height in X-dimensions (ignored for fixed-width barcodes) */
    unsigned char scale;
    int output_options; /* Various output parameters (bind, box etc, see below) */
    int input_mode;     /* Encoding of input data (see DATA_MODE etc below). Default DATA_MODE */
    unsigned char text_gap;
    int warn_level;                     /* Affects error/warning value returned by barcode API (see WARN_XXX below) */
    int debug;                          /* Debugging flags */
    unsigned char
    text[160];            /* Human Readable Text (HRT) (if any), UTF-8, NUL-terminated (output only) */
    uint32_t text_len;
    int rows;                           /* Number of rows used by the symbol (output only) */
    int width;                          /* Width of the generated symbol (output only) */
    unsigned char
    encoded_data[1][144]; /* Encoded data (output only). Allows for rows of 1152 modules */
    float row_height[1];                /* Heights of rows (output only). Allows for 200 row DotCode */
    char errtxt[100];                   /* Error message if an error or warning occurs, NUL-terminated (output only) */
} barcode_symbol_t;

barcode_symbol_t *barcode_encode(unsigned char source[], int length);

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif

