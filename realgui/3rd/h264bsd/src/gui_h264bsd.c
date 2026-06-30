/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include "gui_h264bsd.h"


int gui_h264bsd_get_frame(void *gui_decoder, uint8_t *frame_buff, uint32_t buff_size)
{
    GUI_H264BSD_DECODER *gui_dec = gui_decoder;

    if (!gui_dec || !gui_dec->decoder || !frame_buff)
    {
        return H264BSD_ERR_NULL_PTR;
    }
    storage_t *decoder = gui_dec->decoder;

    // register outside buffer
    gui_dec->decoder->conversionBuffer = (u32 *)frame_buff;
    gui_dec->decoder->conversionBufferSize = buff_size;
    while (gui_dec->len > 0)
    {
        gui_dec->status = h264bsdDecode(decoder, gui_dec->byteStrm, gui_dec->len, 0,
                                        (u32 *) & (gui_dec->bytesRead));

        if (gui_dec->status == H264BSD_PIC_RDY)
        {
            u32 picId, isIdrPic, numErrMbs;
            // ++numFrames;
            // gui_log("h264 frame %f \n", numFrames);
            h264bsdNextOutputPictureBGR565(decoder, &picId, &isIdrPic, &numErrMbs);

            gui_dec->byteStrm += gui_dec->bytesRead;
            gui_dec->len -= gui_dec->bytesRead;
            gui_dec->status = H264BSD_RDY;
            break;
        }

        if (gui_dec->status == H264BSD_ERROR)
        {
#ifdef __WIN32
            printf("General Error with %i bytes left\n", gui_dec->len);
#endif
            return H264BSD_ERR;
        }

        if (gui_dec->status == H264BSD_PARAM_SET_ERROR)
        {
#ifdef __WIN32
            printf("Param Set Error with %i bytes left\n", gui_dec->len);
#endif
            return H264BSD_ERR_PARAM;
        }

        if (gui_dec->status == H264BSD_MEMALLOC_ERROR)
        {
#ifdef __WIN32
            printf("Malloc Error with %i bytes left\n", gui_dec->len);
#endif
            return H264BSD_ERR_MEMALLOC;
        }

        gui_dec->byteStrm += gui_dec->bytesRead;
        gui_dec->len -= gui_dec->bytesRead;
        gui_dec->status = H264BSD_RDY;
    }

    if (gui_dec->len <= 0)
    {
        gui_dec->len = gui_dec->fileSize_totla;
        gui_dec->byteStrm = gui_dec->file_data;
        return H264BSD_ERR_EOF;
    }


    return H264BSD_SUCCESS;
}

int gui_h264bsd_rewind(void *gui_decoder)
{
    GUI_H264BSD_DECODER *gui_dec = gui_decoder;

    if (!gui_dec || !gui_dec->decoder)
    {
        return H264BSD_ERR_NULL_PTR;
    }

    gui_dec->len = gui_dec->fileSize_totla;
    gui_dec->byteStrm = gui_dec->file_data;
    return H264BSD_SUCCESS;
}

int gui_h264bsd_set_src(void *gui_decoder, uint8_t *fileData, long fileSize_totla)
{
    GUI_H264BSD_DECODER *gui_dec = gui_decoder;

    if (!gui_dec || !gui_dec->decoder || !fileData)
    {
        return H264BSD_ERR_NULL_PTR;
    }

    gui_dec->fileSize_totla = fileSize_totla;
    gui_dec->file_data = fileData;
    gui_dec->len = gui_dec->fileSize_totla;
    gui_dec->byteStrm = gui_dec->file_data;
    return H264BSD_SUCCESS;
}

int gui_h264bsd_destroy_decoder(void *gui_decoder)
{
    GUI_H264BSD_DECODER *gui_dec = gui_decoder;

    if (!gui_dec || !gui_dec->decoder)
    {
        return H264BSD_ERR_NULL_PTR;
    }
    storage_t *decoder = gui_dec->decoder;

    // free out of decoder, disable here
    decoder->conversionBuffer = 0;
    decoder->conversionBufferSize = 0;
    h264bsdShutdown(decoder);
    h264bsdFree(decoder);

    h264bsd_free(gui_decoder);
    return H264BSD_SUCCESS;
}

void *gui_h264bsd_create_decoder(uint8_t *fileData, long fileSize)
{
    GUI_H264BSD_DECODER *gui_decoder = NULL;

    if (fileData == NULL) { return NULL; }

    gui_decoder = (GUI_H264BSD_DECODER *)h264bsd_malloc(sizeof(GUI_H264BSD_DECODER));
    if (!gui_decoder)
    {
        return NULL;
    }
    memset(gui_decoder, 0, sizeof(GUI_H264BSD_DECODER));
    gui_decoder->decoder = h264bsdAlloc();
    gui_decoder->status = h264bsdInit(gui_decoder->decoder, 0);
    if (gui_decoder->status > 0)
    {
        h264bsdFree(gui_decoder->decoder);
        h264bsd_free(gui_decoder);
        return NULL;
    }

    gui_decoder->fileSize_totla = fileSize;
    gui_decoder->file_data = fileData;
    gui_decoder->len = fileSize;
    gui_decoder->byteStrm = fileData;

    return (void *)gui_decoder;
}

/*============================================================================*
 *      Streaming API -- one Access Unit (one coded picture) per call
 *============================================================================*/

void *gui_h264bsd_create_decoder_stream(void)
{
    GUI_H264BSD_DECODER *gui_decoder =
        (GUI_H264BSD_DECODER *)h264bsd_malloc(sizeof(GUI_H264BSD_DECODER));
    if (!gui_decoder)
    {
        return NULL;
    }
    memset(gui_decoder, 0, sizeof(GUI_H264BSD_DECODER));

    gui_decoder->decoder = h264bsdAlloc();
    gui_decoder->status  = h264bsdInit(gui_decoder->decoder, 0);
    if (gui_decoder->status > 0)
    {
        h264bsdFree(gui_decoder->decoder);
        h264bsd_free(gui_decoder);
        return NULL;
    }

    /* No source bound: the stream is fed one AU at a time by decode_au. */
    return (void *)gui_decoder;
}

int gui_h264bsd_decode_au(void *gui_decoder, const uint8_t *au, uint32_t au_len,
                          uint8_t *frame_buff, uint32_t buff_size)
{
    GUI_H264BSD_DECODER *gui_dec = gui_decoder;

    if (!gui_dec || !gui_dec->decoder || !au || !frame_buff)
    {
        return H264BSD_ERR_NULL_PTR;
    }

    storage_t *decoder  = gui_dec->decoder;
    u8        *byteStrm = (u8 *)au;
    u32        len      = au_len;
    u32        bytesRead = 0;
    u32        status;

    /* Convert straight into the caller's buffer (no internal allocation when
     * buff_size is large enough -- see h264bsdNextOutputPictureBGR565). */
    decoder->conversionBuffer     = (u32 *)frame_buff;
    decoder->conversionBufferSize = buff_size;

    /* Feed NAL units of this AU until the picture completes.  h264bsdDecode
     * consumes one NAL per call; HDRS_RDY returns readBytes==0 and re-decodes
     * the same NAL on the next call (prevBufNotFinished), so a zero advance is
     * expected and must not be treated as an error -- mirrors get_frame. */
    while (len > 0)
    {
        status = h264bsdDecode(decoder, byteStrm, len, 0, (u32 *)&bytesRead);

        if (status == H264BSD_PIC_RDY)
        {
            u32 picId, isIdrPic, numErrMbs;
            h264bsdNextOutputPictureBGR565(decoder, &picId, &isIdrPic, &numErrMbs);
            return H264BSD_SUCCESS;
        }
        if (status == H264BSD_ERROR)
        {
            return H264BSD_ERR;
        }
        if (status == H264BSD_PARAM_SET_ERROR)
        {
            return H264BSD_ERR_PARAM;
        }
        if (status == H264BSD_MEMALLOC_ERROR)
        {
            return H264BSD_ERR_MEMALLOC;
        }

        byteStrm += bytesRead;
        len      -= bytesRead;
    }

    /* AU carried no complete picture (e.g. parameter sets only). */
    return H264BSD_ERR_EOF;
}

int gui_h264bsd_destroy_stream(void *gui_decoder)
{
    GUI_H264BSD_DECODER *gui_dec = gui_decoder;

    if (!gui_dec || !gui_dec->decoder)
    {
        return H264BSD_ERR_NULL_PTR;
    }
    storage_t *decoder = gui_dec->decoder;

    /* conversionBuffer is caller-owned (the widget render buffer); detach it so
     * h264bsdShutdown does not try to free it. */
    decoder->conversionBuffer     = 0;
    decoder->conversionBufferSize = 0;
    h264bsdShutdown(decoder);
    h264bsdFree(decoder);

    h264bsd_free(gui_decoder);
    return H264BSD_SUCCESS;
}
