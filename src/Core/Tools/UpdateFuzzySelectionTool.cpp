#include "ToolsFunctions.hpp"
#include "../Application.hpp"
#include "../Input.hpp"
#include <GLFW/glfw3.h>
#include "Logger.hpp"
#include <queue>
#include <set>
#include <cmath>

// Helper function to calculate color distance
static float colorDistance(const unsigned char* c1, const unsigned char* c2) {
    // Euclidean distance in RGB space
    float dr = c1[0] - c2[0];
    float dg = c1[1] - c2[1];
    float db = c1[2] - c2[2];
    return std::sqrt(dr * dr + dg * dg + db * db);
}

// Flood fill algorithm for contiguous selection
static void floodFillSelection(
    const std::vector<unsigned char>& pixels,
    int width, int height,
    int startX, int startY,
    float tolerance,
    std::vector<bool>& selected)
{
    if (startX < 0 || startX >= width || startY < 0 || startY >= height) {
        return;
    }

    // Get the seed pixel color
    int seedIdx = (startY * width + startX) * 4;
    unsigned char seedColor[4] = {
        pixels[seedIdx],
        pixels[seedIdx + 1],
        pixels[seedIdx + 2],
        pixels[seedIdx + 3]
    };

    // Skip fully transparent pixels
    if (seedColor[3] == 0) {
        return;
    }

    std::queue<std::pair<int, int>> queue;
    queue.push({startX, startY});
    selected[startY * width + startX] = true;

    while (!queue.empty()) {
        auto [x, y] = queue.front();
        queue.pop();

        // Check all 4 neighbors
        const int dx[] = {0, 0, -1, 1};
        const int dy[] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            // Check bounds
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                continue;
            }

            int idx = ny * width + nx;
            if (selected[idx]) {
                continue;
            }

            // Get neighbor color
            int pixelIdx = idx * 4;
            unsigned char neighborColor[4] = {
                pixels[pixelIdx],
                pixels[pixelIdx + 1],
                pixels[pixelIdx + 2],
                pixels[pixelIdx + 3]
            };

            // Check color similarity
            float distance = colorDistance(seedColor, neighborColor);
            if (distance <= tolerance) {
                selected[idx] = true;
                queue.push({nx, ny});
            }
        }
    }
}

// Select all similar pixels (non-contiguous)
static void selectAllSimilar(
    const std::vector<unsigned char>& pixels,
    int width, int height,
    int seedX, int seedY,
    float tolerance,
    std::vector<bool>& selected)
{
    if (seedX < 0 || seedX >= width || seedY < 0 || seedY >= height) {
        return;
    }

    // Get the seed pixel color
    int seedIdx = (seedY * width + seedX) * 4;
    unsigned char seedColor[4] = {
        pixels[seedIdx],
        pixels[seedIdx + 1],
        pixels[seedIdx + 2],
        pixels[seedIdx + 3]
    };

    // Skip fully transparent pixels
    if (seedColor[3] == 0) {
        return;
    }

    // Check all pixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            int pixelIdx = idx * 4;

            unsigned char pixelColor[4] = {
                pixels[pixelIdx],
                pixels[pixelIdx + 1],
                pixels[pixelIdx + 2],
                pixels[pixelIdx + 3]
            };

            float distance = colorDistance(seedColor, pixelColor);
            if (distance <= tolerance) {
                selected[idx] = true;
            }
        }
    }
}

void Gump::Tools::UpdateFuzzySelectionTool(Application &app)
{
    auto& selection = app.getSelectionState();
    auto& settings = app.getFuzzySelectSettings();
    auto &cam = app.getCamera();

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float width  = static_cast<float>(vp[2]);
    float height = static_cast<float>(vp[3]);
    glm::vec2 windowSize(width, height);

    // Check if mouse is over ImGui window
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }

    // Check for modifier keys
    settings.addToSelection = Input::isKeyHeld(GLFW_KEY_LEFT_SHIFT) || Input::isKeyHeld(GLFW_KEY_RIGHT_SHIFT);
    settings.subtractFromSelection = Input::isKeyHeld(GLFW_KEY_LEFT_ALT) || Input::isKeyHeld(GLFW_KEY_RIGHT_ALT);

    // Perform fuzzy selection on click
    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        if (app.getLayers().empty()) {
            return;
        }

        glm::vec2 worldPos = cam.screenToWorld(Input::getMousePosition(), windowSize);

        // Get the top layer
        auto& topLayer = app.getLayers().back();

        // Check if click is within layer bounds
        glm::vec2 layerPos = topLayer->position;
        if (worldPos.x < layerPos.x || worldPos.x >= layerPos.x + topLayer->getWidth() ||
            worldPos.y < layerPos.y || worldPos.y >= layerPos.y + topLayer->getHeight()) {
            return;
        }

        // Convert world position to layer-local coordinates
        int clickX = static_cast<int>(worldPos.x - layerPos.x);
        int clickY = static_cast<int>(worldPos.y - layerPos.y);

        // Get pixels from the layer texture
        auto pageTexIdOpt = app.getTextureAtlas().getPageTextureID(topLayer->texturePageIndex);
        if (!pageTexIdOpt) {
            LOG_ERROR("Failed to get texture page for fuzzy select");
            return;
        }

        int layerWidth = static_cast<int>(topLayer->getWidth());
        int layerHeight = static_cast<int>(topLayer->getHeight());
        std::vector<unsigned char> pixels(layerWidth * layerHeight * 4);

        // Read layer pixels from GPU
        glBindTexture(GL_TEXTURE_2D, *pageTexIdOpt);
        glm::vec2 layerUVMin = topLayer->getUVMin();

        GLint texWidth, texHeight;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

        int layerTexX = static_cast<int>(layerUVMin.x * texWidth);
        int layerTexY = static_cast<int>(layerUVMin.y * texHeight);

        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *pageTexIdOpt, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            glReadPixels(layerTexX, layerTexY, layerWidth, layerHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        } else {
            LOG_ERROR("Framebuffer incomplete for fuzzy select");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &fbo);
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);

        // Perform fuzzy selection
        std::vector<bool> selectedPixels(layerWidth * layerHeight, false);

        if (settings.contiguous) {
            floodFillSelection(pixels, layerWidth, layerHeight, clickX, clickY, settings.tolerance, selectedPixels);
        } else {
            selectAllSimilar(pixels, layerWidth, layerHeight, clickX, clickY, settings.tolerance, selectedPixels);
        }

        // Find bounding box of selected pixels
        int minX = layerWidth, maxX = 0;
        int minY = layerHeight, maxY = 0;
        bool hasSelection = false;

        for (int y = 0; y < layerHeight; y++) {
            for (int x = 0; x < layerWidth; x++) {
                if (selectedPixels[y * layerWidth + x]) {
                    hasSelection = true;
                    minX = std::min(minX, x);
                    maxX = std::max(maxX, x);
                    minY = std::min(minY, y);
                    maxY = std::max(maxY, y);
                }
            }
        }

        if (hasSelection) {
            // Convert to world coordinates
            glm::vec2 selectionMin = layerPos + glm::vec2(minX, minY);
            glm::vec2 selectionMax = layerPos + glm::vec2(maxX + 1, maxY + 1);

            if (!settings.addToSelection && !settings.subtractFromSelection) {
                // New selection
                selection.hasSelection = true;
                selection.startPos = selectionMin;
                selection.endPos = selectionMax;
                selection.offset = glm::vec2(0.0f);

                // Store the pixel mask
                selection.hasMask = true;
                selection.maskWidth = maxX - minX + 1;
                selection.maskHeight = maxY - minY + 1;
                selection.maskOrigin = selectionMin;
                selection.mask.clear();
                selection.mask.resize(selection.maskWidth * selection.maskHeight, false);

                // Copy the relevant portion of the selection mask
                for (int y = minY; y <= maxY; y++) {
                    for (int x = minX; x <= maxX; x++) {
                        int srcIdx = y * layerWidth + x;
                        int dstIdx = (y - minY) * selection.maskWidth + (x - minX);
                        selection.mask[dstIdx] = selectedPixels[srcIdx];
                    }
                }

            } else if (settings.addToSelection) {
                // Add to existing selection
                if (selection.hasSelection) {
                    glm::vec2 currentMin = selection.getMin();
                    glm::vec2 currentMax = selection.getMax();
                    glm::vec2 newMin = glm::min(currentMin, selectionMin);
                    glm::vec2 newMax = glm::max(currentMax, selectionMax);

                    selection.startPos = newMin;
                    selection.endPos = newMax;

                    // Merge masks
                    int newWidth = static_cast<int>(newMax.x - newMin.x);
                    int newHeight = static_cast<int>(newMax.y - newMin.y);
                    std::vector<bool> newMask(newWidth * newHeight, false);

                    // Copy existing mask
                    if (selection.hasMask) {
                        int offsetX = static_cast<int>(selection.maskOrigin.x - newMin.x);
                        int offsetY = static_cast<int>(selection.maskOrigin.y - newMin.y);

                        for (int y = 0; y < selection.maskHeight; y++) {
                            for (int x = 0; x < selection.maskWidth; x++) {
                                if (selection.mask[y * selection.maskWidth + x]) {
                                    int newX = x + offsetX;
                                    int newY = y + offsetY;
                                    if (newX >= 0 && newX < newWidth && newY >= 0 && newY < newHeight) {
                                        newMask[newY * newWidth + newX] = true;
                                    }
                                }
                            }
                        }
                    }

                    // Add new selection
                    int offsetX = static_cast<int>(selectionMin.x - newMin.x);
                    int offsetY = static_cast<int>(selectionMin.y - newMin.y);

                    for (int y = minY; y <= maxY; y++) {
                        for (int x = minX; x <= maxX; x++) {
                            if (selectedPixels[y * layerWidth + x]) {
                                int newX = (x - minX) + offsetX;
                                int newY = (y - minY) + offsetY;
                                if (newX >= 0 && newX < newWidth && newY >= 0 && newY < newHeight) {
                                    newMask[newY * newWidth + newX] = true;
                                }
                            }
                        }
                    }

                    selection.hasMask = true;
                    selection.maskWidth = newWidth;
                    selection.maskHeight = newHeight;
                    selection.maskOrigin = newMin;
                    selection.mask = std::move(newMask);

                } else {
                    selection.hasSelection = true;
                    selection.startPos = selectionMin;
                    selection.endPos = selectionMax;
                    selection.offset = glm::vec2(0.0f);

                    // Store the pixel mask
                    selection.hasMask = true;
                    selection.maskWidth = maxX - minX + 1;
                    selection.maskHeight = maxY - minY + 1;
                    selection.maskOrigin = selectionMin;
                    selection.mask.clear();
                    selection.mask.resize(selection.maskWidth * selection.maskHeight, false);

                    for (int y = minY; y <= maxY; y++) {
                        for (int x = minX; x <= maxX; x++) {
                            int srcIdx = y * layerWidth + x;
                            int dstIdx = (y - minY) * selection.maskWidth + (x - minX);
                            selection.mask[dstIdx] = selectedPixels[srcIdx];
                        }
                    }
                }
            } else if (settings.subtractFromSelection) {
                // Subtract from selection
                if (selection.hasSelection && selection.hasMask) {
                    // Calculate overlap region
                    glm::vec2 currentMin = selection.maskOrigin;
                    glm::vec2 currentMax = currentMin + glm::vec2(selection.maskWidth, selection.maskHeight);

                    int offsetX = static_cast<int>(selectionMin.x - currentMin.x);
                    int offsetY = static_cast<int>(selectionMin.y - currentMin.y);

                    // Subtract the new selection from existing mask
                    for (int y = minY; y <= maxY; y++) {
                        for (int x = minX; x <= maxX; x++) {
                            if (selectedPixels[y * layerWidth + x]) {
                                int maskX = (x - minX) + offsetX;
                                int maskY = (y - minY) + offsetY;

                                if (maskX >= 0 && maskX < selection.maskWidth && 
                                    maskY >= 0 && maskY < selection.maskHeight) {
                                    selection.mask[maskY * selection.maskWidth + maskX] = false;
                                }
                            }
                        }
                    }

                    // Check if there are any selected pixels left
                    bool hasAnySelection = false;
                    for (bool pixel : selection.mask) {
                        if (pixel) {
                            hasAnySelection = true;
                            break;
                        }
                    }

                    if (!hasAnySelection) {
                        selection.hasSelection = false;
                        selection.clearMask();
                    }
                }
            }

            LOG_INFO("Fuzzy select: selected region {}x{}", maxX - minX + 1, maxY - minY + 1);
        }

        app.updateSelectionMesh();
    }
}
