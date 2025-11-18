#pragma once

#include <memory>

#include <imgui.h>

#include "Window.hpp"

#include "Layer.hpp"

#include "Mesh.hpp"
#include "Shader.hpp"
#include "TextureAtlas.hpp"
#include "Camera2D.hpp"

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
    std::vector<std::unique_ptr<Layer>> _layers;

   std::unique_ptr<OpenGLUtils::Shader> _shader;
   std::unique_ptr<Camera2D> _camera;

    void processInput();
    void update();
    void render();
};
}