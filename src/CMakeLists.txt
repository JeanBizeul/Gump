# ==============================
# Source-level libraries
# ==============================

# ---- Logger (header-only) ----
add_library(logger INTERFACE)
target_include_directories(logger INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Logger)


# ---- OpenGLUtils ----
file(GLOB OPENGL_UTILS_SRC CONFIGURE_DEPENDS OpenGLUtils/*.cpp)

add_library(opengl-utils STATIC ${OPENGL_UTILS_SRC})

target_include_directories(opengl-utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenGLUtils
    ${PROJECT_SOURCE_DIR}/vendor
    ${PROJECT_SOURCE_DIR}/vendor/imgui
    ${PROJECT_SOURCE_DIR}/vendor/imgui/backends
)

target_link_libraries(opengl-utils
    PUBLIC
        logger
        glad
        imgui
        glfw
        glm-header-only
        stb-header-only
        OpenGL::GL
)

# ---- Core ----
file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS Core/*.cpp)
add_library(core STATIC ${CORE_SRC})

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
)

target_link_libraries(core PUBLIC opengl-utils)
