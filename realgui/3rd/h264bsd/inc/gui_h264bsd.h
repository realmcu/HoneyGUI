

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


/*============================================================================*
 *      Streaming API -- feed one Access Unit (AU) at a time
 *
 *  The functions above are file-oriented: the whole elementary stream is given
 *  up front and the decoder walks it.  The streaming API below keeps a
 *  persistent decoder fed one coded picture (Access Unit) per call, which is
 *  what a live transport delivers.  H.264 is inter-coded and the decoder is
 *  stateful (reference frames in the DPB), so AUs MUST be supplied in order and
 *  never dropped.
 *
 *  Distinct names from the file API so a consumer (e.g. gui_stream) can declare
 *  weak stubs for them without colliding with the file-API weak stubs declared
 *  elsewhere (e.g. gui_video).
 *============================================================================*/

/**
 * Create a decoder with no source bound -- the stream is fed later, one Access
 * Unit at a time, via ::gui_h264bsd_decode_au.
 *
 * @return Pointer to the decoder upon success, or NULL upon failure.
 */
void *gui_h264bsd_create_decoder_stream(void);

/**
 * Decode one Access Unit (one coded picture, possibly multiple slices, with any
 * leading SPS/PPS) and, when a picture completes, convert it to BGR565 into the
 * caller's buffer.
 *
 * The decoder may modify @p au in place (RBSP extraction / emulation-prevention
 * removal), so the buffer must be writable.
 *
 * @param gui_decoder Pointer to a decoder from ::gui_h264bsd_create_decoder_stream.
 * @param au          Annex-B bytes of exactly one Access Unit (with start codes).
 * @param au_len      Length of @p au in bytes.
 * @param frame_buff  Destination for the BGR565 picture (16 bpp).
 * @param buff_size   Size of @p frame_buff in bytes (must be >= width*height*2).
 *
 * @return H264BSD_SUCCESS when a picture was produced into @p frame_buff;
 *         H264BSD_ERR_EOF when the AU carried no complete picture (e.g. only
 *         parameter sets); other ::H264BSD_ERR_CODE on a decode error.
 */
int gui_h264bsd_decode_au(void *gui_decoder, const uint8_t *au, uint32_t au_len,
                          uint8_t *frame_buff, uint32_t buff_size);

/**
 * Destroy a streaming decoder created by ::gui_h264bsd_create_decoder_stream.
 *
 * @param gui_decoder Pointer to the decoder instance to be destroyed.
 *
 * @return H264BSD_SUCCESS on success, or a ::H264BSD_ERR_CODE on failure.
 */
int gui_h264bsd_destroy_stream(void *gui_decoder);


#endif
