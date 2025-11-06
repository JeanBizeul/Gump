#include "TextureAtlas.hpp"

#include <filesystem>
#include <iostream>
#include <optional>
#include <algorithm>
#include <cstring>

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION  // Required to make stb_image work
#define STBI_ONLY_PNG             // Only compiles the PNG module
#include "stb/stb_image.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb/stb_rect_pack.h"

#include "Logger.hpp"

using namespace OpenGLUtils;

static int getOpenGLMaxTextureSize() {
    int maxTextureSize = 0;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    return maxTextureSize;
}

TextureAtlas::TextureAtlas(const std::string &path)
    : _texturesFolder(path), _pageSize(getOpenGLMaxTextureSize())
{
    LOG_DEBUG("Creating TextureAtlas with page size {}x{}", _pageSize, _pageSize);
    reloadTextures();
}

TextureAtlas::~TextureAtlas()
{
    if (!_atlasPages.empty()) {
        glDeleteTextures(static_cast<GLsizei>(_atlasPages.size()), _atlasPages.data());
    }
}

void TextureAtlas::reloadTextures()
{
    LOG_INFO("TextureAtlas: Reloading textures from '{}'", _texturesFolder);

    // Clear previous data
    _uvMap.clear();
    _atlasPages.clear();
    _pageWidthUsed.clear();
    _pageHeightUsed.clear();

    // Load images from disk
    auto imgs = loadImagesFromFolder(_texturesFolder);
    if (imgs.empty()) {
        LOG_WARNING("No images found in '{}'", _texturesFolder);
        return;
    }

    // Pack and upload into pages
    packAllImages(imgs);

    LOG_INFO("TextureAtlas: Created {} page(s).", getPageCount());
}

std::unordered_map<std::string, TextureAtlas::ImageData_s>
TextureAtlas::loadImagesFromFolder(const std::string& path)
{
    std::unordered_map<std::string, ImageData_s> result;

    for (auto& file : std::filesystem::recursive_directory_iterator(path))
    {
        if (!file.is_regular_file()) continue;
        if (file.path().extension() != ".png") continue;

        std::string filename = file.path().stem().string();

        auto imgOpt = loadImage(file.path().string());
        if (!imgOpt) continue;

        result[filename] = *imgOpt;     // add to map
        _imageDataCache[file.path().string()] = *imgOpt;  // update cache
        LOG_DEBUG("Loaded PNG rgba image '{}' from '{}' of size <{}x{}>",
            filename, file.path(), imgOpt->width, imgOpt->height);
    }

    return result;
}

std::optional<TextureAtlas::ImageData_s> TextureAtlas::loadImage(const std::string &filePath)
{
    ImageData_s imgData;

    // get last write time
    try {
        auto ftime = std::filesystem::last_write_time(filePath);
        imgData.lastModified = static_cast<std::time_t>(ftime.time_since_epoch().count());
    } catch (...) {
        imgData.lastModified = 0;
    }

    // cached?
    auto it = _imageDataCache.find(filePath);
    if (it != _imageDataCache.end()) {
        if (it->second.lastModified == imgData.lastModified) {
            return it->second;
        }
    }

    int channels = 0;
    unsigned char *data = stbi_load(filePath.c_str(), &imgData.width, &imgData.height, &channels, STBI_rgb_alpha);
    if (!data) {
        LOG_ERROR("stb_image failed to load '{}': {}", filePath, stbi_failure_reason());
        return std::nullopt;
    }

    const int expectedChannels = 4; // STBI_rgb_alpha
    size_t pixelsCount = static_cast<size_t>(imgData.width) * static_cast<size_t>(imgData.height) * expectedChannels;
    imgData.pixels.resize(pixelsCount);
    std::memcpy(imgData.pixels.data(), data, pixelsCount);

    stbi_image_free(data);

    // store in cache
    _imageDataCache[filePath] = imgData;

    return imgData;
}

void TextureAtlas::packAllImages(
    const std::unordered_map<std::string, ImageData_s>& imgs)
{
    _uvMap.clear();
    _atlasPages.clear();

    std::vector<std::pair<std::string, const ImageData_s*>> pending;
    for (auto& [name, img] : imgs)
        pending.push_back({name, &img});

    while (!pending.empty())
    {
        // start new page
        stbrp_context ctx;
        std::vector<stbrp_node> nodes(_pageSize);
        stbrp_init_target(&ctx, _pageSize, _pageSize, nodes.data(), _pageSize);

        std::vector<stbrp_rect> rects;
        rects.reserve(pending.size());

        for (auto& [name, img] : pending)
        {
            stbrp_rect r;
            r.id = (int)rects.size();
            r.w  = img->width;
            r.h  = img->height;
            rects.push_back(r);
        }

        stbrp_pack_rects(&ctx, rects.data(), rects.size());

        // Prepare next-page list
        std::vector<std::pair<std::string, const ImageData_s*>> nextPage;

        // CPU buffer for this page
        std::vector<unsigned char> pixels(_pageSize * _pageSize * 4, 0);

        for (int i = 0; i < rects.size(); i++)
        {
            auto& rect = rects[i];
            const auto& [name, img] = pending[i];

            if (!rect.was_packed)
            {
                nextPage.push_back({name, img});
                continue;
            }

            // Copy pixels into page buffer
            for (int y = 0; y < img->height; y++)
            {
                for (int x = 0; x < img->width; x++)
                {
                    int src = (y * img->width + x) * 4;
                    int dst = ((rect.y + y) * _pageSize + (rect.x + x)) * 4;

                    for (int c = 0; c < 4; c++)
                        pixels[dst + c] = img->pixels[src + c];
                }
            }

            // Store final UVs
            glm::vec2 uvMin = {
                float(rect.x) / _pageSize,
                float(rect.y) / _pageSize
            };
            glm::vec2 uvMax = {
                float(rect.x + img->width) / _pageSize,
                float(rect.y + img->height) / _pageSize
            };

            UVEntry_t entry;
            entry.pageIndex = _atlasPages.size();
            entry.uvMin = uvMin;
            entry.uvMax = uvMax;

            _uvMap[name] = entry;
        }

        // Upload this page to GL
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _pageSize, _pageSize,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        _atlasPages.push_back(tex);

        // Continue with remaining images
        pending = std::move(nextPage);
    }
}

void TextureAtlas::uploadPageToGPU(int pageIndex, const std::vector<unsigned char> &pixels)
{
    unsigned int texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    // Upload RGBA8
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _pageSize, _pageSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // No filtering by default (pixel-perfect)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // clamp so samples outside return edge texel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // keep ID
    _atlasPages.push_back(texId);

    // unbind safe
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureAtlas::bindPage(int pageIndex) const
{
    if (pageIndex < 0 || pageIndex >= static_cast<int>(_atlasPages.size())) {
        LOG_ERROR("bindPage: invalid pageIndex {}", pageIndex);
        return;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _atlasPages[pageIndex]);
}

std::optional<UVEntry_t> TextureAtlas::getUVRect(const std::string &blockName) const
{
    auto it = _uvMap.find(blockName);
    if (it == _uvMap.end()) return std::nullopt;
    return it->second;
}
