#include "UI/UI.hpp"

#include <imgui/imgui.h>

#include "Application.hpp"

void Gump::UI::renderDebugInfo(Gump::Application &app) {
    ImGui::Begin("Debug Info");

    // FPS counter
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / io.Framerate);

    ImGui::Separator();

    // Performance timings (we'll add these to Application)
    auto& timings = app.getPerformanceTimings();

    ImGui::Text("Performance Timings:");
    ImGui::Text("  Update: %.3f ms", timings.updateTime * 1000.0f);
    ImGui::Text("  Render: %.3f ms", timings.renderTime * 1000.0f);
    ImGui::Text("  UI: %.3f ms", timings.uiTime * 1000.0f);
    ImGui::Text("  Total: %.3f ms", timings.totalFrameTime * 1000.0f);

    ImGui::Separator();

    // Memory info
    ImGui::Text("Active Layers: %zu", app.getLayerCount());
    ImGui::Text("Active Stroke: %s", app.hasActiveStroke() ? "Yes" : "No");
    if (app.hasActiveStroke()) {
        auto& stroke = app.getCurrentStroke();
        if (stroke) {
            ImGui::Text("  Stroke Points: %zu", stroke->getPointCount());
        }
    }

    ImGui::Separator();

    // Camera info
    auto& cam = app.getCamera();
    ImGui::Text("Camera Zoom: %.2f", cam.getZoom());

    ImGui::End();
}
