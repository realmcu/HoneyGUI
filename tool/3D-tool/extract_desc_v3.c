/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

#include "extract_obj_desc.h"
#include "extract_gltf_desc.h"
#include "../../realgui/3rd/Lite3D/widget/l3_common.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "../../realgui/3rd/tinyobj/tinyobj_loader_c.h"
#define CGLTF_IMPLEMENTATION
#include "../../realgui/3rd/cgltf/cgltf.h"


uint8_t tool_version = 3;

static L3_MODEL_TYPE get_model_type(const char *filename)
{
    const char *dot = strrchr(filename, '.');

    if (strcmp(dot + 1, "obj") == 0)
    {
        return L3_MODEL_TYPE_OBJ;
    }
    else if (strcmp(dot + 1, "gltf") == 0)
    {
        return L3_MODEL_TYPE_GLTF;
    }

    return L3_MODEL_TYPE_UNKNOWN;
}

static void extract_obj_desc(const char *obj_filename, char *bin_filename, char *txt_filename)
{
    tinyobj_description_t *desc = malloc(sizeof(tinyobj_description_t));
    memset(desc, 0, sizeof(tinyobj_description_t));

    tinyobj_parse_obj(&desc->attrib, &desc->shapes, &desc->num_shapes, &desc->materials,
                      &desc->num_materials, obj_filename, loadFile, NULL, 0);
    desc->textures = (unsigned char **)malloc(desc->num_materials * sizeof(unsigned char *));
    desc->texture_sizes = (unsigned int *)malloc(desc->num_materials * sizeof(unsigned int));
    memset(desc->texture_sizes, 0x00, desc->num_materials * sizeof(unsigned int));

    bool *material_processed = calloc(desc->num_materials, sizeof(bool));

    for (uint32_t i = 0; i < desc->attrib.num_face_num_verts; i++)
    {
        int material_id = desc->attrib.material_ids[i];
        if (material_id != -1  && desc->materials[material_id].diffuse_texname != NULL &&
            !material_processed[material_id])
        {
            printf("desc->materials[%d].diffuse_texname: %s\n", material_id,
                   desc->materials[material_id].diffuse_texname);

            char *txt_file = get_img_file(desc->materials[material_id].diffuse_texname);
            if (txt_file == NULL)
            {
                fprintf(stderr, "Failed to convert filename for material %d\n", material_id);
                continue;
            }

            uint32_t length;
            unsigned char *array = read_array_from_file(txt_file, &length);
            if (array != NULL)
            {
                desc->textures[material_id] = malloc(length);
                desc->texture_sizes[material_id] = length;
                memcpy(desc->textures[material_id], array, length);
                free(array);
                material_processed[material_id] = true;
            }
            else
            {
                printf("Failed to read array from file.\n");
            }

            free(txt_file);
        }
    }

    free(material_processed);

    save_obj_desc_to_binary_file(desc, bin_filename);
    binary_to_txt_array(bin_filename, txt_filename);
}

static void extract_gltf_desc(const char *gltf_filename, char *bin_filename, char *txt_filename)
{
    l3_gltf_model_description_t *gltf_desc = malloc(sizeof(l3_gltf_model_description_t));
    memset(gltf_desc, 0, sizeof(l3_gltf_model_description_t));

    cgltf_options options;
    memset(&options, 0, sizeof(cgltf_options));
    cgltf_data *data = NULL;
    // 1. Analyze the structure of gltf file
    cgltf_result result = cgltf_parse_file(&options, gltf_filename, &data);
    if (result != cgltf_result_success)
    {
        printf("Error: Failed to parse file: %s\n", gltf_filename);
    }
    // 2. Load the related buffer data (from .bin file or base64)
    result = cgltf_load_buffers(&options, data, gltf_filename);
    if (result != cgltf_result_success)
    {
        printf("Error: Failed to load buffers for: %s\n", gltf_filename);
    }

    // 3. Construct l3_gltf_model_description_t
    create_gltf_description(data, gltf_desc);
    printf("Parsing and data conversion complete. Now writing to binary file...\n");

    // 4. Save the descriptive information to a binary file
    save_gltf_desc_to_binary_file(gltf_desc, bin_filename);

    // 5. Convert the binary file to a txt array file
    binary_to_txt_array(bin_filename, txt_filename);

    cgltf_free(data);
    free_gltf_description(gltf_desc);
    free(gltf_desc);
}

static void convert_textures_interactive(void)
{
    printf("Do you want to convert PNG textures to bin format? [Y/n]: ");
    char choice[10];
    if (fgets(choice, sizeof(choice), stdin) != NULL)
    {
        // Default to 'y' if just Enter is pressed
        if (choice[0] == '\n' || choice[0] == 'y' || choice[0] == 'Y')
        {
            printf("\nConverting textures...\n");

            // Call the Python conversion script directly (simpler and cross-platform)
            char cmd[512];
#if defined(_WIN32)
            snprintf(cmd, sizeof(cmd), "python convert_textures.py");
#else
            snprintf(cmd, sizeof(cmd), "python3 convert_textures.py");
#endif

            int result = system(cmd);

            if (result == -1 || result != 0)
            {
                fprintf(stderr, "Warning: Texture conversion may have failed\n");
                fprintf(stderr, "Please check convert_textures.py is in the current directory\n");
            }
        }
        else
        {
            printf("Skipping texture conversion...\n");
        }
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    printf("========================================\n");
    printf("HoneyGUI 3D Model Descriptor Generator\n");
    printf("========================================\n\n");

    // Check for PNG files and ask user
    convert_textures_interactive();

    const char *filename = argv[1];
    L3_MODEL_TYPE model_type = get_model_type(filename);
    if (model_type == L3_MODEL_TYPE_UNKNOWN)
    {
        fprintf(stderr, "Unknown model type for file: %s\n", filename);
        return -1;
    }

    char prefix[100];
    strncpy(prefix, filename, sizeof(prefix) - 1);
    prefix[sizeof(prefix) - 1] = '\0';
    // Find last point
    char *dot = strrchr(prefix, '.');
    if (dot != NULL)
    {
        *dot = '\0';
    }
    char bin_filename[100];
    char txt_filename[100];

    if (model_type == L3_MODEL_TYPE_OBJ)
    {
        snprintf(bin_filename, sizeof(bin_filename), "desc_%s.bin", prefix);
        snprintf(txt_filename, sizeof(txt_filename), "desc_%s.txt", prefix);
        extract_obj_desc(filename, bin_filename, txt_filename);
    }
    else if (model_type == L3_MODEL_TYPE_GLTF)
    {
        snprintf(bin_filename, sizeof(bin_filename), "gltf_desc_%s.bin", prefix);
        snprintf(txt_filename, sizeof(txt_filename), "gltf_desc_%s.txt", prefix);
        extract_gltf_desc(filename, bin_filename, txt_filename);
    }

    return 0;
}