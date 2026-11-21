cmake_minimum_required(VERSION 3.28)
project(aoc_2026 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
include(cmake/options.cmake)
include(cmake/dependencies.cmake)
include(cmake/days.cmake)

aoc_discover_days(DAY_MODULES DAY_REGISTRY)

# Add helpers needed by both solutions and tests here once.
set(AOC_COMMON_MODULES
    src/aoc/io.cppm
)

add_library(aoc_days STATIC)
target_sources(aoc_days PUBLIC FILE_SET CXX_MODULES FILES
    ${AOC_COMMON_MODULES}
    src/aoc/wreath.cppm
    src/aoc/runner.cppm
    ${DAY_MODULES}
)
target_link_libraries(aoc_days PRIVATE foonathan::lexy::core PUBLIC aoc_options)

add_executable(aoc src/main.cpp)
target_sources(aoc PRIVATE FILE_SET CXX_MODULES FILES
    "${DAY_REGISTRY}"
)
target_link_libraries(aoc PRIVATE aoc_days)

if(BUILD_TESTING)
    include(cmake/testing.cmake)
endif()
