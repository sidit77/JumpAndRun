cmake_minimum_required(VERSION 3.15)
project(JumpAndRun C)

set(CMAKE_C_STANDARD 11)

add_library(glad "libs/glad/src/glad.c")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw)

include_directories(include)
include_directories(libs/glad/include)
include_directories(libs/glfw/include)

file(GLOB all_SRCS "src/*.c")

add_executable(JumpAndRun main.c ${all_SRCS})

target_link_libraries(JumpAndRun glfw)
target_link_libraries(JumpAndRun glad)