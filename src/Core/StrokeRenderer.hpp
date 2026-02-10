#pragma once

#include <memory>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "Stroke.hpp"
#include "Shader.hpp"

namespace Gump {

class StrokeRenderer {
public:
    StrokeRenderer();
    ~StrokeRenderer();

    // Render a stroke to the screen (for preview)
    void renderStroke(const Stroke& stroke, const glm::mat4& projectionView, float cameraZoom);

    // Render a stroke directly to a framebuffer/texture (for finalizing)
    void renderStrokeToTexture(const Stroke& stroke, GLuint targetTexture, int textureWidth, int textureHeight, 
                                int layerX, int layerY, int layerWidth, int layerHeight);

    // Load the brush texture
    bool loadBrushTexture(const std::string& path);

private:
    void setupGL();
    void generateStrokeGeometry(const Stroke& stroke);

    std::unique_ptr<OpenGLUtils::Shader> _strokeShader;
    unsigned int _brushTextureID = 0;

    // GPU buffers for stroke points
    unsigned int _vao = 0;
    unsigned int _vbo = 0;
    size_t _pointCount = 0;
};

} // namespace Gump