/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief     A more light-weight mp3 file parser than audio_fs.c
   * @details
   * @author    zengguang_hu
   * @date      2022-6-28
   * @version   1.0
   **************************************************************************************
*/


// Abbrevations:
// ofs = offset             ver = version           btr = bytes to read
// br = bytes read          btw = bytes to write    bw = bytes written
// hdr = header             idx = index             hdl = handle
// frm = frame              sqn = sequence          curr = current
// pos = position           au = audio


// Prefix:
// g = global variable      j = static variable     k = const
// p = pointer              a = array
// u = unsigned integer     s = signed integer
// f = float                d = double
// c = char                 b = bool
// S/z = struct             E/e = enum              U/n = union
// v = void                 x = unknown/unconcerned type
// h = handle


#include "mp3_parser.h"

#include <string.h>
#ifdef OS_RTTHREAD
#include "dfs_posix.h"
#endif
#ifdef OS_FREERTOS
#include "romfs.h"
#endif
#define ID3V1_BYTES         128
#define ID3V2_HEADER_SIZE   10

#define FRAME_NUM_FLAG_MASK             0x01
#define FILE_LEN_FLAG_MASK              0x02
#define TOC_FLAG_MASK                   0x04

#define EXTENDED_HEADER_FLAG_MASK       (1 << 6)

#define MIN(a, b) ((a) < (b) ? (a) : (b))


typedef struct
{
    char acHeader[3];
    uint8_t uVer;
    uint8_t uRevision;
    uint8_t uFlag;
    uint8_t auSize[4];
} __attribute__((packed)) SId3v2Hdr;

typedef struct
{
    uint8_t auExtendedHdrSize[4];
    uint8_t auExtendedFlags[2];
    uint8_t auSizeOfPadding[4];
} __attribute__((packed)) SId3v2ExtendedHdr;

typedef struct
{
    char acFrameId[4];
    uint8_t auSize[4];
    uint8_t auFlags[2];
} __attribute__((packed)) SId3v2FrmHdr;


static EMp3Res InitId3v1Info(Mp3Hdl_t hMp3);
static EMp3Res InitId3v2Info(Mp3Hdl_t hMp3);
static EMp3Res InitAudioFrameInfo(Mp3Hdl_t hMp3);
static inline bool IsFrameHeaderValid(const SMp3FrmHdr *pzHdrToCheck,
                                      const SMp3FrmHdr *pz1stAuFrmHdr);
static inline uint32_t GetSamplingFrequency_Hz(const SMp3FrmHdr *pzFrmHdr);
static inline uint32_t GetBitRate_kbps(const SMp3FrmHdr *pzFrmHdr);
static inline uint32_t GetSamplePerFrm(const SMp3FrmHdr *pzFrmHdr);
static inline uint32_t ComputeFrameBytes(const SMp3FrmHdr *pzFrmHdr);
static inline uint32_t GetTotalFrameBytes(Mp3Hdl_t hMp3);
static inline uint32_t XingInfoOfs(const SMp3FrmHdr *pzFrmHdr);
static inline uint32_t FlagOfs(const SMp3FrmHdr *pzFrmHdr);
static inline uint32_t FrmNumOfs(const SMp3FrmHdr *pzFrmHdr);
static inline uint32_t FileLenOfs(const SMp3FrmHdr *pzFrmHdr);
static inline uint32_t TocOfs(const SMp3FrmHdr *pzFrmHdr);
static inline bool IsCbr(Mp3Hdl_t hMp3);
static inline uint32_t GetBigEndianUint32From(const void *pvAddr);
static EMp3Res SearchFrameFrom(int pxFil, uint8_t *puBuf, uint32_t uBufLen, uint32_t uStartOfs,
                               const SMp3FrmHdr *pz1stAuFrmHdr, uint32_t *puFoundOfs, uint32_t *puFrmBytes);
//static bool SeekRead(FILE *fp, long int ofs, void *read, size_t btr, size_t *br);
static bool SeekRead(int fp, long int ofs, void *read, size_t btr, size_t *br);



Mp3Hdl_t Mp3_CreateHandle(const char *pxFileName, EMp3Res *peRes)
{
    *peRes = MP3RES_OK;
    DBG_DIRECT("Mp3_CreateHandle %d\n", __LINE__);
    Mp3Hdl_t hMp3 = MPS_PARSER_ALLOC(sizeof(*hMp3));
    if (hMp3 == NULL)
    {
        *peRes = MP3RES_MALLOC_FAILED;
        goto Err1;
    }
    DBG_DIRECT("Mp3_CreateHandle %d\n", __LINE__);
    hMp3->xFil = open(pxFileName, O_RDONLY | O_BINARY, 0);
    if (hMp3->xFil == NULL)
    {
        *peRes = MP3RES_FOPEN_ERROR;
        goto Err2;
    }
    DBG_DIRECT("Mp3_CreateHandle %d\n", __LINE__);
    *peRes = InitId3v1Info(hMp3);
    if (*peRes != MP3RES_OK)
    {
        goto Err3;
    }
    DBG_DIRECT("Mp3_CreateHandle %d\n", __LINE__);
    *peRes = InitId3v2Info(hMp3);
    if (*peRes != MP3RES_OK)
    {
        goto Err3;
    }
    DBG_DIRECT("Mp3_CreateHandle %d\n", __LINE__);
    *peRes = InitAudioFrameInfo(hMp3);
    if (*peRes != MP3RES_OK)
    {
        goto Err3;
    }
    DBG_DIRECT("Mp3_CreateHandle %d\n", __LINE__);
    return hMp3;

Err3:
    close(hMp3->xFil);
Err2:
    MPS_PARSER_FREE(hMp3);
Err1:
    return NULL;
}

EMp3Res Mp3_FreeHandle(Mp3Hdl_t hMp3)
{
    if (hMp3 == NULL)
    {
        return MP3RES_OK;
    }

    close(hMp3->xFil);
    MPS_PARSER_FREE(hMp3);

    return MP3RES_OK;
}

EMp3Ver Mp3_GetVersion(Mp3Hdl_t hMp3)
{
    return (EMp3Ver)hMp3->z1stAuFrmHdr.version;
}

EMp3Layer Mp3_GetLayer(Mp3Hdl_t hMp3)
{
    return (EMp3Layer)hMp3->z1stAuFrmHdr.layer;
}

uint32_t Mp3_GetSamplingFrequency_Hz(Mp3Hdl_t hMp3)
{
    return hMp3->uSamplingFreq_Hz;
}

uint32_t Mp3_GetBitRate_kbps(Mp3Hdl_t hMp3)
{
    return hMp3->uBitRate_kbps;
}

EMp3ChannelMode Mp3_GetChannelMode(Mp3Hdl_t hMp3)
{
    return (EMp3ChannelMode)hMp3->z1stAuFrmHdr.channel_mode;
}

float Mp3_GetTotalPlayTime_ms(Mp3Hdl_t hMp3)
{
    return hMp3->fTotalPlayTime_ms;
}

uint32_t Mp3_GetSamplePerFrame(Mp3Hdl_t hMp3)
{
    return hMp3->uSamplePerFrm;
}

float Mp3_GetTimePerFrame_ms(Mp3Hdl_t hMp3)
{
    return hMp3->fTimePerFrm_ms;
}

EMp3Res Mp3_ReadNextFrame(Mp3Hdl_t hMp3, uint8_t **ppuBuf, uint32_t *puLen, float *pfPlayPos_ms)
{
    //ASSERT(hMp3 != NULL && ppuBuf != NULL && puLen != NULL && pfPlayPos_ms != NULL);
    EMp3Res eRes;
    size_t uBr;

    // Don't invoke SearchFrameFrom() straightly, since it will read too much data causing inefficient.

    if (SeekRead(hMp3->xFil, hMp3->uReadOfs, hMp3->auBuf, hMp3->uMaxPossibleFrmBytes, &uBr) != true)
    {
        return MP3RES_FLSEEK_OR_FREAD_ERROR;
    }

    SMp3FrmHdr *pzFrmHdr = (SMp3FrmHdr *)hMp3->auBuf;
    if (!IsFrameHeaderValid(pzFrmHdr, &hMp3->z1stAuFrmHdr) || ComputeFrameBytes(pzFrmHdr) > uBr)
    {
        eRes = SearchFrameFrom(hMp3->xFil, hMp3->auBuf, sizeof(hMp3->auBuf), hMp3->uReadOfs,
                               &hMp3->z1stAuFrmHdr, &hMp3->uReadOfs, puLen);
        if (eRes == MP3RES_SEARCH_FRAME_FAILED)
        {
            return MP3RES_FILE_ENDS;
        }
        else if (eRes != MP3RES_OK)
        {
            return eRes;
        }
    }

    *ppuBuf = hMp3->auBuf;
    *puLen = ComputeFrameBytes(pzFrmHdr);
    *pfPlayPos_ms = hMp3->fTimePerFrm_ms * hMp3->uReadSqnNum;

    ++hMp3->uReadSqnNum;
    hMp3->uReadOfs += *puLen;

    return MP3RES_OK;
}

EMp3Res Mp3_SetPlayPos(Mp3Hdl_t hMp3, float fPos_ms)
{
    if (hMp3->fTotalPlayTime_ms == 0.0f)
    {
        return MP3RES_NO_PLAY_TIME_INFO;
    }

    if (fPos_ms > hMp3->fTotalPlayTime_ms || fPos_ms < 0.0f)
    {
        return MP3RES_POS_BEYOND_END;
    }

    // Update hMp3->uReadOfs.
    if (IsCbr(hMp3))
    {
        uint32_t uOfsFrom1stFrame = fPos_ms / hMp3->fTotalPlayTime_ms * GetTotalFrameBytes(hMp3);
        hMp3->uReadOfs = hMp3->u1stAuFrmOfs + uOfsFrom1stFrame;

        uint32_t uFrmBytes;
        // Adjust hMp3->uReadOfs to the beginning of a frame.
        SearchFrameFrom(hMp3->xFil, hMp3->auBuf, sizeof(hMp3->auBuf), hMp3->uReadOfs,
                        &hMp3->z1stAuFrmHdr, &hMp3->uReadOfs, &uFrmBytes);
    }
    else // VBR
    {
        if (!(hMp3->uFlag & TOC_FLAG_MASK))
        {
            return MP3RES_VBR_WITHOUT_TOC_INFO;
        }

        uint8_t uTocIdx = fPos_ms / hMp3->fTotalPlayTime_ms * TOC_SIZE;
        if (uTocIdx == TOC_SIZE)
        {
            // Set read offset to end to cause MP3RES_FILE_ENDS when Mp3_ReadNextFrame();
            //fseek(hMp3->xFil, 0, SEEK_END);
            //hMp3->uReadOfs = ftell(hMp3->xFil);
            hMp3->uReadOfs = lseek(hMp3->xFil, 0, SEEK_END);
        }
        else
        {
            uint32_t uOfsFrom1stFrame = hMp3->auTocTable[uTocIdx] / 256.0f * GetTotalFrameBytes(hMp3);
            hMp3->uReadOfs  = hMp3->u1stAuFrmOfs + uOfsFrom1stFrame;

            // Fine tune offset using Mp3_ReadNextFrame();
            float fGap = fPos_ms - uTocIdx / 100.0f * hMp3->fTotalPlayTime_ms;
            uint32_t uFrms = (fGap > 0.0f) ? fGap / hMp3->fTimePerFrm_ms : 0;
            MPS_PARSER_LOG("uPos_ms: %f, uTocIdx: %u, auTocTable[uTocIdx]: %u, fGap: %f, uFrms: %u \n",
                           fPos_ms, uTocIdx, hMp3->auTocTable[uTocIdx], fGap, uFrms);
            while (uFrms--)
            {
                uint8_t *puBuf;
                uint32_t uLen;
                float fCurrPlayTime_ms;
                Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
            }
        }
    }
    hMp3->uReadSqnNum = fPos_ms / hMp3->fTotalPlayTime_ms * hMp3->uTotalFrmNum;

    return MP3RES_OK;
}

EMp3Res Mp3_ReadId3v1(Mp3Hdl_t hMp3, uint8_t **ppuBuf, uint32_t *puLen)
{
    size_t uBr;
    size_t filesize = 0;
    //fseek(hMp3->xFil, 0, SEEK_END);
    //filesize = ftell(hMp3->xFil);
    filesize = lseek(hMp3->xFil, 0, SEEK_END);
    if (!hMp3->bIsId3v1Exist)
    {
        return MP3RES_ID3V1_NOT_EXIST;
    }

    size_t uId3v1Ofs = filesize - ID3V1_BYTES;
    if (SeekRead(hMp3->xFil, uId3v1Ofs, hMp3->auBuf, ID3V1_BYTES, &uBr) != true)
    {
        return MP3RES_FLSEEK_OR_FREAD_ERROR;
    }

    *ppuBuf = hMp3->auBuf;
    *puLen = ID3V1_BYTES;
    return MP3RES_OK;
}

EMp3Res Mp3_RetrieveId3v2Frame(Mp3Hdl_t hMp3, const char acFrmId[4], uint8_t **ppuBuf,
                               uint32_t *puLen)
{
    size_t uBr;

    if (hMp3->uId3v2Count == 0)
    {
        return MP3RES_ID3V2_NOT_EXIST;
    }

    uint32_t uId3v2Bytes;
    for (uint32_t uId3v2Ofs = 0;
         uId3v2Ofs < hMp3->u1stAuFrmOfs - sizeof(SId3v2Hdr);  // Can read an id3v2 header at least.
         uId3v2Ofs += uId3v2Bytes)
    {
        SId3v2Hdr zId3v2Hdr;
        if (SeekRead(hMp3->xFil, uId3v2Ofs, &zId3v2Hdr, sizeof(SId3v2Hdr), &uBr) != true)
        {
            return MP3RES_FLSEEK_OR_FREAD_ERROR;
        }
        else if (uBr != sizeof(zId3v2Hdr))
        {
            return MP3RES_ID3V2_NOT_EXIST;
        }

        uId3v2Bytes = sizeof(SId3v2Hdr) +
                      (((zId3v2Hdr.auSize[0] & 0x7fUL) << 21) | ((zId3v2Hdr.auSize[1] & 0x7fUL) << 14) |
                       ((zId3v2Hdr.auSize[2] & 0x7fUL) << 7) | (zId3v2Hdr.auSize[3] & 0x7fUL));

        // Only search id3v2.3 and id3v2.4.
        if (zId3v2Hdr.uVer != 3 && zId3v2Hdr.uVer != 4)
        {
            continue;
        }

        uint32_t uExtendedHdrBytes;
        if (!(zId3v2Hdr.uFlag & EXTENDED_HEADER_FLAG_MASK))
        {
            uExtendedHdrBytes = 0;
        }
        else
        {
            SId3v2ExtendedHdr zExtHdr;
            if (SeekRead(hMp3->xFil, uId3v2Ofs + sizeof(SId3v2Hdr), &zExtHdr, sizeof(zExtHdr), &uBr) != true ||
                uBr != sizeof(zExtHdr))
            {
                return MP3RES_FLSEEK_OR_FREAD_ERROR;
            }

            uExtendedHdrBytes = GetBigEndianUint32From(zExtHdr.auExtendedHdrSize)
                                + sizeof(zExtHdr.auExtendedHdrSize);
        }

        uint32_t uFrmBytes;
        for (uint32_t uFrmOfs = sizeof(SId3v2Hdr) + uExtendedHdrBytes;
             uFrmOfs < uId3v2Bytes - sizeof(SId3v2FrmHdr); // Can read a frame header at least.
             uFrmOfs += uFrmBytes)
        {
            uint32_t uFrmOfsFromBegin = uId3v2Ofs + uFrmOfs;

            SId3v2FrmHdr zFrmHdr;
            if (SeekRead(hMp3->xFil, uFrmOfsFromBegin, &zFrmHdr, sizeof(SId3v2FrmHdr), &uBr) != true)
            {
                return MP3RES_FLSEEK_OR_FREAD_ERROR;
            }
            else if (uBr != sizeof(SId3v2FrmHdr))
            {
                return MP3RES_ID3V2_FRAME_NOT_EXIST;
            }

            uint32_t uSizeExcludingHdr = GetBigEndianUint32From(zFrmHdr.auSize);
            if (uSizeExcludingHdr == 0) // Left bytes are padding "00000000...".
            {
                break;    // Search next id3v2 tag.
            }

            uFrmBytes = uSizeExcludingHdr + sizeof(SId3v2FrmHdr);

            if (memcmp(zFrmHdr.acFrameId, acFrmId, 4) == 0) // Frame is found.
            {
                uint32_t uBtr = MIN(uFrmBytes, sizeof(hMp3->auBuf));
                if (SeekRead(hMp3->xFil, uFrmOfsFromBegin, hMp3->auBuf, uBtr, &uBr) != true || uBr != uBtr)
                {
                    return MP3RES_FLSEEK_OR_FREAD_ERROR;
                }

                *ppuBuf = hMp3->auBuf;
                *puLen = uBr;
                return MP3RES_OK;
            }
        }
    }

    return MP3RES_ID3V2_FRAME_NOT_EXIST;
}

static EMp3Res InitId3v1Info(Mp3Hdl_t hMp3)
{
    size_t filesize;
    size_t uBr;

    //fseek(hMp3->xFil, 0, SEEK_END);
    //filesize = ftell(hMp3->xFil);
    filesize = lseek(hMp3->xFil, 0, SEEK_END);
    if (filesize < ID3V1_BYTES)
    {
        hMp3->bIsId3v1Exist = false;
        return MP3RES_OK;
    }

    char acHdr[3];
    size_t uId3v1Ofs = filesize - ID3V1_BYTES;
    if (SeekRead(hMp3->xFil, uId3v1Ofs, acHdr, sizeof(acHdr), &uBr) != true || uBr != sizeof(acHdr))
    {
        return MP3RES_FLSEEK_OR_FREAD_ERROR;
    }

    hMp3->bIsId3v1Exist = (memcmp(acHdr, "TAG", 3) == 0) ? true : false;
    return MP3RES_OK;
}

static EMp3Res InitId3v2Info(Mp3Hdl_t hMp3)
{
    size_t uBr;
    size_t filesize = 0;
    //fseek(hMp3->xFil, 0, SEEK_END);
    //filesize = ftell(hMp3->xFil);
    filesize = lseek(hMp3->xFil, 0, SEEK_END);
    if (filesize < ID3V2_HEADER_SIZE)
    {
        hMp3->uId3v2Count = 0;
        hMp3->u1stAuFrmOfs = 0;
        return MP3RES_OK;
    }

    // There may be more than 1 id3v2 tag.
    hMp3->uId3v2Count = 0;
    uint32_t uId3v2Bytes;
    uint32_t uOfs;

    for (uOfs = 0; uOfs < filesize; uOfs += uId3v2Bytes)
    {
        SId3v2Hdr zHdr;
        if (SeekRead(hMp3->xFil, uOfs, &zHdr, sizeof(SId3v2Hdr), &uBr) != true || uBr != sizeof(SId3v2Hdr))
        {
            return MP3RES_FLSEEK_OR_FREAD_ERROR;
        }

        if (memcmp(zHdr.acHeader, "ID3", 3) == 0)
        {
            ++hMp3->uId3v2Count;
        }
        else
        {
            break;
        }

        uId3v2Bytes = sizeof(SId3v2Hdr) +
                      (((zHdr.auSize[0] & 0x7fUL) << 21) | ((zHdr.auSize[1] & 0x7fUL) << 14) |
                       ((zHdr.auSize[2] & 0x7fUL) << 7) | (zHdr.auSize[3] & 0x7fUL));
    }
    hMp3->u1stAuFrmOfs = uOfs;

    return MP3RES_OK;
}

static EMp3Res InitAudioFrameInfo(Mp3Hdl_t hMp3)
{
    EMp3Res eRes;

    // 1. Get info from first frame and first audio frame.
    uint32_t uFrameBytes;
    // hMp3->u1stAuFrmOfs was initialed in InitId3v2Info().
    eRes = SearchFrameFrom(hMp3->xFil, hMp3->auBuf, sizeof(hMp3->auBuf), hMp3->u1stAuFrmOfs,
                           NULL, &hMp3->u1stAuFrmOfs, &uFrameBytes);
    if (eRes != MP3RES_OK)
    {
        return eRes;
    }

    SMp3FrmHdr *pzHdr = (SMp3FrmHdr *)hMp3->auBuf;

    if (memcmp(&hMp3->auBuf[XingInfoOfs(pzHdr)], "Xing", 4) == 0)
    {
        hMp3->e1stFrmType = XING_FRAME;
    }
    else if (memcmp(&hMp3->auBuf[XingInfoOfs(pzHdr)], "Info", 4) == 0)
    {
        hMp3->e1stFrmType = INFO_FRAME;
    }
    else
    {
        hMp3->e1stFrmType = AUDIO_FRAME;
    }

    uint32_t uFrmNumInXing = 0;
    if (hMp3->e1stFrmType == XING_FRAME || hMp3->e1stFrmType == INFO_FRAME)
    {
        // Get info from Xing/Info frame.
        hMp3->uFlag = hMp3->auBuf[FlagOfs(pzHdr)];
        if (hMp3->uFlag & FILE_LEN_FLAG_MASK)
        {
            hMp3->uFileLen = GetBigEndianUint32From(&hMp3->auBuf[FileLenOfs(pzHdr)]);
        }

        if (hMp3->uFlag & TOC_FLAG_MASK)
        {
            memcpy(hMp3->auTocTable, &hMp3->auBuf[TocOfs(pzHdr)], TOC_SIZE);
        }

        if (hMp3->uFlag & FRAME_NUM_FLAG_MASK)
        {
            uFrmNumInXing = GetBigEndianUint32From(&hMp3->auBuf[FrmNumOfs(pzHdr)]) - 1;
        }
        else // Failed. If you really want to get it, you need to tranverse the file.
        {
            uFrmNumInXing = (uint32_t) - 1;
        }

        // Search 1st audio frame.
        hMp3->u1stAuFrmOfs += uFrameBytes;
        eRes = SearchFrameFrom(hMp3->xFil, hMp3->auBuf, sizeof(hMp3->auBuf), hMp3->u1stAuFrmOfs,
                               NULL, &hMp3->u1stAuFrmOfs, &uFrameBytes);
        if (eRes != MP3RES_OK)
        {
            return eRes;
        }
    }

    memcpy(&hMp3->z1stAuFrmHdr, hMp3->auBuf, sizeof(hMp3->z1stAuFrmHdr));
    hMp3->uSamplePerFrm = GetSamplePerFrm(&hMp3->z1stAuFrmHdr);
    hMp3->uSamplingFreq_Hz = GetSamplingFrequency_Hz(&hMp3->z1stAuFrmHdr);
    hMp3->uBitRate_kbps = GetBitRate_kbps(&hMp3->z1stAuFrmHdr);
    hMp3->fTimePerFrm_ms = (float)hMp3->uSamplePerFrm / hMp3->uSamplingFreq_Hz * 1000;
    hMp3->fBytesPerFrm =
        (float)hMp3->uSamplePerFrm / hMp3->uSamplingFreq_Hz * hMp3->uBitRate_kbps / 8 * 1000;

    hMp3->uTotalFrmNum = IsCbr(hMp3) ? (GetTotalFrameBytes(hMp3) / hMp3->fBytesPerFrm) : uFrmNumInXing;

    hMp3->uMaxPossibleFrmBytes = IsCbr(hMp3) ?
                                 (uint32_t)hMp3->fBytesPerFrm + 1 :
                                 (uint32_t)((float)hMp3->uSamplePerFrm / hMp3->uSamplingFreq_Hz * 320 / 8 * 1000) + 1;

    hMp3->uReadOfs = hMp3->u1stAuFrmOfs;
    hMp3->uReadSqnNum = 0;

    hMp3->fTotalPlayTime_ms = (hMp3->uTotalFrmNum == (uint32_t) - 1) ?
                              0.0f :
                              hMp3->fTimePerFrm_ms * hMp3->uTotalFrmNum;

    return MP3RES_OK;
}

static inline bool IsFrameHeaderValid(const SMp3FrmHdr *pzHdrToCheck,
                                      const SMp3FrmHdr *pz1stAuFrmHdr)
{
    if (pzHdrToCheck->sync1 == 0xff && pzHdrToCheck->sync2 == 0x7 &&
        pzHdrToCheck->version != VERSION_NONE &&
        pzHdrToCheck->layer == LAYER_3 &&
        pzHdrToCheck->bit_rate_index != 0xf && pzHdrToCheck->bit_rate_index != 0x0 &&
        // Free bitrate is not supported.
        pzHdrToCheck->sample_rate_index != 0x3 &&
        pzHdrToCheck->emphasis != 0x2)
    {
        if (pz1stAuFrmHdr != NULL) // Check consistency.
        {
            return pzHdrToCheck->version == pz1stAuFrmHdr->version &&
                   pzHdrToCheck->sample_rate_index == pz1stAuFrmHdr->sample_rate_index &&
                   pzHdrToCheck->channel_mode == pz1stAuFrmHdr->channel_mode;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

static inline uint32_t XingInfoOfs(const SMp3FrmHdr *pzFrmHdr)
{
    uint32_t uOfs;
    if (pzFrmHdr->version == MPEG_1 && pzFrmHdr->channel_mode != CHANNEL_SINGLE)
    {
        uOfs = 36;
    }
    else if (pzFrmHdr->version == MPEG_2 && pzFrmHdr->channel_mode == CHANNEL_SINGLE)
    {
        uOfs = 13;
    }
    else
    {
        uOfs = 21;
    }
    return uOfs;
}

static inline uint32_t FlagOfs(const SMp3FrmHdr *pzFrmHdr)
{
    return XingInfoOfs(pzFrmHdr) + 4 + 3;
}

static inline uint32_t FrmNumOfs(const SMp3FrmHdr *pzFrmHdr)
{
    return XingInfoOfs(pzFrmHdr) + 8;
}

static inline uint32_t FileLenOfs(const SMp3FrmHdr *pzFrmHdr)
{
    return XingInfoOfs(pzFrmHdr) + 12;
}

static inline uint32_t TocOfs(const SMp3FrmHdr *pzFrmHdr)
{
    return XingInfoOfs(pzFrmHdr) + 16;
}

static inline uint32_t GetSamplingFrequency_Hz(const SMp3FrmHdr *pzFrmHdr)
{
#define BAD ((uint32_t)-1)
    const uint32_t auSamplingFreq_Hz[4][4] =
    {
        {11025, 12000, 8000,  BAD},
        {BAD,   BAD,   BAD,   BAD},
        {22050, 24000, 16000, BAD},
        {44100, 48000, 32000, BAD},
    };
#undef BAD
    return auSamplingFreq_Hz[pzFrmHdr->version][pzFrmHdr->sample_rate_index];
}

static inline uint32_t GetBitRate_kbps(const SMp3FrmHdr *pzFrmHdr)
{
    // ASSERT(pzFrmHdr->layer == LAYER_3);
#define BAD ((uint32_t)-1)
    const uint32_t auBitRateV1[16] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, BAD};
    const uint32_t auBitRateV2[16] = {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, BAD};

    switch (pzFrmHdr->version)
    {
    case MPEG_1:
        return auBitRateV1[pzFrmHdr->bit_rate_index];
    case MPEG_2:
    case MPEG_2P5:
        return auBitRateV2[pzFrmHdr->bit_rate_index];
    default:
        return BAD;
    }
#undef BAD
}

static inline uint32_t GetSamplePerFrm(const SMp3FrmHdr *pzFrmHdr)
{
    ASSERT(pzFrmHdr->layer == LAYER_3);
    const uint32_t auSampleTable[4] = {576, (uint32_t) - 1, 576, 1152};
    return auSampleTable[pzFrmHdr->version];
}

static inline uint32_t ComputeFrameBytes(const SMp3FrmHdr *pzFrmHdr)
{
    // Don't adjust the order of calculation, or may cause unexpected round off. Eg: 720 -> 719.999936.
    return (uint32_t)
           ((float)GetSamplePerFrm(pzFrmHdr) * (1000.0f / 8.0f) / GetSamplingFrequency_Hz(
                pzFrmHdr) * GetBitRate_kbps(pzFrmHdr))
           + pzFrmHdr->padding;
}

static inline uint32_t GetBigEndianUint32From(const void *pvAddr)
{
    const uint8_t *pu = pvAddr;
    return (pu[0] << 24UL) | (pu[1] << 16UL) | (pu[2] << 8UL) | pu[3];
}

// If search success, frame data will be in puBuf,
// frame offset will be in *puFoundOfs, and length of frame data will be in *puFrmBytes.
static EMp3Res SearchFrameFrom(int pxFil, uint8_t *puBuf, uint32_t uBufLen, uint32_t uStartOfs,
                               const SMp3FrmHdr *pz1stAuFrmHdr, uint32_t *puFoundOfs, uint32_t *puFrmBytes)
{
    size_t uBr;

    if (SeekRead(pxFil, uStartOfs, puBuf, uBufLen, &uBr) != true)
    {
        return MP3RES_FLSEEK_OR_FREAD_ERROR;
    }
    else if (uBr < sizeof(SMp3FrmHdr))
    {
        return MP3RES_FILE_ENDS;
    }

    uint32_t i, uMaxSearchTime = uBr - sizeof(SMp3FrmHdr) + 1;
    for (i = 0; i < uMaxSearchTime; ++i)
    {
        // ?? : Does unaligned access works well?
        SMp3FrmHdr *pzHdr = (SMp3FrmHdr *)(puBuf + i);
        if (IsFrameHeaderValid(pzHdr, pz1stAuFrmHdr))
        {
            break;
        }
    }
    if (i == uMaxSearchTime)
    {
        return MP3RES_SEARCH_FRAME_FAILED;
    }

    *puFoundOfs = uStartOfs + i;
    *puFrmBytes = ComputeFrameBytes((SMp3FrmHdr *)(puBuf + i));

    if (i != 0 || uBr < *puFrmBytes)
    {
        if (SeekRead(pxFil, *puFoundOfs, puBuf, *puFrmBytes, &uBr) != true)
        {
            return MP3RES_FLSEEK_OR_FREAD_ERROR;
        }
    }

    return MP3RES_OK;
}

// Must parse id3v2 and id3v1 in advance.
static inline uint32_t GetTotalFrameBytes(Mp3Hdl_t hMp3)
{
    size_t filesize = 0;
    //fseek(hMp3->xFil, 0, SEEK_END);
    //filesize = ftell(hMp3->xFil);
    filesize = lseek(hMp3->xFil, 0, SEEK_END);
    return filesize
           - (hMp3->bIsId3v1Exist ? ID3V1_BYTES : 0) // Id3v1 bytes.
           - hMp3->u1stAuFrmOfs; // Id3v2 bytes;
}

//static bool SeekRead(FILE *fp, long int ofs, void *buf, size_t btr, size_t *br)
static bool SeekRead(int fp, long int ofs, void *buf, size_t btr, size_t *br)
{

//    rt_kprintf("[SeekRead]fp = %x, ofs = %d, btr = %d \n", fp, ofs, btr);
//    if (lseek(fp, ofs, SEEK_SET) != 0)
//    {
//        return false;
//    }
    if (lseek(fp, ofs, SEEK_SET) < 0)
    {
        return false;
    }
//    *br = fread(read, 1, btr, fp);
    *br = read(fp, buf, btr);
    return true;
}

static inline bool IsCbr(Mp3Hdl_t hMp3)
{
    return (hMp3->e1stFrmType == XING_FRAME) ? false : true;
}


#ifdef OS_RTTHREAD

char *audio_name = "/music/f04short_raw.mp3";//please use ANSI encode

void Mp3_Test(const char *pxFileName)
{
    EMp3Res eRes;

    pxFileName = (const char *)audio_name;

    rt_kprintf("!!!!!! void Mp3_Test(const char *pxFileName) !!!!!! \n");


    Mp3Hdl_t hMp3 = Mp3_CreateHandle(pxFileName, &eRes);
    rt_kprintf("Mp3Hdl_t hMp3 = Mp3_CreateHandle(pxFileName, &eRes);""%d \n", eRes);
    rt_kprintf("%d, %d, %d, %d \n", eRes, MP3_BUF_BYTES, hMp3->bIsId3v1Exist, hMp3->uId3v2Count);
    rt_kprintf("0x%08x \n", *(uint32_t *)&hMp3->z1stAuFrmHdr);
    rt_kprintf("%d, %d \n", hMp3->uSamplingFreq_Hz, hMp3->uBitRate_kbps);
    rt_kprintf("%f, %f, %u, %f \n", hMp3->fBytesPerFrm, hMp3->fTimePerFrm_ms, hMp3->uTotalFrmNum,
               hMp3->fTotalPlayTime_ms);
    rt_kprintf("%d, 0x%x, %d \n", hMp3->uMaxPossibleFrmBytes, hMp3->u1stAuFrmOfs, hMp3->e1stFrmType);
    rt_kprintf("0x%x, %d \n", hMp3->uReadOfs, hMp3->uReadSqnNum);
    rt_kprintf("0x%x, 0x%x \n", hMp3->uFlag, hMp3->uFileLen);


    uint8_t *puBuf;
    uint32_t uLen;
    float fCurrPlayTime_ms;
    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    rt_kprintf("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    rt_kprintf("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    rt_kprintf("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    rt_kprintf("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);


    eRes = Mp3_SetPlayPos(hMp3, hMp3->fTotalPlayTime_ms / 2.0f);
    rt_kprintf("eRes = Mp3_SetPlayPos(hMp3, hMp3->fTotalPlayTime_ms / 2.0f);""%d \n", eRes);
    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    rt_kprintf("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    rt_kprintf("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);


    eRes = Mp3_SetPlayPos(hMp3, hMp3->fTotalPlayTime_ms);
    rt_kprintf("eRes = Mp3_SetPlayPos(hMp3, hMp3->fTotalPlayTime_ms);""%d \n", eRes);
    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    rt_kprintf("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    rt_kprintf("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    for (uint32_t i = 0; i < 25; ++i)
    {
        eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
        rt_kprintf("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
        rt_kprintf("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
        rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
                   puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);
    }

    eRes = Mp3_SetPlayPos(hMp3, 20 * 1000);
    rt_kprintf("eRes = Mp3_SetPlayPos(hMp3, 20 * 1000);""%d \n", eRes);
    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    rt_kprintf("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    rt_kprintf("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    eRes = Mp3_SetPlayPos(hMp3, 0);
    rt_kprintf("eRes = Mp3_SetPlayPos(hMp3, 0);""%d \n", eRes);
    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    rt_kprintf("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    rt_kprintf("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);


    eRes = Mp3_ReadId3v1(hMp3, &puBuf, &uLen);
    rt_kprintf("eRes = Mp3_ReadId3v1(hMp3, &puBuf, &uLen);""%d \n", eRes);
    rt_kprintf("%d, %d \n", eRes, uLen);
    rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);


    eRes = Mp3_RetrieveId3v2Frame(hMp3, "TIT2", &puBuf, &uLen);
    rt_kprintf("eRes = Mp3_RetrieveId3v2Frame(hMp3, \"TIT2\", &puBuf, &uLen);""%d \n", eRes);
    rt_kprintf("%d, %d \n", eRes, uLen);
    rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    eRes = Mp3_RetrieveId3v2Frame(hMp3, "TPE1", &puBuf, &uLen);
    rt_kprintf("eRes = Mp3_RetrieveId3v2Frame(hMp3, \"TPE1\", &puBuf, &uLen);""%d \n", eRes);
    rt_kprintf("%d, %d \n", eRes, uLen);
    rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    eRes = Mp3_RetrieveId3v2Frame(hMp3, "TSSE", &puBuf, &uLen);
    rt_kprintf("eRes = Mp3_RetrieveId3v2Frame(hMp3, \"TSSE\", &puBuf, &uLen);""%d\n", eRes);
    rt_kprintf("%d, %d \n", eRes, uLen);
    rt_kprintf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    Mp3_FreeHandle(hMp3);
}

MSH_CMD_EXPORT(Mp3_Test, mcu config test);
#endif

#ifdef OS_FREERTOS

#include "shell_port.h"

char *audio_name = "/music/f04short_raw.mp3";//please use ANSI encode

void Mp3_Test(const char *pxFileName)
{
    EMp3Res eRes;

    pxFileName = (const char *)audio_name;

    DBG_DIRECT("!!!!!! void Mp3_Test(const char *pxFileName) !!!!!! \n");


    Mp3Hdl_t hMp3 = Mp3_CreateHandle(pxFileName, &eRes);
    DBG_DIRECT("Mp3Hdl_t hMp3 = Mp3_CreateHandle(pxFileName, &eRes);""%d \n", eRes);
    DBG_DIRECT("%d, %d, %d, %d \n", eRes, MP3_BUF_BYTES, hMp3->bIsId3v1Exist, hMp3->uId3v2Count);
    DBG_DIRECT("0x%08x \n", *(uint32_t *)&hMp3->z1stAuFrmHdr);
    DBG_DIRECT("%d, %d \n", hMp3->uSamplingFreq_Hz, hMp3->uBitRate_kbps);
    DBG_DIRECT("%f, %f, %u, %f \n", hMp3->fBytesPerFrm, hMp3->fTimePerFrm_ms, hMp3->uTotalFrmNum,
               hMp3->fTotalPlayTime_ms);
    DBG_DIRECT("%d, 0x%x, %d \n", hMp3->uMaxPossibleFrmBytes, hMp3->u1stAuFrmOfs, hMp3->e1stFrmType);
    DBG_DIRECT("0x%x, %d \n", hMp3->uReadOfs, hMp3->uReadSqnNum);
    DBG_DIRECT("0x%x, 0x%x \n", hMp3->uFlag, hMp3->uFileLen);


    uint8_t *puBuf;
    uint32_t uLen;
    float fCurrPlayTime_ms;
    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    DBG_DIRECT("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    DBG_DIRECT("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    DBG_DIRECT("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    DBG_DIRECT("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);


    eRes = Mp3_SetPlayPos(hMp3, hMp3->fTotalPlayTime_ms / 2.0f);
    DBG_DIRECT("eRes = Mp3_SetPlayPos(hMp3, hMp3->fTotalPlayTime_ms / 2.0f);""%d \n", eRes);
    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    DBG_DIRECT("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    DBG_DIRECT("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);


    eRes = Mp3_SetPlayPos(hMp3, hMp3->fTotalPlayTime_ms);
    DBG_DIRECT("eRes = Mp3_SetPlayPos(hMp3, hMp3->fTotalPlayTime_ms);""%d \n", eRes);
    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    DBG_DIRECT("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    DBG_DIRECT("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    for (uint32_t i = 0; i < 25; ++i)
    {
        eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
        DBG_DIRECT("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
        DBG_DIRECT("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
        DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
                   puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);
    }

    eRes = Mp3_SetPlayPos(hMp3, 20 * 1000);
    DBG_DIRECT("eRes = Mp3_SetPlayPos(hMp3, 20 * 1000);""%d \n", eRes);
    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    DBG_DIRECT("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    DBG_DIRECT("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    eRes = Mp3_SetPlayPos(hMp3, 0);
    DBG_DIRECT("eRes = Mp3_SetPlayPos(hMp3, 0);""%d \n", eRes);
    eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &fCurrPlayTime_ms);
    DBG_DIRECT("eRes = Mp3_ReadNextFrame(hMp3, &puBuf, &uLen, &uCurrPlayTime_ms);""%d \n", eRes);
    DBG_DIRECT("%d, %d, %f \n", eRes, uLen, fCurrPlayTime_ms);
    DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);


    eRes = Mp3_ReadId3v1(hMp3, &puBuf, &uLen);
    DBG_DIRECT("eRes = Mp3_ReadId3v1(hMp3, &puBuf, &uLen);""%d \n", eRes);
    DBG_DIRECT("%d, %d \n", eRes, uLen);
    DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);


    eRes = Mp3_RetrieveId3v2Frame(hMp3, "TIT2", &puBuf, &uLen);
    DBG_DIRECT("eRes = Mp3_RetrieveId3v2Frame(hMp3, \"TIT2\", &puBuf, &uLen);""%d \n", eRes);
    DBG_DIRECT("%d, %d \n", eRes, uLen);
    DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    eRes = Mp3_RetrieveId3v2Frame(hMp3, "TPE1", &puBuf, &uLen);
    DBG_DIRECT("eRes = Mp3_RetrieveId3v2Frame(hMp3, \"TPE1\", &puBuf, &uLen);""%d \n", eRes);
    DBG_DIRECT("%d, %d \n", eRes, uLen);
    DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    eRes = Mp3_RetrieveId3v2Frame(hMp3, "TSSE", &puBuf, &uLen);
    DBG_DIRECT("eRes = Mp3_RetrieveId3v2Frame(hMp3, \"TSSE\", &puBuf, &uLen);""%d\n", eRes);
    DBG_DIRECT("%d, %d \n", eRes, uLen);
    DBG_DIRECT("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x \n",
               puBuf[0], puBuf[1], puBuf[2], puBuf[uLen - 4], puBuf[uLen - 3], puBuf[uLen - 2], puBuf[uLen - 1]);

    Mp3_FreeHandle(hMp3);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    mp3_test, Mp3_Test, mp3 test);
#endif
