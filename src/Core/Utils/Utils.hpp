#pragma once

#include <string>
#include "Layer.hpp"
#include "Shader.hpp"
#include "TextureAtlas.hpp"

namespace Gump {

namespace Utils {

std::string openFilePickerDialog(const std::string& title = "Select a file",
    const std::string& filter = "*.*");
std::string saveFilePickerDialog(const std::string& title = "Save a file", const std::string& filter = "*.*");

bool exportPNG(const std::string &filepath, const std::vector<std::unique_ptr<Layer>> &layers, const OpenGLUtils::Shader &shader, const OpenGLUtils::TextureAtlas &textureAtlas, glm::uvec2 windowSize);

} // namespace Utils

} // namespace Gump
