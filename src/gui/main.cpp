/**
 * @file main.cpp
 * @brief Entry point for the Dijkstra graphical interactive application.
 */

#include "app.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include <iostream>

#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#endif

namespace {

void setup_imgui_dark_style() {
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    style.WindowRounding = 6.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;

    colors[ImGuiCol_Text]                  = ImVec4(0.92f, 0.94f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.55f, 0.65f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.11f, 0.13f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.14f, 0.16f, 0.21f, 1.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.13f, 0.15f, 0.20f, 0.98f);
    colors[ImGuiCol_Border]                = ImVec4(0.24f, 0.28f, 0.36f, 0.60f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.18f, 0.21f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.24f, 0.28f, 0.38f, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.28f, 0.34f, 0.46f, 1.00f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.10f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.14f, 0.18f, 0.24f, 1.00f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.16f, 0.22f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.35f, 0.75f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.35f, 0.70f, 0.95f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.45f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.20f, 0.25f, 0.35f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.28f, 0.36f, 0.50f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.35f, 0.45f, 0.62f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.22f, 0.28f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.30f, 0.38f, 0.52f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.36f, 0.46f, 0.62f, 1.00f);
}

} // namespace

int main(int, char **) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    // GL 3.3 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(1280, 800, "Dijkstra Algorithm Interactive Visualizer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    setup_imgui_dark_style();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    dijkstra::gui::App app;

    double last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double current_time = glfwGetTime();
        float delta_time = static_cast<float>(current_time - last_time);
        last_time = current_time;

        app.update(delta_time);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        app.render_ui();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.10f, 0.11f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
