function(aoc_discover_days MODULES_VARIABLE REGISTRY_VARIABLE)
    file(GLOB DAY_MODULES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/days/day*.cppm"
    )

    set(DAY_IMPORTS "")
    set(DAY_CHECKS "")
    set(DAY_ENTRIES "")
    foreach(DAY_MODULE IN LISTS DAY_MODULES)
        get_filename_component(DAY_NAME "${DAY_MODULE}" NAME_WE)
        if(NOT DAY_NAME MATCHES "^day([0-9][0-9])(_([0-9][0-9][0-9][0-9]))?$")
            message(FATAL_ERROR "Day modules must be named dayXX.cppm or dayXX_YYYY.cppm: ${DAY_MODULE}")
        endif()
        set(DAY_YEAR "${CMAKE_MATCH_3}")
        set(DAY_DIGITS "${CMAKE_MATCH_1}")
        if(DAY_YEAR STREQUAL "")
            set(DAY_YEAR 2026)
        endif()
        # Prefix with 1 to avoid interpreting leading zeroes as octal.
        math(EXPR DAY_YEAR "1${DAY_YEAR} - 10000")
        math(EXPR DAY_NUMBER "1${DAY_DIGITS} - 100")
        string(APPEND DAY_IMPORTS "import aoc.${DAY_NAME};\n")
        string(APPEND DAY_CHECKS
            "static_assert(aoc::DayInterface<aoc::${DAY_NAME}::Solution>,\n"
            "    \"${DAY_NAME}::Solution must provide string-like input_path, parse(span<const string>) returning expected<T, string>, and printable part1/part2(const T&)\");\n\n"
        )
        string(APPEND DAY_ENTRIES
            "            Day{${DAY_NUMBER}, ${DAY_YEAR}, [] { return run_day<aoc::${DAY_NAME}::Solution>(${DAY_NUMBER}, ${DAY_YEAR}); }},\n"
        )
    endforeach()
    list(LENGTH DAY_MODULES DAY_COUNT)

    set(REGISTRY_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/day_registry.cppm")
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/day_registry.cppm.in"
        "${REGISTRY_FILE}" @ONLY
    )

    set(${MODULES_VARIABLE} "${DAY_MODULES}" PARENT_SCOPE)
    set(${REGISTRY_VARIABLE} "${REGISTRY_FILE}" PARENT_SCOPE)
endfunction()
