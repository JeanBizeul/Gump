#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <optional>

#include "glm/vec2.hpp"

namespace OpenGLUtils
{

struct UVEntry_t {
    int pageIndex;    // Index of the texture atlas page
    glm::vec2 uvMin;  // Top left corner  (0.0f, 0.0f)
    glm::vec2 uvMax;  // Top right corner (0.5f, 0.5f)
};

class TextureAtlas
{
 public:
    struct ImageData_s {
        int width;
        int height;
        std::time_t lastModified;
        std::vector<unsigned char> pixels;  // RGBA
    };

    TextureAtlas(const std::string &path);
    TextureAtlas() = delete;

    TextureAtlas(const TextureAtlas &other) = delete;
    TextureAtlas &operator=(const TextureAtlas &other) = delete;

    ~TextureAtlas();

    void reloadTextures();
    void bindPage(int pageIndex) const;

    std::optional<UVEntry_t> getUVRect(const std::string &blockName) const;

    int getPageCount() const;
 private:
    std::unordered_map<std::string, TextureAtlas::ImageData_s> loadImagesFromFolder(const std::string &path);  
    std::optional<ImageData_s> loadImage(const std::string &filePath);

    void packAllImages(const std::unordered_map<std::string, ImageData_s>& imgs);
    void uploadPageToGPU(int pageIndex, const std::vector<unsigned char> &pixelData);

    std::string _texturesFolder;
    
    const int _pageSize;

    std::vector<unsigned int> _atlasPages;

    std::vector<int> _pageWidthUsed;
    std::vector<int> _pageHeightUsed;

    std::unordered_map<std::string, UVEntry_t> _uvMap;

    std::unordered_map<std::string, ImageData_s> _imageDataCache;
};

}  // namespace OpenGLUtils
