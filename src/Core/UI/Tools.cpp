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

void Gump::UI::renderTools(Gump::Application &app) {
    ImGui::Begin("Tools");

    std::string selectedTool = app.getSelectedTool();

    // Move tool
    {
        bool isSelected = (selectedTool == "move");
        if (isSelected) {
            // Add thick blue border for selected tool
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.00f, 0.47f, 0.84f, 1.0f)); // Blue border
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f); // Thick border
        }
        
        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            buttonPos,
            ImVec2(buttonPos.x + IconSize.x, buttonPos.y + IconSize.y),
            IM_COL32(0, 0, 0, 255)
        );

        ImGui::BeginChild("move_tool", IconSize, true, ImGuiWindowFlags_NoScrollbar);
        renderMoveTool(app);
        ImGui::EndChild();
        
        if (isSelected) {
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
        
        if (ImGui::IsItemClicked()) {
            app.setSelectedTool("move");
        }
    }
    
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Move Tool (V)");
    }

    // Pencil tool
    {
        bool isSelected = (selectedTool == "pencil");
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.00f, 0.47f, 0.84f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
        }
        
        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            buttonPos,
            ImVec2(buttonPos.x + IconSize.x, buttonPos.y + IconSize.y),
            IM_COL32(0, 0, 0, 255)
        );

        ImGui::BeginChild("pencil_tool", IconSize, true, ImGuiWindowFlags_NoScrollbar);
        renderPencilTool(app);
        ImGui::EndChild();
        
        if (isSelected) {
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
        
        if (ImGui::IsItemClicked()) {
            app.setSelectedTool("pencil");
        }
    }
    
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Pencil Tool (B)");
    }

    // Eraser tool
    {
        bool isSelected = (selectedTool == "eraser");
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.00f, 0.47f, 0.84f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
        }
        
        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            buttonPos,
            ImVec2(buttonPos.x + IconSize.x, buttonPos.y + IconSize.y),
            IM_COL32(0, 0, 0, 255)
        );

        ImGui::BeginChild("eraser_tool", IconSize, true, ImGuiWindowFlags_NoScrollbar);
        renderEraserTool(app);
        ImGui::EndChild();
        
        if (isSelected) {
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
        
        if (ImGui::IsItemClicked()) {
            app.setSelectedTool("eraser");
        }
    }
    
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Eraser Tool (E)");
    }

    ImGui::SameLine();

    // Fill tool
    {
        bool isSelected = (selectedTool == "fill");
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.00f, 0.47f, 0.84f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
        }
        
        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            buttonPos,
            ImVec2(buttonPos.x + IconSize.x, buttonPos.y + IconSize.y),
            IM_COL32(0, 0, 0, 255)
        );

        ImGui::BeginChild("fill_tool", IconSize, true, ImGuiWindowFlags_NoScrollbar);
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImVec2(ImGui::GetCursorScreenPos().x + IconSize.x - 4, ImGui::GetCursorScreenPos().y + IconSize.y - 4),
            IM_COL32(100, 100, 100, 255)
        );
        ImGui::EndChild();
        
        if (isSelected) {
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
        
        if (ImGui::IsItemClicked()) {
            app.setSelectedTool("fill");
        }
    }
    
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Fill Tool (G)");
    }

    // Selection tool
    {
        bool isSelected = (selectedTool == "selection");
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.00f, 0.47f, 0.84f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
        }
        
        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            buttonPos,
            ImVec2(buttonPos.x + IconSize.x, buttonPos.y + IconSize.y),
            IM_COL32(0, 0, 0, 255)
        );

        ImGui::BeginChild("selection_tool", IconSize, true, ImGuiWindowFlags_NoScrollbar);
        renderSelectionTool(app);
        ImGui::EndChild();
        
        if (isSelected) {
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
        
        if (ImGui::IsItemClicked()) {
            app.setSelectedTool("selection");
        }
    }
    
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Selection Tool (M)");
    }

    ImGui::SameLine();

    // Fuzzy Select tool
    {
        bool isSelected = (selectedTool == "fuzzy_select");
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.00f, 0.47f, 0.84f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
        }
        
        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            buttonPos,
            ImVec2(buttonPos.x + IconSize.x, buttonPos.y + IconSize.y),
            IM_COL32(0, 0, 0, 255)
        );

        ImGui::BeginChild("fuzzy_select_tool", IconSize, true, ImGuiWindowFlags_NoScrollbar);
        renderFuzzySelectTool(app);
        ImGui::EndChild();
        
        if (isSelected) {
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
        
        if (ImGui::IsItemClicked()) {
            app.setSelectedTool("fuzzy_select");
        }
    }
    
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Fuzzy Select Tool (Magic Wand)");
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
