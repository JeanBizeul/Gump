#include <memory>

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Camera2D.hpp"

namespace Gump {
class Canva {
 public:
    Canva();
    ~Canva() = default;

    void draw() const;

    void manageInputs();
 private:
    std::unique_ptr<OpenGLUtils::Mesh> _mesh;
    std::unique_ptr<OpenGLUtils::Shader> _shader;
    std::unique_ptr<Camera2D> _camera;
};
}
