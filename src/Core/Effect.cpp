#include "Effect.hpp"
#include "Logger.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace Gump;

Effect::Effect(const std::string& name, const std::string& shaderPath)
    : _name(name)
{
    try {
        _shader = std::make_unique<OpenGLUtils::Shader>(
            "shaders/effect.vert",  // Shared vertex shader
            shaderPath
        );
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load effect shader {}: {}", shaderPath, e.what());
        throw;
    }

    setupQuad();
    // Don't call setupParameters() here - derived class will call it
}

void Effect::setupQuad()
{
    // Full-screen quad vertices (position + UV)
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &_quadVAO);
    glGenBuffers(1, &_quadVBO);

    glBindVertexArray(_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Effect::renderEffect(GLuint targetTexture, GLuint sourceTexture,
                         int texWidth, int texHeight,
                         int regionX, int regionY, int regionWidth, int regionHeight)
{
    // Save current viewport
    GLint savedViewport[4];
    glGetIntegerv(GL_VIEWPORT, savedViewport);

    // Create framebuffer for rendering
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("Framebuffer incomplete for effect rendering");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        return;
    }

    // Set viewport to the region
    glViewport(regionX, regionY, regionWidth, regionHeight);

    // Use shader
    _shader->use();

    // Bind source texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sourceTexture);
    _shader->set("uTexture", 0);

    // Set region uniforms for proper UV mapping
    glm::vec2 uvMin = glm::vec2(
        (float)regionX / texWidth,
        (float)regionY / texHeight
    );
    glm::vec2 uvMax = glm::vec2(
        (float)(regionX + regionWidth) / texWidth,
        (float)(regionY + regionHeight) / texHeight
    );

    _shader->set("uUVMin", uvMin);
    _shader->set("uUVMax", uvMax);

    // Set effect-specific uniforms
    setShaderUniforms();

    // Render quad
    glBindVertexArray(_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Cleanup
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    // Restore viewport
    glViewport(savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3]);
}

void Effect::preview(GLuint sourceTexture, int texWidth, int texHeight,
                    int regionX, int regionY, int regionWidth, int regionHeight,
                    GLuint previewFBO)
{
    // For preview, render to the provided FBO (which displays on screen)
    glBindFramebuffer(GL_FRAMEBUFFER, previewFBO);
    glViewport(regionX, regionY, regionWidth, regionHeight);

    _shader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sourceTexture);
    _shader->set("uTexture", 0);

    glm::vec2 uvMin = glm::vec2(
        (float)regionX / texWidth,
        (float)regionY / texHeight
    );
    glm::vec2 uvMax = glm::vec2(
        (float)(regionX + regionWidth) / texWidth,
        (float)(regionY + regionHeight) / texHeight
    );

    _shader->set("uUVMin", uvMin);
    _shader->set("uUVMax", uvMax);

    setShaderUniforms();

    glBindVertexArray(_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Effect::apply(GLuint targetTexture, int texWidth, int texHeight,
                  int regionX, int regionY, int regionWidth, int regionHeight)
{
    renderEffect(targetTexture, targetTexture, texWidth, texHeight,
                regionX, regionY, regionWidth, regionHeight);

    LOG_INFO("Applied effect '{}' to region {}x{} at ({}, {})", 
             _name, regionWidth, regionHeight, regionX, regionY);
}
