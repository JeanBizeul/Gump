#include "UI/UI.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Application.hpp"
#include "Logger.hpp"

constexpr ImVec2 IconSize = {24, 24}; // 24x24 pixels icons
constexpr ImVec4 BackgroundColor = {0.3, 0.3, 0.3, 0.3}; // Transparent
constexpr ImVec4 SelectedBackgroundColor = {0.6, 0.6, 0.6, 0.6}; // Transparent gray-ish

constexpr int Columns = 3; // Number of columns in the grid

static void renderSelectionTool(Gump::Application &app);
static void renderMoveTool(Gump::Application &app);
static void renderFuzzySelectTool(Gump::Application &app);

void Gump::UI::renderTools(Gump::Application &app) {
    ImGui::Begin("Tools");

    ImGui::SameLine();
    renderMoveTool(app);
    ImGui::SameLine();
    renderSelectionTool(app);
    ImGui::SameLine();
    renderFuzzySelectTool(app);
    ImGui::End();
}

static void renderMoveTool(Gump::Application &app) {
    auto &atlas = app.getTextureAtlas();

    std::optional<OpenGLUtils::UVEntry_t> uvEntryOpt = atlas.getUVRect("move-tool");

    if (uvEntryOpt == std::nullopt) {
        ImGui::Text("Move tool icon not found in texture atlas.");
        return;
    }

    OpenGLUtils::UVEntry_t uvEntry = uvEntryOpt.value();

    ImVec2 uvMin = {uvEntry.uvMin.x, uvEntry.uvMin.y};
    ImVec2 uvMax = {uvEntry.uvMax.x, uvEntry.uvMax.y};

    std::optional<GLuint> textureIDOpt = atlas.getPageTextureID(uvEntry.pageIndex);
    if (!textureIDOpt.has_value()) {
        ImGui::Text("Failed to get texture ID for Move tool icon.");
        return;
    }

    ImVec4 bgColor = (app.getSelectedTool() == "move") ? SelectedBackgroundColor : BackgroundColor;

    if (ImGui::ImageButton("##move-tool",
        (ImTextureRef)(size_t)textureIDOpt.value(),
        IconSize,
        uvMin,
        uvMax,
        bgColor)) {
        app.setSelectedTool("move");
    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("Move Tool\n Allow to move layers & selections around.");
    }
}


static void renderSelectionTool(Gump::Application &app) {
    auto &atlas = app.getTextureAtlas();

    std::optional<OpenGLUtils::UVEntry_t> uvEntryOpt = atlas.getUVRect("selection-tool");

    if (uvEntryOpt == std::nullopt) {
        ImGui::Text("Selection tool icon not found in texture atlas.");
        return;
    }

    OpenGLUtils::UVEntry_t uvEntry = uvEntryOpt.value();

    ImVec2 uvMin = {uvEntry.uvMin.x, uvEntry.uvMin.y};
    ImVec2 uvMax = {uvEntry.uvMax.x, uvEntry.uvMax.y};

    std::optional<GLuint> textureIDOpt = atlas.getPageTextureID(uvEntry.pageIndex);
    if (!textureIDOpt.has_value()) {
        ImGui::Text("Failed to get texture ID for selection tool icon.");
        return;
    }

    ImVec4 bgColor = (app.getSelectedTool() == "selection") ? SelectedBackgroundColor : BackgroundColor;

    if (ImGui::ImageButton("##selection-tool",
        (ImTextureRef)(size_t)textureIDOpt.value(),
        IconSize,
        uvMin,
        uvMax,
        bgColor)) {
        app.setSelectedTool("selection");
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("Selection Tool\n Allow to select and manipulate parts of the canvas.");
    }
}

static void renderFuzzySelectTool(Gump::Application &app)
{
    auto &atlas = app.getTextureAtlas();

    std::optional<OpenGLUtils::UVEntry_t> uvEntryOpt = atlas.getUVRect("fuzzy-select-tool");

    if (uvEntryOpt == std::nullopt) {
        ImGui::Text("Fuzzy select tool icon not found in texture atlas.");
        return;
    }

    OpenGLUtils::UVEntry_t uvEntry = uvEntryOpt.value();

    ImVec2 uvMin = {uvEntry.uvMin.x, uvEntry.uvMin.y};
    ImVec2 uvMax = {uvEntry.uvMax.x, uvEntry.uvMax.y};

    std::optional<GLuint> textureIDOpt = atlas.getPageTextureID(uvEntry.pageIndex);
    if (!textureIDOpt.has_value()) {
        ImGui::Text("Failed to get texture ID for fuzzy select tool icon.");
        return;
    }

    ImVec4 bgColor = (app.getSelectedTool() == "selection") ? SelectedBackgroundColor : BackgroundColor;

    if (ImGui::ImageButton("##fuzzy-select",
        (ImTextureRef)(size_t)textureIDOpt.value(),
        IconSize,
        uvMin,
        uvMax,
        bgColor)) {
        app.setSelectedTool("fuzzy-select");
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("Fuzzy Selection Tool\n Allow to select and manipulate parts of the canvas.");
    }
}
