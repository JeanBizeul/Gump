cmake_minimum_required(VERSION 3.16)
project(Gump LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)

# --- Dependencies ---
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(libzip REQUIRED)

# On Linux, we need X11 for GLFW/ImGui
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
endif()

# --- Subdirectories ---
add_subdirectory(vendor)
add_subdirectory(src)

# --- Application executable ---
file(GLOB APP_FILES CONFIGURE_DEPENDS app/*.cpp app/*.c)
add_executable(gump ${APP_FILES})

target_include_directories(gump PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/vendor
)

# Link in correct dependency order
target_link_libraries(gump PRIVATE
    core            # Your main library
    opengl-utils    # Depends on glad, imgui, etc.
    logger
    imgui
    glad
    glfw
    OpenGL::GL
    libzip::zip
)

# Add X11 on Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(gump PRIVATE X11::X11)
endif()

set_target_properties(gump PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)
