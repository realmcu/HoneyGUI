// h264bsdTest.cpp : Defines the entry point for the console application.
//

#include <stdio.h>

#ifdef __WIN32
#include <SDKDDKVer.h>
#include <tchar.h>
#include <Windows.h>
#endif


#include "h264bsd_decoder.h"
#include "gui_api.h"


#ifdef __arm__
#include "rtl_pinmux.h"
#include "rtl_gpio.h"
#endif

#if 1
static u8 isFirst = 1;
static long fileSize_totla;
u8 *frame_buf_dc;
int h264bsd_test(u8 *fileData, long fileSize, u8 *frame_buf)
{
    static storage_t *decoder = NULL;
    static u32 len = 0;
    u32 *picData = 0;
    static u8 *byteStrm;
    static double numFrames = 0;
    static u32 bytesRead = 0;
    static u32 status = H264BSD_RDY;

#ifdef __WIN32
    static LARGE_INTEGER frequency_li;
    static double frequency;
    static LARGE_INTEGER start;

#endif

    frame_buf_dc = frame_buf;
    if (fileData == NULL) { return 1; }

    if (isFirst)
    {
        fileSize_totla = fileSize;
        len = fileSize;
        byteStrm = fileData;

#ifdef __WIN32
        QueryPerformanceFrequency(&frequency_li);
        frequency = (double)(frequency_li.QuadPart);
        QueryPerformanceCounter(&start);
#endif


        decoder = h264bsdAlloc();
        status = h264bsdInit(decoder, 0);
        if (status > 0)
        {
            return 2;
        }
        isFirst = 0;
    }

    // while(1) {
    // fseek(input, 0L, SEEK_SET);
    // size_t inputRead = fread(fileData, sizeof(u8), fileSize, input);


    while (len > 0)
    {
        status = h264bsdDecode(decoder, byteStrm, len, 0, &bytesRead);

        if (status == H264BSD_PIC_RDY)
        {
            ++numFrames;
            u32 picId, isIdrPic, numErrMbs;
            // gui_log("h264 frame %f \n", numFrames);
            // u8* picData = h264bsdNextOutputPicture(decoder, &picId, &isIdrPic, &numErrMbs);
            // picData = h264bsdNextOutputPictureBGRA(decoder, &picId, &isIdrPic, &numErrMbs);
// #ifdef __arm__
//             Pad_Config(P1_0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
// #endif

            picData = h264bsdNextOutputPictureBGR565(decoder, &picId, &isIdrPic, &numErrMbs);
// #ifdef __arm__
//             Pad_Config(P1_0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_LOW);
// #endif


            byteStrm += bytesRead;
            len -= bytesRead;
            status = H264BSD_RDY;
            break;
        }

        if (status == H264BSD_ERROR)
        {
#ifdef __WIN32
            printf("General Error with %i bytes left\n", len);
#endif
            return -1;
        }

        if (status == H264BSD_PARAM_SET_ERROR)
        {
#ifdef __WIN32
            printf("Param Set Error with %i bytes left\n", len);
#endif
            return -2;
        }

        if (status == H264BSD_MEMALLOC_ERROR)
        {
#ifdef __WIN32
            printf("Malloc Error with %i bytes left\n", len);
#endif
            return -3;
        }

        byteStrm += bytesRead;
        len -= bytesRead;
        status = H264BSD_RDY;
    }

    if (len <= 0)
    {
        isFirst = 1;
        h264bsdShutdown(decoder);
        h264bsdFree(decoder);

#ifdef __WIN32
        LARGE_INTEGER end;
        QueryPerformanceCounter(&end);
        double decodeTime = (double)(end.QuadPart - start.QuadPart) / frequency;
        printf("Decoded %.0f frames in %f seconds (%f fps or %f ms per frame)\n", numFrames, decodeTime,
               numFrames / decodeTime, decodeTime / numFrames * 1000.0);
#endif
        decoder = NULL;
        numFrames = 0;
        return 3;
    }

    // }

    return 0;
}

#endif