#include <memory>

#include <cstddef>

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Camera2D.hpp"

namespace Gump {
class Layer {
 public:
    Layer(size_t width, size_t height);
    ~Layer() = default;

    void draw() const;

    void manageInputs();
 private:
    size_t _width;
    size_t _height;
    std::unique_ptr<OpenGLUtils::Mesh> _mesh;
    std::unique_ptr<OpenGLUtils::Shader> _shader;
    std::unique_ptr<Camera2D> _camera;
};
}
