#include "UI/UI.hpp"
#include "UI/UIStyle.hpp"

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "Logger.hpp"

#include "Application.hpp"

enum class TutorialSection {
    GettingStarted,
    Tools,
    Layers,
    Selection,
    Effects,
    FileOperations,
    Shortcuts
};

void Gump::UI::renderTutorials(Gump::Application &app) {
    LOG_DEBUG("0");
    if (!app.isTutorialsOpen()) {
        return;
    }
    LOG_DEBUG("A");

    ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    
    bool tutorialsOpen = true;
    if (!ImGui::Begin("Tutorials & Wiki", &tutorialsOpen, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }
    LOG_DEBUG("B");
    
    // If the close button was clicked, close the window
    if (!tutorialsOpen) {
        app.closeTutorials();
    }
    LOG_DEBUG("C");
    
    static TutorialSection selectedSection = TutorialSection::GettingStarted;
    static std::string searchQuery = "";
    
    // Left sidebar - Navigation
    ImGui::BeginChild("TutorialsNav", ImVec2(250, 0), true);
    
    // Search bar
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputTextWithHint("##TutorialSearch", "Search tutorials...", &searchQuery)) {
        // Filter tutorials based on search
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Navigation tree
    if (ImGui::TreeNodeEx("Getting Started", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Selectable("Welcome", selectedSection == TutorialSection::GettingStarted)) {
            selectedSection = TutorialSection::GettingStarted;
        }
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNodeEx("Core Features", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Selectable("Tools", selectedSection == TutorialSection::Tools)) {
            selectedSection = TutorialSection::Tools;
        }
        if (ImGui::Selectable("Layers", selectedSection == TutorialSection::Layers)) {
            selectedSection = TutorialSection::Layers;
        }
        if (ImGui::Selectable("Selection", selectedSection == TutorialSection::Selection)) {
            selectedSection = TutorialSection::Selection;
        }
        if (ImGui::Selectable("Effects", selectedSection == TutorialSection::Effects)) {
            selectedSection = TutorialSection::Effects;
        }
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNodeEx("Advanced", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Selectable("File Operations", selectedSection == TutorialSection::FileOperations)) {
            selectedSection = TutorialSection::FileOperations;
        }
        if (ImGui::Selectable("Keyboard Shortcuts", selectedSection == TutorialSection::Shortcuts)) {
            selectedSection = TutorialSection::Shortcuts;
        }
        ImGui::TreePop();
    }
    
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    // Right panel - Tutorial content
    ImGui::BeginChild("TutorialContent", ImVec2(0, 0), false);
    
    switch (selectedSection) {
        case TutorialSection::GettingStarted: {
            ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Welcome to Gump!");
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::TextWrapped("Gump is a powerful 2D digital painting application designed for artists and designers.");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Quick Start Guide");
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::BulletText("Create a new canvas: File → New (Ctrl+N)");
            ImGui::BulletText("Select a tool from the Tools panel on the left");
            ImGui::BulletText("Draw on the canvas using your mouse or graphics tablet");
            ImGui::BulletText("Manage layers in the Layers panel on the right");
            ImGui::BulletText("Save your work: File → Save (Ctrl+S)");
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Interface Overview");
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::TextWrapped("• Top Menu Bar: Access file operations, edit functions, and application settings");
            ImGui::TextWrapped("• Tools Panel: Select different drawing and editing tools");
            ImGui::TextWrapped("• Tool Settings: Adjust settings for the currently selected tool");
            ImGui::TextWrapped("• Brush Settings: Configure brush size, hardness, and opacity");
            ImGui::TextWrapped("• Layers Panel: Manage your image layers");
            ImGui::TextWrapped("• Canvas: Your main workspace for creating artwork");
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.0f, 1.0f), "Tip:");
            ImGui::TextWrapped("Press F1 at any time to open this help window. Use Ctrl+, to access preferences.");
            
            break;
        }
        
        case TutorialSection::Tools: {
            ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Drawing & Editing Tools");
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Pencil Tool (B)");
            ImGui::TextWrapped("Draw freehand strokes with customizable brush settings. Click and drag to paint on the canvas.");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Eraser Tool (E)");
            ImGui::TextWrapped("Remove pixels from the current layer. Functions like the pencil tool but erases instead of drawing.");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Selection Tool (M)");
            ImGui::TextWrapped("Create rectangular selections by clicking and dragging. Use selections to:");
            ImGui::BulletText("Copy, cut, or paste selected areas");
            ImGui::BulletText("Apply effects to specific regions");
            ImGui::BulletText("Move selected content around the canvas");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Move Tool (V)");
            ImGui::TextWrapped("Navigate around your canvas. Click and drag to pan, use mouse wheel to zoom in and out.");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Fuzzy Select Tool (W)");
            ImGui::TextWrapped("Select areas based on color similarity. Click on a color to select all similar pixels.");
            ImGui::BulletText("Tolerance: Adjust how similar colors need to be");
            ImGui::BulletText("Contiguous: Only select connected pixels");
            ImGui::BulletText("Anti-alias: Smooth selection edges");
            ImGui::Spacing();
            
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.0f, 1.0f), "Tip:");
            ImGui::TextWrapped("Most tools have keyboard shortcuts shown in parentheses. You can customize these in Edit → Preferences → Shortcuts.");
            
            break;
        }
        
        case TutorialSection::Layers: {
            ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Working with Layers");
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::TextWrapped("Layers allow you to work on different parts of your image independently without affecting other parts.");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Creating Layers");
            ImGui::TextWrapped("• Click the '+' button in the Layers panel to create a new empty layer");
            ImGui::TextWrapped("• Import an image to automatically create a layer: Image → Import");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Layer Operations");
            ImGui::BulletText("Visibility: Toggle the eye icon to show/hide a layer");
            ImGui::BulletText("Opacity: Adjust the transparency slider to control layer transparency");
            ImGui::BulletText("Rename: Double-click the layer name to rename it");
            ImGui::BulletText("Delete: Click the trash icon to remove a layer");
            ImGui::BulletText("Reorder: Use arrow buttons to move layers up or down");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Layer Tips");
            ImGui::BulletText("The top layer in the list appears on top in your image");
            ImGui::BulletText("You always draw on the topmost visible layer");
            ImGui::BulletText("Use layers to separate different elements of your artwork");
            ImGui::BulletText("Keep a background layer separate from your main artwork");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.0f, 1.0f), "Tip:");
            ImGui::TextWrapped("Give your layers descriptive names to stay organized as your project grows!");
            
            break;
        }
        
        case TutorialSection::Selection: {
            ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Selections & Masks");
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::TextWrapped("Selections let you work on specific areas of your image while protecting the rest.");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Making Selections");
            ImGui::BulletText("Rectangle: Use the Selection Tool (M) and drag to select");
            ImGui::BulletText("Fuzzy Select: Use the Fuzzy Select Tool (W) to select by color");
            ImGui::BulletText("Select All: Edit → Select All (Ctrl+A)");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Working with Selections");
            ImGui::BulletText("Copy: Edit → Copy (Ctrl+C) - copies selected pixels");
            ImGui::BulletText("Cut: Edit → Cut (Ctrl+X) - copies and removes selected pixels");
            ImGui::BulletText("Paste: Edit → Paste (Ctrl+V) - creates a new layer with copied content");
            ImGui::BulletText("Deselect: Edit → Deselect (Ctrl+D) - removes the selection");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Moving Selections");
            ImGui::TextWrapped("With an active selection, hold Shift and drag to move the selection border without moving the pixels.");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.0f, 1.0f), "Tip:");
            ImGui::TextWrapped("The marching ants animation shows your active selection boundary.");
            
            break;
        }
        
        case TutorialSection::Effects: {
            ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Image Effects");
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::TextWrapped("Apply various effects to your selections or entire layers.");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Available Effects");
            ImGui::Spacing();
            
            ImGui::BulletText("Invert: Inverts the colors of the selected area");
            ImGui::BulletText("Blur: Applies a blur effect to soften edges");
            ImGui::BulletText("Color Filter: Adjusts color channels and saturation");
            ImGui::BulletText("Noise: Adds random noise for texture effects");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Applying Effects");
            ImGui::TextWrapped("1. Make a selection or select the entire layer (Ctrl+A)");
            ImGui::TextWrapped("2. Open the Effects panel");
            ImGui::TextWrapped("3. Choose an effect and adjust its parameters");
            ImGui::TextWrapped("4. Click 'Apply' to permanently apply the effect");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.0f, 1.0f), "Tip:");
            ImGui::TextWrapped("Some effects support real-time preview. Hover over an effect to see it before applying.");
            
            break;
        }
        
        case TutorialSection::FileOperations: {
            ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "File Management");
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Project Files (.gump)");
            ImGui::TextWrapped("Gump projects are saved as .gump files which preserve:");
            ImGui::BulletText("All layers with their properties (opacity, visibility, position)");
            ImGui::BulletText("Canvas size and settings");
            ImGui::BulletText("Layer ordering and names");
            ImGui::Spacing();
            
            ImGui::TextWrapped("• Save: File → Save (Ctrl+S) - saves to current file");
            ImGui::TextWrapped("• Save As: File → Save As (Ctrl+Shift+S) - saves to a new file");
            ImGui::TextWrapped("• Load: File → Load Project (Ctrl+L) - opens a saved project");
            ImGui::Spacing();
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Importing & Exporting");
            ImGui::TextWrapped("• Import: Image → Import (Ctrl+I) - imports an image as a new layer");
            ImGui::TextWrapped("• Export: File → Export (Ctrl+E) - exports flattened image as PNG/JPG");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.0f, 1.0f), "Note:");
            ImGui::TextWrapped(".gump files are compressed archives containing all your layers as separate PNG files plus project metadata.");
            
            break;
        }
        
        case TutorialSection::Shortcuts: {
            ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Keyboard Shortcuts");
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::TextWrapped("Speed up your workflow with keyboard shortcuts. All shortcuts can be customized in Edit → Preferences → Shortcuts.");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "File Operations");
            ImGui::BulletText("Ctrl+N: New file");
            ImGui::BulletText("Ctrl+O: Open file");
            ImGui::BulletText("Ctrl+S: Save");
            ImGui::BulletText("Ctrl+Shift+S: Save As");
            ImGui::BulletText("Ctrl+E: Export");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Edit Operations");
            ImGui::BulletText("Ctrl+Z: Undo");
            ImGui::BulletText("Ctrl+Y: Redo");
            ImGui::BulletText("Ctrl+X: Cut");
            ImGui::BulletText("Ctrl+C: Copy");
            ImGui::BulletText("Ctrl+V: Paste");
            ImGui::BulletText("Ctrl+A: Select All");
            ImGui::BulletText("Ctrl+D: Deselect");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Tools");
            ImGui::BulletText("B: Pencil Tool");
            ImGui::BulletText("E: Eraser Tool");
            ImGui::BulletText("M: Selection Tool");
            ImGui::BulletText("V: Move Tool");
            ImGui::BulletText("W: Fuzzy Select Tool");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "View");
            ImGui::BulletText("Ctrl++: Zoom In");
            ImGui::BulletText("Ctrl+-: Zoom Out");
            ImGui::BulletText("Ctrl+0: Reset Zoom");
            ImGui::BulletText("Middle Mouse: Pan Canvas");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Other");
            ImGui::BulletText("F1: Open Tutorials");
            ImGui::BulletText("Ctrl+,: Open Preferences");
            
            break;
        }
    }
    
    ImGui::EndChild();
    
    ImGui::End();
}
