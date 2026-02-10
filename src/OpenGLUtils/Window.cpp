#include "Window.hpp"

#include <stdexcept>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

OpenGLUtils::Window::Window(size_t width, size_t height, std::string name)
{
    // Setup glfw window
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _windowHandle = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!_windowHandle)
        throw std::runtime_error("Failed to create GLFW window");
    
    glfwMakeContextCurrent(_windowHandle);
    glfwSwapInterval(1); // vsync

    // Load all openGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");
    
    // Setup Dear ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(_windowHandle, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_DockingEnable
        | ImGuiConfigFlags_ViewportsEnable;
}

OpenGLUtils::Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (_windowHandle) glfwDestroyWindow(_windowHandle);
    glfwTerminate();
}

bool OpenGLUtils::Window::shouldClose() const
{
    return glfwWindowShouldClose(_windowHandle);
}

void OpenGLUtils::Window::pollEvents()
{
    glfwPollEvents();
}

void OpenGLUtils::Window::beginFrame()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Need to reset some stuff as imgui may have changed them
    glDisable(GL_DEPTH_TEST);             // 2D overlay
    glEnable(GL_BLEND);                   
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLUtils::Window::beginImGuiFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void OpenGLUtils::Window::endFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(_windowHandle);
}

GLFWwindow *OpenGLUtils::Window::getHandle() const
{
    return _windowHandle;
}
