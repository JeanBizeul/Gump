#include "Application.hpp"

#include <vector>
#include <chrono>

#include <glm/glm.hpp>

#include "Logger.hpp"
#include "Vertex.hpp"

#include "UI/UI.hpp"

#include "Tools/ToolsFunctions.hpp"

const size_t WindowWidth = 1680;
const size_t WindowHeight = 980;
const std::string_view WindowName = "Gump";
const std::string TexturesFolderPath = "assets/";

Gump::Application::Application()
    : _windowSize(WindowWidth, WindowHeight), _canvasSize(800, 600)
{
    try {
        LOG_DEBUG("Creating window ...");
        _window = std::make_unique<OpenGLUtils::Window>(WindowWidth, WindowHeight, std::string(WindowName));
        LOG_DEBUG("Creating camera ...");
        _camera = std::make_unique<Camera2D>();
        LOG_DEBUG("Loading assets ...");
        _textureAtlas = std::make_unique<OpenGLUtils::TextureAtlas>(TexturesFolderPath);
        _shader = std::make_unique<OpenGLUtils::Shader>(
            "shaders/canva.vert",
            "shaders/canva.frag"
        );
        _checkerboardShader = std::make_unique<OpenGLUtils::Shader>(
            "shaders/checkerboard.vert",
            "shaders/checkerboard.frag"
        );
        _selectionShader = std::make_unique<OpenGLUtils::Shader>(
            "shaders/selection.vert",
            "shaders/selection.frag"
        );
        LOG_DEBUG("Initializing input ...");
        Input::initialize(_window->getHandle());

        // Initialize checkerboard mesh
        updateCheckerboardMesh();
    } catch (const std::exception& e) {
        LOG_ERROR("Could not create window: {}", e.what());
        throw std::runtime_error("Could not create window");
    }
}

void Gump::Application::run()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    while (_running) {
        // Update time
        auto currentTime = std::chrono::high_resolution_clock::now();
        _time = std::chrono::duration<float>(currentTime - startTime).count();

        _window->pollEvents();
        update();

        _window->beginFrame();
        render();
        _window->beginImGuiFrame();
        Gump::renderUI(*this);
        _window->endFrame();

        Input::update();
        if (_window->shouldClose()) _running = false;
    }
}

void Gump::Application::stop()
{
    _running = false;
}

void Gump::Application::update()
{
    if ((Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE) ||
        Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_MIDDLE))) {
        _camera->move(Input::getMouseDelta());
    }

    if (Input::getMouseScrollDelta().y != 0.0f) {
        _camera->zoom(1.0f + Input::getMouseScrollDelta().y * 0.1f);
    }

    try {
        Tools::ActionFunction.at(_selectedTool)(*this);
    } catch (const std::out_of_range& e) {
        LOG_ERROR("No such tool registered: {}", _selectedTool);
    }
}

void Gump::Application::render()
{
    glm::mat4 pv = _camera->getPVMatrix();

    // Render checkerboard background first
    _checkerboardShader->use();
    _checkerboardShader->set("uProjectionView", pv);
    _checkerboardShader->set("uCanvasSize", glm::vec2(_canvasSize.x, _canvasSize.y));
    if (_checkerboardMesh) {
        _checkerboardMesh->bind();
        _checkerboardMesh->draw();
    }

    // Render layers on top (clipped to canvas bounds)
    _shader->use();
    _shader->set("uProjectionView", pv);
    _shader->set("uCanvasSize", glm::vec2(_canvasSize.x, _canvasSize.y));

    for (const auto& layer : _layers) {
        _textureAtlas->bindPage(layer->texturePageIndex);
        _shader->set("uTexture", 0);
        _shader->set("uTransparency", layer->transparency);
        layer->draw();
    }

    // Render selection overlay
    if (_selectionState.hasSelection && _selectionMesh) {
        _selectionShader->use();
        _selectionShader->set("uProjectionView", pv);
        _selectionShader->set("uTime", _time);

        // Pass selection size to shader
        glm::vec2 selectionSize = _selectionState.getSize();
        _selectionShader->set("uSelectionSize", selectionSize);
        _selectionShader->set("uBorderWidth", (1 / _camera->getZoom()) * 5.0f); // Border width scales with zoom

        _selectionMesh->bind();
        _selectionMesh->draw();
    }
}


// Layer management
size_t Gump::Application::getLayerCount() const
{
    return _layers.size();
}

bool Gump::Application::canLayerMoveUp(size_t index) const
{
    return index > 0 && _layers.size() > 1;
}

bool Gump::Application::canLayerMoveDown(size_t index) const
{
    return index + 1 < _layers.size();
}

void Gump::Application::moveLayerUp(size_t index)
{
    if (!canLayerMoveUp(index)) return;
    std::swap(_layers[index], _layers[index - 1]);
}

void Gump::Application::moveLayerDown(size_t index)
{
    if (!canLayerMoveDown(index)) return;
    std::swap(_layers[index], _layers[index + 1]);
}

Gump::Layer &Gump::Application::getLayer(size_t index) const
{
    return *_layers.at(index);
}

void Gump::Application::addLayer(const std::string &name, size_t width, size_t height,
    glm::vec2 uvMin, glm::vec2 uvMax, size_t textureID)
{
    _layers.push_back(std::make_unique<Layer>(name, width, height, uvMin, uvMax, textureID));
}

std::vector<std::unique_ptr<Gump::Layer>> &Gump::Application::getLayers()
{
    return _layers;
}

OpenGLUtils::Shader &Gump::Application::getShader()
{
    return *_shader;
}

OpenGLUtils::TextureAtlas &Gump::Application::getTextureAtlas()
{
    return *_textureAtlas;
}

glm::uvec2 Gump::Application::getWindowSize() const
{
    return _windowSize;
}

glm::uvec2 Gump::Application::getCanvasSize() const
{
    return _canvasSize;
}

void Gump::Application::setCanvasSize(glm::uvec2 size)
{
    _canvasSize = size;
    updateCheckerboardMesh();
}

Gump::PendingImport& Gump::Application::getPendingImport()
{
    return _pendingImport;
}

Gump::ResizeCanvasRequest& Gump::Application::getResizeCanvasRequest()
{
    return _resizeCanvasRequest;
}

Gump::SelectionState& Gump::Application::getSelectionState()
{
    return _selectionState;
}

Gump::Camera2D& Gump::Application::getCamera()
{
    return *_camera;
}

glm::vec2 Gump::Application::screenToWorld(const glm::vec2& screenPos) const
{
    // Get viewport dimensions
    GLint vp[4] = {0, 0, 0, 0};
    glGetIntegerv(GL_VIEWPORT, vp);
    float width = static_cast<float>(vp[2]);
    float height = static_cast<float>(vp[3]);

    // Convert screen coordinates to OpenGL coordinates
    // Screen Y is top-down, OpenGL Y is bottom-up
    glm::vec2 glPos;
    glPos.x = screenPos.x;
    glPos.y = height - screenPos.y;

    // Reverse the camera transformations (inverse of getViewMatrix):
    // 1. Subtract half screen to get centered coordinates
    glm::vec2 worldPos = glPos - glm::vec2(width * 0.5f, height * 0.5f);

    // 2. Reverse zoom (divide by zoom)
    worldPos /= _camera->getZoom();

    // 3. Add camera position (reverse the -position translation)
    worldPos += _camera->getPosition();

    return worldPos;
}

void Gump::Application::setSelectedTool(const std::string &tool)
{
    _selectedTool = tool;
}

const std::string& Gump::Application::getSelectedTool() const
{
    return _selectedTool;
}

void Gump::Application::updateCheckerboardMesh()
{
    const std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    const std::vector<OpenGLUtils::Vertex_t> vertices = {
        // Position                                    // UVs (unused for checkerboard)
        { { 0.0f,              0.0f,              0.0f }, { 0.0f, 0.0f } }, // top-left
        { { (float)_canvasSize.x, 0.0f,              0.0f }, { 1.0f, 0.0f } }, // top-right
        { { (float)_canvasSize.x, (float)_canvasSize.y, 0.0f }, { 1.0f, 1.0f } }, // bottom-right
        { { 0.0f,              (float)_canvasSize.y, 0.0f }, { 0.0f, 1.0f } }  // bottom-left
    };

    _checkerboardMesh = std::make_unique<OpenGLUtils::Mesh>(vertices, indices);
}

void Gump::Application::updateSelectionMesh()
{
    if (!_selectionState.hasSelection) {
        _selectionMesh.reset();
        return;
    }

    LOG_DEBUG("Updating selection mesh ...");
    LOG_DEBUG("From : x{}, y{}  to  x{}, y{}  with offset  x{}, y{}",
        _selectionState.getMin().x, _selectionState.getMin().y,
        _selectionState.getMax().x, _selectionState.getMax().y,
        _selectionState.offset.x, _selectionState.offset.y);
    glm::vec2 min = _selectionState.getMin() + _selectionState.offset;
    glm::vec2 max = _selectionState.getMax() + _selectionState.offset;

    const std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    const std::vector<OpenGLUtils::Vertex_t> vertices = {
        { { min.x, min.y, 0.0f }, { 0.0f, 0.0f } }, // top-left
        { { max.x, min.y, 0.0f }, { 1.0f, 0.0f } }, // top-right
        { { max.x, max.y, 0.0f }, { 1.0f, 1.0f } }, // bottom-right
        { { min.x, max.y, 0.0f }, { 0.0f, 1.0f } }  // bottom-left
    };

    _selectionMesh = std::make_unique<OpenGLUtils::Mesh>(vertices, indices);
}
