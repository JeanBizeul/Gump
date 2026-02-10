#include "Stroke.hpp"
#include <glad/glad.h>
#include <chrono>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Logger.hpp"

using namespace Gump;

BrushStroke::BrushStroke(const BrushSettings& settings)
    : _settings(settings), _strokeFBO(0), _strokeTexture(0),
      _lastDabPosition(-1000.0f, -1000.0f), _currentTime(0.0f)
{
    // Initialize GPU resources with a reasonable default size
    initGPUResources(glm::uvec2(2048, 2048));

    // Load brush shader
    _brushShader = std::make_unique<OpenGLUtils::Shader>(
        "shaders/brush.vs",
        "shaders/brush.fs"
    );

    // Create a simple quad mesh for brush dabs
    const std::vector<OpenGLUtils::Vertex_t> vertices = {
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
        { {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f } },
        { { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f } }
    };

    const std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    _quadMesh = std::make_unique<OpenGLUtils::Mesh>(vertices, indices);
}

BrushStroke::~BrushStroke()
{
    if (_strokeFBO) {
        glDeleteFramebuffers(1, &_strokeFBO);
    }
    if (_strokeTexture) {
        glDeleteTextures(1, &_strokeTexture);
    }
}

void BrushStroke::initGPUResources(glm::uvec2 size)
{
    // Create FBO for stroke rendering
    glGenFramebuffers(1, &_strokeFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _strokeFBO);

    // Create texture to render stroke into
    glGenTextures(1, &_strokeTexture);
    glBindTexture(GL_TEXTURE_2D, _strokeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _strokeTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("Stroke FBO is incomplete!");
    }

    // Clear the texture to transparent
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BrushStroke::addPoint(const glm::vec2& position, float pressure)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    float timestamp = duration.count() / 1000.0f;

    _points.push_back({position, pressure, timestamp});
    _currentTime = timestamp;
}

void BrushStroke::renderPreview(const glm::mat4& projectionView)
{
    if (_points.empty()) {
        return;
    }

    // Render to stroke FBO
    glBindFramebuffer(GL_FRAMEBUFFER, _strokeFBO);

    // Enable blending for accumulating brush dabs
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _brushShader->use();
    _brushShader->set("uProjectionView", projectionView);
    _brushShader->set("uBrushColor", _settings.color);
    _brushShader->set("uHardness", _settings.hardness);
    _brushShader->set("uEraser", _settings.eraser);

    // TODO: Bind actual brush texture (for now, we'll need a default circular brush)
    // _brushShader->set("uBrushTexture", 0);

    // Render dabs along the stroke with spacing
    for (size_t i = 0; i < _points.size(); i++) {
        const auto& point = _points[i];

        // Calculate actual brush size based on pressure
        float dabSize = _settings.baseSize * point.pressure;
        float dabOpacity = _settings.opacity * point.pressure;

        // Check spacing from last dab
        float distance = glm::length(point.position - _lastDabPosition);
        float minSpacing = dabSize * _settings.spacing;

        if (distance >= minSpacing || i == 0) {
            // Render a brush dab at this point
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(point.position, 0.0f));
            model = glm::scale(model, glm::vec3(dabSize, dabSize, 1.0f));

            glm::mat4 mvp = projectionView * model;
            _brushShader->set("uProjectionView", mvp);

            _quadMesh->bind();
            _quadMesh->draw();

            _lastDabPosition = point.position;
        }
    }

    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool BrushStroke::shouldCommit() const
{
    // Commit every 100 points or 1 second
    return _points.size() >= 100 || (_points.size() > 0 && _currentTime - _points[0].timestamp > 1.0f);
}

void BrushStroke::clear()
{
    _points.clear();
    _lastDabPosition = glm::vec2(-1000.0f, -1000.0f);

    // Clear the stroke texture
    glBindFramebuffer(GL_FRAMEBUFFER, _strokeFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::vec4 BrushStroke::getBoundingBox() const
{
    if (_points.empty()) {
        return glm::vec4(0.0f);
    }

    glm::vec2 min = _points[0].position;
    glm::vec2 max = _points[0].position;

    for (const auto& point : _points) {
        float radius = _settings.baseSize * point.pressure * 0.5f;
        min = glm::min(min, point.position - glm::vec2(radius));
        max = glm::max(max, point.position + glm::vec2(radius));
    }

    return glm::vec4(min.x, min.y, max.x, max.y);
}
