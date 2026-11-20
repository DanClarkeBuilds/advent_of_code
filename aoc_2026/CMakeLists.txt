cmake_minimum_required(VERSION 3.28)
project(aoc_2026 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(AOC_ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)

include(FetchContent)
set(LEXY_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(lexy
    GIT_REPOSITORY https://github.com/foonathan/lexy.git
    GIT_TAG 982f36fe0f663153463657a166e4f173099dea6f # v2025.05.0
    SYSTEM
)
FetchContent_MakeAvailable(lexy)

add_library(aoc_options INTERFACE)
target_compile_options(aoc_options INTERFACE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
if(AOC_ENABLE_SANITIZERS)
    target_compile_options(aoc_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(aoc_options INTERFACE -fsanitize=address,undefined)
endif()

file(GLOB DAY_MODULES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/days/day*.cppm"
)

add_library(aoc_days STATIC)
target_sources(aoc_days PUBLIC FILE_SET CXX_MODULES FILES
    src/aoc/io.cppm
    src/aoc/runner.cppm
    ${DAY_MODULES}
)
target_link_libraries(aoc_days PRIVATE foonathan::lexy::core PUBLIC aoc_options)

set(DAY_IMPORTS "")
set(DAY_ENTRIES "")
foreach(DAY_MODULE IN LISTS DAY_MODULES)
    get_filename_component(DAY_NAME "${DAY_MODULE}" NAME_WE)
    if(NOT DAY_NAME MATCHES "^day([0-9][0-9])$")
        message(FATAL_ERROR "Day modules must be named dayXX.cppm: ${DAY_MODULE}")
    endif()
    # Prefix with 1 to avoid interpreting leading zeroes as octal.
    math(EXPR DAY_NUMBER "1${CMAKE_MATCH_1} - 100")
    string(APPEND DAY_IMPORTS "import aoc.${DAY_NAME};\n")
    string(APPEND DAY_ENTRIES
        "            Day{${DAY_NUMBER}, [] { return run_day(${DAY_NUMBER}, aoc::${DAY_NAME}::input_path, aoc::${DAY_NAME}::parse, aoc::${DAY_NAME}::part1, aoc::${DAY_NAME}::part2); }},\n"
    )
endforeach()
list(LENGTH DAY_MODULES DAY_COUNT)
configure_file(cmake/day_registry.cppm.in generated/day_registry.cppm @ONLY)

add_executable(aoc src/main.cpp)
target_sources(aoc PRIVATE FILE_SET CXX_MODULES FILES
    "${CMAKE_CURRENT_BINARY_DIR}/generated/day_registry.cppm"
)
target_link_libraries(aoc PRIVATE aoc_days)

include(CTest)
if(BUILD_TESTING)
    FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG 5010924c4d8e912e64fd64a11819eecacf572c06 # v3.16.1
        SYSTEM
    )
    FetchContent_MakeAvailable(Catch2)
    # Compile modules directly into the test executable so every test registration
    # is linked, with separate module artifacts from the normal runner build.
    add_executable(aoc_tests)
    target_sources(aoc_tests PRIVATE FILE_SET CXX_MODULES FILES
        src/aoc/io.cppm
        src/aoc/test.cppm
        ${DAY_MODULES}
    )
    target_compile_definitions(aoc_tests PRIVATE
        AOC_TESTING
        AOC_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
    target_link_libraries(aoc_tests PRIVATE
        foonathan::lexy::core aoc_options Catch2::Catch2WithMain
    )
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
    include(Catch)
    catch_discover_tests(aoc_tests)
endif()
