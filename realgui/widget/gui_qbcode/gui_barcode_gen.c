/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <assert.h>
#include <stdio.h>
#include "gui_barcode_gen.h"
#include "gui_api.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/

#define C128_MAX                        160

#define C128_LATCHA                     'A'
#define C128_LATCHB                     'B'
#define C128_LATCHC                     'C'
#define C128_SHIFTA                     'a'
#define C128_SHIFTB                     'b'
#define C128_ABORC                      '9'
#define C128_AORB                       'Z'

#define BARCODE_CODE11                   1           /* Code 11 */
#define BARCODE_C25STANDARD              2           /* 2 of 5 Standard (Matrix) */
#define BARCODE_C25MATRIX                2           /* Legacy */
#define BARCODE_C25INTER                 3           /* 2 of 5 Interleaved */
#define BARCODE_C25IATA                  4           /* 2 of 5 IATA */
#define BARCODE_C25LOGIC                 6           /* 2 of 5 Data Logic */
#define BARCODE_C25IND                   7           /* 2 of 5 Industrial */
#define BARCODE_CODE39                   8           /* Code 39 */
#define BARCODE_EXCODE39                 9           /* Extended Code 39 */
#define BARCODE_EANX                     13          /* EAN (European Article Number) */
#define BARCODE_EANX_CHK                 14          /* EAN + Check Digit */
#define BARCODE_GS1_128                  16          /* GS1-128 */
#define BARCODE_EAN128                   16          /* Legacy */
#define BARCODE_CODABAR                  18          /* Codabar */
#define BARCODE_CODE128                  20          /* Code 128 */
#define BARCODE_DPLEIT                   21          /* Deutsche Post Leitcode */
#define BARCODE_DPIDENT                  22          /* Deutsche Post Identcode */
#define BARCODE_CODE16K                  23          /* Code 16k */
#define BARCODE_CODE49                   24          /* Code 49 */
#define BARCODE_CODE93                   25          /* Code 93 */
#define BARCODE_FLAT                     28          /* Flattermarken */
#define BARCODE_DBAR_OMN                 29          /* GS1 DataBar Omnidirectional */
#define BARCODE_RSS14                    29          /* Legacy */
#define BARCODE_DBAR_LTD                 30          /* GS1 DataBar Limited */
#define BARCODE_RSS_LTD                  30          /* Legacy */
#define BARCODE_DBAR_EXP                 31          /* GS1 DataBar Expanded */
#define BARCODE_RSS_EXP                  31          /* Legacy */
#define BARCODE_TELEPEN                  32          /* Telepen Alpha */
#define BARCODE_UPCA                     34          /* UPC-A */
#define BARCODE_UPCA_CHK                 35          /* UPC-A + Check Digit */
#define BARCODE_UPCE                     37          /* UPC-E */
#define BARCODE_UPCE_CHK                 38          /* UPC-E + Check Digit */
#define BARCODE_POSTNET                  40          /* USPS (U.S. Postal Service) POSTNET */
#define BARCODE_MSI_PLESSEY              47          /* MSI Plessey */
#define BARCODE_FIM                      49          /* Facing Identification Mark */
#define BARCODE_LOGMARS                  50          /* LOGMARS */
#define BARCODE_PHARMA                   51          /* Pharmacode One-Track */
#define BARCODE_PZN                      52          /* Pharmazentralnummer */
#define BARCODE_PHARMA_TWO               53          /* Pharmacode Two-Track */
#define BARCODE_CEPNET                   54          /* Brazilian CEPNet Postal Code */
#define BARCODE_PDF417                   55          /* PDF417 */
#define BARCODE_PDF417COMP               56          /* Compact PDF417 (Truncated PDF417) */
#define BARCODE_PDF417TRUNC              56          /* Legacy */
#define BARCODE_MAXICODE                 57          /* MaxiCode */
#define BARCODE_QRCODE                   58          /* QR Code */
#define BARCODE_CODE128AB                60          /* Code 128 (Suppress Code Set C) */
#define BARCODE_CODE128B                 60          /* Legacy */
#define BARCODE_AUSPOST                  63          /* Australia Post Standard Customer */
#define BARCODE_AUSREPLY                 66          /* Australia Post Reply Paid */
#define BARCODE_AUSROUTE                 67          /* Australia Post Routing */
#define BARCODE_AUSREDIRECT              68          /* Australia Post Redirection */
#define BARCODE_ISBNX                    69          /* ISBN */
#define BARCODE_RM4SCC                   70          /* Royal Mail 4-State Customer Code */
#define BARCODE_DATAMATRIX               71          /* Data Matrix (ECC200) */
#define BARCODE_EAN14                    72          /* EAN-14 */
#define BARCODE_VIN                      73          /* Vehicle Identification Number */
#define BARCODE_CODABLOCKF               74          /* Codablock-F */
#define BARCODE_NVE18                    75          /* NVE-18 (SSCC-18) */
#define BARCODE_JAPANPOST                76          /* Japanese Postal Code */
#define BARCODE_KOREAPOST                77          /* Korea Post */
#define BARCODE_DBAR_STK                 79          /* GS1 DataBar Stacked */
#define BARCODE_RSS14STACK               79          /* Legacy */
#define BARCODE_DBAR_OMNSTK              80          /* GS1 DataBar Stacked Omnidirectional */
#define BARCODE_RSS14STACK_OMNI          80          /* Legacy */
#define BARCODE_DBAR_EXPSTK              81          /* GS1 DataBar Expanded Stacked */
#define BARCODE_RSS_EXPSTACK             81          /* Legacy */
#define BARCODE_PLANET                   82          /* USPS PLANET */
#define BARCODE_MICROPDF417              84          /* MicroPDF417 */
#define BARCODE_USPS_IMAIL               85          /* USPS Intelligent Mail (OneCode) */
#define BARCODE_ONECODE                  85          /* Legacy */
#define BARCODE_PLESSEY                  86          /* UK Plessey */
/* Tbarcode 8 codes */
#define BARCODE_TELEPEN_NUM              87          /* Telepen Numeric */
#define BARCODE_ITF14                    89          /* ITF-14 */
#define BARCODE_KIX                      90          /* Dutch Post KIX Code */
#define BARCODE_AZTEC                    92          /* Aztec Code */
#define BARCODE_DAFT                     93          /* DAFT Code */
#define BARCODE_DPD                      96          /* DPD Code */
#define BARCODE_MICROQR                  97          /* Micro QR Code */

/* Tbarcode 9 codes */
#define BARCODE_HIBC_128                 98          /* HIBC (Health Industry Barcode) Code 128 */
#define BARCODE_HIBC_39                  99          /* HIBC Code 39 */
#define BARCODE_HIBC_DM                  102         /* HIBC Data Matrix */
#define BARCODE_HIBC_QR                  104         /* HIBC QR Code */
#define BARCODE_HIBC_PDF                 106         /* HIBC PDF417 */
#define BARCODE_HIBC_MICPDF              108         /* HIBC MicroPDF417 */
#define BARCODE_HIBC_BLOCKF              110         /* HIBC Codablock-F */
#define BARCODE_HIBC_AZTEC               112         /* HIBC Aztec Code */

/* Tbarcode 10 codes */
#define BARCODE_DOTCODE                  115         /* DotCode */
#define BARCODE_HANXIN                   116         /* Han Xin (Chinese Sensible) Code */

/* Tbarcode 11 codes */
#define BARCODE_MAILMARK_2D              119         /* Royal Mail 2D Mailmark (CMDM) (Data Matrix) */
#define BARCODE_UPU_S10                  120         /* Universal Postal Union S10 */
#define BARCODE_MAILMARK_4S              121         /* Royal Mail 4-State Mailmark */
#define BARCODE_MAILMARK                 121         /* Legacy */

/* barcode specific */
#define BARCODE_AZRUNE                   128         /* Aztec Runes */
#define BARCODE_CODE32                   129         /* Code 32 */
#define BARCODE_EANX_CC                  130         /* EAN Composite */
#define BARCODE_GS1_128_CC               131         /* GS1-128 Composite */
#define BARCODE_EAN128_CC                131         /* Legacy */
#define BARCODE_DBAR_OMN_CC              132         /* GS1 DataBar Omnidirectional Composite */
#define BARCODE_RSS14_CC                 132         /* Legacy */
#define BARCODE_DBAR_LTD_CC              133         /* GS1 DataBar Limited Composite */
#define BARCODE_RSS_LTD_CC               133         /* Legacy */
#define BARCODE_DBAR_EXP_CC              134         /* GS1 DataBar Expanded Composite */
#define BARCODE_RSS_EXP_CC               134         /* Legacy */
#define BARCODE_UPCA_CC                  135         /* UPC-A Composite */
#define BARCODE_UPCE_CC                  136         /* UPC-E Composite */
#define BARCODE_DBAR_STK_CC              137         /* GS1 DataBar Stacked Composite */
#define BARCODE_RSS14STACK_CC            137         /* Legacy */
#define BARCODE_DBAR_OMNSTK_CC           138         /* GS1 DataBar Stacked Omnidirectional Composite */
#define BARCODE_RSS14_OMNI_CC            138         /* Legacy */
#define BARCODE_DBAR_EXPSTK_CC           139         /* GS1 DataBar Expanded Stacked Composite */
#define BARCODE_RSS_EXPSTACK_CC          139         /* Legacy */
#define BARCODE_CHANNEL                  140         /* Channel Code */
#define BARCODE_CODEONE                  141         /* Code One */
#define BARCODE_GRIDMATRIX               142         /* Grid Matrix */
#define BARCODE_UPNQR                    143         /* UPNQR (Univerzalnega Placilnega Naloga QR) */
#define BARCODE_ULTRA                    144         /* Ultracode */
#define BARCODE_RMQR                     145         /* Rectangular Micro QR Code (rMQR) */
#define BARCODE_BC412                    146         /* IBM BC412 (SEMI T1-95) */
#define BARCODE_LAST                     146         /* Max barcode number marker, not barcode */

/* Output options (`symbol->output_options`) */
#define BARCODE_BIND_TOP                 0x0001      /* Boundary bar above the symbol only (not below), does not affect stacking */
/* Note: value was once used by the legacy (never-used) BARCODE_NO_ASCII */
#define BARCODE_BIND                     0x0002      /* Boundary bars above & below the symbol and between stacked symbols */
#define BARCODE_BOX                      0x0004      /* Box around symbol */
#define BARCODE_STDOUT                   0x0008      /* Output to stdout */
#define READER_INIT                      0x0010      /* Reader Initialisation (Programming) */
#define SMALL_TEXT                       0x0020      /* Use smaller font */
#define BOLD_TEXT                        0x0040      /* Use bold font */
#define CMYK_COLOUR                      0x0080      /* CMYK colour space (Encapsulated PostScript and TIF) */
#define BARCODE_DOTTY_MODE               0x0100      /* Plot a matrix symbol using dots rather than squares */
#define GS1_GS_SEPARATOR                 0x0200      /* Use GS instead of FNC1 as GS1 separator (Data Matrix) */
#define OUT_BUFFER_INTERMEDIATE          0x0400      /* Return ASCII values in bitmap buffer (OUT_BUFFER only) */
#define BARCODE_QUIET_ZONES              0x0800      /* Add compliant quiet zones (additional to any specified whitespace) */
/* Note: CODE16K, CODE49, CODABLOCKF, ITF14, EAN/UPC have default quiet zones
 */
#define BARCODE_NO_QUIET_ZONES           0x1000      /* Disable quiet zones, notably those with defaults as listed above */
#define COMPLIANT_HEIGHT                 0x2000      /* Warn if height not compliant, or use standard height (if any) as default */
#define EANUPC_GUARD_WHITESPACE          0x4000      /* Add quiet zone indicators ("<"/">") to HRT whitespace (EAN/UPC) */
#define EMBED_VECTOR_FONT                0x8000      /* Embed font in vector output - currently only for SVG output */

/* Input data types (`symbol->input_mode`) */
#define DATA_MODE                        0           /* Binary */
#define UNICODE_MODE                     1           /* UTF-8 */
#define GS1_MODE                         2           /* GS1 */
/* The following may be OR-ed with above */
#define ESCAPE_MODE                      0x0008      /* Process escape sequences */
#define GS1PARENS_MODE                   0x0010      /* Process parentheses as GS1 AI delimiters (instead of square brackets) */
#define GS1NOCHECK_MODE                  0x0020      /* Do not check validity of GS1 data (except that printable ASCII only) */
#define HEIGHTPERROW_MODE                0x0040      /* Interpret `height` as per-row rather than as overall height */
#define FAST_MODE                        0x0080      /* Use faster if less optimal encodation or other shortcuts if available */
/* Note: affects DATAMATRIX, MICROPDF417, PDF417, QRCODE & UPNQR only */
#define EXTRA_ESCAPE_MODE                0x0100      /* Process special symbology-specific escape sequences */
/* Note: currently Code 128 only */

/* Data Matrix specific options (`symbol->option_3`) */
#define DM_SQUARE                        100         /* Only consider square versions on automatic symbol size selection */
#define DM_DMRE                          101         /* Consider DMRE versions on automatic symbol size selection */

/* QR, Han Xin, Grid Matrix specific options (`symbol->option_3`) */
#define BARCODE_FULL_MULTIBYTE           200         /* Enable Kanji/Hanzi compression for Latin-1 & binary data */

/* Ultracode specific option (`symbol->option_3`) */
#define ULTRA_COMPRESSION                128         /* Enable Ultracode compression (experimental) */

/* Warning and error conditions (API return values) */
#define BARCODE_WARN_HRT_TRUNCATED       1           /* Human Readable Text was truncated (max 159 bytes) */
#define BARCODE_WARN_INVALID_OPTION      2           /* Invalid option given but overridden by barcode */
#define BARCODE_WARN_USES_ECI            3           /* Automatic ECI inserted by barcode */
#define BARCODE_WARN_NONCOMPLIANT        4           /* Symbol created not compliant with standards */
#define BARCODE_ERROR                    5           /* Warn/error marker, not returned */
#define BARCODE_ERROR_TOO_LONG           5           /* Input data wrong length */
#define BARCODE_ERROR_INVALID_DATA       6           /* Input data incorrect */
#define BARCODE_ERROR_INVALID_CHECK      7           /* Input check digit incorrect */
#define BARCODE_ERROR_INVALID_OPTION     8           /* Incorrect option given */
#define BARCODE_ERROR_ENCODING_PROBLEM   9           /* Internal error (should not happen) */
#define BARCODE_ERROR_FILE_ACCESS        10          /* Error opening output file */
#define BARCODE_ERROR_MEMORY             11          /* Memory allocation (malloc) failure */
#define BARCODE_ERROR_FILE_WRITE         12          /* Error writing to output file */
#define BARCODE_ERROR_USES_ECI           13          /* Error counterpart of warning if WARN_FAIL_ALL set (see below) */
#define BARCODE_ERROR_NONCOMPLIANT       14          /* Error counterpart of warning if WARN_FAIL_ALL set */
#define BARCODE_ERROR_HRT_TRUNCATED      15          /* Error counterpart of warning if WARN_FAIL_ALL set */

/* Warning level (`symbol->warn_level`) */
#define WARN_DEFAULT                     0           /* Default behaviour */
#define WARN_FAIL_ALL                    2           /* Treat warning as error */

/* Capability flags (ZBarcode_Cap() `cap_flag`) */
#define BARCODE_CAP_HRT                  0x0001      /* Prints Human Readable Text? */
#define BARCODE_CAP_STACKABLE            0x0002      /* Is stackable? */
#define BARCODE_CAP_EXTENDABLE           0x0004      /* Is extendable with add-on data? (Is EAN/UPC?) */
#define BARCODE_CAP_COMPOSITE            0x0008      /* Can have composite data? */
#define BARCODE_CAP_ECI                  0x0010      /* Supports Extended Channel Interpretations? */
#define BARCODE_CAP_GS1                  0x0020      /* Supports GS1 data? */
#define BARCODE_CAP_DOTTY                0x0040      /* Can be output as dots? */
#define BARCODE_CAP_QUIET_ZONES          0x0080      /* Has default quiet zones? */
#define BARCODE_CAP_FIXED_RATIO          0x0100      /* Has fixed width-to-height (aspect) ratio? */
#define BARCODE_CAP_READER_INIT          0x0200      /* Supports Reader Initialisation? */
#define BARCODE_CAP_FULL_MULTIBYTE       0x0400      /* Supports full-multibyte option? */
#define BARCODE_CAP_MASK                 0x0800      /* Is mask selectable? */
#define BARCODE_CAP_STRUCTAPP            0x1000      /* Supports Structured Append? */
#define BARCODE_CAP_COMPLIANT_HEIGHT     0x2000      /* Has compliant height? */

/* The largest amount of data that can be encoded is 4350 4-byte UTF-8 chars in Han Xin Code */
#define BARCODE_MAX_DATA_LEN             17400
/* Maximum number of segments allowed for (`seg_count`) */
#define BARCODE_MAX_SEG_COUNT            256

/* Debug flags (`symbol->debug`) */
#define BARCODE_DEBUG_PRINT              0x0000      /* Print debug info (if any) to stdout */ // 0x0001
#define BARCODE_DEBUG_TEST               0x0000      /* For internal test use only */           // 0x0002

#define BARCODE_EXTERN extern

/* `is_sane()` flags */
#define IS_SPC_F                         0x0001      /* Space */
#define IS_HSH_F                         0x0002      /* Hash sign # */
#define IS_AST_F                         0x0004      /* Asterisk sign * */
#define IS_PLS_F                         0x0008      /* Plus sign + */
#define IS_MNS_F                         0x0010      /* Minus sign - */
#define IS_NUM_F                         0x0020      /* Number 0-9 */
#define IS_UPO_F                         0x0040      /* Uppercase letter, apart from A-F and X */
#define IS_UHX_F                         0x0080      /* Uppercase hex A-F */
#define IS_UX__F                         0x0100      /* Uppercase X */
#define IS_LWO_F                         0x0200      /* Lowercase letter, apart from a-f and x */
#define IS_LHX_F                         0x0400      /* Lowercase hex a-f */
#define IS_LX__F                         0x0800      /* Lowercase x */
#define IS_C82_F                         0x1000      /* CSET82 punctuation (apart from *, + and -) */
#define IS_SIL_F                         0x2000      /* SILVER/TECHNETIUM punctuation .$/% (apart from space, + and -) */
#define IS_CLI_F                         0x4000      /* CALCIUM INNER punctuation $:/. (apart from + and -) (Codabar) */
#define IS_ARS_F                         0x8000      /* ARSENIC uppercase subset (VIN) */

#define IS_UPR_F (IS_UPO_F | IS_UHX_F | IS_UX__F)    /* Uppercase letters */
#define IS_LWR_F (IS_LWO_F | IS_LHX_F | IS_LX__F)    /* Lowercase letters */

/* The most commonly used set */
#define NEON_F IS_NUM_F /* NEON "0123456789" */

/* Simple versions of <ctype.h> functions with no dependence on locale */
#define __isdigit(c) ((c) <= '9' && (c) >= '0')
#define __isupper(c) ((c) >= 'A' && (c) <= 'Z')
#define __islower(c) ((c) >= 'a' && (c) <= 'z')

#define INTERNAL
#define INTERNAL_DATA_EXTERN extern
#define INTERNAL_DATA

/* Sets a module to dark/black */
#define set_module(s, y, x)                                  \
    do                                                       \
    {                                                        \
        (s)->encoded_data[(y)][(x) >> 3] |= 1 << ((x)&0x07); \
    } while (0)

/*============================================================================*
 *                            Variables
 *============================================================================*/

INTERNAL_DATA const char C128Table[107][6] =  /* Used by CODABLOCKF and CODE16K also */
{
    /* Code 128 character encodation - Table 1 (with final CODE16K-only character in place of Stop character) */
    {'2', '1', '2', '2', '2', '2'},
    {'2', '2', '2', '1', '2', '2'},
    {'2', '2', '2', '2', '2', '1'},
    {'1', '2', '1', '2', '2', '3'},
    {'1', '2', '1', '3', '2', '2'},
    {'1', '3', '1', '2', '2', '2'},
    {'1', '2', '2', '2', '1', '3'},
    {'1', '2', '2', '3', '1', '2'},
    {'1', '3', '2', '2', '1', '2'},
    {'2', '2', '1', '2', '1', '3'},
    {'2', '2', '1', '3', '1', '2'},
    {'2', '3', '1', '2', '1', '2'},
    {'1', '1', '2', '2', '3', '2'},
    {'1', '2', '2', '1', '3', '2'},
    {'1', '2', '2', '2', '3', '1'},
    {'1', '1', '3', '2', '2', '2'},
    {'1', '2', '3', '1', '2', '2'},
    {'1', '2', '3', '2', '2', '1'},
    {'2', '2', '3', '2', '1', '1'},
    {'2', '2', '1', '1', '3', '2'},
    {'2', '2', '1', '2', '3', '1'},
    {'2', '1', '3', '2', '1', '2'},
    {'2', '2', '3', '1', '1', '2'},
    {'3', '1', '2', '1', '3', '1'},
    {'3', '1', '1', '2', '2', '2'},
    {'3', '2', '1', '1', '2', '2'},
    {'3', '2', '1', '2', '2', '1'},
    {'3', '1', '2', '2', '1', '2'},
    {'3', '2', '2', '1', '1', '2'},
    {'3', '2', '2', '2', '1', '1'},
    {'2', '1', '2', '1', '2', '3'},
    {'2', '1', '2', '3', '2', '1'},
    {'2', '3', '2', '1', '2', '1'},
    {'1', '1', '1', '3', '2', '3'},
    {'1', '3', '1', '1', '2', '3'},
    {'1', '3', '1', '3', '2', '1'},
    {'1', '1', '2', '3', '1', '3'},
    {'1', '3', '2', '1', '1', '3'},
    {'1', '3', '2', '3', '1', '1'},
    {'2', '1', '1', '3', '1', '3'},
    {'2', '3', '1', '1', '1', '3'},
    {'2', '3', '1', '3', '1', '1'},
    {'1', '1', '2', '1', '3', '3'},
    {'1', '1', '2', '3', '3', '1'},
    {'1', '3', '2', '1', '3', '1'},
    {'1', '1', '3', '1', '2', '3'},
    {'1', '1', '3', '3', '2', '1'},
    {'1', '3', '3', '1', '2', '1'},
    {'3', '1', '3', '1', '2', '1'},
    {'2', '1', '1', '3', '3', '1'},
    {'2', '3', '1', '1', '3', '1'},
    {'2', '1', '3', '1', '1', '3'},
    {'2', '1', '3', '3', '1', '1'},
    {'2', '1', '3', '1', '3', '1'},
    {'3', '1', '1', '1', '2', '3'},
    {'3', '1', '1', '3', '2', '1'},
    {'3', '3', '1', '1', '2', '1'},
    {'3', '1', '2', '1', '1', '3'},
    {'3', '1', '2', '3', '1', '1'},
    {'3', '3', '2', '1', '1', '1'},
    {'3', '1', '4', '1', '1', '1'},
    {'2', '2', '1', '4', '1', '1'},
    {'4', '3', '1', '1', '1', '1'},
    {'1', '1', '1', '2', '2', '4'},
    {'1', '1', '1', '4', '2', '2'},
    {'1', '2', '1', '1', '2', '4'},
    {'1', '2', '1', '4', '2', '1'},
    {'1', '4', '1', '1', '2', '2'},
    {'1', '4', '1', '2', '2', '1'},
    {'1', '1', '2', '2', '1', '4'},
    {'1', '1', '2', '4', '1', '2'},
    {'1', '2', '2', '1', '1', '4'},
    {'1', '2', '2', '4', '1', '1'},
    {'1', '4', '2', '1', '1', '2'},
    {'1', '4', '2', '2', '1', '1'},
    {'2', '4', '1', '2', '1', '1'},
    {'2', '2', '1', '1', '1', '4'},
    {'4', '1', '3', '1', '1', '1'},
    {'2', '4', '1', '1', '1', '2'},
    {'1', '3', '4', '1', '1', '1'},
    {'1', '1', '1', '2', '4', '2'},
    {'1', '2', '1', '1', '4', '2'},
    {'1', '2', '1', '2', '4', '1'},
    {'1', '1', '4', '2', '1', '2'},
    {'1', '2', '4', '1', '1', '2'},
    {'1', '2', '4', '2', '1', '1'},
    {'4', '1', '1', '2', '1', '2'},
    {'4', '2', '1', '1', '1', '2'},
    {'4', '2', '1', '2', '1', '1'},
    {'2', '1', '2', '1', '4', '1'},
    {'2', '1', '4', '1', '2', '1'},
    {'4', '1', '2', '1', '2', '1'},
    {'1', '1', '1', '1', '4', '3'},
    {'1', '1', '1', '3', '4', '1'},
    {'1', '3', '1', '1', '4', '1'},
    {'1', '1', '4', '1', '1', '3'},
    {'1', '1', '4', '3', '1', '1'},
    {'4', '1', '1', '1', '1', '3'},
    {'4', '1', '1', '3', '1', '1'},
    {'1', '1', '3', '1', '4', '1'},
    {'1', '1', '4', '1', '3', '1'},
    {'3', '1', '1', '1', '4', '1'},
    {'4', '1', '1', '1', '3', '1'},
    {'2', '1', '1', '4', '1', '2'},
    {'2', '1', '1', '2', '1', '4'},
    {'2', '1', '1', '2', '3', '2'},
    {/* Only used by CODE16K */ '2', '1', '1', '1', '3', '3'}
};


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/* Converts a character 0-9, A-F to its equivalent integer value */
INTERNAL int gui_barcode_gen_ctoi(const char source);

/* Expands from a width pattern to a bit pattern */
INTERNAL void gui_barcode_gen_expand(struct barcode_symbol *symbol, const char data[],
                                     const int length);

/* Treats source as ISO/IEC 8859-1 and copies into `symbol->text`, converting to UTF-8. Control chars (incl. DEL) and
   non-ISO/IEC 8859-1 (0x80-9F) are replaced with spaces. Returns warning if truncated, else 0 */
INTERNAL int gui_barcode_gen_hrt_cpy_iso8859_1(struct barcode_symbol *symbol,
                                               const unsigned char source[],
                                               const int length);

/* Converts a character 0-9, A-F to its equivalent integer value */
INTERNAL int gui_barcode_gen_ctoi(const char source)
{
    if (__isdigit(source))
    {
        return (source - '0');
    }

    if ((source >= 'A') && (source <= 'F'))
    {
        return (source - 'A' + 10);
    }

    if ((source >= 'a') && (source <= 'f'))
    {
        return (source - 'a' + 10);
    }
    return -1;
}

/* Expands from a width pattern to a bit pattern */
INTERNAL void gui_barcode_gen_expand(struct barcode_symbol *symbol, const char data[],
                                     const int length)
{
    int reader;
    int writer, i;
    int latch, num;
    const int row = symbol->rows;

    // we only use code128AUTO, update encoded data every time at the same row;
    // symbol->rows++;
    if (row != 0)
    {
        gui_log("Row index out of bounds");
        return;
    }
    memset(symbol->encoded_data[row], 0, 144);
    writer = 0;
    latch = 1;

    for (reader = 0; reader < length; reader++)
    {
        num = gui_barcode_gen_ctoi(data[reader]);
        assert(num >= 0);
        for (i = 0; i < num; i++)
        {
            if (latch)
            {
                set_module(symbol, row, writer);
            }
            writer++;
        }

        latch = !latch;
    }

    if (writer > symbol->width)
    {
        symbol->width = writer;
    }
}

INTERNAL int gui_barcode_gen_hrt_cpy_iso8859_1(struct barcode_symbol *symbol,
                                               const unsigned char source[],
                                               const int length)
{
    int i, j;
    int warn_number = 0;

    for (i = 0, j = 0; i < length && j < (int)sizeof(symbol->text); i++)
    {
        if (source[i] < 0x80)
        {
            symbol->text[j++] = source[i] >= ' ' && source[i] != 0x7F ? source[i] : ' ';
        }
        else if (source[i] < 0xC0)
        {
            if (source[i] >= 0xA0)
            {
                /* 0x80-0x9F not valid ISO/IEC 8859-1 */
                if (j + 2 >= (int)sizeof(symbol->text))
                {
                    warn_number = BARCODE_WARN_HRT_TRUNCATED;
                    break;
                }
                symbol->text[j++] = 0xC2;
                symbol->text[j++] = source[i];
            }
            else
            {
                symbol->text[j++] = ' ';
            }
        }
        else
        {
            if (j + 2 >= (int)sizeof(symbol->text))
            {
                warn_number = BARCODE_WARN_HRT_TRUNCATED;
                break;
            }
            symbol->text[j++] = 0xC3;
            symbol->text[j++] = source[i] - 0x40;
        }
    }

    if (j == sizeof(symbol->text))
    {
        warn_number = BARCODE_WARN_HRT_TRUNCATED;
        j--;
    }
    symbol->text[j] = '\0';

    if (warn_number)
    {
        strcpy(symbol->errtxt, "249: Human Readable Text truncated");
    }

    return warn_number;
}

/* Determine appropriate mode for a given character */
INTERNAL int gui_barcode_gen_c128_parunmodd(const unsigned char llyth)
{
    int modd;

    if (llyth <= 31)
    {
        modd = C128_SHIFTA;
    }
    else if ((llyth >= 48) && (llyth <= 57))
    {
        modd = C128_ABORC;
    }
    else if (llyth <= 95)
    {
        modd = C128_AORB;
    }
    else if (llyth <= 127)
    {
        modd = C128_SHIFTB;
    }
    else if (llyth <= 159)
    {
        modd = C128_SHIFTA;
    }
    else if (llyth <= 223)
    {
        modd = C128_AORB;
    }
    else
    {
        modd = C128_SHIFTB;
    }

    return modd;
}

/**
 * Bring together same type blocks
 */
static void gui_barcode_gen_c128_grwp(int list[2][C128_MAX], int *p_indexliste)
{
    /* bring together same type blocks */
    if (*p_indexliste > 1)
    {
        int i = 1;
        while (i < *p_indexliste)
        {
            if (list[1][i - 1] == list[1][i])
            {
                int j;
                /* bring together */
                list[0][i - 1] = list[0][i - 1] + list[0][i];
                j = i + 1;

                /* decrease the list */
                while (j < *p_indexliste)
                {
                    list[0][j - 1] = list[0][j];
                    list[1][j - 1] = list[1][j];
                    j++;
                }
                *p_indexliste = *p_indexliste - 1;
                i--;
            }
            i++;
        }
    }
}

static void __first_block_gen_c128(int list[2][C128_MAX],
                                   int *current_src,
                                   const int length,
                                   const char *manual_set,
                                   const int indexliste,
                                   const int next)
{
    int current = *current_src;
    int i = 0;
    if (current == C128_ABORC)
    {
        if (manual_set && manual_set[i])
        {
            list[1][i] = manual_set[i];
            current = manual_set[i];
        }
        else if ((indexliste == 1) && (length == 2))
        {
            /* Rule 1a */
            list[1][i] = C128_LATCHC;
            current = C128_LATCHC;
        }
        else if (length >= 4)
        {
            /* Rule 1b */
            list[1][i] = C128_LATCHC;
            current = C128_LATCHC;
        }
        else
        {
            current = C128_AORB; /* Determine below */
        }
    }

    if (current == C128_AORB)
    {
        if (manual_set && (manual_set[i] == 'A' || manual_set[i] == 'B'))
        {
            list[1][i] = manual_set[i];
        }
        else if (next == C128_SHIFTA)
        {
            /* Rule 1c */
            list[1][i] = C128_LATCHA;
        }
        else
        {
            /* Rule 1d */
            list[1][i] = C128_LATCHB;
        }
    }
    else if (current == C128_SHIFTA)
    {
        /* Rule 1c */
        list[1][i] = C128_LATCHA;
    }
    else if (current == C128_SHIFTB)
    {
        /* Unless C128_LATCHX set above, can only be C128_SHIFTB */
        /* Rule 1d */
        list[1][i] = C128_LATCHB;
    }
    *current_src = current;
}

static void __other_block_gen_c128(int list[2][C128_MAX],
                                   int *current_src,
                                   const int length,
                                   const char *manual_set,
                                   const int next,
                                   const int last,
                                   const int i)
{
    int current = *current_src;
    if (current == C128_ABORC)
    {
        if (manual_set && manual_set[i])
        {
            list[1][i] = manual_set[i];
            current = manual_set[i];
        }
        else if (length >= 4)
        {
            /* Rule 3 - note Rule 3b (odd C blocks) dealt with later */
            list[1][i] = C128_LATCHC;
            current = C128_LATCHC;
        }
        else
        {
            current = C128_AORB; /* Determine below */
        }
    }

    if (current == C128_AORB)
    {
        if (manual_set && (manual_set[i] == 'A' || manual_set[i] == 'B'))
        {
            list[1][i] = manual_set[i];
        }
        else if (last == C128_LATCHA || last == C128_SHIFTB)
        {
            /* Maintain state */
            list[1][i] = C128_LATCHA;
        }
        else if (last == C128_LATCHB || last == C128_SHIFTA)
        {
            /* Maintain state */
            list[1][i] = C128_LATCHB;
        }
        else if (next == C128_SHIFTA)
        {
            list[1][i] = C128_LATCHA;
        }
        else
        {
            list[1][i] = C128_LATCHB;
        }
    }
    else if (current == C128_SHIFTA)
    {
        if (manual_set && manual_set[i] == 'A')
        {
            list[1][i] = C128_LATCHA;
        }
        else if (length > 1)
        {
            /* Rule 4 */
            list[1][i] = C128_LATCHA;
        }
        else if (last == C128_LATCHA || last == C128_SHIFTB)
        {
            /* Maintain state */
            list[1][i] = C128_LATCHA;
        }
        else if (last == C128_LATCHC)
        {
            list[1][i] = C128_LATCHA;
        }
    }
    else if (current == C128_SHIFTB)
    {
        /* Unless C128_LATCHX set above, can only be C128_SHIFTB */
        if (manual_set && manual_set[i] == 'B')
        {
            list[1][i] = C128_LATCHB;
        }
        else if (length > 1)
        {
            /* Rule 5 */
            list[1][i] = C128_LATCHB;
        }
        else if (last == C128_LATCHB || last == C128_SHIFTA)
        {
            /* Maintain state */
            list[1][i] = C128_LATCHB;
        }
        else if (last == C128_LATCHC)
        {
            list[1][i] = C128_LATCHB;
        }
    }
    *current_src = current;
}

/**
 * Implements rules from ISO 15417 Annex E
 */
INTERNAL void gui_barcode_gen_c128_dxsmooth(int list[2][C128_MAX], int *p_indexliste,
                                            const char *manual_set)
{
    int i, last, next;
    const int indexliste = *p_indexliste;

    for (i = 0; i < indexliste; i++)
    {
        int current = list[1][i]; /* Either C128_ABORC, C128_AORB, C128_SHIFTA or C128_SHIFTB */
        int length = list[0][i];

        last = next = 0;
        if (i != 0)
        {
            last = list[1][i - 1];
        }

        if (i != indexliste - 1)
        {
            next = list[1][i + 1];
        }

        if (i == 0)
        {
            /* first block */
            __first_block_gen_c128(list, &current, length, manual_set, indexliste, next);
        }
        else
        {
            __other_block_gen_c128(list, &current, length, manual_set, next, last, i);
        } /* Rule 2 is implemented elsewhere, Rule 6 is implied */
    }

    gui_barcode_gen_c128_grwp(list, p_indexliste);
}

/**
 * Translate Code 128 Set A characters into barcodes.
 * This set handles all control characters NUL to US.
 */
INTERNAL void gui_barcode_gen_c128_set_a(const unsigned char source, int values[], int *bar_chars)
{
    if (source > 127)
    {
        if (source < 160)
        {
            values[(*bar_chars)] = (source - 128) + 64;
        }
        else
        {
            values[(*bar_chars)] = (source - 128) - 32;
        }
    }
    else
    {
        if (source < 32)
        {
            values[(*bar_chars)] = source + 64;
        }
        else
        {
            values[(*bar_chars)] = source - 32;
        }
    }
    (*bar_chars)++;
}

/**
 * Translate Code 128 Set B characters into barcodes.
 * This set handles all characters which are not part of long numbers and not
 * control characters.
 */
INTERNAL int gui_barcode_gen_c128_set_b(const unsigned char source, int values[], int *bar_chars)
{
    if (source >= 128 + 32)
    {
        values[(*bar_chars)] = source - 32 - 128;
    }
    else if (source >= 128)
    {
        /* Should never happen */
        return 0; /* Not reached */
    }
    else if (source >= 32)
    {
        values[(*bar_chars)] = source - 32;
    }
    else
    {
        /* Should never happen */
        return 0; /* Not reached */
    }
    (*bar_chars)++;
    return 1;
}

/* Translate Code 128 Set C characters into barcodes
 * This set handles numbers in a compressed form
 */
INTERNAL void gui_barcode_gen_c128_set_c(const unsigned char source_a, const unsigned char source_b,
                                         int values[],
                                         int *bar_chars)
{
    int weight;

    weight = (10 * gui_barcode_gen_ctoi(source_a)) + gui_barcode_gen_ctoi(source_b);
    values[(*bar_chars)] = weight;
    (*bar_chars)++;
}

/* Put set data into set[]. If source given (GS1_MODE) then resolves odd C blocks */
INTERNAL void c128_put_in_set(int list[2][C128_MAX], const int indexliste, char set[C128_MAX],
                              const unsigned char *source)
{
    int read = 0;
    int i, j;

    for (i = 0; i < indexliste; i++)
    {
        for (j = 0; j < list[0][i]; j++)
        {
            set[read++] = list[1][i];
        }
    }

    if (source)
    {
        /* Watch out for odd-length Mode C blocks */
        int c_count = 0;
        for (i = 0; i < read; i++)
        {
            if (set[i] == 'C')
            {
                if (source[i] == '[')
                {
                    if (c_count & 1)
                    {
                        if ((i - c_count) != 0)
                        {
                            set[i - c_count] = 'B';
                        }
                        else
                        {
                            set[i - 1] = 'B';
                        }
                    }
                    c_count = 0;
                }
                else
                {
                    c_count++;
                }
            }
            else
            {
                if (c_count & 1)
                {
                    if ((i - c_count) != 0)
                    {
                        set[i - c_count] = 'B';
                    }
                    else
                    {
                        set[i - 1] = 'B';
                    }
                }
                c_count = 0;
            }
        }

        if (c_count & 1)
        {
            if ((i - c_count) != 0)
            {
                set[i - c_count] = 'B';
            }
            else
            {
                set[i - 1] = 'B';
            }
        }

        for (i = 1; i < read - 1; i++)
        {
            if ((set[i] == 'C') && ((set[i - 1] == 'B') && (set[i + 1] == 'B')))
            {
                set[i] = 'B';
            }
        }
    }
}

static bool __detect_special_code(struct barcode_symbol *symbol,
                                  unsigned char *src,
                                  unsigned char source[],
                                  unsigned char src_buf[],
                                  char *manual_set,
                                  int length)
{
    int j = 0;
    if ((symbol->input_mode & EXTRA_ESCAPE_MODE) && symbol->symbology == BARCODE_CODE128)
    {
        char manual_ch = '\0';
        for (int i = 0; i < length; i++)
        {
            if (source[i] == '\\' && i + 2 < length && source[i + 1] == '^' && ((source[i + 2] >= 'A' &&
                                                                                 source[i + 2] <= 'C') || source[i + 2] == '^'))
            {
                if (source[i + 2] != '^')
                {
                    i += 2;
                    manual_ch = source[i];
                }
                else
                {
                    /* Escape sequence '\^^' */
                    manual_set[j] = manual_ch;
                    src_buf[j++] = source[i++];
                    manual_set[j] = manual_ch;
                    src_buf[j++] = source[i++];
                    /* Drop second '^' */
                }
            }
            else
            {
                manual_set[j] = manual_ch;
                src_buf[j++] = source[i];
            }
        }

        if (j != length)
        {
            length = j;
            if (length == 0)
            {
                strcpy(symbol->errtxt, "842: No input data");
                gui_free(symbol);
                return false;//BARCODE_ERROR_INVALID_DATA;
            }
            src = src_buf;
            src[length] = '\0';
        }
    }
    return true;
}

static void __latch_to_extended_mode(char *fset, int length)
{
    for (int i = 0, j = 0, k = 0; i < length; i++)
    {
        if (fset[i] == 'f')
        {
            j++;
        }
        else
        {
            j = 0;
        }

        if (j >= 5)
        {
            for (k = i; k > (i - 5); k--)
            {
                fset[k] = 'F';
            }
        }

        if ((j >= 3) && (i == (length - 1)))
        {
            for (k = i; k > (i - 3); k--)
            {
                fset[k] = 'F';
            }
        }
    }
}

static void __revert_to_646(char *fset, int length)
{
    if (length > C128_MAX)
    {
        return;
    }
    int i, j, k;
    for (i = 1; i < length; i++)
    {
        if ((fset[i - 1] == 'F') && (fset[i] == ' '))
        {
            /* Detected a change from 8859-1 to 646 - count how long for */
            for (j = 0; ((i + j) < length) && (fset[i + j] == ' '); j++)
                ;

            /* Count how many 8859-1 beyond */
            k = 0;
            if (i + j < length)
            {
                for (k = 1; ((i + j + k) < length) && (fset[i + j + k] != ' '); k++)
                    ;
            }

            if (j < 3 || (j < 5 && k > 2))
            {
                /* Change to shifting back rather than latching back */
                /* Inverts the same figures recommended by Annex E note 3 */
                for (k = 0; k < j; k++)
                {
                    fset[i + k] = 'n';
                }
            }
        }
    }
}

static void __decide_mode(struct barcode_symbol *symbol,
                          unsigned char *src,
                          unsigned char src_buf[],
                          char *manual_set,
                          int list[2][C128_MAX],
                          int *indexliste_src,
                          int *indexchaine_src,
                          int length)
{
    int indexliste = 0;
    int indexchaine = 0;

    int mode = gui_barcode_gen_c128_parunmodd(src[indexchaine]);
    if (mode == C128_ABORC && (symbol->symbology == BARCODE_CODE128AB ||
                               (manual_set[indexchaine] == 'A' || manual_set[indexchaine] == 'B')))
    {
        mode = C128_AORB;
    }

    do
    {
        list[1][indexliste] = mode;
        while ((list[1][indexliste] == mode) && (indexchaine < length))
        {
            list[0][indexliste]++;
            indexchaine++;
            if (indexchaine == length)
            {
                break;
            }

            mode = gui_barcode_gen_c128_parunmodd(src[indexchaine]);
            if (mode == C128_ABORC && (symbol->symbology == BARCODE_CODE128AB ||
                                       (manual_set[indexchaine] == 'A' || manual_set[indexchaine] == 'B')))
            {
                mode = C128_AORB;
            }

            if (manual_set[indexchaine] != manual_set[indexchaine - 1])
            {
                break;
            }
        }
        indexliste++;
    }
    while (indexchaine < length);

    if (src == src_buf)
    {
        /* Need to re-index `manual_set` to have sames indexes as `list` blocks for `gui_barcode_gen_c128_dxsmooth()` */
        int j = 0;
        for (int i = 1; i < indexliste; i++)
        {
            j += list[0][i - 1];
            manual_set[i] = manual_set[j];
        }
    }

    *indexliste_src = indexliste;
    *indexchaine_src = indexchaine;
}

static void __resolve_odd_length(int list[2][C128_MAX], int indexliste)
{
    if ((list[1][0] == C128_LATCHC) && (list[0][0] & 1))
    {
        /* Rule 2 */
        list[0][1]++;
        list[0][0]--;
        if (indexliste == 1)
        {
            list[0][1] = 1;
            list[1][1] = C128_LATCHB;
            indexliste = 2;
        }
    }

    if (indexliste > 1)
    {
        for (int i = 1; i < indexliste; i++)
        {
            if ((list[1][i] == C128_LATCHC) && (list[0][i] & 1))
            {
                /* Rule 3b */
                list[0][i - 1]++;
                list[0][i]--;
            }
        }
    }
}

static void __calculate_glyph_count(char set[],
                                    char fset[],
                                    int length,
                                    int *glyph_count)
{
    char last_set = set[0];
    for (int i = 0; i < length; i++)
    {
        if ((set[i] == 'a') || (set[i] == 'b'))
        {
            glyph_count += 2; /* 1 codeword */
        }

        if ((fset[i] == 'f') || (fset[i] == 'n'))
        {
            glyph_count += 2; /* May be overestimate if in latch */
        }

        if (((set[i] == 'A') || (set[i] == 'B')) || (set[i] == 'C'))
        {
            if (set[i] != last_set)
            {
                last_set = set[i];
                glyph_count += 2;
            }
        }

        if (i == 0)
        {
            if (fset[i] == 'F')
            {
                glyph_count += 4; /* 2 codewords */
            }
        }
        else
        {
            if ((fset[i] == 'F') && (fset[i - 1] != 'F'))
            {
                glyph_count += 4;
            }
            if ((fset[i] != 'F') && (fset[i - 1] == 'F'))
            {
                glyph_count += 4;
            }
        }

        if (set[i] == 'C')
        {
            glyph_count += 1; /* Half a codeword */
        }
        else
        {
            glyph_count += 2;
        }
    }
}

static void __prepapre_encoede(struct barcode_symbol *symbol,
                               const char set[],
                               const char fset[],
                               int values[],
                               int *bar_characters_src,
                               char *current_set_src,
                               int *f_state)
{
    int bar_characters = *bar_characters_src;
    char current_set = *current_set_src;
    if (symbol->output_options & READER_INIT)
    {
        /* Reader Initialisation mode */
        switch (set[0])
        {
        case 'A': /* Start A */
            {
                values[bar_characters++] = 103;
                current_set = 'A';
                values[bar_characters++] = 96; /* FNC3 */
            }
            break;

        case 'B': /* Start B */
            {
                values[bar_characters++] = 104;
                current_set = 'B';
                values[bar_characters++] = 96; /* FNC3 */
            }
            break;

        case 'C': /* Start C */
            {
                values[bar_characters++] = 104; /* Start B */
                values[bar_characters++] = 96;  /* FNC3 */
                values[bar_characters++] = 99;  /* Code C */
                current_set = 'C';
            }
            break;

        default:
            break;
        }
    }
    else
    {
        /* Normal mode */
        switch (set[0])
        {
        case 'A': /* Start A */
            {
                values[bar_characters++] = 103;
                current_set = 'A';
            }
            break;

        case 'B': /* Start B */
            {
                values[bar_characters++] = 104;
                current_set = 'B';
            }
            break;

        case 'C': /* Start C */
            {
                values[bar_characters++] = 105;
                current_set = 'C';
            }
            break;

        default:
            break;
        }
    }

    if (fset[0] == 'F')
    {
        switch (current_set)
        {
        case 'A':
            {
                values[bar_characters++] = 101;
                values[bar_characters++] = 101;
                *f_state = 1;
            }
            break;

        case 'B':
            {
                values[bar_characters++] = 100;
                values[bar_characters++] = 100;
                *f_state = 1;
            }
            break;

        default:
            break;
        }
    }
    *bar_characters_src = bar_characters;
    *current_set_src = current_set;
}

static void __encode_data(const char set[],
                          const char fset[],
                          unsigned char *src,
                          int values[],
                          int *bar_characters_src,
                          char *current_set_src,
                          int *f_state_src,
                          int length)
{
    char current_set = *current_set_src;
    int bar_characters = *bar_characters_src;
    int f_state = *f_state_src;
    int read = 0;
    do
    {
        if ((read != 0) && (set[read] != current_set))
        {
            /* Latch different code set */
            switch (set[read])
            {
            case 'A':
                {
                    values[bar_characters++] = 101;
                    current_set = 'A';
                }
                break;

            case 'B':
                {
                    values[bar_characters++] = 100;
                    current_set = 'B';
                }
                break;

            case 'C':
                {
                    values[bar_characters++] = 99;
                    current_set = 'C';
                }
                break;

            default:
                break;
            }
        }

        if (read != 0)
        {
            if ((fset[read] == 'F') && (f_state == 0))
            {
                /* Latch beginning of extended mode */
                switch (current_set)
                {
                case 'A':
                    {
                        values[bar_characters++] = 101;
                        values[bar_characters++] = 101;
                        f_state = 1;
                    }
                    break;

                case 'B':
                    {
                        values[bar_characters++] = 100;
                        values[bar_characters++] = 100;
                        f_state = 1;
                    }
                    break;

                default:
                    break;
                }
            }

            if ((fset[read] == ' ') && (f_state == 1))
            {
                /* Latch end of extended mode */
                switch (current_set)
                {
                case 'A':
                    {
                        values[bar_characters++] = 101;
                        values[bar_characters++] = 101;
                        f_state = 0;
                    }
                    break;

                case 'B':
                    {
                        values[bar_characters++] = 100;
                        values[bar_characters++] = 100;
                        f_state = 0;
                    }
                    break;

                default:
                    break;
                }
            }
        }

        if ((fset[read] == 'f' && f_state == 0) || (fset[read] == 'n' && f_state == 1))
        {
            /* Shift to or from extended mode */
            switch (current_set)
            {
            case 'A':
                {
                    values[bar_characters++] = 101; /* FNC 4 */
                }
                break;

            case 'B':
                {
                    values[bar_characters++] = 100; /* FNC 4 */
                }
                break;

            default:
                break;
            }
        }

        if ((set[read] == 'a') || (set[read] == 'b'))
        {
            /* Insert shift character */
            values[bar_characters++] = 98;
        }

        switch (set[read])
        {
        /* Encode data characters */
        case 'a':
        case 'A':
            {
                gui_barcode_gen_c128_set_a(src[read], values, &bar_characters);
                read++;
            }
            break;

        case 'b':
        case 'B':
            {
                (void)gui_barcode_gen_c128_set_b(src[read], values, &bar_characters);
                read++;
            }
            break;

        case 'C':
            {
                gui_barcode_gen_c128_set_c(src[read], src[read + 1], values, &bar_characters);
                read += 2;
            }
            break;

        default:
            break;
        }
    }
    while (read < length);

    *bar_characters_src = bar_characters;
    *current_set_src = current_set;
    *f_state_src = f_state;
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/* Handle Code 128, 128B and HIBC 128 */
barcode_symbol_t *gui_barcode_gen_barcode_encode(unsigned char source[], int length)
{
    int i, values[C128_MAX] = {0}, bar_characters = 0, total_sum;
    //int error_number = 0;
    int indexchaine, indexliste, f_state = 0;
    unsigned char src_buf[C128_MAX + 1];
    unsigned char *src = source;
    char manual_set[C128_MAX] = {0};
    int list[2][C128_MAX] = {{0}};
    char set[C128_MAX] = {0}, fset[C128_MAX], current_set = ' ';
    int glyph_count = 0; /* Codeword estimate times 2 */
    char dest[200];
    char *d = dest;

    struct barcode_symbol *symbol = gui_malloc(sizeof(barcode_symbol_t));
    GUI_ASSERT(symbol != NULL);
    memset(symbol, 0, sizeof(barcode_symbol_t));//note

    /* Suppresses clang-analyzer-core.UndefinedBinaryOperatorResult warning on fset which is fully set */
    assert(length > 0);

    if (length > C128_MAX)
    {
        /* This only blocks ridiculously long input - the actual length of the
           resulting barcode depends on the type of data, so this is trapped later */
        sprintf(symbol->errtxt, "340: Input too long (%d character maximum)", C128_MAX);
        gui_free(symbol);
        return NULL;//BARCODE_ERROR_TOO_LONG;
    }

    /* Detect special Code Set escapes for Code 128 in extra escape mode only */
    if (!__detect_special_code(symbol, src, source, src_buf, manual_set, length))
    {
        return NULL;
    }

    /* Detect extended ASCII characters */
    for (i = 0; i < length; i++)
    {
        fset[i] = src[i] >= 128 ? 'f' : ' ';
    }

    /* Decide when to latch to extended mode - Annex E note 3 */
    __latch_to_extended_mode(fset, length);

    /* Decide if it is worth reverting to 646 encodation for a few characters as described in 4.3.4.2 (d) */
    __revert_to_646(fset, length);

    /* Decide on mode using same system as PDF417 and rules of ISO 15417 Annex E */
    __decide_mode(symbol, src, src_buf, manual_set, list, &indexliste, &indexchaine, length);
    gui_barcode_gen_c128_dxsmooth(list, &indexliste, src == src_buf ? manual_set : NULL);

    /* Resolve odd length C128_LATCHC blocks */
    __resolve_odd_length(list, indexliste);

    /* Put set data into set[]. Giving NULL as source as used to resolve odd C blocks which has been done above */
    c128_put_in_set(list, indexliste, set, NULL /*source*/);

    // if (symbol->debug & BARCODE_DEBUG_PRINT)
    // {
    //     gui_log("Data: %.*s (%d)\n", length, src, length);
    //     gui_log(" Set: %.*s\n", length, set);
    //     gui_log("FSet: %.*s\n", length, fset);
    // }

    /* Now we can calculate how long the barcode is going to be - and stop it from
       being too long */
    __calculate_glyph_count(set, fset, length, &glyph_count);

    if (glyph_count > 120)
    {
        /* 60 * 2 */
        strcpy(symbol->errtxt, "341: Input too long (60 symbol character maximum)");
        gui_free(symbol);
        return NULL;//BARCODE_ERROR_TOO_LONG;
    }

    /* So now we know what start character to use - we can get on with it! */
    __prepapre_encoede(symbol, set, fset, values, &bar_characters, &current_set, &f_state);

    /* Encode the data */
    __encode_data(set, fset, src, values, &bar_characters, &current_set, &f_state, length);

    /* Destination setting and check digit calculation */
    memcpy(d, C128Table[values[0]], 6);
    d += 6;
    total_sum = values[0];

    for (i = 1; i < bar_characters; i++, d += 6)
    {
        memcpy(d, C128Table[values[i]], 6);
        total_sum += values[i] * i; /* Note can't overflow as 106 * 60 * 60 = 381600 */
    }
    total_sum %= 103;
    memcpy(d, C128Table[total_sum], 6);
    d += 6;
    values[bar_characters++] = total_sum;

    /* Stop character */
    memcpy(d, "2331112", 7);
    d += 7;
    values[bar_characters++] = 106;

    // if (symbol->debug & BARCODE_DEBUG_PRINT)
    // {
    //     fputs("Codewords:", stdout);
    //     for (i = 0; i < bar_characters; i++)
    //     {
    //         printf(" %d", values[i]);
    //     }
    //     printf(" (%d)\n", bar_characters);
    //     printf("Barspaces: %.*s\n", (int)(d - dest), dest);
    //     printf("Checksum:  %d\n", total_sum);
    // }

    gui_barcode_gen_expand(symbol, dest, d - dest);

    (void)gui_barcode_gen_hrt_cpy_iso8859_1(symbol, src, length); /* Truncation can't happen */
    // APP_PRINT_TRACE1("symbol->text %s", TRACE_STRING(symbol->text));

    return symbol;//error_number;
}

