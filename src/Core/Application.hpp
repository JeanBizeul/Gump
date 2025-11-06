#pragma once

#include <memory>

#include <imgui.h>

#include "Window.hpp"

#include "Canva.hpp"

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

    OpenGLUtils::TextureAtlas &getTextureAtlas();

 private:
    std::unique_ptr<OpenGLUtils::Window> _window;
    bool _running = true;

    std::unique_ptr<OpenGLUtils::TextureAtlas> _textureAtlas;
    std::unique_ptr<Canva> _canva;

    void processInput();
    void update();
    void render();
};
}