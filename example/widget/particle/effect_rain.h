/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EFFECT_RAIN_H
#define EFFECT_RAIN_H

#include "gui_particle_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

void effect_rain_config(particle_effect_config_t *config);
gui_particle_widget_t *effect_rain_demo_init(void);

#ifdef __cplusplus
}
#endif

#endif /* EFFECT_RAIN_H */
