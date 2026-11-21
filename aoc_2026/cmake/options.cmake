option(AOC_ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)

add_library(aoc_options INTERFACE)
target_compile_definitions(aoc_options INTERFACE AOC_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_options(aoc_options INTERFACE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
if(AOC_ENABLE_SANITIZERS)
    target_compile_options(aoc_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(aoc_options INTERFACE -fsanitize=address,undefined)
endif()
