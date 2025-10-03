#pragma once
#include <string>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"

namespace OpenGLUtils
{
class Shader
{
    public:
        Shader(const std::string &vertexPath,
            const std::string &fragmentPath);
        ~Shader();

        void use();

        void set(const std::string &name, bool value) const;
        void set(const std::string &name, int value) const;
        void set(const std::string &name, float value) const;
        void set(const std::string &name, glm::vec2 value) const;
        void set(const std::string &name, glm::vec3 value) const;
        void set(const std::string &name, glm::vec4 value) const;
        void set(const std::string &name, glm::mat2 value) const;
        void set(const std::string &name, glm::mat3 value) const;
        void set(const std::string &name, glm::mat4 value) const;
    private:
        unsigned int _shaderProgram;
};
}
