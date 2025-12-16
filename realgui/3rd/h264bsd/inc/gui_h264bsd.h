

#ifndef __GUI_H264BSD__H___
#define __GUI_H264BSD__H___


#include <stdint.h>
#include "h264bsd_decoder.h"
#include "h264bsd_port.h"

typedef enum
{
    H264BSD_SUCCESS  = 0,
    H264BSD_ERR,
    H264BSD_ERR_NULL_PTR,
    H264BSD_ERR_PARAM,
    H264BSD_ERR_MEMALLOC,
    H264BSD_ERR_EOF,
} H264BSD_ERR_CODE;


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


/**
 * Reset the decoder to replay, source data should be reloaded externally first.
 *
 * This function resets the specified H.264 decoder instance so that the video can be replayed
 * from the beginning. It is important to reload the source data externally before calling
 * this function to ensure proper decoding.
 *
 * @param gui_decoder Pointer to the decoder instance.
 *
 * @return Returns H264BSD_SUCCESS if the reset is successful, or other H264BSD_ERR_CODE if there is a failure.
 */
int gui_h264bsd_rewind(void *gui_decoder);

int gui_h264bsd_set_src(void *gui_decoder, uint8_t *fileData, long fileSize_totla);

/**
 * Retrieve a decoded RGB frame from the decoder.
 *
 * This function retrieves a decoded RGB frame from the specified H.264 decoder instance
 * and stores the frame data in the provided buffer.
 *
 * @param gui_decoder Pointer to the decoder instance.
 * @param frame_buff Pointer to the buffer where the decoded RGB frame data will be stored.
 * @param buff_size Size of the provided buffer in bytes.
 *
 * @return Returns H264BSD_SUCCESS if the frame is successfully retrieved, or other H264BSD_ERR_CODE if there is a failure.
 */
int gui_h264bsd_get_frame(void *gui_decoder, uint8_t *frame_buff, uint32_t buff_size);


/**
 * Destroy a decoder instance.
 *
 * This function destroys an existing H.264 decoder instance and frees associated resources.
 *
 * @param gui_decoder Pointer to the decoder instance to be destroyed.
 *
 * @return Returns 0 if the decoder is successfully destroyed, or a H264BSD_ERR_CODE if there is a failure.
 */
int gui_h264bsd_destroy_decoder(void *gui_decoder);

/**
 * Create an H.264 decoder instance.
 *
 * This function creates a decoder by using the provided H.264 file data and file size,
 * and returns a pointer to the decoder.
 *
 * @param fileData Pointer to the H.264 file data. Note: The decoder will modify the source data,
 *                 so ensure that the memory area pointed by fileData is writable.
 * @param fileSize The size of the file data in bytes.
 *
 * @return Returns a pointer to the decoder upon success, or NULL upon failure.
 */
void *gui_h264bsd_create_decoder(uint8_t *fileData, long fileSize);



#endif
