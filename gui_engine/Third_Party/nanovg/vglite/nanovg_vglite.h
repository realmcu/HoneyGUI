#ifndef NANOVG_VGLITE_H
#define NANOVG_VGLITE_H

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "nanovg.h"

#ifndef M_PI
#define M_PI 3.1415926f
#endif /*M_PI*/

#ifdef __cplusplus
extern "C" {
#endif

NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                          uint8_t *data);

void nvgDeleteAGGE(NVGcontext *ctx);

#ifdef __cplusplus
}
#endif

#endif /* NANOVG_VGLITE_H */
