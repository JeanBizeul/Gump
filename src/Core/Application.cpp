#include "Application.hpp"

#include <vector>
#include <chrono>

#include "Logger.hpp"
#include "Vertex.hpp"

#include "UI/UI.hpp"

const size_t WindowWidth = 1080;
const size_t WindowHeight = 720;
const std::string_view WindowName = "Gump";

Gump::Application::Application()
{
    try {
        LOG_DEBUG("Creating window ...");
        _window = std::make_unique<OpenGLUtils::Window>(WindowWidth, WindowHeight, std::string(WindowName));
        LOG_DEBUG("Loading assets ...");
    } catch (std::exception e) {
        LOG_ERROR("Could not create window: {}", e.what());
        throw std::runtime_error("Could not create window");
    }

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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
    Gump::renderUI(*this);
}
