cmake_minimum_required(VERSION 3.16)
project(Gump LANGUAGES C CXX)

# -----------------------------
# C++ standard
# -----------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------
# Find packages
# -----------------------------
# Pass vcpkg toolchain when configuring if using vcpkg
# Pass -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake when running cmake on windows
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# -----------------------------
# GLAD (single C source)
# -----------------------------
add_library(glad OBJECT vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

# -----------------------------
# IMGUI (submodule)
# -----------------------------
add_library(imgui
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC vendor/imgui vendor/imgui/backends)
target_link_libraries(imgui PUBLIC glfw)

# -----------------------------
# Main executable
# -----------------------------
add_executable(gump src/main.cpp)

# Link GLAD as OBJECT library into the executable
target_sources(gump PRIVATE $<TARGET_OBJECTS:glad>)

# Link the rest
target_link_libraries(gump PRIVATE glfw imgui OpenGL::GL)

# -----------------------------
# Optional: output directories
# -----------------------------
set_target_properties(gump PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)