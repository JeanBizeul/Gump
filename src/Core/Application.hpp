#pragma once

#include <memory>

#include <imgui.h>

#include "Window.hpp"

#include "Mesh.hpp"
#include "Shader.hpp"
#include "TextureAtlas.hpp"

namespace Gump
{
class Application
{
 public:
    Application();
    ~Application() = default;

    void run();
    void stop();

 private:
    std::unique_ptr<OpenGLUtils::Window> _window;
    bool _running = true;

    void processInput();
    void update();
    void render();
};
}