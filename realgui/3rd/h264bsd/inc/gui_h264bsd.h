

#ifndef __GUI_H264BSD__H___
#define __GUI_H264BSD__H___


#include <stdint.h>
#include "h264bsd_decoder.h"
#include "h264bsd_port.h"

enum
{
    H264BSD_SUCCESS  = 0,
    H264BSD_ERR,
    H264BSD_ERR_NULL_PTR,
    H264BSD_ERR_PARAM,
    H264BSD_ERR_MEMALLOC,
    H264BSD_ERR_EOF,
};


typedef struct
{
    uint8_t *file_data;
    long fileSize_totla;
    uint32_t len;
    uint8_t *byteStrm;
    uint32_t bytesRead;
    uint32_t status;
    storage_t *decoder;

    // uint32_t picId;
    // uint32_t isIdrPic;
    // uint32_t numErrMbs;
} GUI_H264BSD_DECODER;

int gui_h264bsd_get_frame(void *gui_decoder, uint8_t *frame_buff, uint32_t buff_size);

int gui_h264bsd_rewind(void *gui_decoder);

int gui_h264bsd_destroy_decoder(void *gui_decoder);

void *gui_h264bsd_create_decoder(uint8_t *fileData, long fileSize);



#endif
