cmake_minimum_required(VERSION 3.16)
project(Gump LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_subdirectory(vendor)
add_subdirectory(src)

file(GLOB APP_FILES CONFIGURE_DEPENDS app/*.cpp app/*.c)
add_executable(gump ${APP_FILES})

target_include_directories(gump PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/vendor)

target_link_libraries(gump PRIVATE 
    glfw
    imgui
    OpenGL::GL
    glad
    opengl-utils
    logger
)

set_target_properties(gump PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)
