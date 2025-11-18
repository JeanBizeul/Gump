#include <cstddef>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace OpenGLUtils {
/**
 * This class abstracts the glfw window & thus opengl context creation.
 */
class Window {
 public:
    Window(size_t width, size_t height, std::string name);
    ~Window();

    bool shouldClose() const;
    void pollEvents();
    void beginFrame();
    void beginImGuiFrame();
    void endFrame();

    GLFWwindow *getHandle() const;

 private:
   /** I use a raw GLFWwindow* pointer here instead of std::unique_ptr
   *  because GLFWwindow is an opaque type and cannot be used with std::unique_ptr
   *  with a custom deleter on MSVC reliably. Lifetime is fully managed by this class. */
    GLFWwindow *_windowHandle = nullptr;
    size_t _width;
    size_t _height;
    const std::string _name;
};
}