/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Linux EGL + OpenGL ES 2.0 triangle demo.
 *
 * Rendering uses an EGL Pbuffer surface. The resulting pixels are copied to
 * the existing SDL LCD simulator.
 */

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fb_sdl.h"

static const char vertex_shader_source[] =
    "attribute vec2 a_position;\n"
    "attribute vec3 a_color;\n"
    "varying vec3 v_color;\n"
    "void main(void)\n"
    "{\n"
    "    v_color = a_color;\n"
    "    gl_Position = vec4(a_position, 0.0, 1.0);\n"
    "}\n";

static const char fragment_shader_source[] =
    "precision mediump float;\n"
    "varying vec3 v_color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = vec4(v_color, 1.0);\n"
    "}\n";

static void print_egl_error(const char *operation)
{
    fprintf(stderr, "%s failed: EGL error 0x%04x\n",
            operation, (unsigned int)eglGetError());
}

static GLuint compile_shader(GLenum type, const char *source)
{
    GLuint shader;
    GLint compiled;
    GLchar log[512];

    shader = glCreateShader(type);
    if (shader == 0)
    {
        return 0;
    }

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE)
    {
        return shader;
    }

    glGetShaderInfoLog(shader, sizeof(log), NULL, log);
    fprintf(stderr, "GLES2 shader compilation failed: %s\n", log);
    glDeleteShader(shader);
    return 0;
}

static GLuint create_program(void)
{
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
    GLint linked;
    GLchar log[512];

    vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    if (vertex_shader == 0 || fragment_shader == 0)
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }

    program = glCreateProgram();
    if (program == 0)
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glBindAttribLocation(program, 0, "a_position");
    glBindAttribLocation(program, 1, "a_color");
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE)
    {
        return program;
    }

    glGetProgramInfoLog(program, sizeof(log), NULL, log);
    fprintf(stderr, "GLES2 program link failed: %s\n", log);
    glDeleteProgram(program);
    return 0;
}

static int initialize_egl(EGLDisplay *display, EGLConfig *config,
                          EGLSurface *surface, EGLContext *context,
                          EGLint width, EGLint height)
{
    EGLint config_count;
    EGLint major;
    EGLint minor;
    const EGLint config_attributes[] =
    {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };
    const EGLint context_attributes[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    const EGLint surface_attributes[] =
    {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_NONE
    };

    *display = EGL_NO_DISPLAY;
    *config = (EGLConfig)0;
    *surface = EGL_NO_SURFACE;
    *context = EGL_NO_CONTEXT;

    *display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (*display == EGL_NO_DISPLAY)
    {
        print_egl_error("eglGetDisplay");
        return -1;
    }

    if (eglInitialize(*display, &major, &minor) != EGL_TRUE)
    {
        print_egl_error("eglInitialize");
        return -1;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE)
    {
        print_egl_error("eglBindAPI");
        return -1;
    }

    if (eglChooseConfig(*display, config_attributes, config, 1,
                        &config_count) != EGL_TRUE || config_count != 1)
    {
        print_egl_error("eglChooseConfig");
        return -1;
    }

    *surface = eglCreatePbufferSurface(*display, *config, surface_attributes);
    if (*surface == EGL_NO_SURFACE)
    {
        print_egl_error("eglCreatePbufferSurface");
        return -1;
    }

    *context = eglCreateContext(*display, *config, EGL_NO_CONTEXT,
                                context_attributes);
    if (*context == EGL_NO_CONTEXT)
    {
        print_egl_error("eglCreateContext");
        return -1;
    }

    if (eglMakeCurrent(*display, *surface, *surface, *context) != EGL_TRUE)
    {
        print_egl_error("eglMakeCurrent");
        return -1;
    }

    fprintf(stderr, "EGL %d.%d initialized with an OpenGL ES 2.0 context\n",
            major, minor);
    return 0;
}

static void release_egl(EGLDisplay display, EGLSurface surface, EGLContext context)
{
    if (display == EGL_NO_DISPLAY)
    {
        return;
    }

    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);
    if (context != EGL_NO_CONTEXT)
    {
        eglDestroyContext(display, context);
    }
    if (surface != EGL_NO_SURFACE)
    {
        eglDestroySurface(display, surface);
    }
    eglTerminate(display);
    eglReleaseThread();
}

static void draw_triangle(GLuint program, EGLint width, EGLint height)
{
    static const GLfloat vertices[] =
    {
        0.0f,  0.72f, 1.0f, 0.25f, 0.20f,
        -0.72f, -0.60f, 0.20f, 1.0f, 0.35f,
        0.72f, -0.60f, 0.25f, 0.45f, 1.0f
    };

    glViewport(0, 0, width, height);
    glClearColor(0.08f, 0.11f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          vertices);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          vertices + 2);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

static void copy_to_lcd(const uint8_t *rgba, uint32_t *argb, int width, int height)
{
    int x;
    int y;

    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            const uint8_t *source = rgba + (((height - 1 - y) * width + x) * 4);
            argb[y * width + x] = ((uint32_t)source[3] << 24) |
                                  ((uint32_t)source[0] << 16) |
                                  ((uint32_t)source[1] << 8) |
                                  (uint32_t)source[2];
        }
    }

    port_direct_draw_bitmap_to_lcd(0, 0, width, height,
                                   (const uint8_t *)argb);
}

static void *render_thread(void *arg)
{
    EGLDisplay display;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;
    GLuint program;
    int width;
    int height;
    size_t pixel_count;
    uint8_t *rgba;
    uint32_t *argb;

    (void)arg;
    width = sim_get_width();
    height = sim_get_height();
    program = 0;
    pixel_count = (size_t)width * (size_t)height;
    rgba = NULL;
    argb = NULL;

    if (initialize_egl(&display, &config, &surface, &context,
                       width, height) != 0)
    {
        release_egl(display, surface, context);
        return NULL;
    }

    program = create_program();
    if (program == 0)
    {
        release_egl(display, surface, context);
        return NULL;
    }

    rgba = malloc(pixel_count * 4);
    argb = malloc(pixel_count * sizeof(*argb));
    if (rgba == NULL || argb == NULL)
    {
        fprintf(stderr, "Unable to allocate the EGL readback buffers\n");
        free(rgba);
        free(argb);
        glDeleteProgram(program);
        release_egl(display, surface, context);
        return NULL;
    }

    glEnable(GL_DEPTH_TEST);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    while (1)
    {
        draw_triangle(program, width, height);
        glFinish();
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
        copy_to_lcd(rgba, argb, width, height);
        usleep(16000);
    }

    free(rgba);
    free(argb);
    glDeleteProgram(program);
    release_egl(display, surface, context);
    return NULL;
}

__attribute__((constructor(1001))) static void opengl_demo(void)
{
    pthread_t thread;

    if (pthread_create(&thread, NULL, render_thread, NULL) != 0)
    {
        fprintf(stderr, "Unable to start the EGL render thread\n");
        return;
    }

    pthread_detach(thread);
}
