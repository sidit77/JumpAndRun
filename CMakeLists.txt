cmake_minimum_required(VERSION 3.15)
project(JumpAndRun)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)

set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FLATBUFFERS_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw)
add_subdirectory(libs/resourcemanager)
add_subdirectory(libs/flatbuffers ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build EXCLUDE_FROM_ALL)

add_compile_definitions(_USE_MATH_DEFINES)
file(COPY "libs/creature-runtime/MeshBone.h" "libs/creature-runtime/CreatureModule.h" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/libs/creature-runtime/include/")

include_directories(${CMAKE_CURRENT_BINARY_DIR}/libs/creature-runtime/include/)
include_directories(libs/resourcemanager/include)
include_directories(libs/glad/include)
include_directories(libs/glfw/include)
include_directories(libs/glm/glm)
include_directories(libs/imgui)
include_directories(libs/creature-runtime/gason)
include_directories(libs/stb)
include_directories(libs/tinytoml/include)
include_directories(libs/cpp-colors/include)


add_library(glad "libs/glad/src/glad.c")
add_library(gason "libs/creature-runtime/gason/gason.cpp")
add_library(creature-runtime
        "libs/creature-runtime/MeshBone.cpp"
        "libs/creature-runtime/CreatureModule.cpp")
add_library(imgui
        "libs/imgui/imgui.cpp"
        "libs/imgui/imgui_demo.cpp"
        "libs/imgui/imgui_draw.cpp"
        "libs/imgui/imgui_widgets.cpp"
        "libs/imgui/examples/imgui_impl_glfw.cpp"
        "libs/imgui/examples/imgui_impl_opengl3.cpp")

file(GLOB_RECURSE all_SRCS "src/*.cpp")
file(GLOB_RECURSE all_RES "res/*.*" )

foreach(P IN LISTS all_RES)
    file(RELATIVE_PATH P_R ${PROJECT_SOURCE_DIR} ${P})
    list(APPEND all_RES_rel ${P_R})
endforeach()
rm_embed_resources(RESOURCES ${all_RES_rel})

add_executable(JumpAndRun ${all_SRCS} ${RESOURCES})

target_link_libraries(JumpAndRun glfw glad imgui gason creature-runtime flatbuffers)

set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
        DEPENDS ${destination}
        COMMENT "symbolic link resources folder from ${source} => ${destination}"
)