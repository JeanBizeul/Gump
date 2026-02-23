#include "UI/UIStyle.hpp"
#include <imgui.h>

namespace Gump {
namespace UI {

void applyProfessionalTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Professional Windows-like color scheme
    // Grays and subtle blues with black text
    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f); // Black text
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // Gray disabled text
    colors[ImGuiCol_WindowBg]               = ImVec4(0.94f, 0.94f, 0.94f, 1.00f); // Light gray window background
    colors[ImGuiCol_ChildBg]                = ImVec4(0.94f, 0.94f, 0.94f, 1.00f); // Same as window
    colors[ImGuiCol_PopupBg]                = ImVec4(0.94f, 0.94f, 0.94f, 0.98f); // Popup background
    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.30f); // Black borders with transparency
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // No shadow
    colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); // White input backgrounds
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // Light gray hover
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.85f, 0.85f, 0.85f, 1.00f); // Darker gray active
    colors[ImGuiCol_TitleBg]                = ImVec4(0.80f, 0.80f, 0.80f, 1.00f); // Gray title bar
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.00f, 0.47f, 0.84f, 1.00f); // Blue active title bar (Windows accent)
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.88f, 0.88f, 0.88f, 1.00f); // Light gray collapsed
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f); // Menu bar gray
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.94f, 0.94f, 0.94f, 1.00f); // Scrollbar background
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.69f, 0.69f, 0.69f, 1.00f); // Scrollbar grab
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f); // Darker on hover
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.39f, 0.39f, 0.39f, 1.00f); // Even darker when active
    colors[ImGuiCol_CheckMark]              = ImVec4(0.00f, 0.47f, 0.84f, 1.00f); // Blue checkmark (Windows accent)
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.00f, 0.47f, 0.84f, 1.00f); // Blue slider
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.00f, 0.37f, 0.67f, 1.00f); // Darker blue when active
    colors[ImGuiCol_Button]                 = ImVec4(0.88f, 0.88f, 0.88f, 1.00f); // Light gray button
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.78f, 0.78f, 0.78f, 1.00f); // Darker gray on hover
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.68f, 0.68f, 0.68f, 1.00f); // Even darker when pressed
    colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.47f, 0.84f, 0.31f); // Blue header with transparency
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.47f, 0.84f, 0.47f); // More opaque on hover
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.00f, 0.47f, 0.84f, 0.67f); // Even more opaque when active
    colors[ImGuiCol_Separator]              = ImVec4(0.00f, 0.00f, 0.00f, 0.30f); // Black separator
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.00f, 0.47f, 0.84f, 0.78f); // Blue on hover
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.00f, 0.47f, 0.84f, 1.00f); // Full blue when active
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.00f, 0.47f, 0.84f, 0.20f); // Subtle blue resize grip
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.00f, 0.47f, 0.84f, 0.47f); // More visible on hover
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.00f, 0.47f, 0.84f, 0.67f); // Even more visible when active
    colors[ImGuiCol_Tab]                    = ImVec4(0.88f, 0.88f, 0.88f, 1.00f); // Light gray tabs
    colors[ImGuiCol_TabHovered]             = ImVec4(0.00f, 0.47f, 0.84f, 0.80f); // Blue on hover
    colors[ImGuiCol_TabActive]              = ImVec4(0.94f, 0.94f, 0.94f, 1.00f); // Match window background
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.88f, 0.88f, 0.88f, 1.00f); // Gray when unfocused
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // Slightly lighter
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.00f, 0.47f, 0.84f, 0.70f); // Blue docking preview
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // Dark empty docking background
    colors[ImGuiCol_PlotLines]              = ImVec4(0.00f, 0.47f, 0.84f, 1.00f); // Blue plot lines
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f); // Orange on hover
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.00f, 0.47f, 0.84f, 1.00f); // Blue histogram
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f); // Orange on hover
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.78f, 0.78f, 0.78f, 1.00f); // Gray table header
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.50f); // Strong black borders
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.00f, 0.00f, 0.00f, 0.20f); // Light black borders
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // Transparent row background
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.00f, 0.00f, 0.00f, 0.06f); // Subtle alternating row
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 0.47f, 0.84f, 0.35f); // Blue text selection
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f); // Yellow drag-drop target
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.00f, 0.47f, 0.84f, 1.00f); // Blue navigation highlight
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f); // White windowing highlight
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f); // Gray dim background
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f); // Dark modal dim

    // Style adjustments for a professional look
    style.WindowPadding     = ImVec2(8.0f, 8.0f);
    style.FramePadding      = ImVec2(4.0f, 3.0f);
    style.ItemSpacing       = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing  = ImVec2(4.0f, 4.0f);
    style.IndentSpacing     = 21.0f;
    style.ScrollbarSize     = 16.0f;
    style.GrabMinSize       = 10.0f;

    // Border and rounding
    style.WindowBorderSize  = 1.0f;
    style.ChildBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.TabBorderSize     = 0.0f;

    // Rounding (minimal for professional look)
    style.WindowRounding    = 0.0f;
    style.ChildRounding     = 0.0f;
    style.FrameRounding     = 0.0f;
    style.PopupRounding     = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding      = 0.0f;
    style.TabRounding       = 0.0f;

    // Alignment
    style.WindowTitleAlign  = ImVec2(0.0f, 0.5f);
    style.ButtonTextAlign   = ImVec2(0.5f, 0.5f);
}

// StyleScope implementation
StyleScope::StyleScope(int colorCount, int varCount) 
    : _colorCount(colorCount), _varCount(varCount) {}

StyleScope::~StyleScope() {
    if (_colorCount > 0) ImGui::PopStyleColor(_colorCount);
    if (_varCount > 0) ImGui::PopStyleVar(_varCount);
}

// Predefined style presets
namespace StylePreset {

StyleScope ErrorText() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.1f, 0.1f, 1.0f)); // Red
    return StyleScope(1, 0);
}

StyleScope WarningText() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.0f, 1.0f)); // Orange
    return StyleScope(1, 0);
}

StyleScope SuccessText() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.2f, 1.0f)); // Green
    return StyleScope(1, 0);
}

StyleScope DisabledText() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
    return StyleScope(1, 0);
}

StyleScope PrimaryButton() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.47f, 0.84f, 1.0f)); // Blue button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.00f, 0.37f, 0.67f, 1.0f)); // Darker blue
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.00f, 0.27f, 0.50f, 1.0f)); // Even darker
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White text
    return StyleScope(4, 0);
}

StyleScope DangerButton() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f)); // Red button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f)); // Lighter red
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f)); // Darker red
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White text
    return StyleScope(4, 0);
}

StyleScope SubtleBackground() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.88f, 0.88f, 0.88f, 1.0f)); // Slightly darker gray
    return StyleScope(1, 0);
}

} // namespace StylePreset

} // namespace UI
} // namespace Gump
