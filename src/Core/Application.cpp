#include "Application.hpp"

#include <vector>
#include <chrono>
#include <cstring>

#include <glm/glm.hpp>

#include "Logger.hpp"
#include "Vertex.hpp"

#include "UI/UI.hpp"

#include "Tools/ToolsFunctions.hpp"

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

        // Initialize checkerboard mesh
        updateCheckerboardMesh();
        
        // Initialize selection mask texture
        glGenTextures(1, &_selectionMaskTexture);
        
        // Create default empty layer
        LOG_DEBUG("Creating default empty layer...");
        createEmptyLayer();
    } catch (const std::exception& e) {
        LOG_ERROR("Could not create window: {}", e.what());
        throw std::runtime_error("Could not create window");
    }
}

void Gump::Application::run()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    while (_running) {
        // Update time
        auto currentTime = std::chrono::high_resolution_clock::now();
        _time = std::chrono::duration<float>(currentTime - startTime).count();

        _window->pollEvents();
        update();

        _window->beginFrame();
        render();
        _window->beginImGuiFrame();
        Gump::renderUI(*this);
        _window->endFrame();

        Input::update();
        if (_window->shouldClose()) _running = false;
    }
}

void Gump::Application::stop()
{
    _running = false;
}

void Gump::Application::commitStrokeToLayer(Layer& layer, const BrushStroke& stroke)
{
    if (_layers.empty()) {
        LOG_ERROR("Cannot commit stroke: no layers available");
        return;
    }

    // Get stroke bounding box for partial update
    glm::vec4 bbox = stroke.getBoundingBox();
    glm::vec2 bboxMin(bbox.x, bbox.y);
    glm::vec2 bboxMax(bbox.z, bbox.w);
    
    // Clamp to layer bounds
    bboxMin = glm::max(bboxMin, glm::vec2(0.0f));
    bboxMax = glm::min(bboxMax, glm::vec2(layer.getWidth(), layer.getHeight()));
    
    int width = static_cast<int>(bboxMax.x - bboxMin.x);
    int height = static_cast<int>(bboxMax.y - bboxMin.y);
    
    if (width <= 0 || height <= 0) {
        LOG_WARNING("Stroke bounding box outside layer bounds");
        return;
    }

    LOG_INFO("Committing stroke to layer (bbox: {}x{} at {}, {})", width, height, bboxMin.x, bboxMin.y);

    // Get the texture page for the layer
    auto pageTexIdOpt = _textureAtlas->getPageTextureID(layer.texturePageIndex);
    if (!pageTexIdOpt) {
        LOG_ERROR("Failed to get texture page for layer");
        return;
    }

    // Get UV coordinates for the layer
    glm::vec2 layerUVMin = layer.getUVMin();
    glm::vec2 layerUVMax = layer.getUVMax();

    // Calculate texture coordinates in pixels
    GLuint layerTexture = *pageTexIdOpt;
    glBindTexture(GL_TEXTURE_2D, layerTexture);
    
    GLint texWidth, texHeight;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

    // Convert layer UV to pixel coordinates
    int layerTexX = static_cast<int>(layerUVMin.x * texWidth);
    int layerTexY = static_cast<int>(layerUVMin.y * texHeight);

    // Create FBO to render stroke onto layer texture
    GLuint compositeFBO;
    glGenFramebuffers(1, &compositeFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, compositeFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, layerTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("Composite FBO incomplete");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &compositeFBO);
        return;
    }

    // Set viewport to the entire texture (important!)
    glViewport(0, 0, texWidth, texHeight);

    // Setup blending for merging stroke
    glEnable(GL_BLEND);
    if (_brushSettings.eraser) {
        // Eraser mode: subtract alpha
        glBlendFuncSeparate(GL_ZERO, GL_ZERO, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        // Paint mode: normal alpha blending
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                           GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Render stroke texture onto layer texture
    // We need to render the stroke texture as a textured quad
    // First, create a simple shader to blit the stroke texture
    
    // For now, use a simple approach: read pixels from stroke and write to layer
    // This is less efficient but works reliably
    
    // Get stroke texture
    GLuint strokeTexture = stroke.getStrokeTexture();
    
    // Read pixels from stroke FBO
    std::vector<unsigned char> strokePixels(width * height * 4);
    
    // Bind stroke texture and read the relevant region
    GLuint readFBO;
    glGenFramebuffers(1, &readFBO);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, strokeTexture, 0);
    
    if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        // Read the stroke region
        glReadPixels(static_cast<int>(bboxMin.x), static_cast<int>(bboxMin.y), 
                     width, height, GL_RGBA, GL_UNSIGNED_BYTE, strokePixels.data());
    }
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &readFBO);

    // Now blend these pixels onto the layer texture
    // Calculate where to write in the layer texture
    int writeX = layerTexX + static_cast<int>(bboxMin.x);
    int writeY = layerTexY + static_cast<int>(bboxMin.y);

    // Read existing layer pixels
    glBindFramebuffer(GL_FRAMEBUFFER, compositeFBO);
    std::vector<unsigned char> layerPixels(width * height * 4);
    glReadPixels(writeX, writeY, width, height, GL_RGBA, GL_UNSIGNED_BYTE, layerPixels.data());

    // Manually blend stroke pixels onto layer pixels
    for (int i = 0; i < width * height; i++) {
        int idx = i * 4;
        float srcA = strokePixels[idx + 3] / 255.0f;
        float dstA = layerPixels[idx + 3] / 255.0f;
        
        if (_brushSettings.eraser) {
            // Eraser: reduce alpha
            layerPixels[idx + 3] = static_cast<unsigned char>(dstA * (1.0f - srcA) * 255.0f);
        } else {
            // Normal blending
            float outA = srcA + dstA * (1.0f - srcA);
            if (outA > 0.0f) {
                for (int c = 0; c < 3; c++) {
                    float srcC = strokePixels[idx + c] / 255.0f;
                    float dstC = layerPixels[idx + c] / 255.0f;
                    float outC = (srcC * srcA + dstC * dstA * (1.0f - srcA)) / outA;
                    layerPixels[idx + c] = static_cast<unsigned char>(outC * 255.0f);
                }
                layerPixels[idx + 3] = static_cast<unsigned char>(outA * 255.0f);
            }
        }
    }

    // Write blended pixels back to layer texture
    glBindTexture(GL_TEXTURE_2D, layerTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, writeX, writeY, width, height,
                    GL_RGBA, GL_UNSIGNED_BYTE, layerPixels.data());

    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &compositeFBO);

    // Restore viewport to window size
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    glViewport(0, 0, vp[2], vp[3]);

    LOG_INFO("Stroke committed to layer");
}

void Gump::Application::startStroke(const glm::vec2& position, float pressure)
{
    if (_layers.empty()) {
        LOG_WARNING("Cannot start stroke: no layers available");
        return;
    }

    // Initialize default brush settings if needed
    if (_brushSettings.baseSize == 0.0f) {
        _brushSettings.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Black
        _brushSettings.baseSize = 10.0f;
        _brushSettings.hardness = 0.8f;
        _brushSettings.opacity = 1.0f;
        _brushSettings.spacing = 0.1f;
        _brushSettings.eraser = false;
    }

    LOG_INFO("Starting new stroke at ({}, {})", position.x, position.y);

    // Create new stroke
    _currentStroke = std::make_unique<BrushStroke>(_brushSettings);
    _currentStroke->addPoint(position, pressure);
}

void Gump::Application::continueStroke(const glm::vec2& position, float pressure)
{
    if (!_currentStroke) {
        LOG_WARNING("Cannot continue stroke: no active stroke");
        return;
    }

    _currentStroke->addPoint(position, pressure);

    // Check if we should commit and start a new stroke chunk
    if (_currentStroke->shouldCommit()) {
        LOG_INFO("Stroke reached commit threshold, committing to layer");
        
        if (!_layers.empty()) {
            // Commit to the top layer
            auto& topLayer = _layers.back();
            commitStrokeToLayer(*topLayer, *_currentStroke);
        }

        // Clear the stroke but keep drawing
        _currentStroke->clear();
    }
}

void Gump::Application::endStroke()
{
    if (!_currentStroke) {
        LOG_WARNING("Cannot end stroke: no active stroke");
        return;
    }

    LOG_INFO("Ending stroke");

    // Commit final stroke to layer
    if (!_layers.empty()) {
        auto& topLayer = _layers.back();
        commitStrokeToLayer(*topLayer, *_currentStroke);
    }

    // Clear the stroke
    _currentStroke.reset();
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
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float width  = static_cast<float>(vp[2]);
    float height = static_cast<float>(vp[3]);

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

    // Render active brush stroke preview
    if (_currentStroke) {
        _currentStroke->renderPreview(pv);
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

void Gump::Application::createEmptyLayer()
{
    // Create a unique name for the new layer
    std::string newLayerName = "Layer_" + std::to_string(_layers.size() + 1);
    
    // Create transparent pixels for the layer (matching canvas size)
    std::vector<unsigned char> emptyPixels(_canvasSize.x * _canvasSize.y * 4, 0);
    
    // Add the empty pixels to the texture atlas
    if (_textureAtlas->addImageFromPixels(newLayerName, _canvasSize.x, _canvasSize.y, emptyPixels)) {
        // Get UV coordinates for the new texture
        auto uvRectOpt = _textureAtlas->getUVRect(newLayerName);
        if (uvRectOpt) {
            // Create a new layer
            addLayer(newLayerName, _canvasSize.x, _canvasSize.y, 
                    uvRectOpt->uvMin, uvRectOpt->uvMax, uvRectOpt->pageIndex);

            LOG_INFO("Created new empty layer '{}'", newLayerName);
        } else {
            LOG_ERROR("Failed to get UV coordinates for new empty layer");
        }
    } else {
        LOG_ERROR("Failed to add empty layer to texture atlas");
    }
}
