/*
 * ============================================================================
 * OpenGL + GLFW Demo - Step 2: Uniform 变量
 * ============================================================================
 *
 * 学习目标：
 *   1. 理解 Uniform 变量 - CPU 向 GPU 传递全局参数
 *   2. 动态控制光照参数 (光源方向、颜色)
 *   3. 理解 glGetUniformLocation / glUniform* 的用法
 *
 * Uniform vs Attribute vs Varying:
 *   ┌──────────┬─────────────────┬─────────────────────────┐
 *   │ 类型     │ 来源            │ 特点                    │
 *   ├──────────┼─────────────────┼─────────────────────────┤
 *   │ Attribute│ glVertex/glNormal│ 每个顶点不同           │
 *   │ Uniform  │ glUniform*      │ 全局共享，CPU动态设置   │
 *   │ Varying  │ 顶点着色器输出  │ 自动插值到片段着色器    │
 *   └──────────┴─────────────────┴─────────────────────────┘
 *
 * 数据流:
 *   CPU                              GPU
 *   ───                              ───
 *   glUniform3f(uLightDir, x,y,z) ──→ uniform vec3 uLightDir
 *   glUniform3f(uBaseColor, r,g,b)──→ uniform vec3 uBaseColor
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
#include <GL/glu.h>       /* GLU: OpenGL 工具库，提供 gluPerspective, gluLookAt 等 */
#include "fb_sdl.h"       /* 自定义: 把渲染结果显示到 LCD */

/* 模型数据的最大容量 */
#define MAX_VERTICES 1000  /* 最多 1000 个顶点 */
#define MAX_FACES 2000     /* 最多 2000 个三角面 */

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

/* 面索引数组: faces[i] = {v0, v1, v2}
 * 每个面由 3 个顶点索引组成，指向 vertices 数组
 * 例如: faces[0] = {0, 1, 2} 表示第一个三角形用顶点 0, 1, 2 */
static int faces[MAX_FACES][3];

static int vertex_count = 0;  /* 实际加载的顶点数量 */
static int face_count = 0;    /* 实际加载的面数量 */

/* ============================================================================
 * 着色器源代码
 * ============================================================================
 *
 * 着色器是运行在 GPU 上的小程序，用 GLSL 语言编写。
 *
 * 两种着色器：
 *   1. 顶点着色器 (Vertex Shader): 每个顶点执行一次
 *      - 输入: 顶点位置、法线等
 *      - 输出: 变换后的位置 (gl_Position)
 *      - 本模型有 740 个顶点，所以执行 740 次 (并行)
 *
 *   2. 片段着色器 (Fragment Shader): 每个像素执行一次
 *      - 输入: 从顶点着色器插值得到的数据
 *      - 输出: 像素颜色 (gl_FragColor)
 *      - 屏幕 454x454 ≈ 20万像素，所以执行约 20万次 (并行)
 */

/* 顶点着色器源代码
 *
 * #version 120: 使用 GLSL 1.20 版本 (兼容 OpenGL 2.1)
 *
 * 内置输入变量 (由固定管线提供):
 *   gl_Vertex  - 顶点位置，来自 glVertex3fv()
 *   gl_Normal  - 顶点法线，来自 glNormal3fv()
 *   gl_ModelViewProjectionMatrix - MVP 矩阵，来自 glMatrixMode/gluPerspective/gluLookAt
 *   gl_NormalMatrix - 法线变换矩阵 (ModelView 的逆转置)
 *
 * varying 变量:
 *   用于从顶点着色器传递数据到片段着色器
 *   GPU 会自动在三角形内部进行线性插值
 */
static const char *vertex_shader_src =
    "#version 120\n"

    /* varying: 输出到片段着色器，GPU 会在三角形内插值 */
    "varying vec3 vNormal;\n"

    "void main() {\n"
    /* 将法线从模型空间变换到视图空间
     * gl_NormalMatrix = (ModelView矩阵的左上3x3)的逆转置
     * 为什么要逆转置? 因为法线不能直接用 ModelView 变换，会变形 */
    "    vNormal = gl_NormalMatrix * gl_Normal;\n"

    /* 将顶点位置从模型空间变换到裁剪空间
     * gl_ModelViewProjectionMatrix = Projection × View × Model
     * 这是固定管线帮我们算好的组合矩阵 */
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "}\n";

/* 片段着色器源代码
 *
 * 【第二步改动】添加 Uniform 变量:
 *   - uLightDir:  光源方向，由 CPU 每帧设置
 *   - uBaseColor: 基础颜色，由 CPU 设置
 *   - uAmbient:   环境光强度
 *
 * Uniform 的特点:
 *   - 所有像素共享同一个值
 *   - CPU 可以动态修改
 *   - 适合传递: 光源参数、时间、相机位置等
 */
static const char *fragment_shader_src =
    "#version 120\n"

    /* varying: 从顶点着色器接收，已经被 GPU 插值过 */
    "varying vec3 vNormal;\n"

    /* 【新增】Uniform 变量 - 由 CPU 通过 glUniform* 设置 */
    "uniform vec3 uLightDir;\n"   /* 光源方向 */
    "uniform vec3 uBaseColor;\n"  /* 基础颜色 */
    "uniform float uAmbient;\n"   /* 环境光强度 */

    "void main() {\n"
    /* 归一化法线 (插值后长度可能不是 1) */
    "    vec3 N = normalize(vNormal);\n"

    /* 【改动】使用 Uniform 变量，而不是硬编码 */
    "    vec3 L = normalize(uLightDir);\n"

    /* 漫反射计算 + 环境光 */
    "    float diff = max(dot(N, L), 0.0);\n"
    "    float lighting = diff + uAmbient;\n"  /* 漫反射 + 环境光 */

    /* 【改动】使用 Uniform 颜色 */
    "    vec3 color = uBaseColor * lighting;\n"

    /* 输出像素颜色 */
    "    gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

/* 着色器程序 ID (GPU 上的程序句柄) */
static GLuint g_program = 0;

/* 【新增】Uniform 变量的位置 (Location)
 *
 * 每个 Uniform 在着色器程序中有一个"位置"编号
 * 需要先用 glGetUniformLocation 获取，然后用 glUniform* 设置值
 */
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

    /* 链接程序: 把顶点着色器和片段着色器连接起来
     * 检查 varying 变量是否匹配等 */
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

    /* 【新增】获取 Uniform 变量的位置
     *
     * glGetUniformLocation: 查询 Uniform 变量在程序中的位置
     * 返回 -1 表示变量不存在或被优化掉了
     *
     * 为什么需要位置?
     *   着色器编译后，变量名变成了数字编号
     *   我们需要这个编号才能设置值
     */
    g_loc_lightDir = glGetUniformLocation(g_program, "uLightDir");
    g_loc_baseColor = glGetUniformLocation(g_program, "uBaseColor");
    g_loc_ambient = glGetUniformLocation(g_program, "uAmbient");

    printf("Shaders compiled and linked successfully!\n");
    printf("Uniform locations: lightDir=%d, baseColor=%d, ambient=%d\n",
           g_loc_lightDir, g_loc_baseColor, g_loc_ambient);
}

/* ============================================================================
 * OBJ 文件加载
 * ============================================================================
 *
 * OBJ 是一种简单的 3D 模型文件格式:
 *   v x y z      - 顶点坐标
 *   vn x y z     - 顶点法线 (本代码未使用，自己计算)
 *   vt u v       - 纹理坐标 (本代码未使用)
 *   f v1 v2 v3   - 面，由 3 个顶点索引组成
 *
 * 例如:
 *   v 1.0 2.0 3.0           # 顶点 1
 *   v 4.0 5.0 6.0           # 顶点 2
 *   v 7.0 8.0 9.0           # 顶点 3
 *   f 1 2 3                 # 面，使用顶点 1,2,3 (索引从 1 开始)
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
        /* 解析面行: "f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3"
         * %*d 表示读取但忽略 (跳过纹理和法线索引) */
        else if (line[0] == 'f' && line[1] == ' ')
        {
            int v1, v2, v3;
            sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &v1, &v2, &v3);
            /* OBJ 索引从 1 开始，C 数组从 0 开始，所以 -1 */
            faces[face_count][0] = v1 - 1;
            faces[face_count][1] = v2 - 1;
            faces[face_count][2] = v3 - 1;
            face_count++;
        }
    }
    fclose(f);
    printf("Loaded: %d vertices, %d faces\n", vertex_count, face_count);
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

    /* 【新增】帧计数器，用于动画 */
    int frame = 0;

    /* ========== 渲染循环 ========== */
    while (1)
    {
        frame++;  /* 每帧递增 */

        /* ---------- 设置投影矩阵 ----------
         * 投影矩阵: 定义相机的"镜头"
         * gluPerspective(fov, aspect, near, far)
         *   fov    = 45°  视野角度
         *   aspect = w/h  宽高比
         *   near   = 0.1  近裁剪面
         *   far    = 100  远裁剪面
         */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (double)w / h, 0.1, 100.0);

        /* ---------- 设置模型视图矩阵 ----------
         * 模型视图矩阵 = View × Model
         *
         * gluLookAt: 设置相机位置和朝向
         *   eye    = (0, 0, mouse_zoom)  相机位置
         *   center = (0, 0, 0)           看向原点
         *   up     = (0, 1, 0)           上方向
         *
         * glRotatef: 旋转模型 (响应鼠标拖动)
         */
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, mouse_zoom, 0, 0, 0, 0, 1, 0);

        glRotatef(mouse_rot_x, 1, 0, 0);  /* 绕 X 轴旋转 */
        glRotatef(mouse_rot_y, 0, 1, 0);  /* 绕 Y 轴旋转 */

        /* ---------- 清空帧缓冲 ---------- */
        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);  /* 背景色: 深蓝灰 */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* ---------- 启用着色器程序 ----------
         * 从这里开始，所有绘制都会使用我们的着色器
         * 而不是固定管线的默认处理 */
        glUseProgram(g_program);

        /* ---------- 【改动】设置 Uniform 变量 - 让光源旋转！ ----------
         *
         * 这就是 Uniform 的意义：
         * 我们可以每帧传入不同的值，实现动态效果
         *
         * 如果光源方向写死在着色器里，就做不到这个！
         */
        float angle = frame * 0.02f;  /* 每帧旋转一点 */
        float lx = cosf(angle) * 2.0f;  /* 光源绕 Y 轴旋转 */
        float ly = 1.0f;
        float lz = sinf(angle) * 2.0f;

        glUniform3f(g_loc_lightDir, lx, ly, lz);  /* 每帧不同的光源方向！ */
        glUniform3f(g_loc_baseColor, 0.9f, 0.7f, 0.6f);
        glUniform1f(g_loc_ambient, 0.2f);

        /* ---------- 绘制模型 ----------
         * glBegin/glEnd: 立即模式绘制 (简单但效率低)
         *
         * 对于每个三角形:
         *   1. glNormal3fv: 设置法线 → 传给着色器的 gl_Normal
         *   2. glVertex3fv: 设置顶点 → 传给着色器的 gl_Vertex
         *
         * GPU 会并行执行所有顶点的着色器代码
         */
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < face_count; i++)
        {
            glNormal3fv(normals[i]);              /* 设置这个面的法线 */
            glVertex3fv(vertices[faces[i][0]]);   /* 第 1 个顶点 */
            glVertex3fv(vertices[faces[i][1]]);   /* 第 2 个顶点 */
            glVertex3fv(vertices[faces[i][2]]);   /* 第 3 个顶点 */
        }
        glEnd();

        /* 关闭着色器 (恢复固定管线，虽然这里没用到) */
        glUseProgram(0);

        /* 等待 GPU 完成所有渲染操作 */
        glFinish();

        /* ---------- 读取像素并显示 ----------
         * glReadPixels: 从 GPU 帧缓冲读取像素到 CPU 内存
         *
         * 注意: OpenGL 的 Y 轴是向上的，LCD 的 Y 轴是向下的
         * 所以需要垂直翻转 */
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
