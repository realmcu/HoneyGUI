/****************************************************************************
 * IoTPay modules 1.0 2017/10/19
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING
 * ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED. ALIPAY, INC.
 * AND ITS LICENSORS SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE
 * AS A RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
 * DERIVATIVES. IN NO EVENT WILL ALIPAY OR ITS LICENSORS BE LIABLE FOR ANY LOST
 * REVENUE, PROFIT OR DATA, OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL,
 * INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY
 * OF LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF ALIPAY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 ****************************************************************************
 * You acknowledge that this software is not designed, licensed or intended
 * for use in the design, construction, operation or maintenance of any
 * nuclear facility.
*****************************************************************************/

#ifndef ALIPAY_VENDOR_H
#define ALIPAY_VENDOR_H

#include "alipay_common.h"

EXTERNC retval_t UPAY_storage_read(uint32_t item,
                                   uint8_t *buf_content, uint32_t *len_content);

EXTERNC retval_t UPAY_storage_write(uint32_t item,
                                    const uint8_t *buf_content, uint32_t len_content);

EXTERNC retval_t UPAY_storage_delete(uint32_t items);


EXTERNC retval_t UPAY_storage_exists(uint32_t item);


EXTERNC uint32_t UPAY_get_timestamp(void);

EXTERNC void UPAY_ble_write(PARAM_IN uint8_t *data, PARAM_IN uint16_t len);

EXTERNC retval_t UPAY_get_deviceid(PARAM_OUT uint8_t *buf_did, PARAM_INOUT uint32_t *len_did);

EXTERNC retval_t UPAY_get_sn(PARAM_OUT uint8_t *buf_sn, PARAM_INOUT uint32_t *len_sn);

EXTERNC retval_t ALIPAY_get_imei(PARAM_OUT uint8_t *buf_imei, PARAM_INOUT uint32_t *len_imei);

typedef enum
{
    ALIPAY_WATCH,
    ALIPAY_BAND,
    ALIPAY_CARD,
    ALIPAY_QZWT,
    ALIPAY_QZBD,
    ALIPAY_QZCRD,
} alipay_product_type;

EXTERNC alipay_product_type UPAY_get_productType(void);

EXTERNC uint32_t UPAY_get_protocType(void);

EXTERNC void UPAY_get_companyName(PARAM_OUT uint8_t *buffer, PARAM_INOUT uint32_t *len);

EXTERNC void UPAY_get_productModel(PARAM_OUT uint8_t *buffer, PARAM_INOUT uint32_t *len);

EXTERNC void *ALIPAY_malloc(uint32_t size);

EXTERNC void *ALIPAY_calloc(uint32_t nblock, uint32_t size);

EXTERNC void ALIPAY_free(void *pt);

EXTERNC void *ALIPAY_realloc(void *pt, uint32_t size);

typedef enum
{
    ALIPAY_LEVEL_FATAL = 0x01,
    ALIPAY_LEVEL_ERRO,
    ALIPAY_LEVEL_WARN,
    ALIPAY_LEVEL_INFO,
    ALIPAY_LEVEL_DBG,
} UPAY_log_level;

EXTERNC void UPAY_log(UPAY_log_level, const char *format, int32_t value);
EXTERNC void UPAY_log_ext(const char *format, ...);
#endif






