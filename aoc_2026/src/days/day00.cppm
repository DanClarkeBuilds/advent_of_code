module;

#include <algorithm>
#include <cstdint>
#include <expected>
#include <functional>
#include <numeric>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>

#ifdef AOC_TESTING
#include <initializer_list>

#include <catch2/catch_test_macros.hpp>
#endif

export module aoc.day00;

#ifdef AOC_TESTING
import aoc.test;
#endif

namespace aoc::day00 {
    using Input = std::vector<int>;

    namespace {
        using namespace lexy::dsl;

        // Warm-up grammar: one or more nonnegative integers separated by commas.
        struct Numbers {
            static constexpr auto rule = list(integer<int>, sep(comma)) + eof;
            static constexpr auto value = lexy::as_list<Input>;
        };
    } // namespace

    export struct Solution {
        static constexpr auto input_path = "inputs/day00.txt";
        static constexpr auto example_path = "examples/day00.txt";

        [[nodiscard]] static std::expected<Input, std::string>
        parse(const std::span<const std::string> lines) {
            if (lines.size() != 1) {
                return std::unexpected("Expected exactly one line of comma-separated integers");
            }
            const auto &text = lines.front();

            const lexy::string_input input(text.data(), text.size());
            auto result = lexy::parse<Numbers>(input, lexy::noop);
            if (!result) {
                return std::unexpected(
                    "Expected comma-separated nonnegative integers in the int range");
            }

            return std::move(result).value();
        }

        [[nodiscard]] static std::int64_t part1(const Input &input) {
            return std::ranges::fold_left(input, 0, std::plus<std::int64_t>{});
        }

        [[nodiscard]] static std::int64_t part2(const Input &input) {
            return std::ranges::fold_left(input, 1, std::multiplies<std::int64_t>{});
        }
    };

#ifdef AOC_TESTING
    namespace {
        const auto validate = aoc::test::Validator(Solution::parse);
    } // namespace

    TEST_CASE("day00 part1 example", "[day00]") {
        validate(Solution::part1).with(Solution::example_path).equals(60);
    }

    TEST_CASE("day00 part1", "[day00][input]") {
        validate(Solution::part1).with_optional(Solution::input_path).equals(60);
    }

    TEST_CASE("day00 part2 example", "[day00]") {
        validate(Solution::part2).with(Solution::example_path).equals(6'000LL);
    }

    TEST_CASE("day00 part2", "[day00][input]") {
        validate(Solution::part2).with_optional(Solution::input_path).equals(6'000LL);
    }

    TEST_CASE("Day 0 parses comma-separated integers", "[day00]") {
        auto input = Solution::parse(std::vector<std::string>{"10,20,30"});
        REQUIRE(input.has_value());
        CHECK(*input == std::vector<int>{10, 20, 30});
    }

    TEST_CASE("Day 0 uses 64-bit arithmetic", "[day00]") {
        CHECK(Solution::part1(Input{2'000'000'000, 2'000'000'000}) == 4'000'000'000LL);
        CHECK(Solution::part2(Input{50'000, 50'000}) == 2'500'000'000LL);
    }

    TEST_CASE("Day 0 requires exactly one input line", "[day00]") {
        CHECK_FALSE(Solution::parse(std::vector<std::string>{}).has_value());
        CHECK_FALSE(Solution::parse(std::vector<std::string>{"1,2", "3,4"}).has_value());
    }

    TEST_CASE("Day 0 rejects malformed input", "[day00]") {
        for (const auto *text : {"", " ", "1,", "1,,2", "1 2", "-1", "1,x", "1,2junk",
                                 "9999999999999999999999999", " 0, 2, 3 "}) {
            CAPTURE(text);
            auto input = Solution::parse(std::vector<std::string>{text});
            REQUIRE_FALSE(input.has_value());
            CHECK_FALSE(input.error().empty());
        }
    }
#endif
} // namespace aoc::day00
