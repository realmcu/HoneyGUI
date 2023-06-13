#ifndef __OS_API_H__
#define __OS_API_H__

/**
  * @brief  System/OS/Peripheral API structure.
  */

typedef int (*PFN_IRQHANDLE)(void *);

typedef struct
{
    int (*i2cRead)(uint16_t slaveAddr, uint16_t reg, int regLength, uint8_t *readData, int readDataSize,
                   int argument);
    int (*i2cWrite)(uint16_t slaveAddr, uint16_t reg, int regLength, uint8_t *writeData,
                    int writeDataSize, int argument);
    void *(*malloc)(int size);
    void (*free)(void *ptr);
    uint64_t (*GetTimeNs)(void);
    void (*uSleep)(uint32_t);
    void (*saveData)(uint8_t *, uint32_t);
    void (*restoreData)(uint8_t *, uint32_t);
    int (*dbgOutput)(const char *);
    int (*enableGpioIRQ)(uint16_t, uint16_t, PFN_IRQHANDLE);
    int (*disableGpioIRQ)(uint16_t, uint16_t);
    int (*setGpio)(uint16_t, uint16_t, uint16_t);
    int (*securityCodeWrite)(uint32_t, uint8_t *, uint32_t);
    int (*i2cTransfer)(uint16_t slaveAddr, uint16_t reg, int regLength, uint8_t *writeData,
                       int writeDataSize, uint8_t *readData, int readDataSize, int argument);
} OsAPI, *pOsAPI;

#endif /* __OS_API_H__ */
