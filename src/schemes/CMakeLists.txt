file(GLOB all_SCHEME "*.fbs")

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
set_property(TARGET schema PROPERTY GENERATED_INCLUDES_DIR ${generated_includes_dir})

add_library(fb-schemes INTERFACE)
target_include_directories(fb-schemes INTERFACE ${generated_includes_dir})
target_link_libraries(fb-schemes INTERFACE flatbuffers)
add_dependencies(fb-schemes schema)
