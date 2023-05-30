#ifndef __WRISTBAND_PEDO_H__
#define __WRISTBAND_PEDO_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "stdbool.h"



void send_sport_data(void);
void WristBandPedoDataBlockInit(void);
bool WristBandPedoDataStore(uint8_t *buf, uint16_t len);
static bool WristBandPedoDataRestore(uint8_t *buf, uint16_t len);
void minute_sport_handler(void);
void doNewTimeSettingForPedoData(uint32_t old_sec, uint32_t new_sec);



#ifdef __cplusplus
}
#endif

#endif
