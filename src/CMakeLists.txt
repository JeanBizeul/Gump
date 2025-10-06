# Logger header-only
add_library(logger INTERFACE)
target_include_directories(logger INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Logger)

# OpenGLUtils static lib
file(GLOB OPENGL_UTILS_SRC CONFIGURE_DEPENDS OpenGLUtils/*.cpp)
add_library(opengl-utils STATIC ${OPENGL_UTILS_SRC})
target_include_directories(opengl-utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenGLUtils
    ${PROJECT_SOURCE_DIR}/vendor
    ${PROJECT_SOURCE_DIR}/vendor/imgui
    ${PROJECT_SOURCE_DIR}/vendor/imgui/backends
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(opengl-utils
    PUBLIC logger OpenGL::GL glm-header-only stb-header-only glad glfw
)

# Core library
file(GLOB CORE_SRC CONFIGURE_DEPENDS Core/*.cpp)
add_library(core STATIC ${CORE_SRC})
# THIS LINE SHOULD REFERENCE CORE, NOT OPENGL-UTILS
target_include_directories(core PUBLIC
    ${PROJECT_SOURCE_DIR}/Core
)
# Core depends on opengl-utils (and transitively gets all includes)
target_link_libraries(core
    PRIVATE opengl-utils
)
