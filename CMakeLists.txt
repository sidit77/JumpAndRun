cmake_minimum_required(VERSION 3.15)
project(JumpAndRun)

set(CMAKE_CXX_STANDARD 17)

add_library(glad "libs/glad/src/glad.c")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw)

include_directories(include)
include_directories(libs/glad/include)
include_directories(libs/glfw/include)
include_directories(libs/glm/glm)

file(GLOB all_SRCS "src/*.cpp")

add_executable(JumpAndRun main.cpp ${all_SRCS})

target_link_libraries(JumpAndRun glfw)
target_link_libraries(JumpAndRun glad)