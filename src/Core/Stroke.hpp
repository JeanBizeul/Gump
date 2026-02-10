#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Gump {

// Represents a single point in a stroke
struct StrokePoint {
    glm::vec2 position;  // World position
    float pressure = 1.0f; // For future tablet support

    StrokePoint(glm::vec2 pos, float press = 1.0f)
        : position(pos), pressure(press) {}
};

// Brush settings for a stroke
struct BrushSettings {
    enum class Shape {
        Circle = 0,
        Square = 1,
        Texture = 2
    };

    float size = 10.0f;           // Brush size in pixels
    glm::vec4 color = {0.0f, 0.0f, 0.0f, 1.0f}; // RGBA color
    float hardness = 1.0f;        // Brush edge hardness (0=soft, 1=hard)
    float spacing = 0.1f;         // Spacing between brush stamps (0-1, relative to size)
    float opacity = 1.0f;         // Brush opacity (0-1)
    bool isEraser = false;        // Whether this is an eraser stroke
    Shape shape = Shape::Circle;  // Brush shape
};

// Represents a complete stroke (from mouse down to mouse up)
class Stroke {
public:
    Stroke(const BrushSettings& settings);
    ~Stroke() = default;

    // Add a point to the stroke
    void addPoint(const glm::vec2& position, float pressure = 1.0f);

    // Get stroke data
    const std::vector<StrokePoint>& getPoints() const { return _points; }
    const BrushSettings& getBrushSettings() const { return _brushSettings; }

    // Check if stroke has any points
    bool isEmpty() const { return _points.empty(); }
    size_t getPointCount() const { return _points.size(); }

    // Get bounding box of the stroke
    glm::vec2 getMin() const;
    glm::vec2 getMax() const;

private:
    std::vector<StrokePoint> _points;
    BrushSettings _brushSettings;
};

} // namespace Gump
