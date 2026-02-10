#include "Application.hpp"

#include <vector>
#include <chrono>
#include <cstring>

#include <glm/glm.hpp>

#include "Logger.hpp"
#include "Vertex.hpp"

#include "UI/UI.hpp"

#include "Tools/ToolsFunctions.hpp"
#include "Effects/InvertEffect.hpp"

const size_t WindowWidth = 1920;
const size_t WindowHeight = 1080;
const std::string_view WindowName = "Gump";
const std::string TexturesFolderPath = "assets/";

Gump::Application::Application()
    : _windowSize(WindowWidth, WindowHeight), _canvasSize(800, 600)
{
    try {
        LOG_DEBUG("Creating window ...");
        _window = std::make_unique<OpenGLUtils::Window>(WindowWidth, WindowHeight, std::string(WindowName));
        LOG_DEBUG("Creating camera ...");
        _camera = std::make_unique<Camera2D>();
        LOG_DEBUG("Loading assets ...");
        _textureAtlas = std::make_unique<OpenGLUtils::TextureAtlas>(TexturesFolderPath);
        _shader = std::make_unique<OpenGLUtils::Shader>(
            "shaders/canva.vert",
            "shaders/canva.frag"
        );
        _checkerboardShader = std::make_unique<OpenGLUtils::Shader>(
            "shaders/checkerboard.vert",
            "shaders/checkerboard.frag"
        );
        _selectionShader = std::make_unique<OpenGLUtils::Shader>(
            "shaders/selection.vert",
            "shaders/selection.frag"
        );
        _maskedSelectionShader = std::make_unique<OpenGLUtils::Shader>(
            "shaders/masked_selection.vert",
            "shaders/masked_selection.frag"
        );
        LOG_DEBUG("Initializing input ...");
        Input::initialize(_window->getHandle());

        // Initialize OpenGL settings
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Enable point sprites for brush rendering
        glEnable(GL_PROGRAM_POINT_SIZE); // Allow shaders to control point size

        // Initialize checkerboard mesh
        updateCheckerboardMesh();

        // Initialize selection mask texture
        glGenTextures(1, &_selectionMaskTexture);

        // Initialize stroke renderer
        _strokeRenderer = std::make_unique<StrokeRenderer>();

        // Initialize effects system
        _effects.push_back(std::make_unique<InvertEffect>());

        // Create a default empty layer
        LOG_DEBUG("Creating default empty layer ...");
        addEmptyLayer("Layer 1", 800, 600);
    } catch (const std::exception& e) {
        LOG_ERROR("Could not create window: {}", e.what());
        throw std::runtime_error("Could not create window");
    }
}

void Gump::Application::run()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    while (_running) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        // Update time
        auto currentTime = std::chrono::high_resolution_clock::now();
        _time = std::chrono::duration<float>(currentTime - startTime).count();

        _window->pollEvents();

        // Measure update time
        auto updateStart = std::chrono::high_resolution_clock::now();
        update();
        auto updateEnd = std::chrono::high_resolution_clock::now();
        _performanceTimings.updateTime = std::chrono::duration<float>(updateEnd - updateStart).count();

        _window->beginFrame();

        // Measure render time
        auto renderStart = std::chrono::high_resolution_clock::now();
        render();
        auto renderEnd = std::chrono::high_resolution_clock::now();
        _performanceTimings.renderTime = std::chrono::duration<float>(renderEnd - renderStart).count();

        _window->beginImGuiFrame();

        // Measure UI time
        auto uiStart = std::chrono::high_resolution_clock::now();
        Gump::renderUI(*this);
        auto uiEnd = std::chrono::high_resolution_clock::now();
        _performanceTimings.uiTime = std::chrono::duration<float>(uiEnd - uiStart).count();

        _window->endFrame();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        _performanceTimings.totalFrameTime = std::chrono::duration<float>(frameEnd - frameStart).count();

        Input::update();
        if (_window->shouldClose()) _running = false;
    }
}

void Gump::Application::stop()
{
    _running = false;
}

void Gump::Application::update()
{
    if ((Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE) ||
        Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_MIDDLE))) {
        _camera->move(Input::getMouseDelta());
    }

    if (Input::getMouseScrollDelta().y != 0.0f) {
        _camera->zoom(1.0f + Input::getMouseScrollDelta().y * 0.1f);
    }

    try {
        Tools::ActionFunction.at(_selectedTool)(*this);
    } catch (const std::out_of_range& e) {
        LOG_ERROR("No such tool registered: {}", _selectedTool);
    }
}

void Gump::Application::render()
{
    // Get and set viewport at the start of each frame
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float width  = static_cast<float>(vp[2]);
    float height = static_cast<float>(vp[3]);
    
    // Set viewport for the main rendering pass
    glViewport(0, 0, vp[2], vp[3]);

    glm::mat4 pv = _camera->getPVMatrix(width, height);

    // Render checkerboard background first
    _checkerboardShader->use();
    _checkerboardShader->set("uProjectionView", pv);
    _checkerboardShader->set("uCanvasSize", glm::vec2(_canvasSize.x, _canvasSize.y));
    if (_checkerboardMesh) {
        _checkerboardMesh->bind();
        _checkerboardMesh->draw();
    }

    // Render layers on top (clipped to canvas bounds)
    _shader->use();
    _shader->set("uProjectionView", pv);
    _shader->set("uCanvasSize", glm::vec2(_canvasSize.x, _canvasSize.y));

    for (const auto& layer : _layers) {
        _textureAtlas->bindPage(layer->texturePageIndex);
        _shader->set("uTexture", 0);
        _shader->set("uTransparency", layer->transparency);
        layer->draw();
    }

    // Render selection overlay
    if (_selectionState.hasSelection && _selectionMesh) {
        // Use masked selection shader if we have a pixel mask
        if (_selectionState.hasMask) {
            _maskedSelectionShader->use();
            _maskedSelectionShader->set("uProjectionView", pv);
            _maskedSelectionShader->set("uTime", _time);
            _maskedSelectionShader->set("uBorderWidth", (1 / _camera->getZoom()) * 5.0f);
            
            // Bind the mask texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _selectionMaskTexture);
            _maskedSelectionShader->set("uMaskTexture", 0);
            
            // Pass mask size
            _maskedSelectionShader->set("uMaskSize", glm::vec2(_selectionState.maskWidth, _selectionState.maskHeight));
            
            _selectionMesh->bind();
            _selectionMesh->draw();
        } else {
            // Use regular selection shader for rectangular selections
            _selectionShader->use();
            _selectionShader->set("uProjectionView", pv);
            _selectionShader->set("uTime", _time);

            // Pass selection size to shader
            glm::vec2 selectionSize = _selectionState.getSize();
            _selectionShader->set("uSelectionSize", selectionSize);
            _selectionShader->set("uBorderWidth", (1 / _camera->getZoom()) * 5.0f);

            _selectionMesh->bind();
            _selectionMesh->draw();
        }
    }

    // Render active stroke (preview)
    if (_currentStroke && !_currentStroke->isEmpty()) {
        _strokeRenderer->renderStroke(*_currentStroke, pv, _camera->getZoom(), _time);
    }
    
    // Don't call glfwSwapBuffers here - Window::endFrame() handles it
}


// Layer management
size_t Gump::Application::getLayerCount() const
{
    return _layers.size();
}

bool Gump::Application::canLayerMoveUp(size_t index) const
{
    return index > 0 && _layers.size() > 1;
}

bool Gump::Application::canLayerMoveDown(size_t index) const
{
    return index + 1 < _layers.size();
}

void Gump::Application::moveLayerUp(size_t index)
{
    if (!canLayerMoveUp(index)) return;
    std::swap(_layers[index], _layers[index - 1]);
}

void Gump::Application::moveLayerDown(size_t index)
{
    if (!canLayerMoveDown(index)) return;
    std::swap(_layers[index], _layers[index + 1]);
}

Gump::Layer &Gump::Application::getLayer(size_t index) const
{
    return *_layers.at(index);
}

void Gump::Application::addLayer(const std::string &name, size_t width, size_t height,
    glm::vec2 uvMin, glm::vec2 uvMax, size_t textureID)
{
    _layers.push_back(std::make_unique<Layer>(name, width, height, uvMin, uvMax, textureID));
}

void Gump::Application::addEmptyLayer(const std::string &name, size_t width, size_t height)
{
    // Generate a unique name if the requested name is already taken
    std::string uniqueName = generateUniqueLayerName(name);
    
    LOG_INFO("Creating empty layer '{}' with size {}x{}", uniqueName, width, height);
    
    // Create transparent pixels (RGBA with all zeros)
    std::vector<unsigned char> emptyPixels(width * height * 4, 0);
    
    // Add the empty image to the texture atlas
    if (_textureAtlas->addImageFromPixels(uniqueName, width, height, emptyPixels)) {
        // Get UV coordinates for the new texture
        auto uvRectOpt = _textureAtlas->getUVRect(uniqueName);
        if (uvRectOpt) {
            // Create the layer
            addLayer(uniqueName, width, height, uvRectOpt->uvMin, uvRectOpt->uvMax, uvRectOpt->pageIndex);
            LOG_INFO("Successfully created empty layer '{}'", uniqueName);
        } else {
            LOG_ERROR("Failed to get UV coordinates for empty layer '{}'", uniqueName);
        }
    } else {
        LOG_ERROR("Failed to add empty layer '{}' to texture atlas", uniqueName);
    }
}

bool Gump::Application::isLayerNameTaken(const std::string& name, size_t excludeIndex) const
{
    for (size_t i = 0; i < _layers.size(); i++) {
        if (i == excludeIndex) {
            continue; // Skip the excluded layer (used when renaming)
        }
        if (_layers[i]->name == name) {
            return true;
        }
    }
    return false;
}

std::string Gump::Application::generateUniqueLayerName(const std::string& baseName) const
{
    // If the base name is not taken, use it as-is
    if (!isLayerNameTaken(baseName)) {
        return baseName;
    }
    
    // Otherwise, append a number to make it unique
    int counter = 1;
    std::string uniqueName;
    
    do {
        uniqueName = baseName + " (" + std::to_string(counter) + ")";
        counter++;
    } while (isLayerNameTaken(uniqueName));
    
    return uniqueName;
}

std::vector<std::unique_ptr<Gump::Layer>> &Gump::Application::getLayers()
{
    return _layers;
}

OpenGLUtils::Shader &Gump::Application::getShader()
{
    return *_shader;
}

OpenGLUtils::TextureAtlas &Gump::Application::getTextureAtlas()
{
    return *_textureAtlas;
}

glm::uvec2 Gump::Application::getWindowSize() const
{
    return _windowSize;
}

glm::uvec2 Gump::Application::getCanvasSize() const
{
    return _canvasSize;
}

void Gump::Application::setCanvasSize(glm::uvec2 size)
{
    _canvasSize = size;
    updateCheckerboardMesh();
}

Gump::PendingImport& Gump::Application::getPendingImport()
{
    return _pendingImport;
}

Gump::ResizeCanvasRequest& Gump::Application::getResizeCanvasRequest()
{
    return _resizeCanvasRequest;
}

Gump::SelectionState& Gump::Application::getSelectionState()
{
    return _selectionState;
}

Gump::Clipboard& Gump::Application::getClipboard()
{
    return _clipboard;
}

Gump::Camera2D& Gump::Application::getCamera()
{
    return *_camera;
}

Gump::FuzzySelectSettings& Gump::Application::getFuzzySelectSettings()
{
    return _fuzzySelectSettings;
}

Gump::PerformanceTimings& Gump::Application::getPerformanceTimings()
{
    return _performanceTimings;
}

// Brush and stroke management
Gump::BrushSettings& Gump::Application::getBrushSettings()
{
    return _brushSettings;
}

std::unique_ptr<Gump::Stroke>& Gump::Application::getCurrentStroke()
{
    return _currentStroke;
}

bool Gump::Application::hasActiveStroke() const
{
    return _currentStroke != nullptr;
}

void Gump::Application::startStroke(const glm::vec2& position, float pressure)
{
    if (_currentStroke) {
        LOG_WARNING("Starting new stroke while one is already active - cancelling previous stroke");
        _currentStroke.reset();
    }
    
    _currentStroke = std::make_unique<Stroke>(_brushSettings);
    _currentStroke->addPoint(position, pressure);
    
    LOG_DEBUG("Stroke started at ({}, {}) with {} points", position.x, position.y, _currentStroke->getPointCount());
}

void Gump::Application::continueStroke(const glm::vec2& position, float pressure)
{
    if (!_currentStroke) {
        LOG_WARNING("Attempting to continue stroke that hasn't been started");
        return;
    }
    
    _currentStroke->addPoint(position, pressure);
}

void Gump::Application::finishStroke()
{
    if (!_currentStroke) {
        LOG_WARNING("Attempting to finish stroke that hasn't been started");
        return;
    }
    
    if (_currentStroke->isEmpty()) {
        LOG_WARNING("Finishing empty stroke");
        _currentStroke.reset();
        return;
    }
    
    LOG_INFO("Finishing stroke with {} points", _currentStroke->getPointCount());
    
    // Apply stroke to the top layer
    if (!_layers.empty()) {
        applyStrokeToLayer(*_currentStroke, *_layers.back());
    } else {
        LOG_WARNING("No layers available to apply stroke to");
    }
    
    // Clear the current stroke
    _currentStroke.reset();
}

void Gump::Application::cancelStroke()
{
    if (_currentStroke) {
        LOG_INFO("Stroke cancelled");
        _currentStroke.reset();
    }
}

void Gump::Application::applyStrokeToLayer(const Stroke& stroke, Layer& layer)
{
    LOG_INFO("Applying stroke to layer '{}'", layer.name);

    // Get the texture page for this layer
    auto pageTexIdOpt = _textureAtlas->getPageTextureID(layer.texturePageIndex);
    if (!pageTexIdOpt) {
        LOG_ERROR("Failed to get texture page for layer");
        return;
    }

    GLuint textureID = *pageTexIdOpt;

    // Get texture dimensions
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLint texWidth, texHeight;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

    // Get layer's position in the texture atlas
    glm::vec2 layerUVMin = layer.getUVMin();
    int layerTexX = static_cast<int>(layerUVMin.x * texWidth);
    int layerTexY = static_cast<int>(layerUVMin.y * texHeight);

    int layerWidth = static_cast<int>(layer.getWidth());
    int layerHeight = static_cast<int>(layer.getHeight());

    // Convert stroke points from world coordinates to layer-local coordinates
    Stroke localStroke(stroke.getBrushSettings());
    for (const auto& point : stroke.getPoints()) {
        glm::vec2 localPos = point.position - layer.position;
        localStroke.addPoint(localPos, point.pressure);
    }

    // Render the stroke to the layer texture
    _strokeRenderer->renderStrokeToTexture(localStroke, textureID, texWidth, texHeight,
                                          layerTexX, layerTexY, layerWidth, layerHeight);

    LOG_INFO("Successfully applied stroke with {} points to layer '{}'", stroke.getPointCount(), layer.name);
}

void Gump::Application::copySelection()
{
    if (!_selectionState.hasSelection || _layers.empty()) {
        LOG_WARNING("No selection or no layers available for copy");
        return;
    }

    // Get the top layer (last in the vector)
    auto& topLayer = _layers.back();

    // Calculate selection bounds in layer space
    glm::vec2 selMin = _selectionState.getMin();
    glm::vec2 selMax = _selectionState.getMax();

    // Clamp selection to layer bounds
    selMin = glm::max(selMin, glm::vec2(0.0f));
    selMax = glm::min(selMax, glm::vec2(topLayer->getWidth(), topLayer->getHeight()));

    int selWidth = static_cast<int>(selMax.x - selMin.x);
    int selHeight = static_cast<int>(selMax.y - selMin.y);

    if (selWidth <= 0 || selHeight <= 0) {
        LOG_WARNING("Invalid selection dimensions for copy");
        return;
    }

    LOG_INFO("Copying selection {}x{} to clipboard", selWidth, selHeight);

    // Get the texture page for the top layer
    auto pageTexIdOpt = _textureAtlas->getPageTextureID(topLayer->texturePageIndex);
    if (!pageTexIdOpt) {
        LOG_ERROR("Failed to get texture page for layer");
        return;
    }

    // Read pixels from the GPU texture
    _clipboard.pixels.resize(selWidth * selHeight * 4);

    glBindTexture(GL_TEXTURE_2D, *pageTexIdOpt);

    // Get UV coordinates for the layer
    glm::vec2 layerUVMin = topLayer->getUVMin();
    glm::vec2 layerUVMax = topLayer->getUVMax();

    // Calculate texture coordinates in pixels
    GLint texWidth, texHeight;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

    // Convert layer UV to pixel coordinates
    int layerTexX = static_cast<int>(layerUVMin.x * texWidth);
    int layerTexY = static_cast<int>(layerUVMin.y * texHeight);

    // Calculate selection position in texture
    int texX = layerTexX + static_cast<int>(selMin.x);
    int texY = layerTexY + static_cast<int>(selMin.y);

    // Create a framebuffer to read from the texture
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *pageTexIdOpt, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        // Read pixels from the framebuffer
        glReadPixels(texX, texY, selWidth, selHeight, GL_RGBA, GL_UNSIGNED_BYTE, _clipboard.pixels.data());
    } else {
        LOG_ERROR("Framebuffer incomplete, cannot read pixels");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    // Update clipboard metadata
    _clipboard.width = selWidth;
    _clipboard.height = selHeight;
    _clipboard.hasData = true;

    LOG_INFO("Selection copied to clipboard");
}

void Gump::Application::cutSelection()
{
    if (!_selectionState.hasSelection || _layers.empty()) {
        LOG_WARNING("No selection or no layers available for cut");
        return;
    }

    // First, copy the selection to clipboard
    copySelection();

    if (!_clipboard.hasData) {
        LOG_ERROR("Failed to copy selection before cutting");
        return;
    }

    // Get the top layer
    auto& topLayer = _layers.back();

    // Calculate selection bounds in layer space
    glm::vec2 selMin = _selectionState.getMin();
    glm::vec2 selMax = _selectionState.getMax();

    // Clamp selection to layer bounds
    selMin = glm::max(selMin, glm::vec2(0.0f));
    selMax = glm::min(selMax, glm::vec2(topLayer->getWidth(), topLayer->getHeight()));

    int selWidth = static_cast<int>(selMax.x - selMin.x);
    int selHeight = static_cast<int>(selMax.y - selMin.y);

    LOG_INFO("Clearing selection area {}x{} from layer", selWidth, selHeight);

    // Get the texture page for the top layer
    auto pageTexIdOpt = _textureAtlas->getPageTextureID(topLayer->texturePageIndex);
    if (!pageTexIdOpt) {
        LOG_ERROR("Failed to get texture page for layer");
        return;
    }

    // Create transparent pixels to clear the selection
    std::vector<unsigned char> transparentPixels(selWidth * selHeight * 4, 0);

    glBindTexture(GL_TEXTURE_2D, *pageTexIdOpt);

    // Get UV coordinates for the layer
    glm::vec2 layerUVMin = topLayer->getUVMin();

    // Calculate texture coordinates in pixels
    GLint texWidth, texHeight;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

    // Convert layer UV to pixel coordinates
    int layerTexX = static_cast<int>(layerUVMin.x * texWidth);
    int layerTexY = static_cast<int>(layerUVMin.y * texHeight);

    // Calculate selection position in texture
    int texX = layerTexX + static_cast<int>(selMin.x);
    int texY = layerTexY + static_cast<int>(selMin.y);

    // Update the texture with transparent pixels
    glTexSubImage2D(GL_TEXTURE_2D, 0, texX, texY, selWidth, selHeight,
                    GL_RGBA, GL_UNSIGNED_BYTE, transparentPixels.data());

    LOG_INFO("Selection area cleared from layer");
}

void Gump::Application::pasteClipboard()
{
    if (!_clipboard.hasData) {
        LOG_WARNING("Clipboard is empty, nothing to paste");
        return;
    }

    LOG_INFO("Pasting clipboard content {}x{}", _clipboard.width, _clipboard.height);

    // Create a unique name for the new layer
    std::string newLayerName = "Pasted_" + std::to_string(_layers.size() + 1);

    // Add the clipboard pixels to the texture atlas
    if (_textureAtlas->addImageFromPixels(newLayerName, _clipboard.width, _clipboard.height, _clipboard.pixels)) {
        // Get UV coordinates for the new texture
        auto uvRectOpt = _textureAtlas->getUVRect(newLayerName);
        if (uvRectOpt) {
            // Create a new layer
            addLayer(newLayerName, _clipboard.width, _clipboard.height, 
                    uvRectOpt->uvMin, uvRectOpt->uvMax, uvRectOpt->pageIndex);

            LOG_INFO("Created new layer '{}' from clipboard", newLayerName);
        } else {
            LOG_ERROR("Failed to get UV coordinates for pasted layer");
        }
    } else {
        LOG_ERROR("Failed to add clipboard content to texture atlas");
    }
}

void Gump::Application::setSelectedTool(const std::string &tool)
{
    _selectedTool = tool;
}

const std::string& Gump::Application::getSelectedTool() const
{
    return _selectedTool;
}

void Gump::Application::updateCheckerboardMesh()
{
    const std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    const std::vector<OpenGLUtils::Vertex_t> vertices = {
        // Position                                    // UVs (unused for checkerboard)
        { { 0.0f,              0.0f,              0.0f }, { 0.0f, 0.0f } }, // top-left
        { { (float)_canvasSize.x, 0.0f,              0.0f }, { 1.0f, 0.0f } }, // top-right
        { { (float)_canvasSize.x, (float)_canvasSize.y, 0.0f }, { 1.0f, 1.0f } }, // bottom-right
        { { 0.0f,              (float)_canvasSize.y, 0.0f }, { 0.0f, 1.0f } }  // bottom-left
    };

    _checkerboardMesh = std::make_unique<OpenGLUtils::Mesh>(vertices, indices);
}

void Gump::Application::updateSelectionMesh()
{
    if (!_selectionState.hasSelection) {
        _selectionMesh.reset();
        return;
    }

    glm::vec2 min = _selectionState.getMin() + _selectionState.offset;
    glm::vec2 max = _selectionState.getMax() + _selectionState.offset;

    const std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    const std::vector<OpenGLUtils::Vertex_t> vertices = {
        { { min.x, min.y, 0.0f }, { 0.0f, 0.0f } }, // top-left
        { { max.x, min.y, 0.0f }, { 1.0f, 0.0f } }, // top-right
        { { max.x, max.y, 0.0f }, { 1.0f, 1.0f } }, // bottom-right
        { { min.x, max.y, 0.0f }, { 0.0f, 1.0f } }  // bottom-left
    };

    _selectionMesh = std::make_unique<OpenGLUtils::Mesh>(vertices, indices);
    
    // Update mask texture if we have a pixel mask
    if (_selectionState.hasMask) {
        updateSelectionMaskTexture();
    }
}

void Gump::Application::updateSelectionMaskTexture()
{
    if (!_selectionState.hasMask || _selectionState.mask.empty()) {
        return;
    }

    // Convert boolean mask to byte texture (0 or 255)
    std::vector<unsigned char> maskTexture(_selectionState.maskWidth * _selectionState.maskHeight);
    for (size_t i = 0; i < _selectionState.mask.size(); i++) {
        maskTexture[i] = _selectionState.mask[i] ? 255 : 0;
    }

    // Upload to GPU
    glBindTexture(GL_TEXTURE_2D, _selectionMaskTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 
                 _selectionState.maskWidth, _selectionState.maskHeight, 
                 0, GL_RED, GL_UNSIGNED_BYTE, maskTexture.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Gump::Application::sendSelectionToNewLayer()
{
    if (!_selectionState.hasSelection || _layers.empty()) {
        LOG_WARNING("No selection or no layers available");
        return;
    }

    // Get the top layer (last in the vector)
    auto& topLayer = _layers.back();

    // Calculate selection bounds in layer space
    glm::vec2 selMin = _selectionState.getMin();
    glm::vec2 selMax = _selectionState.getMax();

    // Clamp selection to layer bounds
    selMin = glm::max(selMin, glm::vec2(0.0f));
    selMax = glm::min(selMax, glm::vec2(topLayer->getWidth(), topLayer->getHeight()));

    int selWidth = static_cast<int>(selMax.x - selMin.x);
    int selHeight = static_cast<int>(selMax.y - selMin.y);

    if (selWidth <= 0 || selHeight <= 0) {
        LOG_WARNING("Invalid selection dimensions");
        return;
    }

    LOG_INFO("Extracting selection {}x{} from layer", selWidth, selHeight);

    // Get the texture page for the top layer
    auto pageTexIdOpt = _textureAtlas->getPageTextureID(topLayer->texturePageIndex);
    if (!pageTexIdOpt) {
        LOG_ERROR("Failed to get texture page for layer");
        return;
    }

    // Read pixels from the GPU texture
    std::vector<unsigned char> pixels(selWidth * selHeight * 4);

    glBindTexture(GL_TEXTURE_2D, *pageTexIdOpt);

    // Get UV coordinates for the layer
    glm::vec2 layerUVMin = topLayer->getUVMin();
    glm::vec2 layerUVMax = topLayer->getUVMax();

    // Calculate texture coordinates in pixels
    GLint texWidth, texHeight;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

    // Convert layer UV to pixel coordinates
    int layerTexX = static_cast<int>(layerUVMin.x * texWidth);
    int layerTexY = static_cast<int>(layerUVMin.y * texHeight);

    // Calculate selection position in texture
    int texX = layerTexX + static_cast<int>(selMin.x);
    int texY = layerTexY + static_cast<int>(selMin.y);

    // Create a framebuffer to read from the texture
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *pageTexIdOpt, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        // Read pixels from the framebuffer
        glReadPixels(texX, texY, selWidth, selHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    } else {
        LOG_ERROR("Framebuffer incomplete, cannot read pixels");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    // Don't flip - the pixels are already in the correct orientation
    // The UV coordinates in Layer already handle the Y-flip

    // Create a unique name for the new layer
    std::string newLayerName = "Selection_" + std::to_string(_layers.size() + 1);

    // Add the extracted pixels to the texture atlas (use pixels directly, not flipped)
    if (_textureAtlas->addImageFromPixels(newLayerName, selWidth, selHeight, pixels)) {
        // Get UV coordinates for the new texture
        auto uvRectOpt = _textureAtlas->getUVRect(newLayerName);
        if (uvRectOpt) {
            // Create a new layer at the selection position
            addLayer(newLayerName, selWidth, selHeight,                     uvRectOpt->uvMin, uvRectOpt->uvMax, uvRectOpt->pageIndex);

            // Position the new layer at the selection location
            auto& newLayer = _layers.back();
            // Note: Layer positioning would need to be implemented if layers support transforms

            LOG_INFO("Created new layer '{}' from selection", newLayerName);
        } else {
            LOG_ERROR("Failed to get UV coordinates for new layer");
        }
    } else {
        LOG_ERROR("Failed to add selection to texture atlas");
    }
}

// Effects system
std::vector<std::unique_ptr<Gump::Effect>>& Gump::Application::getEffects()
{
    return _effects;
}

void Gump::Application::applySelectedEffect()
{
    if (_selectedEffectIndex < 0 || _selectedEffectIndex >= static_cast<int>(_effects.size())) {
        LOG_WARNING("No effect selected or invalid effect index");
        return;
    }

    if (!_selectionState.hasSelection || _layers.empty()) {
        LOG_WARNING("No selection or no layers available for effect");
        return;
    }

    auto& effect = _effects[_selectedEffectIndex];
    auto& topLayer = _layers.back();

    // Calculate selection bounds in layer space
    glm::vec2 selMin = _selectionState.getMin();
    glm::vec2 selMax = _selectionState.getMax();

    // Clamp selection to layer bounds
    selMin = glm::max(selMin, glm::vec2(0.0f));
    selMax = glm::min(selMax, glm::vec2(topLayer->getWidth(), topLayer->getHeight()));

    int selWidth = static_cast<int>(selMax.x - selMin.x);
    int selHeight = static_cast<int>(selMax.y - selMin.y);

    if (selWidth <= 0 || selHeight <= 0) {
        LOG_WARNING("Invalid selection dimensions for effect");
        return;
    }

    // Get the texture page for the top layer
    auto pageTexIdOpt = _textureAtlas->getPageTextureID(topLayer->texturePageIndex);
    if (!pageTexIdOpt) {
        LOG_ERROR("Failed to get texture page for layer");
        return;
    }

    glBindTexture(GL_TEXTURE_2D, *pageTexIdOpt);

    // Get texture dimensions
    GLint texWidth, texHeight;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

    // Get layer's position in the texture atlas
    glm::vec2 layerUVMin = topLayer->getUVMin();
    int layerTexX = static_cast<int>(layerUVMin.x * texWidth);
    int layerTexY = static_cast<int>(layerUVMin.y * texHeight);

    // Calculate selection position in texture
    int texX = layerTexX + static_cast<int>(selMin.x);
    int texY = layerTexY + static_cast<int>(selMin.y);

    // Apply the effect to the texture region
    effect->apply(*pageTexIdOpt, texWidth, texHeight, texX, texY, selWidth, selHeight);

    LOG_INFO("Applied effect '{}' to selection", effect->getName());
}
