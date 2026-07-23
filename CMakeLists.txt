cmake_minimum_required(VERSION 3.20)
project(gpo VERSION 0.0.1)


set( GPO_VERSION_ABI 1 )
set( GPO_DESCRIPTION "GpO Base Project" )
set( GPO_LICENSE LGPL )


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
 message( FATAL "no in source building allowed." )
endif( )


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC ")



include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND  ON  CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11      ON CACHE BOOL "" FORCE)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)

# GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v0.1.36
    GIT_SHALLOW    TRUE
)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.6
    GIT_SHALLOW    TRUE
)


FetchContent_MakeAvailable(glfw glm glad imgui)



set(OPENGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# ImGui as static lib
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_lib PUBLIC glfw OpenGL::GL)

# main bin
add_executable(gpo_proyecto_final
    include/stb/stb_image.h
    src/GPO_proyecto_final.cpp
    src/GPO_aux.cpp
    src/GPO_gui.cpp
    src/GPO_postprocessing.cpp
)

target_include_directories(gpo_proyecto_final PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/khr
    ${PROJECT_SOURCE_DIR}/include/stb
)

target_compile_definitions(gpo_proyecto_final PRIVATE
    GLM_ENABLE_EXPERIMENTAL
)

target_link_libraries(gpo_proyecto_final PRIVATE
    OpenGL::GL
    glfw
    glm
    glad
    imgui_lib
)

set_target_properties(gpo_proyecto_final PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET gpo_proyecto_final POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:gpo_proyecto_final>/data"
    COMMENT "Copiando assets a bin/data..."
)
