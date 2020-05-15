cmake_minimum_required(VERSION 3.15)
project(JumpAndRun)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)

set(BUILD_SHARED_LIBS      OFF CACHE BOOL "")
set(OPTION_BUILD_EXAMPLES  OFF CACHE BOOL "")
set(OPTION_BUILD_TESTS     OFF CACHE BOOL "")
set(OPTION_BUILD_GPU_TESTS OFF CACHE BOOL "")
set(OPTION_BUILD_TOOLS     OFF CACHE BOOL "")

add_subdirectory(libs/glfw)
add_subdirectory(libs/glbinding)
add_subdirectory(libs/resourcemanager)

add_definitions(-DGLFW_INCLUDE_NONE)

#include_directories(libs/glbinding/source/glbinding/include)
#include_directories(${CMAKE_BINARY_DIR}/libs/glbinding/source/glbinding/include)
#add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
add_library(imgui
        "libs/imgui/imgui.cpp"
        "libs/imgui/imgui_demo.cpp"
        "libs/imgui/imgui_draw.cpp"
        "libs/imgui/imgui_widgets.cpp"
        "libs/imgui/examples/imgui_impl_glfw.cpp"
        "libs/imgui/examples/imgui_impl_opengl3.cpp")

include_directories(libs/resourcemanager/include)
#include_directories(libs/glad/include)
include_directories(libs/glfw/include)
include_directories(libs/glm/glm)
include_directories(libs/imgui)

target_include_directories(imgui PRIVATE libs/glbinding/source/glbinding/include)
target_include_directories(imgui PRIVATE libs/glbinding/3rdparty/KHR/include)
target_include_directories(imgui PRIVATE ${CMAKE_BINARY_DIR}/libs/glbinding/source/glbinding/include)

file(GLOB_RECURSE all_SRCS "src/*.cpp")
file(GLOB_RECURSE all_RES "res/*.*" )

foreach(P IN LISTS all_RES)
    file(RELATIVE_PATH P_R ${PROJECT_SOURCE_DIR} ${P})
    list(APPEND all_RES_rel ${P_R})
endforeach()

rm_embed_resources(RESOURCES ${all_RES_rel})

add_executable(JumpAndRun ${all_SRCS} ${RESOURCES})

find_package(OpenGL REQUIRED)

target_link_libraries(JumpAndRun ${OPENGL_LIBRARY} glbinding glfw imgui)