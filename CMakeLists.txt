cmake_minimum_required(VERSION 3.15)
project(JumpAndRun)

set(CMAKE_CXX_STANDARD 17)

add_library(glad "libs/glad/src/glad.c")
add_library(imgui
        "libs/imgui/imgui.cpp"
        "libs/imgui/imgui_demo.cpp"
        "libs/imgui/imgui_draw.cpp"
        "libs/imgui/imgui_widgets.cpp"
        "libs/imgui/examples/imgui_impl_glfw.cpp"
        "libs/imgui/examples/imgui_impl_opengl3.cpp")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw)
add_subdirectory(libs/resourcemanager)

include_directories(libs/resourcemanager/include)
include_directories(libs/glad/include)
include_directories(libs/glfw/include)
include_directories(libs/glm/glm)
include_directories(libs/imgui)

file(GLOB_RECURSE all_SRCS "src/*.cpp")
file(GLOB_RECURSE all_RES "res/*.*" )

foreach(P IN LISTS all_RES)
    file(RELATIVE_PATH P_R ${PROJECT_SOURCE_DIR} ${P})
    list(APPEND all_RES_rel ${P_R})
endforeach()

rm_embed_resources(RESOURCES ${all_RES_rel})

add_executable(JumpAndRun ${all_SRCS} ${RESOURCES})

target_link_libraries(JumpAndRun glfw)
target_link_libraries(JumpAndRun glad)
target_link_libraries(JumpAndRun imgui)