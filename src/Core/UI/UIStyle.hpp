#pragma once

#include <imgui.h>
#include <string>

namespace Gump {
namespace UI {

    // Apply the professional Windows-style theme to ImGui
    void applyProfessionalTheme();

    // Helper class for scoped style modifications (like CSS classes)
    // Usage: { StyleScope red(StylePreset::ErrorText); ImGui::Text("Error!"); }
    class StyleScope {
    public:
        StyleScope(int colorCount, int varCount);
        ~StyleScope();

    private:
        int _colorCount;
        int _varCount;
    };

    // Predefined style presets that can be stacked
    namespace StylePreset {
        StyleScope ErrorText();      // Red text for errors
        StyleScope WarningText();    // Orange/yellow text for warnings
        StyleScope SuccessText();    // Green text for success
        StyleScope DisabledText();   // Dimmed text
        StyleScope PrimaryButton();  // Emphasized button style
        StyleScope DangerButton();   // Red/dangerous action button
        StyleScope SubtleBackground(); // Darker background for panels
    }

} // namespace UI
} // namespace Gump
