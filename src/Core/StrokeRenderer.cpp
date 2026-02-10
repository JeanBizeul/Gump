#include "StrokeRenderer.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"

#include <glad/glad.h>

#include <stb_image.h>

using namespace Gump;

StrokeRenderer::StrokeRenderer()
{
    setupGL();

    // Load the stroke shader
    try {
        _strokeShader = std::make_unique<OpenGLUtils::Shader>(
            "shaders/stroke.vert",
            "shaders/stroke.frag"
        );
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load stroke shader: {}", e.what());
        throw;
    }

    // Load the default brush texture
    if (!loadBrushTexture("assets/brushes/round-brush.png")) {
        LOG_WARNING("Failed to load default brush texture");
    }
}

StrokeRenderer::~StrokeRenderer()
{
    if (_vao) glDeleteVertexArrays(1, &_vao);
    if (_vbo) glDeleteBuffers(1, &_vbo);
    if (_brushTextureID) glDeleteTextures(1, &_brushTextureID);
}

void StrokeRenderer::setupGL()
{
    // Create VAO and VBO for stroke points
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    // Position attribute (we'll upload stroke points here)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    // Pressure attribute
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

bool StrokeRenderer::loadBrushTexture(const std::string& path)
{
    // Delete old texture if exists
    if (_brushTextureID) {
        glDeleteTextures(1, &_brushTextureID);
        _brushTextureID = 0;
    }

    // Load image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data) {
        LOG_ERROR("Failed to load brush texture: {}", path);
        return false;
    }

    // Create OpenGL texture
    glGenTextures(1, &_brushTextureID);
    glBindTexture(GL_TEXTURE_2D, _brushTextureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    LOG_INFO("Loaded brush texture: {} ({}x{}, {} channels)", path, width, height, channels);
    return true;
}

void StrokeRenderer::generateStrokeGeometry(const Stroke& stroke)
{
    const auto& points = stroke.getPoints();
    _pointCount = points.size();

    if (_pointCount == 0) return;

    // Prepare vertex data: [x, y, pressure] for each point
    std::vector<float> vertexData;
    vertexData.reserve(_pointCount * 3);

    for (const auto& point : points) {
        vertexData.push_back(point.position.x);
        vertexData.push_back(point.position.y);
        vertexData.push_back(point.pressure);
    }

    // Upload to GPU
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void StrokeRenderer::renderStroke(const Stroke& stroke, const glm::mat4& projectionView)
{
    if (stroke.isEmpty() || !_brushTextureID) return;

    // Generate geometry from stroke points
    generateStrokeGeometry(stroke);

    if (_pointCount == 0) return;

    const auto& settings = stroke.getBrushSettings();

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use stroke shader
    _strokeShader->use();
    _strokeShader->set("uProjectionView", projectionView);
    _strokeShader->set("uBrushColor", settings.color);
    _strokeShader->set("uBrushSize", settings.size);
    _strokeShader->set("uBrushHardness", settings.hardness);
    _strokeShader->set("uBrushOpacity", settings.opacity);
    _strokeShader->set("uBrushSpacing", settings.spacing);
    _strokeShader->set("uBrushTexture", 0);

    // Bind brush texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _brushTextureID);

    // Draw points
    glBindVertexArray(_vao);
    glDrawArrays(GL_POINTS, 0, _pointCount);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}