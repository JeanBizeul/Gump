#pragma once

#include <memory>

#include <imgui.h>

#include "Window.hpp"

#include "Layer.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "TextureAtlas.hpp"
#include "Camera2D.hpp"
#include "Input.hpp"

namespace Gump
{

struct PendingImport {
    bool isPending = false;
    std::string layerName;
    size_t width;
    size_t height;
    glm::vec2 uvMin;
    glm::vec2 uvMax;
    size_t texturePageIndex;
    glm::uvec2 suggestedCanvasSize;
};

struct ResizeCanvasRequest {
    bool isRequested = false;
};

struct Clipboard {
    bool hasData = false;
    int width = 0;
    int height = 0;
    std::vector<unsigned char> pixels;
};

struct FuzzySelectSettings {
    float tolerance = 32.0f;          // Color similarity tolerance (0-255)
    bool contiguous = true;            // Only select connected pixels
    bool antiAlias = true;             // Smooth selection edges
    bool sampleMerged = false;         // Sample from all visible layers
    bool addToSelection = false;       // Add to existing selection (Shift)
    bool subtractFromSelection = false; // Subtract from selection (Alt)
};

struct SelectionState {
    bool hasSelection = false;
    glm::vec2 startPos;
    glm::vec2 endPos;
    glm::vec2 offset{0.0f, 0.0f}; // For moving the selection

    // Fuzzy selection data
    bool hasMask = false;              // Whether we have a pixel mask
    std::vector<bool> mask;            // Pixel-level selection mask
    int maskWidth = 0;                 // Width of the mask
    int maskHeight = 0;                // Height of the mask
    glm::vec2 maskOrigin{0.0f, 0.0f}; // Top-left position of the mask in world space

    glm::vec2 getMin() const {
        return glm::vec2(glm::min(startPos.x, endPos.x), glm::min(startPos.y, endPos.y));
    }

    glm::vec2 getMax() const {
        return glm::vec2(glm::max(startPos.x, endPos.x), glm::max(startPos.y, endPos.y));
    }

    glm::vec2 getSize() const {
        return getMax() - getMin();
    }

    void clearMask() {
        hasMask = false;
        mask.clear();
        maskWidth = 0;
        maskHeight = 0;
    }
};

class Application
{
 public:
    Application();
    ~Application() = default;

    void run();
    void stop();

    bool canLayerMoveUp(size_t index) const;
    bool canLayerMoveDown(size_t index) const;
    void moveLayerUp(size_t index);
    void moveLayerDown(size_t index);
    void addLayer(const std::string &name, size_t width, size_t height,
      glm::vec2 uvMin, glm::vec2 uvMax, size_t textureID);
    void addEmptyLayer(const std::string &name, size_t width, size_t height);
    size_t getLayerCount() const;
    Layer &getLayer(size_t index) const;
    std::vector<std::unique_ptr<Layer>> &getLayers();

    OpenGLUtils::Shader &getShader();
    OpenGLUtils::TextureAtlas &getTextureAtlas();

    glm::uvec2 getWindowSize() const;
    Camera2D& getCamera();

    // Canvas management
    glm::uvec2 getCanvasSize() const;
    void setCanvasSize(glm::uvec2 size);

    // Pending import
    PendingImport& getPendingImport();

    // Manual canvas resize
    ResizeCanvasRequest& getResizeCanvasRequest();

    void setSelectedTool(const std::string& tool);
    const std::string& getSelectedTool() const;

    // Selection management
    SelectionState& getSelectionState();
    void updateSelectionMesh();

    // Fuzzy select settings
    FuzzySelectSettings& getFuzzySelectSettings();

    // Clipboard operations
    Clipboard& getClipboard();
    void copySelection();
    void cutSelection();
    void pasteClipboard();

    // Extract pixels from selection and create a new layer
    void sendSelectionToNewLayer();
    
    // Layer name validation
    bool isLayerNameTaken(const std::string& name, size_t excludeIndex = -1) const;
    std::string generateUniqueLayerName(const std::string& baseName) const;

 private:
    bool _running = true;
    glm::uvec2 _windowSize;
    glm::uvec2 _canvasSize;
    PendingImport _pendingImport;
    ResizeCanvasRequest _resizeCanvasRequest;
    Clipboard _clipboard;

    std::string _selectedTool = "move";
    SelectionState _selectionState;
    FuzzySelectSettings _fuzzySelectSettings;

    float _time = 0.0f;

    std::unique_ptr<OpenGLUtils::Window> _window;
    std::unique_ptr<OpenGLUtils::TextureAtlas> _textureAtlas;
    std::vector<std::unique_ptr<Layer>> _layers;
    std::unique_ptr<OpenGLUtils::Shader> _shader;
    std::unique_ptr<OpenGLUtils::Shader> _selectionShader;
    std::unique_ptr<OpenGLUtils::Shader> _maskedSelectionShader;
    std::unique_ptr<OpenGLUtils::Shader> _checkerboardShader;
    std::unique_ptr<OpenGLUtils::Mesh> _checkerboardMesh;
    std::unique_ptr<Camera2D> _camera;
    std::unique_ptr<OpenGLUtils::Mesh> _selectionMesh;
    
    // Selection mask texture for fuzzy select
    GLuint _selectionMaskTexture = 0;

    void update();
    void render();
    void updateCheckerboardMesh();
    void updateSelectionMaskTexture();
};
}