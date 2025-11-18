#pragma once

#include <memory>

#include <imgui.h>

#include "Window.hpp"

#include "Layer.hpp"

#include "Mesh.hpp"
#include "Shader.hpp"
#include "TextureAtlas.hpp"
#include "Camera2D.hpp"
#include "Input.hpp"

namespace Gump
{
class Application
{
 public:
    Application();
    ~Application() = default;

    void run();
    void stop();

    bool canLayerMoveUp(size_t index) const;
    bool canLayerMoveDown(size_t index) const;
    void moveLayerUp(size_t index);
    void moveLayerDown(size_t index);
    void addLayer(const std::string &name, size_t width, size_t height,
      glm::vec2 uvMin, glm::vec2 uvMax, size_t textureID);
    size_t getLayerCount() const;
    Layer &getLayer(size_t index) const;
    std::vector<std::unique_ptr<Layer>> &getLayers();

    OpenGLUtils::Shader &getShader();
    OpenGLUtils::TextureAtlas &getTextureAtlas();


 private:
    bool _running = true;
 
    std::unique_ptr<OpenGLUtils::Window> _window;
    std::unique_ptr<OpenGLUtils::TextureAtlas> _textureAtlas;
    std::vector<std::unique_ptr<Layer>> _layers;
    std::unique_ptr<OpenGLUtils::Shader> _shader;
    std::unique_ptr<Camera2D> _camera;

    void update();
    void render();
};
}