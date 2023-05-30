#ifndef __WRISTBAND_SLEEP_H__
#define __WRISTBAND_SLEEP_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "stdbool.h"



void send_sleep_data(void);
void WristBandSleepDataBlockInit(void);
bool WristBandSleepDataStore(uint8_t *buf, uint16_t len);
static bool WristBandSleepDataRestore(uint8_t *buf, uint16_t len);



#ifdef __cplusplus
}
#endif

#endif
