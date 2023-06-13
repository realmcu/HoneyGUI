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

#ifndef ALIPAY_COMMON_H
#define ALIPAY_COMMON_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define PARAM_IN
#define PARAM_OUT
#define PARAM_INOUT

#define ALIPAY_ITEM_PRIVATE_KEY     (0x01 << 0)
#define ALIPAY_ITEM_SHARE_KEY       (0x01 << 1)
#define ALIPAY_ITEM_SEED            (0x01 << 2)
#define ALIPAY_ITEM_TIMEDIFF        (0x01 << 3)
#define ALIPAY_ITEM_NICKNAME        (0x01 << 4)
#define ALIPAY_ITEM_LOGONID         (0x01 << 5)
#define ALIPAY_ITEM_BINDFLAG        (0x01 << 6)
#define ALIPAY_ITEM_TID             65
#define ALIPAY_ITEM_UID             66
#define ALIPAY_ITEM_DEGRADE_STATUS  67
#define ALIPAY_ITEM_TRANSPORT_ACTIVITY_STATUS   68
#define MAX_KV_NUM  11

typedef enum
{
    RV_OK = 0,
    RV_WRITE_ERROR,
    RV_READ_ERROR,
    RV_DEL_ERROR,
    RV_NOT_FOUND,
    RV_IO_ERROR,
    RV_NOT_INITIALISED,
    RV_NETWORK_ERROR,
    RV_ECC_GENERATE_ERROR,
    RV_ECC_SHARE_ERROR,
    RV_BUF_TOO_SHORT,
    RV_ENCRYPTION_ERRO,
    RV_DECRYPTION_ERRO,
    RV_WRONG_TIMESTAMP,
    RV_WRONG_PARAM,
    RV_PRODUCT_MODEL_ERROR,
    RV_BINDING_REPEAT,
    RV_SEMAPHORE_CREATE_ERROR,
    RV_DATA_WAITING_TIMEOUT,
    RV_BINDING_DATA_FORMAT_ERROR,
    RV_SIGN_ERROR,
    RV_VERIFY_ERROR,
    RV_JS_ERROR,
    RV_GEN_BUSCODE_ERROR,
    RV_BUSCARDDATA_INVALID,
    RV_LIMITED_BY_SECURITY,
    RV_UNSUPPORTED_CARD,
    RV_UNKNOWN
} retval_t;

typedef enum
{
    STATUS_BINDING_FAIL = 0x0,
    STATUS_UNBINDED,
    STATUS_START_BINDING,
    STATUS_GETTING_PROFILE,
    STATUS_SAVING_DATA,
    STATUS_SAVING_DATA_OK,
    STATUS_FINISH_BINDING,
    STATUS_FINISH_BINDING_OK,
    STATUS_BINDING_OK = 0xA5,
    STATUS_UNKNOWN = 0xFF
} binding_status_e;

#if 0
typedef unsigned short     uint16_t;
typedef unsigned char      uint8_t;
typedef long long          int64_t;
typedef unsigned long long uint64_t;
typedef int                int32_t;
typedef unsigned int       uint32_t;
#endif

#if !defined(__cplusplus)
#ifndef bool
typedef unsigned char bool;
#endif
#ifndef false
#define false ((bool)0)
#define true  ((bool)1)
#endif
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#define STATIC static

#endif /* common_h */

