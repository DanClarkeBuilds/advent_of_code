module;

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>

#ifdef AOC_TESTING
#include <catch2/catch_test_macros.hpp>
#endif

export module aoc.day01;

#ifdef AOC_TESTING
import aoc.test;
#endif

namespace aoc::day01 {
    using Input = std::string;

    export struct Solution {
        static constexpr auto input_path = "inputs/day01.txt";
        static constexpr auto example_path = "examples/day01.txt";

        static std::expected<Input, std::string> parse(std::string_view text) {
            // TODO: parse the puzzle input with lexy.
            return std::string(text);
        }

        static std::int64_t part1(const Input &) {
            // TODO: solve part 1.
            return 0;
        }

        static std::int64_t part2(const Input &) {
            // TODO: solve part 2.
            return 0;
        }
    };

#ifdef AOC_TESTING
    namespace {
        const auto validate = aoc::test::Validator(Solution::parse);
    }

    TEST_CASE("day01 part1 example", "[day01]") {
        SKIP("TODO: add the example and expected answer, then remove this SKIP.");
        validate(Solution::part1).with(Solution::example_path).equals(0);
    }

    TEST_CASE("day01 part1", "[day01][input]") {
        SKIP("TODO: set the expected answer, then remove this SKIP.");
        validate(Solution::part1).with_optional(Solution::input_path).equals(0);
    }

    TEST_CASE("day01 part2 example", "[day01]") {
        SKIP("TODO: add the example and expected answer, then remove this SKIP.");
        validate(Solution::part2).with(Solution::example_path).equals(0);
    }

    TEST_CASE("day01 part2", "[day01][input]") {
        SKIP("TODO: set the expected answer, then remove this SKIP.");
        validate(Solution::part2).with_optional(Solution::input_path).equals(0);
    }
#endif
} // namespace aoc::day01
