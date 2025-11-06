#include "TextureAtlas.hpp"

#include <filesystem>
#include <iostream>
#include <cstring>

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb/stb_image.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb/stb_rect_pack.h"

#include "Logger.hpp" // optional, can replace with std::cout

using namespace OpenGLUtils;

TextureAtlas::TextureAtlas(const std::string &path, int pageSize)
    : _texturesFolder(path), _pageSize(pageSize)
{
    reloadTextures();
}

TextureAtlas::~TextureAtlas()
{
    for (auto &page : _pages)
    {
        if (page.textureId)
            glDeleteTextures(1, &page.textureId);
        delete page.ctx;
    }
}

int TextureAtlas::getPageCount() const
{
    return static_cast<int>(_pages.size());
}

void TextureAtlas::reloadTextures()
{
    _uvMap.clear();
    _pages.clear();

    auto imgs = loadImagesFromFolder(_texturesFolder);
    packAllImages(imgs);
}

std::unordered_map<std::string, TextureAtlas::ImageData_s>
TextureAtlas::loadImagesFromFolder(const std::string &path)
{
    std::unordered_map<std::string, ImageData_s> result;

    for (auto &file : std::filesystem::recursive_directory_iterator(path))
    {
        if (!file.is_regular_file()) continue;
        if (file.path().extension() != ".png" && file.path().extension() != ".PNG") continue;

        std::string name = file.path().stem().string();
        auto imgOpt = loadImage(file.path().string());
        if (!imgOpt) continue;

        result[name] = *imgOpt;
        _imageDataCache[file.path().string()] = *imgOpt;
    }

    return result;
}

std::optional<TextureAtlas::ImageData_s> TextureAtlas::loadImage(const std::string &filePath)
{
    ImageData_s imgData;
    try {
        imgData.lastModified =
            std::filesystem::last_write_time(filePath).time_since_epoch().count();
    } catch (...) { imgData.lastModified = 0; }

    auto it = _imageDataCache.find(filePath);
    if (it != _imageDataCache.end() && it->second.lastModified == imgData.lastModified)
        return it->second;

    int channels = 0;
    unsigned char* data = stbi_load(filePath.c_str(), &imgData.width, &imgData.height, &channels, STBI_rgb_alpha);
    if (!data)
    {
        LOG_ERROR("Failed to load '{}': {}", filePath, stbi_failure_reason());
        return std::nullopt;
    }

    size_t size = static_cast<size_t>(imgData.width) * imgData.height * 4;
    imgData.pixels.resize(size);
    std::memcpy(imgData.pixels.data(), data, size);
    stbi_image_free(data);

    _imageDataCache[filePath] = imgData;
    return imgData;
}

void TextureAtlas::packAllImages(const std::unordered_map<std::string, ImageData_s>& imgs)
{
    for (auto &p : imgs)
        addImage(p.first, p.second);
}

bool TextureAtlas::addImage(const std::string &name, const ImageData_s &img)
{
    if (_uvMap.contains(name))
        return true; // already exists

    stbrp_rect r;
    r.w = img.width;
    r.h = img.height;
    r.id = 0;

    for (size_t i = 0; i < _pages.size(); i++)
    {
        AtlasPage &page = _pages[i];
        stbrp_pack_rects(page.ctx, &r, 1);

        if (r.was_packed)
        {
            writeImageToPage(page, r.x, r.y, img);
            updateUV(name, img, (int)i, r.x, r.y);
            return true;
        }
    }

    // No page fit → create new page
    createNewPage();
    AtlasPage &pg = _pages.back();
    stbrp_pack_rects(pg.ctx, &r, 1);
    if (!r.was_packed)
    {
        LOG_ERROR("Image '{}' too big to fit page", name);
        return false;
    }

    writeImageToPage(pg, r.x, r.y, img);
    updateUV(name, img, (int)(_pages.size() - 1), r.x, r.y);
    return true;
}

void TextureAtlas::updateUV(const std::string &name, const ImageData_s &img,
                            int pageIndex, int x, int y)
{
    UVEntry_t uv;
    uv.pageIndex = pageIndex;
    uv.uvMin = { float(x) / _pageSize, float(y) / _pageSize };
    uv.uvMax = { float(x + img.width) / _pageSize, float(y + img.height) / _pageSize };
    _uvMap[name] = uv;
}

void TextureAtlas::writeImageToPage(AtlasPage &page, int x, int y, const ImageData_s &img)
{
    if (page.cpuPixels.empty())
        page.cpuPixels.resize(_pageSize * _pageSize * 4, 0);

    for (int iy = 0; iy < img.height; iy++)
    {
        for (int ix = 0; ix < img.width; ix++)
        {
            size_t src = (iy * img.width + ix) * 4;
            size_t dst = ((y + iy) * _pageSize + (x + ix)) * 4;
            memcpy(&page.cpuPixels[dst], &img.pixels[src], 4);
        }
    }

    glBindTexture(GL_TEXTURE_2D, page.textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, img.width, img.height,
                    GL_RGBA, GL_UNSIGNED_BYTE, img.pixels.data());
}

void TextureAtlas::createNewPage()
{
    AtlasPage page;
    page.width = _pageSize;
    page.height = _pageSize;
    page.cpuPixels.resize(_pageSize * _pageSize * 4, 0);

    page.nodes.resize(_pageSize);
    page.ctx = new stbrp_context;
    stbrp_init_target(page.ctx, _pageSize, _pageSize, page.nodes.data(), page.nodes.size());

    glGenTextures(1, &page.textureId);
    glBindTexture(GL_TEXTURE_2D, page.textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _pageSize, _pageSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, page.cpuPixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    _pages.push_back(std::move(page));
}

std::optional<UVEntry_t> TextureAtlas::getUVRect(const std::string &blockName) const
{
    auto it = _uvMap.find(blockName);
    if (it == _uvMap.end()) return std::nullopt;
    return it->second;
}

void TextureAtlas::bindPage(int pageIndex) const
{
    if (pageIndex < 0 || pageIndex >= (int)_pages.size()) return;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _pages[pageIndex].textureId);
}
