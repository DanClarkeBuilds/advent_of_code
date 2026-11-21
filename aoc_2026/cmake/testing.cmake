# Compile modules directly into the test executable so every test registration
# is linked, with separate module artifacts from the normal runner build.
add_executable(aoc_tests)
target_sources(aoc_tests PRIVATE FILE_SET CXX_MODULES FILES
    ${AOC_COMMON_MODULES}
    src/aoc/test.cppm
    ${DAY_MODULES}
)
target_compile_definitions(aoc_tests PRIVATE AOC_TESTING)
target_link_libraries(aoc_tests PRIVATE
    foonathan::lexy::core aoc_options Catch2::Catch2WithMain
)

list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)
catch_discover_tests(aoc_tests)
