#include "TextureAtlas.hpp"

#include <filesystem>
#include <iostream>
#include <optional>

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION  // Required to make stb_image work
#define STBI_ONLY_PNG             // Only compiles the PNG module
#include "stb/stb_image.h"

#include "Logger.hpp"

static std::optional<OpenGLUtils::ImageData_t> loadImage(const std::string &path)
{
    OpenGLUtils::ImageData_t imgData;
    int channels;
    unsigned char *data = stbi_load(path.c_str(), &imgData.width, &imgData.height,
        &channels, STBI_rgb_alpha);

    if (data) {
        imgData.pixels.assign(data, data + (sizeof(char) * imgData.width * imgData.height * channels));
        return imgData;
    } else {
        LOG_ERROR("Failed to load image '{}': {}", path, stbi_failure_reason());
        return std::nullopt;
    }
}

static std::vector<std::pair<std::string, OpenGLUtils::ImageData_t>> loadImagesFromFolder
    (const std::string &path)
{
    std::filesystem::recursive_directory_iterator it(path);
    std::vector<std::pair<std::string, OpenGLUtils::ImageData_t>> imgsData;
    std::optional<OpenGLUtils::ImageData_t> temp;
    std::string filename;
    unsigned int success = 0;
    unsigned int failure = 0;

    LOG_INFO("Loading all PNG files recursively from '{}'", path);
    for (auto &file : it) {
        if (file.is_regular_file()) {
            temp = loadImage(file.path());
            if (temp != std::nullopt) {
                filename = file.path().stem().string();
                imgsData.push_back({filename, temp.value()});
                LOG_DEBUG("Loaded PNG rgba image '{}' from '{}'",
                    filename, file.path().c_str());
                success++;
            } else
                failure++;
        }
    }
    LOG_INFO("Succesfully loaded {}/{} PNG rgba images", success, success + failure);
    return imgsData;
}

OpenGLUtils::TextureAtlas::TextureAtlas(const std::string &path, int tileWidth, int tileHeight)
    : _texturesFolder(path), _tileWidth(tileWidth), _tileHeight(tileHeight)
{
    reloadTextures();
}

void OpenGLUtils::TextureAtlas::reloadTextures()
{
    std::vector<std::pair<std::string, ImageData_t>> imgsData = loadImagesFromFolder(_texturesFolder);
    int numTiles = imgsData.size();
    if (numTiles == 0) return;

    // Compute atlas size (aiming for a square)
    int tilesPerRow = static_cast<int>(std::ceil(std::sqrt(numTiles)));
    _atlasWidth = tilesPerRow * _tileWidth;
    _atlasHeight = tilesPerRow * _tileHeight;

    // Allocate enough space for the whole image
    std::vector<unsigned char> atlasPixel(_atlasWidth * _atlasHeight * 4, 0);

    // Copy each tile into the atlas (buffer + uvMap)
    for (int i = 0; i < numTiles; i++) {
        const auto &[name, img] = imgsData[i];

        int row = i / tilesPerRow;
        int col = i % tilesPerRow;
        int dstX = col * _tileWidth;
        int dstY = row * _tileHeight;

        // Copy pixel by pixel ...
        for (int y = 0; y < _tileHeight; y++) {
            for (int x = 0; x < _tileWidth; x++) {
                int srcIndex = (y * img.width + x) * 4;
                int dstIndex = ((dstY + y) * _atlasWidth + (dstX + x)) * 4;

                // ... and color by color
                for (int c = 0; c < 4; c++)
                    atlasPixel[dstIndex + c] = img.pixels[srcIndex + c];
            }
        }

        // Store normalized UVs in _uvMap
        glm::vec2 uvMin = {
            static_cast<float>(dstX) / _atlasWidth,
            static_cast<float>(dstY) / _atlasHeight
        };
        glm::vec2 uvMax = {
            static_cast<float>(dstX + _tileWidth) / _atlasWidth,
            static_cast<float>(dstY + _tileHeight) / _atlasHeight
        };
        _uvMap[name] = {uvMin, uvMax};
    }

    // Upload to the GPU
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _atlasWidth, _atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlasPixel.data());

    // Pixel perfect rendering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // What to do if texture is not big enough: repeat last pixel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void OpenGLUtils::TextureAtlas::bind()
{
    glBindTexture(GL_TEXTURE0, _id);
}

/// Defaults to give the first texture of the atlas if not found.
OpenGLUtils::UVRect_t OpenGLUtils::TextureAtlas::getUVRect(const std::string &blockName) const
{
    if (_uvMap.find(blockName) == _uvMap.end())
        return {{0.0f, 0.0f}, {_tileWidth, _tileHeight}};
    return _uvMap.find(blockName)->second;
}
