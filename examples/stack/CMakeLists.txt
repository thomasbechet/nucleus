cmake_minimum_required(VERSION 3.5.0)

project(stack LANGUAGES C VERSION 0.0.1)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

add_executable(
    ${PROJECT_NAME}
    main.c
)

find_package(OpenGL REQUIRED)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} m Xrandr X11 dl pthread)
endif (UNIX)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} m)
endif (WIN32)

include_directories(${CMAKE_SOURCE_DIR})

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

include(CheckCCompilerFlag)
function(enable_c_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_C_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_c_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()

enable_c_compiler_flag_if_supported("-Wall")
enable_c_compiler_flag_if_supported("-Wextra")
enable_c_compiler_flag_if_supported("-Werror")
# enable_c_compiler_flag_if_supported("-pedantic")
enable_c_compiler_flag_if_supported("-ansi")
# enable_c_compiler_flag_if_supported("-nostdlib")
enable_c_compiler_flag_if_supported("-Wstrict-prototypes")
enable_c_compiler_flag_if_supported("-fstrict-aliasing")
enable_c_compiler_flag_if_supported("-Wstrict-aliasing=3")
# Temporary to remove unused lua functions
# enable_c_compiler_flag_if_supported("-Os")
# enable_c_compiler_flag_if_supported("-O0")
# enable_c_compiler_flag_if_supported("-g")

enable_c_compiler_flag_if_supported("-Wno-unused-function")
enable_c_compiler_flag_if_supported("-Wno-unused-parameter")
enable_c_compiler_flag_if_supported("-Wno-unused-variable")
