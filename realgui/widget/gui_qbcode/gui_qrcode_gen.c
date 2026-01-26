/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "gui_qrcode_gen.h"
#include "gui_api.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/

#ifndef QRCODEGEN_TEST
#define testable static  // Keep functions private
#else
#define testable  // Expose private functions
#endif

#define NDEBUG

/*---- Macro constants and functions ----*/

#define QRCODEGEN_BUFFER_LEN_FOR_VERSION(n)  ((((n) * 4 + 17) * ((n) * 4 + 17) + 7) / 8 + 1)

#define QRCODEGEN_BUFFER_LEN_MAX             QRCODEGEN_BUFFER_LEN_FOR_VERSION(QRCODEGEN_VERSION_MAX)

/*---- Functions (high level) to generate QR Codes ----*/

// Sentinel value for use in only some functions.
#define LENGTH_OVERFLOW                     -1

#define QRCODEGEN_REED_SOLOMON_DEGREE_MAX    30  // Based on the table above


/*============================================================================*
 *                           Types
 *============================================================================*/

typedef enum
{
    // Must be declared in ascending order of error protection
    // so that an internal qrcodegen function works properly
    QRCODEGEN_ECC_LOW = 0,   // The QR Code can tolerate about  7% erroneous codewords
    QRCODEGEN_ECC_MEDIUM,    // The QR Code can tolerate about 15% erroneous codewords
    QRCODEGEN_ECC_QUARTILE,  // The QR Code can tolerate about 25% erroneous codewords
    QRCODEGEN_ECC_HIGH,      // The QR Code can tolerate about 30% erroneous codewords
} T_QRCODEGEN_ECC;

typedef enum
{
    // A special value to tell the QR Code encoder to
    // automatically select an appropriate mask pattern
    QRCODEGEN_MASK_AUTO = -1,
    // The eight actual mask patterns
    QRCODEGEN_MASK_0    =  0,
    QRCODEGEN_MASK_1,
    QRCODEGEN_MASK_2,
    QRCODEGEN_MASK_3,
    QRCODEGEN_MASK_4,
    QRCODEGEN_MASK_5,
    QRCODEGEN_MASK_6,
    QRCODEGEN_MASK_7,
} T_QRCODEGEN_MASK;

/*
 * Describes how a segment's data bits are interpreted.
 */
typedef enum
{
    QRCODEGEN_MODE_NUMERIC      = 0x1,
    QRCODEGEN_MODE_ALPHANUMERIC = 0x2,
    QRCODEGEN_MODE_BYTE         = 0x4,
    QRCODEGEN_MODE_KANJI        = 0x8,
    QRCODEGEN_MODE_ECI          = 0x7,
} T_QRCODEGEN_MODE;

struct qrcodegen_Segment
{
    T_QRCODEGEN_MODE mode;
    int num_chars;
    uint8_t *data;
    int bit_length;
};

typedef struct
{
    uint8_t qrcode[QRCODEGEN_BUFFER_LEN_MAX];
    uint16_t size;
    uint16_t times;
    uint8_t gen_version_min;
    uint8_t gen_version_max;
    uint8_t data_ready;
    //uint8_t* p_image_data;
} T_APP_QRCODE;


/*============================================================================*
 *                           Constants
 *============================================================================*/

static const char *ALPHANUMERIC_CHARSET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

// For generating error correction codes.
testable const int8_t ECC_CODEWORDS_PER_BLOCK[4][41] =
{
    // Version: (note that index 0 is for padding, and is set to an illegal value)
    //0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
    {-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Low
    {-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},  // Medium
    {-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Quartile
    {-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // High
};

// For generating error correction codes.
testable const int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41] =
{
    // Version: (note that index 0 is for padding, and is set to an illegal value)
    //0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
    {-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Low
    {-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Medium
    {-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Quartile
    {-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // High
};

// For automatic mask pattern selection.
static const int PENALTY_N1 =  3;
static const int PENALTY_N2 =  3;
static const int PENALTY_N3 = 40;
static const int PENALTY_N4 = 10;

/*============================================================================*
 *                            Variables
 *============================================================================*/

T_APP_QRCODE app_qrcode = {0};


/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/*
 * Encodes the given text string to a QR Code, returning true if successful.
 * If the data is too long to fit in any version in the given range
 * at the given ECC level, then false is returned.
 *
 * The input text must be encoded in UTF-8 and contain no NULs.
 * Requires 1 <= min_version <= max_version <= 40.
 *
 * The smallest possible QR Code version within the given range is automatically
 * chosen for the output. Iff boost_ecl is true, then the ECC level of the result
 * may be higher than the ecl argument if it can be done without increasing the
 * version. The mask is either between QRCODEGEN_MASK_0 to 7 to force that mask, or
 * QRCODEGEN_MASK_AUTO to automatically choose an appropriate mask (which may be slow).
 *
 * About the arrays, letting len = QRCODEGEN_BUFFER_LEN_FOR_VERSION(max_version):
 * - Before calling the function:
 *   - The array ranges temp_buffer[0 : len] and qrcode[0 : len] must allow
 *     reading and writing; hence each array must have a length of at least len.
 *   - The two ranges must not overlap (aliasing).
 *   - The initial state of both ranges can be uninitialized
 *     because the function always writes before reading.
 * - After the function returns:
 *   - Both ranges have no guarantee on which elements are initialized and what values are stored.
 *   - temp_buffer contains no useful data and should be treated as entirely uninitialized.
 *   - If successful, qrcode can be passed into gui_qrcode_gen_get_size() and gui_qrcode_gen_get_module().
 *
 * If successful, the resulting QR Code may use numeric,
 * alphanumeric, or byte mode to encode the text.
 *
 * In the most optimistic case, a QR Code at version 40 with low ECC
 * can hold any UTF-8 string up to 2953 bytes, or any alphanumeric string
 * up to 4296 characters, or any digit string up to 7089 characters.
 * These numbers represent the hard upper limit of the QR Code standard.
 *
 * Please consult the QR Code specification for information on
 * data capacities per version, ECC level, and text encoding mode.
 */
bool gui_qrcode_gen_encode_text(const char       *text,
                                uint8_t           temp_buffer[],
                                uint8_t          *qrcode,
                                T_QRCODEGEN_ECC   ecl,
                                int               min_version,
                                int               max_version,
                                T_QRCODEGEN_MASK  mask,
                                bool              boost_ecl);

/*
 * Encodes the given binary data to a QR Code, returning true if successful.
 * If the data is too long to fit in any version in the given range
 * at the given ECC level, then false is returned.
 *
 * Requires 1 <= min_version <= max_version <= 40.
 *
 * The smallest possible QR Code version within the given range is automatically
 * chosen for the output. Iff boost_ecl is true, then the ECC level of the result
 * may be higher than the ecl argument if it can be done without increasing the
 * version. The mask is either between QRCODEGEN_MASK_0 to 7 to force that mask, or
 * QRCODEGEN_MASK_AUTO to automatically choose an appropriate mask (which may be slow).
 *
 * About the arrays, letting len = QRCODEGEN_BUFFER_LEN_FOR_VERSION(max_version):
 * - Before calling the function:
 *   - The array ranges data_and_temp[0 : len] and qrcode[0 : len] must allow
 *     reading and writing; hence each array must have a length of at least len.
 *   - The two ranges must not overlap (aliasing).
 *   - The input array range data_and_temp[0 : data_len] should normally be
 *     valid UTF-8 text, but is not required by the QR Code standard.
 *   - The initial state of data_and_temp[data_len : len] and qrcode[0 : len]
 *     can be uninitialized because the function always writes before reading.
 * - After the function returns:
 *   - Both ranges have no guarantee on which elements are initialized and what values are stored.
 *   - data_and_temp contains no useful data and should be treated as entirely uninitialized.
 *   - If successful, qrcode can be passed into gui_qrcode_gen_get_size() and gui_qrcode_gen_get_module().
 *
 * If successful, the resulting QR Code will use byte mode to encode the data.
 *
 * In the most optimistic case, a QR Code at version 40 with low ECC can hold any byte
 * sequence up to length 2953. This is the hard upper limit of the QR Code standard.
 *
 * Please consult the QR Code specification for information on
 * data capacities per version, ECC level, and text encoding mode.
 */
bool gui_qrcode_gen_encode_binary(uint8_t           data_and_temp[],
                                  size_t            data_len,
                                  uint8_t          *qrcode,
                                  T_QRCODEGEN_ECC   ecl,
                                  int               min_version,
                                  int               max_version,
                                  T_QRCODEGEN_MASK  mask,
                                  bool              boost_ecl);

/*---- Functions (low level) to generate QR Codes ----*/

/*
 * Encodes the given segments to a QR Code, returning true if successful.
 * If the data is too long to fit in any version at the given ECC level,
 * then false is returned.
 *
 * The smallest possible QR Code version is automatically chosen for
 * the output. The ECC level of the result may be higher than the
 * ecl argument if it can be done without increasing the version.
 *
 * About the byte arrays, letting len = QRCODEGEN_BUFFER_LEN_FOR_VERSION(QRCODEGEN_VERSION_MAX):
 * - Before calling the function:
 *   - The array ranges temp_buffer[0 : len] and qrcode[0 : len] must allow
 *     reading and writing; hence each array must have a length of at least len.
 *   - The two ranges must not overlap (aliasing).
 *   - The initial state of both ranges can be uninitialized
 *     because the function always writes before reading.
 *   - The input array segs can contain segments whose data buffers overlap with temp_buffer.
 * - After the function returns:
 *   - Both ranges have no guarantee on which elements are initialized and what values are stored.
 *   - temp_buffer contains no useful data and should be treated as entirely uninitialized.
 *   - Any segment whose data buffer overlaps with temp_buffer[0 : len]
 *     must be treated as having invalid values in that array.
 *   - If successful, qrcode can be passed into gui_qrcode_gen_get_size() and gui_qrcode_gen_get_module().
 *
 * Please consult the QR Code specification for information on
 * data capacities per version, ECC level, and text encoding mode.
 *
 * This function allows the user to create a custom sequence of segments that switches
 * between modes (such as alphanumeric and byte) to encode text in less space.
 * This is a low-level API; the high-level API is gui_qrcode_gen_encode_text() and gui_qrcode_gen_encode_binary().
 */
bool gui_qrcode_gen_encode_segments(const struct      qrcodegen_Segment segs[],
                                    size_t            len,
                                    T_QRCODEGEN_ECC   ecl,
                                    uint8_t           temp_buffer[],
                                    uint8_t          *qrcode);

/*
 * Encodes the given segments to a QR Code, returning true if successful.
 * If the data is too long to fit in any version in the given range
 * at the given ECC level, then false is returned.
 *
 * Requires 1 <= min_version <= max_version <= 40.
 *
 * The smallest possible QR Code version within the given range is automatically
 * chosen for the output. Iff boost_ecl is true, then the ECC level of the result
 * may be higher than the ecl argument if it can be done without increasing the
 * version. The mask is either between QRCODEGEN_MASK_0 to 7 to force that mask, or
 * QRCODEGEN_MASK_AUTO to automatically choose an appropriate mask (which may be slow).
 *
 * About the byte arrays, letting len = QRCODEGEN_BUFFER_LEN_FOR_VERSION(QRCODEGEN_VERSION_MAX):
 * - Before calling the function:
 *   - The array ranges temp_buffer[0 : len] and qrcode[0 : len] must allow
 *     reading and writing; hence each array must have a length of at least len.
 *   - The two ranges must not overlap (aliasing).
 *   - The initial state of both ranges can be uninitialized
 *     because the function always writes before reading.
 *   - The input array segs can contain segments whose data buffers overlap with temp_buffer.
 * - After the function returns:
 *   - Both ranges have no guarantee on which elements are initialized and what values are stored.
 *   - temp_buffer contains no useful data and should be treated as entirely uninitialized.
 *   - Any segment whose data buffer overlaps with temp_buffer[0 : len]
 *     must be treated as having invalid values in that array.
 *   - If successful, qrcode can be passed into gui_qrcode_gen_get_size() and gui_qrcode_gen_get_module().
 *
 * Please consult the QR Code specification for information on
 * data capacities per version, ECC level, and text encoding mode.
 *
 * This function allows the user to create a custom sequence of segments that switches
 * between modes (such as alphanumeric and byte) to encode text in less space.
 * This is a low-level API; the high-level API is gui_qrcode_gen_encode_text() and gui_qrcode_gen_encode_binary().
 */
bool gui_qrcode_gen_encode_segments_advanced(const struct       qrcodegen_Segment segs[],
                                             size_t             len,
                                             T_QRCODEGEN_ECC    ecl,
                                             int                min_version,
                                             int                max_version,
                                             T_QRCODEGEN_MASK   mask,
                                             bool               boost_ecl,
                                             uint8_t            temp_buffer[],
                                             uint8_t           *qrcode);

/*
 * Tests whether the given string can be encoded as a segment in numeric mode.
 * A string is encodable iff each character is in the range 0 to 9.
 */
bool gui_qrcode_gen_is_numeric(const char *text);

/*
 * Tests whether the given string can be encoded as a segment in alphanumeric mode.
 * A string is encodable iff each character is in the following set: 0 to 9, A to Z
 * (uppercase only), space, dollar, percent, asterisk, plus, hyphen, period, slash, colon.
 */
bool gui_qrcode_gen_is_alphanumeric(const char *text);

/*
 * Returns the number of bytes (uint8_t) needed for the data buffer of a segment
 * containing the given number of characters using the given mode. Notes:
 * - Returns SIZE_MAX on failure, i.e. num_chars > INT16_MAX or the internal
 *   calculation of the number of needed bits exceeds INT16_MAX (i.e. 32767).
 * - Otherwise, all valid results are in the range [0, ceil(INT16_MAX / 8)], i.e. at most 4096.
 * - It is okay for the user to allocate more bytes for the buffer than needed.
 * - For byte mode, num_chars measures the number of bytes, not Unicode code points.
 * - For ECI mode, num_chars must be 0, and the worst-case number of bytes is returned.
 *   An actual ECI segment can have shorter data. For non-ECI modes, the result is exact.
 */
size_t gui_qrcode_gen_calc_segment_buffer_size(T_QRCODEGEN_MODE mode, size_t num_chars);

/*
 * Returns a segment representing the given binary data encoded in
 * byte mode. All input byte arrays are acceptable. Any text string
 * can be converted to UTF-8 bytes and encoded as a byte mode segment.
 */
struct qrcodegen_Segment gui_qrcode_gen_make_bytes(const uint8_t data[], size_t len, uint8_t buf[]);

/*
 * Returns a segment representing the given string of decimal digits encoded in numeric mode.
 */
struct qrcodegen_Segment gui_qrcode_gen_make_numeric(const char *digits, uint8_t buf[]);

/*
 * Returns a segment representing the given text string encoded in alphanumeric mode.
 * The characters allowed are: 0 to 9, A to Z (uppercase only), space,
 * dollar, percent, asterisk, plus, hyphen, period, slash, colon.
 */
struct qrcodegen_Segment gui_qrcode_gen_make_alphanumeric(const char *text, uint8_t buf[]);

/*
 * Returns a segment representing an Extended Channel Interpretation
 * (ECI) designator with the given assignment value.
 */
struct qrcodegen_Segment gui_qrcode_gen_make_eci(long assign_val, uint8_t buf[]);

testable void gui_qrcode_gen_append_bits_to_buffer(unsigned int val, int num_bits, uint8_t buffer[],
                                                   int *bit_len);

testable void gui_qrcode_gen_add_ecc_and_interleave(uint8_t data[], int version,
                                                    T_QRCODEGEN_ECC ecl,
                                                    uint8_t result[]);

testable int gui_qrcode_gen_get_num_data_codewords(int version, T_QRCODEGEN_ECC ecl);

testable int gui_qrcode_gen_get_num_raw_data_modules(int ver);

testable void gui_qrcode_gen_reed_solomon_compute_divisor(int degree, uint8_t result[]);

testable void gui_qrcode_gen_reed_solomon_compute_remainder(const uint8_t data[], int data_len,
                                                            const uint8_t generator[], int degree, uint8_t result[]);

testable uint8_t gui_qrcode_gen_reed_solomon_multiply(uint8_t x, uint8_t y);

testable void gui_qrcode_gen_initialize_function_modules(int version, uint8_t *qrcode);

static void gui_qrcode_gen_draw_light_function_modules(uint8_t *qrcode, int version);

static void gui_qrcode_gen_draw_format_bits(T_QRCODEGEN_ECC ecl, T_QRCODEGEN_MASK mask,
                                            uint8_t *qrcode);

testable int gui_qrcode_gen_get_alignment_pattern_positions(int version, uint8_t result[7]);

static void gui_qrcode_gen_fill_rectangle(int left, int top, int width, int height,
                                          uint8_t *qrcode);

static void gui_qrcode_gen_draw_codewords(const uint8_t data[], int data_len, uint8_t *qrcode);

static void gui_qrcode_gen_apply_mask(const uint8_t function_modules[], uint8_t *qrcode,
                                      T_QRCODEGEN_MASK mask);

static long gui_qrcode_gen_get_penalty_score(const uint8_t *qrcode);

static int gui_qrcode_gen_finder_penalty_count_patterns(const int run_history[7], int qrsize);

static int gui_qrcode_gen_finder_penalty_terminate_and_count(bool current_run_color,
                                                             int current_run_length,
                                                             int run_history[7], int qrsize);

static void gui_qrcode_gen_finder_penalty_add_history(int current_run_length, int run_history[7],
                                                      int qrsize);

testable bool gui_qrcode_gen_get_module_bounded(const uint8_t *qrcode, int x, int y);

testable void gui_qrcode_gen_set_module_bounded(uint8_t *qrcode, int x, int y, bool is_dark);

testable void gui_qrcode_gen_set_module_unbounded(uint8_t *qrcode, int x, int y, bool is_dark);

static bool gui_qrcode_gen_get_bit(int x, int i);

testable int gui_qrcode_gen_calc_segment_bit_length(T_QRCODEGEN_MODE mode, size_t num_chars);

testable int gui_qrcode_gen_get_total_bits(const struct qrcodegen_Segment segs[], size_t len,
                                           int version);

static int gui_qrcode_gen_num_char_count_bits(T_QRCODEGEN_MODE mode, int version);

/*---- High-level QR Code encoding functions ----*/

// Public function - see documentation comment in header file.
bool gui_qrcode_gen_encode_text(const char       *text,
                                uint8_t           temp_buffer[],
                                uint8_t          *qrcode,
                                T_QRCODEGEN_ECC   ecl,
                                int               min_version,
                                int               max_version,
                                T_QRCODEGEN_MASK  mask,
                                bool              boost_ecl)
{
    struct qrcodegen_Segment seg;
    size_t buf_len;
    size_t text_len = strlen(text);

    if (text_len == 0)
    {
        return gui_qrcode_gen_encode_segments_advanced(NULL, 0, ecl, min_version, max_version, mask,
                                                       boost_ecl,
                                                       temp_buffer, qrcode);
    }

    buf_len = (size_t)QRCODEGEN_BUFFER_LEN_FOR_VERSION(max_version);

    if (gui_qrcode_gen_is_numeric(text))
    {
        if (gui_qrcode_gen_calc_segment_buffer_size(QRCODEGEN_MODE_NUMERIC, text_len) > buf_len)
        {
            goto fail;
        }

        seg = gui_qrcode_gen_make_numeric(text, temp_buffer);
    }
    else if (gui_qrcode_gen_is_alphanumeric(text))
    {
        if (gui_qrcode_gen_calc_segment_buffer_size(QRCODEGEN_MODE_ALPHANUMERIC, text_len) > buf_len)
        {
            goto fail;
        }

        seg = gui_qrcode_gen_make_alphanumeric(text, temp_buffer);
    }
    else
    {
        if (text_len > buf_len)
        {
            goto fail;
        }

        for (size_t i = 0; i < text_len; i++)
        {
            temp_buffer[i] = (uint8_t)text[i];
        }

        seg.mode = QRCODEGEN_MODE_BYTE;
        seg.bit_length = gui_qrcode_gen_calc_segment_bit_length(seg.mode, text_len);

        if (seg.bit_length == LENGTH_OVERFLOW)
        {
            goto fail;
        }

        seg.num_chars = (int)text_len;
        seg.data = temp_buffer;
    }

    return gui_qrcode_gen_encode_segments_advanced(&seg, 1, ecl, min_version, max_version, mask,
                                                   boost_ecl,
                                                   temp_buffer, qrcode);

fail:
    qrcode[0] = 0;  // Set size to invalid value for safety
    return false;
}

bool gui_qrcode_gen_encode_binary(uint8_t           data_and_temp[],
                                  size_t            data_len,
                                  uint8_t          *qrcode,
                                  T_QRCODEGEN_ECC   ecl,
                                  int               min_version,
                                  int               max_version,
                                  T_QRCODEGEN_MASK  mask,
                                  bool              boost_ecl)
{
    struct qrcodegen_Segment seg;

    seg.mode = QRCODEGEN_MODE_BYTE;
    seg.bit_length = gui_qrcode_gen_calc_segment_bit_length(seg.mode, data_len);

    if (seg.bit_length == LENGTH_OVERFLOW)
    {
        qrcode[0] = 0;  // Set size to invalid value for safety
        return false;
    }

    seg.num_chars = (int)data_len;
    seg.data = data_and_temp;

    return gui_qrcode_gen_encode_segments_advanced(&seg, 1, ecl, min_version, max_version, mask,
                                                   boost_ecl,
                                                   data_and_temp, qrcode);
}

// Appends the given number of low-order bits of the given value to the given byte-based
// bit buffer, increasing the bit length. Requires 0 <= num_bits <= 16 and val < 2^num_bits.
testable void gui_qrcode_gen_append_bits_to_buffer(unsigned int  val,
                                                   int           num_bits,
                                                   uint8_t       buffer[],
                                                   int          *bit_len)
{
    //assert(0 <= num_bits && num_bits <= 16 && (unsigned long)val >> num_bits == 0);
    for (int i = num_bits - 1; i >= 0; i--, (*bit_len)++)
    {
        buffer[*bit_len >> 3] |= ((val >> i) & 1) << (7 - (*bit_len & 7));
    }
}

/*---- Low-level QR Code encoding functions ----*/

// Public function - see documentation comment in header file.
bool gui_qrcode_gen_encode_segments(const struct qrcodegen_Segment  segs[],
                                    size_t                          len,
                                    T_QRCODEGEN_ECC                 ecl,
                                    uint8_t                         temp_buffer[],
                                    uint8_t                        *qrcode)
{
    return gui_qrcode_gen_encode_segments_advanced(segs, len, ecl,
                                                   QRCODEGEN_VERSION_MIN, QRCODEGEN_VERSION_MAX, QRCODEGEN_MASK_AUTO, true, temp_buffer, qrcode);
}

// Public function - see documentation comment in header file.
bool gui_qrcode_gen_encode_segments_advanced(const struct qrcodegen_Segment  segs[],
                                             size_t                          len,
                                             T_QRCODEGEN_ECC                 ecl,
                                             int                             min_version,
                                             int                             max_version,
                                             T_QRCODEGEN_MASK                mask,
                                             bool                            boost_ecl,
                                             uint8_t                         temp_buffer[],
                                             uint8_t                        *qrcode)
{
    //assert(segs != NULL || len == 0);
    //assert(QRCODEGEN_VERSION_MIN <= min_version && min_version <= max_version && max_version <= QRCODEGEN_VERSION_MAX);
    //assert(0 <= (int)ecl && (int)ecl <= 3 && -1 <= (int)mask && (int)mask <= 7);

    // Find the minimal version number to use
    int version, data_used_bits;
    int data_capacity_bits;
    int bit_len;
    int bit;
    int terminator_bits;
    long minPenalty;
    long penalty;

    for (version = min_version; ; version++)
    {
        data_capacity_bits = gui_qrcode_gen_get_num_data_codewords(version,
                                                                   ecl) * 8;  // Number of data bits available
        data_used_bits = gui_qrcode_gen_get_total_bits(segs, len, version);

        if (data_used_bits != LENGTH_OVERFLOW && data_used_bits <= data_capacity_bits)
        {
            break;    // This version number is found to be suitable
        }

        if (version >= max_version)
        {
            // All versions in the range could not fit the given data
            qrcode[0] = 0;  // Set size to invalid value for safety

            return false;
        }
    }
    //assert(data_used_bits != LENGTH_OVERFLOW);

    // Increase the error correction level while the data still fits in the current version number
    for (int i = (int)QRCODEGEN_ECC_MEDIUM; i <= (int)QRCODEGEN_ECC_HIGH; i++)
    {
        // From low to high
        if (boost_ecl
            && data_used_bits <= gui_qrcode_gen_get_num_data_codewords(version, (T_QRCODEGEN_ECC)i) * 8)
        {
            ecl = (T_QRCODEGEN_ECC)i;
        }
    }

    // Concatenate all segments to create the data bit string
    memset(qrcode, 0, (size_t)QRCODEGEN_BUFFER_LEN_FOR_VERSION(version) * sizeof(qrcode[0]));
    bit_len = 0;
    for (size_t i = 0; i < len; i++)
    {
        const struct qrcodegen_Segment *seg = &segs[i];
        gui_qrcode_gen_append_bits_to_buffer((unsigned int)seg->mode, 4, qrcode, &bit_len);
        gui_qrcode_gen_append_bits_to_buffer((unsigned int)seg->num_chars,
                                             gui_qrcode_gen_num_char_count_bits(seg->mode, version),
                                             qrcode,
                                             &bit_len);

        for (int j = 0; j < seg->bit_length; j++)
        {
            bit = (seg->data[j >> 3] >> (7 - (j & 7))) & 1;
            gui_qrcode_gen_append_bits_to_buffer((unsigned int)bit, 1, qrcode, &bit_len);
        }
    }
    //assert(bit_len == data_used_bits);

    // Add terminator and pad up to a byte if applicable
    data_capacity_bits = gui_qrcode_gen_get_num_data_codewords(version, ecl) * 8;
    //assert(bit_len <= data_capacity_bits);
    terminator_bits = data_capacity_bits - bit_len;

    if (terminator_bits > 4)
    {
        terminator_bits = 4;
    }

    gui_qrcode_gen_append_bits_to_buffer(0, terminator_bits, qrcode, &bit_len);
    gui_qrcode_gen_append_bits_to_buffer(0, (8 - bit_len % 8) % 8, qrcode, &bit_len);
    //assert(bit_len % 8 == 0);

    // Pad with alternating bytes until data capacity is reached
    for (uint8_t pad_byte = 0xEC; bit_len < data_capacity_bits; pad_byte ^= 0xEC ^ 0x11)
    {
        gui_qrcode_gen_append_bits_to_buffer(pad_byte, 8, qrcode, &bit_len);
    }

    // Compute ECC, draw modules
    gui_qrcode_gen_add_ecc_and_interleave(qrcode, version, ecl, temp_buffer);
    gui_qrcode_gen_initialize_function_modules(version, qrcode);
    gui_qrcode_gen_draw_codewords(temp_buffer, gui_qrcode_gen_get_num_raw_data_modules(version) / 8,
                                  qrcode);
    gui_qrcode_gen_draw_light_function_modules(qrcode, version);
    gui_qrcode_gen_initialize_function_modules(version, temp_buffer);

    // Do masking
    if (mask == QRCODEGEN_MASK_AUTO)
    {
        // Automatically choose best mask
        minPenalty = LONG_MAX;

        for (int i = 0; i < 8; i++)
        {
            T_QRCODEGEN_MASK msk = (T_QRCODEGEN_MASK)i;
            gui_qrcode_gen_apply_mask(temp_buffer, qrcode, msk);
            gui_qrcode_gen_draw_format_bits(ecl, msk, qrcode);
            penalty = gui_qrcode_gen_get_penalty_score(qrcode);

            if (penalty < minPenalty)
            {
                mask = msk;
                minPenalty = penalty;
            }

            gui_qrcode_gen_apply_mask(temp_buffer, qrcode, msk);  // Undoes the mask due to XOR
        }
    }
    //assert(0 <= (int)mask && (int)mask <= 7);
    gui_qrcode_gen_apply_mask(temp_buffer, qrcode, mask);  // Apply the final choice of mask
    gui_qrcode_gen_draw_format_bits(ecl, mask, qrcode);  // Overwrite old format bits

    return true;
}

/*---- Error correction code generation functions ----*/

// Appends error correction bytes to each block of the given data array, then interleaves
// bytes from the blocks and stores them in the result array. data[0 : data_len] contains
// the input data. data[data_len : raw_codewords] is used as a temporary work area and will
// be clobbered by this function. The final answer is stored in result[0 : raw_codewords].
testable void gui_qrcode_gen_add_ecc_and_interleave(uint8_t         data[],
                                                    int             version,
                                                    T_QRCODEGEN_ECC ecl,
                                                    uint8_t         result[])
{
    // Calculate parameter numbers
    //assert(0 <= (int)ecl && (int)ecl < 4 && QRCODEGEN_VERSION_MIN <= version && version <= QRCODEGEN_VERSION_MAX);
    int num_blocks = NUM_ERROR_CORRECTION_BLOCKS[(int)ecl][version];
    int block_ecc_len = ECC_CODEWORDS_PER_BLOCK  [(int)ecl][version];
    int raw_codewords = gui_qrcode_gen_get_num_raw_data_modules(version) / 8;
    int data_len = gui_qrcode_gen_get_num_data_codewords(version, ecl);
    int num_short_blocks = num_blocks - raw_codewords % num_blocks;
    int short_block_data_len = raw_codewords / num_blocks - block_ecc_len;
    int dat_len;
    uint8_t *ecc;
    uint8_t rsdiv[QRCODEGEN_REED_SOLOMON_DEGREE_MAX];
    const uint8_t *dat = data;

    // Split data into blocks, calculate ECC, and interleave
    // (not concatenate) the bytes into a single sequence
    gui_qrcode_gen_reed_solomon_compute_divisor(block_ecc_len, rsdiv);

    for (int i = 0; i < num_blocks; i++)
    {
        dat_len = short_block_data_len + (i < num_short_blocks ? 0 : 1);
        ecc = &data[data_len];  // Temporary storage
        gui_qrcode_gen_reed_solomon_compute_remainder(dat, dat_len, rsdiv, block_ecc_len, ecc);

        for (int j = 0, k = i; j < dat_len; j++, k += num_blocks)    // Copy data
        {
            if (j == short_block_data_len)
            {
                k -= num_short_blocks;
            }

            result[k] = dat[j];
        }

        for (int j = 0, k = data_len + i; j < block_ecc_len; j++, k += num_blocks)  // Copy ECC
        {
            result[k] = ecc[j];
        }

        dat += dat_len;
    }
}

// Returns the number of 8-bit codewords that can be used for storing data (not ECC),
// for the given version number and error correction level. The result is in the range [9, 2956].
testable int gui_qrcode_gen_get_num_data_codewords(int version, T_QRCODEGEN_ECC ecl) \
{
    int v = version, e = (int)ecl;
    //assert(0 <= e && e < 4);
    return gui_qrcode_gen_get_num_raw_data_modules(v) / 8
    - ECC_CODEWORDS_PER_BLOCK    [e][v]
    * NUM_ERROR_CORRECTION_BLOCKS[e][v];
}

// Returns the number of data bits that can be stored in a QR Code of the given version number, after
// all function modules are excluded. This includes remainder bits, so it might not be a multiple of 8.
// The result is in the range [208, 29648]. This could be implemented as a 40-entry lookup table.
testable int gui_qrcode_gen_get_num_raw_data_modules(int ver)
{
    //assert(QRCODEGEN_VERSION_MIN <= ver && ver <= QRCODEGEN_VERSION_MAX);
    int result = (16 * ver + 128) * ver + 64;
    int num_align;

    if (ver >= 2)
    {
        num_align = ver / 7 + 2;
        result -= (25 * num_align - 10) * num_align - 55;

        if (ver >= 7)
        {
            result -= 36;
        }
    }
    //assert(208 <= result && result <= 29648);
    return result;
}

/*---- Reed-Solomon ECC generator functions ----*/

// Computes a Reed-Solomon ECC generator polynomial for the given degree, storing in result[0 : degree].
// This could be implemented as a lookup table over all possible parameter values, instead of as an algorithm.
testable void gui_qrcode_gen_reed_solomon_compute_divisor(int degree, uint8_t result[])
{
    //assert(1 <= degree && degree <= QRCODEGEN_REED_SOLOMON_DEGREE_MAX);
    // Polynomial coefficients are stored from highest to lowest power, excluding the leading term which is always 1.
    // For example the polynomial x^3 + 255x^2 + 8x + 93 is stored as the uint8 array {255, 8, 93}.
    memset(result, 0, (size_t)degree * sizeof(result[0]));
    result[degree - 1] = 1;  // Start off with the monomial x^0

    // Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
    // drop the highest monomial term which is always 1x^degree.
    // Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
    uint8_t root = 1;

    for (int i = 0; i < degree; i++)
    {
        // Multiply the current product by (x - r^i)
        for (int j = 0; j < degree; j++)
        {
            result[j] = gui_qrcode_gen_reed_solomon_multiply(result[j], root);

            if (j + 1 < degree)
            {
                result[j] ^= result[j + 1];
            }
        }
        root = gui_qrcode_gen_reed_solomon_multiply(root, 0x02);
    }
}

// Computes the Reed-Solomon error correction codeword for the given data and divisor polynomials.
// The remainder when data[0 : data_len] is divided by divisor[0 : degree] is stored in result[0 : degree].
// All polynomials are in big endian, and the generator has an implicit leading 1 term.
testable void gui_qrcode_gen_reed_solomon_compute_remainder(const uint8_t data[],
                                                            int           data_len,
                                                            const uint8_t generator[],
                                                            int           degree,
                                                            uint8_t       result[])
{
    uint8_t factor;

    //assert(1 <= degree && degree <= QRCODEGEN_REED_SOLOMON_DEGREE_MAX);
    memset(result, 0, (size_t)degree * sizeof(result[0]));

    for (int i = 0; i < data_len; i++)    // Polynomial division
    {
        factor = data[i] ^ result[0];
        memmove(&result[0], &result[1], (size_t)(degree - 1) * sizeof(result[0]));
        result[degree - 1] = 0;

        for (int j = 0; j < degree; j++)
        {
            result[j] ^= gui_qrcode_gen_reed_solomon_multiply(generator[j], factor);
        }
    }
}

#undef QRCODEGEN_REED_SOLOMON_DEGREE_MAX

// Returns the product of the two given field elements modulo GF(2^8/0x11D).
// All inputs are valid. This could be implemented as a 256*256 lookup table.
testable uint8_t gui_qrcode_gen_reed_solomon_multiply(uint8_t x, uint8_t y)
{
    // Russian peasant multiplication
    uint8_t z = 0;

    for (int i = 7; i >= 0; i--)
    {
        z = (uint8_t)((z << 1) ^ ((z >> 7) * 0x11D));
        z ^= ((y >> i) & 1) * x;
    }

    return z;
}

/*---- Drawing function modules ----*/

// Clears the given QR Code grid with light modules for the given
// version's size, then marks every function module as dark.
testable void gui_qrcode_gen_initialize_function_modules(int version, uint8_t *qrcode)
{
    // Initialize QR Code
    int qrsize = version * 4 + 17;
    int num_align;
    uint8_t align_pat_pos[7];

    memset(qrcode, 0, (size_t)((qrsize * qrsize + 7) / 8 + 1) * sizeof(qrcode[0]));
    qrcode[0] = (uint8_t)qrsize;

    // Fill horizontal and vertical timing patterns
    gui_qrcode_gen_fill_rectangle(6, 0, 1, qrsize, qrcode);
    gui_qrcode_gen_fill_rectangle(0, 6, qrsize, 1, qrcode);

    // Fill 3 finder patterns (all corners except bottom right) and format bits
    gui_qrcode_gen_fill_rectangle(0, 0, 9, 9, qrcode);
    gui_qrcode_gen_fill_rectangle(qrsize - 8, 0, 8, 9, qrcode);
    gui_qrcode_gen_fill_rectangle(0, qrsize - 8, 9, 8, qrcode);

    // Fill numerous alignment patterns
    num_align = gui_qrcode_gen_get_alignment_pattern_positions(version, align_pat_pos);

    for (int i = 0; i < num_align; i++)
    {
        for (int j = 0; j < num_align; j++)
        {
            // Don't draw on the three finder corners
            if (!((i == 0 && j == 0) || (i == 0 && j == num_align - 1) || (i == num_align - 1 && j == 0)))
            {
                gui_qrcode_gen_fill_rectangle(align_pat_pos[i] - 2, align_pat_pos[j] - 2, 5, 5, qrcode);
            }
        }
    }

    // Fill version blocks
    if (version >= 7)
    {
        gui_qrcode_gen_fill_rectangle(qrsize - 11, 0, 3, 6, qrcode);
        gui_qrcode_gen_fill_rectangle(0, qrsize - 11, 6, 3, qrcode);
    }
}

static void __draw_numerous_alignment_patterns(uint8_t *qrcode, const uint8_t align_pat_pos[7],
                                               const int num_align)
{
    for (int i = 0; i < num_align; i++)
    {
        for (int j = 0; j < num_align; j++)
        {
            if ((i == 0 && j == 0) || (i == 0 && j == num_align - 1) || (i == num_align - 1 && j == 0))
            {
                continue;    // Don't draw on the three finder corners
            }

            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    gui_qrcode_gen_set_module_bounded(qrcode, align_pat_pos[i] + dx, align_pat_pos[j] + dy, dx == 0 &&
                                                      dy == 0);
                }
            }
        }
    }
}

static void __draw_version_blocks(uint8_t *qrcode, const int qrsize, const int version)
{
    // Calculate error correction code and pack bits
    int rem = version;  // version is uint6, in the range [7, 40]

    for (int i = 0; i < 12; i++)
    {
        rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
    }

    long bits = (long)version << 12 | rem;  // uint18
    //assert(bits >> 18 == 0);

    // Draw two copies
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            int k = qrsize - 11 + j;
            gui_qrcode_gen_set_module_bounded(qrcode, k, i, (bits & 1) != 0);
            gui_qrcode_gen_set_module_bounded(qrcode, i, k, (bits & 1) != 0);
            bits >>= 1;
        }
    }
}
// Draws light function modules and possibly some dark modules onto the given QR Code, without changing
// non-function modules. This does not draw the format bits. This requires all function modules to be previously
// marked dark (namely by gui_qrcode_gen_initialize_function_modules()), because this may skip redrawing dark function modules.
static void gui_qrcode_gen_draw_light_function_modules(uint8_t *qrcode, int version)
{
    // Draw horizontal and vertical timing patterns
    int qrsize = gui_qrcode_gen_get_size(qrcode);
    uint8_t align_pat_pos[7];
    int num_align;
    int dist;

    for (int i = 7; i < qrsize - 7; i += 2)
    {
        gui_qrcode_gen_set_module_bounded(qrcode, 6, i, false);
        gui_qrcode_gen_set_module_bounded(qrcode, i, 6, false);
    }

    // Draw 3 finder patterns (all corners except bottom right; overwrites some timing modules)
    for (int dy = -4; dy <= 4; dy++)
    {
        for (int dx = -4; dx <= 4; dx++)
        {
            dist = abs(dx);

            if (abs(dy) > dist)
            {
                dist = abs(dy);
            }

            if (dist == 2 || dist == 4)
            {
                gui_qrcode_gen_set_module_unbounded(qrcode, 3 + dx, 3 + dy, false);
                gui_qrcode_gen_set_module_unbounded(qrcode, qrsize - 4 + dx, 3 + dy, false);
                gui_qrcode_gen_set_module_unbounded(qrcode, 3 + dx, qrsize - 4 + dy, false);
            }
        }
    }

    // Draw numerous alignment patterns
    num_align = gui_qrcode_gen_get_alignment_pattern_positions(version, align_pat_pos);
    __draw_numerous_alignment_patterns(qrcode, align_pat_pos, num_align);

    // Draw version blocks
    if (version >= 7)
    {
        __draw_version_blocks(qrcode, qrsize, version);
    }
}

// Draws two copies of the format bits (with its own error correction code) based
// on the given mask and error correction level. This always draws all modules of
// the format bits, unlike gui_qrcode_gen_draw_light_function_modules() which might skip dark modules.
static void gui_qrcode_gen_draw_format_bits(T_QRCODEGEN_ECC   ecl,
                                            T_QRCODEGEN_MASK  mask,
                                            uint8_t          *qrcode)
{
    // Calculate error correction code and pack bits
    //assert(0 <= (int)mask && (int)mask <= 7);
    static const int table[] = {1, 0, 3, 2};
    int data = table[(int)ecl] << 3 | (int)mask;  // errCorrLvl is uint2, mask is uint3
    int rem = data;
    int bits;
    int qrsize;

    for (int i = 0; i < 10; i++)
    {
        rem = (rem << 1) ^ ((rem >> 9) * 0x537);
    }

    bits = (data << 10 | rem) ^ 0x5412;  // uint15
    //assert(bits >> 15 == 0);

    // Draw first copy
    for (int i = 0; i <= 5; i++)
    {
        gui_qrcode_gen_set_module_bounded(qrcode, 8, i, gui_qrcode_gen_get_bit(bits, i));
    }

    gui_qrcode_gen_set_module_bounded(qrcode, 8, 7, gui_qrcode_gen_get_bit(bits, 6));
    gui_qrcode_gen_set_module_bounded(qrcode, 8, 8, gui_qrcode_gen_get_bit(bits, 7));
    gui_qrcode_gen_set_module_bounded(qrcode, 7, 8, gui_qrcode_gen_get_bit(bits, 8));

    for (int i = 9; i < 15; i++)
    {
        gui_qrcode_gen_set_module_bounded(qrcode, 14 - i, 8, gui_qrcode_gen_get_bit(bits, i));
    }

    // Draw second copy
    qrsize = gui_qrcode_gen_get_size(qrcode);

    for (int i = 0; i < 8; i++)
    {
        gui_qrcode_gen_set_module_bounded(qrcode, qrsize - 1 - i, 8, gui_qrcode_gen_get_bit(bits, i));
    }

    for (int i = 8; i < 15; i++)
    {
        gui_qrcode_gen_set_module_bounded(qrcode, 8, qrsize - 15 + i, gui_qrcode_gen_get_bit(bits, i));
    }

    gui_qrcode_gen_set_module_bounded(qrcode, 8, qrsize - 8, true);  // Always dark
}

// Calculates and stores an ascending list of positions of alignment patterns
// for this version number, returning the length of the list (in the range [0,7]).
// Each position is in the range [0,177), and are used on both the x and y axes.
// This could be implemented as lookup table of 40 variable-length lists of unsigned bytes.
testable int gui_qrcode_gen_get_alignment_pattern_positions(int version, uint8_t result[7])
{
    int num_align;
    int step;

    if (version == 1)
    {
        return 0;
    }

    num_align = version / 7 + 2;
    step = (version == 32) ? 26 :
           (version * 4 + num_align * 2 + 1) / (num_align * 2 - 2) * 2;

    for (int i = num_align - 1, pos = version * 4 + 10; i >= 1; i--, pos -= step)
    {
        result[i] = (uint8_t)pos;
    }

    result[0] = 6;
    return num_align;
}

// Sets every module in the range [left : left + width] * [top : top + height] to dark.
static void gui_qrcode_gen_fill_rectangle(int left, int top, int width, int height,
                                          uint8_t *qrcode)
{
    for (int dy = 0; dy < height; dy++)
    {
        for (int dx = 0; dx < width; dx++)
        {
            gui_qrcode_gen_set_module_bounded(qrcode, left + dx, top + dy, true);
        }
    }
}

/*---- Drawing data modules and masking ----*/

// Draws the raw codewords (including data and ECC) onto the given QR Code. This requires the initial state of
// the QR Code to be dark at function modules and light at codeword modules (including unused remainder bits).
static void gui_qrcode_gen_draw_codewords(const uint8_t data[], int data_len, uint8_t *qrcode)
{
    int qrsize = gui_qrcode_gen_get_size(qrcode);
    int i = 0;  // Bit index into the data
    int x, y;
    bool upward;
    bool dark;

    // Do the funny zigzag scan
    for (int right = qrsize - 1; right >= 1;
         right -= 2)    // Index of right column in each column pair
    {
        if (right == 6)
        {
            right = 5;
        }

        for (int vert = 0; vert < qrsize; vert++)    // Vertical counter
        {
            for (int j = 0; j < 2; j++)
            {
                x = right - j;  // Actual x coordinate
                upward = ((right + 1) & 2) == 0;
                y = upward ? qrsize - 1 - vert : vert;  // Actual y coordinate

                if (!gui_qrcode_gen_get_module_bounded(qrcode, x, y) && i < data_len * 8)
                {
                    dark = gui_qrcode_gen_get_bit(data[i >> 3], 7 - (i & 7));
                    gui_qrcode_gen_set_module_bounded(qrcode, x, y, dark);
                    i++;
                }
                // If this QR Code has any remainder bits (0 to 7), they were assigned as
                // 0/false/light by the constructor and are left unchanged by this method
            }
        }
    }
    //assert(i == data_len * 8);
}

// XORs the codeword modules in this QR Code with the given mask pattern
// and given pattern of function modules. The codeword bits must be drawn
// before masking. Due to the arithmetic of XOR, calling gui_qrcode_gen_apply_mask() with
// the same mask value a second time will undo the mask. A final well-formed
// QR Code needs exactly one (not zero, two, etc.) mask applied.
static void gui_qrcode_gen_apply_mask(const uint8_t      function_modules[],
                                      uint8_t           *qrcode,
                                      T_QRCODEGEN_MASK   mask)
{
    //assert(0 <= (int)mask && (int)mask <= 7);  // Disallows QRCODEGEN_MASK_AUTO
    int qrsize = gui_qrcode_gen_get_size(qrcode);
    bool invert = 0;
    bool val;

    for (int y = 0; y < qrsize; y++)
    {
        for (int x = 0; x < qrsize; x++)
        {
            if (gui_qrcode_gen_get_module_bounded(function_modules, x, y))
            {
                continue;
            }
            switch ((int)mask)
            {
            case 0:
                {
                    invert = (x + y) % 2 == 0;
                }
                break;

            case 1:
                {
                    invert = y % 2 == 0;
                }
                break;

            case 2:
                {
                    invert = x % 3 == 0;
                }
                break;

            case 3:
                {
                    invert = (x + y) % 3 == 0;
                }
                break;

            case 4:
                {
                    invert = (x / 3 + y / 2) % 2 == 0;
                }
                break;

            case 5:
                {
                    invert = x * y % 2 + x * y % 3 == 0;
                }
                break;

            case 6:
                {
                    invert = (x * y % 2 + x * y % 3) % 2 == 0;
                }
                break;

            case 7:
                {
                    invert = ((x + y) % 2 + x * y % 3) % 2 == 0;
                }
                break;

            //default:  //assert(false);  return;
            default:
                break;
            }

            val = gui_qrcode_gen_get_module_bounded(qrcode, x, y);
            gui_qrcode_gen_set_module_bounded(qrcode, x, y, val ^ invert);
        }
    }
}

static void __calculate_penalty_rows(const uint8_t *qrcode, const int qrsize, long *result)
{
    int run_history[7] = {0};
    for (int y = 0; y < qrsize; y++)
    {
        bool run_color = false;
        int run_x = 0;

        memset(run_history, 0, sizeof(run_history));

        for (int x = 0; x < qrsize; x++)
        {
            if (gui_qrcode_gen_get_module_bounded(qrcode, x, y) == run_color)
            {
                run_x++;

                if (run_x == 5)
                {
                    result += PENALTY_N1;
                }
                else if (run_x > 5)
                {
                    result++;
                }
            }
            else
            {
                gui_qrcode_gen_finder_penalty_add_history(run_x, run_history, qrsize);

                if (!run_color)
                {
                    result += gui_qrcode_gen_finder_penalty_count_patterns(run_history, qrsize) * PENALTY_N3;
                }

                run_color = gui_qrcode_gen_get_module_bounded(qrcode, x, y);
                run_x = 1;
            }
        }

        result += gui_qrcode_gen_finder_penalty_terminate_and_count(run_color, run_x, run_history,
                                                                    qrsize) * PENALTY_N3;
    }
}

static void __calculate_penalty_cols(const uint8_t *qrcode, const int qrsize, long *result)
{
    int run_history[7] = {0};
    for (int x = 0; x < qrsize; x++)
    {
        bool run_color = false;
        int run_y = 0;

        memset(run_history, 0, sizeof(run_history));

        for (int y = 0; y < qrsize; y++)
        {
            if (gui_qrcode_gen_get_module_bounded(qrcode, x, y) == run_color)
            {
                run_y++;

                if (run_y == 5)
                {
                    result += PENALTY_N1;
                }
                else if (run_y > 5)
                {
                    result++;
                }
            }
            else
            {
                gui_qrcode_gen_finder_penalty_add_history(run_y, run_history, qrsize);

                if (!run_color)
                {
                    result += gui_qrcode_gen_finder_penalty_count_patterns(run_history, qrsize) * PENALTY_N3;
                }

                run_color = gui_qrcode_gen_get_module_bounded(qrcode, x, y);
                run_y = 1;
            }
        }

        result += gui_qrcode_gen_finder_penalty_terminate_and_count(run_color, run_y, run_history,
                                                                    qrsize) * PENALTY_N3;
    }
}

// Calculates and returns the penalty score based on state of the given QR Code's current modules.
// This is used by the automatic mask choice algorithm to find the mask pattern that yields the lowest score.
static long gui_qrcode_gen_get_penalty_score(const uint8_t *qrcode)
{
    int qrsize = gui_qrcode_gen_get_size(qrcode);
    long result = 0;
    bool color;
    int total;
    int dark;
    int k;

    // Adjacent modules in row&column having same color, and finder-like patterns
    __calculate_penalty_rows(qrcode, qrsize, &result);
    __calculate_penalty_cols(qrcode, qrsize, &result);

    // 2*2 blocks of modules having same color
    for (int y = 0; y < qrsize - 1; y++)
    {
        for (int x = 0; x < qrsize - 1; x++)
        {
            color = gui_qrcode_gen_get_module_bounded(qrcode, x, y);

            if ((color == gui_qrcode_gen_get_module_bounded(qrcode, x + 1, y))
                && (color == gui_qrcode_gen_get_module_bounded(qrcode, x, y + 1))
                && (color == gui_qrcode_gen_get_module_bounded(qrcode, x + 1, y + 1)))
            {
                result += PENALTY_N2;
            }
        }
    }

    // Balance of dark and light modules
    dark = 0;
    for (int y = 0; y < qrsize; y++)
    {
        for (int x = 0; x < qrsize; x++)
        {
            if (gui_qrcode_gen_get_module_bounded(qrcode, x, y))
            {
                dark++;
            }
        }
    }
    total = qrsize * qrsize;  // Note that size is odd, so dark/total != 1/2
    // Compute the smallest integer k >= 0 such that (45-5k)% <= dark/total <= (55+5k)%
    k = (int)((labs(dark * 20L - total * 10L) + total - 1) / total) - 1;
    //assert(0 <= k && k <= 9);
    result += k * PENALTY_N4;
    //assert(0 <= result && result <= 2568888L);  // Non-tight upper bound based on default values of PENALTY_N1, ..., N4
    return result;
}

// Can only be called immediately after a light run is added, and
// returns either 0, 1, or 2. A helper function for gui_qrcode_gen_get_penalty_score().
static int gui_qrcode_gen_finder_penalty_count_patterns(const int run_history[7], int qrsize)
{
    (void)qrsize;
    int n = run_history[1];
    //assert(n <= qrsize * 3);  (void)qrsize;
    bool core = n > 0 && run_history[2] == n && run_history[3] == n * 3 && run_history[4] == n &&
                run_history[5] == n;

    // The maximum QR Code size is 177, hence the dark run length n <= 177.
    // Arithmetic is promoted to int, so n*4 will not overflow.
    return (core && run_history[0] >= n * 4 && run_history[6] >= n ? 1 : 0)
           + (core && run_history[6] >= n * 4 && run_history[0] >= n ? 1 : 0);
}

// Must be called at the end of a line (row or column) of modules. A helper function for gui_qrcode_gen_get_penalty_score().
static int gui_qrcode_gen_finder_penalty_terminate_and_count(bool current_run_color,
                                                             int  current_run_length,
                                                             int  run_history[7],
                                                             int  qrsize)
{
    if (current_run_color)    // Terminate dark run
    {
        gui_qrcode_gen_finder_penalty_add_history(current_run_length, run_history, qrsize);
        current_run_length = 0;
    }

    current_run_length += qrsize;  // Add light border to final run
    gui_qrcode_gen_finder_penalty_add_history(current_run_length, run_history, qrsize);

    return gui_qrcode_gen_finder_penalty_count_patterns(run_history, qrsize);
}

// Pushes the given value to the front and drops the last value. A helper function for gui_qrcode_gen_get_penalty_score().
static void gui_qrcode_gen_finder_penalty_add_history(int current_run_length, int run_history[7],
                                                      int qrsize)
{
    if (run_history[0] == 0)
    {
        current_run_length += qrsize;    // Add light border to initial run
    }

    memmove(&run_history[1], &run_history[0], 6 * sizeof(run_history[0]));
    run_history[0] = current_run_length;
}

/*---- Basic QR Code information ----*/

// Public function - see documentation comment in header file.
int gui_qrcode_gen_get_size(const uint8_t *qrcode)
{
    GUI_ASSERT(qrcode != NULL);
    int result = (int) * (qrcode);
//    assert((QRCODEGEN_VERSION_MIN * 4 + 17) <= result
//      && result <= (QRCODEGEN_VERSION_MAX * 4 + 17));

    return result;
}

// Public function - see documentation comment in header file.
bool gui_qrcode_gen_get_module(const uint8_t *qrcode, int x, int y)
{
    //assert(qrcode != NULL);
    int qrsize = (int) * (qrcode);

    return (0 <= x && x < qrsize && 0 <= y && y < qrsize) &&
           gui_qrcode_gen_get_module_bounded(qrcode, x, y);
}

// Returns the color of the module at the given coordinates, which must be in bounds.
testable bool gui_qrcode_gen_get_module_bounded(const uint8_t *qrcode, int x, int y)
{
    int qrsize = (int) * (qrcode);
    //assert(21 <= qrsize && qrsize <= 177 && 0 <= x && x < qrsize && 0 <= y && y < qrsize);
    int index = y * qrsize + x;

    return gui_qrcode_gen_get_bit(qrcode[(index >> 3) + 1], index & 7);
}

// Sets the color of the module at the given coordinates, which must be in bounds.
testable void gui_qrcode_gen_set_module_bounded(uint8_t *qrcode, int x, int y, bool is_dark)
{
    int qrsize = (int) * (qrcode);
    //assert(21 <= qrsize && qrsize <= 177 && 0 <= x && x < qrsize && 0 <= y && y < qrsize);
    int index = y * qrsize + x;
    int bit_index = index & 7;
    int byte_index = (index >> 3) + 1;

    if (is_dark)
    {
        qrcode[byte_index] |= 1 << bit_index;
    }
    else
    {
        qrcode[byte_index] &= (1 << bit_index) ^ 0xFF;
    }
}

// Sets the color of the module at the given coordinates, doing nothing if out of bounds.
testable void gui_qrcode_gen_set_module_unbounded(uint8_t *qrcode, int x, int y, bool is_dark)
{
    int qrsize = (int) * (qrcode);

    if (0 <= x && x < qrsize && 0 <= y && y < qrsize)
    {
        gui_qrcode_gen_set_module_bounded(qrcode, x, y, is_dark);
    }
}

// Returns true iff the i'th bit of x is set to 1. Requires x >= 0 and 0 <= i <= 14.
static bool gui_qrcode_gen_get_bit(int x, int i)
{
    return ((x >> i) & 1) != 0;
}

/*---- Segment handling ----*/

// Public function - see documentation comment in header file.
bool gui_qrcode_gen_is_numeric(const char *text)
{
    //assert(text != NULL);
    for (; *text != '\0'; text++)
    {
        if (*text < '0' || *text > '9')
        {
            return false;
        }
    }
    return true;
}

// Public function - see documentation comment in header file.
bool gui_qrcode_gen_is_alphanumeric(const char *text)
{
    //assert(text != NULL);
    for (; *text != '\0'; text++)
    {
        if (strchr(ALPHANUMERIC_CHARSET, *text) == NULL)
        {
            return false;
        }
    }
    return true;
}

// Public function - see documentation comment in header file.
size_t gui_qrcode_gen_calc_segment_buffer_size(T_QRCODEGEN_MODE mode, size_t num_chars)
{
    int temp = gui_qrcode_gen_calc_segment_bit_length(mode, num_chars);

    if (temp == LENGTH_OVERFLOW)
    {
        return SIZE_MAX;
    }

    //assert(0 <= temp && temp <= INT16_MAX);
    return ((size_t)temp + 7) / 8;
}

// Returns the number of data bits needed to represent a segment
// containing the given number of characters using the given mode. Notes:
// - Returns LENGTH_OVERFLOW on failure, i.e. num_chars > INT16_MAX
//   or the number of needed bits exceeds INT16_MAX (i.e. 32767).
// - Otherwise, all valid results are in the range [0, INT16_MAX].
// - For byte mode, num_chars measures the number of bytes, not Unicode code points.
// - For ECI mode, num_chars must be 0, and the worst-case number of bits is returned.
//   An actual ECI segment can have shorter data. For non-ECI modes, the result is exact.
testable int gui_qrcode_gen_calc_segment_bit_length(T_QRCODEGEN_MODE mode, size_t num_chars)
{
    long result;

    // All calculations are designed to avoid overflow on all platforms
    if (num_chars > (unsigned int)INT16_MAX)
    {
        return LENGTH_OVERFLOW;
    }

    result = (long)num_chars;

    if (mode == QRCODEGEN_MODE_NUMERIC)
    {
        result = (result * 10 + 2) / 3;    // ceil(10/3 * n)
    }
    else if (mode == QRCODEGEN_MODE_ALPHANUMERIC)
    {
        result = (result * 11 + 1) / 2;    // ceil(11/2 * n)
    }
    else if (mode == QRCODEGEN_MODE_BYTE)
    {
        result *= 8;
    }
    else if (mode == QRCODEGEN_MODE_KANJI)
    {
        result *= 13;
    }
    else if (mode == QRCODEGEN_MODE_ECI && num_chars == 0)
    {
        result = 3 * 8;
    }
    else    // Invalid argument
    {
        //assert(false);
        return LENGTH_OVERFLOW;
    }

    //assert(result >= 0);
    if (result > INT16_MAX)
    {
        return LENGTH_OVERFLOW;
    }

    return (int)result;
}

// Public function - see documentation comment in header file.
struct qrcodegen_Segment gui_qrcode_gen_make_bytes(const uint8_t data[], size_t len, uint8_t buf[])
{
    //assert(data != NULL || len == 0);
    struct qrcodegen_Segment result;

    result.mode = QRCODEGEN_MODE_BYTE;
    result.bit_length = gui_qrcode_gen_calc_segment_bit_length(result.mode, len);
    //assert(result.bit_length != LENGTH_OVERFLOW);
    result.num_chars = (int)len;

    if (len > 0)
    {
        memcpy(buf, data, len * sizeof(buf[0]));
    }

    result.data = buf;

    return result;
}

// Public function - see documentation comment in header file.
struct qrcodegen_Segment gui_qrcode_gen_make_numeric(const char *digits, uint8_t buf[])
{
    //assert(digits != NULL);
    struct qrcodegen_Segment result;
    size_t len = strlen(digits);
    int bit_len;
    int accum_count;
    unsigned int accum_data;
    char c;

    result.mode = QRCODEGEN_MODE_NUMERIC;
    bit_len = gui_qrcode_gen_calc_segment_bit_length(result.mode, len);
    //assert(bit_len != LENGTH_OVERFLOW);
    result.num_chars = (int)len;

    if (bit_len > 0)
    {
        memset(buf, 0, ((size_t)bit_len + 7) / 8 * sizeof(buf[0]));
    }

    result.bit_length = 0;

    accum_data = 0;
    accum_count = 0;

    for (; *digits != '\0'; digits++)
    {
        c = *digits;
        //assert('0' <= c && c <= '9');
        accum_data = accum_data * 10 + (unsigned int)(c - '0');
        accum_count++;

        if (accum_count == 3)
        {
            gui_qrcode_gen_append_bits_to_buffer(accum_data, 10, buf, &result.bit_length);
            accum_data = 0;
            accum_count = 0;
        }
    }

    if (accum_count > 0)  // 1 or 2 digits remaining
    {
        gui_qrcode_gen_append_bits_to_buffer(accum_data, accum_count * 3 + 1, buf, &result.bit_length);
    }

    //assert(result.bit_length == bit_len);
    result.data = buf;
    return result;
}

// Public function - see documentation comment in header file.
struct qrcodegen_Segment gui_qrcode_gen_make_alphanumeric(const char *text, uint8_t buf[])
{
    //assert(text != NULL);
    struct qrcodegen_Segment result;
    size_t len = strlen(text);
    int bit_len;
    int accum_count;
    unsigned int accum_data;

    result.mode = QRCODEGEN_MODE_ALPHANUMERIC;
    bit_len = gui_qrcode_gen_calc_segment_bit_length(result.mode, len);
    //assert(bit_len != LENGTH_OVERFLOW);
    result.num_chars = (int)len;

    if (bit_len > 0)
    {
        memset(buf, 0, ((size_t)bit_len + 7) / 8 * sizeof(buf[0]));
    }

    result.bit_length = 0;

    accum_data = 0;
    accum_count = 0;

    for (; *text != '\0'; text++)
    {
        const char *temp = strchr(ALPHANUMERIC_CHARSET, *text);
        //assert(temp != NULL);
        accum_data = accum_data * 45 + (unsigned int)(temp - ALPHANUMERIC_CHARSET);

        accum_count++;

        if (accum_count == 2)
        {
            gui_qrcode_gen_append_bits_to_buffer(accum_data, 11, buf, &result.bit_length);
            accum_data = 0;
            accum_count = 0;
        }
    }

    if (accum_count > 0)  // 1 character remaining
    {
        gui_qrcode_gen_append_bits_to_buffer(accum_data, 6, buf, &result.bit_length);
    }

    //assert(result.bit_length == bit_len);
    result.data = buf;
    return result;
}

// Public function - see documentation comment in header file.
struct qrcodegen_Segment gui_qrcode_gen_make_eci(long assign_val, uint8_t buf[])
{
    struct qrcodegen_Segment result;

    result.mode = QRCODEGEN_MODE_ECI;
    result.num_chars = 0;
    result.bit_length = 0;

    if (assign_val < 0)
        //assert(false);
        ;
    else if (assign_val < (1 << 7))
    {
        memset(buf, 0, 1 * sizeof(buf[0]));
        gui_qrcode_gen_append_bits_to_buffer((unsigned int)assign_val, 8, buf, &result.bit_length);
    }
    else if (assign_val < (1 << 14))
    {
        memset(buf, 0, 2 * sizeof(buf[0]));
        gui_qrcode_gen_append_bits_to_buffer(2, 2, buf, &result.bit_length);
        gui_qrcode_gen_append_bits_to_buffer((unsigned int)assign_val, 14, buf, &result.bit_length);
    }
    else if (assign_val < 1000000L)
    {
        memset(buf, 0, 3 * sizeof(buf[0]));
        gui_qrcode_gen_append_bits_to_buffer(6, 3, buf, &result.bit_length);
        gui_qrcode_gen_append_bits_to_buffer((unsigned int)(assign_val >> 10), 11, buf, &result.bit_length);
        gui_qrcode_gen_append_bits_to_buffer((unsigned int)(assign_val & 0x3FF), 10, buf,
                                             &result.bit_length);
    }
    else
    {

    }
    //assert(false);
    result.data = buf;
    return result;
}

// Calculates the number of bits needed to encode the given segments at the given version.
// Returns a non-negative number if successful. Otherwise returns LENGTH_OVERFLOW if a segment
// has too many characters to fit its length field, or the total bits exceeds INT16_MAX.
testable int gui_qrcode_gen_get_total_bits(const struct qrcodegen_Segment segs[], size_t len,
                                           int version)
{
    //assert(segs != NULL || len == 0);
    long result = 0;
    int num_chars;
    int bit_length;
    int ccbits;

    for (size_t i = 0; i < len; i++)
    {
        num_chars  = segs[i].num_chars;
        bit_length = segs[i].bit_length;
        //assert(0 <= num_chars  && num_chars  <= INT16_MAX);
        //assert(0 <= bit_length && bit_length <= INT16_MAX);
        ccbits = gui_qrcode_gen_num_char_count_bits(segs[i].mode, version);
        //assert(0 <= ccbits && ccbits <= 16);

        if (num_chars >= (1L << ccbits))
        {
            return LENGTH_OVERFLOW;    // The segment's length doesn't fit the field's bit width
        }

        result += 4L + ccbits + bit_length;

        if (result > INT16_MAX)
        {
            return LENGTH_OVERFLOW;    // The sum might overflow an int type
        }
    }
    //assert(0 <= result && result <= INT16_MAX);
    return (int)result;
}

// Returns the bit width of the character count field for a segment in the given mode
// in a QR Code at the given version number. The result is in the range [0, 16].
static int gui_qrcode_gen_num_char_count_bits(T_QRCODEGEN_MODE mode, int version)
{
    //assert(QRCODEGEN_VERSION_MIN <= version && version <= QRCODEGEN_VERSION_MAX);
    int i = (version + 7) / 17;
    static int temp[3];

    switch (mode)
    {
    case QRCODEGEN_MODE_NUMERIC:
        {
            temp[0] = 10;
            temp[1] = 12;
            temp[2] = 14;
        }
        break;

    case QRCODEGEN_MODE_ALPHANUMERIC:
        {
            temp[0] = 9;
            temp[1] = 11;
            temp[2] = 13;
        }
        break;

    case QRCODEGEN_MODE_BYTE:
        {
            temp[0] = 8;
            temp[1] = 16;
            temp[2] = 16;
        }
        break;

    case QRCODEGEN_MODE_KANJI:
        {
            temp[0] = 8;
            temp[1] = 10;
            temp[2] = 12;
        }
        break;

    case QRCODEGEN_MODE_ECI:
        {
            temp[0] = 0;
            temp[1] = 0;
            temp[2] = 0;
        }
        break;

    //default:  assert(false);  return -1;  // Dummy value
    default:
        {
            temp[0] = -1;
            temp[1] = -1;
            temp[2] = -1;
        }
        break;
    }

    return temp[i];
}

uint8_t *gui_qrcode_gen_get_data(unsigned char *data, uint16_t data_length, uint8_t flag)
{
    T_QRCODEGEN_ECC err_cor_lvl = QRCODEGEN_ECC_HIGH;  // Error correction level
    uint8_t *temp_buffer = gui_malloc(QRCODEGEN_BUFFER_LEN_MAX);
    bool gen_qbcode_ret;

    app_qrcode.gen_version_min = QRCODEGEN_VERSION_MIN;
    app_qrcode.gen_version_max = QRCODEGEN_VERSION_MAX;

    if (flag == 0)//text
    {
        gen_qbcode_ret = gui_qrcode_gen_encode_text((char *)data, temp_buffer, app_qrcode.qrcode,
                                                    err_cor_lvl,
                                                    app_qrcode.gen_version_min, app_qrcode.gen_version_max,
                                                    QRCODEGEN_MASK_AUTO, true);

        //gui_qrcode_gen_log("qrcode size %d", gui_qrcode_gen_get_size(app_qrcode.qrcode));
        if (gen_qbcode_ret)
        {

        }
        else
        {
            gui_free(temp_buffer);
            return NULL;
        }
    }
    else if (flag == 1)//binary
    {
        memcpy(temp_buffer, data, QRCODEGEN_BUFFER_LEN_MAX);
        gen_qbcode_ret = gui_qrcode_gen_encode_binary(temp_buffer, data_length, app_qrcode.qrcode,
                                                      err_cor_lvl,
                                                      app_qrcode.gen_version_min, app_qrcode.gen_version_max, QRCODEGEN_MASK_AUTO, true);

        if (gen_qbcode_ret)
        {
        }
        else
        {
            gui_free(temp_buffer);
            return NULL;
        }
    }

    gui_free(temp_buffer);
    return app_qrcode.qrcode;
}

#undef LENGTH_OVERFLOW
