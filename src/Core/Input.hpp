#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <glm/vec2.hpp>

namespace Gump {

class Input {
public:
    enum class KeyState {
        None,
        Pressed,
        Released,
        Held
    };

    static void initialize(GLFWwindow* window);
    static void update();

    static bool isKeyPressed(int key);
    static bool isKeyReleased(int key);
    static bool isKeyHeld(int key);

    static glm::vec2 getMousePosition();
    static glm::vec2 getMouseDelta();
    static glm::vec2 getMouseScrollDelta();

    static bool isMouseButtonPressed(int button);
    static bool isMouseButtonReleased(int button);
    static bool isMouseButtonHeld(int button);

private:
    static GLFWwindow* _window;
    static std::unordered_map<int, KeyState> _keys;
    static std::unordered_map<int, KeyState> _mouseButtons;

    static double _mouseX, _mouseY;
    static double _prevMouseX, _prevMouseY;
    static double _mouseDeltaX, _mouseDeltaY;
    static glm::vec2 _mouseScrollDelta;

    static GLFWkeyfun prevKeyCallback;
    static GLFWmousebuttonfun prevMouseButtonCallback;
    static GLFWcursorposfun prevCursorPosCallback;
    static GLFWscrollfun prevScrollCallback;

    static void keyCallback(GLFWwindow*, int, int, int, int);
    static void mouseButtonCallback(GLFWwindow*, int, int, int);
    static void cursorPosCallback(GLFWwindow*, double, double);
    static void scrollCallback(GLFWwindow*, double, double);
};

}
