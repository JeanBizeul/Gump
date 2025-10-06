#include <memory>

#include "OpenGLUtils/Window.hpp"

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

    void processInput();
    void update();
    void render();
};
}