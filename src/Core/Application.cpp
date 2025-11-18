#include "Application.hpp"

#include <vector>
#include <chrono>

#include <glm/glm.hpp>

#include "Logger.hpp"
#include "Vertex.hpp"

#include "UI/UI.hpp"

const size_t WindowWidth = 1680;
const size_t WindowHeight = 980;
const std::string_view WindowName = "Gump";
const std::string TexturesFolderPath = "assets/textures/";

Gump::Application::Application()
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
        LOG_DEBUG("Initializing input ...");
        Input::initialize(_window->getHandle());
    } catch (std::exception e) {
        LOG_ERROR("Could not create window: {}", e.what());
        throw std::runtime_error("Could not create window");
    }
}

void Gump::Application::run()
{
    while (_running) {
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
}

void Gump::Application::render()
{
    glm::mat4 pv = _camera->getPVMatrix();

    _shader->use();
    _shader->set("uProjectionView", pv);

    for (const auto& layer : _layers) {
        _textureAtlas->bindPage(layer->texturePageIndex);
        _shader->set("uTexture", 0);
        _shader->set("uTransparency", layer->transparency);
        layer->draw();
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
