#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

typedef struct
{
    tinyobj_attrib_t attrib;

    unsigned int num_shapes;
    tinyobj_shape_t *shapes;

    unsigned int num_materials;
    tinyobj_material_t *materials;

    unsigned int *texture_sizes;
    unsigned char **textures;

} gui_description_t;


void loadFile(void *ctx, const char *filename, const int is_mtl, const char *obj_filename,
              char **buffer, size_t *len)
{
    long string_size = 0, read_size = 0;
    FILE *handler = fopen(filename, "r");

    if (handler)
    {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);
        *buffer = (char *)malloc(sizeof(char) * (string_size + 1));
        read_size = fread(*buffer, sizeof(char), (size_t) string_size, handler);
        (*buffer)[string_size] = '\0';
        // if (string_size != read_size)
        // {
        //     free(*buffer);
        //     *buffer = NULL;
        // }
        fclose(handler);
    }

    *len = read_size;
}

char *get_img_file(const char *filename)
{
    const char *dot = strchr(filename, '.');
    size_t len = dot - filename;
    char *txtFilename = (char *)malloc(len + 5);;
    strncpy(txtFilename, filename, len);
    strcpy(txtFilename + len, ".txt");
    return txtFilename;
}


// png
unsigned char *read_array_from_file(const char *filename, uint32_t *length)
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

void write_string(FILE *file, const char *string)
{
    size_t length = string ? strlen(string) : 0;
    length = string ? length + 1 : 0;
    fwrite(&length, sizeof(size_t), 1, file);
    if (length > 0)
    {
        fwrite(string, sizeof(char), length, file);
    }
}

void save_desc_to_binary_file(gui_description_t *desc, const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Failed to open file for writing");
        return;
    }
    // Save face type
    bool allRectangle = true;
    bool allTriangle = true;
    int defalutValue = 0;
    for (int i = 0; i < desc->attrib.num_face_num_verts; i++)
    {
        if (desc->attrib.face_num_verts[i] != 4)
        {
            allRectangle = false;
        }
        if (desc->attrib.face_num_verts[i] != 3)
        {
            allTriangle = false;
        }
    }
    if (allRectangle)
    {
        defalutValue = 0;
    }
    else if (allTriangle)
    {
        defalutValue = 1;
    }
    else
    {
        defalutValue = 2;
    }
    fwrite(&defalutValue, sizeof(int), 1, file);

    // Save attribute
    fwrite(&desc->attrib.num_vertices, sizeof(unsigned int), 1, file);
    fwrite(&desc->attrib.num_normals, sizeof(unsigned int), 1, file);
    fwrite(&desc->attrib.num_texcoords, sizeof(unsigned int), 1, file);
    fwrite(&desc->attrib.num_faces, sizeof(unsigned int), 1, file);
    fwrite(&desc->attrib.num_face_num_verts, sizeof(unsigned int), 1, file);
    fwrite(&desc->attrib.pad0, sizeof(int), 1, file);

    fwrite(desc->attrib.vertices, sizeof(float), desc->attrib.num_vertices * 3, file);
    fwrite(desc->attrib.normals, sizeof(float), desc->attrib.num_normals * 3, file);
    fwrite(desc->attrib.texcoords, sizeof(float), desc->attrib.num_texcoords * 2, file);
    fwrite(desc->attrib.faces, sizeof(tinyobj_vertex_index_t), desc->attrib.num_faces, file);
    fwrite(desc->attrib.face_num_verts, sizeof(int), desc->attrib.num_face_num_verts, file);
    fwrite(desc->attrib.material_ids, sizeof(int), desc->attrib.num_face_num_verts, file);

    // Save shapes information
    fwrite(&desc->num_shapes, sizeof(unsigned int), 1, file);
    for (int i = 0; i < desc->num_shapes; i++)
    {
        printf("desc->shapes[%d].name: %s\n", i, desc->shapes[i].name);
        // write_string(file, desc->shapes[i].name);
        fwrite(&desc->shapes[i].face_offset, sizeof(unsigned int), 1, file);
        fwrite(&desc->shapes[i].length, sizeof(unsigned int), 1, file);
    }

    // Save materials information
    fwrite(&desc->num_materials, sizeof(unsigned int), 1, file);
    for (int i = 0; i < desc->num_materials; i++)
    {
        const tinyobj_material_t *mat = &desc->materials[i];
        // write_string(file, mat->name);
        fwrite(mat->ambient, sizeof(float), 3, file);
        fwrite(mat->diffuse, sizeof(float), 3, file);
        fwrite(mat->specular, sizeof(float), 3, file);
        fwrite(mat->transmittance, sizeof(float), 3, file);
        fwrite(mat->emission, sizeof(float), 3, file);
        fwrite(&mat->shininess, sizeof(float), 1, file);
        fwrite(&mat->ior, sizeof(float), 1, file);
        fwrite(&mat->dissolve, sizeof(float), 1, file);
        fwrite(&mat->illum, sizeof(int), 1, file);
        // write_string(file, mat->ambient_texname);
        // write_string(file, mat->diffuse_texname);
        // write_string(file, mat->specular_texname);
        // write_string(file, mat->specular_highlight_texname);
        // write_string(file, mat->bump_texname);
        // write_string(file, mat->displacement_texname);
        // write_string(file, mat->alpha_texname);
    }

    // Save textures
    for (int i = 0; i < desc->num_materials; i++)
    {
        unsigned int texture_size = desc->texture_sizes[i];
        fwrite(&texture_size, sizeof(unsigned int), 1, file);
    }
    for (int i = 0; i < desc->num_materials; i++)
    {
        fwrite(desc->textures[i], sizeof(unsigned char), desc->texture_sizes[i], file);
    }


    fclose(file);
    printf("Write description to bin file successfully!\n");
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
    size_t length = dot_position ? (size_t)(dot_position - base_name) : strlen(base_name);

    char *array_name = (char *)malloc(length + 4);
    strcpy(array_name, "_ac");
    strncat(array_name, base_name, length);
    array_name[length + 3] = '\0';

    fprintf(txt_file, "static const unsigned char %s[%ld] = {", array_name, file_size);
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
    int result = system("python png2c.py");
    if (result == -1)
    {
        fprintf(stderr, "Failed to run png2c.py\n");
    }

    const char *obj_filename = argv[1];
    gui_description_t *desc = malloc(sizeof(gui_description_t));

    tinyobj_parse_obj(&desc->attrib, &desc->shapes, &desc->num_shapes, &desc->materials,
                      &desc->num_materials, obj_filename, loadFile, NULL, 0);
    desc->textures = (unsigned char **)malloc(desc->num_materials * sizeof(unsigned char *));
    desc->texture_sizes = (unsigned int *)malloc(desc->num_materials * sizeof(unsigned int));

    for (uint32_t i = 0; i < desc->attrib.num_face_num_verts; i++)
    {
        int material_id = desc->attrib.material_ids[i];
        if (material_id != -1)
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
            }
            else
            {
                printf("Failed to read array from file.\n");
            }

            free(txt_file);
        }
    }


    save_desc_to_binary_file(desc, "desc.bin");
    binary_to_txt_array("desc.bin", "desc.txt");

    return 0;
}