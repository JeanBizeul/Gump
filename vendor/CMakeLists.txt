# GLAD
add_library(glad OBJECT glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# IMGUI
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
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

# GLM (header-only)
add_library(glm-header-only INTERFACE)
target_include_directories(glm-header-only INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glm)

# STB (header-only)
add_library(stb-header-only INTERFACE)
target_include_directories(stb-header-only INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)
