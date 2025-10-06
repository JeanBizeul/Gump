#include <memory>

#include <imgui.h>

#include "Window.hpp"

#include "Mesh.hpp"
#include "Shader.hpp"

namespace Gump
{
class Application
{
 public:
    Application();
    ~Application() = default;

    void run();
    void stop();

 private:
    std::unique_ptr<OpenGLUtils::Window> _window;
    bool _running = true;

    std::unique_ptr<OpenGLUtils::Mesh> _demoCube;
    std::unique_ptr<OpenGLUtils::Mesh> _demoLight;
    std::unique_ptr<OpenGLUtils::Shader> _demoCubeShader;
    std::unique_ptr<OpenGLUtils::Shader> _demoLightShader;

    void processInput();
    void update();
    void render();
};
}