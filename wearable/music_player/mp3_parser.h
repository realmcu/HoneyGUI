// Abbrevations:
// ofs = offset             ver = version           hdl = handle
// frm = frame              sqn = sequence          curr = current
// pos = position           hdr = header            au = audio


// Prefix:
// g = global variable      j = static variable     k = const
// p = pointer              a = array
// u = unsigned integer     s = signed integer
// f = float                d = double
// c = char                 b = bool
// S/z = struct             E/e = enum              U/n = union
// v = void                 x = unknown/unconcerned type
// h = handle


#ifndef MP3_PARSER_H
#define MP3_PARSER_H


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
//#include "os_mem.h"
//#include "trace.h"




//#define MP3_PARSER_USE_LIBC

#ifdef MP3_PARSER_USE_LIBC
#define MPS_PARSER_ALLOC(...)               malloc(__VA_ARGS__)
#define MPS_PARSER_FREE                     free
#define MPS_PARSER_LOG(...)                 printf(__VA_ARGS__)
#else
//#include <rtthread.h>
#include "trace.h"
#define MPS_PARSER_ALLOC(...)               malloc(__VA_ARGS__)
#define MPS_PARSER_FREE                     free
#define MPS_PARSER_LOG(...)                 DBG_DIRECT(__VA_ARGS__)
#endif



#ifndef NDEBUG
#undef ASSERT
#define ASSERT(e) \
    do \
    { \
        if(!(e)) \
        { \
            MPS_PARSER_LOG("(" #e ") assert failed! Func: %s. Line: %d.", __func__, __LINE__); \
            MPS_PARSER_LOG("(" #e ") assert failed! Func: %s. Line: %d.", __func__, __LINE__); \
            for(;;); \
        } \
    } while(0)
#else
#define ASSERT(e) ((void)0)
#endif

// (SamplesPerFrame / 32kHz * 40kB/s + padding). Also enough for MPEG2 and MPEG2.5.
#define MAX_POSSIBLE_FRAME_BYTES    (1152/32*40+1)
#define TOC_SIZE                    100
#define MP3_BUF_BYTES (MAX_POSSIBLE_FRAME_BYTES + sizeof(MP3_FHEADER))


typedef enum
{
    ftyp = 0,
    moov,
    mvhd,
    meta,
    trak,
    tkhd,
    tref,
    trgr,
    edts,
    mdia,
    mdhd,
    mdlr,
    hdlr,
    elng,
    minf,
    vmhd,
    smhd,
    hmhd,
    sthd,
    nmhd,
    dinf,
    dref,
    stbl,
    stsd,
    stts,
    ctts,
    cslg,
    stsc,
    stsz,
    stco,
    co64,
    stss,
    stsh,
    padb,
    stdp,
    sdtp,
    sbgp,
    sgpd,
    subs,
    saiz,
    saio,
    udta,
    mvex,
    mehd,
    trex,
    leva,
    moof,
    mfhd,
    traf,
    tfhd,
    trun,
    iods,
    stz2,
    mdat,
} MP4_BOX_TYPE;

typedef struct
{
    uint8_t size[4];
    uint8_t type[4]; /* 32bit */
} MP4_BOX_HEADER;

typedef struct
{
    uint8_t     header[8];
    uint8_t     resved0[6];
    uint16_t    dataRefInx;
    uint8_t     resved1[8];
    uint16_t    channelCount;
    uint16_t    sampleCount;
    uint16_t    preDefined;
    uint8_t     resved2[2];
    uint32_t    sampleRate;
    uint8_t     ESDescriptor[54];
} MP4_AUDIO_DESCRIPT;

typedef struct
{
    uint16_t           version;
    uint8_t            flags;
    uint32_t           count;
    MP4_AUDIO_DESCRIPT mp4AudioDescript;
} MP4_STSD_BOX;

typedef struct
{
    uint64_t totalFrameCnt;
    uint32_t frameTableStartOffset;
    uint32_t frameTableLength;
    uint32_t chunkTableOffset;
    uint32_t chunkTableLength;
    uint32_t mdatOffset;
    uint32_t mdatLength;
    uint32_t curFrameNum;
} MP4_HEADER_INFO;

typedef enum
{
    CHANNEL_STEREO       = 0,
    CHANNEL_JOINT_STEREO = 1,
    CHANNEL_DOUBLE       = 2,
    CHANNEL_SINGLE       = 3,
} MP3_CHANNEL_MODE, EMp3ChannelMode;

typedef enum
{
    MPEG_2P5            = 0,
    VERSION_NONE        = 1,
    MPEG_2              = 2,
    MPEG_1              = 3,
} MP3_VERSION, EMp3Ver;

typedef enum
{
    LAYER_NONE          = 0,
    LAYER_3             = 1,
    LAYER_2             = 2,
    LAYER_1             = 3,
} MP3_LAYER, EMp3Layer;

typedef struct
{
    uint8_t sync1: 8;
    uint8_t error_protection: 1;
    uint8_t layer: 2;
    uint8_t version: 2;
    uint8_t sync2: 3;
    uint8_t extension: 1;
    uint8_t padding: 1;
    uint8_t sample_rate_index: 2;
    uint8_t bit_rate_index: 4;
    uint8_t emphasis: 2;
    uint8_t original: 1;
    uint8_t copyright: 1;
    uint8_t mode_extension: 2;
    uint8_t channel_mode: 2;
} MP3_FHEADER, SMp3FrmHdr;

typedef struct
{
    uint8_t  header[4];
    uint8_t  xingFlag[4];
    uint8_t  totalFrameLen[4];
    uint8_t  fileSize[4];
} XING_HEADER_INFO;

typedef enum
{
    AOT_SPECIFIC_CONFIG     = 0, /* Defined in AOT Specifc Config */
    CHANNEL_1PCS            = 1, /* front-center */
    CHANNEL_2PCS            = 2, /* front-left,front-right */
    CHANNEL_3PCS            = 3, /* front-center,front-left, front-right */
    CHANNEL_4PCS            = 4, /* front-center, front-left, front-right, back-center */
    CHANNEL_5PCS            = 5, /* front-center, front-left, front-right, back-left, back-right */
    CHANNEL_6PCS            = 6, /* front-center, front-left, front-right, back-left, back-right, LFE-channel */
    CHANNEL_7PCS            = 7, /* front-center, front-left, front-right,
                                    side-left, side-right, back-left, back-right, LFE-channel */
} AAC_CHANNEL_MODE;

typedef struct
{
    uint16_t sync1: 12; /* sync word, ADTS  always 0xFFF; LATM: */
    uint8_t ID: 1;      /* 0 MPEG-4   1 MPEG-2*/
    uint8_t layer: 2;   /* always '00' */
    uint8_t protection_absent: 1; /* 0 has CRC   1 no CRC */
    uint8_t profile: 2; /* 0 main profile
                          1 low complexity profile(LC)
                          2 scalable sampling rate profile(SSR)
                          3 reserved*/
    uint8_t sample_rate_index: 4;
    uint8_t private_bit: 1;
    uint8_t channel_configuration: 3;
    uint8_t original_copy: 1;
    uint8_t home: 1;
} ADTS_FIXED_FHEADER;

typedef struct
{
    uint8_t copyright_identification_bit: 1;
    uint8_t copyright_identification_start: 1;
    uint16_t aac_frame_length: 13; /* size(AACFrame)=(ADTS header+AAC stream);
                                     aac_frame_length=(protection_absent == 1 ? 7 : 9) + size(AACFrame)*/
    uint16_t adts_buffer_fullness: 11; /* 0x7FF variable code rate */
    uint8_t number_of_raw_data_blocks_in_frame: 2;
} ADTS_VARIABLE_FHEADER;

/*============================================================================*
  *                                  Variables
  *============================================================================*/
typedef enum
{
    TIT2 = 0,
    TALB,
    TCOM,
    TPE1,
    TPE2,
    TCON,
    TXXX,
    TOFN,
    TIME,

    ALL_TAGS,
} ID3V2_TAG;

typedef struct
{
    uint8_t *info;
    uint16_t length;
} ID3V2_TAG_INFO;

typedef struct
{
    uint32_t startOffset;
    uint32_t length;
} ID3V2_INFO;

typedef enum
{
    ID3V2_HEADER            = 0x10,
    ID3V1_HEADER            = 0x20,
    MP3_HEADER              = 0x30,
    XING_HEADER             = 0x40,
    AAC_HEADER              = 0x50,
    XING_OR_MP3_HEADER      = 0x60,
    RTK_ADTS_HEADER         = 0x70,
    RTK_LATM_HEADER         = 0x80,
    RTK_SBC_HEADER          = 0x90,
    MP4_HEADER              = 0xA0,
    HEADER_TYPE_NONE        = 0xFF,
} HEADER_TYPE;

typedef struct
{
    uint8_t                 frameNum;
    uint16_t                length;   /* frame length: before decode */
    uint8_t                 *content;
} FRAME_CONTENT;

typedef enum
{
    UNKNOWN     = 0,
    RTK         = 0x10,
    AAC         = 0x20,
    MP3         = 0x30,
    MP4         = 0x40,
    FLAC        = 0x50,
} AUDIO_FILE_FORMAT;

typedef enum
{
    MP3_CBR     = 0x31,
    MP3_VBR     = 0x32,
    MP3_VBRI    = 0x33,
} MP3_SUB_FORMAT;

typedef enum
{
    RTK_SBC      = 0x11,
    RTK_ADTS     = 0x12,
    RTK_LATM     = 0x13,
} RTK_TRANSPORT_FORMAT;

typedef struct
{
    uint32_t    sampling_frequency;
    uint8_t     channel_mode;
    uint8_t     block_length;
    uint8_t     subbands;
    uint8_t     allocation_method;
    uint8_t     bitpool;
} SBC_FRAME_INFO;

typedef struct
{
    uint32_t    sampling_frequency;
    uint8_t     channel_mode;
    uint16_t    frame_duration;
    uint32_t    bit_rate;
} ADTS_FRAME_INFO;

typedef struct
{
    uint32_t    sampling_frequency;
    uint8_t     channel_mode;
} LATM_FRAME_INFO;

typedef struct
{
    RTK_TRANSPORT_FORMAT rtkTransFormat;
    union
    {
        SBC_FRAME_INFO sbc;
        ADTS_FRAME_INFO adts;
        LATM_FRAME_INFO latm;
    } rtk_trans_info;
} RTK_FRAME_INFO;

typedef struct
{
    uint32_t    sampling_frequency;
    uint8_t     channel_mode;
    uint16_t    frame_duration;
} AAC_FRAME_INFO;

typedef struct
{
    MP3_SUB_FORMAT    mp3_sub_format;
    uint32_t          sampling_frequency;
    uint8_t           channel_mode;
    uint8_t           version;
    uint8_t           layer;
    uint32_t          bit_rate;
    uint32_t          total_frames;
    uint32_t          total_file_size;
    uint16_t          frame_size_after_decode; /* frame size: after decode */
    uint16_t          sample_counts;
    uint16_t          frame_duration;
} MP3_FRAME_INFO;

typedef struct
{
    uint32_t          sampling_frequency;
    uint8_t           channel_mode;
    uint32_t          bit_rate;
    uint32_t          total_frames;
    uint32_t          total_file_size;
    uint16_t          sample_counts;
} MP4_FRAME_INFO;

typedef struct
{
    AUDIO_FILE_FORMAT format;
    union
    {
        RTK_FRAME_INFO  rtk;
        AAC_FRAME_INFO  aac;
        MP3_FRAME_INFO  mp3;
        MP4_FRAME_INFO  mp4;
    } format_info;
} FRAME_INFO;

typedef enum
{
    AUDIO_FRAME = 1, // CBR
    XING_FRAME,     // VBR
    INFO_FRAME,     // CBR or VBR. More likely to be CBR.
} E1stFrmType;

typedef struct
{
    // Buffers:
    //FILE *xFil;
    int xFil;
    // Used to perform interal operations and return frame data.
    uint8_t auBuf[MP3_BUF_BYTES];

    // Id3v1/Id3v2 info:
    bool bIsId3v1Exist;
    uint8_t uId3v2Count; // There may be more than 1 id3v2 tag.

    // Audio parameters:
    uint32_t uSamplingFreq_Hz;
    uint32_t uBitRate_kbps; // Only valid when CBR.

    uint32_t uSamplePerFrm;
    float fBytesPerFrm; // Only valid when CBR.
    float fTimePerFrm_ms;
    uint32_t uTotalFrmNum; // -1 if get failed.
    float fTotalPlayTime_ms; // 0 if get failed.

    uint32_t uMaxPossibleFrmBytes;

    uint32_t u1stAuFrmOfs;
    SMp3FrmHdr z1stAuFrmHdr;
    E1stFrmType e1stFrmType;

    // f_read position:
    uint32_t uReadOfs;
    uint32_t uReadSqnNum;

    // Parameters get from Xing/Info frame,
    // only valid when e1stFrmType == XING_FRAME or INFO_FRAME:
    uint8_t uFlag;
    uint32_t uFileLen;
    uint8_t auTocTable[TOC_SIZE];
} *Mp3Hdl_t;


typedef enum
{
    MP3RES_OK = 0,

    MP3RES_MALLOC_FAILED,
    MP3RES_FOPEN_ERROR, // 2
    MP3RES_FLSEEK_ERROR,
    MP3RES_FREAD_ERROR,
    MP3RES_FLSEEK_OR_FREAD_ERROR, //5
    MP3RES_UNSUPPORTED_FORMAT,
    MP3RES_SEARCH_FRAME_FAILED, // 7
    MP3RES_NO_PLAY_TIME_INFO,
    MP3RES_POS_BEYOND_END,
    MP3RES_VBR_WITHOUT_TOC_INFO,  // 10
    MP3RES_ID3V1_NOT_EXIST,
    MP3RES_ID3V2_NOT_EXIST,
    MP3RES_ID3V2_FRAME_NOT_EXIST, // 13
    MP3RES_FILE_ENDS,
    MP3RES_OTHER_ERR,
} EMp3Res;


/** @param[in]  pxFileName: File name in file system.
  * @param[out]  peRes: Operation result. May be MP3RES_OK, MP3RES_MALLOC_FAILED, MP3RES_FLSEEK_OR_FREAD_ERROR, MP3RES_UNSUPPORTED_FORMAT, MP3RES_OTHER_ERR.
  *
  * @return Handle. NULL if operation failed.
  */
Mp3Hdl_t Mp3_CreateHandle(const char *pxFileName, EMp3Res *peRes);

/** @note Do not free handle that is alread freed.
  */
EMp3Res Mp3_FreeHandle(Mp3Hdl_t hMp3);

EMp3Ver Mp3_GetVersion(Mp3Hdl_t hMp3);
EMp3Layer Mp3_GetLayer(Mp3Hdl_t hMp3);
uint32_t Mp3_GetSamplingFrequency_Hz(Mp3Hdl_t hMp3);
uint32_t Mp3_GetBitRate_kbps(Mp3Hdl_t hMp3);
EMp3ChannelMode Mp3_GetChannelMode(Mp3Hdl_t hMp3);
uint32_t Mp3_GetSamplePerFrame(Mp3Hdl_t hMp3);
float Mp3_GetTimePerFrame_ms(Mp3Hdl_t hMp3);

/*** @return 0.0f if failed. Others if successes.
  */
float Mp3_GetTotalPlayTime_ms(Mp3Hdl_t hMp3);

/** @note Check returned value before use buffer data.
  * @param[in]  hMp3: Handle.
  * @param[out]  ppuBuf: Return a buffer that contains 128 bytes id3v1 info if operation successes. Buffer data keep valid until another mp3 api is invoked.
  * @param[out]  puLen: Data bytes in buffer. Return 128 if operation successes.
  *
  * @return MP3RES_OK, MP3RES_ID3V1_NOT_EXIST, MP3RES_FLSEEK_OR_FREAD_ERROR, MP3RES_OTHER_ERR.
  */
EMp3Res Mp3_ReadId3v1(Mp3Hdl_t hMp3, uint8_t **ppuBuf, uint32_t *puLen);

/** @brief Retrieve id3v2 frame by frame ID.
  * @note Check returned value before use buffer data.
  * @param[in]  hMp3: Handle.
  * @param[in]  acFrmId: id3v2 frame id. Eg. TIT2, TPE1, TABL...
  * @param[out]  ppuBuf: Return a buffer that contains id3v2 frame if operation successes. Buffer data keep valid until another mp3 api is invoked.
  * @param[out]  puLen: Data bytes in buffer.
  *
  * @return MP3RES_OK, MP3RES_ID3V2_NOT_EXIST, MP3RES_ID3V2_FRAME_NOT_EXIST, MP3RES_FLSEEK_OR_FREAD_ERROR, MP3RES_OTHER_ERR.
  */
EMp3Res Mp3_RetrieveId3v2Frame(Mp3Hdl_t hMp3, const char acFrmId[4], uint8_t **ppuBuf,
                               uint32_t *puLen);

/** @note Check returned value before use buffer data.
  * @param[in]  hMp3: Handle.
  * @param[out]  ppuBuf: Return a buffer that contains audio frame if operation successes. Buffer data keep valid until another mp3 api is invoked.
  * @param[out]  puLen: Data bytes in buffer.
  * @param[out]  pfPlayPos_ms: Current play position in ms.
  *
  * @return MP3RES_OK, MP3RES_FILE_ENDS, MP3RES_FLSEEK_OR_FREAD_ERROR, MP3RES_OTHER_ERR.
  */
EMp3Res Mp3_ReadNextFrame(Mp3Hdl_t hMp3, uint8_t **ppuBuf, uint32_t *puLen, float *pfPlayPos_ms);

EMp3Res Mp3_SetPlayPos(Mp3Hdl_t hMp3, float fPos_ms);




#endif // MP3_PARSER_H
