#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include "alipay_common.h"

typedef struct alipay_tansit_CardBaseVO
{
    char  cardNo[40];
    char  cardType[40];
    char  title[40];
} alipay_tansit_CardBaseVO_t;


EXTERNC bool utransport_get_precheck_status(PARAM_IN bool inlineFlag);

EXTERNC retval_t utransport_get_cardList(PARAM_OUT alipay_tansit_CardBaseVO_t *card_list,
                                         PARAM_INOUT uint32_t *len_card_list, PARAM_OUT uint32_t *card_num, PARAM_IN bool inlineFlag);

EXTERNC retval_t he110670(PARAM_IN char *cardNo, PARAM_IN char *cardType,
                          PARAM_OUT uint8_t *transitcode, PARAM_INOUT uint32_t *len_transitcode, PARAM_IN bool inlineFlag);

EXTERNC void upay_vendor_recv_data_handle(PARAM_IN const uint8_t *data, PARAM_IN uint16_t len);

EXTERNC void utransport_trigger_update(void);

EXTERNC retval_t transport_get_thelast_transitCode(PARAM_OUT char *title,
                                                   PARAM_OUT uint8_t *transitcode, PARAM_OUT uint32_t *len_transitcode);

#endif