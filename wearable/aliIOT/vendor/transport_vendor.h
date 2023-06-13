#ifndef _TRANSPORT_VENDOR_H
#define _TRANSPORT_VENDOR_H

#include "alipay_common.h"

EXTERNC void *upay_open_rsvd_part(PARAM_IN char filename[36]);

EXTERNC int upay_write_rsvd_part(PARAM_IN void *fd, PARAM_IN void *data,
                                 PARAM_IN uint32_t data_len);

EXTERNC int upay_read_rsvd_part(PARAM_IN void *fd, PARAM_OUT void *buffer,
                                PARAM_INOUT uint32_t *read_len);

EXTERNC int upay_close_rsvd_part(PARAM_IN void *fd);

EXTERNC int upay_access_rsvd_part(PARAM_IN char filename[36]);

EXTERNC int upay_remove_rsvd_part(PARAM_IN char filename[36]);

EXTERNC int upay_clear_rsvd_part(void);

EXTERNC void UPAY_ble_write_vendor(PARAM_IN const uint8_t *data, PARAM_IN uint16_t len);

EXTERNC bool UPAY_link_status(void);

EXTERNC void UPAY_sleep_ms(PARAM_IN uint32_t ms);

#endif
