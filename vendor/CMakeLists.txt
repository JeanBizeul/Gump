# -----------------------------
# GLAD (single C source)
# -----------------------------
add_library(glad OBJECT glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# -----------------------------
# IMGUI (submodule)
# -----------------------------
add_library(imgui
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC imgui imgui/backends)
target_link_libraries(imgui PUBLIC glfw)

# -----------------------------
# GLM (header-only)
# -----------------------------
add_subdirectory(glm)