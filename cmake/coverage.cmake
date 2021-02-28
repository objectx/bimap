#
# Copyright (c) 2021 Polyphony Digital Inc. All rights reserved.
#

include_guard(GLOBAL)

set (s_ ${PROJECT_NAME}-coverage_)

add_library (${s_} INTERFACE)
add_library (${PROJECT_NAME}::coverage ALIAS ${s_})

if (CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)")
    target_compile_options (${s_} INTERFACE -fprofile-instr-generate -fcoverage-mapping)
    target_link_options (${s_} INTERFACE -fprofile-instr-generate -fcoverage-mapping)
endif ()
