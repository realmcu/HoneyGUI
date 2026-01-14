# OpenGL 可编程管线学习笔记

## 学习路线

| 步骤 | 主题 | 状态 |
|------|------|------|
| 1 | 最简单的着色器 | ✅ 完成 |
| 2 | Uniform 变量 | ✅ 完成 |
| 3 | 自定义矩阵 (摆脱 gluPerspective) | 待做 |
| 4 | VBO 顶点缓冲 (摆脱 glBegin/glEnd) | 待做 |
| 5 | 纹理映射 | 待做 |

---

## 核心概念

### 1. 固定管线 vs 可编程管线

```
固定管线 (旧):
  glVertex3fv() ──→ [GPU内部固定算法] ──→ 屏幕像素
                    (你无法修改)

可编程管线 (新):
  glVertex3fv() ──→ [你写的着色器代码] ──→ 屏幕像素
                    (完全可控)
```

### 2. 两种着色器

| 着色器 | 执行时机 | 执行次数 | 主要职责 |
|--------|----------|----------|----------|
| 顶点着色器 | 每个顶点 | 顶点数 (如 740) | 坐标变换 |
| 片段着色器 | 每个像素 | 像素数 (如 20万) | 计算颜色 |

### 3. GPU 并行处理

```
CPU: 串行处理
  顶点0 → 顶点1 → 顶点2 → ... (一个一个)

GPU: 并行处理
  顶点0 ─┐
  顶点1 ─┼→ 同时完成！(几千个核心)
  顶点2 ─┤
  ...   ─┘
```

---

## 变量类型

### Attribute (属性)
- **来源**: `glVertex3fv()`, `glNormal3fv()`
- **特点**: 每个顶点不同
- **示例**: 顶点位置、法线、纹理坐标

```c
// CPU 端
glVertex3fv(pos);    // → gl_Vertex
glNormal3fv(normal); // → gl_Normal
```

### Uniform (统一变量)
- **来源**: `glUniform*()`
- **特点**: 全局共享，CPU 动态设置
- **示例**: 矩阵、光源、时间、颜色

```c
// CPU 端
GLint loc = glGetUniformLocation(program, "uLightDir");
glUniform3f(loc, 1.0, 1.0, 2.0);

// GPU 端 (着色器)
uniform vec3 uLightDir;
```

### Varying (易变变量)
- **来源**: 顶点着色器输出
- **特点**: GPU 自动在三角形内插值
- **示例**: 传递法线、颜色到片段着色器

```glsl
// 顶点着色器
varying vec3 vNormal;
void main() {
    vNormal = gl_NormalMatrix * gl_Normal;  // 输出
}

// 片段着色器
varying vec3 vNormal;  // 接收 (已插值)
void main() {
    vec3 N = normalize(vNormal);
}
```

---

## 坐标变换流程

```
模型空间 ──→ 世界空间 ──→ 相机空间 ──→ 裁剪空间 ──→ 屏幕空间
  原始顶点    Model矩阵    View矩阵    Projection    视口变换
              (旋转平移)   (相机位置)   (透视投影)
```

### 矩阵设置

```c
// 投影矩阵 - 定义"镜头"
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45.0, w/h, 0.1, 100.0);  // 透视投影

// 模型视图矩阵 - 定义"场景"
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);   // 相机位置
glRotatef(45, 0, 1, 0);                  // 模型旋转
```

### 着色器中使用

```glsl
// 组合矩阵 (固定管线提供)
gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

// 等价于
gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
```

---

## 光照计算

### 漫反射 (Lambertian)

```glsl
vec3 N = normalize(vNormal);     // 表面法线
vec3 L = normalize(uLightDir);   // 光源方向
float diff = max(dot(N, L), 0.0); // cos(夹角)

// dot(N, L) = cos(θ)
// 面向光源: cos ≈ 1 (亮)
// 背向光源: cos ≈ 0 (暗)
```

### 法线的作用

```
        光源
          ↓
    N ↗   │
      \   │
       \  │
        \ │
    ──────●────── 表面

N·L = cos(θ) = 亮度
```

---

## 着色器编译流程

```c
// 1. 创建着色器
GLuint shader = glCreateShader(GL_VERTEX_SHADER);

// 2. 设置源代码
glShaderSource(shader, 1, &src, NULL);

// 3. 编译
glCompileShader(shader);

// 4. 创建程序
GLuint program = glCreateProgram();

// 5. 附加着色器
glAttachShader(program, vertexShader);
glAttachShader(program, fragmentShader);

// 6. 链接
glLinkProgram(program);

// 7. 使用
glUseProgram(program);
```

---

## OpenGL 函数命名规则

```
gl      Vertex    3       f       v
│         │       │       │       │
│         │       │       │       └── v = vector (数组指针)
│         │       │       └── f = float 类型
│         │       └── 3 = 3个分量 (x,y,z)
│         └── 功能名
└── OpenGL 前缀
```

| 后缀 | 含义 |
|------|------|
| `3f` | 3 个 float 参数 |
| `3fv` | float[3] 数组指针 |
| `4d` | 4 个 double 参数 |
| `Matrix4fv` | 4x4 float 矩阵 |

---

## 关键 API 速查

### 着色器相关
| 函数 | 作用 |
|------|------|
| `glCreateShader(type)` | 创建着色器 |
| `glShaderSource(...)` | 设置源代码 |
| `glCompileShader(s)` | 编译 |
| `glCreateProgram()` | 创建程序 |
| `glAttachShader(p, s)` | 附加着色器 |
| `glLinkProgram(p)` | 链接 |
| `glUseProgram(p)` | 启用/禁用 |

### Uniform 相关
| 函数 | 作用 |
|------|------|
| `glGetUniformLocation(p, "name")` | 获取位置 |
| `glUniform1f(loc, v)` | 设置 float |
| `glUniform3f(loc, x, y, z)` | 设置 vec3 |
| `glUniformMatrix4fv(loc, ...)` | 设置 mat4 |

### 矩阵相关
| 函数 | 作用 |
|------|------|
| `glMatrixMode(mode)` | 切换当前矩阵 |
| `glLoadIdentity()` | 重置为单位矩阵 |
| `gluPerspective(...)` | 透视投影 |
| `gluLookAt(...)` | 设置相机 |
| `glRotatef(angle, x, y, z)` | 旋转 |
| `glTranslatef(x, y, z)` | 平移 |

---

## 下一步学习

### 第三步: 自定义矩阵
- 自己构建 Model/View/Projection 矩阵
- 摆脱 `gluPerspective`, `gluLookAt`
- 理解矩阵数学

### 第四步: VBO 顶点缓冲
- 摆脱 `glBegin/glEnd` (效率低)
- 使用 `glGenBuffers`, `glBufferData`
- 顶点数据一次性上传到 GPU

### 第五步: 纹理映射
- 加载图片到 GPU
- UV 坐标
- `sampler2D` 采样器
