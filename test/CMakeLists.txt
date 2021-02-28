
cmake_minimum_required (VERSION 3.19)

set (app_ test-${PROJECT_NAME})

find_package (doctest)
find_package (rapidcheck)

add_executable (${app_} main.cpp bimap.cpp)
target_compile_definitions (${app_} PRIVATE DOCTEST_CONFIG_SUPER_FAST_ASSERTS=1)
target_link_libraries (${app_} PRIVATE bimap::settings bimap doctest::doctest rapidcheck::rapidcheck)
