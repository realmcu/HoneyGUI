/*
 * File      : mptool.h
 */
#ifndef __MPTOOL_H__
#define __MPTOOL_H__

#include <rtdef.h>


#ifdef __cplusplus
extern "C" {
#endif

#define ARRAY_TO_UINT16(p) (((*(p)) & 0xff) + ((*((p)+1)) << 8))

/** Get local uint32_t from external 4 uint8_t, Little-Endian format  */
#define ARRAY_TO_UINT32(p) (((uint32_t)(*((p)+0)) & 0xff) + ((uint32_t)(*((p)+1)) << 8) \
                            + ((uint32_t)(*((p)+2)) << 16)  + ((uint32_t)(*((p)+3)) << 24))
/** PUT external 2 uint8_t from local uint16_t, Little-Endian Format */
#define UINT16_TO_ARRAY(p,w)                                                      \
    {*((uint8_t *)p)     = (uint8_t)((uint16_t)(w) & 0xff);                          \
        *(((uint8_t *)p)+1) = /*lint -e(572,778)*/ (uint8_t)(((uint16_t)(w)>>8) & 0xff);}

/** PUT external 4 uint8_t from local uint32_t, Little endian Format */
#define UINT32_TO_ARRAY(p,w)                                  \
    {*((uint8_t *)p)     = (uint8_t)((uint32_t)(w) & 0xff);      \
        *(((uint8_t *)p)+1) = (uint8_t)(((uint32_t)(w)>>8) & 0xff);  \
        *(((uint8_t *)p)+2) = (uint8_t)(((uint32_t)(w)>>16) & 0xff); \
        *(((uint8_t *)p)+3) = (uint8_t)(((uint32_t)(w)>>24) & 0xff);}

#define LE_EXTRN2DWORD(p) (((unsigned long)(*((p)+0)) & 0xff) + ((unsigned long)(*((p)+1)) << 8) \
                           + ((unsigned long)(*((p)+2)) << 16)  + ((unsigned long)(*((p)+3)) << 24))

#define MP_RST_PIN          P3_4
#define MP_LOG_PIN          P3_5
#define FW_PACKET_SIZE      128
#define IMAGE_PACKET_SIZE   (1024*4)
#define IMAGE_START_ADDR    0x0082C000

#define SUB_FILE_DATA_OFFSET    512

union sub_file_ind
{
    uint32_t    value;
    uint8_t     byte[4];
};

#ifdef __cplusplus
}
#endif

#endif

