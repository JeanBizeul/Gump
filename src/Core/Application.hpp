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
#include "Stroke.hpp"
#include "StrokeRenderer.hpp"
#include "Effect.hpp"
#include "ShortcutManager.hpp"
#include "ActionRegistry.hpp"
#include "PreferencesManager.hpp"

namespace Gump
{

struct PerformanceTimings {
    float updateTime = 0.0f;
    float renderTime = 0.0f;
    float uiTime = 0.0f;
    float totalFrameTime = 0.0f;
};

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
    Camera2D &getCamera();
    void resetCameraZoom() { _camera->resetZoom(); }
    // Canvas management
    glm::uvec2 getCanvasSize() const;
    void setCanvasSize(glm::uvec2 size);
    // Pending import
    PendingImport &getPendingImport();
    // Manual canvas resize
    ResizeCanvasRequest &getResizeCanvasRequest();
    void setSelectedTool(const std::string &tool);
    const std::string &getSelectedTool() const;
    // Selection management
    SelectionState &getSelectionState();
    void updateSelectionMesh();
    // Fuzzy select settings
    FuzzySelectSettings &getFuzzySelectSettings();
    // Clipboard operations
    Clipboard &getClipboard();
    void copySelection();
    void cutSelection();
    void pasteClipboard();
    // Extract pixels from selection and create a new layer
    void sendSelectionToNewLayer();
    
    // Effects system
    std::vector<std::unique_ptr<Effect>>& getEffects();
    int getSelectedEffectIndex() const { return _selectedEffectIndex; }
    void setSelectedEffectIndex(int index) { _selectedEffectIndex = index; }
    void applySelectedEffect();
    
    // Effect preview
    int getEffectPreviewIndex() const { return _effectPreviewIndex; }
    void setEffectPreviewIndex(int index);
    void clearEffectPreview();
    void renderEffectPreview();
    GLuint getEffectPreviewTexture() const { return _effectPreviewTexture; }
    bool isEffectPreviewActive() const { return _effectPreviewActive; }
    
    // Layer name validation
    bool isLayerNameTaken(const std::string &name, size_t excludeIndex = -1) const;
    std::string generateUniqueLayerName(const std::string &baseName) const;
    // Brush and stroke management
    BrushSettings &getBrushSettings();
    std::unique_ptr<Stroke> &getCurrentStroke();
    bool hasActiveStroke() const;
    void startStroke(const glm::vec2 &position, float pressure = 1.0f);
    void continueStroke(const glm::vec2 &position, float pressure = 1.0f);
    void finishStroke();
    void cancelStroke();
    void applyStrokeToLayer(const Stroke &stroke, Layer &layer);
    
    // Performance monitoring
    PerformanceTimings& getPerformanceTimings();

    // Preferences
    void openPreferences() { _preferencesOpen = true; }
    void closePreferences();
    bool isPreferencesOpen() const { return _preferencesOpen; }
    bool* getPreferencesOpenPtr() { return &_preferencesOpen; }
    
    // Shortcut management
    ShortcutManager& getShortcutManager() { return _shortcutManager; }
    
    // Preferences management
    PreferencesManager& getPreferencesManager() { return _preferencesManager; }
    
    // Apply style settings from preferences to ImGui
    void applyStyleSettingsFromPreferences();

    // File management
    const std::string& getCurrentFilePath() const { return _currentFilePath; }
    void setCurrentFilePath(const std::string& path) { _currentFilePath = path; }

private:
    bool _running = true;
    std::string _currentFilePath; // Current save file path
    glm::uvec2 _windowSize;
    glm::uvec2 _canvasSize;
    PendingImport _pendingImport;
    ResizeCanvasRequest _resizeCanvasRequest;
    Clipboard _clipboard;
    std::string _selectedTool = "move";
    SelectionState _selectionState;
    FuzzySelectSettings _fuzzySelectSettings;
    // Brush and stroke state
    BrushSettings _brushSettings;
    std::unique_ptr<Stroke> _currentStroke;
    std::unique_ptr<StrokeRenderer> _strokeRenderer;
    
    // Performance monitoring
    PerformanceTimings _performanceTimings;

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

    // Effects system
    std::vector<std::unique_ptr<Effect>> _effects;
    int _selectedEffectIndex = -1;

    // Effect preview
    int _effectPreviewIndex = -1;
    GLuint _effectPreviewTexture = 0;
    int _effectPreviewWidth = 0;
    int _effectPreviewHeight = 0;
    bool _effectPreviewActive = false;

    bool _preferencesOpen = false;
    ShortcutManager _shortcutManager;
    PreferencesManager _preferencesManager;

    void update();
    void render();
    void updateCheckerboardMesh();
    void updateSelectionMaskTexture();
};

}