/*
 * ============================================================================
 * OpenGL + GLFW Demo - 自定义矩阵实现
 * ============================================================================
 *
 * 本示例展示：
 *   1. 手动构建透视投影矩阵和视图矩阵
 *   2. 通过 Uniform 传递 MVP 矩阵到着色器
 *   3. 完全摆脱固定管线函数 (gluPerspective/gluLookAt)
 *   4. 动态光照效果 (光源旋转动画)
 *
 * 矩阵变换流程:
 *   模型空间 ──Model──→ 世界空间 ──View──→ 相机空间 ──Projection──→ 裁剪空间
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

/* GL_GLEXT_PROTOTYPES: 启用 OpenGL 扩展函数的原型声明
 * 比如 glCreateShader, glCompileShader 等着色器相关函数 */
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>  /* GLFW: 窗口和 OpenGL 上下文管理 */
#include "fb_sdl.h"       /* 自定义: 把渲染结果显示到 LCD */

/* 模型数据的最大容量 */
#define MAX_VERTICES 1000  /* 最多 1000 个顶点 */
#define MAX_FACES 2000     /* 最多 2000 个三角面 */

/* ============================================================================
 * 矩阵工具函数
 * ============================================================================ */

/* 4x4 矩阵类型 (列主序，OpenGL 标准) */
typedef float mat4[16];

/* 矩阵乘法: result = a × b */
static void mat4_multiply(mat4 result, const mat4 a, const mat4 b)
{
    mat4 tmp;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            tmp[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++)
            {
                tmp[i * 4 + j] += a[k * 4 + j] * b[i * 4 + k];
            }
        }
    }
    memcpy(result, tmp, sizeof(mat4));
}

/* 单位矩阵 */
static void mat4_identity(mat4 m)
{
    memset(m, 0, sizeof(mat4));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

/* 透视投影矩阵 - 替代 gluPerspective */
static void mat4_perspective(mat4 m, float fov_y, float aspect, float near, float far)
{
    float f = 1.0f / tanf(fov_y / 2.0f);

    memset(m, 0, sizeof(mat4));
    m[0]  = f / aspect;
    m[5]  = f;
    m[10] = -(far + near) / (far - near);
    m[11] = -1.0f;
    m[14] = -(2.0f * far * near) / (far - near);
}

/* 视图矩阵 - 替代 gluLookAt */
static void mat4_look_at(mat4 m, float eye[3], float center[3], float up[3])
{
    /* 计算相机坐标系的三个轴 */
    float f[3] =
    {
        center[0] - eye[0],
        center[1] - eye[1],
        center[2] - eye[2]
    };

    /* 归一化 forward */
    float len = sqrtf(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
    f[0] /= len; f[1] /= len; f[2] /= len;

    /* right = forward × up */
    float r[3] =
    {
        f[1] *up[2] - f[2] *up[1],
        f[2] *up[0] - f[0] *up[2],
        f[0] *up[1] - f[1] *up[0]
    };
    len = sqrtf(r[0] * r[0] + r[1] * r[1] + r[2] * r[2]);
    r[0] /= len; r[1] /= len; r[2] /= len;

    /* up' = right × forward */
    float u[3] =
    {
        r[1] *f[2] - r[2] *f[1],
        r[2] *f[0] - r[0] *f[2],
        r[0] *f[1] - r[1] *f[0]
    };

    /* 构建视图矩阵 (旋转 + 平移) */
    mat4_identity(m);
    m[0] = r[0];  m[4] = r[1];  m[8]  = r[2];
    m[1] = u[0];  m[5] = u[1];  m[9]  = u[2];
    m[2] = -f[0]; m[6] = -f[1]; m[10] = -f[2];
    m[12] = -(r[0] * eye[0] + r[1] * eye[1] + r[2] * eye[2]);
    m[13] = -(u[0] * eye[0] + u[1] * eye[1] + u[2] * eye[2]);
    m[14] = f[0] * eye[0] + f[1] * eye[1] + f[2] * eye[2];
}

/* 旋转矩阵 (绕任意轴) */
static void mat4_rotate(mat4 m, float angle, float x, float y, float z)
{
    float len = sqrtf(x * x + y * y + z * z);
    x /= len; y /= len; z /= len;

    float c = cosf(angle);
    float s = sinf(angle);
    float t = 1.0f - c;

    mat4_identity(m);
    m[0] = t * x * x + c;     m[4] = t * x * y - s * z;   m[8]  = t * x * z + s * y;
    m[1] = t * x * y + s * z;   m[5] = t * y * y + c;     m[9]  = t * y * z - s * x;
    m[2] = t * x * z - s * y;   m[6] = t * y * z + s * x;   m[10] = t * z * z + c;
}

/* ============================================================================
 * 模型数据存储
 * ============================================================================ */

/* 顶点位置数组: vertices[i] = {x, y, z}
 * 例如: vertices[0] = {0.5, 1.0, 0.2} 表示第一个顶点的坐标 */
static float vertices[MAX_VERTICES][3];

/* 面法线数组: normals[i] = {nx, ny, nz}
 * 法线是垂直于表面的单位向量，用于光照计算
 * 例如: normals[0] = {0, 1, 0} 表示第一个面朝上 */
static float normals[MAX_FACES][3];

/* 面索引数组: faces[i][j] = 顶点索引, faces_tex[i][j] = 纹理坐标索引 */
static int faces[MAX_FACES][3];      /* 顶点索引 */
static int faces_tex[MAX_FACES][3];  /* 纹理坐标索引 */

static int vertex_count = 0;  /* 实际加载的顶点数量 */
static int face_count = 0;    /* 实际加载的面数量 */

/* VBO (Vertex Buffer Object) - GPU 显存中的顶点缓冲 */
static GLuint g_vbo_position = 0;  /* 顶点位置缓冲 */
static GLuint g_vbo_normal = 0;    /* 顶点法线缓冲 */
static GLuint g_vbo_texcoord = 0;  /* 纹理坐标缓冲 */
static int g_triangle_count = 0;   /* 三角形数量 */

/* 纹理对象 */
static GLuint g_texture = 0;

/* 纹理坐标数组 */
static float texcoords[MAX_VERTICES][2];
static int texcoord_count = 0;

/* ============================================================================
 * 着色器源代码
 * ============================================================================
 *
 * 添加纹理映射支持：
 *   - 顶点着色器传递纹理坐标
 *   - 片段着色器采样纹理
 */

/* 顶点着色器源代码 */
static const char *vertex_shader_src =
    "#version 120\n"

    /* 自定义 MVP 矩阵 */
    "uniform mat4 uMVP;\n"
    "uniform mat4 uModelView;\n"

    /* varying: 输出到片段着色器，GPU 会在三角形内插值 */
    "varying vec3 vNormal;\n"
    "varying vec2 vTexCoord;\n"

    "void main() {\n"
    /* 使用自定义矩阵变换法线 */
    "    vNormal = mat3(uModelView) * gl_Normal;\n"

    /* 传递纹理坐标 */
    "    vTexCoord = gl_MultiTexCoord0.xy;\n"

    /* 使用自定义 MVP 矩阵变换顶点 */
    "    gl_Position = uMVP * gl_Vertex;\n"
    "}\n";

/* 片段着色器源代码 */
static const char *fragment_shader_src =
    "#version 120\n"

    /* varying: 从顶点着色器接收，已经被 GPU 插值过 */
    "varying vec3 vNormal;\n"
    "varying vec2 vTexCoord;\n"

    /* Uniform 变量 - 由 CPU 通过 glUniform* 设置 */
    "uniform vec3 uLightDir;\n"   /* 光源方向 */
    "uniform vec3 uBaseColor;\n"  /* 基础颜色 */
    "uniform float uAmbient;\n"   /* 环境光强度 */
    "uniform sampler2D uTexture;\n"  /* 纹理采样器 */

    "void main() {\n"
    /* 采样纹理 */
    "    vec4 texColor = texture2D(uTexture, vTexCoord);\n"

    /* 光照计算 */
    "    vec3 N = normalize(vNormal);\n"
    "    vec3 L = normalize(uLightDir);\n"
    "    float diff = max(dot(N, L), 0.0);\n"
    "    float lighting = diff + uAmbient;\n"

    /* 纹理颜色 × 光照 */
    "    vec3 color = texColor.rgb * lighting;\n"
    "    gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

/* 着色器程序 ID (GPU 上的程序句柄) */
static GLuint g_program = 0;

/* 矩阵 Uniform 位置 */
static GLint g_loc_mvp = -1;
static GLint g_loc_modelview = -1;

/* 光照 Uniform 位置 */
static GLint g_loc_lightDir = -1;
static GLint g_loc_baseColor = -1;
static GLint g_loc_ambient = -1;

/* ============================================================================
 * 着色器编译函数
 * ============================================================================
 *
 * 着色器编译流程:
 *   1. glCreateShader()   - 创建着色器对象
 *   2. glShaderSource()   - 设置源代码
 *   3. glCompileShader()  - 编译 (GLSL → GPU 机器码)
 *   4. glGetShaderiv()    - 检查编译结果
 *
 * @param type: GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER
 * @param src:  GLSL 源代码字符串
 * @return:     编译后的着色器 ID
 */
static GLuint compile_shader(GLenum type, const char *src)
{
    /* 创建着色器对象，返回一个 ID (类似文件句柄) */
    GLuint shader = glCreateShader(type);

    /* 设置着色器源代码
     * 参数: 着色器ID, 字符串数量, 字符串数组, 长度数组(NULL=自动) */
    glShaderSource(shader, 1, &src, NULL);

    /* 编译着色器: GLSL 源代码 → GPU 可执行的机器码 */
    glCompileShader(shader);

    /* 检查编译状态 */
    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        /* 编译失败，获取错误日志 */
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        printf("Shader compile error: %s\n", log);
    }
    return shader;
}

/* ============================================================================
 * 着色器程序初始化
 * ============================================================================
 *
 * 着色器程序 = 顶点着色器 + 片段着色器 链接在一起
 *
 * 链接流程:
 *   1. glCreateProgram()  - 创建程序对象
 *   2. glAttachShader()   - 附加着色器
 *   3. glLinkProgram()    - 链接 (类似 C 的链接器)
 *   4. glUseProgram()     - 激活程序 (在渲染循环中调用)
 */
static void init_shaders(void)
{
    /* 编译顶点着色器和片段着色器 */
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

    /* 创建着色器程序 */
    g_program = glCreateProgram();

    /* 附加着色器到程序 */
    glAttachShader(g_program, vs);
    glAttachShader(g_program, fs);

    /* 链接程序 */
    glLinkProgram(g_program);

    /* 检查链接状态 */
    GLint ok;
    glGetProgramiv(g_program, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetProgramInfoLog(g_program, 512, NULL, log);
        printf("Program link error: %s\n", log);
    }

    /* 获取矩阵 Uniform 位置 */
    g_loc_mvp = glGetUniformLocation(g_program, "uMVP");
    g_loc_modelview = glGetUniformLocation(g_program, "uModelView");

    /* 获取光照 Uniform 位置 */
    g_loc_lightDir = glGetUniformLocation(g_program, "uLightDir");
    g_loc_baseColor = glGetUniformLocation(g_program, "uBaseColor");
    g_loc_ambient = glGetUniformLocation(g_program, "uAmbient");

    printf("Shaders compiled and linked successfully!\n");
    printf("Uniform locations: MVP=%d, ModelView=%d, lightDir=%d\n",
           g_loc_mvp, g_loc_modelview, g_loc_lightDir);
}

/* ============================================================================
 * 生成程序化纹理 - 棋盘格
 * ============================================================================
 *
 * 生成一个简单的棋盘格纹理来演示纹理映射
 * 不需要外部图片文件，代码自包含
 */
static void create_checkerboard_texture(void)
{
    const int size = 256;  /* 纹理大小 256x256 */
    const int grid = 16;   /* 每个格子大小 */

    unsigned char *data = malloc(size * size * 3);  /* RGB 格式 */

    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            /* 判断是黑格还是白格 */
            int is_white = ((x / grid) + (y / grid)) % 2;
            unsigned char color = is_white ? 255 : 64;

            int idx = (y * size + x) * 3;
            data[idx + 0] = color;  /* R */
            data[idx + 1] = color;  /* G */
            data[idx + 2] = color;  /* B */
        }
    }

    /* 创建 OpenGL 纹理 */
    glGenTextures(1, &g_texture);
    glBindTexture(GL_TEXTURE_2D, g_texture);

    /* 设置纹理参数 */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* 上传纹理数据到 GPU */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);
    printf("Checkerboard texture created: %dx%d\n", size, size);
}

/* ============================================================================
 * OBJ 文件加载
 * ============================================================================
 *
 * OBJ 是一种简单的 3D 模型文件格式:
 *   v x y z      - 顶点坐标
 *   vn x y z     - 顶点法线 (本代码未使用，自己计算)
 *   vt u v       - 纹理坐标
 *   f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 - 面
 */
static void load_obj(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) { printf("Cannot open %s\n", filename); return; }

    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        /* 解析顶点行: "v x y z" */
        if (line[0] == 'v' && line[1] == ' ')
        {
            sscanf(line, "v %f %f %f",
                   &vertices[vertex_count][0],   /* x */
                   &vertices[vertex_count][1],   /* y */
                   &vertices[vertex_count][2]);  /* z */
            vertex_count++;
        }
        /* 解析纹理坐标行: "vt u v" */
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
        {
            sscanf(line, "vt %f %f",
                   &texcoords[texcoord_count][0],  /* u */
                   &texcoords[texcoord_count][1]); /* v */
            texcoord_count++;
        }
        /* 解析面行: "f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3" */
        else if (line[0] == 'f' && line[1] == ' ')
        {
            int v1, v2, v3, vt1, vt2, vt3;
            sscanf(line, "f %d/%d/%*d %d/%d/%*d %d/%d/%*d",
                   &v1, &vt1, &v2, &vt2, &v3, &vt3);
            /* OBJ 索引从 1 开始，C 数组从 0 开始，所以 -1 */
            faces[face_count][0] = v1 - 1;
            faces[face_count][1] = v2 - 1;
            faces[face_count][2] = v3 - 1;
            faces_tex[face_count][0] = vt1 - 1;
            faces_tex[face_count][1] = vt2 - 1;
            faces_tex[face_count][2] = vt3 - 1;
            face_count++;
        }
    }
    fclose(f);
    printf("Loaded: %d vertices, %d texcoords, %d faces\n",
           vertex_count, texcoord_count, face_count);
}

/* ============================================================================
 * 计算面法线
 * ============================================================================
 *
 * 法线 (Normal): 垂直于表面的单位向量，用于光照计算
 *
 * 计算方法: 叉积 (Cross Product)
 *   给定三角形三个顶点 v0, v1, v2
 *   边向量: a = v1 - v0, b = v2 - v0
 *   法线: n = a × b (叉积)
 *
 *        v1
 *       /|
 *      / |
 *     /  |  ← 法线 n 垂直于这个面
 *    /   |
 *   v0───v2
 *      a→   b→
 */
static void compute_normals(void)
{
    for (int i = 0; i < face_count; i++)
    {
        /* 获取三角形的三个顶点 */
        float *v0 = vertices[faces[i][0]];
        float *v1 = vertices[faces[i][1]];
        float *v2 = vertices[faces[i][2]];

        /* 计算两条边向量 */
        float ax = v1[0] - v0[0], ay = v1[1] - v0[1], az = v1[2] - v0[2];  /* a = v1 - v0 */
        float bx = v2[0] - v0[0], by = v2[1] - v0[1], bz = v2[2] - v0[2];  /* b = v2 - v0 */

        /* 叉积: n = a × b
         * nx = ay*bz - az*by
         * ny = az*bx - ax*bz
         * nz = ax*by - ay*bx */
        normals[i][0] = ay * bz - az * by;
        normals[i][1] = az * bx - ax * bz;
        normals[i][2] = ax * by - ay * bx;

        /* 归一化: 使法线长度为 1 (单位向量)
         * 这样 dot(N, L) 的结果才是 cos(夹角) */
        float len = sqrtf(normals[i][0] * normals[i][0] +
                          normals[i][1] * normals[i][1] +
                          normals[i][2] * normals[i][2]);
        if (len > 0)
        {
            normals[i][0] /= len;
            normals[i][1] /= len;
            normals[i][2] /= len;
        }
    }
}

/* ============================================================================
 * 顶点归一化
 * ============================================================================
 *
 * 目的: 把模型缩放和平移到原点附近，大小约为 [-1, 1]
 *
 * 为什么需要?
 *   - 不同模型的原始坐标范围差异很大
 *   - 归一化后，相机参数可以通用
 *   - 模型居中显示，不会跑出屏幕
 *
 * 步骤:
 *   1. 找到包围盒 (bounding box) 的最小/最大坐标
 *   2. 计算中心点，平移到原点
 *   3. 计算缩放因子，缩放到 [-1, 1] 范围
 */
static void normalize_vertices(void)
{
    /* 初始化包围盒的最小/最大值 */
    float minv[3] = {1e9, 1e9, 1e9};
    float maxv[3] = {-1e9, -1e9, -1e9};

    /* 遍历所有顶点，找到包围盒 */
    for (int i = 0; i < vertex_count; i++)
    {
        for (int j = 0; j < 3; j++)  /* j: 0=x, 1=y, 2=z */
        {
            if (vertices[i][j] < minv[j]) { minv[j] = vertices[i][j]; }
            if (vertices[i][j] > maxv[j]) { maxv[j] = vertices[i][j]; }
        }
    }

    /* 计算包围盒中心 */
    float cx = (minv[0] + maxv[0]) / 2;
    float cy = (minv[1] + maxv[1]) / 2;
    float cz = (minv[2] + maxv[2]) / 2;

    /* 计算缩放因子: 让最大维度变成 2 (即 [-1, 1]) */
    float scale = 2.0f / fmaxf(fmaxf(maxv[0] - minv[0], maxv[1] - minv[1]), maxv[2] - minv[2]);

    /* 应用变换: 先平移到原点，再缩放 */
    for (int i = 0; i < vertex_count; i++)
    {
        vertices[i][0] = (vertices[i][0] - cx) * scale;
        vertices[i][1] = (vertices[i][1] - cy) * scale;
        vertices[i][2] = (vertices[i][2] - cz) * scale;
    }
}

/* ============================================================================
 * 创建 VBO - 上传顶点数据到 GPU 显存
 * ============================================================================
 *
 * VBO 优势:
 *   - 数据只上传一次，存储在 GPU 显存
 *   - 渲染时直接从显存读取，无需每帧传输
 *   - 一次 glDrawArrays 调用绘制所有顶点
 *
 * 数据组织:
 *   将面索引展开为顶点数组，每个三角形 3 个顶点
 */
static void create_vbo(void)
{
    /* 计算需要的顶点数量 (每个面 3 个顶点) */
    g_triangle_count = face_count;
    int total_vertices = face_count * 3;

    /* 分配临时数组，存储展开后的顶点数据 */
    float *positions = malloc(total_vertices * 3 * sizeof(float));
    float *normals_data = malloc(total_vertices * 3 * sizeof(float));
    float *texcoords_data = malloc(total_vertices * 2 * sizeof(float));

    /* 展开面索引为顶点数组 */
    for (int i = 0; i < face_count; i++)
    {
        /* 每个面的法线 */
        float *n = normals[i];

        /* 三个顶点 */
        for (int j = 0; j < 3; j++)
        {
            int idx = i * 3 + j;
            int v_idx = faces[i][j];
            int vt_idx = faces_tex[i][j];

            /* 复制顶点位置 */
            positions[idx * 3 + 0] = vertices[v_idx][0];
            positions[idx * 3 + 1] = vertices[v_idx][1];
            positions[idx * 3 + 2] = vertices[v_idx][2];

            /* 复制法线 (每个面的三个顶点共享同一个法线) */
            normals_data[idx * 3 + 0] = n[0];
            normals_data[idx * 3 + 1] = n[1];
            normals_data[idx * 3 + 2] = n[2];

            /* 复制纹理坐标 */
            texcoords_data[idx * 2 + 0] = texcoords[vt_idx][0];
            texcoords_data[idx * 2 + 1] = texcoords[vt_idx][1];
        }
    }

    /* 创建位置 VBO */
    glGenBuffers(1, &g_vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo_position);
    glBufferData(GL_ARRAY_BUFFER, total_vertices * 3 * sizeof(float),
                 positions, GL_STATIC_DRAW);

    /* 创建法线 VBO */
    glGenBuffers(1, &g_vbo_normal);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo_normal);
    glBufferData(GL_ARRAY_BUFFER, total_vertices * 3 * sizeof(float),
                 normals_data, GL_STATIC_DRAW);

    /* 创建纹理坐标 VBO */
    glGenBuffers(1, &g_vbo_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo_texcoord);
    glBufferData(GL_ARRAY_BUFFER, total_vertices * 2 * sizeof(float),
                 texcoords_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);  /* 解绑 */

    /* 释放临时数组 */
    free(positions);
    free(normals_data);
    free(texcoords_data);

    printf("VBO created: %d triangles, %d vertices\n", g_triangle_count, total_vertices);
}

/* ============================================================================
 * 渲染线程
 * ============================================================================
 *
 * OpenGL 渲染必须在创建上下文的线程中进行
 * 这个函数在独立线程中运行，持续渲染画面
 *
 * 渲染循环流程:
 *   1. 设置变换矩阵 (Projection, ModelView)
 *   2. 清空帧缓冲
 *   3. 启用着色器
 *   4. 绘制三角形
 *   5. 读取像素，显示到 LCD
 *   6. 等待下一帧
 */
static void *render_thread(void *arg)
{
    (void)arg;

    /* 获取屏幕尺寸 */
    int w = sim_get_width();
    int h = sim_get_height();

    /* ========== 初始化 GLFW 和 OpenGL 上下文 ========== */
    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  /* 隐藏窗口，我们只用离屏渲染 */
    GLFWwindow *win = glfwCreateWindow(w, h, "GL", NULL, NULL);
    glfwMakeContextCurrent(win);  /* 绑定 OpenGL 上下文到当前线程 */

    /* 分配帧缓冲内存 */
    uint32_t *fb = malloc(w * h * 4);   /* OpenGL 读取的原始像素 */
    uint32_t *tmp = malloc(w * h * 4);  /* 转换后的像素 (用于 LCD) */

    /* ========== OpenGL 初始化 ========== */
    glViewport(0, 0, w, h);      /* 设置视口大小 */
    glEnable(GL_DEPTH_TEST);     /* 启用深度测试 (近的物体遮挡远的) */

    /* 初始化着色器程序 */
    init_shaders();

    /* 创建纹理 */
    create_checkerboard_texture();

    /* 创建 VBO - 上传顶点数据到 GPU */
    create_vbo();

    /* 帧计数器，用于动画 */
    int frame = 0;

    /* ========== 渲染循环 ========== */
    while (1)
    {
        frame++;  /* 每帧递增 */

        /* ========== 手动构建矩阵 ========== */

        /* 1. 投影矩阵 */
        mat4 proj;
        mat4_perspective(proj, 45.0f * M_PI / 180.0f, (float)w / h, 0.1f, 100.0f);

        /* 2. 视图矩阵 */
        float eye[3] = {0, 0, mouse_zoom};
        float center[3] = {0, 0, 0};
        float up[3] = {0, 1, 0};
        mat4 view;
        mat4_look_at(view, eye, center, up);

        /* 3. 模型矩阵 (旋转) */
        mat4 rot_x, rot_y, model;
        mat4_rotate(rot_x, mouse_rot_x * M_PI / 180.0f, 1, 0, 0);
        mat4_rotate(rot_y, mouse_rot_y * M_PI / 180.0f, 0, 1, 0);
        mat4_multiply(model, rot_y, rot_x);

        /* 4. 组合 MVP = Projection × View × Model */
        mat4 modelview, mvp;
        mat4_multiply(modelview, view, model);
        mat4_multiply(mvp, proj, modelview);

        /* ---------- 清空帧缓冲 ---------- */
        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);  /* 背景色: 深蓝灰 */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* ---------- 启用着色器程序 ---------- */
        glUseProgram(g_program);

        /* 通过 Uniform 传递矩阵 */
        glUniformMatrix4fv(g_loc_mvp, 1, GL_FALSE, mvp);
        glUniformMatrix4fv(g_loc_modelview, 1, GL_FALSE, modelview);

        /* ---------- 设置光照参数 ---------- */
        float angle = frame * 0.02f;  /* 每帧旋转一点 */
        float lx = cosf(angle) * 2.0f;  /* 光源绕 Y 轴旋转 */
        float ly = 1.0f;
        float lz = sinf(angle) * 2.0f;

        glUniform3f(g_loc_lightDir, lx, ly, lz);
        glUniform3f(g_loc_baseColor, 0.9f, 0.7f, 0.6f);
        glUniform1f(g_loc_ambient, 0.2f);

        /* ---------- 绑定纹理 ---------- */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_texture);
        glEnable(GL_TEXTURE_2D);

        /* ---------- 使用 VBO 绘制模型 ---------- */

        /* 启用顶点属性数组 */
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        /* 绑定位置 VBO */
        glBindBuffer(GL_ARRAY_BUFFER, g_vbo_position);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        /* 绑定法线 VBO */
        glBindBuffer(GL_ARRAY_BUFFER, g_vbo_normal);
        glNormalPointer(GL_FLOAT, 0, 0);

        /* 绑定纹理坐标 VBO */
        glBindBuffer(GL_ARRAY_BUFFER, g_vbo_texcoord);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);

        /* 一次调用绘制所有三角形 */
        glDrawArrays(GL_TRIANGLES, 0, g_triangle_count * 3);

        /* 禁用顶点属性数组 */
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /* 关闭着色器 */
        glUseProgram(0);

        /* 等待 GPU 完成所有渲染操作 */
        glFinish();

        /* ---------- 读取像素并显示 ---------- */
        glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, fb);

        /* 像素格式转换 + 垂直翻转 */
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                /* 从底部读取 (翻转 Y) */
                uint32_t p = fb[(h - 1 - y) * w + x];

                /* OpenGL: RGBA (R在低位)
                 * LCD:    ARGB (A在高位, R在次高位)
                 * 需要重新排列字节顺序 */
                uint8_t r = p & 0xFF;
                uint8_t g = (p >> 8) & 0xFF;
                uint8_t b = (p >> 16) & 0xFF;
                tmp[y * w + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
            }
        }

        /* 显示到 LCD */
        port_direct_draw_bitmap_to_lcd(0, 0, w, h, (uint8_t *)tmp);

        /* 等待约 16ms (约 60 FPS) */
        usleep(16000);
    }

    free(fb);
    free(tmp);
    return NULL;
}

/* ============================================================================
 * 程序入口 (构造函数)
 * ============================================================================
 *
 * __attribute__((constructor(1001))): GCC 扩展
 *   - 在 main() 之前自动执行
 *   - 1001 是优先级 (数字越小越先执行)
 *
 * 初始化流程:
 *   1. 加载 OBJ 模型文件
 *   2. 归一化顶点坐标
 *   3. 计算面法线
 *   4. 启动渲染线程
 */
__attribute__((constructor(1001))) static void opengl_demo(void)
{
    /* 加载 3D 模型 */
    load_obj("example/face.obj");

    /* 归一化顶点到 [-1, 1] 范围 */
    normalize_vertices();

    /* 计算每个面的法线向量 */
    compute_normals();

    /* 创建渲染线程
     * OpenGL 渲染在独立线程中进行，不阻塞主程序 */
    pthread_t t;
    pthread_create(&t, NULL, render_thread, NULL);
    printf("OpenGL render thread started\n");
}
