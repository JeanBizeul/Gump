#pragma once

#include <string>
#include <memory>
#include <vector>
#include <glm/vec2.hpp>
#include <glad/glad.h>

#include "Shader.hpp"

namespace Gump {

// Parameter types for effects
struct EffectParameter {
    enum class Type {
        Float,
        Int,
        Bool,
        Color
    };

    std::string name;
    Type type;
    float floatValue = 0.0f;
    int intValue = 0;
    bool boolValue = false;
    float colorValue[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    // Min/max for sliders
    float minValue = 0.0f;
    float maxValue = 1.0f;

    EffectParameter(const std::string& n, Type t) : name(n), type(t) {}
};

// Base class for all effects
class Effect {
public:
    Effect(const std::string& name, const std::string& shaderPath);
    virtual ~Effect() = default;

    // Get effect name
    const std::string& getName() const { return _name; }

    // Get parameters for UI
    std::vector<EffectParameter>& getParameters() { return _parameters; }
    const std::vector<EffectParameter>& getParameters() const { return _parameters; }

    // Apply effect to a texture region (for preview)
    void preview(GLuint sourceTexture, int texWidth, int texHeight,
                 int regionX, int regionY, int regionWidth, int regionHeight,
                 GLuint previewFBO);

    // Apply effect permanently to a texture region
    void apply(GLuint targetTexture, int texWidth, int texHeight,
               int regionX, int regionY, int regionWidth, int regionHeight);

protected:
    // Override this to add parameters
    virtual void setupParameters() = 0;

    // Override this to set shader uniforms
    virtual void setShaderUniforms() = 0;

    std::string _name;
    std::unique_ptr<OpenGLUtils::Shader> _shader;
    std::vector<EffectParameter> _parameters;

private:
    void renderEffect(GLuint targetTexture, GLuint sourceTexture,
                     int texWidth, int texHeight,
                     int regionX, int regionY, int regionWidth, int regionHeight);

    // Full-screen quad for rendering
    GLuint _quadVAO = 0;
    GLuint _quadVBO = 0;
    void setupQuad();
};

} // namespace Gump
