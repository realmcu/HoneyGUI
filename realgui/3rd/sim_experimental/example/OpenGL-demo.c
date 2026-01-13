/*
 * OpenGL + GLFW Demo - Interactive 3D OBJ Rendering
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include "fb_sdl.h"

#define MAX_VERTICES 1000
#define MAX_FACES 2000

static float vertices[MAX_VERTICES][3];
static float normals[MAX_FACES][3];
static int faces[MAX_FACES][3];
static int vertex_count = 0;
static int face_count = 0;

static void load_obj(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) { printf("Cannot open %s\n", filename); return; }

    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            sscanf(line, "v %f %f %f",
                   &vertices[vertex_count][0],
                   &vertices[vertex_count][1],
                   &vertices[vertex_count][2]);
            vertex_count++;
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            int v1, v2, v3;
            sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &v1, &v2, &v3);
            faces[face_count][0] = v1 - 1;
            faces[face_count][1] = v2 - 1;
            faces[face_count][2] = v3 - 1;
            face_count++;
        }
    }
    fclose(f);
    printf("Loaded: %d vertices, %d faces\n", vertex_count, face_count);
}

static void compute_normals(void)
{
    for (int i = 0; i < face_count; i++)
    {
        float *v0 = vertices[faces[i][0]];
        float *v1 = vertices[faces[i][1]];
        float *v2 = vertices[faces[i][2]];

        float ax = v1[0] - v0[0], ay = v1[1] - v0[1], az = v1[2] - v0[2];
        float bx = v2[0] - v0[0], by = v2[1] - v0[1], bz = v2[2] - v0[2];

        normals[i][0] = ay * bz - az * by;
        normals[i][1] = az * bx - ax * bz;
        normals[i][2] = ax * by - ay * bx;

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

static void normalize_vertices(void)
{
    float minv[3] = {1e9, 1e9, 1e9};
    float maxv[3] = {-1e9, -1e9, -1e9};

    for (int i = 0; i < vertex_count; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (vertices[i][j] < minv[j]) { minv[j] = vertices[i][j]; }
            if (vertices[i][j] > maxv[j]) { maxv[j] = vertices[i][j]; }
        }
    }

    float cx = (minv[0] + maxv[0]) / 2;
    float cy = (minv[1] + maxv[1]) / 2;
    float cz = (minv[2] + maxv[2]) / 2;
    float scale = 2.0f / fmaxf(fmaxf(maxv[0] - minv[0], maxv[1] - minv[1]), maxv[2] - minv[2]);

    for (int i = 0; i < vertex_count; i++)
    {
        vertices[i][0] = (vertices[i][0] - cx) * scale;
        vertices[i][1] = (vertices[i][1] - cy) * scale;
        vertices[i][2] = (vertices[i][2] - cz) * scale;
    }
}

static void *render_thread(void *arg)
{
    (void)arg;
    int w = sim_get_width();
    int h = sim_get_height();

    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow *win = glfwCreateWindow(w, h, "GL", NULL, NULL);
    glfwMakeContextCurrent(win);

    uint32_t *fb = malloc(w * h * 4);
    uint32_t *tmp = malloc(w * h * 4);

    glViewport(0, 0, w, h);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_pos[] = {1.0f, 1.0f, 2.0f, 0.0f};
    GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    GLfloat mat_color[] = {0.9f, 0.7f, 0.6f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_color);

    while (1)
    {
        // Projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (double)w / h, 0.1, 100.0);

        // Modelview with mouse interaction
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, mouse_zoom, 0, 0, 0, 0, 1, 0);

        // Light fixed relative to camera (like headlight)
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

        glRotatef(mouse_rot_x, 1, 0, 0);
        glRotatef(mouse_rot_y, 0, 1, 0);

        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw model
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < face_count; i++)
        {
            glNormal3fv(normals[i]);
            glVertex3fv(vertices[faces[i][0]]);
            glVertex3fv(vertices[faces[i][1]]);
            glVertex3fv(vertices[faces[i][2]]);
        }
        glEnd();
        glFinish();

        // Read and convert pixels
        glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, fb);
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                uint32_t p = fb[(h - 1 - y) * w + x];
                uint8_t r = p & 0xFF;
                uint8_t g = (p >> 8) & 0xFF;
                uint8_t b = (p >> 16) & 0xFF;
                tmp[y * w + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
            }
        }
        port_direct_draw_bitmap_to_lcd(0, 0, w, h, (uint8_t *)tmp);

        usleep(16000);
    }

    free(fb);
    free(tmp);
    return NULL;
}

__attribute__((constructor(1001))) static void opengl_demo(void)
{
    load_obj("example/face.obj");
    normalize_vertices();
    compute_normals();

    pthread_t t;
    pthread_create(&t, NULL, render_thread, NULL);
    printf("OpenGL render thread started\n");
}
