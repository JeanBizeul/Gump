#include "UI/UI.hpp"

#include <imgui_internal.h>

#include "Logger.hpp"

void Gump::UI::setupDockLayout(int dockspaceId)
{
    ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->Size);

    ImGuiID dockIdMain = dockspaceId;

    // Left dock
    ImGuiID dockIdLeft = 0;
    ImGui::DockBuilderSplitNode(dockIdMain, ImGuiDir_Left, 0.25f, &dockIdLeft, &dockIdMain);

    ImGuiID dockIdTopLeft = 0;
    ImGui::DockBuilderSplitNode(dockIdLeft, ImGuiDir_Up, 0.20f, &dockIdTopLeft, &dockIdLeft);
    ImGui::DockBuilderDockWindow("Tools", dockIdTopLeft);

    ImGuiID dockIdBottomLeft = 0;
    ImGui::DockBuilderSplitNode(dockIdLeft, ImGuiDir_Down, 0.80f, &dockIdBottomLeft, &dockIdLeft);
    ImGui::DockBuilderDockWindow("Tool Settings", dockIdBottomLeft);

    // Right dock
    ImGuiID dockIdRight = 0;
    ImGui::DockBuilderSplitNode(dockIdMain, ImGuiDir_Right, 0.33f, &dockIdRight, &dockIdMain);

    ImGuiID dockIdTopRight = 0;
    ImGui::DockBuilderSplitNode(dockIdRight, ImGuiDir_Up, 0.65f, &dockIdTopRight, &dockIdRight);
    ImGui::DockBuilderDockWindow("Brush Settings", dockIdTopRight);

    ImGuiID dockIdBottomRight = 0;
    ImGui::DockBuilderSplitNode(dockIdRight, ImGuiDir_Down, 0.35f, &dockIdBottomRight, &dockIdRight);
    ImGui::DockBuilderDockWindow("Layers", dockIdBottomRight);

    ImGui::DockBuilderFinish(dockspaceId);
}
