#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <optional>

#include "stb/stb_rect_pack.h"

#include "glm/vec2.hpp"

namespace OpenGLUtils
{

struct UVEntry_t {
    int pageIndex;    // Index of the texture atlas page
    glm::vec2 uvMin;  // Top-left corner (0..1)
    glm::vec2 uvMax;  // Bottom-right corner (0..1)
};

class TextureAtlas
{
public:
    struct ImageData_s {
        int width;
        int height;
        std::time_t lastModified;
        std::vector<unsigned char> pixels; // RGBA
    };

    explicit TextureAtlas(const std::string &path);
    TextureAtlas() = delete;
    TextureAtlas(const TextureAtlas &other) = delete;
    TextureAtlas &operator=(const TextureAtlas &other) = delete;
    ~TextureAtlas();

    // Load all images from folder and create atlas pages
    void reloadTextures();

    // Bind a specific page
    void bindPage(int pageIndex) const;

    // Get UV for a given image name
    std::optional<UVEntry_t> getUVRect(const std::string &blockName) const;

    // Total number of atlas pages
    int getPageCount() const;

    // Add one image dynamically
    bool addImageFromFile(const std::string &filePath);
    bool addImageFromFile(const std::wstring &filePath);

private:
    struct AtlasPage {
        unsigned int textureId = 0;
        std::vector<unsigned char> cpuPixels;
        int width = 0;
        int height = 0;

        // stb_rect_pack context
        stbrp_context* ctx = nullptr;
        std::vector<struct stbrp_node> nodes;
    };

    std::unordered_map<std::string, ImageData_s> loadImagesFromFolder(const std::string &path);
    std::optional<ImageData_s> loadImage(const std::string &filePath);

    bool addImage(const std::string &name, const ImageData_s& img);
    void packAllImages(const std::unordered_map<std::string, ImageData_s>& imgs);
    void createNewPage();
    void writeImageToPage(AtlasPage& page, int x, int y, const ImageData_s& img);
    void updateUV(const std::string &name, const ImageData_s &img, int pageIndex, int x, int y);

private:
    std::string _texturesFolder;
    const int _pageSize;

    std::vector<AtlasPage> _pages;
    std::unordered_map<std::string, UVEntry_t> _uvMap;
    std::unordered_map<std::string, ImageData_s> _imageDataCache;
};

} // namespace OpenGLUtils
