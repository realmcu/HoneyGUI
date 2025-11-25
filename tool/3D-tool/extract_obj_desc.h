/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __EXTRACT_OBJ_DESC_H__
#define __EXTRACT_OBJ_DESC_H__
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

#include "../../realgui/3rd/tinyobj/tinyobj_loader_c.h"


/*============================================================================*
 *                               Types
 *============================================================================*/
typedef struct
{
    tinyobj_attrib_t attrib;

    unsigned int num_shapes;
    tinyobj_shape_t *shapes;

    unsigned int num_materials;
    tinyobj_material_t *materials;

    unsigned int *texture_sizes;
    unsigned char **textures;

} tinyobj_description_t;

/*============================================================================*
 *                            Functions
 *============================================================================*/

void loadFile(void *ctx, const char *filename, const int is_mtl,
              const char *obj_filename, char **buffer, size_t *len);

char *get_img_file(const char *filename);

unsigned char *read_array_from_file(const char *filename, uint32_t *length);

void save_obj_desc_to_binary_file(tinyobj_description_t *desc, const char *filename);

void binary_to_txt_array(const char *binary_filename, const char *txt_filename);

#ifdef __cplusplus
}
#endif

#endif
