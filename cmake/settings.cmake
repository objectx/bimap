#
# Copyright (c) 2021 Polyphony Digital Inc. All rights reserved.
#

# Defines ${PROJECT_NAME}::settings interface library.

include_guard (GLOBAL)

set (s_ ${PROJECT_NAME}-settings)
add_library (${s_} INTERFACE)
add_library (${PROJECT_NAME}::settings ALIAS ${s_})
target_compile_features (${s_} INTERFACE cxx_std_17)

if (CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)")
    target_compile_options (${s_} INTERFACE -Wall -Wextra -Wpedantic -Werror -march=native)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "^MSVC")
    target_compile_options (${s_} INTERFACE -WX -W4)
endif ()

if (TARGET ${PROJECT_NAME}::sanitizers)
    target_link_libraries (${s_} INTERFACE ${PROJECT_NAME}::sanitizers)
endif ()
if (TARGET ${PROJECT_NAME}::coverage)
    target_link_libraries (${s_} INTERFACE ${PROJECT_NAME}::coverage)
endif ()
