#include "UI/UI.hpp"
#include "UI/UIStyle.hpp"

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
static void renderPencilTool(Gump::Application &app);
static void renderEraserTool(Gump::Application &app);

void Gump::UI::renderTools(Gump::Application &app)
{
    ImGui::Begin("Tools");

    auto& shortcutMgr = app.getShortcutManager();
    auto& prefMgr = app.getPreferencesManager();
    
    // Get accent color for outline
    float r, g, b, a;
    ImVec4 accentColor = ImVec4(0.0f, 0.47f, 0.84f, 1.0f); // Default blue
    if (prefMgr.getStyleColor("Accent", r, g, b, a)) {
        accentColor = ImVec4(r, g, b, a);
    }
    
    // Helper lambda to get shortcut string for an action
    auto getShortcutText = [&](const std::string& actionId) -> std::string {
        for (const auto& shortcut : shortcutMgr.getShortcuts()) {
            if (shortcut.actionId == actionId && shortcut.key != -1) {
                return shortcut.toString();
            }
        }
        return "";
    };
    
    // Helper lambda to draw outline around selected tool
    auto drawSelectedOutline = [&]() {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImU32 outlineCol = ImGui::ColorConvertFloat4ToU32(accentColor);
        draw_list->AddRect(min, max, outlineCol, 0.0f, 0, 2.5f); // 2.5px thick outline
    };

    ImGui::Text("Drawing Tools");
    ImGui::Separator();

    // Start a table for icon grid layout
    if (ImGui::BeginTable("ToolsGrid", Columns, ImGuiTableFlags_None)) {
        
        // Pencil
        ImGui::TableNextColumn();
        bool isPencilSelected = app.getSelectedTool() == "pencil";
        ImVec4 pencilBg = isPencilSelected ? SelectedBackgroundColor : BackgroundColor;
        ImGui::PushStyleColor(ImGuiCol_Button, pencilBg);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SelectedBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, SelectedBackgroundColor);
        
        if (ImGui::Button("##Pencil", IconSize)) {
            app.setSelectedTool("pencil");
        }
        if (isPencilSelected) {
            drawSelectedOutline();
        }
        if (ImGui::IsItemHovered()) {
            std::string shortcut = getShortcutText("tool.pencil");
            if (!shortcut.empty()) {
                ImGui::SetTooltip("Pencil Tool (%s)", shortcut.c_str());
            } else {
                ImGui::SetTooltip("Pencil Tool");
            }
        }
        
        ImVec2 pencilPos = ImGui::GetItemRectMin();
        ImGui::SetCursorScreenPos(pencilPos);
        renderPencilTool(app);
        
        ImGui::PopStyleColor(3);

        // Eraser
        ImGui::TableNextColumn();
        bool isEraserSelected = app.getSelectedTool() == "eraser";
        ImVec4 eraserBg = isEraserSelected ? SelectedBackgroundColor : BackgroundColor;
        ImGui::PushStyleColor(ImGuiCol_Button, eraserBg);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SelectedBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, SelectedBackgroundColor);
        
        if (ImGui::Button("##Eraser", IconSize)) {
            app.setSelectedTool("eraser");
        }
        if (isEraserSelected) {
            drawSelectedOutline();
        }
        if (ImGui::IsItemHovered()) {
            std::string shortcut = getShortcutText("tool.eraser");
            if (!shortcut.empty()) {
                ImGui::SetTooltip("Eraser Tool (%s)", shortcut.c_str());
            } else {
                ImGui::SetTooltip("Eraser Tool");
            }
        }
        
        ImVec2 eraserPos = ImGui::GetItemRectMin();
        ImGui::SetCursorScreenPos(eraserPos);
        renderEraserTool(app);
        
        ImGui::PopStyleColor(3);

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Text("Selection Tools");
    ImGui::Separator();

    if (ImGui::BeginTable("SelectionToolsGrid", Columns, ImGuiTableFlags_None)) {
        
        // Selection
        ImGui::TableNextColumn();
        bool isSelectionSelected = app.getSelectedTool() == "selection";
        ImVec4 selectionBg = isSelectionSelected ? SelectedBackgroundColor : BackgroundColor;
        ImGui::PushStyleColor(ImGuiCol_Button, selectionBg);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SelectedBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, SelectedBackgroundColor);
        
        if (ImGui::Button("##Selection", IconSize)) {
            app.setSelectedTool("selection");
        }
        if (isSelectionSelected) {
            drawSelectedOutline();
        }
        if (ImGui::IsItemHovered()) {
            std::string shortcut = getShortcutText("tool.selection");
            if (!shortcut.empty()) {
                ImGui::SetTooltip("Selection Tool (%s)", shortcut.c_str());
            } else {
                ImGui::SetTooltip("Selection Tool");
            }
        }
        
        ImVec2 selectionPos = ImGui::GetItemRectMin();
        ImGui::SetCursorScreenPos(selectionPos);
        renderSelectionTool(app);
        
        ImGui::PopStyleColor(3);

        // Fuzzy Select
        ImGui::TableNextColumn();
        bool isFuzzySelectSelected = app.getSelectedTool() == "fuzzy_select";
        ImVec4 fuzzyBg = isFuzzySelectSelected ? SelectedBackgroundColor : BackgroundColor;
        ImGui::PushStyleColor(ImGuiCol_Button, fuzzyBg);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SelectedBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, SelectedBackgroundColor);
        
        if (ImGui::Button("##FuzzySelect", IconSize)) {
            app.setSelectedTool("fuzzy_select");
        }
        if (isFuzzySelectSelected) {
            drawSelectedOutline();
        }
        if (ImGui::IsItemHovered()) {
            std::string shortcut = getShortcutText("tool.fuzzy_select");
            if (!shortcut.empty()) {
                ImGui::SetTooltip("Fuzzy Select Tool (%s)", shortcut.c_str());
            } else {
                ImGui::SetTooltip("Fuzzy Select Tool");
            }
        }
        
        ImVec2 fuzzyPos = ImGui::GetItemRectMin();
        ImGui::SetCursorScreenPos(fuzzyPos);
        renderFuzzySelectTool(app);
        
        ImGui::PopStyleColor(3);

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Text("Navigation");
    ImGui::Separator();

    if (ImGui::BeginTable("NavigationGrid", Columns, ImGuiTableFlags_None)) {
        
        // Move
        ImGui::TableNextColumn();
        bool isMoveSelected = app.getSelectedTool() == "move";
        ImVec4 moveBg = isMoveSelected ? SelectedBackgroundColor : BackgroundColor;
        ImGui::PushStyleColor(ImGuiCol_Button, moveBg);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SelectedBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, SelectedBackgroundColor);
        
        if (ImGui::Button("##Move", IconSize)) {
            app.setSelectedTool("move");
        }
        if (isMoveSelected) {
            drawSelectedOutline();
        }
        if (ImGui::IsItemHovered()) {
            std::string shortcut = getShortcutText("tool.move");
            if (!shortcut.empty()) {
                ImGui::SetTooltip("Move Tool (%s)", shortcut.c_str());
            } else {
                ImGui::SetTooltip("Move Tool");
            }
        }
        
        ImVec2 movePos = ImGui::GetItemRectMin();
        ImGui::SetCursorScreenPos(movePos);
        renderMoveTool(app);
        
        ImGui::PopStyleColor(3);

        ImGui::EndTable();
    }

    ImGui::End();
}

static void renderSelectionTool(Gump::Application &app)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    // Draw a dashed rectangle icon
    const float thickness = 2.0f;
    const ImU32 col = IM_COL32(255, 255, 255, 255);
    
    // Draw dashed lines
    for (float i = 0; i < IconSize.x; i += 4) {
        draw_list->AddLine(ImVec2(p.x + i, p.y), ImVec2(p.x + i + 2, p.y), col, thickness);
        draw_list->AddLine(ImVec2(p.x + i, p.y + IconSize.y), ImVec2(p.x + i + 2, p.y + IconSize.y), col, thickness);
    }
    for (float i = 0; i < IconSize.y; i += 4) {
        draw_list->AddLine(ImVec2(p.x, p.y + i), ImVec2(p.x, p.y + i + 2), col, thickness);
        draw_list->AddLine(ImVec2(p.x + IconSize.x, p.y + i), ImVec2(p.x + IconSize.x, p.y + i + 2), col, thickness);
    }
}

static void renderMoveTool(Gump::Application &app)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    // Draw a cross/move icon (four arrows pointing outward)
    const float thickness = 2.0f;
    const ImU32 col = IM_COL32(255, 255, 255, 255);
    
    ImVec2 center(p.x + IconSize.x / 2, p.y + IconSize.y / 2);
    float arrowSize = IconSize.x / 3;
    
    // Vertical line
    draw_list->AddLine(ImVec2(center.x, p.y + 2), ImVec2(center.x, p.y + IconSize.y - 2), col, thickness);
    // Horizontal line
    draw_list->AddLine(ImVec2(p.x + 2, center.y), ImVec2(p.x + IconSize.x - 2, center.y), col, thickness);
    
    // Arrow heads (simplified)
    float arrowHeadSize = 4.0f;
    // Up arrow
    draw_list->AddTriangleFilled(
        ImVec2(center.x, p.y + 2),
        ImVec2(center.x - arrowHeadSize, p.y + 2 + arrowHeadSize),
        ImVec2(center.x + arrowHeadSize, p.y + 2 + arrowHeadSize),
        col
    );
    // Down arrow
    draw_list->AddTriangleFilled(
        ImVec2(center.x, p.y + IconSize.y - 2),
        ImVec2(center.x - arrowHeadSize, p.y + IconSize.y - 2 - arrowHeadSize),
        ImVec2(center.x + arrowHeadSize, p.y + IconSize.y - 2 - arrowHeadSize),
        col
    );
    // Left arrow
    draw_list->AddTriangleFilled(
        ImVec2(p.x + 2, center.y),
        ImVec2(p.x + 2 + arrowHeadSize, center.y - arrowHeadSize),
        ImVec2(p.x + 2 + arrowHeadSize, center.y + arrowHeadSize),
        col
    );
    // Right arrow
    draw_list->AddTriangleFilled(
        ImVec2(p.x + IconSize.x - 2, center.y),
        ImVec2(p.x + IconSize.x - 2 - arrowHeadSize, center.y - arrowHeadSize),
        ImVec2(p.x + IconSize.x - 2 - arrowHeadSize, center.y + arrowHeadSize),
        col
    );
}

static void renderFuzzySelectTool(Gump::Application &app)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    // Draw a magic wand icon
    const float thickness = 2.0f;
    const ImU32 col = IM_COL32(255, 255, 255, 255);
    
    // Wand stick (diagonal line)
    draw_list->AddLine(
        ImVec2(p.x + IconSize.x * 0.3f, p.y + IconSize.y * 0.7f),
        ImVec2(p.x + IconSize.x * 0.7f, p.y + IconSize.y * 0.3f),
        col, thickness
    );
    
    // Star at the tip
    ImVec2 starCenter(p.x + IconSize.x * 0.7f, p.y + IconSize.y * 0.3f);
    float starSize = 4.0f;
    
    // Draw sparkles around the wand tip
    draw_list->AddCircleFilled(starCenter, 2.0f, col);
    draw_list->AddLine(ImVec2(starCenter.x - starSize, starCenter.y), ImVec2(starCenter.x + starSize, starCenter.y), col, thickness);
    draw_list->AddLine(ImVec2(starCenter.x, starCenter.y - starSize), ImVec2(starCenter.x, starCenter.y + starSize), col, thickness);
}

static void renderPencilTool(Gump::Application &app)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    // Draw a pencil icon
    const float thickness = 2.0f;
    const ImU32 col = IM_COL32(255, 255, 255, 255);
    
    // Pencil body (diagonal rectangle)
    ImVec2 points[4] = {
        ImVec2(p.x + IconSize.x * 0.2f, p.y + IconSize.y * 0.8f),
        ImVec2(p.x + IconSize.x * 0.8f, p.y + IconSize.y * 0.2f),
        ImVec2(p.x + IconSize.x * 0.7f, p.y + IconSize.y * 0.1f),
        ImVec2(p.x + IconSize.x * 0.1f, p.y + IconSize.y * 0.7f)
    };
    
    draw_list->AddQuadFilled(points[0], points[1], points[2], points[3], IM_COL32(150, 150, 150, 255));
    draw_list->AddQuad(points[0], points[1], points[2], points[3], col, thickness);
    
    // Pencil tip (triangle)
    draw_list->AddTriangleFilled(
        ImVec2(p.x + IconSize.x * 0.1f, p.y + IconSize.y * 0.7f),
        ImVec2(p.x + IconSize.x * 0.2f, p.y + IconSize.y * 0.8f),
        ImVec2(p.x + IconSize.x * 0.05f, p.y + IconSize.y * 0.95f),
        IM_COL32(80, 80, 80, 255)
    );
}

static void renderEraserTool(Gump::Application &app)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    // Draw an eraser icon (tilted rectangle)
    const float thickness = 2.0f;
    const ImU32 col = IM_COL32(255, 255, 255, 255);
    
    // Eraser body
    ImVec2 points[4] = {
        ImVec2(p.x + IconSize.x * 0.3f, p.y + IconSize.y * 0.6f),
        ImVec2(p.x + IconSize.x * 0.7f, p.y + IconSize.y * 0.2f),
        ImVec2(p.x + IconSize.x * 0.8f, p.y + IconSize.y * 0.35f),
        ImVec2(p.x + IconSize.x * 0.4f, p.y + IconSize.y * 0.75f)
    };
    
    draw_list->AddQuadFilled(points[0], points[1], points[2], points[3], IM_COL32(200, 150, 150, 255));
    draw_list->AddQuad(points[0], points[1], points[2], points[3], col, thickness);
}
