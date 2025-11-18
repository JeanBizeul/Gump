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

    size_t getLayerCount() const;
    bool canLayerMoveUp(size_t index) const;
    bool canLayerMoveDown(size_t index) const;
    void moveLayerUp(size_t index);
    void moveLayerDown(size_t index);
    Layer &getLayer(size_t index) const;
    void addLayer(std::string name, size_t width, size_t height);

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