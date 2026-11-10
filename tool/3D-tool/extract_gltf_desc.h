/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __EXTRACT_GLTF_DESC_H__
#define __EXTRACT_GLTF_DESC_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                            Header Files
 *============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../../realgui/3rd/cgltf/cgltf.h"
#include "../../realgui/3rd/Lite3D/widget/l3_gltf.h"
/*============================================================================*
 *                               Types
 *============================================================================*/


/*============================================================================*
 *                            Functions
 *============================================================================*/

void create_gltf_description(cgltf_data *data, l3_gltf_model_description_t *gltf_desc);

void free_gltf_description(l3_gltf_model_description_t *desc);

bool save_gltf_desc_to_binary_file(const l3_gltf_model_description_t *model, const char *filename);


#ifdef __cplusplus
}
#endif

#endif
