#include <stdio.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "board.h"
#include "cli.h"
#include "gui.h"

static void error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static int run_gui(void) {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(2560, 1440, "C Template", NULL, NULL);
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

    glViewport(0, 0, 2560, 1440);

    gui_init(window);

    board_t *board = build_board();
    reset_board_to_classic(board);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        gui_render(board);

        glfwSwapBuffers(window);
    }

    destroy_board(board);
    gui_terminate();
    glfwTerminate();

    return 0;
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--gui") == 0)
        return run_gui();

    return cli_run(argc, argv);
}
