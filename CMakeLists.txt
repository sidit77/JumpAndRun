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
file(GLOB_RECURSE all_SCHEME "src/*.fbs")
file(GLOB_RECURSE all_RES "res/*.*" )

foreach(P IN LISTS all_RES)
    file(RELATIVE_PATH P_R ${PROJECT_SOURCE_DIR} ${P})
    list(APPEND all_RES_rel ${P_R})
endforeach()
rm_embed_resources(RESOURCES ${all_RES_rel})

if(FLATBUFFERS_FLATC_EXECUTABLE)
    set(FLATC_TARGET "")
    set(FLATC ${FLATBUFFERS_FLATC_EXECUTABLE})
else()
    set(FLATC_TARGET flatc)
    set(FLATC flatc)
endif()
set(FLATC_ARGS --gen-object-api)
set(generated_includes_dir "${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build/include")
foreach(schema ${all_SCHEME})
    get_filename_component(filename ${schema} NAME_WE)
    set(generated_include ${generated_includes_dir}/${filename}_generated.h)
    add_custom_command(
            OUTPUT ${generated_include}
            COMMAND ${FLATC} ${FLATC_ARGS}
            -o ${generated_includes_dir}
            -c ${schema}
            DEPENDS ${FLATC_TARGET} ${schema}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    list(APPEND all_generated_files ${generated_include})
endforeach()
add_custom_target(schema DEPENDS ${all_generated_files})
include_directories(${generated_includes_dir})
set_property(TARGET schema PROPERTY GENERATED_INCLUDES_DIR ${generated_includes_dir})

add_executable(JumpAndRun ${all_SRCS} ${RESOURCES})

if(MSVC)
    set_target_properties(JumpAndRun PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
endif()

add_dependencies(JumpAndRun schema)

target_link_libraries(JumpAndRun glfw glad imgui gason creature-runtime flatbuffers)

set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
        DEPENDS ${destination}
        COMMENT "symbolic link resources folder from ${source} => ${destination}"
)