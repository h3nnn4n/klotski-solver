#ifndef SRC_GUI_H_
#define SRC_GUI_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

#include "sample.h"

extern GLFWwindow          *gui_window;
extern struct ImGuiContext *gui_ctx;
extern struct ImGuiIO      *gui_io;

int  ImGui_ImplOpenGL3_Init(const char *glsl_version);
int  ImGui_ImplGlfw_InitForOpenGL(GLFWwindow *window, bool install_callbacks);
void ImGui_ImplGlfw_Shutdown(void);
void ImGui_ImplOpenGL3_Shutdown(void);
void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData *draw_data);
void ImGui_ImplOpenGL3_NewFrame(void);
void ImGui_ImplGlfw_NewFrame(void);

void gui_init(GLFWwindow *win);
void gui_terminate(void);
void gui_render(Sample *s);

#endif
