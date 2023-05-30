#ifndef __WRISTBAND_HEART_RATE_H__
#define __WRISTBAND_HEART_RATE_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "stdbool.h"



void send_heart_data(void);
void WristBandHeartDataBlockInit(void);
bool WristBandHeartRateDataStore(uint8_t *buf, uint16_t len);
static bool WristBandHeartRateDataRestore(uint8_t *buf, uint16_t len);



#ifdef __cplusplus
}
#endif

#endif
