#include <string.h>
#include <libgen.h>
#include <unistd.h>

#include "extract_obj_desc.h"
#include "../../realgui/3rd/Lite3D/widget/l3_common.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION


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

void save_obj_desc_to_binary_file(tinyobj_description_t *desc, const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Failed to open file for writing");
        return;
    }

    // Save file header
    l3_desc_file_head_t file_header;
    memset(&file_header, 0, sizeof(l3_desc_file_head_t));
    file_header.magic = 0x3344;
    file_header.model_type = 0;
    extern uint8_t tool_version;
    file_header.version = tool_version;
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
    file_header.face_type = defalutValue;
    file_header.payload_offset = sizeof(l3_desc_file_head_t);
    fwrite(&file_header, sizeof(l3_desc_file_head_t), 1, file);

    long payload_start_pos = ftell(file);
    if (payload_start_pos != file_header.payload_offset)
    {
        printf("Warning: Header size mismatch!\n");
    }

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
        if (desc->texture_sizes[i] > 0)
        {
            fwrite(desc->textures[i], sizeof(unsigned char), desc->texture_sizes[i], file);
        }
    }

    long total_file_size = ftell(file);
    if (total_file_size == -1L)
    {
        perror("Failed to get file size with ftell");
        fclose(file);
    }
    printf("Final calculated file size: %ld bytes\n", total_file_size);
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        perror("Failed to seek back to the beginning of the file");
        fclose(file);
    }
    file_header.file_size = (uint32_t)total_file_size; // update file_size
    fwrite(&file_header, sizeof(l3_desc_file_head_t), 1, file);

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
