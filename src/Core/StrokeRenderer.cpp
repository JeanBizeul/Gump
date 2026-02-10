#include "StrokeRenderer.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

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
    if (points.size() < 2) {
        _pointCount = points.size();

        if (_pointCount == 0) return;

        // Single point - just render it
        std::vector<float> vertexData;
        vertexData.push_back(points[0].position.x);
        vertexData.push_back(points[0].position.y);
        vertexData.push_back(points[0].pressure);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return;
    }

    // Generate Catmull-Rom spline control points
    // Each segment is defined by 4 control points: P0, P1, P2, P3
    // The curve goes from P1 to P2, using P0 and P3 for smoothness

    const auto& settings = stroke.getBrushSettings();
    float spacing = settings.size * settings.spacing;

    std::vector<float> vertexData;

    // Add first point
    vertexData.push_back(points[0].position.x);
    vertexData.push_back(points[0].position.y);
    vertexData.push_back(points[0].pressure);

    // Process each segment
    for (size_t i = 0; i < points.size() - 1; i++) {
        // Get 4 control points for Catmull-Rom
        glm::vec2 p0 = (i == 0) ? points[i].position : points[i - 1].position;
        glm::vec2 p1 = points[i].position;
        glm::vec2 p2 = points[i + 1].position;
        glm::vec2 p3 = (i + 2 < points.size()) ? points[i + 2].position : points[i + 1].position;

        float pressure1 = points[i].pressure;
        float pressure2 = points[i + 1].pressure;

        // Estimate curve length (rough approximation)
        float chordLength = glm::length(p2 - p1);
        float controlLength = glm::length(p1 - p0) + glm::length(p2 - p1) + glm::length(p3 - p2);
        float approxLength = (chordLength + controlLength) * 0.5f;

        // Calculate number of samples based on spacing
        int numSamples = std::max(1, static_cast<int>(approxLength / spacing));

        // Sample along the Catmull-Rom curve
        for (int j = 1; j <= numSamples; j++) {
            float t = static_cast<float>(j) / numSamples;

            // Catmull-Rom basis functions
            float t2 = t * t;
            float t3 = t2 * t;

            // Catmull-Rom spline matrix (with tau = 0.5 for uniform parameterization)
            glm::vec2 position =
                0.5f * ((2.0f * p1) +
                        (-p0 + p2) * t +
                        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);

            float pressure = glm::mix(pressure1, pressure2, t);

            vertexData.push_back(position.x);
            vertexData.push_back(position.y);
            vertexData.push_back(pressure);
        }
    }

    _pointCount = vertexData.size() / 3;

    // Upload to GPU
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void StrokeRenderer::renderStroke(const Stroke& stroke, const glm::mat4& projectionView, float cameraZoom)
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
    _strokeShader->set("uBrushSize", settings.size * cameraZoom); // Scale with zoom
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

void StrokeRenderer::renderStrokeToTexture(const Stroke& stroke, GLuint targetTexture, 
                                           int textureWidth, int textureHeight,
                                           int layerX, int layerY, int layerWidth, int layerHeight)
{
    if (stroke.isEmpty() || !_brushTextureID) return;

    // Generate geometry from stroke points
    generateStrokeGeometry(stroke);

    if (_pointCount == 0) return;

    const auto& settings = stroke.getBrushSettings();

    // Save the current viewport BEFORE changing anything
    GLint savedViewport[4];
    glGetIntegerv(GL_VIEWPORT, savedViewport);

    // Create framebuffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Attach the target texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTexture, 0);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("Framebuffer not complete for stroke rendering");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        return;
    }

    // Set viewport to the layer's region within the texture atlas
    glViewport(layerX, layerY, layerWidth, layerHeight);

    // Enable blending - use appropriate blend mode based on whether it's an eraser
    glEnable(GL_BLEND);
    if (settings.isEraser) {
        // Eraser: subtract alpha
        glBlendFuncSeparate(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        // Normal drawing: standard alpha blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Create orthographic projection for the layer (in layer-local coordinates)
    glm::mat4 projection = glm::ortho(0.0f, (float)layerWidth, (float)layerHeight, 0.0f, -1.0f, 1.0f);

    // Use stroke shader
    _strokeShader->use();
    _strokeShader->set("uProjectionView", projection);
    _strokeShader->set("uBrushColor", settings.color);
    _strokeShader->set("uBrushSize", settings.size); // No zoom scaling for texture rendering
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

    // Cleanup
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    // Restore the original viewport
    glViewport(savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3]);

    LOG_DEBUG("Rendered stroke to texture at ({}, {}) with size {}x{}", layerX, layerY, layerWidth, layerHeight);
}