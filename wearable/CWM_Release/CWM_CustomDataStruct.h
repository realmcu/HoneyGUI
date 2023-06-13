#ifndef __CWM_CUSTOMDATASTRUCT_H__
#define __CWM_CUSTOMDATASTRUCT_H__

typedef struct CustomSensorData
{
    uint32_t sensorType;
    union
    {
        float fData[16];
        double dData[10];
    };
} CustomSensorData;

#endif /* __CWM_CUSTOMDATASTRUCT_H__ */
