#include "Application.hpp"

#include "Logger/Logger.hpp"

const size_t WindowWidth = 720;
const size_t WindowHeight = 480;
const std::string_view WindowName = "Gump";

Gump::Application::Application()
{
    try {
        LOG_DEBUG("Creating window ...");
        _window = std::make_unique<OpenGLUtils::Window>(WindowWidth, WindowHeight, std::string(WindowName));
        LOG_DEBUG("Window created succesfully");
    } catch (std::exception e) {
        LOG_ERROR("Could not create window: {}", e.what());
        throw std::runtime_error("Could not create window");
    }
}

void Gump::Application::run()
{
    while (_running) {
        processInput();
        update();

        _window->beginFrame();
        render();
        _window->endFrame();

        if (_window->shouldClose()) _running = false;
    }
}

void Gump::Application::stop()
{
    _running = false;
}

void Gump::Application::processInput()
{

}

void Gump::Application::update()
{
    
}

void Gump::Application::render()
{
    
}
