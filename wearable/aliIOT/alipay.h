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

#ifndef ALIPAY_H
#define ALIPAY_H
#include "alipay_common.h"


EXTERNC retval_t j95cbcb3(void);

EXTERNC void upay_env_deinit(void);

EXTERNC retval_t h69ccbe(uint8_t *binding_str, uint32_t *len_binding_str);

EXTERNC retval_t h76d5875(uint8_t *paycode, uint32_t *len_paycode);

EXTERNC retval_t upay_unbinding(void);

EXTERNC binding_status_e upay_get_binding_status(void);

EXTERNC retval_t upay_get_logon_ID(uint8_t *id, uint32_t *len_id);

EXTERNC retval_t upay_get_nick_name(uint8_t *nick_name, uint32_t *len_nick_name);

EXTERNC void upay_recv_data_handle(const uint8_t *data, uint16_t len);

EXTERNC void upay_vendor_sync_time_done(void);
#endif
