enable_tnt_compile_flags()

include_directories(${LUAJIT_INCLUDE_DIRS})
include_directories(${MSGPUCK_INCLUDE_DIRS})

function(build_module module files)
    add_library(${module} SHARED ${files})
    target_link_libraries(${module} ${MSGPUCK_LIBRARIES})
    set_target_properties(${module} PROPERTIES PREFIX "")
    add_dependencies(${module} api)
    if(TARGET_OS_DARWIN)
        set_target_properties(${module} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    endif(TARGET_OS_DARWIN)
endfunction()


add_compile_flags("C;CXX"
    "-Wno-unused-parameter")

if(POLICY CMP0037)
    cmake_policy(SET CMP0037 OLD)
endif(POLICY CMP0037)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/small
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${PROJECT_SOURCE_DIR}/src/lib/small/test/
        ${CMAKE_CURRENT_BINARY_DIR}/small
        COMMENT Create a symlink for libsmall to fix out-of-source tests)
add_custom_target(symlink_small_tests ALL
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/small)

add_custom_target(test
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/small
    COMMAND ${PROJECT_SOURCE_DIR}/test/test-run.py
        --builddir=${PROJECT_BINARY_DIR}
        --vardir=${PROJECT_BINARY_DIR}/test/var)

add_custom_target(test-force
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/small
    COMMAND ${PROJECT_SOURCE_DIR}/test/test-run.py
        --builddir=${PROJECT_BINARY_DIR}
        --vardir=${PROJECT_BINARY_DIR}/test/var
        --force)

add_subdirectory(app)
add_subdirectory(app-tap)
add_subdirectory(box)
add_subdirectory(unit)

# Move tarantoolctl config
if (NOT ${PROJECT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
    configure_file(
        "${PROJECT_SOURCE_DIR}/test/.tarantoolctl"
        "${PROJECT_BINARY_DIR}/test/.tarantoolctl"
        )
endif()

# Disable connector_c for 1.6
#add_subdirectory(connector_c)
