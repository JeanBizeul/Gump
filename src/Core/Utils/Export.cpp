#include "Utils/Utils.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image_write.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Logger.hpp"

using namespace Gump;

GLuint fbo, colorTex;

static glm::uvec2 findBiggestLayerSize(const std::vector<std::unique_ptr<Layer>> &layers)
{
    unsigned int outWidth = 0;
    unsigned int outHeight = 0;

    for (const auto &layer : layers)
    {
        if (layer->getWidth() > outWidth)
            outWidth = layer->getWidth();
        if (layer->getHeight() > outHeight)
            outHeight = layer->getHeight();
    }
    return glm::uvec2(outWidth, outHeight);
}

static void createExportFBO(unsigned int width, unsigned int height)
{
    LOG_DEBUG("Creating export FBO of size {}x{}", width, height);
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create the render texture
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, colorTex, 0);

    // No depth buffer needed unless your renderer requires it

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("Export FBO is incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


bool Utils::exportPNG(const std::string &filepath, const std::vector<std::unique_ptr<Layer>> &layers, const OpenGLUtils::Shader &shader, const OpenGLUtils::TextureAtlas &atlas)
{
    LOG_INFO("Preparing to export PNG to {}", filepath);
    glm::uvec2 exportSize = findBiggestLayerSize(layers);
    createExportFBO(exportSize.x, exportSize.y);


    LOG_DEBUG("Rendering layers to the frame buffer");
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, exportSize.x, exportSize.y);

    glClearColor(0, 0, 0, 0); // transparent output
    glClear(GL_COLOR_BUFFER_BIT);

    // render each layer in correct order
    glm::mat4 pv = glm::ortho(0.0f, static_cast<float>(exportSize.x),  0.0f, static_cast<float>(exportSize.y), -1.0f, 1.0f);

    shader.use();
    shader.set("uProjectionView", pv);

    for (const auto& layer : layers) {
        atlas.bindPage(layer->texturePageIndex);
        shader.set("uTexture", 0);
        shader.set("uTransparency", layer->transparency);
        layer->draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    LOG_DEBUG("Reading pixels from frame buffer");
    std::vector<unsigned char> pixels(exportSize.x * exportSize.y * 4);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, exportSize.x, exportSize.y,
                GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Flip vertically
    for (int y = 0; y < exportSize.y / 2; ++y)
    {
        int top = y * exportSize.x * 4;
        int bottom = (exportSize.y - 1 - y) * exportSize.x * 4;
        for (int x = 0; x < exportSize.x * 4; ++x)
            std::swap(pixels[top + x], pixels[bottom + x]);
    }

    LOG_INFO("Writing PNG to {}", filepath);
    stbi_write_png(filepath.c_str(), exportSize.x, exportSize.y, 4,
                pixels.data(), exportSize.x * 4);
    LOG_INFO("Export complete");
    return true;
}
