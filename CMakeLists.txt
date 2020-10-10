cmake_minimum_required(VERSION 3.15)
project(JumpAndRun)

set(CMAKE_CXX_STANDARD 17)

add_definitions("-DVFS_LARGEFILE_SUPPORT")
add_definitions("-DVFS_IGNORE_CASE")
add_definitions("-DVFS_SUPPORT_ZIP")

add_subdirectory(libs)
add_subdirectory(src)


