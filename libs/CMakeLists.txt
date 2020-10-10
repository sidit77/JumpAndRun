set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)
add_subdirectory(glfw)

add_subdirectory(glclasses)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "glm/glm")

add_library(imgui
        "imgui/imgui.cpp"
        "imgui/imgui_demo.cpp"
        "imgui/imgui_draw.cpp"
        "imgui/imgui_widgets.cpp"
        "imgui/examples/imgui_impl_glfw.cpp"
        "imgui/examples/imgui_impl_opengl3.cpp")
target_include_directories(imgui PUBLIC "imgui")
target_link_libraries(imgui glad glfw)

set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FLATBUFFERS_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(flatbuffers ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build EXCLUDE_FROM_ALL)


#add_definitions("-DVFS_LARGEFILE_SUPPORT")
#add_definitions("-DVFS_IGNORE_CASE")
#add_definitions("-DVFS_SUPPORT_ZIP")
#if(NOT MSVC)
#    if(NOT CMAKE_BUILD_TYPE)
#        set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
#    endif()
#endif()
#add_subdirectory(ttvfs/ttvfs)
#add_subdirectory(ttvfs/ttvfs_zip)

set(CMAKE_POLICY_DEFAULT_CMP0054 NEW)
set(TTVFS_BUILD_CFILEAPI OFF CACHE BOOL "Build C-style API wrapper" FORCE)
add_subdirectory(ttvfs)
message("${TTVFS_INCLUDE_DIRS}")
target_include_directories(ttvfs PUBLIC "${TTVFS_INCLUDE_DIRS}")

add_library(gason "creature-runtime/gason/gason.cpp")
target_include_directories(gason PUBLIC "creature-runtime/gason")

file(COPY "creature-runtime/MeshBone.h" "creature-runtime/CreatureModule.h" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/libs/creature-runtime/include/")
add_library(creature-runtime
        "creature-runtime/MeshBone.cpp"
        "creature-runtime/CreatureModule.cpp")
target_compile_definitions(creature-runtime PRIVATE _USE_MATH_DEFINES)
target_include_directories(creature-runtime PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/libs/creature-runtime/include)
target_link_libraries(creature-runtime gason glm)

add_library(tinytoml INTERFACE)
target_include_directories(tinytoml INTERFACE "tinytoml/include")

add_library(cpp-colors INTERFACE)
target_include_directories(cpp-colors INTERFACE "cpp-colors/include")