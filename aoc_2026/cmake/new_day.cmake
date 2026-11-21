cmake_minimum_required(VERSION 3.28)

if(NOT DEFINED DAY OR NOT DAY MATCHES "^[0-9][0-9]?$")
    message(FATAL_ERROR "Set DAY to a number from 1 to 25, for example: make new DAY=1")
endif()

# Strip leading zeroes before numeric comparisons (08 and 09 are not octal).
string(REGEX REPLACE "^0" "" DAY_NUMBER "${DAY}")
if(DAY_NUMBER STREQUAL "" OR DAY_NUMBER LESS 1 OR DAY_NUMBER GREATER 25)
    message(FATAL_ERROR "Set DAY to a number from 1 to 25, for example: make new DAY=1")
endif()

if(DAY_NUMBER LESS 10)
    set(DAY_NAME "day0${DAY_NUMBER}")
else()
    set(DAY_NAME "day${DAY_NUMBER}")
endif()

get_filename_component(AOC_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(MODULE_PATH "${AOC_ROOT}/src/days/${DAY_NAME}.cppm")
set(EXAMPLE_PATH "${AOC_ROOT}/examples/${DAY_NAME}.txt")
set(INPUT_PATH "${AOC_ROOT}/inputs/${DAY_NAME}.txt")

# Check every output before writing anything, including dangling symlinks.
foreach(OUTPUT_PATH IN ITEMS "${MODULE_PATH}" "${EXAMPLE_PATH}" "${INPUT_PATH}")
    if(EXISTS "${OUTPUT_PATH}" OR IS_SYMLINK "${OUTPUT_PATH}")
        message(FATAL_ERROR "Refusing to overwrite ${OUTPUT_PATH}")
    endif()
endforeach()

file(MAKE_DIRECTORY "${AOC_ROOT}/src/days" "${AOC_ROOT}/examples" "${AOC_ROOT}/inputs")
configure_file("${CMAKE_CURRENT_LIST_DIR}/day.cppm.in" "${MODULE_PATH}" @ONLY)
file(WRITE "${EXAMPLE_PATH}" "")
file(WRITE "${INPUT_PATH}" "")

message(STATUS "Created src/days/${DAY_NAME}.cppm, examples/${DAY_NAME}.txt and inputs/${DAY_NAME}.txt")
message(STATUS "Fill in the parser and solutions, then remove each test's SKIP when its answer is known.")
