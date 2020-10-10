cmake_minimum_required(VERSION 3.15)
project(JumpAndRun)

set(CMAKE_CXX_STANDARD 17)

add_definitions("-DVFS_LARGEFILE_SUPPORT")
add_definitions("-DVFS_IGNORE_CASE")
add_definitions("-DVFS_SUPPORT_ZIP")

add_subdirectory(libs)

file(GLOB_RECURSE all_SRCS "src/*.cpp")
file(GLOB_RECURSE all_SCHEME "src/*.fbs")

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

add_executable(JumpAndRun ${all_SRCS})

#if(MSVC)
#    set_target_properties(JumpAndRun PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
#endif()

add_dependencies(JumpAndRun schema)

target_link_libraries(JumpAndRun glclasses glfw glad imgui creature-runtime flatbuffers ttvfs ttvfs_zip tinytoml cpp-colors glm stb)

set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
        DEPENDS ${destination}
        COMMENT "symbolic link resources folder from ${source} => ${destination}"
)