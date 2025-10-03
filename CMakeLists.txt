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
# Vendor libraries
# -----------------------------
add_subdirectory(vendor)

# -----------------------------
# Main executable
# -----------------------------
add_executable(gump src/main.cpp)

# Link GLAD as OBJECT library into the executable
target_sources(gump PRIVATE $<TARGET_OBJECTS:glad>)

# Link the rest
target_link_libraries(gump PRIVATE glfw imgui OpenGL::GL glm-header-only)

# -----------------------------
# Output directories
# -----------------------------
set_target_properties(gump PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)