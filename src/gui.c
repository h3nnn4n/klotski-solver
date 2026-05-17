#include "gui.h"

GLFWwindow          *gui_window = NULL;
struct ImGuiContext *gui_ctx    = NULL;
struct ImGuiIO      *gui_io     = NULL;

void gui_init(GLFWwindow *win) {
    gui_window = win;

    gui_ctx = igCreateContext(NULL);
    igSetCurrentContext(gui_ctx);

    const char *glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    igStyleColorsDark(NULL);
}

void gui_terminate(void) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(gui_ctx);
    gui_ctx = NULL;
}

void gui_render(Sample *s) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    {
        ImVec2 sz = {300, 200};
        igSetNextWindowSize(sz, ImGuiCond_FirstUseEver);
        if (igBegin("Demo", NULL, 0)) {
            igText("Counter: %d", s->counter);

            if (igButton("Reset", (ImVec2){80, 20})) {
                sample_reset(s);
            }

            igSeparator();
            igColorEdit3("Color", s->color, 0);
        }
        igEnd();
    }

    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}
