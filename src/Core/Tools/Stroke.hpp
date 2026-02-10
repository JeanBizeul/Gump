#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Shader.hpp"
#include "Mesh.hpp"

namespace Gump {

struct StrokePoint {
    glm::vec2 position;
    float pressure;      // 0.0 - 1.0 (for size/opacity variation)
    float timestamp;     // For smoothing/interpolation
};

struct BrushSettings {
    glm::vec4 color;
    float baseSize;      // Base brush size in pixels
    float hardness;      // 0.0 (soft) - 1.0 (hard)
    float opacity;       // 0.0 - 1.0
    float spacing;       // Distance between dabs (as ratio of size)
    bool eraser;         // Eraser mode
};

class BrushStroke {
public:
    BrushStroke(const BrushSettings& settings);
    ~BrushStroke();

    // Add point to stroke
    void addPoint(const glm::vec2& position, float pressure = 1.0f);

    // Render current stroke to temporary FBO (live preview)
    void renderPreview(const glm::mat4& projectionView);

    // Check if stroke should be committed (point count threshold, time, etc.)
    bool shouldCommit() const;

    // Get the accumulated stroke texture to merge into layer
    unsigned int getStrokeTexture() const { return _strokeTexture; }

    // Clear stroke data after commit
    void clear();

    // Get bounding box of stroke (for partial texture updates)
    glm::vec4 getBoundingBox() const; // (min.x, min.y, max.x, max.y)

private:
    std::vector<StrokePoint> _points;
    BrushSettings _settings;

    // GPU resources
    unsigned int _strokeFBO;
    unsigned int _strokeTexture;
    std::unique_ptr<OpenGLUtils::Shader> _brushShader;
    std::unique_ptr<OpenGLUtils::Mesh> _quadMesh;

    // Stroke state
    glm::vec2 _lastDabPosition;
    float _currentTime;

    // Generate interpolated dabs between points
    void generateDabs();

    // Initialize GPU resources
    void initGPUResources(glm::uvec2 size);
};

} // namespace Gump
