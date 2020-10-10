set(CMAKE_CXX_STANDARD 17)

add_subdirectory(schemes)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../bin")

file(GLOB_RECURSE all_SRCS "*.cpp")

add_executable(JumpAndRun ${all_SRCS})

target_link_libraries(JumpAndRun fb-schemes glclasses glfw glad imgui creature-runtime ttvfs ttvfs_zip tinytoml cpp-colors glm stb)

#if(MSVC)
#    set_target_properties(JumpAndRun PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
#endif()

set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
add_custom_command(
        TARGET JumpAndRun POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
        DEPENDS ${destination}
        COMMENT "symbolic link resources folder from ${source} => ${destination}"
)