#
# Copyright (c) 2021 Polyphony Digital Inc. All rights reserved.
#

# Defines ${PROJECT_NAME}::sanitizers interface library.
include_guard (GLOBAL)

set (s_ ${PROJECT_NAME}-sanitizers_)
add_library (${s_} INTERFACE)
add_library (${PROJECT_NAME}::sanitizers ALIAS ${s_})
if (CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)")
    target_compile_options (${s_} INTERFACE -fsanitize=address,undefined)
    target_link_options (${s_} INTERFACE -fsanitize=address,undefined)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "^MSVC")
    # There is no simple ways to remove the /RTC1 flag from the default compile option...
    target_compile_options (${s_} INTERFACE $<$<CONFIG:Release>:-fsanitize=address>)
    target_link_options (${s_} INTERFACE $<$<CONFIG:Release>:-fsanitize=address>)
endif ()

