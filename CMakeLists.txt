
cmake_minimum_required (VERSION 3.19)

project (bimap
         VERSION 0.0.1
         LANGUAGES CXX)

list (INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (BIMAP_ENABLE_SANITIZERS)
    include (sanitizers)
endif ()
if (BIMAP_ENABLE_COVERAGE)
    include (coverage)
endif ()
include (settings)

function (setvar_default_ var_)
    if (NOT DEFINED ${var_})
        set (${var_} ${ARGN} PARENT_SCOPE)
    endif ()
endfunction ()

setvar_default_ (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
setvar_default_ (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
setvar_default_ (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include (${CMAKE_BINARY_DIR}/conan_paths.cmake)

set (lib_ bimap)
add_library (${lib_} INTERFACE)
target_compile_features (${lib_} INTERFACE cxx_std_17)
target_include_directories (${lib_} INTERFACE inc)
target_link_libraries (${lib_} INTERFACE bimap::settings)
target_sources (${lib_} PRIVATE inc/bimap.hpp)

add_subdirectory (test)
