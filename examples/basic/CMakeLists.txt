cmake_minimum_required(VERSION 3.5.0)

project(basic LANGUAGES C VERSION 0.0.1)

include_directories(../../)

add_executable(
    ${PROJECT_NAME}
    main.c
)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 90) # C89 standard

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
enable_c_compiler_flag_if_supported("-pedantic")
enable_c_compiler_flag_if_supported("-ansi")
# enable_c_compiler_flag_if_supported("-nostdlib")
enable_c_compiler_flag_if_supported("-Wstrict-prototypes")
enable_c_compiler_flag_if_supported("-Wold-style-definition")
enable_c_compiler_flag_if_supported("-fstrict-aliasing")
enable_c_compiler_flag_if_supported("-Wstrict-aliasing=3")
enable_c_compiler_flag_if_supported("-fno-eliminate-unused-debug-types")
# Temporary to remove unused lua functions
enable_c_compiler_flag_if_supported("-Wno-unused-function")
# enable_c_compiler_flag_if_supported("-Os")
# enable_c_compiler_flag_if_supported("-O0")
# enable_c_compiler_flag_if_supported("-g")
