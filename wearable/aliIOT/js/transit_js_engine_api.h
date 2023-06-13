#ifndef TRANSIT_JS_ENGINE_API_H
#define TRANSIT_JS_ENGINE_API_H

#include "alipay_common.h"

EXTERNC bool js_engine_init(void);
EXTERNC void js_engine_destroy(void);
EXTERNC bool js_engine_eval_script(char *eval_pack, uint32_t len_eval_pack);
EXTERNC bool js_engine_hasPrepared(void);

#endif