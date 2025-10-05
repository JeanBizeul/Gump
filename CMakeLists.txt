cmake_minimum_required(VERSION 3.16)
project(Gump LANGUAGES C CXX)

# -----------------------------
# C++ standard
# -----------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 17)
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
file(GLOB SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.c)
add_executable(gump
    # Add all source files in src/ using glob
    ${SRC_FILES}
)

# Include src/ for main executable
target_include_directories(gump PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/vendor)

# Link GLAD as OBJECT library into the executable
target_sources(gump PRIVATE $<TARGET_OBJECTS:glad>)

# Link the rest
target_link_libraries(gump PRIVATE glfw imgui OpenGL::GL glm-header-only stb-header-only)

# -----------------------------
# Output directories
# -----------------------------
set_target_properties(gump PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)