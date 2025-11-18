#include "Input.hpp"
#include "imgui.h"

using namespace Gump;

// Static definitions
GLFWwindow* Input::_window = nullptr;
std::unordered_map<int, Input::KeyState> Input::_keys;
std::unordered_map<int, Input::KeyState> Input::_mouseButtons;

double Input::_mouseX = 0.0, Input::_mouseY = 0.0;
double Input::_prevMouseX = 0.0, Input::_prevMouseY = 0.0;
double Input::_mouseDeltaX = 0.0, Input::_mouseDeltaY = 0.0;
double Input::_mouseScrollDelta = 0.0;

GLFWkeyfun Input::prevKeyCallback = nullptr;
GLFWmousebuttonfun Input::prevMouseButtonCallback = nullptr;
GLFWcursorposfun Input::prevCursorPosCallback = nullptr;
GLFWscrollfun Input::prevScrollCallback = nullptr;

void Input::initialize(GLFWwindow* window)
{
    _window = window;

    // Store previous callbacks (installed by ImGui)
    prevKeyCallback = glfwSetKeyCallback(window, keyCallback);
    prevMouseButtonCallback = glfwSetMouseButtonCallback(window, mouseButtonCallback);
    prevCursorPosCallback = glfwSetCursorPosCallback(window, cursorPosCallback);
    prevScrollCallback = glfwSetScrollCallback(window, scrollCallback);
}

void Input::update()
{
    for (auto& [k, s] : _keys) {
        if (s == KeyState::Pressed) s = KeyState::Held;
        else if (s == KeyState::Released) s = KeyState::None;
    }

    for (auto& [b, s] : _mouseButtons) {
        if (s == KeyState::Pressed) s = KeyState::Held;
        else if (s == KeyState::Released) s = KeyState::None;
    }
}

bool Input::isKeyPressed(int key)
{
    return _keys[key] == KeyState::Pressed;
}

bool Input::isKeyReleased(int key)
{
    return _keys[key] == KeyState::Released;
}

bool Input::isKeyHeld(int key)
{
    return _keys[key] == KeyState::Held;
}

bool Input::isMouseButtonPressed(int button)
{
    return _mouseButtons[button] == KeyState::Pressed;
}

bool Input::isMouseButtonReleased(int button)
{
    return _mouseButtons[button] == KeyState::Released;
}

bool Input::isMouseButtonHeld(int button)
{
    return _mouseButtons[button] == KeyState::Held;
}

glm::vec2 Input::getMousePosition()
{
    return { static_cast<float>(_mouseX), static_cast<float>(_mouseY) };
}

glm::vec2 Input::getMouseDelta()
{
    return { static_cast<float>(_mouseDeltaX), static_cast<float>(_mouseDeltaY) };
}

double Input::getMouseScrollDelta()
{
    return _mouseScrollDelta;
}

// ------------------------
// CALLBACKS
// ------------------------

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureKeyboard)
    {
        if (action == GLFW_PRESS) _keys[key] = KeyState::Pressed;
        else if (action == GLFW_RELEASE) _keys[key] = KeyState::Released;
    }

    // Forward to ImGui's callback
    if (prevKeyCallback) prevKeyCallback(window, key, scancode, action, mods);
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {
        if (action == GLFW_PRESS) _mouseButtons[button] = KeyState::Pressed;
        else if (action == GLFW_RELEASE) _mouseButtons[button] = KeyState::Released;
    }

    // Forward to ImGui
    if (prevMouseButtonCallback)
        prevMouseButtonCallback(window, button, action, mods);
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {
        _mouseDeltaX = -(xpos - _prevMouseX);
        _mouseDeltaY = ypos - _prevMouseY;

        _prevMouseX = xpos;
        _prevMouseY = ypos;

        _mouseX = xpos;
        _mouseY = ypos;
    }

    // Forward to ImGui
    if (prevCursorPosCallback)
        prevCursorPosCallback(window, xpos, ypos);
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {
        _mouseScrollDelta = yoffset;
    }

    // Forward to ImGui
    if (prevScrollCallback)
        prevScrollCallback(window, xoffset, yoffset);
}
