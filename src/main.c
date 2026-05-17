#include <stdio.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gui.h"
#include "sample.h"
#include "shader_c.h"

static void error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main(void) {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "C Template", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load OpenGL functions\n");
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, 1280, 720);

    gui_init(window);

    Sample   sample;
    uint64_t seed = (uint64_t)time(NULL);
    sample_init(&sample, seed, 1u);

    Shader *shader = newShader("shaders/main.vert", "shaders/main.frag", NULL);
    if (!shader) {
        fprintf(stderr, "Failed to load shader\n");
        gui_terminate();
        glfwTerminate();
        return 1;
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        sample_tick(&sample);

        glClearColor(sample.color[0], sample.color[1], sample.color[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Shader_use(shader);
        Shader_setVec3f(shader, "u_color", sample.color[0], sample.color[1], sample.color[2]);

        gui_render(&sample);

        glfwSwapBuffers(window);
    }

    Shader_destroy(shader);
    gui_terminate();
    glfwTerminate();

    return 0;
}
