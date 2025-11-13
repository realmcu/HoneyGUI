

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#include "l3_gltf.h"
#include <stdio.h>
#include <libgen.h>

static int total_triangle_count = 0;

static char *get_img_file(const char *filename)
{
    const char *dot = strchr(filename, '.');
    size_t len = dot - filename;
    char *txtFilename = (char *)malloc(len + 5);;
    strncpy(txtFilename, filename, len);
    strcpy(txtFilename + len, ".txt");
    return txtFilename;
}

// png
static unsigned char *read_array_from_file(const char *filename, uint32_t *length)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *file_content = (char *)malloc(file_size + 1);
    if (file_content == NULL)
    {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';
    fclose(file);

    char *start = strchr(file_content, '{');
    if (start == NULL)
    {
        fprintf(stderr, "Failed to find array start\n");
        free(file_content);
        return NULL;
    }
    start++;

    char *end = strchr(start, '}');
    if (end == NULL)
    {
        fprintf(stderr, "Failed to find array end\n");
        free(file_content);
        return NULL;
    }
    *end = '\0';

    size_t count = 0;
    for (char *p = start; p < end; p++)
    {
        if (*p == ',')
        {
            count++;
        }
    }
    count++;

    unsigned char *array = (unsigned char *)malloc(count);
    if (array == NULL)
    {
        perror("Failed to allocate memory for array");
        free(file_content);
        return NULL;
    }

    char *token = strtok(start, ", \n\t");

    size_t i = 0;
    while (token != NULL && i < count)
    {
        while (*token == ' ' || *token == '\n' || *token == '\t') { token++; }
        if (*token == '\n') { token++; }
        if (strlen(token) == 0)
        {
            token = strtok(NULL, ", \n\t");
            continue;
        }
        unsigned int value;
        if (sscanf(token, "%x", &value) != 1)
        {
            fprintf(stderr, "i: %zu Failed to parse value: %s\n", i, token);
            free(array);
            free(file_content);
            return NULL;
        }
        array[i++] = (unsigned char)value;
        token = strtok(NULL, ", \n\t");
    }

    if (i != count)
    {
        fprintf(stderr, "Mismatch between found and counted values (found %zu, expected %zu)\n", i, count);
        free(array);
        free(file_content);
        return NULL;
    }

    free(file_content);

    if (length != NULL)
    {
        *length = count;
    }

    return array;
}


static int find_node_index(cgltf_node *target_node, cgltf_node *all_nodes, int num_nodes)
{
    if (!target_node) { return -1; }
    return (target_node - all_nodes);
}

static void transpose_matrix(const float *src, float *dst)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            dst[i * 4 + j] = src[j * 4 + i];
        }
    }
}

static void create_gltf_description(cgltf_data *data, l3_gltf_model_description_t *gltf_desc)
{
    gltf_desc->node_count = data->nodes_count;
    gltf_desc->mesh_count = data->meshes_count;
    gltf_desc->material_count = data->materials_count;
    gltf_desc->skin_count = data->skins_count;

    gltf_desc->nodes = malloc(sizeof(l3_gltf_node_t) * gltf_desc->node_count);
    gltf_desc->meshes = malloc(sizeof(l3_gltf_mesh_t) * gltf_desc->mesh_count);
    gltf_desc->materials = malloc(sizeof(l3_gltf_material_t) * gltf_desc->material_count);
    gltf_desc->skins = malloc(sizeof(l3_gltf_skin_t) * gltf_desc->skin_count);

    memset(gltf_desc->nodes, 0, sizeof(l3_gltf_node_t) * gltf_desc->node_count);
    memset(gltf_desc->meshes, 0, sizeof(l3_gltf_mesh_t) * gltf_desc->mesh_count);
    memset(gltf_desc->materials, 0, sizeof(l3_gltf_material_t) * gltf_desc->material_count);
    memset(gltf_desc->skins, 0, sizeof(l3_gltf_skin_t) * gltf_desc->skin_count);

    // ---- 1. Nodes ----
    for (size_t i = 0; i < data->nodes_count; ++i)
    {
        cgltf_node *src_node = &data->nodes[i];
        l3_gltf_node_t *dst_node = &gltf_desc->nodes[i];

        // TRS
        if (src_node->has_translation)
        {
            memcpy(dst_node->translation, src_node->translation, sizeof(float) * 3);
        }
        else
        {
            dst_node->translation[0] = dst_node->translation[1] = dst_node->translation[2] = 0.0f;
        }
        if (src_node->has_rotation)
        {
            memcpy(dst_node->rotation, src_node->rotation, sizeof(float) * 4);
        }
        else
        {
            dst_node->rotation[0] = dst_node->rotation[1] = dst_node->rotation[2] = 0.0f;
            dst_node->rotation[3] = 1.0f;
        }
        if (src_node->has_scale)
        {
            memcpy(dst_node->scale, src_node->scale, sizeof(float) * 3);
        }
        else
        {
            dst_node->scale[0] = dst_node->scale[1] = dst_node->scale[2] = 1.0f;
        }

        dst_node->mesh_index = src_node->mesh ? (int)(src_node->mesh - data->meshes) : -1;

        dst_node->parent_index = find_node_index(src_node->parent, data->nodes, data->nodes_count);

        dst_node->skin_index = src_node->skin ? (int)(src_node->skin - data->skins) : -1;

        // Correlated child nodes
        dst_node->num_children = src_node->children_count;
        if (dst_node->num_children > 0)
        {
            dst_node->children_indices = malloc(sizeof(int) * dst_node->num_children);
            for (size_t k = 0; k < dst_node->num_children; ++k)
            {
                dst_node->children_indices[k] = find_node_index(src_node->children[k], data->nodes,
                                                                data->nodes_count);
            }
        }
        else
        {
            dst_node->children_indices = NULL;
        }
    }

    // ---- 2. Scene Root ----
    if (data->scene)
    {
        gltf_desc->scene_root_count = data->scene->nodes_count;
        gltf_desc->scene_root_indices = malloc(sizeof(int) * gltf_desc->scene_root_count);
        for (size_t i = 0; i < gltf_desc->scene_root_count; ++i)
        {
            gltf_desc->scene_root_indices[i] = find_node_index(data->scene->nodes[i], data->nodes,
                                                               data->nodes_count);
        }
    }

    // ---- 3. Materials ----
    for (size_t i = 0; i < data->materials_count; ++i)
    {
        cgltf_material *src_mat = &data->materials[i];
        l3_gltf_material_t *dst_mat = &gltf_desc->materials[i];

        if (src_mat->has_pbr_metallic_roughness)
        {
            cgltf_texture_view *base_color_tex_view = &src_mat->pbr_metallic_roughness.base_color_texture;

            if (base_color_tex_view != NULL && base_color_tex_view->texture != NULL &&
                base_color_tex_view->texture->image != NULL)
            {
                char *uri_name = base_color_tex_view->texture->image->uri;
                if (uri_name != NULL)
                {
                    printf("Found texture uri: %s\n", uri_name);
                    char *txt_file = get_img_file(uri_name);
                    if (txt_file == NULL)
                    {
                        fprintf(stderr, "Failed to convert filename for material %s\n", uri_name);
                        continue;
                    }
                    uint32_t length;
                    unsigned char *array = read_array_from_file(txt_file, &length);
                    if (array != NULL)
                    {
                        dst_mat->texture_data = malloc(length);
                        memcpy(dst_mat->texture_data, array, length);
                        dst_mat->texture_length = length;
                        free(array);
                    }
                    else
                    {
                        printf("Failed to read array from file %s.\n", txt_file);
                    }
                    free(txt_file);
                }
            }
            dst_mat->base_color[0] = (uint8_t)(src_mat->pbr_metallic_roughness.base_color_factor[0] * 255);
            dst_mat->base_color[1] = (uint8_t)(src_mat->pbr_metallic_roughness.base_color_factor[1] * 255);
            dst_mat->base_color[2] = (uint8_t)(src_mat->pbr_metallic_roughness.base_color_factor[2] * 255);
            dst_mat->base_color[3] = (uint8_t)(src_mat->pbr_metallic_roughness.base_color_factor[3] * 255);
        }
    }

    // ---- 4. Total Primitives ----
    unsigned int total_primitives = 0;
    for (size_t i = 0; i < data->meshes_count; ++i)
    {
        total_primitives += data->meshes[i].primitives_count;
    }
    // Allocate a continuous memory block for all primitives at once
    l3_gltf_primitive_t *all_primitives_pool = malloc(sizeof(l3_gltf_primitive_t) * total_primitives);
    memset(all_primitives_pool, 0, sizeof(l3_gltf_primitive_t) * total_primitives);


    // ---- 5. Meshes & Primitives ----
    unsigned int primitive_pool_offset = 0;
    for (size_t i = 0; i < data->meshes_count; ++i)
    {
        cgltf_mesh *src_mesh = &data->meshes[i];
        l3_gltf_mesh_t *dst_mesh = &gltf_desc->meshes[i];

        dst_mesh->primitive_count = src_mesh->primitives_count;
        dst_mesh->primitives = &all_primitives_pool[primitive_pool_offset];

        for (size_t j = 0; j < src_mesh->primitives_count; ++j)
        {
            cgltf_primitive *src_prim = &src_mesh->primitives[j];
            l3_gltf_primitive_t *dst_prim = &dst_mesh->primitives[j];

            // --- 5.1 Correlated Material ----
            if (src_prim->material)
            {
                int material_index = src_prim->material - data->materials;
                dst_prim->material = &gltf_desc->materials[material_index];
            }
            else
            {
                dst_prim->material = NULL; // or a default material
            }

            // --- 5.2 Create triangular arrays ---
            if (src_prim->type != cgltf_primitive_type_triangles)
            {
                printf("Warning: Primitive %zu of mesh %zu is not of type TRIANGLES. Skipping.\n", j, i);
                dst_prim->triangle_count = 0;
                dst_prim->triangles = NULL;
                continue;
            }

            cgltf_accessor *pos_accessor = NULL;
            cgltf_accessor *norm_accessor = NULL;
            cgltf_accessor *uv_accessor = NULL;
            cgltf_accessor *joints_accessor = NULL;
            cgltf_accessor *weights_accessor = NULL;

            for (size_t k = 0; k < src_prim->attributes_count; ++k)
            {
                cgltf_attribute *attr = &src_prim->attributes[k];
                if (attr->type == cgltf_attribute_type_position)
                {
                    pos_accessor = attr->data;
                }
                else if (attr->type == cgltf_attribute_type_normal)
                {
                    norm_accessor = attr->data;
                }
                else if (attr->type == cgltf_attribute_type_texcoord)
                {
                    uv_accessor = attr->data;
                }
                else if (attr->type == cgltf_attribute_type_joints)
                {
                    joints_accessor = attr->data;
                }
                else if (attr->type == cgltf_attribute_type_weights)
                {
                    weights_accessor = attr->data;
                }
            }

            if (!pos_accessor || !src_prim->indices)
            {
                printf("Warning: Primitive %zu of mesh %zu is missing positions or indices. Skipping.\n", j, i);
                continue;
            }

            // Count triangles
            dst_prim->triangle_count = src_prim->indices->count / 3;
            if (dst_prim->triangle_count == 0)
            {
                dst_prim->triangles = NULL;
                continue;
            }
            dst_prim->triangles = malloc(sizeof(l3_gltf_triangle_t) * dst_prim->triangle_count);
            // Fill triangle data
            for (size_t tri_idx = 0; tri_idx < dst_prim->triangle_count; ++tri_idx)
            {
                l3_gltf_triangle_t *current_triangle = &dst_prim->triangles[tri_idx];

                for (int vert_of_tri = 0; vert_of_tri < 3; ++vert_of_tri)
                {
                    unsigned int original_vertex_index;
                    // Calculate the position in the indices buffer
                    size_t index_buffer_pos = tri_idx * 3 + vert_of_tri;
                    cgltf_accessor_read_uint(src_prim->indices, index_buffer_pos, &original_vertex_index, 1);

                    // Get the vertex data
                    l3_gltf_complete_vertex_t *vert_to_fill = &current_triangle->vertices[vert_of_tri];

                    // Read position
                    cgltf_accessor_read_float(pos_accessor, original_vertex_index,
                                              (cgltf_float *) & (vert_to_fill->pos), 3);

                    // Read normal
                    if (norm_accessor)
                    {
                        cgltf_accessor_read_float(norm_accessor, original_vertex_index,
                                                  (cgltf_float *) & (vert_to_fill->normal), 3);
                    }
                    else
                    {
                        memset(&vert_to_fill->normal, 0, sizeof(float) * 3);
                    }

                    // Read texcoord
                    if (uv_accessor)
                    {
                        cgltf_accessor_read_float(uv_accessor, original_vertex_index,
                                                  (cgltf_float *) & (vert_to_fill->texcoord), 2);
                    }
                    else
                    {
                        memset(&vert_to_fill->texcoord, 0, sizeof(float) * 2);
                    }

                    // Read joints
                    if (joints_accessor)
                    {
                        cgltf_uint temp_joints[4];
                        cgltf_accessor_read_uint(joints_accessor, original_vertex_index, temp_joints, 4);
                        vert_to_fill->joints[0] = (unsigned short)temp_joints[0];
                        vert_to_fill->joints[1] = (unsigned short)temp_joints[1];
                        vert_to_fill->joints[2] = (unsigned short)temp_joints[2];
                        vert_to_fill->joints[3] = (unsigned short)temp_joints[3];
                    }
                    else
                    {
                        memset(vert_to_fill->joints, 0, sizeof(unsigned short) * 4);
                    }

                    // Read weights
                    if (weights_accessor)
                    {
                        cgltf_accessor_read_float(weights_accessor, original_vertex_index, vert_to_fill->weights, 4);
                    }
                    else
                    {
                        memset(vert_to_fill->weights, 0, sizeof(float) * 4);
                    }
                }
            }
        }
        primitive_pool_offset += src_mesh->primitives_count;
    }


    // ---- 6. Skins ----
    for (size_t i = 0; i < gltf_desc->skin_count; ++i)
    {
        cgltf_skin *src_skin = &data->skins[i];
        l3_gltf_skin_t *dst_skin = &gltf_desc->skins[i];

        dst_skin->joint_count = src_skin->joints_count;

        // Allocate and fill in the joint node indices
        dst_skin->joint_indices = malloc(sizeof(uint32_t) * dst_skin->joint_count);
        for (size_t j = 0; j < dst_skin->joint_count; ++j)
        {
            dst_skin->joint_indices[j] = find_node_index(src_skin->joints[j], data->nodes, data->nodes_count);
        }

        // Allocate and fill in the inverse bind matrices
        if (src_skin->inverse_bind_matrices)
        {
            size_t matrix_count = src_skin->inverse_bind_matrices->count;

            dst_skin->inverse_bind_matrices = malloc(sizeof(l3_4x4_matrix_t) * matrix_count);

            // The temporary buffer is used to read raw data
            float *temp_float_buffer = malloc(sizeof(float) * 16 * matrix_count);
            cgltf_accessor_unpack_floats(src_skin->inverse_bind_matrices, temp_float_buffer, 16 * matrix_count);

            // Process each matrix individually: transpose and fill into the target structure
            for (size_t j = 0; j < matrix_count; j++)
            {
                float *original_matrix = temp_float_buffer + j * 16;

                dst_skin->inverse_bind_matrices[j].u.e._11 = original_matrix[0];
                dst_skin->inverse_bind_matrices[j].u.e._12 = original_matrix[4];
                dst_skin->inverse_bind_matrices[j].u.e._13 = original_matrix[8];
                dst_skin->inverse_bind_matrices[j].u.e._14 = original_matrix[12];

                dst_skin->inverse_bind_matrices[j].u.e._21 = original_matrix[1];
                dst_skin->inverse_bind_matrices[j].u.e._22 = original_matrix[5];
                dst_skin->inverse_bind_matrices[j].u.e._23 = original_matrix[9];
                dst_skin->inverse_bind_matrices[j].u.e._24 = original_matrix[13];

                dst_skin->inverse_bind_matrices[j].u.e._31 = original_matrix[2];
                dst_skin->inverse_bind_matrices[j].u.e._32 = original_matrix[6];
                dst_skin->inverse_bind_matrices[j].u.e._33 = original_matrix[10];
                dst_skin->inverse_bind_matrices[j].u.e._34 = original_matrix[14];

                dst_skin->inverse_bind_matrices[j].u.e._41 = original_matrix[3];
                dst_skin->inverse_bind_matrices[j].u.e._42 = original_matrix[7];
                dst_skin->inverse_bind_matrices[j].u.e._43 = original_matrix[11];
                dst_skin->inverse_bind_matrices[j].u.e._44 = original_matrix[15];
            }

            free(temp_float_buffer);
        }
    }

    // ---- 7. Animations, Channels, Samplers ----
    gltf_desc->animation = NULL;
    if (data->animations_count > 0)
    {
        gltf_desc->animation = malloc(sizeof(l3_gltf_single_animation_t));
        cgltf_animation *src_anim = &data->animations[data->animations_count - 1];

        gltf_desc->animation->channel_count = src_anim->channels_count;
        gltf_desc->animation->sampler_count = src_anim->samplers_count;

        gltf_desc->animation->channels = malloc(sizeof(l3_gltf_channel_t) *
                                                gltf_desc->animation->channel_count);
        gltf_desc->animation->samplers = malloc(sizeof(l3_gltf_sampler_t) *
                                                gltf_desc->animation->sampler_count);

        // Traverse all channels of this animation
        for (size_t i = 0; i < gltf_desc->animation->channel_count; ++i)
        {
            cgltf_animation_channel *src_channel = &src_anim->channels[i];
            l3_gltf_channel_t *dst_channel = &gltf_desc->animation->channels[i];

            // Set target nodes and paths
            dst_channel->target_node_index = find_node_index(src_channel->target_node, data->nodes,
                                                             data->nodes_count);
            switch (src_channel->target_path)
            {
            case cgltf_animation_path_type_translation:
                dst_channel->target_path = GLTF_ANIMATION_PATH_TRANSLATION;
                break;
            case cgltf_animation_path_type_rotation:
                dst_channel->target_path = GLTF_ANIMATION_PATH_ROTATION;
                break;
            case cgltf_animation_path_type_scale:
                dst_channel->target_path = GLTF_ANIMATION_PATH_SCALE;
                break;
            default:
                break;
            }
            // Set sampler indices
            dst_channel->sampler_index = (uint32_t)(src_channel->sampler - src_anim->samplers);
        }

        // Traverse all samplers of this animation
        for (size_t i = 0; i < src_anim->samplers_count; ++i)
        {
            cgltf_animation_sampler *src_sampler = &src_anim->samplers[i];
            l3_gltf_sampler_t *dst_sampler = &gltf_desc->animation->samplers[i];

            switch (src_sampler->interpolation)
            {
            case cgltf_interpolation_type_linear:
                dst_sampler->interpolation = GLTF_INTERPOLATION_TYPE_LINEAR;
                break;
            case cgltf_interpolation_type_step:
                dst_sampler->interpolation = GLTF_INTERPOLATION_TYPE_STEP;
                break;
            case cgltf_interpolation_type_cubic_spline:
                dst_sampler->interpolation = GLTF_INTERPOLATION_TYPE_CUBICSPLINE;
                break;
            default:
                break;
            }

            dst_sampler->output_type = (src_sampler->output->type == cgltf_type_vec3) ? GLTF_TYPE_VEC3 :
                                       GLTF_TYPE_VEC4;

            // Extract input (time) data
            dst_sampler->input_count = src_sampler->input->count;
            dst_sampler->input_data = malloc(sizeof(float) * dst_sampler->input_count);
            cgltf_accessor_unpack_floats(src_sampler->input, dst_sampler->input_data, dst_sampler->input_count);

            // Extract output (value) data
            dst_sampler->output_count = src_sampler->output->count;
            size_t output_floats_count = cgltf_accessor_unpack_floats(src_sampler->output, NULL, 0);
            dst_sampler->output_data = malloc(sizeof(float) * output_floats_count);
            cgltf_accessor_unpack_floats(src_sampler->output, dst_sampler->output_data, output_floats_count);
        }
    }
}

void free_gltf_description(l3_gltf_model_description_t *desc)
{
    if (!desc) { return; }

    for (unsigned int i = 0; i < desc->node_count; ++i)
    {
        if (desc->nodes[i].children_indices)
        {
            free(desc->nodes[i].children_indices);
        }
    }

    if (desc->mesh_count > 0 && desc->meshes[0].primitives)
    {
        l3_gltf_primitive_t *all_primitives_pool = desc->meshes[0].primitives;
        unsigned int total_primitives = 0;
        for (unsigned int i = 0; i < desc->mesh_count; ++i)
        {
            total_primitives += desc->meshes[i].primitive_count;
        }

        for (unsigned int i = 0; i < total_primitives; ++i)
        {
            if (all_primitives_pool[i].triangles)
            {
                free(all_primitives_pool[i].triangles);
            }
        }
        free(all_primitives_pool);
    }

    if (desc->nodes) { free(desc->nodes); }
    if (desc->meshes) { free(desc->meshes); }
    if (desc->materials)
    {
        if (desc->materials->texture_data)
        {
            free(desc->materials->texture_data);
        }
        free(desc->materials);
    }
    if (desc->scene_root_indices) { free(desc->scene_root_indices); }

    for (size_t i = 0; i < desc->skin_count; ++i)
    {
        if (desc->skins[i].joint_indices)
        {
            free(desc->skins[i].joint_indices);
        }

        if (desc->skins[i].inverse_bind_matrices)
        {
            free(desc->skins[i].inverse_bind_matrices);
        }
    }
    if (desc->skins) { free(desc->skins); }

    if (desc->animation)
    {
        if (desc->animation->channels) { free(desc->animation->channels); }

        if (desc->animation->samplers)
        {
            for (unsigned int i = 0; i < desc->animation->sampler_count; ++i)
            {
                if (desc->animation->samplers[i].input_data)
                {
                    free(desc->animation->samplers[i].input_data);
                }
                if (desc->animation->samplers[i].output_data)
                {
                    free(desc->animation->samplers[i].output_data);
                }
            }
            free(desc->animation->samplers);
        }
        free(desc->animation);
    }

    memset(desc, 0, sizeof(l3_gltf_model_description_t));
}

static void blob_init(g3m_data_blob_t *blob)
{
    blob->data = NULL;
    blob->size = 0;
    blob->capacity = 0;
}
static void blob_free(g3m_data_blob_t *blob)
{
    if (blob->data)
    {
        free(blob->data);
    }
    blob_init(blob);
}
// Append data to blob and return its offset in blob
static uint32_t append_to_blob(g3m_data_blob_t *blob, const void *data_to_append,
                               size_t size_to_append)
{
    if (size_to_append == 0) { return (uint32_t) - 1; }

    if (blob->size + size_to_append > blob->capacity)
    {
        size_t new_capacity = blob->capacity > 0 ? blob->capacity * 2 : 1024 * 1024; // Initial 1MB
        if (new_capacity < blob->size + size_to_append)
        {
            new_capacity = blob->size + size_to_append;
        }
        uint8_t *new_data = (uint8_t *)realloc(blob->data, new_capacity);
        if (!new_data)
        {
            fprintf(stderr, "Error: Failed to reallocate memory for data blob.\n");
            return (uint32_t) - 1;
        }
        blob->data = new_data;
        blob->capacity = new_capacity;
    }

    uint32_t offset = (uint32_t)blob->size;
    memcpy(blob->data + offset, data_to_append, size_to_append);
    blob->size += size_to_append;
    return offset;
}

static bool save_desc_to_binary_file(const l3_gltf_model_description_t *model, const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        perror("Failed to open file for writing");
        return false;
    }

    g3m_data_blob_t blob;
    blob_init(&blob);

    // --- Phase 1: Prepare all on-disk data structures ---
    int *scene_roots_on_disk = NULL;
    g3m_node_on_disk_t *nodes_on_disk = NULL;
    g3m_mesh_on_disk_t *meshes_on_disk = NULL;
    g3m_primitive_on_disk_t *primitives_on_disk = NULL;
    g3m_skin_on_disk_t *skin_on_disk = NULL;
    g3m_channel_on_disk_t *channels_on_disk = NULL;
    g3m_sampler_on_disk_t *samplers_on_disk = NULL;
    g3m_material_on_disk_t *materials_on_disk = NULL;
    g3m_texture_on_disk_t *textures_on_disk = NULL;

    // 1. Basic statistical information
    g3m_header_t header = {0};
    header.scene_root_count = model->scene_root_count;
    header.node_count = model->node_count;
    header.mesh_count = model->mesh_count;
    header.skin_count = model->skin_count;
    header.channel_count = model->animation->channel_count;
    header.sampler_count = model->animation->sampler_count;
    header.material_count = model->material_count;

    // Primitive count
    header.primitive_count = 0;
    for (uint32_t i = 0; i < model->mesh_count; ++i)
    {
        header.primitive_count += model->meshes[i].primitive_count;
    }

    // Texture count
    header.texture_count = 0;
    for (uint32_t i = 0; i < model->material_count; ++i)
    {
        if (model->materials[i].texture_data != NULL)
        {
            header.texture_count++;
        }
    }

    // 2. Allocate on-disk arrays
    scene_roots_on_disk = (int *)malloc(sizeof(int) * header.scene_root_count);
    nodes_on_disk       = (g3m_node_on_disk_t *)malloc(sizeof(g3m_node_on_disk_t) * header.node_count);
    meshes_on_disk      = (g3m_mesh_on_disk_t *)malloc(sizeof(g3m_mesh_on_disk_t) * header.mesh_count);
    primitives_on_disk  = (g3m_primitive_on_disk_t *)malloc(sizeof(g3m_primitive_on_disk_t) *
                                                            header.primitive_count);
    skin_on_disk        = (g3m_skin_on_disk_t *)malloc(sizeof(g3m_skin_on_disk_t) * header.skin_count);
    channels_on_disk    = (g3m_channel_on_disk_t *)malloc(sizeof(g3m_channel_on_disk_t) *
                                                          header.channel_count);
    samplers_on_disk    = (g3m_sampler_on_disk_t *)malloc(sizeof(g3m_sampler_on_disk_t) *
                                                          header.sampler_count);
    materials_on_disk   = (g3m_material_on_disk_t *)malloc(sizeof(g3m_material_on_disk_t) *
                                                           header.material_count);
    textures_on_disk    = (g3m_texture_on_disk_t *)malloc(sizeof(g3m_texture_on_disk_t) *
                                                          header.texture_count);

    if (!scene_roots_on_disk || !nodes_on_disk || !meshes_on_disk || !primitives_on_disk ||
        !skin_on_disk || !channels_on_disk || !samplers_on_disk || !materials_on_disk || !textures_on_disk)
    {
        fprintf(stderr, "Error: Memory allocation failed for on-disk arrays.\n");
        goto cleanup;
    }

    memcpy(scene_roots_on_disk, model->scene_root_indices, sizeof(int) * model->scene_root_count);

    // 3. Traverse and populate the on-disk array, while simultaneously constructing the blob
    // Nodes
    for (uint32_t i = 0; i < model->node_count; ++i)
    {
        memcpy(nodes_on_disk[i].translation, model->nodes[i].translation, 3 * sizeof(float));
        memcpy(nodes_on_disk[i].rotation, model->nodes[i].rotation, 4 * sizeof(float));
        memcpy(nodes_on_disk[i].scale, model->nodes[i].scale, 3 * sizeof(float));

        nodes_on_disk[i].skin_index = model->nodes[i].skin_index;
        nodes_on_disk[i].mesh_index = model->nodes[i].mesh_index;
        nodes_on_disk[i].parent_index = model->nodes[i].parent_index;
        nodes_on_disk[i].children_count = model->nodes[i].num_children;
        nodes_on_disk[i].children_offset = append_to_blob(&blob, model->nodes[i].children_indices,
                                                          sizeof(int) * model->nodes[i].num_children);
    }

    // Meshes & Primitives
    uint32_t primitive_index = 0;
    uint32_t mesh_primitive_start = 0;
    for (uint32_t i = 0; i < model->mesh_count; ++i)
    {
        // Set the primitive range of the mesh
        meshes_on_disk[i].primitive_start_index = mesh_primitive_start;
        meshes_on_disk[i].primitive_count = model->meshes[i].primitive_count;
        mesh_primitive_start += model->meshes[i].primitive_count;

        for (uint32_t j = 0; j < model->meshes[i].primitive_count; ++j)
        {
            const l3_gltf_primitive_t *src_prim = &model->meshes[i].primitives[j];
            g3m_primitive_on_disk_t *dst_prim = &primitives_on_disk[primitive_index];

            dst_prim->material_index = (int32_t)(src_prim->material - model->materials);
            dst_prim->triangle_count = src_prim->triangle_count;
            total_triangle_count += src_prim->triangle_count;

            size_t triangles_size = src_prim->triangle_count * sizeof(l3_gltf_triangle_t);
            dst_prim->triangles_offset = append_to_blob(&blob, src_prim->triangles, triangles_size);

            primitive_index++;
        }
    }

    // Skins
    for (uint32_t i = 0; i < model->skin_count; ++i)
    {
        l3_gltf_skin_t *src_skin = &model->skins[i];
        g3m_skin_on_disk_t *dst_skin = &skin_on_disk[i];

        dst_skin->joints_count = src_skin->joint_count;

        // 1. Inverse Bind Matrices (IBM)
        if (src_skin->inverse_bind_matrices)
        {
            size_t ibm_data_size = src_skin->joint_count * 16 * sizeof(float);
            dst_skin->inverse_bind_matrices_offset = append_to_blob(&blob, src_skin->inverse_bind_matrices,
                                                                    ibm_data_size);
        }
        else
        {
            dst_skin->inverse_bind_matrices_offset = 0;
        }

        // 2. Joints Index Array
        if (src_skin->joint_count > 0)
        {
            size_t joints_data_size = src_skin->joint_count * sizeof(uint32_t);
            dst_skin->joints_offset = append_to_blob(&blob, src_skin->joint_indices, joints_data_size);
        }
        else
        {
            dst_skin->joints_offset = 0;
        }
    }

    // Channels
    for (uint32_t i = 0; i < model->animation->channel_count; ++i)
    {
        l3_gltf_channel_t *src_channel = &model->animation->channels[i];
        g3m_channel_on_disk_t *dst_channel = &channels_on_disk[i];

        dst_channel->sampler_index = src_channel->sampler_index;
        dst_channel->target_node_index = src_channel->target_node_index;
        dst_channel->target_path = (uint32_t)src_channel->target_path;
    }

    // Samplers
    for (uint32_t i = 0; i < model->animation->sampler_count; ++i)
    {
        l3_gltf_sampler_t *src_sampler = &model->animation->samplers[i];
        g3m_sampler_on_disk_t *dst_sampler = &samplers_on_disk[i];

        // Input time axis
        size_t input_data_size = src_sampler->input_count * sizeof(float);
        dst_sampler->input_offset = append_to_blob(&blob, src_sampler->input_data, input_data_size);
        dst_sampler->input_count = src_sampler->input_count;

        // Output keyframe values
        size_t output_element_size = (src_sampler->output_type == GLTF_TYPE_VEC3) ? sizeof(
                                         float) * 3 : sizeof(float) * 4;
        size_t output_data_size = src_sampler->output_count * output_element_size;
        dst_sampler->output_offset = append_to_blob(&blob, src_sampler->output_data, output_data_size);
        dst_sampler->output_count = src_sampler->output_count;

        dst_sampler->interpolation_type = (uint32_t)src_sampler->interpolation;
        dst_sampler->output_type = (uint32_t)src_sampler->output_type;
    }

    // Materials
    uint32_t texture_index = 0;
    for (uint32_t i = 0; i < model->material_count; ++i)
    {
        memcpy(materials_on_disk[i].base_color, model->materials[i].base_color, sizeof(uint8_t) * 4);

        if (model->materials[i].texture_data != NULL)
        {
            materials_on_disk[i].texture_index = texture_index;
            textures_on_disk[texture_index].data_size = model->materials[i].texture_length;

            textures_on_disk[texture_index].data_offset = append_to_blob(&blob,
                                                                         model->materials[i].texture_data, model->materials[i].texture_length);
            texture_index++;
        }
        else
        {
            materials_on_disk[i].texture_index = -1;
        }
    }

    // --- Phase 2: Calculate the layout and write it to the file ---

    // Calculate the absolute offsets of each data block
    uint32_t current_offset = sizeof(g3m_header_t);

    header.scene_roots_offset = current_offset;
    current_offset += sizeof(int) * header.scene_root_count;

    header.nodes_offset = current_offset;
    current_offset += sizeof(g3m_node_on_disk_t) * header.node_count;

    header.meshes_offset = current_offset;
    current_offset += sizeof(g3m_mesh_on_disk_t) * header.mesh_count;

    header.primitives_offset = current_offset;
    current_offset += sizeof(g3m_primitive_on_disk_t) * header.primitive_count;

    header.skins_offset = current_offset;
    current_offset += sizeof(g3m_skin_on_disk_t) * header.skin_count;

    header.channels_offset = current_offset;
    current_offset += sizeof(g3m_channel_on_disk_t) * header.channel_count;

    header.samplers_offset = current_offset;
    current_offset += sizeof(g3m_sampler_on_disk_t) * header.sampler_count;

    header.materials_offset = current_offset;
    current_offset += sizeof(g3m_material_on_disk_t) * header.material_count;

    header.textures_offset = current_offset;
    current_offset += sizeof(g3m_texture_on_disk_t) * header.texture_count;


    header.data_blob_offset = current_offset;
    header.data_blob_size = (uint32_t)blob.size;


    fwrite(&header, sizeof(g3m_header_t), 1,
           fp);                                            // 1. Header
    fwrite(scene_roots_on_disk, sizeof(int), header.scene_root_count,
           fp);                                           // 2. Scene Root Nodes
    fwrite(nodes_on_disk, sizeof(g3m_node_on_disk_t), header.node_count,
           fp);                                           // 3. Node Data
    fwrite(meshes_on_disk, sizeof(g3m_mesh_on_disk_t), header.mesh_count,
           fp);                                           // 4. Mesh Data
    fwrite(primitives_on_disk, sizeof(g3m_primitive_on_disk_t), header.primitive_count,
           fp);                                           // 5. Primitive Data
    fwrite(skin_on_disk, sizeof(g3m_skin_on_disk_t), header.skin_count,
           fp);                                           // 6. Skin Data
    fwrite(channels_on_disk, sizeof(g3m_channel_on_disk_t), header.channel_count,
           fp);                                           // 7. Animation Channel Data
    fwrite(samplers_on_disk, sizeof(g3m_sampler_on_disk_t), header.sampler_count,
           fp);                                           // 8. Animation Sampler Data
    fwrite(materials_on_disk, sizeof(g3m_material_on_disk_t), header.material_count,
           fp);                                           // 9. Material Data
    fwrite(textures_on_disk, sizeof(g3m_texture_on_disk_t), header.texture_count,
           fp);                                           // 10. Texture Data
    fwrite(blob.data, 1, blob.size,
           fp);                                           // 11. Data Blob

    printf("Successfully saved model to %s\n", filename);
    printf("----------------------------------\n");
    printf("Scene Root Count:    %u \n", header.scene_root_count);
    printf("Node Count:          %u \n", header.node_count);
    printf("Mesh Count:          %u \n", header.mesh_count);
    printf("Primitive Count:     %u \n", header.primitive_count);
    printf("Skin Count:          %u \n", header.skin_count);
    printf("Channel Count:       %u \n", header.channel_count);
    printf("Sampler Count:       %u \n", header.sampler_count);
    printf("Material Count:      %u \n", header.material_count);
    printf("Texture Count:       %u \n", header.texture_count);
    printf("Triangle Count:      %d \n", total_triangle_count);

    printf("Header Size:         %zu bytes\n", sizeof(g3m_header_t));
    printf("Nodes Array Size:    %zu bytes\n", sizeof(g3m_node_on_disk_t) * header.node_count);
    printf("Meshes Array Size:   %zu bytes\n", sizeof(g3m_mesh_on_disk_t) * header.mesh_count);
    printf("Primitives Array Size:%zu bytes\n",
           sizeof(g3m_primitive_on_disk_t) * header.primitive_count);
    printf("Skins Array Size:    %zu bytes\n", sizeof(g3m_skin_on_disk_t) * header.skin_count);
    printf("Channels Array Size: %zu bytes\n", sizeof(g3m_channel_on_disk_t) * header.channel_count);
    printf("Samplers Array Size: %zu bytes\n", sizeof(g3m_sampler_on_disk_t) * header.sampler_count);
    printf("Materials Array Size:%zu bytes\n", sizeof(g3m_material_on_disk_t) * header.material_count);
    printf("Textures Array Size:%zu bytes\n", sizeof(g3m_texture_on_disk_t) * header.texture_count);
    printf("Data Blob Size:      %zu bytes\n", blob.size);
    printf("Total File Size:     %ld bytes\n", ftell(fp));
    printf("----------------------------------\n");

    fclose(fp);
    blob_free(&blob);

    return true;

cleanup:
    if (fp) { fclose(fp); }
    blob_free(&blob);
    if (scene_roots_on_disk) { free(scene_roots_on_disk); }
    if (nodes_on_disk) { free(nodes_on_disk); }
    if (meshes_on_disk) { free(meshes_on_disk); }
    if (primitives_on_disk) { free(primitives_on_disk); }
    if (skin_on_disk) { free(skin_on_disk); }
    if (channels_on_disk) { free(channels_on_disk); }
    if (samplers_on_disk) { free(samplers_on_disk); }
    if (materials_on_disk) { free(materials_on_disk); }
    if (textures_on_disk) { free(textures_on_disk); }
    return false;
}

void binary_to_txt_array(const char *binary_filename, const char *txt_filename)
{

    FILE *binary_file = fopen(binary_filename, "rb");
    if (!binary_file)
    {
        perror("Failed to open binary file for reading");
        return;
    }

    FILE *txt_file = fopen(txt_filename, "w");
    if (!txt_file)
    {
        perror("Failed to open txt file for writing");
        fclose(binary_file);
        return;
    }

    fseek(binary_file, 0, SEEK_END);
    long file_size = ftell(binary_file);
    fseek(binary_file, 0, SEEK_SET);

    unsigned char *buffer = (unsigned char *)malloc(file_size);
    if (!buffer)
    {
        perror("Failed to allocate memory");
        fclose(binary_file);
        fclose(txt_file);
        return;
    }

    fread(buffer, 1, file_size, binary_file);
    fclose(binary_file);

    char *file_copy = strdup(binary_filename);
    char *base_name = basename(file_copy);
    char *dot_position = strrchr(base_name, '.');
    if (dot_position != NULL)
    {
        *dot_position = '\0';
    }
    size_t array_name_size = strlen(base_name) + 4;
    char *array_name = (char *)malloc(array_name_size);
    snprintf(array_name, array_name_size, "_ac%s", base_name);

    fprintf(txt_file, "__attribute__((aligned(4))) static const unsigned char %s[%ld] = {", array_name,
            file_size);
    for (long i = 0; i < file_size; i++)
    {
        if (i % 40 == 0)
        {
            fprintf(txt_file, "\n    ");
        }
        if (i == file_size - 1)
        {
            fprintf(txt_file, "0x%02x", buffer[i]);
        }
        else
        {
            fprintf(txt_file, "0x%02x, ", buffer[i]);
        }
    }
    fprintf(txt_file, "\n};\n");

    free(buffer);
    fclose(txt_file);

    printf("Convert binary file to txt array file successfully!\n");
}




int main(int argc, char **argv)
{
    const char *gltf_path = argv[1];
    l3_gltf_model_description_t *gltf_desc = malloc(sizeof(l3_gltf_model_description_t));
    memset(gltf_desc, 0, sizeof(l3_gltf_model_description_t));

    cgltf_options options;
    memset(&options, 0, sizeof(cgltf_options));
    cgltf_data *data = NULL;
    // 1. Analyze the structure of gltf file
    cgltf_result result = cgltf_parse_file(&options, gltf_path, &data);
    if (result != cgltf_result_success)
    {
        printf("Error: Failed to parse file: %s\n", gltf_path);
    }
    // 2. Load the related buffer data (from .bin file or base64)
    result = cgltf_load_buffers(&options, data, gltf_path);
    if (result != cgltf_result_success)
    {
        printf("Error: Failed to load buffers for: %s\n", gltf_path);
    }

    // 3. Construct l3_gltf_model_description_t
    create_gltf_description(data, gltf_desc);
    printf("Parsing and data conversion complete. Now writing to binary file...\n");

    // 4. Save the descriptive information to a binary file
    save_desc_to_binary_file(gltf_desc, "gltf_desc.bin");

    // 5. Convert the binary file to a txt array file
    binary_to_txt_array("gltf_desc.bin", "gltf_desc.txt");

    cgltf_free(data);
    free_gltf_description(gltf_desc);
    free(gltf_desc);
    return 0;

}