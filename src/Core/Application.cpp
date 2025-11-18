#include "Application.hpp"

#include <vector>
#include <chrono>

#include <glm/glm.hpp>

#include "Logger.hpp"
#include "Vertex.hpp"

#include "UI/UI.hpp"

const size_t WindowWidth = 1080;
const size_t WindowHeight = 720;
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
    } catch (std::exception e) {
        LOG_ERROR("Could not create window: {}", e.what());
        throw std::runtime_error("Could not create window");
    }
}

void Gump::Application::run()
{
    while (_running) {
        _window->pollEvents();
        processInput();
        update();

        _window->beginFrame();
        render();
        _window->beginImGuiFrame();
        Gump::renderUI(*this);
        _window->endFrame();

        if (_window->shouldClose()) _running = false;
    }
}

void Gump::Application::stop()
{
    _running = false;
}

OpenGLUtils::TextureAtlas &Gump::Application::getTextureAtlas()
{
    return *_textureAtlas;
}

void Gump::Application::processInput()
{

}

void Gump::Application::update()
{
}

void Gump::Application::render()
{
    glm::mat4 pv = _camera->getPVMatrix();

    _shader->use();
    _shader->set("uProjectionView", pv);

    for (const auto& layer : _layers)
        layer->draw();
}
