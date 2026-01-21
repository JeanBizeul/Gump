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

struct PendingImport {
    bool isPending = false;
    std::string layerName;
    size_t width;
    size_t height;
    glm::vec2 uvMin;
    glm::vec2 uvMax;
    size_t texturePageIndex;
    glm::uvec2 suggestedCanvasSize;
};

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

    glm::uvec2 getWindowSize() const;
    
    // Canvas management
    glm::uvec2 getCanvasSize() const;
    void setCanvasSize(glm::uvec2 size);
    
    // Pending import
    PendingImport& getPendingImport();

 private:
    bool _running = true;
    glm::uvec2 _windowSize;
    glm::uvec2 _canvasSize;
    PendingImport _pendingImport;

    std::unique_ptr<OpenGLUtils::Window> _window;
    std::unique_ptr<OpenGLUtils::TextureAtlas> _textureAtlas;
    std::vector<std::unique_ptr<Layer>> _layers;
    std::unique_ptr<OpenGLUtils::Shader> _shader;
    std::unique_ptr<OpenGLUtils::Shader> _checkerboardShader;
    std::unique_ptr<OpenGLUtils::Mesh> _checkerboardMesh;
    std::unique_ptr<Camera2D> _camera;

    void update();
    void render();
    void updateCheckerboardMesh();
};
}