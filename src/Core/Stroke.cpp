#include "Stroke.hpp"
#include <limits>
#include <algorithm>

using namespace Gump;

Stroke::Stroke(const BrushSettings& settings)
    : _brushSettings(settings)
{
}

void Stroke::addPoint(const glm::vec2& position, float pressure)
{
    _points.emplace_back(position, pressure);
}

glm::vec2 Stroke::getMin() const
{
    if (_points.empty()) {
        return glm::vec2(0.0f);
    }
    
    glm::vec2 min(std::numeric_limits<float>::max());
    for (const auto& point : _points) {
        min.x = std::min(min.x, point.position.x);
        min.y = std::min(min.y, point.position.y);
    }
    
    // Expand by brush size
    float radius = _brushSettings.size * 0.5f;
    return min - glm::vec2(radius);
}

glm::vec2 Stroke::getMax() const
{
    if (_points.empty()) {
        return glm::vec2(0.0f);
    }
    
    glm::vec2 max(std::numeric_limits<float>::lowest());
    for (const auto& point : _points) {
        max.x = std::max(max.x, point.position.x);
        max.y = std::max(max.y, point.position.y);
    }
    
    // Expand by brush size
    float radius = _brushSettings.size * 0.5f;
    return max + glm::vec2(radius);
}
