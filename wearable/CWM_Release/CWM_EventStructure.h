#ifndef __CWM_EVENT_STRUCTURE_H__
#define __CWM_EVENT_STRUCTURE_H__

/**
  * @brief  CyweeMotion task handle event structure.
  */
typedef struct
{
    uint32_t tid;
    uint32_t handleType;
    int (*handleEvent)(void *pHandle, uint32_t evtType, void *evtData);
    void *mem;
} CWMHandle_t, *pCWMHandle_t;


/**
  * @brief  Sensor event structure.
  */
#ifdef __CC_ARM                         /* ARM Compiler */
#pragma anon_unions
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
#elif defined (__GNUC__)                /* GNU GCC Compiler */
#elif defined (__ADSPBLACKFIN__)        /* for VisualDSP++ Compiler */
#elif defined (_MSC_VER)
#endif

typedef struct
{
    uint32_t sensorType;
    uint32_t index;
    union
    {
        struct
        {
            float fData[16];
        };
        struct
        {
            double dData[8];
        };
        struct
        {
            int iData[16];
        };
        struct
        {
            int    memSize;
            void *memData;
        };
    };
    uint64_t timestamp_ns;
} SensorEVT_t, *pSensorEVT_t;

#endif /* __CWM_EVENT_STRUCTURE_H__ */
