# ==============================
# Vendor dependencies
# ==============================

# ---- GLAD ----
add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)


# ---- IMGUI ----
file(GLOB IMGUI_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp/imgui_stdlib.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)


# ---- GLM (header-only) ----
add_library(glm-header-only INTERFACE)
target_include_directories(glm-header-only INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glm)


# ---- STB (header-only) ----
add_library(stb-header-only INTERFACE)
target_include_directories(stb-header-only INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)
